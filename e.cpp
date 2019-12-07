#include <algorithm>
#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
using namespace std;

namespace Solution
{
	const pair<int, int> NO_SOLUTION = make_pair(-1, -1);

	const int N = (int)1e5 + 10;
	vector<int> g[N];
	int deg[N];

	int used[N];

	int strange[N];
	int par[N];
	int inCycle[N];
	int n;
	pair<int, int> Block;

	void init(vector<vector<int>> &graph)
	{
		n = (int)graph.size();
		for (int i = 0; i < n; i++)
		{
			deg[i] = (int)graph[i].size();
			g[i] = graph[i];
		}
	}

	//[START] Check octopus
	bool goodEdge(int a, int b)
	{
		if (make_pair(a, b) == Block || make_pair(b, a) == Block)
			return false;
		return true;
	}
	int getSize(int v)
	{
		used[v] = 1;
		int sz = 1;
		for (int to : g[v])
		{
			if (goodEdge(v, to) && !used[to])
				sz += getSize(to);
		}
		return sz;
	}
	bool isConnected()
	{
		memset(used, 0, sizeof(used));
		return getSize(0) == n;
	}
	int getCntCycles(int v, int p)
	{
		used[v] = 1;
		int cnt = 0;
		for (int to : g[v])
		{
			if (!goodEdge(v, to))
				continue;
			if (!used[to])
			{
				par[to] = v;
				cnt += getCntCycles(to, v);
			}
			else if (used[to] == 1 && to != p)
			{
				int x = v;
				while (x != to)
				{
					inCycle[x] = 1;
					x = par[x];
				}
				inCycle[to] = 1;
				cnt++;
			}
		}
		used[v] = 2;
		return cnt;
	}
	int getCntCycles()
	{
		memset(used, 0, sizeof(used));
		memset(inCycle, 0, sizeof(inCycle));
		return getCntCycles(0, -1);
	}
	bool isOctopus()
	{
		if (!isConnected())
			return false;
		if (getCntCycles() != 1)
			return false;
		for (int i = 0; i < n; i++)
		{
			int d = 0;
			for (int to : g[i])
				if (goodEdge(i, to))
					d++;
			//Good if d == 1 || d == 2 || (d == 3 && inCycle[i])
			if (!(d == 1 || d == 2 || (d == 3 && inCycle[i])))
				return false;
		}
		return true;
	}
	//[END] Check octopus

	//[START] Get candidates
	int go(int from, int to)
	{
//		printf("%d %d\n", from, to);
		if (deg[to] == 3 || deg[to] == 1)
			return to;
		if (deg[to] == 2)
			return g[to][0] == from ? go(to, g[to][1]) : go(to, g[to][0]);
		assert(deg[to] >= 1 && deg[to] <= 3);
	}
	vector<int> getCandidates()
	{
//		for (int i = 0; i < n; i++)
//			printf("%d ", deg[i]);
		for (int i = 0; i < n; i++)
		{
			if (deg[i] > 4)
				return {};
			if (deg[i] == 4)
				return {i};
		}

		vector <int> res = {};
		for (int i = 0; i < n; i++)
		{
			if (deg[i] == 3)
			{
				vector <int> loc = vector<int>();
				for (int to : g[i])
				{
					int e = go(i, to);
					if (deg[e] == 3)
						loc.push_back(e);
				}
				if (loc.size() == 3)
				{
					strange[i] = 1;
					for (int x : loc)
						res.push_back(x);
				}
			}
		}
		return res;
	}
	//[END] Get candidates

	pair<int, int> findOctopus(vector <vector<int>> graph)
	{
		init(graph);
		vector <int> candidates = getCandidates();
//		printf("%d\n", (int)candidates.size());
		if ((int)candidates.size() > 10)
			return NO_SOLUTION;
		for (int v : candidates)
		{
//			printf("%d\n", v);
			for (int to : g[v])
			{
				Block = make_pair(v, to);
				if (isOctopus())
					return make_pair(v, to);
			}
		}
		return NO_SOLUTION;
	}
}

vector<vector<int>> g;

int main()
{
	int n, m;
	scanf("%d%d", &n, &m);
	g = vector<vector<int>>(n);	
	for (int i = 0; i < m; i++)
	{
		int a, b;
		scanf("%d%d", &a, &b);
		a--, b--;
		g[a].push_back(b);
		g[b].push_back(a);
	}
	pair<int, int> result = Solution::findOctopus(g);
	printf("%d %d\n", result.first + 1, result.second + 1);
	return 0;
}
