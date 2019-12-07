#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <iostream>
#include <cstdio>
#include<random>

using namespace std;

const int N = 1e5 + 10;
int n;
int perm[N];
mt19937 rnd(100500);

int query(int a, int b, int c)
{
	cout << "? " << a << ' ' << b << ' ' << c << endl;
	int res;
	cin >> res;
	return res;
}

int extract(vector<int>& v, int id)
{
	auto res = v[id];
	swap(v[id], v.back());
	v.pop_back();
	return res;
}

int find_zero()
{
	auto zero = 0;
	for (int i = 0; i < n; ++i)
		zero = query(zero, i, zero);
	return zero;
}

int find_one(int zero)
{
	vector<int> cands;
	for (int i = 0; i < n; ++i)
		if (i != zero)
			cands.push_back(i);
	while (cands.size() > 1)
	{
		auto x = extract(cands, rnd() % cands.size());
		auto y = extract(cands, rnd() % cands.size());
		auto q_res = query(x, y, zero);
		if (q_res == x)
			cands.push_back(y);
		else if (q_res == y)
			cands.push_back(x);
	}
	return cands.back();
}

void solve()
{
	cin >> n;
	if (n == 1)
	{
		cout << "! 0" << endl;
		return;
	}
	auto zero = find_zero();
	auto one = find_one(zero);
	perm[zero] = 0;
	perm[one] = 1;
	auto last = one;
	for (int i = 2; i < n; ++i)
	{
		last = query(last, one, one);
		perm[last] = i;
	}
	cout << "! ";
	for (int i = 0; i < n; ++i)
		cout << perm[i] << ' ';
	cout << endl;
}

int main()
{
	//freopen("input.txt", "r", stdin);

	solve();
}