//RandomUsername (Nikola Jovanovic)
//Me[N]talci Inc.
//Bubblecup V8 Round 1
//HQNP2

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

/*
Buffer:
h,H - adds helloworld
q,Q - adds source
0-9 - N copies
t,T - sort by ascii
a,A - convert to ascii decimal (1-3 digits)
b,B - convert to ascii binary (8 digits)

Accumulator:
+,- - inc/dec acc
m,M - bufferlen to acc

Deletions:
o,O - delete prime/pow2 from the end 
p,P - delete prime/pow2 from the beginning
?,!   - 47 delete from the beginning/end

Substitutions:
i,I - inc ascii everything
n,N - rot13 (num: +13 mod10)
u,U - uppercase 
l,L - lowercase 
c,C - swap case
e,E - leet
        ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 0123456789
        48(03=6#|JXLM~09Q257UVW%Y2 a6<d3f9hijk1m^0p9r57uvw*y2 O!ZEA$G/B9

Info:
1000 tps
Nlen <= 1000
source : 20000
buffer : 20000
only alphanum in buffer

Number of test cases per file : 10, 10, 100, 1000, 80, 20, 40, 500, 100, 50, 200
Total : 2110

HE : 04=DA 36=eb 38=EB 59=SP HL : 20=ZD 43=AE 97=PT
costs: 40,23,6,46,29,12,52,35,18,1
free pairs : (H,U) (I,V) (J,W), (K,X)

This solution achieves a score of 1507 points on SPOJ
*/

bool DEBUG, ALL;

// SOL 1
struct sol1
{
    char conv[10];
    int shift;
    bool in[200];
    char l;
    char s[MAXLEN];

    vector<char> out;

    char h_seq[41];
    char he_seq[24];

    void geth()
    {
        copy(h_seq, h_seq+40, back_inserter(out));
        return;
    }

    void gethe()
    {
        copy(he_seq, he_seq+23, back_inserter(out));
        return;
    }

    int d(int a, int b)
    {
        // a --> b
        return (b - a + 26) % 26;
    }

    void ROT1(bool bannedLetters)
    {
        if(bannedLetters)
        {
            if(out[out.size()-1] == 'N')
                out.pop_back();
            else
                out.push_back('N');
            out.push_back('I');
            out.push_back('N');
        }
        else
        {
            out.push_back('I');
        }
    }

    bool first;

    void adjustShift(int newShift)
    {
        if(first)
        {
            shift = newShift;
            first = false;
            return;
        }
        if(d(shift, newShift) >= 13)
        {
            if(out[out.size()-1] == 'N')
                out.pop_back();
            else
                out.push_back('N');
            shift = (shift + 13) % 26;
        }
        int dif = d(shift, newShift);
        for(int i=1; i<=dif; i++)
        {
            //check banned letters
            if(in[ 25 - shift +  65 ] || in[ 25 - shift + 97 ])
                ROT1(true);
            else
                ROT1(false);
            shift++;
            if(shift == 26) shift = 0;
        }
    }

    bool modLarge;

    // generate a single letter
    void genLetter(char c, bool two)
    {
        adjustShift( d(tolower(c), 'h') );
        if(c < 'a' && !modLarge)
        {
             if(!first) out.push_back('C');
            modLarge = true;
        }
        else if(c >= 'a' && modLarge)
        {
             if(!first) out.push_back('C');
            modLarge = false;
        }
        if(two)
            gethe();
        else
            geth();
    }

    bool HE(int a, int b)
    {
        if(a==0 && b==4) return true;
        if(a==3 && b==6) return true;
        if(a==3 && b==8) return true;
        if(a==5 && b==9) return true;
        return false;
    }


