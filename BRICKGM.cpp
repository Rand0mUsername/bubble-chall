//RandomUsername (Nikola Jovanovic)
//.deathSatanBunny
//Bubblecup V7
//BRICKGM

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

// debug
bool DBG;

// vars

int n,k,Skor;
char brick[5];
int a[22][11],b[22][11],dp[22][11][4];
int rowPlace,rotPlace,gameOver;
int gameScore,currSc,bestScore,bestIt;

int twos,threes,more;
int topRow,col,previ,prevj,marker;
int linesCleared,sumSq,iteration;

int A,B,C,D,E;
int enrageConst;
int heightPTS,twoPTS,threePTS,morePTS,gamePTS,PTS;

int tildaScoring(int brickIdx,int topI,int topJ,int b[22][11]);

bool justGame;
int rottBack;

// atomic functions

// are coords valid?
bool good(int i,int j) 
{
    if(i<1) return false;
    if(i>20) return false;
    if(j<1) return false;
    if(j>10) return false;
    return true;
}

// Finding the coords of the previous spot when moving in dirr direction
int previi(int i,int dirr) {
    if(dirr==0) // hor
        return i;
    if(dirr==1) // ver
        return i-1;
    if(dirr==2) // main dia
        return i-1;
    if(dirr==3) // other dia
        return i-1;
}

// Finding the coords of the previous spot when moving in dirr direction
int prevjj(int j,int dirr) 
{
    if(dirr==0) // hor
        return j-1;
    if(dirr==1) // ver
        return j;
    if(dirr==2) // main dia
        return j-1;
    if(dirr==3) // other dia
        return j+1;
}

void rotBrick() // Rotating the brick once
{
  brick[3]=brick[2];
  brick[2]=brick[1];
  brick[1]=brick[0];
  brick[0]=brick[3];
}

void cpy(int a[22][11],int b[22][11]) // copying the game state
{
    for(int i=0;i<=20;i++)
         for(int j=1;j<=10;j++)
          b[i][j]=a[i][j];
}

void print() // printing the game state for debug
{
 for(int i=0;i<=20;i++)
 {
     for(int j=1;j<=10;j++)
          cout<<a[i][j]<<" ";
     cout<<endl;
 }
}

int charToInt(char* x) // conversion
{
    int ret=0;
    int len=strlen(x);
    int neg=0;
    if(x[0]=='-')
        neg=1;
    for(int i=neg;i<len;i++)
        {
            ret*=10;
            ret+=x[i]-'0';
        }
    if(neg) return -ret;
    return ret;
}

// Calculating the number of 2s and 3s
void calc2s3s(int brickIdx,int b[22][11]) 
{
    // Reset dp
    for(int i=1;i<=20;i++)
        for(int j=1;j<=10;j++)
         for(int dir=0;dir<=3;dir++)
          dp[i][j][dir]=0;

    twos=0;
    threes=0;
    more=0;

    // Calc stuff
    for(int dir=0;dir<=3;dir++)
        for(int i=1;i<=20;i++)
            for(int j=1;j<=10;j++)
            {
                previ=previi(i,dir);
                prevj=prevjj(j,dir);
                if(previ==0 || prevj==0)
                    dp[i][j][dir]=1;
                else if(b[i][j]!=0 && b[i][j]==b[previ][prevj])
                    dp[i][j][dir]=dp[previ][prevj][dir]+1;
                else
                    dp[i][j][dir]=1;

                if(dp[i][j][dir]==2) twos++;
                if(dp[i][j][dir]==3) {threes++; twos--;}
                else if(dp[i][j][dir]==4) {more++; threes--;}
            }
}

int placeLeft;

