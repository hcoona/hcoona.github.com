#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <iomanip>

using namespace std;

template<typename _Ty1, typename _Ty2>
bool PredByFirstGt(const pair<_Ty1, _Ty2> &a, const pair<_Ty1, _Ty2> &b)
{
	return a.first > b.first;
}

template<typename _Ty1, typename _Ty2>
bool PredBySecondGt(const pair<_Ty1, _Ty2> &a, const pair<_Ty1, _Ty2> &b)
{
	return a.second > b.second;
}

template<typename _Ty1, typename _Ty2>
bool PredBySecondLt(const pair<_Ty1, _Ty2> &a, const pair<_Ty1, _Ty2> &b)
{
	return a.second < b.second;
}

int main(void)
{
	const int NMax = 100, MMax = 100;
	pair<int, int> deviceData[NMax][MMax];
	int mList[MMax];

	int t;
	cin >> t;
	while (t--) {
		int n;
		cin >> n;

		map<int, int> bandwidthToPrice;

		for (int nit = 0; nit < n; nit++) {
			int m;
			cin >> m;
			mList[nit] = m;
			for (int mit = 0; mit < m; mit++) {
				int b, p;
				cin >> b >> p;
				deviceData[nit][mit] = make_pair(b, p);
				bandwidthToPrice[b] = 0;
			}
			sort(&deviceData[nit][0], &deviceData[nit][m], PredByFirstGt<int, int>);
		}

		for (int nit = 0; nit < n; nit++) {
			for (map<int, int>::iterator it = bandwidthToPrice.begin(); it != bandwidthToPrice.end(); ++it) {
				if (it->second != -1) {
					pair<int, int> *startPos = &deviceData[nit][0];
					pair<int, int> *endPos = upper_bound(
						startPos,
						&deviceData[nit][mList[nit]],
						*it,
						PredByFirstGt<int, int>);
					if (startPos == endPos) {
						it->second = -1;
					} else {
						it->second += min_element(startPos, endPos, PredBySecondLt<int, int>)->second;
					}
				}
			}
		}

		double maxBperP = -1.0;
		for (map<int, int>::iterator it = bandwidthToPrice.begin(); it != bandwidthToPrice.end(); ++it) {
			double BperP = static_cast<double>(it->first) / static_cast<double>(it->second);
			maxBperP = max(maxBperP, BperP);
		}
		cout << setprecision(3) << fixed << maxBperP << endl;
	}
	return 0;
}

