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
#include <cstdlib>
#include <ctime>

#include <ILM/ImathColor.h>
#include <ILM/ImathVec.h>

#include <iostream>
#include <iterator>
#include <vector>

int main( int Argc , char ** Argv )
{
    -- Argc , ++ Argv ;
    if ( Argc != 3 )
    {
        // std::cout << "Here\n";
        return EXIT_FAILURE ;
    }

    //
    int NumSites = atoi( Argv[0] ) ;
    int Size     = atoi( Argv[1] ) ;

    // std::cout << NumSites << " " << Size << "\n";

    // 1) Generate the 2D sites and the fill color.
    //
    std::vector< Imath::V2f > SiteVec ;
    std::vector< int > SeedVecA( Size * Size , - 1 ) ;
    std::vector< Imath::C3c > RandomColorVec ;
    if ( NumSites > 1 )
    {
        srand( time(NULL) ) ;

        for ( int i = 0 ; i < NumSites ; ++ i )
        {
            float X = static_cast< float >( rand() ) / RAND_MAX * Size ;
            float Y = static_cast< float >( rand() ) / RAND_MAX * Size ;

            Imath::V2i Cell( static_cast< int >( floorf( X ) ) ,
                             static_cast< int >( floorf( Y ) ) ) ;
            SiteVec.push_back( Imath::V2f( Cell.x + 0.5f , Cell.y + 0.5f ) ) ;

            SeedVecA[Cell.x + Cell.y * Size] = i ;

            Imath::C3c C( static_cast< unsigned char >( static_cast< float >( rand() ) / RAND_MAX * 255.0f ) ,
                          static_cast< unsigned char >( static_cast< float >( rand() ) / RAND_MAX * 255.0f ) ,
                          static_cast< unsigned char >( static_cast< float >( rand() ) / RAND_MAX * 255.0f ) ) ;
            RandomColorVec.push_back( C ) ;
        }
    }
    else
    {
        NumSites = 1 ;

        SiteVec.push_back( Imath::V2f( 0.5f , 0.5f ) ) ;
        SeedVecA[0] = 0 ;
        RandomColorVec.push_back( Imath::C3c( 0 , 255 , 0 ) ) ;
    }
    std::vector< int > SeedVecB( SeedVecA ) ;

    //
    const int SizeLowTwo = static_cast< int >( ceilf( logf( static_cast< float >( Size ) ) ) ) ;

    //
    static const Imath::V2i OffsetArray[8] = { Imath::V2i( - 1 , - 1 ) ,
                                               Imath::V2i(   0 , - 1 ) ,
                                               Imath::V2i(   1 , - 1 ) ,
                                               Imath::V2i( - 1 ,   0 ) ,
                                               Imath::V2i(   1 ,   0 ) ,
                                               Imath::V2i( - 1 ,   1 ) ,
                                               Imath::V2i(   0 ,   1 ) ,
                                               Imath::V2i(   1 ,   1 ) } ;

    int * Ping = & SeedVecA[0] ;
    int * Pong = & SeedVecB[0] ;

    for ( int k = Size / 2 ; k > 0 ; k = k >> 1 )
    {
        fprintf( stdout , "k = %d\n" , k ) ;

        for ( int y = 0 ; y < Size ; ++ y )
        {
            for ( int x = 0 ; x < Size ; ++ x )
            {
                const int CellIdx = x + y * Size ;
                const int Seed = Ping[CellIdx] ;
                if ( Seed > - 1 )
                {
                    Imath::V2i Cell( x , y ) ;
                    for ( int i = 0 ; i < 8 ; ++ i )
                    {
                        const Imath::V2i & FillCell = Cell + k * OffsetArray[i] ;
                        if ( FillCell.x >= 0 && FillCell.x < Size && FillCell.y >= 0 && FillCell.y < Size )
                        {
                            const int FillCellIdx = FillCell.x + FillCell.y * Size ;
                            const int FillSeed = Pong[FillCellIdx] ;
                            if ( FillSeed < 0 )
                            {
                                Pong[FillCellIdx] = Seed ;
                            }
                            else
                            {
                                const Imath::V2f & FillP = Imath::V2f( FillCell.x + 0.5f , FillCell.y + 0.5f ) ;
                                if ( ( FillP - SiteVec[Seed] ).length() < ( FillP - SiteVec[FillSeed] ).length() )
                                {
                                    Pong[FillCellIdx] = Seed ;
                                }
                            }
                        }
                    }
                }
            }
        }

        std::copy( Pong , Pong + SeedVecA.size() , Ping ) ;
        std::swap( Ping , Pong ) ;
    }

    //
    FILE * Output = fopen( Argv[2] , "wb" ) ;
    fprintf( Output , "P6\n%d %d\n255\n" , Size , Size ) ;

    std::vector< Imath::C3c > Pixels( Size * Size , Imath::C3c( 0 ) ) ;
    for ( int y = 0 ; y < Size ; ++ y )
    {
        for ( int x = 0 ; x < Size ; ++ x )
        {
            const int Seed = Pong[x + y * Size] ;
            if ( Seed != - 1 )
            {
                Pixels[x + y * Size] = RandomColorVec[Seed] ;
            }
        }
    }

    for( std::vector< Imath::V2f >::const_iterator itr = SiteVec.begin() ; itr != SiteVec.end() ; ++ itr )
    {
        const int x = static_cast< int >( floorf( itr->x ) ) ;
        const int y = static_cast< int >( floorf( itr->y ) ) ;
        Pixels[x + y * Size] = Imath::C3c( 255 , 0 , 0 ) ;
    }

    fwrite( & Pixels[0].x , 3 , Pixels.size() , Output ) ;
    fclose( Output ) ;

    return EXIT_SUCCESS ;
}
