#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <bits/stdc++.h>
using namespace std;

#ifdef LOCAL
#define eprintf(...) fprintf(stderr, __VA_ARGS__)
#define print_var(x) cerr << #x << " = " << x << endl
#define print_array(arr, len) {cerr << #arr << " = "; for (int i = 0; i < len; i++) cerr << arr[i] << " "; cerr << endl;}
#define print_iterable(it) {cerr << #it << " = "; for (const auto &e : it) cerr << e << " "; cerr << endl;}
#else
#define eprintf(...) (void)0
#define print_var(x) (void)0
#define print_array(arr, len) (void)0
#define print_iterable(it) (void)0
#endif

typedef long long ll;
const int N = 200500;
const int LOG_C = 31;
const int SZ = 1 << 19;
const int TRIE = N * LOG_C;
const int A = 3;

int enc_char(char c)
{
    if (c == 'r')
        return 0;
    if (c == 'a')
        return 1;
    return 2;
}

struct Frame
{
    int x, y, r;
    char c;
    Frame(): x(), y(), r(), c() {}
    void scan()
    {
        char buf[2];
        scanf("%d%d%d%s", &x, &y, &r, buf);
        c = buf[0];
    }
};

struct SegTree
{
    int t[2 * SZ], fg[2 * SZ];
    vector<int> crd;
    SegTree(): t()
    {
        memset(fg, -1, sizeof(fg));
    }
    void process_crd()
    {
        sort(crd.begin(), crd.end());
        crd.resize(unique(crd.begin(), crd.end()) - crd.begin());
    }
    int get_crd(int p)
    {
        return lower_bound(crd.begin(), crd.end(), p) - crd.begin();
    }
    void apply(int v, int x)
    {
        t[v] = fg[v] = x;
    }
    void push(int v)
    {
        if (fg[v] == -1)
            return;
        apply(2 * v, fg[v]);
        apply(2 * v + 1, fg[v]);
        fg[v] = -1;
    }
    void set(int v, int l, int r, int a, int b, int x)
    {
        if (a > r || b < l)
            return;
        if (a <= l && r <= b)
        {
            apply(v, x);
            return;
        }
        push(v);
        int m = (l + r) / 2;
        set(2 * v, l, m, a, b, x);
        set(2 * v + 1, m + 1, r, a, b, x);
    }
    void set(int a, int b, int x)
    {
        set(1, 0, SZ - 1, get_crd(a), get_crd(b), x);
    }
    int get(int v, int l, int r, int p)
    {
        if (l == r)
            return t[v];
        push(v);
        int m = (l + r) / 2;
        if (p <= m)
            return get(2 * v, l, m, p);
        return get(2 * v + 1, m + 1, r, p);
    }
    int get(int p)
    {
        return get(1, 0, SZ - 1, get_crd(p));
    }
};

struct Event
{
    int x, y1, y2, t, id;
    Event(): x(), y1(), y2(), t(), id() {}
    Event(int _x, int _y1, int _y2, int _t, int _id):
        x(_x), y1(_y1), y2(_y2), t(_t), id(_id) {}
    bool operator<(const Event& e) const
    {
        return x < e.x;
    }
};

struct Graph
{
    int n;
    Frame frames[N];
    bool active[N];
    int parent[N], depth[N];
    SegTree tree;

    Graph(): n(), parent(), depth() {}

    void process()
    {
        scanf("%d", &n);
        for (int i = 1; i <= n; i++)
        {
            frames[i].scan();
            active[i] = true;
        }

        vector<Event> events;
        for (int i = 1; i <= n; i++)
        {
            const auto& f = frames[i];
            int R = 2 * f.r;
            events.emplace_back(f.x - R, f.y - R, f.y + R, +1, i);
            events.emplace_back(f.x + R, f.y - R, f.y + R, -1, i);
            tree.crd.push_back(f.y - R);
            tree.crd.push_back(f.y + R);
        }
        sort(events.begin(), events.end());
        tree.process_crd();

        for (const auto& e : events)
        {
            if (e.t == +1)
            {
                parent[e.id] = tree.get(e.y1);
                depth[e.id] = depth[parent[e.id]] + 1;
                if (depth[e.id] > LOG_C)
                    throw logic_error("LOG_C is too small");
                tree.set(e.y1, e.y2, e.id);
            }
            else
                tree.set(e.y1, e.y2, parent[e.id]);
        }
    }

    string navigate(int v)
    {
        string res;
        while (v != 0)
        {
            if (active[v])
                res += frames[v].c;
            v = parent[v];
        }
        reverse(res.begin(), res.end());
        return res;
    }
}
graph;

struct Dict
{
    int vcnt;
    int go[TRIE][A];
    int term[TRIE];
    int touched[TRIE], timer;
    char word[N];

    Dict(): term(), touched(), timer(), word()
    {
        vcnt = 1;
        memset(go, -1, sizeof(go));
    }

    void add_word(char w[], int len)
    {
        int v = 0;
        for (int i = 0; i < len; i++)
        {
            int ch = enc_char(w[i]);
            int& to = go[v][ch];
            if (to == -1)
                to = vcnt++;
            v = to;
            if (touched[v] < timer)
            {
                term[v]++;
                touched[v] = timer;
            }
        }
    }

    void process()
    {
        int m;
        scanf("%d", &m);
        for (int i = 0; i < m; i++)
        {
            scanf("%s", word);
            int len = strlen(word);
            timer++;
            for (int j = 0; j < len; j++)
                add_word(word + j, min(LOG_C, len - j));
        }
    }

    int query(const string& s)
    {
        int v = 0;
        for (char c : s)
        {
            int ch = enc_char(c);
            v = go[v][ch];
            if (v == -1)
                return 0;
        }
        return term[v];
    }
}
dict;

void process_queries()
{
    int q;
    scanf("%d", &q);
    for (int i = 0; i < q; i++)
    {
        char buf[2];
        int id;
        scanf("%s%d", buf, &id);
        char c = buf[0];
        if (c == '+')
            graph.active[id] = true;
        else if (c == '-')
            graph.active[id] = false;
        else
            printf("%d\n", dict.query(graph.navigate(id)));
    }
}

int main()
{
#ifdef LOCAL
    freopen("input.txt", "r", stdin);
#endif

    graph.process();
    dict.process();
    process_queries();

    eprintf("\n\ntime = %.3lf\n", (double)clock() / CLOCKS_PER_SEC);
}
