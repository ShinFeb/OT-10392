#include <stdlib.h>
#if ( _WIN32 || __WIN32__ || _WIN64 || __WIN64__ )
#define INT64 "%I64d"
#else
#define INT64 "%lld"
#endif

#if ( _WIN32 || __WIN32__ || _WIN64 || __WIN64__ )
#define UNS64 "%I64u"
#else
#define UNS64 "%llu"
#endif
#include <iostream>
#include <cstdio>
#include <vector>
#include <cstring>
#include <string>
#include <algorithm>
using namespace std;


#ifdef LOCAL
	#define eprintf(...) fprintf(stderr, __VA_ARGS__)
#else
	#define eprintf(...) 42
#endif

const int INT_INF = (int) 1e9 + 100;
const long long INF = (long long) 1e18 + 100;

const int LOG = 20;
const int LEVEL = (1 << LOG);
const int SIZE = (LEVEL << 1);



struct Func
{
	// y = -(x - st) * (x - (st - 1) ) / 2 + val
	// y = -x ^ 2 / 2 + x (st - 1 + st) / 2 - st * (st - 1) / 2 + val
	
	long long a_x2, b_x2, c_x2;
	
	Func() : a_x2(), b_x2(), c_x2() {}
	
	Func(long long st, long long val)
	{
		a_x2 = -1;
		b_x2 = 2 * st - 1;
		c_x2 = -st * (st - 1) + 2 * val;
	}

	long long getVal(long long x)
	{
		return (a_x2 * x * x + b_x2 * x + c_x2) / 2;
	}

	int getFirstPos(const Func &A) const
	{
		long long db_x2 = b_x2 - A.b_x2;
		long long dc_x2 = c_x2 - A.c_x2;

		// this - new, best on suffix
		// A - old, best on prefix
		// this - A   : increasing
		// x * b + c >= 0   <--->  x * 2b + 2c >= 0  <---> x * 2b >= -2c  <--->   x >= -2c / 2b
		
//		eprintf("2b = " INT64 ", 2c = " INT64 "\n", db_x2, dc_x2);
		if (db_x2 <= 0) throw; // incorrect order of adding Functions to Hull
		if (dc_x2 >= 0) throw; // we may return 0 for example, but still, it shouldn't happens

		return min( (-dc_x2 + db_x2 - 1) / db_x2, (long long) INT_INF );

	}
};


struct CHull
{
	vector <pair <Func, int> > hull;
	int ptr;

	CHull() : hull(), ptr() {}

	long long getVal(int i)
	{
		if (hull.empty() )
			return -INF;
		while (ptr + 1 < (int) hull.size() && hull[ptr + 1].second <= i)
			ptr++;
		return hull[ptr].first.getVal(i);
	}

	void addFunc(int i, long long val)
	{
		Func cur = Func(i, val);
		while (!hull.empty() )
		{
			int pos = hull.back().second;
			if (pos < i || cur.getVal(pos) < hull.back().first.getVal(pos) )
				break;
			hull.pop_back();
		}
	
		ptr = max(0, min(ptr, (int) hull.size() - 1) );

		if (hull.empty() )
			hull.emplace_back(cur, i);
		else if (hull.back().second < i && cur.getVal(i) > hull.back().first.getVal(i) )
		{
			hull.emplace_back(cur, i);
		}
		else
		{
			int st = cur.getFirstPos(hull.back().first);
			if (st < INT_INF)
			{
				hull.emplace_back(cur, st);
			}
		}

	}
};


struct Tree
{
	CHull hull[SIZE];
	
	Tree() : hull() {}

	long long getVal(int v, int tl, int tr, int i, int l, int r)
	{
		if (r < tl || tr < l) return -INF;
		if (l <= tl && tr <= r)
			return hull[v].getVal(i);

		int tm = (tl + tr) / 2;
		return max(
				getVal(2 * v, tl, tm, i, l, r),
				getVal(2 * v + 1, tm + 1, tr, i, l, r)
				);
	}

	long long getVal(int i, int x)
	{
		long long ans = getVal(1, 0, LEVEL - 1, i, 0, x);
//		eprintf("getVal(%d, %d) = " INT64 "\n", i, x, ans);
		return ans;
	}

	void addFunc(int i, int x, long long val)
	{
		int pos = x + LEVEL;
		while (pos >= 1)
		{
			hull[pos].addFunc(i, val);
			pos /= 2;
		}
	}
} tree;



const int N = (int) 1e6 + 100;
int a[N];
vector <int> vals;
int pos[N];

int main()
{
#ifdef LOCAL
//	freopen("input.txt", "r", stdin);
//	freopen("output.txt", "w", stdout);
#endif
	int n;
	scanf("%d", &n);
	for (int i = 0; i < n; i++)
	{
		scanf("%d", &a[i] );
		vals.push_back(a[i] );
	}

	sort(vals.begin(), vals.end() );
	vals.resize(unique(vals.begin(), vals.end() ) - vals.begin() );

	for (int i = 0; i < n; i++)
	{
		pos[i] = lower_bound(vals.begin(), vals.end(), a[i]) - vals.begin();
	}

	tree.addFunc(0, 0, 0);
	for (int i = 0; i < n; i++)
	{
//		eprintf("i = %d\n", i);
		int x = pos[i];
		long long cur = tree.getVal(i, x);
		tree.addFunc(i + 1, x, cur + a[i] );
	}
	long long ans = tree.getVal(n, LEVEL - 1);

	printf(INT64 "\n", ans);

	return 0;
}

