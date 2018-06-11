#include <bits/stdc++.h>

#define MAXN 60
#define SHELF_TYPES 8
#define ROTATIONS 4


using namespace std;

// input
int n, m;
char roomInput[MAXN][MAXN];

// other
int UID; // next shelf ID, from 1
int GLOBAL_LOCAL_ID = 1;
int roomIDs[MAXN][MAXN];
bool roomVis[MAXN][MAXN];
double density;

// bfs reusable
int dx[4] = {0, 0, -1, 1};
int dy[4] = {1, -1, 0, 0};
bool bfsVis[MAXN][MAXN];
int dist[MAXN][MAXN]; // zmajevo jaje
int max_dist = -1;

// neis
int MAX_NB_FREE[SHELF_TYPES] = {4, 10, 8, 8, 9, 9, 8, 8};

void loadInput() {
    scanf("%d %d", &n, &m);
    for (int i = 0; i < n; i ++) {
        scanf("%s", roomInput[i]);
    }
}

void initRoom() {
    int num_obstacles = 0;
    roomIDs[0][0] = 0;
    roomVis[0][0] = true;

    for (int i = 0; i < n; i ++) {
        for (int j = 0; j < m; j ++) {
            if (i == 0 && j == 0) continue;
            if (roomInput[i][j] == 'X') {
                num_obstacles++;
                roomIDs[i][j] = -1;
                roomVis[i][j] = true;
            } else {
                roomIDs[i][j] = -1;
                roomVis[i][j] = false;
            }
        }
    }

    density = (double)num_obstacles / (m*n);
}

void print(char mat[MAXN][MAXN]) {
    for (int i = 0; i < n; i ++) {
        for(int j = 0; j < m; j ++) {
            printf("%c", mat[i][j]);
        }
        printf("\n");
    }
}

void print(int mat[MAXN][MAXN]) {
    for (int i = 0; i < n; i ++) {
        for(int j = 0; j < m; j ++) {
            printf("%d ", mat[i][j]);
        }
        printf("\n");
    }
}

struct coords {
    int x, y;
    coords(int xx, int yy) : x(xx), y(yy) {}
    bool valid() {
        return x >= 0 && y >= 0 && x < n && y < m;
    }
};

// kancer
inline bool operator==(const coords& a, const coords& b) {
    return a.x == b.x && a.y == b.y;
}
struct coords_hash {
    inline std::size_t operator()(const coords& c) const {
        return c.x * 100 + c.y;
    }
};

void print(vector<coords> shelf) {
    for (coords c : shelf) {
        printf("(%d %d)\t", c.x, c.y);
    }
    printf("\n");
}

// provides mapping of shelf type and rotation to coords
// of places it occupies relative to shelf location
vector<coords> shelfMapping[SHELF_TYPES][ROTATIONS];

vector<coords> rotateShelf(vector<coords> shelf) {
    vector<coords> newShelf;
    for (coords c : shelf) {
        newShelf.push_back( coords(c.y, -c.x) );
    }
    return newShelf;
}

void initShelfMapping() {
    shelfMapping[0][0] = { coords(0, 0) };
    shelfMapping[1][0] = { coords(0, 0), coords(1, 0), coords(2, 0), coords(3, 0) }; // linija
    shelfMapping[2][0] = { coords(0, 0), coords(1, 0), coords(0, 1), coords(1, 1) }; // kocka
    shelfMapping[3][0] = { coords(0, 0), coords(1, 0), coords(2, 0), coords(1, 1) }; // kita
    shelfMapping[4][0] = { coords(0, 0), coords(1, 0), coords(2, 0), coords(0, 1) }; // kuka na desno
    shelfMapping[5][0] = { coords(0, 0), coords(0, 1), coords(1, 1), coords(2, 1) }; // kuka na levo
    shelfMapping[6][0] = { coords(0, 0), coords(1, 0), coords(1, 1), coords(2, 1) }; // z na gore
    shelfMapping[7][0] = { coords(0, 0), coords(0, 1), coords(1, 1), coords(1, 2) }; // obicno z

    for (int i = 0; i < SHELF_TYPES; i ++) {
        for (int j = 1; j < ROTATIONS; j ++) {
            shelfMapping[i][j] = rotateShelf(shelfMapping[i][j - 1]);
        }
    }
}

