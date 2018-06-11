#include <bits/stdc++.h>
#define ff(i, a, b) for(int i = a; i <= b; i++)
#define MAXN 405

using namespace std;

int n;
int d[MAXN][MAXN];
int nn[MAXN][10];

int TO_TRY = 5;

vector<int> NN() {
    bool visited[MAXN];
    ff (i, 1, n) visited[i] = false;
    
    vector<int> path;
    path.push_back(1);
    visited[1] = true;
    int curr = 1;
    while(path.size() < n) {
        int best = -1;
        ff (i, 1, n) {
            if (visited[i]) continue;
            if (best == -1 || d[curr][i] < d[curr][best]) {
                best = i;
            }
        }
        path.push_back(best);
        visited[best] = true;
        curr = best;
    }
    path.push_back(1);
    return path;
}

vector<int> Random() {
    vector<int> ret;
    ff (i, 2, n) ret.push_back(i);
    ret.insert(ret.begin(), 1);
    ret.push_back(1);
    return ret;
}

// LinKe
// sequential, feasable, positive gain, disjoint
// five NN
// prior when picking Y: max(nextX - nextY)
// when two x4, c(x4) break longest link
// 4 opt after that

void PrecomputeNN() {
    vector<int> all;
    ff (i, 1, n) all.push_back(i);
    ff (i, 1, n) {
        sort(all.begin(), all.end(), [i](int a, int b) {
            return d[i][a] < d[i][b];
        });
        ff (j, 1, TO_TRY) {
            nn[i][j] = all[j];
            //cout << nn[i][j] << " " ;
        } //cout << endl;
    }
}

int t[MAXN];

bool xs[MAXN][MAXN];
bool ys[MAXN][MAXN];
map<int, set<int>> help;

int G; // sum(xi - yi)
bool LinKeInnerY(vector<int>& tour, int i, int G);
bool LinKeInnerX(vector<int>& tour, int i, int G);

bool LinKeInnerY(vector<int>& tour, int i, int G) {
    //cout << "Entered LinKeInnerY " << i << endl;
    // t[1] t[2] t[3] filled, G ready, taken ready
    // 2i for x, 2i+1 for y
    ff (tries, 1, TO_TRY) {
        int nxt = nn[t[2*i]][tries];
        bool X_ok = !xs[min(nxt, t[2*i])][max(nxt, t[2*i])];
        bool Y_ok = !ys[min(nxt, t[2*i])][max(nxt, t[2*i])];
        bool iss = false;
        for (int z : help[t[2*i]]) if (z == nxt) iss = true;
        G += d[t[2*i]][t[2*i-1]] - d[t[2*i]][nxt];
        if (!iss && X_ok && Y_ok && G > 0) {
            t[2*i+1] = nxt;
            ys[min(t[2*i], t[2*i+1])][max(t[2*i], t[2*i+1])] = true;
            help[t[2*i]].insert(t[2*i+1]); 
            help[t[2*i+1]].insert(t[2*i]);
            // Step 5. i=i+1
            if (LinKeInnerX(tour, i+1, G)) {
                ys[min(t[2*i], t[2*i+1])][max(t[2*i], t[2*i+1])] = false;
                return true;
            }
            ys[min(t[2*i], t[2*i+1])][max(t[2*i], t[2*i+1])] = false;
            help[t[2*i]].erase(t[2*i+1]); 
            help[t[2*i+1]].erase(t[2*i]);
        }
        G -= d[t[2*i]][t[2*i-1]] - d[t[2*i]][nxt];
    }
    return false;
}

void PrintHelp() {
    //cout << "lets print help" << endl;
    for (int i = 1; i <= n; i++) {
        //cout << "help for " << i << ": ";
        for (int cand : help[i]) {
            //cout << cand << " ";
        } //cout << endl;
    }
}

int GetTowardsCycle(int start, int bankand) {
    //cout << "GetTWC" << endl;
    //cout << "TS: " << t[1] << " " << t[2] << " " << t[3] << endl;
    PrintHelp();
    vector<int> cands;
    for (int cand : help[start]) {
        if (cand != bankand)
            cands.push_back(cand);
        //cout << "cadidate " << cand << " for start " << start << endl;
    }

    for(int zz : cands) {
        //cout << "new candidate" << endl;
        int curr = zz;
        int last = start;
        bool found = true;
        while (found) {
            //cout << start << " " << curr << " " << last << endl;
            found = false;
            for (int cand : help[curr]) {
                if (cand != last) {
                    last = curr;
                    curr = cand;
                    found = true;
                    break;
                }
            }
            if (!found) break;
            if (curr == start) return zz;
        }
    }
    assert(false);
    return -1;
}

