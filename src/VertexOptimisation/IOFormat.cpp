#include <bits/stdc++.h>
using namespace std;

int main()
{
	ios::sync_with_stdio(false);

	freopen("../../outputs/data/douglasSegments.data", "r", stdin);

	int SizeW, SizeH;
	cin >> SizeW >> SizeH;

	int segs;
	cin >> segs;

	vector< vector< pair<int, int> > > segments;
	segments.clear();

	vector< pair<int, int> > segment;

	int n, x, y;
	for(int i = 0; i<segs; i++)
	{
		segment.clear();

		cin >> n;
		while(n--)
		{
			cin >> y >> x;
			segment.push_back(make_pair(x, y));
		}
		segments.push_back(segment);
	}

	freopen("../../outputs/data/optimisedSeeds.data", "r", stdin);

	cin >> n;

	set< pair<int, int> > otherPoints;
	otherPoints.clear();

	for(int i = 0; i<n; i++)
	{
		cin >> x >> y;
		otherPoints.insert(make_pair(x, y));
	}

	n = otherPoints.size();

	freopen("../../outputs/data/triangle.poly", "w", stdout);

	int cnt = 0, total = 0;
	for(int i = 0; i<segs; i++)
		total += segments[i].size();

	cout << total + n + 4 << " 2 0 0\n";
	for(int i = 0; i<segs; i++)
	{
		for(int j = 0; j<segments[i].size(); j++)
		{
			cnt++;
			cout << cnt << " " << segments[i][j].first << " " << -1*segments[i][j].second << " 0\n";
		}
	}

	for(set< pair<int, int> >::iterator it = otherPoints.begin(); it != otherPoints.end(); it++)
	{
		cnt++;
		cout << cnt << " " << it->first << " " << -1*(it->second) << " 0\n";
	}

	// x SizeW
	// y SizeH
	cnt++;
	cout << cnt << " " << 0 << " " << 0 << " " << 0 << "\n";
	cnt++;
	cout << cnt << " " << SizeW << " " << 0 << " " << 0 << "\n";
	cnt++;
	cout << cnt << " " << 0 << " " << -1 * SizeH << " " << 0 << "\n";
	cnt++;
	cout << cnt << " " << SizeW << " " << -1 * SizeH << " " << 0 << "\n";

	int last = cnt;

	cout << total - segs + 4 << " 0\n";
	cnt = 0;
	int cnt1 = 0;
	for(int i = 0; i<segs; i++)
	{
		cnt++;
		for(int j = 1; j<segments[i].size(); j++)
		{
			cnt++;
			cnt1++;
			cout << cnt1 << " " << cnt-1 << " " << cnt << " 0\n";
		}
	}
	cnt1++;
	cout << cnt1 << " " << last-3 << " " << last-2 << " 0\n";
	cnt1++;
	cout << cnt1 << " " << last-3 << " " << last-1 << " 0\n";
	cnt1++;
	cout << cnt1 << " " << last-2 << " " << last << " 0\n";
	cnt1++;
	cout << cnt1 << " " << last-1 << " " << last << " 0\n";
	
	assert(cnt1 == (total - segs + 4));

	cout << "0\n";

	return 0;
}