void BFS() {
    max_dist = -1;
    queue<coords> q;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            bfsVis[i][j] = false;
        }
    }
    q.push({0, 0});
    dist[0][0] = 0;
    while (!q.empty()) {
        coords curr = q.front();
        q.pop();
        for (int i = 0; i < 4; i++) {
            coords nxt(curr.x + dx[i], curr.y + dy[i]);
            if (nxt.valid() && !bfsVis[nxt.x][nxt.y] && !roomVis[nxt.x][nxt.y]) {
                bfsVis[nxt.x][nxt.y] = true;
                dist[nxt.x][nxt.y] = dist[curr.x][curr.y] + 1;
                max_dist = max(max_dist, dist[nxt.x][nxt.y]);
                q.push(nxt);
            }
        }
    }
}

struct shelf {
    int local_id;
    int id;
    int x, y;
    int type;
    int rotation;
    vector<coords> fields;
    unordered_set<coords, coords_hash> neis;

    double score;
    
    shelf(int id, int x, int y, int type, int rotation) {
        this->local_id = GLOBAL_LOCAL_ID++;
        this->id = id;
        this->x = x;
        this->y = y;
        this->type = type;
        this->rotation = rotation;

        populateFields();
    }

    void populateFields() {
        fields.clear();
        for (coords c : shelfMapping[type][rotation]) {
            fields.emplace_back( coords(c.x + x, c.y + y) );
        }
    }

    // Lazy
    void populateNeighbours() {
        // Maybe do this better?
        neis.clear();
        for (coords f : fields) {
            for (int i = 0; i < 4; i++) {
                coords c({f.x+dx[i], f.y+dy[i]});
                if (c.valid()) {
                    neis.insert(c);
                }
            }
        }
        for (coords f : fields) {
            neis.erase({f.x, f.y});
        }
    }

    void print() const {
        printf("Shelf: %d\tType: %d\tRot: %d\tFields:\t", id, type, rotation);
        for (coords c : fields) {
            printf("(%d, %d)\t", c.x, c.y);
        }
        printf("\n");
    }

    void printFancy() {
        string type_names[8] = {
            "mali",
            "linija",
            "kocka",
            "kita",
            "kuka na desno",
            "kuka na levo",
            "z na gore",
            "obicno z"
        };
        string rot_names[4] = {
            "bez rot",
            "kazaljka",
            "180",
            "suprotno"
        };
        printf("Shelf ID: %3d |Type: %14s(%d) |Rot: %9s(%d) |Score: %0.2f |Fields: ", id, type_names[type].c_str(), type,
            rot_names[rotation].c_str(), rotation, score);
        for (coords c : fields) {
            printf("(%d, %d) ", c.x, c.y);
        }
        printf("\n");
    }

    void printOutput() {
        printf("%d %d %d %d\n", x+1, y+1, type, rotation);
    }