// Scoring and collapsing once
int scoreAndCollapseOnce(int b[22][11]) 
{
    linesCleared=0;
    sumSq=0;

    // Reset dp

    for(int i=1;i<=20;i++)
        for(int j=1;j<=10;j++)
            for(int dir=0;dir<=3;dir++)
                dp[i][j][dir]=0;

    // Calc dp

    for(int dir=0;dir<=3;dir++)
        for(int i=1;i<=20;i++)
            for(int j=1;j<=10;j++)
            {
                previ=previi(i,dir);
                prevj=prevjj(j,dir);
                if(previ==0 || prevj==0)
                    dp[i][j][dir]=1;
                else if(b[i][j]!=0 && b[i][j]==b[previ][prevj])
                    dp[i][j][dir]=dp[previ][prevj][dir]+1;
                else
                    dp[i][j][dir]=1;
            }

    // Mark for deletion and compute the score

    for(int i=1;i<=20;i++)
        for(int j=1;j<=10;j++)
          for(int dir=0;dir<=3;dir++)
          {
              if(dp[i][j][dir]==3)
              {
                  sumSq+=9;
                  linesCleared++;

                  previ=previi(i,dir);
                  prevj=prevjj(j,dir);
                  b[previ][prevj]=-1;

                  previ=previi(previ,dir);
                  prevj=prevjj(prevj,dir);
                  b[previ][prevj]=-1;

                  b[i][j]=-1;
              }
              else if(dp[i][j][dir]>3)
              {
                      sumSq+=dp[i][j][dir]*dp[i][j][dir];
                      sumSq-=(dp[i][j][dir]-1)*(dp[i][j][dir]-1);
                      b[i][j]=-1;
              }
           }

    // Collapse
    placeLeft=0;
    for(int j=1;j<=10;j++)
    {
        marker=20;
        for(int i=20;i>=1;i--)
        {
            if(b[i][j]!=-1 && b[i][j]!=0)
                b[marker--][j]=b[i][j];
        }
        b[0][j]=20-marker;
        placeLeft+=marker/3;
        while(marker>=1)
            b[marker--][j]=0;
    }
    return sumSq*linesCleared*iteration; // Game score
}

// Full scoring&collapsing process
int fullScoreAndCollapse(int b[22][11]) 
{
    gameScore=0;
    iteration=1;
    currSc=1;
    while(currSc>0) // While you can break things, break them
    {
        currSc=scoreAndCollapseOnce(b);
        gameScore+=currSc;
        iteration++;
    }
    return gameScore;
}

// Updating the state of the game
void updateState(int a[22][11],int it) 
{
    // set rowPlace and rotPlace + draw that brick
    rowPlace=it;
    rotPlace=0;
    rottBack=0;
    while(rowPlace>10)
    {
            rowPlace-=10;
            rotPlace++;
            rotBrick();
            rottBack++;
    }
    topRow=20-a[0][rowPlace]-2;
    a[topRow][rowPlace]=brick[0]-'0';
    a[topRow+1][rowPlace]=brick[1]-'0';
    a[topRow+2][rowPlace]=brick[2]-'0';
    a[0][rowPlace]+=3;
    gameScore=fullScoreAndCollapse(a); // now just score and collapse it
    rottBack=3-rottBack;
    while(rottBack--)
     rotBrick();
}

// Trying all 30 possible moves
void tryThirty(int a[22][11],int brickIdx) 
{
    for(int it=1;it<=30;it++)
    {
        if(!justGame && (k==4 || k==5 || k==6) && (it==5 || it==15 || it==25) ) continue;
        for(int i=0;i<=20;i++)
            for(int j=1;j<=10;j++)
                b[i][j]=a[i][j];
        col=it%10;
        if( col==0 )
             col=10;
        topRow=20-a[0][col]-2;
        if(topRow<1)
        {
                if(it%10==0)
                  rotBrick();
                continue;
        }
        b[topRow][col]=brick[0]-'0';
        b[topRow+1][col]=brick[1]-'0';
        b[topRow+2][col]=brick[2]-'0';

        currSc=tildaScoring(brickIdx,topRow,col,b); // Grading
        if(currSc>bestScore || bestIt==-1)
        {

            bestIt=it;
            bestScore=currSc;
        }
        if(it%10==0)
         rotBrick();
    }
}

// Placing the current brick
void brickPlacement(int brickIdx) 
{

  if(k==3 && (n-brickIdx<=2 || placeLeft<1))
     justGame=true;
  else if(k==4 && (n-brickIdx<=10 || placeLeft<10))
     justGame=true;
  else if(k==5 && (n-brickIdx<=10 || placeLeft<15))
     justGame=true;
  else if(k==6 && (n-brickIdx<=10 || placeLeft<25))
     justGame=true;
  bestScore=-1,bestIt=-1;
  tryThirty(a,brickIdx); // Try all 30 moves
  if(bestIt==-1)
    {
        gameOver=true;
        return;
    }
  cpy(a,b);
  updateState(a,bestIt); // Play the best move, game score gets filled
  if(k==3 && gameScore>enrageConst)
  {
      // do it all again
      cpy(b,a);
      bestScore=-1,bestIt=-1;
      justGame=true;
      tryThirty(a,brickIdx); // Try all 30 moves
      updateState(a,bestIt); // Play the best move, game score gets filled
  }
  justGame=false;
}

