#include <bits/stdc++.h>
using namespace std;

struct TRI{
	int A, B, C;	
};

const int N = 100000;
int X[N], Y[N];
int col[3][2000][2000];
int fincol[3][2000][2000];

int main()
{
	freopen("../../outputs/data/originalColors.data", "r", stdin);

	// cout << "Here0\n";

	int SizeW, SizeH;
	cin >> SizeW >> SizeH;

	for(int x = 0; x<SizeW; x++)
		for(int y = 0; y<SizeH; y++)
			for(int i = 0; i<3; i++)
				cin >> col[i][x][y];

	// cout << "Here1\n";

	freopen("../../outputs/data/triangle.1.ele", "r", stdin);

	int temp, ind;

	int ntri;
	cin >> ntri >> temp >> temp;

	TRI triangles[ntri];
	for(int i = 0; i<ntri; i++)
	{
		cin >> ind;
		cin >> triangles[i].A >> triangles[i].B >> triangles[i].C;
	}

	// cout << "Here2\n";

	freopen("../../outputs/data/triangle.1.node", "r", stdin);

	int nPoints;
	cin >> nPoints >> temp >> temp >> temp;

	double tempX, tempY;

	for(int i = 0; i<nPoints; i++)
	{
		cin >> ind;
		cin >> tempX >> tempY >> temp;
		tempY = -1.0*tempY;
		X[ind] = floor(tempX + 0.5);
		Y[ind] = floor(tempY + 0.5);
	}

	// cout << "Here3\n";

	set< pair<int, int> > boundary;
	vector< pair<int, int> > Points;
	vector<int> coltri[3];

	int med[3];

	int x1, y1, x2, y2;

	for(int i = 0; i<ntri; i++)
	{
		boundary.clear();
		boundary.insert(make_pair(Y[triangles[i].A], X[triangles[i].A]));
		boundary.insert(make_pair(Y[triangles[i].B], X[triangles[i].B]));
		boundary.insert(make_pair(Y[triangles[i].C], X[triangles[i].C]));

		x1 = X[triangles[i].A];
		y1 = Y[triangles[i].A];

		x2 = X[triangles[i].B];
		y2 = Y[triangles[i].B];

		for(int y = min(y1, y2)+1; y<max(y1, y2); y++)
		{
			int x = (x1*(y2 - y) - x2*(y1 - y))/(y2 - y1);
			boundary.insert(make_pair(y, x));
		}

		x1 = X[triangles[i].A];
		y1 = Y[triangles[i].A];

		x2 = X[triangles[i].C];
		y2 = Y[triangles[i].C];

		for(int y = min(y1, y2)+1; y<max(y1, y2); y++)
		{
			int x = (x1*(y2 - y) - x2*(y1 - y))/(y2 - y1);
			boundary.insert(make_pair(y, x));
		}

		x1 = X[triangles[i].B];
		y1 = Y[triangles[i].B];

		x2 = X[triangles[i].C];
		y2 = Y[triangles[i].C];

		for(int y = min(y1, y2)+1; y<max(y1, y2); y++)
		{
			int x = (x1*(y2 - y) - x2*(y1 - y))/(y2 - y1);
			boundary.insert(make_pair(y, x));
		}

		Points.clear();

		int lasty = -1, minx, maxx;
		for(set< pair<int, int> >::iterator it = boundary.begin(); it != boundary.end(); it++)
		{
			if(lasty != it->first)
			{
				if(lasty != -1)
				{
					for(int x = minx; x<=maxx; x++)
						Points.push_back(make_pair(x, lasty));
				}
				lasty = it->first;
				minx = it->second;
				maxx = it->second;

				continue;
			}

			minx = min(minx, it->second);
			maxx = max(maxx, it->second);
		}

		if(lasty != -1)
		{
			for(int x = minx; x<=maxx; x++)
				Points.push_back(make_pair(x, lasty));
		}

		for(int j = 0; j<3; j++)
			coltri[j].clear();

		for(int j = 0; j<Points.size(); j++)
			for(int k = 0; k<3; k++)
				if(Points[j].first >= 0)
					coltri[k].push_back(col[k][Points[j].first][Points[j].second]);

		for(int j = 0; j<3; j++)
		{
			assert(coltri[j].size() != 0);
			sort(coltri[j].begin(), coltri[j].end());
			med[j] = coltri[j][coltri[j].size() / 2];
		}

		for(int j = 0; j<Points.size(); j++)
			for(int k = 0; k<3; k++)
				if(Points[j].first >= 0)
					fincol[k][Points[j].first][Points[j].second] = med[k];
	}

	freopen("../../outputs/data/finalColors.data", "w", stdout);

	for(int x = 0; x<SizeW; x++)
	{
		for(int y = 0; y<SizeH; y++)
			for(int k = 0; k<3; k++)
				cout << fincol[k][x][y] << " ";
		cout << endl;
	}

	return 0;
}	