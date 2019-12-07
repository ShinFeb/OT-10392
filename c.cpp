#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <map>
#include <set>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <cstring>
using namespace std;

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

typedef pair<int, int> pii;

const int N = (int)2e5 + 10;
const int SQ = 500;

int n, k, q_cnt;
int c[N], on[N];
vector<pii> blocks[N];
int delta[N];

int heavy_id[N];
int heavy_pairs[SQ][SQ];
int heavy_cnt = 0;
int ans = 0, heavy_bound = 0;

void read()
{
	scanf("%d%d%d", &n, &k, &q_cnt);
	for (int i = 0; i < n; i++)
		scanf("%d", &c[i]);
}

bool is_heavy(int color)
{
	return (int)blocks[color].size() > heavy_bound;
}

void init_blocks()
{
	int left = 0;
	for (int i = 1; i <= n; i++)
	{
		if (c[i] != c[left])
		{
			blocks[c[left]].push_back(make_pair(left == 0 ? 0 : c[left - 1], c[i]));
			left = i;
		}
	}
}

void init_heavy()
{
	for (int i = 1; i <= k; i++)
	{
		if (is_heavy(i))
		{
			delta[heavy_cnt] = (int)blocks[i].size();
			heavy_id[i] = heavy_cnt++;
		}
	}
	for (int i = 1; i <= k; i++)
	{
		if (!is_heavy(i))
			continue;
		for (auto &block : blocks[i])
		{
			if (is_heavy(block.first))
				heavy_pairs[heavy_id[i]][heavy_id[block.first]]++;
			if (is_heavy(block.second))
				heavy_pairs[heavy_id[i]][heavy_id[block.second]]++;
		}
	}
}

void prepare()
{
	heavy_bound = (int)sqrt(n) / 2 + 2;
	init_blocks();
	init_heavy();
}

int get_count_segments(int x, int y, int z)
{
	return (x + (x ^ y) + (y ^ z) + z) >> 1;
}

void apply_light(int color)
{
	int type = on[color] ? +1 : -1;
	for (auto &block : blocks[color])
	{
		ans += get_count_segments(on[block.first], !on[color], on[block.second]) - 
			get_count_segments(on[block.first], on[color], on[block.second]);

		if (is_heavy(block.first))
			delta[heavy_id[block.first]] += type;
		if (is_heavy(block.second))
			delta[heavy_id[block.second]] += type;
	}
}

void apply_heavy(int color)
{
	int id = heavy_id[color];
	int type = on[color] ? +1 : -1;

	ans += delta[id] * -type;
	for (int i = 0; i < heavy_cnt; i++)
		delta[i] += type * heavy_pairs[id][i];
}

void apply_query(int color)
{
	if (is_heavy(color))
		apply_heavy(color);
	else
		apply_light(color);
	on[color] = !on[color];
}

void solve()
{
	for (int i = 0; i < q_cnt; i++)
	{
		int color;
		scanf("%d", &color);
		apply_query(color);
		printf("%d\n", ans);
	}
}

int main()
{
	read();
	prepare();
	solve();
	return 0;
}
