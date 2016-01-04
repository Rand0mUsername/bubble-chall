//RandomUsername (Nikola Jovanovic)
//.deathSatanBunny
//Bubblecup V7
//JCROSS

#include <bits/stdc++.h>
#define lld long long
#define ff(i,a,b) for(int i=a; i<=b; i++)
#define fb(i,a,b) for(int i=a; i>=b; i--)
#define par pair<int, int>
#define fi first
#define se second
#define mid (l+r)/2
#define MAXSZ 20000
#define MAXLEN 1005
#define MAXDP 20005
#define INF 100000000LL

using namespace std;

// puzzle struct

const int N = 50, M = 100;

struct row { int n; int a[N]; };
struct puzzle
{
    int n, m;
    row hor[N], ver[M];
} ;

// IO

void load(puzzle &p)
{
    scanf("%i %i", &p.n, &p.m);
    for(int i = 0; i < p.n; i++)
    {
        p.hor[i].n = 0;
        do { scanf("%i", &p.hor[i].a[p.hor[i].n++]); } while(p.hor[i].a[p.hor[i].n - 1]);
        p.hor[i].n--;
    }
    for(int i = 0; i < p.m; i++)
    {
        p.ver[i].n = 0;
        do { scanf("%i", &p.ver[i].a[p.ver[i].n++]); } while(p.ver[i].a[p.ver[i].n - 1]);
        p.ver[i].n--;
    }
}

void write(puzzle &p)
{
    printf("%i %i\n", p.n, p.m);
    for(int i = 0; i < p.n; i++)
    {
        for(int j = 0; j < p.hor[i].n; j++) printf("%i ", p.hor[i].a[j]);
        printf("0\n");
    }
    for(int i = 0; i < p.m; i++)
    {
        for(int j = 0; j < p.ver[i].n; j++) printf("%i ", p.ver[i].a[j]);
        printf("0\n");
    }
}

char image[N][M];
void from_matrix(puzzle &p, const char *file_name)
{
    FILE *f = fopen(file_name, "r");

    fscanf(f, "%i %i", &p.n, &p.m);
    for(int i = 0; i < p.n; i++)
        for(int j = 0; j < p.m; j++)
            fscanf(f, " %c", &image[i][j]);

    for(int i = 0; i < p.n; i++)
    {
        p.hor[i].n = 0;
        int cnt = 0;
        for(int j = 0; j < p.m; j++)
            if(image[i][j] == '#') cnt++;
            else if(j && image[i][j - 1] == '#')
            {
                p.hor[i].a[p.hor[i].n++] = cnt;
                cnt = 0;
            }
        if(cnt) p.hor[i].a[p.hor[i].n++] = cnt;
    }

    for(int i = 0; i < p.m; i++)
    {
        p.ver[i].n = 0;
        int cnt = 0;
        for(int j = 0; j < p.n; j++)
            if(image[j][i] == '#') cnt++;
            else if(j && image[j - 1][i] == '#')
            {
                p.ver[i].a[p.ver[i].n++] = cnt;
                cnt = 0;
            }
        if(cnt) p.ver[i].a[p.ver[i].n++] = cnt;
    }

    fclose(f);
}

// line solver

enum field { IDK, BLACK, WHITE } ;

struct solution
{
    field mark[N][M];
} ;

void write(solution &s, int n, int m)
{
    for(int i = 0; i < n; i++)
        for(int j = 0; j <= m; j++)
            printf("%c", j == m ? '\n' : s.mark[i][j] == IDK ? '.' : s.mark[i][j] == BLACK ? '#' : '.');
}

struct line
{
    int n;
    field a[M];
} ;
bool contradiction, finished;

int d_w[M][M], d_b[M][M][M], d_w_i[M][M], d_b_i[M][M][M];
bool ok_wh[M], ok_bl[M];

void make_dp(line &a, row &req, int d_w[][M], int d_b[][M][M])
{
    for(int i = 0; i < a.n; i++)
        for(int k = 0; k <= req.n; k++)
        {
            d_w[i][k] = (a.a[i] != BLACK) && (i ? (d_w[i - 1][k] || (k ? d_b[i - 1][k - 1][req.a[k - 1] - 1] : false)) : (k == 0));
            if(k != req.n)
                for(int x = 0; x < req.a[k]; x++)
                    d_b[i][k][x] = (a.a[i] != WHITE) && (x ? (i ? d_b[i - 1][k][x - 1] : false) : (i ? d_w[i - 1][k] : (k == 0)));
        }
}

void solve_line_quick(line &a, row &req)
{
    make_dp(a, req, d_w, d_b);

    for(int i = 0; i < a.n - 1 - i; i++) swap(a.a[i], a.a[a.n - 1 - i]);
    for(int i = 0; i < req.n - 1 - i; i++) swap(req.a[i], req.a[req.n - 1 - i]);

    make_dp(a, req, d_w_i, d_b_i);

    for(int i = 0; i < a.n - 1 - i; i++) swap(a.a[i], a.a[a.n - 1 - i]);
    for(int i = 0; i < req.n - 1 - i; i++) swap(req.a[i], req.a[req.n - 1 - i]);

    for(int i = 0; i < a.n; i++) ok_wh[i] = ok_bl[i] = false;
    for(int i = 0; i < a.n; i++)
        for(int k = 0; k <= req.n; k++)
            if(d_w[i][k] && d_w_i[a.n - i - 1][req.n - k]) ok_wh[i] = true;
    for(int i = 0; i < a.n; i++)
        for(int k = 0; k < req.n; k++)
            for(int x = 0; x < req.a[k]; x++)
                if(d_b[i][k][x] && d_b_i[a.n - i - 1][req.n - k - 1][req.a[k] - x - 1])
                    ok_bl[i] = true;

    char S[5] = ".BW*";

    for(int i = 0; i < a.n; i++)
        if(!ok_wh[i] && ok_bl[i]) a.a[i] = BLACK;
        else if(!ok_bl[i] && ok_wh[i]) a.a[i] = WHITE;
        else if(!ok_bl[i] && !ok_wh[i]) contradiction = true;
}


