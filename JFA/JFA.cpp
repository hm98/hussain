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
bool salient[N][N];

int main()
{
    std::ios::sync_with_stdio(0);

    freopen("outPoints.out", "r", stdin);
    freopen("randomPoints.out", "w", stdout);

    srand( time(NULL) ) ;

    int SizeW, SizeH;
    int NumSites = 0;
    int segs;

    std::cin >> SizeW >> SizeH;
    std::cin >> segs;

    std::vector< Imath::V2f > SiteVec ;
    std::vector< int > SeedVecA( SizeW * SizeH , - 1 ) ;


    while(segs--)
    {
        int temp;
        std::cin >> temp;
        NumSites += temp;
        while(temp--)
        {
            float X, Y;
            std::cin >> Y >> X;

            assert(X < SizeW);
            assert(Y < SizeH);
            assert(X >= 0);
            assert(Y >= 0);

            Imath::V2i Cell( static_cast< int >( floorf( X ) ) ,
                             static_cast< int >( floorf( Y ) ) ) ;
            SiteVec.push_back( Imath::V2f( Cell.x + 0.5f , Cell.y + 0.5f ) ) ;

            SeedVecA[Cell.x + Cell.y * SizeW] = NumSites - temp + 1;
        }
    }

    std::vector< int > SeedVecB( SeedVecA ) ;

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

    for ( int k = std::max(SizeW, SizeH) / 2 ; k > 0 ; k = k >> 1 )
    {
        // fprintf( stdout , "k = %d\n" , k ) ; 

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


    FILE * Output = fopen( "Distance_Map.png" , "wb" ) ;
    FILE * Output1 = fopen( "Feature_Map.png" , "wb" ) ;
    FILE * Output2 = fopen( "Init_Random_Points.png" , "wb" ) ;

    fprintf( Output , "P6\n%d %d\n255\n" , SizeW , SizeH ) ;
    fprintf( Output1 , "P6\n%d %d\n255\n" , SizeW , SizeH ) ;
    fprintf( Output2 , "P6\n%d %d\n255\n" , SizeW , SizeH ) ;

    std::vector< Imath::C3c > Pixels_Distance_Map( SizeW * SizeH , Imath::C3c( 0 ) ) ;
    std::vector< Imath::C3c > Pixels_Feature_Map( SizeW * SizeH , Imath::C3c( 0 ) ) ;
    std::vector< Imath::C3c > Pixels_Random_Points( SizeW * SizeH , Imath::C3c( 0 ) ) ;

    float maxDist = 0.0;
    for ( int y = 0 ; y < SizeH ; ++ y )
    {
        for ( int x = 0 ; x < SizeW ; ++ x )
        {
            const int Seed = Pong[x + y * SizeW] ;
            if ( Seed != - 1 )
            {
                float D = dist(x, y, SiteVec[Seed].x, SiteVec[Seed].y);
                maxDist = std::max(D, maxDist);
            }
        }
    }

    maxDist = maxDist; // Can comment this out

    float Li = SAMPLING_DENSITY*(SizeW + SizeH);
    float m = Li/2;
    float minF = 1000.0, maxF = -1000.0;

    for ( int y = 0 ; y < SizeH ; ++ y )
    {
        for ( int x = 0 ; x < SizeW ; ++ x )
        {
            const int Seed = Pong[x + y * SizeW] ;
            if ( Seed != - 1 )
            {
                float D = dist(x, y, SiteVec[Seed].x, SiteVec[Seed].y);
                D = D/maxDist;
                D = std::min(D, float(1.0));
                D = D*255;
                Pixels_Distance_Map[x + y * SizeW] = Imath::C3c( D,
                                                    D ,
                                                    D );
                float F;
                int val = D/m;
                if(val%2 == 1)
                    F = (255.0/m)*(val%((int)m));
                else
                    F = (255.0/m)*(m - (val%((int)m)));

                minF = std::min(minF, F);
                maxF = std::max(maxF, F);
            }
        }
    }

    freopen("saliency.out", "r", stdin);

    int sx, sy;
    while(scanf("%d %d", &sx, &sy) != EOF)
    {
        salient[sx][sy] = true;
    }

    std::vector<int> fromSelect_foreground;
    std::vector<int> fromSelect_background;
    fromSelect_foreground.clear();
    fromSelect_background.clear();

    // std::cout << minF << " " << maxF << "\n";

    std::cout << SizeW << " " << SizeH << "\n";

    for ( int y = 0 ; y < SizeH ; ++ y )
    {
        for ( int x = 0 ; x < SizeW ; ++ x )
        {
            const int Seed = Pong[x + y * SizeW] ;
            if ( Seed != - 1 )
            {
                float D = dist(x, y, SiteVec[Seed].x, SiteVec[Seed].y);
                D = D/maxDist;
                D = std::min(D, float(1.0));
                D = D*255;

                float F;
                int val = D/m;
                if(val%2 == 1)
                    F = (255.0/m)*(val%((int)m));
                else
                    F = (255.0/m)*(m - (val%((int)m)));



                F -= minF;
                F = (F*255.0)/(maxF - minF);
                Pixels_Feature_Map[x + y *SizeW] = Imath::C3c( F,
                                                    F ,
                                                    F );

                std::cout << (int)F << " ";

                if(F < 50)
                {
                    if(salient[x][y])
                        fromSelect_foreground.push_back(x + y *SizeW);
                    else
                        fromSelect_background.push_back(x + y *SizeW);
                }
            }
            else
                assert(false);
        }
        std::cout << "\n";
    }

    assert(SizeW != 0);
    assert(fromSelect_foreground.size() != 0);
    assert(fromSelect_background.size() != 0);

    int toSelect_foreground = 300;
    int toSelect_background = 100;

    std::cout << toSelect_foreground + toSelect_background << "\n";
    while(toSelect_foreground--)
    {
        int pos = rand()%fromSelect_foreground.size();
        Pixels_Random_Points[fromSelect_foreground[pos]] = Imath::C3c(0, 255, 0);
        std::cout << fromSelect_foreground[pos]%SizeW << " " << fromSelect_foreground[pos]/SizeW << "\n";
    }
    while(toSelect_background--)
    {
        int pos = rand()%fromSelect_background.size();
        Pixels_Random_Points[fromSelect_background[pos]] = Imath::C3c(0, 0, 255);
        std::cout << fromSelect_background[pos]%SizeW << " " << fromSelect_background[pos]/SizeW << "\n";
    }

    freopen("selectedPoints.out", "r", stdin);

    int GPoints;
    std::cin >> GPoints;
    while(GPoints--)
    {
        float X, Y;
        std::cin >> Y >> X;
        Pixels_Random_Points[(int)X + (int)Y *SizeW] = Imath::C3c( 255,
                                                    0 ,
                                                    0 );
    }

    fwrite( & Pixels_Distance_Map[0].x , 3 , Pixels_Distance_Map.size() , Output );
    fwrite( & Pixels_Feature_Map[0].x , 3 , Pixels_Feature_Map.size() , Output1 );
    fwrite( & Pixels_Random_Points[0].x , 3 , Pixels_Random_Points.size() , Output2 );

    fclose( Output ) ;
    fclose( Output1 ) ;
    fclose( Output2 ) ;

    return EXIT_SUCCESS ;
}