    int len;
    int Run(char* n, bool execute, int rotor)
    {
        len = strlen(n);
        memset(in, false, sizeof(in));
        out.clear();

        //first 
        first = true;
        modLarge = false;

        for(int i=0; i<len; i++)
        {
            if(i<len-1 && HE(n[i]-'0', n[i+1]-'0'))
            {
                l = conv[ n[i] - '0' ];

                //this changes when we change the map
                if(n[i+1] == '6')
                {
                    l = 'e';
                }

                genLetter(l, true);
                in[l] = true;
                l = conv[ n[i+1] - '0' ];
                in[l] = true;
                i++;
                continue;
            }
            l = conv[ n[i] - '0' ];
            if(!modLarge && (l=='Z' || l=='E' || l=='S' || l=='T'))
            {
                l += 32;
            }
            //Possible: D l Z/z E/e A S/s b T/t B P
            genLetter(l, false);
            in[l] = true;
            first = false;
        }
        if(modLarge)
            out.push_back('C');

        adjustShift(0);
        out.push_back('E');
        out.push_back('+');
        for(int i=1; i<=rotor; i++)
            out.push_back('N');

        if(!execute)
            return out.size();

        //Do I wanna solve?
        if(ALL || out.size() <= MAXSZ)
        {
            for(int i=0; i<out.size(); i++)
            {
                printf("%c", out[i]);
            }
            printf("\n");
            return true;
        }
        return false;
    }

    bool Solve(char* n)
    {
        strcpy(s, n);
        int len = strlen(s);

        int rots[10] ={0, 7, 4, 1, 8, 5, 2, 9, 6, 3};
        int bestR;
        int bestBS = -1;
        int currBS;
        for(int i=0; i<=9; i++)
        {
            currBS = Run(s, false, rots[i]);

            if(currBS < bestBS || bestBS == -1)
            {
                bestBS = currBS;
                bestR = i;
            }
            for(int j=0; j<len; j++)
            {
                if(s[j] == '0') s[j] = '9';
                else s[j]--;
            }
            //try rots
        }
        for(int i=1; i<=bestR; i++)
        {
            for(int j=0; j<len; j++)
            {
                if(s[j] == '0') s[j] = '9';
                else s[j]--;
            }
        }
        return Run(s, true, rots[bestR]);
    }

    sol1()
    {
        //Possible: D l Z/z E/e A S/s b T/t B P
        strcpy(conv, "DlZEASbTBP");
        strcpy(h_seq, "HHHHH?HHHHH?HHHHH?HHHH?HHHHH?HHHHH?HHHH?");
        strcpy(he_seq, "HHHHH?HHHHH?HHHHH?HHHH?");
    }
};

//SOL 2
int DP[20][MAXDP][30];
int C[20][MAXDP][30]; //0-small 1-large 2-idk
par from[20][MAXDP][30];
int rots[20][MAXDP][100];
int rots_tt[20][MAXDP];
bool del_table[MAXSZ + 5][3005]; //iterations
char ch_table[MAXDP + 5];

struct sol2
{
    // del table

    bool pp2test(int n)
    {
        // pow2
        int n1 = n;
        while(n1 % 2 == 0)
        {
            n1 /= 2;
        }
        if(n1 == 1) return true;

        // prime
        int lim = sqrt(n);
        for(int i=2; i<=lim; i++)
        {
            if(n % i == 0)
                return false;
        }
        return true;
    }

    int l[MAXSZ + 5];

    void GenLeft()
    {
       int zeros = 0;
       for(int i=1; i<=MAXSZ; i++)
       {
          if(!pp2test(i))
             zeros++;
          l[i] = zeros;
       }
    }

    int getZeros(int it, int k)
    {
        while(k--)
            it = l[it];
        return it;
    }

    int dplen;

    void Precompute(int len, int k, int SIDE)
    {
        // minimal length
        int hi = MAXSZ;
        int lo = 1;
        int pivot;
        int zs;
        while(hi > lo)
        {
            pivot = (hi + lo) / 2;
            zs = getZeros(pivot, k);
            if(zs >= len) hi = pivot;
            else lo = pivot + 1;
        }
        while(hi < MAXSZ && l[hi] == l[hi+1])
            hi++;
        // length = exactly hi
        dplen = hi;
        int zeros = 0;
        for(int i=1; i<=k; i++)
        {
             zeros = 0;
             //i-th iteration of deletion
             if(i%2 == SIDE)
             {
                 //front
                 for(int j=1; j<=hi; j++)
                 {
                     if(del_table[j][tz])
                        continue;
                     zeros++;
                     if(pp2test(zeros))
                        del_table[j][tz] = true;
                 }
             }
             else
             {
                 //back
                 for(int j=hi; j>=1; j--)
                 {
                    if(del_table[j][tz])
                        continue;
                     zeros++;
                    if(pp2test(zeros))
                       del_table[j][tz] = true;
                 }
             }
        }
    }

