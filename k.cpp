#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <algorithm>
using namespace std;

const int N = 55;
const int INF = (short) 3e4;
const int MAX_COM = 100;

const int DX[] = {-1, 0, 1, 0};
const int DY[] = {0, -1, 0, 1};

int n, m;
char board[N][N];
char buf[N * N];
int sx, sy;
int fx, fy;

short inner_dist[3][N * N][N * N];
short inner_par[3][N * N][N * N];

short go[N * N][N * N];
short go_mid[N * N][N * N];
short go_mid_type[N * N][N * N];

short global_dist[N * N];
short global_par[N * N];

int q[N * N];
int ql, qr;

int encode(int x, int y)
{
    return x * m + y;
}

void decode(int v, int & x, int & y)
{
    x = v / m;
    y = v % m;
}

void mirror(int midx, int midy, int type, int x, int y,
        int & nx, int & ny)
{
    nx = 2 * midx - x;
    ny = 2 * midy - y;
    if (type == 1)
        nx++;
    if (type == 2)
        ny++;
}

bool good_cell(int x, int y)
{
    return x >= 0 && x < n && y >= 0 && y < m && board[x][y] == '.';
}

void inner_bfs(int startx, int starty, int dx, int dy)
{
    int type = 2 * dy + dx;
    int start = encode(startx, starty);
    short * dist = inner_dist[type][start];
    short * par = inner_par[type][start];

    fill(dist, dist + n * m, INF);
    fill(par, par + n * m, -1);
    dist[start] = dx + dy;
    ql = qr = 0;
    q[qr++] = start;
    if (type > 0)
    {
        int neighbour = encode(startx + dx, starty + dy);
        dist[neighbour] = 1;
        par[neighbour] = start;
        par[start] = neighbour;
        q[qr++] = neighbour;
    }

    while (ql < qr)
    {
        int v = q[ql++];
        int x, y;
        decode(v, x, y);

        for (int d = 0; d < 4; d++)
        {
            int nx = x + DX[d];
            int ny = y + DY[d];
            if (!good_cell(nx, ny))
                continue;
            int symx, symy;
            mirror(startx, starty, type, nx, ny, symx, symy);
            if (!good_cell(symx, symy))
                continue;

            int to = encode(nx, ny);
            if (dist[to] == INF)
            {
                dist[to] = dist[v] + 2;
                par[to] = v;
                q[qr++] = to;
            }
        }
    }

    for (int v = 0; v < n * m; v++)
    {
        if (dist[v] > MAX_COM)
            continue;

        int x, y;
        decode(v, x, y);
        int symx, symy;
        mirror(startx, starty, type, x, y, symx, symy);
        int to = encode(symx, symy);

        if (go[v][to] > dist[v])
        {
            go[v][to] = go[to][v] = dist[v];
            go_mid[v][to] = go_mid[to][v] = start;
            go_mid_type[v][to] = go_mid_type[to][v] = type;
        }
    }
}

void bfs()
{
    int start = encode(sx, sy);
    fill(global_dist, global_dist + n * m, INF);
    fill(global_par, global_par + n * m, -1);
    global_dist[start] = 0;
    ql = qr = 0;
    q[qr++] = start;

    while (ql < qr)
    {
        int v = q[ql++];

        for (int to = 0; to < n * m; to++)
        {
            if (go[v][to] == INF)
                continue;

            if (global_dist[to] == INF)
            {
                global_dist[to] = global_dist[v] + 1;
                global_par[to] = v;
                q[qr++] = to;
            }
        }
    }
}

char get_dir(int from, int to)
{
    int x1, y1;
    decode(from, x1, y1);
    int x2, y2;
    decode(to, x2, y2);

    int dx = x2 - x1;
    int dy = y2 - y1;
    if (dx == 1)
        return 'U';
    if (dx == -1)
        return 'D';
    if (dy == 1)
        return 'L';
    return 'R';
}

int res_total;
vector<string> res;

void print_command(int from, int to)
{
    int mid = go_mid[from][to];
    int type = go_mid_type[from][to];
    short * dist = inner_dist[type][mid];
    short * par = inner_par[type][mid];

    int ptr = 0;
    int v = to;
    while (true)
    {
        int p = par[v];
        if (p == -1)
            break;
        buf[ptr++] = get_dir(p, v);
        if (type > 0 && dist[v] == 1)
            break;
        v = p;
    }

    if (type > 0)
    {
        for (int i = ptr; i < 2 * ptr - 1; i++)
            buf[i] = buf[i - ptr];
        reverse(buf + ptr, buf + 2 * ptr - 1);
        buf[2 * ptr - 1] = 0;
    }
    else
    {
        for (int i = ptr; i < 2 * ptr; i++)
            buf[i] = buf[i - ptr];
        reverse(buf + ptr, buf + 2 * ptr);
        buf[2 * ptr] = 0;
    }

    res_total++;
    res.push_back(buf);
}

void solve()
{
    scanf("%d%d", &n, &m);
    for (int i = 0; i < n; i++)
        scanf("%s", board[i]);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
        {
            if (board[i][j] == 'F')
            {
                sx = i, sy = j;
                board[i][j] = '.';
            }
            if (board[i][j] == 'S')
            {
                fx = i, fy = j;
                board[i][j] = '.';
            }
        }

    for (int i = 0; i < n * m; i++)
        for (int j = 0; j < n * m; j++)
            go[i][j] = INF;

    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
        {
            if (!good_cell(i, j))
                continue;
            inner_bfs(i, j, 0, 0);
            if (i + 1 < n && board[i + 1][j] == '.')
                inner_bfs(i, j, 1, 0);
            if (j + 1 < m && board[i][j + 1] == '.')
                inner_bfs(i, j, 0, 1);
        }

    bfs();

    int finish = encode(fx, fy);
    if (global_dist[finish] == INF)
        puts("-1");
    else
    {
        int v = finish;
        while (true)
        {
            int p = global_par[v];
            if (p == -1)
                break;
            print_command(p, v);
            v = p;
        }
        printf("%d\n", res_total);
        for (string s : res)
            puts(s.c_str());
    }
}

int main()
{
#ifdef LOCAL
    freopen("input.txt", "r", stdin);
#endif

    solve();

    return 0;
}
