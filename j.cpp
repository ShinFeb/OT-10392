#include <stdio.h>
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <set>
#include <unordered_set>
using namespace std;
typedef long long ll;

const int MAXN = 200100;
const int TREE = 1 << 18; // 262144
const int INF = (int)1e9;
const ll LINF = (ll)1e18;

int sign(int a)
{
	return a > 0 ? 1 : a < 0 ? -1 : 0;
}

bool cmp(int *a, int *b, int len)
{
	for (int i = 0; i < len; ++i)
		if (a[i] != b[i])
			return a[i] < b[i];
	return false;
}

struct Tree
{
	int tree[TREE * 2];
	int pushed[TREE * 2];
	int size;

	Tree(): size(0)
	{
		for (int i = 0; i < TREE * 2; ++i)
			tree[i] = INF;
	}
	
	void init(int node, int node_l, int node_r, int seg_l, int seg_r)
	{
		pushed[node] = 0;
		if (node_r <= seg_l || node_l >= seg_r)
			return;
		if (node < TREE)
		{
			init(node * 2, node_l, (node_l + node_r) / 2, seg_l, seg_r);
			init(node * 2 + 1, (node_l + node_r) / 2, node_r, seg_l, seg_r);
			tree[node] = min(tree[node * 2], tree[node * 2 + 1]);
		}
	}

	void init(int *ar, int new_size)
	{
		for (int i = TREE; i < TREE + new_size; ++i)
			tree[i] = ar[i - TREE];
		for (int i = TREE + new_size; i < TREE + size; ++i)
			tree[i] = INF;
		init(1, 0, TREE, 0, max(size, new_size));
		size = new_size;
	}
	
	void push(int node)
	{
		if (pushed[node] == 0)
			return;
		tree[node] -= pushed[node];
		if (node < TREE)
		{
			pushed[node * 2] += pushed[node];
			pushed[node * 2 + 1] += pushed[node];
		}
		pushed[node] = 0;
	}

	/*int get_min(int node, int node_l, int node_r, int seg_l, int seg_r)
	{
		push(node);
		if (node_r <= seg_l || node_l >= seg_r)
			return INF;
		if (node_l >= seg_l && node_r <= seg_r)
			return tree[node];
		return min(get_min(node * 2, node_l, (node_l + node_r) / 2, seg_l, seg_r),
			get_min(node * 2 + 1, (node_l + node_r) / 2, node_r, seg_l, seg_r));
	}

	int get_min(int seg_l, int seg_r)
	{
		return get_min(1, 0, TREE, seg_l, seg_r);
	}*/
	
	void dec(int node, int node_l, int node_r, int seg_l, int seg_r, int value)
	{
		push(node);
		if (node_r <= seg_l || node_l >= seg_r)
			return;
		if (node_l >= seg_l && node_r <= seg_r)
		{
			pushed[node] += value;
			return;
		}
		dec(node * 2, node_l, (node_l + node_r) / 2, seg_l, seg_r, value);
		dec(node * 2 + 1, (node_l + node_r) / 2, node_r, seg_l, seg_r, value);
		tree[node] = min(tree[node * 2] - pushed[node * 2], tree[node * 2 + 1] - pushed[node * 2 + 1]);
	}

	void dec(int seg_l, int seg_r, int value)
	{
		dec(1, 0, TREE, seg_l, seg_r, value);
	}

	int find(int node, int node_l, int node_r, int seg_l, int seg_r, int value)
	{
		push(node);
		if (tree[node] > value)
			return -1;
		if (node_r <= seg_l || node_l >= seg_r)
			return -1;
		if (node >= TREE)
			return node_l;
		int found = find(node * 2 + 1, (node_l + node_r) / 2, node_r, seg_l, seg_r, value);
		if (found == -1)
			found = find(node * 2, node_l, (node_l + node_r) / 2, seg_l, seg_r, value);
		return found;
	}
	
	int find(int seg_l, int seg_r, int value)
	{
		if (seg_l >= seg_r)
			return -1;
		return find(1, 0, TREE, seg_l, seg_r, value);
	}
};

struct DSU
{
	int par[MAXN];
	int rank[MAXN];
	int mx[MAXN];
	bool marked[MAXN];
	DSU() {}

	void init(int size)
	{
		for (int i = 0; i < size; ++i)
		{
			par[i] = i;
			rank[i] = 0;
			mx[i] = i;
			marked[i] = false;
		}
	}

	int get_par(int i)
	{
		if (par[i] == i)
			return i;
		return par[i] = get_par(par[i]);
	}

	int get_max(int i)
	{
		i = get_par(i);
		return mx[i];
	}

	bool get_mark(int i)
	{
		i = get_par(i);
		return marked[i];
	}

	void set_mark(int i, bool val)
	{
		i = get_par(i);
		marked[i] = val;
	}

	bool merge(int a, int b)
	{
		a = get_par(a);
		b = get_par(b);
		if (a == b)
			return false;
		if (marked[a] != marked[b])
			throw;
		if (rank[a] < rank[b])
			swap(a, b);
		if (rank[a] == rank[b])
			++rank[a];
		mx[a] = max(mx[a], mx[b]);
		par[b] = a;
		return true;
	}
};