    // ^ del table

    int tt, tz;

    // vars

    struct solinfo
    {
        int score;
        int i;
        int rot;
        int m;
    };

    bool ban[150]; //mala
    char pban[4][2];
    char s[MAXLEN];
    int len;
    solinfo best;
    int cs;

    vector<int> out[10];
    int modLarge;

    char mapa[15];
    int new_rot;

    bool in[150];

    // ^vars

    inline void resetRots(int m, int i)
    {
        if(rots_tt[m][i] < tt)
        {
            rots_tt[m][i] = tt;
            rots[m][i][0] = 0;
        }
        return;
    }

    inline int d(int a, int b)
    {
        // a --> b
        return (b - a + 26) % 26;
    }

    inline char inrot(char ch, int rot, int cs)
    {
        char ret = ((ch - 'a') - rot + 26) % 26 + 'a';
        if(cs == 1) ret -= 32;
        return ret;
    }

    char h_seq[41], he_seq[24], hel_seq[7];
    inline void getH(int rotor) { copy(h_seq, h_seq+40, back_inserter(out[rotor])); }
    inline void getHE(int rotor) { copy(he_seq, he_seq+23, back_inserter(out[rotor])); }
    inline void getHEL(int rotor) { copy(hel_seq, hel_seq+6, back_inserter(out[rotor])); }

    inline void ROT1(bool bannedLetters, int rotor)
    {
        if(bannedLetters)
        {
            if(out[rotor][out[rotor].size()-1] == 'N')
                out[rotor].pop_back();
            else
                out[rotor].push_back('N');
            out[rotor].push_back('I');
            out[rotor].push_back('N');
        }
        else
        {
            out[rotor].push_back('I');
        }
    }

    void adjustShift(int shift, int newShift, int rotor)
    {
        // first
        if(d(shift, newShift) >= 13)
        {
            if(!out[rotor].empty() && out[rotor][out[rotor].size()-1] == 'N')
                out[rotor].pop_back();
            else
                out[rotor].push_back('N');
            shift = (shift + 13) % 26;
        }
        int dif = d(shift, newShift);
        for(int i=1; i<=dif; i++)
        {
            // check banned letters
            if(in[ 25 - shift + 97 ]) // only lower
                ROT1(true, rotor);
            else
                ROT1(false, rotor);
            shift++;
            if(shift == 26) shift = 0;
        }
    }

    // SOL2:

    void UpdateDP(int m, int a, int rota, int b, int rotb, int c, int cs)
    {
            //add rota -> rotb to the cost
            int dd = d(rota, rotb);
            if(dd >= 13) {c++; dd -= 13;}
            c += dd;

            resetRots(m, b);

            bool has = false;
            for(int i=1; i<=rots[m][b][0]; i++)
                if(rots[m][b][i] == rotb) has = true;

            if(!has)
            {
                DP[m][b][rotb] = DP[m][a][rota] + c;
                from[m][b][rotb] = par(a, rota);
                rots[m][b][ ++rots[m][b][0] ] = rotb;
                C[m][b][rotb] = cs;
            }
            else if(DP[m][a][rota] + c < DP[m][b][rotb])
            {
                DP[m][b][rotb] = DP[m][a][rota] + c;
                from[m][b][rotb] = par(a, rota);
                C[m][b][rotb] = cs;
            }
    }