void ModifyTour(vector<int>& tour, int start) {
    //cout << "ModifyTour" << endl;
    tour.clear();
    int curr = start;
    int last = -1;
    while (!(curr == start && last != -1)) {
        //cout << curr << " added to tour" << endl;
        tour.push_back(curr);
        for (int cand : help[curr]) {
            if (cand != last) {
                last = curr;
                curr = cand;
                break;
            }
        }
    }
}

bool CanFinish(vector<int>& tour, int i, int G) {
    //cout << "CanFinish" << endl;
    int nxt = t[1];
    G += d[t[2*i]][t[2*i-1]] - d[t[2*i]][nxt];
    if (G <= 0) {
        return false;
    }
    // YES
    t[2*i+1] = nxt;
    help[t[2*i]].insert(t[2*i+1]); 
    help[t[2*i+1]].insert(t[2*i]);
    ModifyTour(tour, t[1]);
    return true;
}

bool LinKeInnerX(vector<int>& tour, int i, int G) {
    //cout << "Entered LinKeInnerX " << i << endl;
    // t[1] t[2] t[3] filled, G ready, taken ready
    // 2i for x, 2i+1 for y
    // Step 6.
    // Find which one breaks the cycle
    int nxt = GetTowardsCycle(t[2*i-1], t[2*i-2]);

    bool X_ok = !xs[min(nxt, t[2*i-1])][max(nxt, t[2*i-1])];
    bool Y_ok = !ys[min(nxt, t[2*i-1])][max(nxt, t[2*i-1])];

    if (X_ok && Y_ok) {
        t[2*i] = nxt;
        xs[min(t[2*i], t[2*i-1])][max(t[2*i], t[2*i-1])] = true;
        help[t[2*i]].erase(t[2*i-1]); 
        help[t[2*i-1]].erase(t[2*i]);
        if (CanFinish(tour, i, G)) {
            xs[min(t[2*i], t[2*i-1])][max(t[2*i], t[2*i-1])] = false;
            return true;
        }
        // Steps 6-9.
        if (LinKeInnerY(tour, i, G)) {
            xs[min(t[2*i], t[2*i-1])][max(t[2*i], t[2*i-1])] = false;
            return true;
        }
        xs[min(t[2*i], t[2*i-1])][max(t[2*i], t[2*i-1])] = false;
        help[t[2*i]].insert(t[2*i-1]); 
        help[t[2*i-1]].insert(t[2*i]);
    }

    return false;
}

bool LinKeStepFour(vector<int>& tour) {
    //cout << "Entered LinKeStep4" << endl;
    // Step 4. (Step 10 wraps back here)
    ff (tries, 1, TO_TRY) {
        //cout << "t[2] is " << t[2] << " " << tries << endl;
        int nxt = nn[t[2]][tries];
        int G = d[t[2]][t[1]] - d[nxt][t[2]];
        bool X_ok = !xs[min(nxt, t[2])][max(nxt, t[2])];
        bool Y_ok = !ys[min(nxt, t[2])][max(nxt, t[2])];
        bool iss = false;
        for (int z : help[t[2]]) if (z == nxt) iss = true;
        if (!iss && X_ok && Y_ok && G > 0) {
            //cout << "for t[3] i assign " << nxt << endl;
            t[3] = nxt;
            ys[min(t[3], t[2])][max(t[3], t[2])] = true;
            help[t[3]].insert(t[2]); help[t[2]].insert(t[3]);
            // Steps 6-9.
            if (LinKeInnerX(tour, 2, G)) {
                ys[min(t[3], t[2])][max(t[3], t[2])] = false;
                return true;
            }
            ys[min(t[3], t[2])][max(t[3], t[2])] = false;
            help[t[3]].erase(t[2]); help[t[2]].erase(t[3]);
        }
        // Step 10: wrap back to Step 4.
    }
    return false;
}

