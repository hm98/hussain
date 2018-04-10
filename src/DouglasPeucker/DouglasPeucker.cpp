#include <cmath>
#include <utility>
#include <vector>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <fstream>
using namespace std;

#define SAMPLING_DENSITY 0.006

typedef std::pair<int, int> Point;

void SaveImagePGM(char *filename, char *buffer, int width, int height);
double PerpendicularDistance(const Point &pt, const Point &lineStart, const Point &lineEnd);
void RamerDouglasPeucker(const vector<Point> &pointList, double epsilon, vector<Point> &out);
void getPoints(int x1, int y1, int x2, int y2);

vector<Point> temp;

int main()
{
  freopen("../../outputs/data/edgeSegments.data", "r", stdin);
  freopen("../../outputs/data/douglasSegments.data", "w", stdout);

  int width, height;
  scanf("%d %d", &width, &height);
  printf("%d %d\n", width, height);
  unsigned char fin[width*height];
  memset(fin, 0, width*height);
  int x1, y1, x2, y2;

  double Li = SAMPLING_DENSITY*(width + height); // Sampling Interval

  // Now applying Douglas Pecker Algorithm

  vector<Point> pointList;
  vector<Point> pointListOut;
  vector<Point> selectedPoints;
  int selectCnt = 0;
  

  int nSeg;
  scanf("%d", &nSeg);
  printf("%d\n", nSeg);

  for(int xx = 1; xx<=nSeg; xx++)
  {
    pointList.clear();
    pointListOut.clear();

    int cnt;
    scanf("%d", &cnt);
    while(cnt--)
    {
      scanf("%d %d", &x1, &y1);
      pointList.push_back(Point(x1, y1));
    }

    RamerDouglasPeucker(pointList, Li, pointListOut); 

    selectCnt += pointListOut.size();


    printf("%lu\n", pointListOut.size());
    for(int i = 0; i<pointListOut.size(); i++)
    {
      x1 = pointListOut[i].first;
      y1 = pointListOut[i].second;
      selectedPoints.push_back(Point(x1, y1));
      printf("%d %d\n", x1, y1);
    }

    for(int i = 1; i<pointListOut.size(); i++)
    {
      x1 = pointListOut[i-1].first;
      y1 = pointListOut[i-1].second;
      x2 = pointListOut[i].first;
      y2 = pointListOut[i].second;

      temp.clear();

      getPoints(x1, y1, x2, y2);

      temp.push_back(Point(x1, y1));
      temp.push_back(Point(x2, y2));

      for(int j = 0; j<temp.size(); j++)
      {
        x1 = temp[j].first;
        y1 = temp[j].second;
        fin[x1*width + y1] = 255;
      }
    }
  }

  SaveImagePGM((char *)"../../outputs/images/Douglas.pgm", (char *)fin, width, height);


  freopen("../../outputs/data/douglasPoints.data", "w", stdout);

  printf("%d\n", selectCnt);
  for(int i = 0; i<selectedPoints.size(); i++)
    printf("%d %d\n", selectedPoints[i].first, selectedPoints[i].second);

  return 0; 	
}

///---------------------------------------------------------------------------------
/// Save a buffer as a .pgm image
///
void SaveImagePGM(char *filename, char *buffer, int width, int height){
  FILE *fp = fopen(filename, "wb");

  // .PGM header
  fprintf(fp, "P5\n");
  fprintf(fp, "# Some comment here!\n");
  fprintf(fp, "%d %d\n", width, height);
  fprintf(fp, "255\n");

  // Grayscale image
  fwrite(buffer, 1, width*height, fp);

  fclose( fp );
} //end-SaveImagePGM

double PerpendicularDistance(const Point &pt, const Point &lineStart, const Point &lineEnd)
{
  double dx = lineEnd.first - lineStart.first;
  double dy = lineEnd.second - lineStart.second;

  //Normalise
  double mag = pow(pow(dx,2.0)+pow(dy,2.0),0.5);
  if(mag > 0.0)
  {
    dx /= mag; dy /= mag;
  }

  double pvx = pt.first - lineStart.first;
  double pvy = pt.second - lineStart.second;

  //Get dot product (project pv onto normalized direction)
  double pvdot = dx * pvx + dy * pvy;

  //Scale line direction vector
  double dsx = pvdot * dx;
  double dsy = pvdot * dy;

  //Subtract this from pv
  double ax = pvx - dsx;
  double ay = pvy - dsy;

  return pow(pow(ax,2.0)+pow(ay,2.0),0.5);
}

void RamerDouglasPeucker(const vector<Point> &pointList, double epsilon, vector<Point> &out)
{
  if(pointList.size()<2)
    throw invalid_argument("Not enough points to simplify");

  // Find the point with the maximum distance from line between start and end
  double dmax = 0.0;
  size_t index = 0;
  size_t end = pointList.size()-1;
  for(size_t i = 1; i < end; i++)
  {
    double d = PerpendicularDistance(pointList[i], pointList[0], pointList[end]);
    if (d > dmax)
    {
      index = i;
      dmax = d;
    }
  }

  // If max distance is greater than epsilon, recursively simplify
  if(dmax > epsilon)
  {
    // Recursive call
    vector<Point> recResults1;
    vector<Point> recResults2;
    vector<Point> firstLine(pointList.begin(), pointList.begin()+index+1);
    vector<Point> lastLine(pointList.begin()+index, pointList.end());
    RamerDouglasPeucker(firstLine, epsilon, recResults1);
    RamerDouglasPeucker(lastLine, epsilon, recResults2);
 
    // Build the result list
    out.assign(recResults1.begin(), recResults1.end()-1);
    out.insert(out.end(), recResults2.begin(), recResults2.end());
    if(out.size()<2)
      throw runtime_error("Problem assembling output");
  } 
  else 
  {
    //Just return start and end points
    out.clear();
    out.push_back(pointList[0]);
    out.push_back(pointList[end]);
  }
  return;
}

void getPoints(int x1, int y1, int x2, int y2)
{
  if(abs(x1 - x2) < 2)
  {
    for(int i = min(y1, y2) + 1; i<max(y1, y2); i++)
      temp.push_back(Point(x1, i));
    return;
  }
  if(abs(y1 - y2) < 2)
  {
    for(int i = min(x1, x2) + 1; i<max(x1, x2); i++)
      temp.push_back(Point(i, y1));
    return;
  }

  int midx = (x1 + x2)>>1;
  int midy = (y1 + y2)>>1;

  temp.push_back(Point(midx, midy));

  getPoints(x1, y1, midx, midy);
  getPoints(midx, midy, x2, y2);

  return;
}