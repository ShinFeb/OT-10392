#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <cstring>
using namespace std;


const int N = 1e6;

const int A = 30;
const int INF = (int)1e9;

struct Eertree
{
	int sz, last;
	vector<pair<int, int> > poses[N];
	int go[N][A];
	int len[N], diff[N];
	int link[N], series_link[N];
	int baby[N];
	int dp[N], last_dp[N];

	Eertree() : len(), diff(), link(), series_link(), dp(), last_dp()
	{
		sz = 2;
		last = 0;
		memset(go, -1, sizeof(go));
		len[0] = -1;
		fill(dp, dp + N, INF);
		dp[0] = 0;
	}

	int go_until_pal(const string &s, int i, int v)
	{
		while (s[i] != s[i - len[v] - 1])
			v = link[v];
		return v;
	}

	pair<vector<int>, vector<int> > add_char(const string &s, int i)
	{
		last = go_until_pal(s, i, last);
		int &v = go[last][s[i] - 'a'];
		if (v == -1)
		{
			v = sz++;
			len[v] = len[last] + 2;
			if (last == 0)
				link[v] = 1;
			else
			{
				last = go_until_pal(s, i, link[last]);
				link[v] = go[last][s[i] - 'a'];
			}
			diff[v] = len[v] - len[link[v]];
			if (diff[v] == diff[link[v]])
			{
				baby[v] = baby[link[v]];
				series_link[v] = series_link[link[v]];
			}
			else
			{
				baby[v] = v;
				series_link[v] = link[v];
			}
		}
		last = v;
		pair<vector<int>, vector<int> > ans;
		for(int u = v; u != 1; u = series_link[u])
		{
			int B = baby[u];
			auto & vec = poses[B];
			int left = i - len[u] + 1;
			if( vec.size() != 0 && vec.back().first == left )
			{
				vec.back().second = len[u];
			}
			else
			{
				vec.push_back( make_pair(left, len[u]) );
			}
			ans.first.push_back(i - len[B] + 1);
			if( vec.size() != 1)
			{
				int L = vec[vec.size() - 2].first;
				int len_ = vec[vec.size() - 2].second;
				int R = L + len_ - 1;
				int L_ = R - len[u] + 1;
				ans.second.push_back(L_);
			}
			if(vec.size() != 1)
			{
				if(vec[vec.size() - 1].second == vec[vec.size() - 2].second)
				{
					vec[vec.size() - 2] = vec[vec.size() - 1];
					vec.pop_back();
				}
			}
		}
		return ans;
	}

	void calc_dp(int v, int i)
	{
		int baby_len = len[series_link[v]] + diff[v];
		int res = dp[i - baby_len] + 1;
		if (link[v] != series_link[v])
			res = min(res, last_dp[link[v]]);
		last_dp[v] = res;
		dp[i] = min(dp[i], res);
	}
};

int f[N];
int n;
int res[N];


void add(int x, int val)
{
	for(int i = x; i <= n; i = (i | ( i + 1) ) )
	{
		f[i] += val;
	}
}


int get(int x)
{
	int res = 0;
	for(int i = x; i > 0; i = (i & (i + 1) ) - 1)
	{
		res += f[i];
	}
	return res;
}


int get(int L, int R)
{
	if(R < L) return 0;
	return get(R) - get(L - 1);
}

struct Three{
	int l, r, num;
	Three(): l(0), r(0), num(0) {}
	Three(int l, int r, int num): l(l), r(r), num(num) {}
	bool operator<(const Three & t) const
	{
		return r < t.r;
	}
};

Eertree eertree;

void solve(string s, vector< Three > & q)
{
	memset(f, 0, sizeof f);

	n = (int)s.length();
	s = "#" + s;
	int cur = 0;
	for(int i = 1; i <= n; i++)
	{
		auto vec = eertree.add_char(s, i);
		vector<int> adding = vec.first, deleting = vec.second; 
		for(int j = 0; j < deleting.size(); j++)
		{
			int cur = deleting[j];
			if(get(cur, cur) == 0) continue;
			if(get(cur, cur) != 1) throw;
			add(cur, -1);
		}
		for(int j = 0; j < adding.size(); j++)
		{
			int cur = adding[j];
			if(get(cur, cur) == 1) continue;
			if(get(cur, cur) != 0) throw;
			add(cur, 1);
		}
		for(; cur < q.size() && q[cur].r == i - 1; cur++)
		{
			int j = q[cur].l;
			int cnt = eertree.sz - 2 - get(1, j);
			res[q[cur].num] = cnt;
		}
	}
}


int main()
{
//    freopen("input.txt", "r", stdin);
    string s;
    cin >> s;
	int m;
	cin >> m;
	vector<Three> q;
	for(int i = 0; i < m; i++)
	{
	    int l, r;
	    cin >> l >> r;
	    l--, r--;
	    q.push_back(Three(l, r, i));
	}
	sort(q.begin(), q.end());
	solve(s, q);
	for(int i = 0; i < m; i++)
	{
		printf("%d\n", res[i]);
	}
/*	for(int len = 14; len <= 20; len++)
	{
		printf("%d\n", len);
		for(int mask = 0; mask < (1 << len); mask++)
		{
			string s;
			for(int i = 0; i < len; i++)
			{
				if( (1 << i) & mask)
				{
					s += 'a';
				}
				else
				{
					s += 'b';
				}
			}
	//	cin >> s;%
			solve(s);
		}
	}*/
	return 0;
}