Tree tree;
int prefixes[MAXN];
int sorted[MAXN];
int available[MAXN];
DSU been;
int doit_res[MAXN];

bool checker[MAXN];

void doit(int n)
{
	copy(prefixes, prefixes + n, sorted);
	sort(sorted, sorted + n);
	copy(sorted, sorted + n, available);
	for (int i = n - 1; i >= 0; --i)
	{
		if (i != n - 1 && sorted[i] == sorted[i + 1])
			available[i] = available[i + 1];
		else
			available[i] -= i;
		checker[i] = false;
	}
	tree.init(available, n);
	been.init(n);
	for (int i = 0; i < n; ++i)
	{
		int found = tree.find(0, upper_bound(sorted, sorted + n, prefixes[i] - 1) - sorted, 0);
		int j = found == -1 ? 0 : sorted[found] + 1;
		if (been.get_mark(j))
			j = been.get_max(j) + 1;
		been.set_mark(j, true);
		if (j > 0 && been.get_mark(j - 1))
			been.merge(j - 1, j);
		if (j < n - 1 && been.get_mark(j + 1))
			been.merge(j, j + 1);
		doit_res[i] = j;
		tree.dec(lower_bound(sorted, sorted + n, j) - sorted,
			upper_bound(sorted, sorted + n, prefixes[i] - 1) - sorted, 1);
	}
	for (int i = 0; i < n; ++i)
		if (doit_res[i] > prefixes[i])
			throw;
	for (int i = 0; i < n; ++i)
	{
		if (checker[doit_res[i]])
			throw;
		checker[doit_res[i]] = true;
	}
}

pair<pair<int, int>, int> pre_prefixes[MAXN];

int pos[2][MAXN];
int dest[2][MAXN];
int true_dest[2][MAXN];

int s[MAXN];
int n;

ll solve(int first, int *res)
{
	ll sum = 0;
	int ind[2] = {0, 0};
	for (int i = 0; i < n; ++i)
	{
		int mod = s[i] & 1;
		pos[mod][ind[mod]] = i;
		dest[mod][ind[mod]] = ind[mod] ? dest[mod][ind[mod] - 1] + 2 : first ^ mod;
		sum += abs(dest[mod][ind[mod]] - pos[mod][ind[mod]]);
		++ind[mod];
	}

	for (int mod = 0; mod < 2; ++mod)
	{
		int dir = 0;
		int count = 1;
		for (int i = 0; i <= ind[mod]; ++i)
		{
			int cur_dir = i == ind[mod] ? 0 : sign(dest[mod][i] - pos[mod][i]);
			if (cur_dir == 0)
				true_dest[mod][i] = dest[mod][i];
			if (dir != 0 && cur_dir != dir)
			{
				int border_i = dir == 1 ? i - 1 : i - count;
				int border = dest[mod][border_i];
				for (int j = 0; j < count; ++j)
					pre_prefixes[j] = make_pair(make_pair(-dir * s[pos[mod][border_i - dir * j]],
						abs(pos[mod][border_i - dir * j] - border) / 2), j);
				sort(pre_prefixes, pre_prefixes + count);
				for (int j = 0; j < count; ++j)
					prefixes[j] = pre_prefixes[j].first.second;
				doit(count);
				for (int j = 0; j < count; ++j)
					true_dest[mod][border_i - dir * pre_prefixes[j].second] = border - dir * doit_res[j] * 2;
			}
			if (cur_dir == dir)
				++count;
			else
				count = 1;
			dir = cur_dir;
		}
	}
	
	ll sum2 = 0;
	ind[0] = 0;
	ind[1] = 0;
	for (int i = 0; i < n; ++i)
	{
		int mod = s[i] & 1;
		res[true_dest[mod][ind[mod]]] = s[i];
		sum2 += abs(true_dest[mod][ind[mod]] - pos[mod][ind[mod]]);
		++ind[mod];
	}

	if (sum != sum2)
		throw;
	return sum;
}

int res[2][MAXN];
ll res_sum[2];

void solve()
{
	scanf("%d", &n);
	int count[2] = {0, 0};
	for (int i = 0; i < n; ++i)
	{
		scanf("%d", &s[i]);
		++count[s[i] & 1];
	}
	res_sum[0] = count[0] >= count[1] ? solve(0, res[0]) : LINF;
	res_sum[1] = count[1] >= count[0] ? solve(1, res[1]) : LINF;
	int *ans = res_sum[0] < res_sum[1] ||
		res_sum[0] == res_sum[1] && cmp(res[0], res[1], n) ?
		res[0] : res[1];
	for (int i = 0; i < n; ++i)
		printf("%d ", ans[i]);
}

int main()
{
#ifdef LOCAL
	freopen("input.txt", "r", stdin);
	freopen("output.txt", "w", stdout);
#endif

	/*int t;
	scanf("%d", &t);
	while(t--)
	{
		int n;
		scanf("%d", &n);
		for (int i = 0; i < n; ++i)
			scanf("%d", &prefixes[i]);
		doit(n);
		for (int i = 0; i < n; ++i)
			printf("%d ", doit_res[i]);
		printf("\n");
	}*/

	solve();

	return 0;
}