// Scoring functions

int calcCloseScore(int I,int J,int b[22][11])
{
    int ret=0;
    int i,j;

    // let's go
    i=I-1,j=J-1;
    if(good(i,j))
    {
        if(b[i][j]==b[I][J]) ret++;
    }

    i=I,j=J-1;
    if(good(i,j))
    {
        if(b[i][j]==b[I][J]) ret++;
        if(b[i][j]==b[I+1][J]) ret++;
    }

    i=I+1,j=J-1;
    if(good(i,j))
    {
        if(b[i][j]==b[I][J]) ret++;
        if(b[i][j]==b[I+1][J]) ret++;
        if(b[i][j]==b[I+2][J]) ret++;
    }

    i=I+2,j=J-1;
    if(good(i,j))
    {
        if(b[i][j]==b[I+1][J]) ret++;
        if(b[i][j]==b[I+2][J]) ret++;
    }

    i=I+3,j=J-1;
    if(good(i,j))
    {
        if(b[i][j]==b[I+2][J]) ret++;
    }

    i=I-1,j=J;
    if(good(i,j))
    {
        if(b[i][j]==b[I][J]) ret++;
    }

    i=I+3,j=J;
    if(good(i,j))
    {
        if(b[i][j]==b[I+2][J]) ret++;
    }

    i=I-1,j=J+1;
    if(good(i,j))
    {
        if(b[i][j]==b[I][J]) ret++;
    }

    i=I,j=J+1;
    if(good(i,j))
    {
        if(b[i][j]==b[I][J]) ret++;
        if(b[i][j]==b[I+1][J]) ret++;
    }

    i=I+1,j=J+1;
    if(good(i,j))
    {
        if(b[i][j]==b[I][J]) ret++;
        if(b[i][j]==b[I+1][J]) ret++;
        if(b[i][j]==b[I+2][J]) ret++;
    }

    i=I+2,j=J+1;
    if(good(i,j))
    {
        if(b[i][j]==b[I+1][J]) ret++;
        if(b[i][j]==b[I+2][J]) ret++;
    }

    i=I+3,j=J+1;
    if(good(i,j))
    {
        if(b[i][j]==b[I+2][J]) ret++;
    }


    return ret;
}

// Special scoring function
int tildaScoring(int brickIdx,int topI,int topJ,int b[22][11]) 
{
    if(k<=6)
    {
        if(justGame)
        {
            return fullScoreAndCollapse(b);
        }

        calc2s3s(brickIdx,b);
        heightPTS=b[0][topJ];
        twoPTS=twos;
        threePTS=threes;
        morePTS=more;
        PTS=A*heightPTS+B*twoPTS+C*(D*threePTS+E*morePTS); // Full score
    }
    else
    {
        heightPTS=topI;
        gamePTS=fullScoreAndCollapse(b);
        twoPTS=calcCloseScore(topI,topJ,b);
        PTS=A*heightPTS+B*gamePTS+C*twoPTS;
    }
    return PTS;
}

// Setting some constant values, optimized by genetic algorithms
void setConsts() 
{
    if(k==3)
    {
        A = -100;
        B = 37;
        C = -173;
        D = 8;
        E = 89;
        enrageConst=600;

    }
    else if(k==4)
    {
        A = -816;
        B = 3;
        C = -50;
        D = 459;
        E = 550;
        enrageConst=0;
    }
    else if(k==5)
    {
        A = -50;
        B = 206;
        C = -527;
        D = 256;
        E = 326;
        enrageConst=754;
    }
    else if(k==6)
    {
        A = -816;
        B = 3;
        C = -50;
        D = 459;
        E = 550;
        enrageConst=0;
    }
    else
    {
        A=790,B=472,C=370;
    }
}

// Main function

int main(int argc, char** argv)
{
    DBG=false;

    scanf("%d %d",&n,&k);  // Input & Preparation
    Skor=0;

    setConsts();


    gameOver=false;
    for(int i=1;i<=n;i++)
    {
        scanf("%s",brick);
        brickPlacement(i); // Place every brick
        if(gameOver) return 0;
        Skor+= gameScore; // Game score
       printf("%d %d\n",rowPlace,rotPlace);
    }
   
    if(k == 7) cout<<"x";
   
    return 0;
}