    void tryH(int m, int i, int rot)
    {
            if(!del_table[i][tz])
            {
                // the new_rot is known!
                new_rot = d( tolower(ch_table[i]), 'h');
                if(isupper(ch_table[i])) cs = 1; else cs = 0;
                UpdateDP(m, i, rot, i+1, new_rot, 40, cs);
            }
            else
            {
                // the rot is unknown!
                new_rot = rot;
                while( ban[inrot('h', new_rot, 0)] )
                    new_rot = (new_rot-1+26)%26;
                UpdateDP(m, i, rot, i+1, new_rot, 40, 2);
            }
    }

    void tryHE(int m, int i, int rot)
    {
            if(!del_table[i][tz])
            {
                //the new_rot is known!
                new_rot = d( tolower(ch_table[i]), 'h');
                if(isupper(ch_table[i])) cs = 1; else cs = 0;

                if( !ban[inrot('e', new_rot, 0)]
                   && (del_table[i+1][tz] || ch_table[i+1] == inrot('e', new_rot, cs)) )
                    UpdateDP(m, i, rot, i+2, new_rot, 23, cs);
            }
            else if(del_table[i][tz] && !del_table[i+1][tz])
            {
                //the new_rot is known!
                new_rot = d( tolower(ch_table[i+1]), 'e');
                if(isupper(ch_table[i+1])) cs = 1; else cs = 0;

                if( !ban[inrot('h', new_rot, 0)] )
                    UpdateDP(m, i, rot, i+2, new_rot, 23, cs);
            }
            else
            {
                //the rot is unknown!
                new_rot = rot;
                while( ban[inrot('h', new_rot, 0)] || ban[inrot('e', new_rot, 0)] )
                    new_rot = (new_rot-1+26)%26;
                UpdateDP(m, i, rot, i+2, new_rot, 23, 2);
            }
    }

    void tryHEL(int m, int i, int rot)
    {
            if(!del_table[i][tz])
            {
                // the new_rot is known!
                new_rot = d( tolower(ch_table[i]), 'h');
                if(isupper(ch_table[i])) cs = 1; else cs = 0;

                if( !ban[inrot('e', new_rot, 0)] && !ban[inrot('l', new_rot, 0)]
                   && (del_table[i+1][tz] || ch_table[i+1] == inrot('e', new_rot, cs))
                   && (del_table[i+2][tz] || ch_table[i+2] == inrot('l', new_rot, cs)) )
                    UpdateDP(m, i, rot, i+3, new_rot, 6, cs);
            }
            else if(del_table[i][tz] && !del_table[i+1][tz])
            {
                //the new_rot is known!
                new_rot = d( tolower(ch_table[i+1]), 'e');
                if(isupper(ch_table[i+1])) cs = 1; else cs = 0;

                if( !ban[inrot('h', new_rot, 0)] && !ban[inrot('l', new_rot, 0)]
                   && (del_table[i+2][tz] || ch_table[i+2] == inrot('l', new_rot, cs)) )
                    UpdateDP(m, i, rot, i+3, new_rot, 6, cs);
            }
            else if(del_table[i][tz] && del_table[i+1][tz] && !del_table[i+2][tz])
            {
                //the new_rot is known!
                new_rot = d( tolower(ch_table[i+2]), 'l');
                if(isupper(ch_table[i+2])) cs = 1; else cs = 0;

                if( !ban[inrot('h', new_rot, 0)] && !ban[inrot('e', new_rot, 0)] )
                    UpdateDP(m, i, rot, i+3, new_rot, 6, cs);
            }
            else
            {
                // the rot is unknown!
                new_rot = rot;
                while( ban[inrot('h', new_rot, 0)] || ban[inrot('e', new_rot, 0)] || ban[inrot('l', new_rot, 0)])
                {
                    new_rot = (new_rot-1+26)%26;
                    if(new_rot == rot)
                    {
                        new_rot = -1;
                        break;
                    }
                }
                if(new_rot != -1)
                    UpdateDP(m, i, rot, i+3, new_rot, 6, 2);
            }
    }


