#include <string.h>
#include <algorithm>
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
#include <cstdio>
#include <cstdlib>

typedef long long ll;
ll L, R, Q;

int count_div(ll x)
{
	int cnt = 0;
	for (ll i = 1; i * i <= x; i++)
	{
		if (x % i == 0)
		{
			cnt++;
			if (i != x / i)
				cnt++;
		}
	}
	return cnt;
}

int main()
{
	scanf(INT64 "" INT64 "" INT64, &L, &R, &Q);
	ll aL = L / Q;
	ll aR = R / Q;
	ll target = (aL == aR ? aL * Q : Q);
	if (target == 0)
		puts("infinity");
	else
		printf("%d\n", count_div(target));
	return 0;
}