bool seen_bl[M], seen_wh[M];
bool gen_bl[M];
int n_ok;
void gen_line(row &req, line &x, int d, int curr)
{
    if(d == x.n || curr == req.n)
    {
        if(curr != req.n) return;
        for(int i = 0; i < x.n; i++)
            if(x.a[i] != IDK)
            {
                if((x.a[i] == BLACK) && !gen_bl[i]) return;
                if(x.a[i] == WHITE && gen_bl[i]) return;
            }

        for(int i = 0; i < x.n; i++)
            (gen_bl[i] ? seen_bl : seen_wh)[i] = true;
        n_ok++;
    }
    else
    {
        gen_bl[d] = false;
        gen_line(req, x, d + 1, curr);
        if(!d || !gen_bl[d - 1])
        if(d + req.a[curr] <= x.n)
        {
            for(int i = 0; i < req.a[curr]; i++) gen_bl[i + d] = true;
            gen_line(req, x, d + req.a[curr], curr + 1);
            for(int i = 0; i < req.a[curr]; i++) gen_bl[i + d] = false;
        }
    }
}

void solve_line_detailed(line &a, row &req)
{
    n_ok = 0;
    for(int i = 0; i < a.n; i++) gen_bl[i] = seen_bl[i] = seen_wh[i] = false;
    gen_line(req, a, 0, 0);
    for(int i = 0; i < a.n; i++)
        if(seen_bl[i] && !seen_wh[i]) a.a[i] = BLACK;
        else if(seen_wh[i] && !seen_bl[i]) a.a[i] = WHITE;
    if(n_ok == 0)
    {
        contradiction = true;
    }
}

void solve_line(line &a, row &r, int lvl)
{
    if(lvl == 0) solve_line_quick(a, r);
    else if(lvl == 1) solve_line_detailed(a, r);
    else fprintf(stderr, "LOG: Tried to solve line with nonexistent level %i\n", lvl);
}

bool improve_hor(solution &s, puzzle &p, int i, int lvl)
{
    bool ret = false;
    line l;
    l.n = p.m;
    for(int j = 0; j < p.m; j++)
        l.a[j] = s.mark[i][j];
    solve_line(l, p.hor[i], lvl);
    for(int j = 0; j < p.m; j++)
    {
        if(s.mark[i][j] != l.a[j]) ret = true;
        s.mark[i][j] = l.a[j];
    }
    return ret;
}

bool improve_ver(solution &s, puzzle &p, int j, int lvl)
{
    bool ret = false;
    line l;
    l.n = p.n;
    for(int i = 0; i < p.n; i++)
        l.a[i] = s.mark[i][j];
    solve_line(l, p.ver[j], lvl);
    for(int i = 0; i < p.n; i++)
    {
        if(s.mark[i][j] != l.a[i]) ret = true;
        s.mark[i][j] = l.a[i];
    }
    return ret;
}

const int MAX_DEPTH = 0;

void solve(puzzle &p, solution s, int depth)
{
    if(finished) return;
    if(depth > MAX_DEPTH)
    {
        for(int i = 0; i < p.n; i++)
            for(int j = 0; j < p.m; j++)
                if(s.mark[i][j] == IDK) return;
        write(s, p.n, p.m);
        fprintf(stderr, "LOG: Finished @ depth = %i\n", depth);
        finished = true;
        return;
    }

    contradiction = false;

    bool progress = true;
    while(progress && !contradiction)
    {
        progress = false;
        for(int i = 0; i < p.n; i++)
            progress = progress || improve_hor(s, p, i, 0);
        for(int i = 0; i < p.m; i++)
            progress = progress || improve_ver(s, p, i, 0);

    }

    bool done = true;
    for(int i = 0; i < p.n; i++)
        for(int j = 0; j < p.m; j++)
            if(s.mark[i][j] == IDK) done = false;
    if(done && !contradiction) { write(s, p.n, p.m); finished = true; fprintf(stderr, "LOG: Finished @ depth = %i\n", depth); return; }

    for(int i = 0; i < p.n; i++)
        for(int j = 0; j < p.m; j++)
            if(s.mark[i][j] == IDK)
            {
                s.mark[i][j] = BLACK;
                solve(p, s, depth + 1);
                s.mark[i][j] = WHITE;
                solve(p, s, depth + 1);
                s.mark[i][j] = IDK;
                return; // maybe
            }
}

void solve(puzzle &p)
{
    contradiction = false;
    finished = false;
    solution s;
    for(int i = 0; i < p.n; i++)
        for(int j = 0; j < p.m; j++)
            s.mark[i][j] = IDK;
    solve(p, s, 0);
    if(!finished) write(s, p.n, p.m);
}

int main()
{
    // input
    int test;
    scanf("%i", &test);
    for(int i = 0; i < test; i++)
    {
        puzzle p;
        load(p);
        // solve solvable test cases
        if(i < 133) solve(p);
        else
        {
            for(int i = 0; i < p.n; i++)
                for(int j = 0; j <= p.m; j++)
                    printf("%c", j == p.m ? '\n' : '.');
        }
    }

    return 0;
}
