/**
 * Copyright (c) 2014, Bo Zhou<Bo.Schwarzstein@gmail.com> and J CUBE Inc. Tokyo, Japan
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the <organization>.
 * 4. Neither the name of the <organization> nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDER AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <cuda_runtime.h>
#include <cuda_runtime_api.h>

#include <iostream>
#include <iterator>
#include <vector>

__global__ void Kernel( int SizeX , int SizeY , const float2 * SiteArray , const int * Ping , int * Pong , int k , int * Mutex )
{
    //
    const int CellX = threadIdx.x + blockIdx.x * blockDim.x ;
    const int CellY = threadIdx.y + blockIdx.y * blockDim.y ;

    const int CellIdx = CellX + CellY * SizeX ;
    const int Seed = Ping[CellIdx] ;
    if ( Seed < 0 )
    {
        return ;
    }

    //
    const int2 OffsetArray[8] = { { - 1 , - 1 } ,
                                  {   0 , - 1 } ,
                                  {   1 , - 1 } ,
                                  { - 1 ,   0 } ,
                                  {   1 ,   0 } ,
                                  { - 1 ,   1 } ,
                                  {   0 ,   1 } ,
                                  {   1 ,   1 } } ;

    for ( int i = 0 ; i < 8 ; ++ i )
    {
        const int FillCellX = CellX + k * OffsetArray[i].x ;
        const int FillCellY = CellY + k * OffsetArray[i].y ; 
        if ( FillCellX >= 0 && FillCellX < SizeX && FillCellY >= 0 && FillCellY < SizeY )
        {
            //
            const int FillCellIdx = FillCellX + FillCellY * SizeX ;

            // Lock
            //
            while ( atomicCAS( Mutex , - 1 , FillCellIdx ) == FillCellIdx )
            {
            }

            const int FillSeed = Pong[FillCellIdx] ;

            if ( FillSeed < 0 )
            {
                Pong[FillCellIdx] = Seed ;
            }
            else
            {
                float2 P = make_float2( FillCellX + 0.5f , FillCellY + 0.5f ) ;

                float2 A = SiteArray[Seed] ;
                float2 PA = make_float2( A.x - P.x , A.y - P.y ) ;
                float PALength = PA.x * PA.x + PA.y * PA.y ;

                const float2 B = SiteArray[FillSeed] ;
                float2 PB = make_float2( B.x - P.x , B.y - P.y ) ;
                float PBLength = PB.x * PB.x + PB.y * PB.y ;

                if ( PALength < PBLength )
                {
                    Pong[FillCellIdx] = Seed ;
                }
            }

            // Release
            //
            atomicExch( Mutex , - 1 ) ;
        }
    }
}

int main( int Argc , char * Argv[] )
{
    -- Argc , ++ Argv ;
    if ( Argc != 3 )
    {
        return EXIT_FAILURE ;
    }

    //
    int NumSites = atoi( Argv[0] ) ;
    int Size     = atoi( Argv[1] ) ;

    //
    int NumCudaDevice = 0 ;
    cudaGetDeviceCount( & NumCudaDevice ) ;
    if ( ! NumCudaDevice )
    {
        return EXIT_FAILURE ;
    }

    //
    //
    std::vector< float2 > SiteVec ;
    std::vector< int >    SeedVec( Size * Size , - 1 ) ;
    std::vector< uchar3 > RandomColorVec ;
    for ( int i = 0 ; i < NumSites ; ++ i )
    {
        float X = static_cast< float >( rand() ) / RAND_MAX * Size ;
        float Y = static_cast< float >( rand() ) / RAND_MAX * Size ;
        int CellX = static_cast< int >( floorf( X ) ) ;
        int CellY = static_cast< int >( floorf( Y ) ) ;

        SiteVec.push_back( make_float2( CellX + 0.5f , CellY + 0.5f ) ) ;
        SeedVec[CellX + CellY * Size] = i ;

        RandomColorVec.push_back( make_uchar3( static_cast< unsigned char >( static_cast< float >( rand() ) / RAND_MAX * 255.0f ) ,
                                               static_cast< unsigned char >( static_cast< float >( rand() ) / RAND_MAX * 255.0f ) ,
                                               static_cast< unsigned char >( static_cast< float >( rand() ) / RAND_MAX * 255.0f ) ) ) ;
    }

    //
    size_t SiteSize = NumSites * sizeof( float2 ) ;

    float2 * SiteArray = NULL ;
    cudaMalloc( & SiteArray , SiteSize ) ;
    cudaMemcpy( SiteArray , & SiteVec[0] , SiteSize , cudaMemcpyHostToDevice ) ;

    //
    size_t BufferSize = Size * Size * sizeof( int ) ;

    int * Ping = NULL , * Pong = NULL ;
    cudaMalloc( & Ping , BufferSize ) , cudaMemcpy( Ping , & SeedVec[0] , BufferSize , cudaMemcpyHostToDevice ) ;
    cudaMalloc( & Pong , BufferSize ) , cudaMemcpy( Pong , Ping , BufferSize , cudaMemcpyDeviceToDevice ) ;

    //
    int * Mutex = NULL ;
    cudaMalloc( & Mutex , sizeof( int ) ) , cudaMemset( Mutex , - 1 , sizeof( int ) ) ;

    //
    //
    cudaDeviceProp CudaDeviceProperty ;
    cudaGetDeviceProperties( & CudaDeviceProperty , 0 ) ;

    dim3 BlockDim( CudaDeviceProperty.warpSize , CudaDeviceProperty.warpSize ) ;
    dim3 GridDim( ( Size + BlockDim.x - 1 ) / BlockDim.x ,
                  ( Size + BlockDim.y - 1 ) / BlockDim.y ) ;

    for ( int k = Size / 2 ; k > 0 ; k = k >> 1 )
    {
        Kernel<<< GridDim , BlockDim >>>( Size , Size , SiteArray , Ping , Pong , k , Mutex ) ;
        cudaDeviceSynchronize() ;

        cudaMemcpy( Ping , Pong , BufferSize , cudaMemcpyDeviceToDevice ) ;
        std::swap( Ping , Pong ) ;
    }
    cudaMemcpy( & SeedVec[0] , Pong , BufferSize , cudaMemcpyDeviceToHost ) ;

    //
    cudaFree( SiteArray ) ;
    cudaFree( Ping ) ;
    cudaFree( Pong ) ;
    cudaFree( Mutex ) ;

    //
    //
    FILE * Output = fopen( Argv[2] , "wb" ) ;
    fprintf( Output , "P6\n%d %d\n255\n" , Size , Size ) ;

    std::vector< uchar3 > Pixels( Size * Size ) ;
    for ( int y = 0 ; y < Size ; ++ y )
    {
        for ( int x = 0 ; x < Size ; ++ x )
        {
            const int Seed = SeedVec[x + y * Size] ;
            if ( Seed != - 1 )
            {
                Pixels[x + y * Size] = RandomColorVec[Seed] ;
            }
        }
    }

    for( std::vector< float2 >::const_iterator itr = SiteVec.begin() ; itr != SiteVec.end() ; ++ itr )
    {
        const int x = static_cast< int >( floorf( itr->x ) ) ;
        const int y = static_cast< int >( floorf( itr->y ) ) ;
        Pixels[x + y * Size] = make_uchar3( 255 , 0 , 0 ) ;
    }

    fwrite( & Pixels[0].x , 3 , Pixels.size() , Output ) ;
    fclose( Output ) ;

    return EXIT_SUCCESS ;
}