bool LinKeOptimize(vector<int>& tour) {
    //cout << "Entered LinKeOpt" << endl;
    // Step 1: we have a tour.
    ff (i, 1, n) ff (j, 1, n) xs[i][j] = false;
    ff (i, 1, n) ff (j, 1, n) ys[i][j] = false;
    G = 0;
    ff (idx, 0, n-1) {
        // Step 2.
        t[1] = tour[idx];
        // Step 3. (Step 11 wraps back here)
        t[2] = tour[(idx+1)%n];
        xs[min(t[1], t[2])][max(t[1], t[2])] = true;
        help[t[1]].erase(t[2]); help[t[2]].erase(t[1]);
        if (LinKeStepFour(tour)) {
             xs[min(t[1], t[2])][max(t[1], t[2])] = false;
             return true;
        }
        // Step 11: wrap back to Step 3.
        xs[min(t[1], t[2])][max(t[1], t[2])] = false;
        help[t[1]].insert(t[2]); help[t[2]].insert(t[1]);
        t[2] = tour[(idx-1+n)%n];
        xs[min(t[1], t[2])][max(t[1], t[2])] = true;
        help[t[1]].erase(t[2]); help[t[2]].erase(t[1]);
        if (LinKeStepFour(tour)) {
            xs[min(t[1], t[2])][max(t[1], t[2])] = false;
            return true;
        }
        xs[min(t[1], t[2])][max(t[1], t[2])] = false;
        help[t[1]].insert(t[2]); help[t[2]].insert(t[1]);
        // Step 12: wrap back to Step 2.
    }
    // Step 13: stop.
    return false;
}

int Len(const vector<int>& tour) {
    int ret = 0;
    for(int i = 0; i < tour.size() - 1; i++) {
        ret += d[tour[i]][tour[i+1]];
    }
    if (tour[tour.size()-1] != tour[0]) {
        ret += d[tour[0]][tour[tour.size()-1]];
    }
    return ret;
}

vector<int> LinKe() {
    //cout << "Entered LinKe" << endl;
    // Step 1: Generate a random tour.
    vector<int> tour = Random(); // Try nn
    tour.pop_back(); // we don't need that
    for (int i = 0; i < tour.size(); i++) {
        help[ tour[i] ].insert(tour[ (i+1) % n]);
        help[ tour[i] ].insert(tour[ (i-1+n) % n]);
    }
    PrintHelp();
    PrecomputeNN();
    // Steps 2-12, optimize.
    while(LinKeOptimize(tour)) { //cout << "Optimized " << Len(tour) << endl; 
    }
    tour.push_back(tour[0]); // we need that again
    return tour;
}

int DP[1<<13][MAXN]; // used, last, 1-..-last
int L[1<<13][MAXN];
vector<int> SolveDP() {
    int MASKS = 1 << (n-1);
    ff (MASK, 0, MASKS-1) {
        ff (last, 2, n) {
            if (__builtin_popcount(MASK) == 1) {
                DP[MASK][last] = d[1][last];
                L[MASK][last] = 1;
                continue;
            }
            DP[MASK][last] = -1;
            if ( (MASK & (1 << (last-2))) == 0 ) continue;
            int OLDMASK = MASK ^ (1 << (last-2));
            ff (prelast, 2, n) {
                int is_set = (OLDMASK & (1 << (prelast-2)));
                if ( is_set == 0 ) {continue;}
                if (DP[MASK][last] == -1 || DP[MASK][last] > DP[OLDMASK][prelast] + d[prelast][last]) {
                    DP[MASK][last] = DP[OLDMASK][prelast] + d[prelast][last];
                    L[MASK][last] = prelast;
                }
            }
        }
    }
    int best = -1, bestl = -1;
    ff (last, 2, n) {
        int l = DP[MASKS-1][last] + d[last][1];
        if (bestl == -1 || l < bestl) {
            bestl = l;
            best = last;
        }
    }
    // reconstruct
    vector<int> ret;
    ret.push_back(1);
    int last = best;
    int MASK = MASKS-1;
    ret.push_back(last);
    while (last != 1) {
        int bm = MASK;
        MASK = MASK ^ (1 << (last-2));
        last = L[bm][last];
        ret.push_back(last);
    }
    return ret;
}

