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
#include <string>
#include <fstream>
#include <cassert>

#include <ILM/ImathColor.h>
#include <ILM/ImathVec.h>

#include <iostream>
#include <iterator>
#include <vector>

#define SAMPLING_DENSITY 0.02

float dist(int x1, int y1, int x2, int y2)
{
    float D = (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2);
    D = sqrt(D);
    return D;
}

const int N = 1025;
int F[N][N];
int numx[N*N], denx[N*N], numy[N*N], deny[N*N];

int main()
{
    std::ios::sync_with_stdio(0);

    freopen("randomPoints.out", "r", stdin);
    freopen("Delaunay.out", "w", stdout);

    int SizeW, SizeH;
    int NumSites = 0;
    int segs;

    std::cin >> SizeW >> SizeH;

    for(int y = 0; y < SizeH; y++)
        for(int x = 0; x < SizeW; x++)
            std::cin >> F[x][y];

    std::cin >> NumSites;

    std::vector< Imath::V2i > SiteVec ;
    

    int X, Y;

    for(int i = 0; i<NumSites; i++)
    {
        std::cin >> X >> Y;
        Imath::V2i Cell(X , Y);
        SiteVec.push_back( Cell ) ;
    }

    freopen("selectedPoints.out", "r", stdin);

    int not_move;
    std::cin >> not_move;

    for(int i = 0; i<not_move; i++)
    {
        std::cin >> Y >> X;
        Imath::V2i Cell(X , Y);
        SiteVec.push_back( Cell ) ;
    }

    

    //
    static const Imath::V2i OffsetArray[8] = { Imath::V2i( - 1 , - 1 ) ,
                                               Imath::V2i(   0 , - 1 ) ,
                                               Imath::V2i(   1 , - 1 ) ,
                                               Imath::V2i( - 1 ,   0 ) ,
                                               Imath::V2i(   1 ,   0 ) ,
                                               Imath::V2i( - 1 ,   1 ) ,
                                               Imath::V2i(   0 ,   1 ) ,
                                               Imath::V2i(   1 ,   1 ) } ;

    int * Ping;
    int * Pong;

    std::vector< int > SeedVecA;
    std::vector< int > SeedVecB;


    for(int iter = 0; iter < 10; iter++)
    {

        std::vector< int >( SizeW * SizeH , -1 ).swap(SeedVecA);
        std::vector< int >( SizeW * SizeH , -1 ).swap(SeedVecB);

        for(int i = 0; i<(NumSites + not_move); i++)
        {
            SeedVecA[SiteVec[i].x + SiteVec[i].y * SizeW] = i;
            SeedVecB[SiteVec[i].x + SiteVec[i].y * SizeW] = i;
        }

        Ping = & SeedVecA[0] ;
        Pong = & SeedVecB[0] ;

        for ( int k = std::max(SizeW, SizeH) / 2 ; k > 0 ; k = k >> 1 )
        {
            for ( int y = 0 ; y < SizeH ; ++ y )
            {
                for ( int x = 0 ; x < SizeW ; ++ x )
                {
                    const int CellIdx = x + y * SizeW ;
                    const int Seed = Ping[CellIdx] ;
                    if ( Seed > - 1 )
                    {
                        Imath::V2i Cell( x , y ) ;
                        for ( int i = 0 ; i < 8 ; ++ i )
                        {
                            const Imath::V2i & FillCell = Cell + k * OffsetArray[i] ;
                            if ( FillCell.x >= 0 && FillCell.x < SizeW && FillCell.y >= 0 && FillCell.y < SizeH )
                            {
                                const int FillCellIdx = FillCell.x + FillCell.y * SizeW ;
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

        for(int i = 0; i<(SizeH*SizeW); i++)
        {
            numx[i] = 0;
            denx[i] = 0;
            numy[i] = 0;
            deny[i] = 0;
        }

        for(int y = 0; y<SizeH; y++)
        {
            for(int x = 0; x<SizeW; x++)
            {
                const int Seed = Pong[x + y*SizeW];
                if(Seed != -1)
                {
                    numx[Seed] += F[x][y]*x;
                    denx[Seed] += F[x][y];

                    numy[Seed] += F[x][y]*y;
                    deny[Seed] += F[x][y];
                }
            }
        }
        for(int i = 0; i<NumSites; i++)
        {
            if(denx[i] != 0 && deny[i] != 0)
            {
                Imath::V2i Cell(numx[i]/denx[i] , numy[i]/deny[i]);
                SiteVec[i] = Cell;
            }
            // std::cerr << numx[i] << " " << denx[i] << " " << numy[i] << " " << deny[i] << "\n";
        }
    }


    FILE * Output = fopen( "Vertex_Opti.png" , "wb" ) ;

    fprintf( Output , "P6\n%d %d\n255\n" , SizeW , SizeH ) ;

    std::vector< Imath::C3c > Pixels_Vertex_Opti( SizeW * SizeH , Imath::C3c( 0 ) ) ;

    // Color those vertices
    for(int i = 0; i<NumSites; i++)
        Pixels_Vertex_Opti[SiteVec[i].x + SiteVec[i].y *SizeW] = Imath::C3c( 255,0 ,0 );
    std::cout << NumSites << "\n";
    for(int i = 0; i<NumSites; i++)
        std::cout << SiteVec[i].x << " " << SiteVec[i].y << "\n";
    for(int i = NumSites; i<(NumSites + not_move); i++)
        Pixels_Vertex_Opti[SiteVec[i].x + SiteVec[i].y *SizeW] = Imath::C3c( 0,255 ,0 );

    fwrite( & Pixels_Vertex_Opti[0].x , 3 , Pixels_Vertex_Opti.size() , Output );

    fclose( Output ) ;

    return EXIT_SUCCESS ;
}