    solinfo SolveDP(int m)//mask
    {
        int rot;

        rots[m][1][ ++rots[m][1][0] ] = 0;
        rots_tt[m][1] = tt;
        DP[m][1][0] = 0;
        from[m][1][0] = par(1, 0);
        C[m][1][0] = 2;

        for(int i=1; i<=dplen; i++)
        {
            resetRots(m, i);
            // expand each of these
            for(int j=1; j<=rots[m][i][0]; j++)
            {
                rot = rots[m][i][j];
                //expanding DP[m][i][rot]

                //add H
                if(i <= dplen) tryH(m, i, rot);
                //add HE
                if(i <= dplen - 1) tryHE(m, i, rot);
                //add HEL
                if(i <= dplen - 2) tryHEL(m, i, rot);
            }
        }
        solinfo bestScore;
        bestScore.score = INF;

        for(int i=dplen+1; i>=dplen+1; i--)
        {
            resetRots(m, i);
            for(int j=1; j<=rots[m][i][0]; j++)
            {
                rot = rots[m][i][j];
                if(DP[m][i][rot] < bestScore.score)
                {
                    bestScore.score = DP[m][i][rot];
                    bestScore.i = i;
                    bestScore.rot = rot;
                    bestScore.m = m;
                }
            }
            if(!del_table[i-1][tz])
                break;
        }
        return bestScore;
    }

    void AllIterations(int lvl, int mask)
    {
        // optimizing


        // the best map
        ban['h'] = false; ban['u'] = true;
        ban['i'] = false; ban['v'] = true;
        ban['j'] = true; ban['w'] = false;
        ban['k'] = true; ban['x'] = false;

        best = SolveDP(mask);
        return;
    }

    void GetOutput(int DELS, int SIDE, int rotor)
    {
        out[rotor].clear();
        memset(in, false, sizeof(in));

        int m = best.m;
        par rb(best.i, best.rot);
        stack<par> s;
        while(from[m][rb.fi][rb.se].fi != rb.fi)
        {
            s.push(rb);
            rb = from[m][rb.fi][rb.se];
        }

        par curr(1, 0);
        par nxt;
        modLarge = false;
        int cas;

        // stack -> out vector
        while(!s.empty())
        {
            nxt = s.top();
            s.pop();
            cas = C[m][nxt.fi][nxt.se];

            // fix case
            if(modLarge && cas == 0)
            {
                out[rotor].push_back('C');
                modLarge = false;
            }
            else if(!modLarge && cas == 1)
            {
                out[rotor].push_back('C');
                modLarge = true;
            }

            // fix rot
            adjustShift(curr.se, nxt.se, rotor);

            // generate letters and fix in
            if(nxt.fi - curr.fi == 1)
            {
                getH(rotor);
                in[inrot('h', nxt.se, 0)] = true;
            }
            else if(nxt.fi - curr.fi == 2)
            {
                getHE(rotor);
                in[inrot('h', nxt.se, 0)] = true;
                in[inrot('e', nxt.se, 0)] = true;
            }
            else if(nxt.fi - curr.fi == 3)
            {
                getHEL(rotor);
                in[inrot('h', nxt.se, 0)] = true;
                in[inrot('e', nxt.se, 0)] = true;
                in[inrot('l', nxt.se, 0)] = true;
            }
            else
                assert(0);
            curr = nxt;
        }
        adjustShift(curr.se, 0, rotor);
        if(modLarge)
            out[rotor].push_back('C');
        for(int i=1; i<=DELS; i++)
        {
            if(i%2 == SIDE)
                out[rotor].push_back('P');
            else
                out[rotor].push_back('O');
        }
        out[rotor].push_back('E');
        out[rotor].push_back('+');
        for(int i=1; i<=rotor; i++)
            out[rotor].push_back('N');
    }

    int Run(int DELS, int SIDE, int rotor)
    {
        best.score = INF;
        AllIterations(0, 0);
        if(best.score == INF)
        {
            return INF;
        }
        GetOutput(DELS, SIDE, rotor);
        return out[rotor].size();
    }