    bool allReachable() const {
        // Fake place
        place(); 
        // Currently placed ids [1...UID]

        unordered_set<int> reachable_ids;
        queue<coords> q;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                bfsVis[i][j] = false;
            }
        }
        q.push({0, 0});
        bfsVis[0][0] = true;
        dist[0][0] = 0;
        max_dist = -1;
        while (!q.empty()) {
            coords curr = q.front();
            q.pop();
            for (int i = 0; i < 4; i++) {
                coords nxt(curr.x + dx[i], curr.y + dy[i]);
                if (nxt.valid() && !bfsVis[nxt.x][nxt.y]) {
                    bfsVis[nxt.x][nxt.y] = true;
                    dist[nxt.x][nxt.y] = dist[curr.x][curr.y] + 1;
                    max_dist = max(max_dist, dist[nxt.x][nxt.y]);
                    if (!roomVis[nxt.x][nxt.y]) {
                        // Empty, continue bfs
                        q.push(nxt);
                    } else {
                        // Filled, add reachable id
                        if (roomIDs[nxt.x][nxt.y] != -1) {
                            reachable_ids.insert(roomIDs[nxt.x][nxt.y]);
                        }
                    }
                }
            }

        }

        // Fake place unplaced
        unplace();
        bool ok = (reachable_ids.size() == UID);
        return ok;
    }

    bool placeable() const {
        for (coords c : fields) {
            if (!c.valid()) {
                return false;
            }
            if (roomVis[c.x][c.y]) {
                return false;
            }
        }
        return true;
    }

    bool place() const {
        if (!placeable()) {
            printf("Error: cannot place shelf!\n");
            print();
            return false;
        }
        for (coords c : fields) {
            roomVis[c.x][c.y] = true;
            roomIDs[c.x][c.y] = UID;
        }
        UID++;
        return true;
    }

    bool unplace() const {
        UID--;
        for (coords c : fields) {
            roomVis[c.x][c.y] = false;
            roomIDs[c.x][c.y] = -1;
        }
        return true;
    }

    // Scoring fns: [0, 1]

    double scoreDownRight() {
        double dr = 0;
        for (coords field : fields) {
            dr += ((double)field.x/n + (double)field.y/m) / 2;
        }
        return dr / (int)fields.size();
    }

    double scoreFreeAround() {
        // Find neighbours
        int nb_free = 0;
        if (neis.empty()) populateNeighbours();
        for (coords nei : neis) {
            if (!roomVis[nei.x][nei.y]) nb_free++;
        }
        double pc_taken = 1 - (double)nb_free / MAX_NB_FREE[type];
        return pc_taken;
    }

    double scoreType() {
        if (type == 1) return 1;
        return 0;
    }

    double scoreTunnels() {
        if (neis.empty()) populateNeighbours();
        int total = 0;
        int good = 0;
        for (coords nei : neis) {
              if (!roomVis[nei.x][nei.y])
            if (nei.x-1 < 0 || nei.y-1 < 0 || roomVis[nei.x-1][nei.y-1])
              if (nei.y-1 < 0 || roomVis[nei.x][nei.y-1])
                if (nei.x+1 >= n || nei.y-1 < 0 || roomVis[nei.x+1][nei.y-1])
            if (nei.x-1 < 0 || nei.y+1 >= m || roomVis[nei.x-1][nei.y+1])
              if (nei.y+1 >= m || roomVis[nei.x][nei.y+1])
                if (nei.x+1 >= n || nei.y+1 >= m|| roomVis[nei.x+1][nei.y+1])
            if (nei.x-1 < 0 || !roomVis[nei.x-1][nei.y])
              if (nei.x+1 >= n || !roomVis[nei.x+1][nei.y])
               good++;
        }
        return (double)good/total;
    }

    double scoreDistFromOrigin() {
        double ds = 0;
        for (coords field : fields) {
            ds += dist[field.x][field.y] / (double)max_dist;
        }
        double sc = ds / (int)fields.size();
        return sc;
    }

    double scoreRight() {
        // hmm
        double dr = 0;
        for (coords field : fields) {
            dr += (double)field.x/n;
        }
        return dr / (int)fields.size();
    }

    // All scoring fns with weights

    // Done with scoring fns, don't change calcScore
    double calcScore() {
        double ret = 0;
        //ret += 1      * scoreDownRight();
            
        ret += 1      * scoreDistFromOrigin();
        ret += 2.55   * scoreFreeAround();

        // ret += 100  * scoreType();
        if (density < 0.3)
            ret += 0.25 * scoreType();
        return ret;
    }
};

inline bool operator< (const shelf& a, const shelf& b){ 
    if (a.score == b.score) a.local_id < b.local_id;
    return a.score > b.score;
    // unstable?
}

//#include "seedlings.h"

///////////////////////////// first.cpp
#define DBG 0

const double TIME_LIMIT = 5.0;
const double BACKOFF_DELTA = -1000;
int MAGICNA_PATKA = 65;

clock_t t_start;
double TimeElapsed() {
    return (double)(clock() - t_start) / CLOCKS_PER_SEC;
}

vector<shelf> out;

void resetTp() {
    UID = 1;
    out.clear();
}

multiset<shelf> fastPruneAndScore(const multiset <shelf>& cands) {
    if(DBG) cout << "Fast prune and score" << endl;
    // Prune by placeability and score
    multiset<shelf> new_cands;
    for (shelf s : cands) {
        if (s.placeable()) {
            s.score = s.calcScore();
            new_cands.insert(s);
        }
    }
    return new_cands;
}

