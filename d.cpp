#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>

using namespace std;

const int mod = (int)1e9 + 7;
const int K = 10;
const int N = (int)1e7 + 10;

int add(int a, int b)
{
	a += b;
	if (a < 0)
		return a + mod;
	if (a < mod)
		return a;
	return a - mod;
}

int mult(int a, int b)
{
	return (1LL * a * b) % mod;
}

int my_pow(int a, int k)
{
	if (k == 0)
		return 1;
	if (k & 1)
		return mult(my_pow(a, k - 1), a);
	int t = my_pow(a, k / 2);
	return mult(t, t);
}

int rev(int a)
{
	return my_pow(a, mod - 2);
}

int fact[N], rfact[N];

void init(int n)
{
	fact[0] = 1;
	for (int i = 1; i <= n * K; i++)
		fact[i] = mult(fact[i - 1], i);
	rfact[n * K] = rev(fact[n * K]);
	rfact[0] = 1;
	for (int i = n * K - 1; i > 0; --i)
		rfact[i] = mult(rfact[i + 1], i + 1);
}

int get_c(int n, int k)
{
	if (n < k || k < 0)
		return 0;
	return mult(fact[n], mult(rfact[k], rfact[n - k]));
}

int even_case(int n)
{
	return my_pow(K, n / 2);
}

int count_with_fix_sum(int len, int sum)
{
	int ans = 0;
	for (int i = 0; i <= len; i++)
	{
		int variants = get_c(len, i);
		int reduced = sum - i * K;
		if (reduced < 0)
			break;
		int partitions = get_c(reduced + len - 1, len - 1);
		ans = add(ans, (i % 2 == 0 ? 1 : -1) * mult(partitions, variants));
	}
	return ans;
}

int odd_case(int n)
{
	if (n == 1)
		return 1;
	int total_sum = (n / 2) * (K - 1);
	int ans = 0;
	for (int middle = 0; middle < K; middle++)
	{
		int remain_sum = (total_sum - middle) / 2;
		if (remain_sum * 2 + middle == total_sum)
			ans = add(ans, count_with_fix_sum(n / 2, remain_sum));
	}
	return ans;
}

int main()
{
#ifdef LOCAL
	freopen("input.txt", "r", stdin);
	freopen("output.txt", "w", stdout);
#else
	//freopen("input.txt", "r", stdin);
	//freopen("output.txt", "w", stdout);
#endif
	int n;
	scanf("%d", &n);
	init(n);
	printf("%d\n", n % 2 == 0 ? even_case(n) : odd_case(n));
	return 0;
}