    bool Solve(char* n, int DELS, int SIDE, int ROTSTRY)
    {
        tz++;
        strcpy(s, n);
        len = strlen(s);
        Precompute(len, DELS, SIDE);
        int rots[10] ={0, 7, 4, 1, 8, 5, 2, 9, 6, 3};
        int bestR;
        int bestBS = -1;
        int currBS;
        for(int i=0; i<=9; i++)
        {
                tt++;
                int zeros = 0;
                for(int i=1; i<=dplen; i++)
                {
                    if(!del_table[i][tz])
                    {
                        ch_table[i] = mapa[ s[zeros] - '0' ];
                        zeros++;
                    }
                    else ch_table[i]='_';
                }

            if(ROTSTRY == 2)
            {
                if(i == 9 || i == 6 ) currBS = Run(DELS, SIDE, rots[i]);
                else currBS = INF;
            }
            else if(ROTSTRY == 3)
            {
                if(i == 9 || i == 6 || i == 3) currBS = Run(DELS, SIDE, rots[i]);
                else currBS = INF;
            }
            else if(ROTSTRY == 4)
            {
                if(i == 9 || i == 6 || i == 3 || i == 2) currBS = Run(DELS, SIDE, rots[i]);
                else currBS = INF;
            }
            else if(ROTSTRY == 5)
            {
                if(i == 9 || i == 6 || i == 3 || i == 1 || i == 4) currBS = Run(DELS, SIDE, rots[i]);
                else currBS = INF;
            }

            if(currBS < bestBS || bestBS == -1)
            {
                bestBS = currBS;
                bestR = i;
            }
            for(int j=0; j<len; j++)
            {
                if(s[j] == '0') s[j] = '9';
                else s[j]--;
            }
            //try roti
        }
        // rots[bestR] = output

        if(ALL || out[ rots[bestR ] ].size() <= MAXSZ) // testing
        {
            for(int i=0; i<out[ rots[bestR ] ].size(); i++)
            {
                printf("%c", out[ rots[bestR ] ][i]);
            }
            printf("\n");
            return true;
        }
        return false;
    }

    sol2()
    {
        tt = tz = 0;
        GenLeft();
        //Possible: D l Z/z E/e A S/s b T/t B P
        strcpy(mapa, "DlZEASbTBP");
        strcpy(h_seq, "HHHHH?HHHHH?HHHHH?HHHH?HHHHH?HHHHH?HHHH?");
        strcpy(he_seq, "HHHHH?HHHHH?HHHHH?HHHH?");
        strcpy(hel_seq, "HHHHH?");
        ban['a'] = false, ban['b'] = false, ban['p'] = false, ban['d'] = false;
        ban['e'] = false, ban['s'] = false, ban['t'] = false, ban['l'] = false, ban['z'] = false;
        ban['n'] = true, ban['o'] = true, ban['c'] = true, ban['q'] = true;
        ban['r'] = true, ban['f'] = true, ban['g'] = true, ban['y'] = true, ban['m'] = true;
        pban[0][0] = 'h', pban[0][1] = 'u', pban[1][0] = 'i', pban[1][1] = 'v';
        pban[2][0] = 'j', pban[2][1] = 'w', pban[3][0] = 'k', pban[3][1] = 'x';
    }

};

// the needed number and the number of test cases
char n[MAXLEN];
int t;

int main()
{
    // flags
    DEBUG = false;
    ALL = false;

    // print to file
    if(DEBUG)
    {
        freopen("input.txt","r",stdin);
        freopen("output1.txt","w",stdout);
    }
    // input
    scanf("%d", &t);
    sol1 Solver1; 
    sol2 Solver2; 
    for(int tc=1; tc<=t; tc++)
    {
        scanf("%s", n);
        int tl = strlen(n);
        // Decide which solver to used based on parameters
        if(tl <= 400)
        {
           if(Solver1.Solve(n))
                continue;
        }
        if(tl <= 710)
        {
            if(Solver2.Solve(n, 7, 1, 5))
                continue;
        }
        if(tl <= 710)
        {
            if(Solver2.Solve(n, 8, 1, 4))
                continue;
        }
        if(tl <= 710)
        {
            if(Solver2.Solve(n, 6, 1, 4))
                continue;
        }
        printf("\n"); //Can't be solved
    }
}