int slowPruneAndPlace(multiset<shelf>& cands, int max_to_place) {
    int nb_placed = 0;
    auto it = cands.begin();
    while (it != cands.end()) {
        const shelf& s = (*it);
        // We still need to check placeable again, but slow prune now
        if (s.placeable() && s.allReachable()) { 
            // Place and done, id is important here
            s.place();
            out.push_back(s);
            nb_placed++;
        }
        ++it;
        if (nb_placed == max_to_place) break;
    }
    cands.erase(cands.begin(), it);
    return nb_placed;
}
// candidate_type: (type, rotation)
bool goPlace(multiset<shelf>& cands) {
    cands = fastPruneAndScore(cands);

    if(DBG) {
        cout << "print fancy"<< endl;
        int IT = 0;
        for (shelf s : cands) {
            IT++;
            s.printFancy();    
            if (IT == 20) break;
        }
    }

    // sort(cands.begin(), cands.end());
    int nb_placed = slowPruneAndPlace(cands, MAGICNA_PATKA);
    
    if (nb_placed > 0) return true;
    else return false;
}

void solve() {
    // Build a vector of all types except the small one
    multiset<shelf> cands;
    int local_id = 1;
    for (int x = 0; x < n; x++) {
        for (int y = 0; y < m; y++) {
            for (int i = 1; i < SHELF_TYPES; i++) {
                for (int j = 0; j < ROTATIONS; j++) {
                    if (i == 2 && j > 0) continue;
                    if (i == 1 && j > 1) continue;
                    cands.insert({-2, x, y, i, j});
                }
            }
        }
    }
    if (DBG) cout << "We have " << cands.size() << " candidates." <<endl;

    // Place them while you can
    if (DBG) cout << "Placing biggies" << endl;
    while (goPlace(cands)) {
        if(DBG) cout << "We have " << cands.size() << " candidates." <<endl;
        if (TimeElapsed() > TIME_LIMIT - BACKOFF_DELTA) return;
    }

    // Leave only the small ones
    cands.clear();
    for (int x = 0; x < n; x++) {
        for (int y = 0; y < m; y++) {
            cands.insert({-2, x, y, 0, 0});
        }
    }

    //MAGICNA_PATKA = 10000000;

    // Place them while you can
    if(DBG) cout << "Placing smalls" << endl;
    while (goPlace(cands)) {
        if (TimeElapsed() > TIME_LIMIT - BACKOFF_DELTA) return;
    }
}

void solveTrivial() {
    if (!roomVis[0][1]) {
        out.push_back({1, 0, 1, 0, 0});
    }
    if (!roomVis[1][0]) {
        out.push_back({1, 1, 0, 0, 0});
    }
}

void printOutput() {
    if (out.empty()) {
        if(DBG) cerr << "Time elapsed " << TimeElapsed() << endl;
        assert(false);
    }
    int total_score = 0;
    for (shelf s : out) {
        if (s.type == 0) total_score++;
        else total_score += 6;
    }
    printf("%d %d\n", (int)out.size(), total_score);
    for (shelf s : out) {
        s.printOutput();
    }
}

int main() {
    //freopen("examples/01.in", "r", stdin);
    //freopen("examples/01.out", "w", stdout);
    srand(time(NULL));
    t_start = clock();
    initShelfMapping();
    int t;
    scanf("%d", &t);
    while (t--) {
        resetTp();
        loadInput();
        
        if (n >= 45) MAGICNA_PATKA = 68;
        else if (n >= 44) MAGICNA_PATKA = 25;
        else if (n >= 31) MAGICNA_PATKA = 19;
        else if (n >= 29) MAGICNA_PATKA = 7;
        else MAGICNA_PATKA = 1;

        initRoom();
        BFS();

        if((TimeElapsed() > TIME_LIMIT - BACKOFF_DELTA)) {
            solveTrivial();
        } else {
            solve();
        }
        printOutput();
    }
    cerr << "Time elapsed " << TimeElapsed() << endl;
    return 0;
}