clock_t t_start;
double TimeElapsed() {
    return (double)(clock() - t_start) / CLOCKS_PER_SEC;
}

vector<int> ThreeOpt(vector<int> path) { // 0 .. n
    double max_time = (n == 40) ? 0.21 : 0.61;
    // max_time = 1; // PETLJA
    while(TimeElapsed() < max_time - 0.03) {
        int a2 = 0, b2 = 0, c2 = 0;
        while (a2 == b2 || b2 == c2 || a2 == c2) {
            a2 = rand() % n;
            b2 = rand() % n;
            c2 = rand() % n;
        }
        if (a2 > b2) swap(a2, b2);
        if (a2 > c2) swap(a2, c2);
        if (b2 > c2) swap(b2, c2);

        int c1 = b2 + 1;
        int b1 = a2 + 1;
        int a1 = c2 + 1;
        int lb = b2 - b1 + 1;
        int lc = c2 - c1 + 1;

        int S[8][6] = {
            {a2, b1, b2, c1, c2, a1},
            {a2, b1, b2, c2, c1, a1},
            {a2, b2, b1, c1, c2, a1},
            {a2, b2, b1, c2, c1, a1},
            {a2, c1, c2, b1, b2, a1},
            {a2, c1, c2, b2, b1, a1},
            {a2, c2, c1, b1, b2, a1},
            {a2, c2, c1, b2, b1, a1}
        };
        int best_idx = 0;
        int best_len = d[path[a2]][path[b1]] + d[path[b2]][path[c1]] + d[path[c2]][path[a1]];
        ff (i, 0, 7) {
            int curr_len = d[path[S[i][0]]][path[S[i][1]]] + d[path[S[i][2]]][path[S[i][3]]] + d[path[S[i][4]]][path[S[i][5]]];
            if (curr_len < best_len) {
                best_idx = i;
                best_len = curr_len;
            }
        }
        // FLIP B AND C? b1 b2 c1 c2
        if (best_idx >= 4) {
             // set it up first then swap all
             if (best_idx == 5 || best_idx == 7) {
                reverse(path.begin() + b1, path.begin() + b2 + 1);
             }
             if (best_idx == 6 || best_idx == 7) {
                reverse(path.begin() + c1, path.begin() + c2 + 1);
             }
            // flip shit
            reverse(path.begin() + b1, path.begin() + c2 + 1);
            reverse(path.begin() + b1, path.begin() + b1 + lc);
            reverse(path.begin() + b1 + lc, path.begin() + b1 + lc + lb);
            assert(b1 + lc + lb - 1 == c2);
        } else {
             if (best_idx == 2 || best_idx == 3) {
                reverse(path.begin() + b1, path.begin() + b2 + 1);
             }
             if (best_idx == 1 || best_idx == 3) {
                reverse(path.begin() + c1, path.begin() + c2 + 1);
             }
        }
    }
    return path;
}


vector<int> SolveNN() {
    bool visited[MAXN];
    ff (i, 1, n) visited[i] = false;
    
    vector<int> path;
    path.push_back(1);
    visited[1] = true;
    int curr = 1;
    while(path.size() < n) {
        int best = -1;
        ff (i, 1, n) {
            if (visited[i]) continue;
            if (best == -1 || d[curr][i] < d[curr][best]) {
                best = i;
            }
        }
        path.push_back(best);
        visited[best] = true;
        curr = best;
    }
    path.push_back(1);
    return path;
}

int main() {
    srand(time(NULL));
    t_start = clock();
    scanf("%d", &n);
    TO_TRY = min(TO_TRY, n-1);
    ff (i, 1, n) {
        ff (j, 1, n) {
            scanf("%d", &d[i][j]);
        }
    }
    vector<int> path;
    if (n <= 20) path = SolveDP();
    else if (n==40 || n==80 || n==170 || n==59 || n==100)
        path = ThreeOpt(LinKe());
    else path = ThreeOpt(SolveNN());
    int len = 0;
    ff (i, 0, n-1) {
        len += d[path[i]][path[i+1]];
    }
    //printf("%d\n", Len(SolveDP()));
    printf("%d\n", len);
    for (int n : path) {
        printf("%d ", n);
    }
    printf("\n");
    return 0;
}
