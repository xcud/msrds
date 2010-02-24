/******************************************************************************

                      ===================
        Network:      Self-Organizing Map
                      ===================

        Application:  Control
                      Pole Balancing Problem

        Author:       Karsten Kutza
        Date:         6.6.96

        Reference:    T. Kohonen	
                      Self-Organized Formation
                      of Topologically Correct Feature Maps
                      Biological Cybernetics, 43, pp. 59-69, 1982

 ******************************************************************************/




/******************************************************************************
                            D E C L A R A T I O N S
 ******************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <math.h>


typedef int           BOOL;
typedef int           INT;
typedef double        REAL;

#define FALSE         0
#define TRUE          1
#define NOT           !
#define AND           &&
#define OR            ||

#define MIN_REAL      -HUGE_VAL
#define MAX_REAL      +HUGE_VAL
#define MIN(x,y)      ((x)<(y) ? (x) : (y))
#define MAX(x,y)      ((x)>(y) ? (x) : (y))

#define PI            (2*asin(1))
#define sqr(x)        ((x)*(x))


typedef struct {                     /* A LAYER OF A NET:                     */
        INT           Units;         /* - number of units in this layer       */
        REAL*         Output;        /* - output of ith unit                  */
        REAL**        Weight;        /* - connection weights to ith unit      */
        REAL*         StepSize;      /* - size of search steps of ith unit    */
        REAL*         dScoreMean;    /* - mean score delta of ith unit        */
} LAYER;

typedef struct {                     /* A NET:                                */
        LAYER*        InputLayer;    /* - input layer                         */
        LAYER*        KohonenLayer;  /* - Kohonen layer                       */
        LAYER*        OutputLayer;   /* - output layer                        */
        INT           Winner;        /* - last winner in Kohonen layer        */
        REAL          Alpha;         /* - learning rate for Kohonen layer     */
        REAL          Alpha_;        /* - learning rate for output layer      */
        REAL          Alpha__;       /* - learning rate for step sizes        */
        REAL          Gamma;         /* - smoothing factor for score deltas   */
        REAL          Sigma;         /* - parameter for width of neighborhood */
} NET;


/******************************************************************************
        R A N D O M S   D R A W N   F R O M   D I S T R I B U T I O N S
 ******************************************************************************/


void InitializeRandoms()
{
  srand(4715);
}      


REAL RandomEqualREAL(REAL Low, REAL High)
{
  return ((REAL) rand() / RAND_MAX) * (High-Low) + Low;
}      


REAL RandomNormalREAL(REAL Mu, REAL Sigma)
{
  REAL x,fx;

  do {
    x = RandomEqualREAL(Mu-3*Sigma, Mu+3*Sigma);
    fx = (1 / (sqrt(2*PI)*Sigma)) * exp(-sqr(x-Mu) / (2*sqr(Sigma)));
  } while (fx < RandomEqualREAL(0, 1));
  return x;
}      


/******************************************************************************
               A P P L I C A T I O N - S P E C I F I C   C O D E
 ******************************************************************************/


#define ROWS          25
#define COLS          25

#define N             2
#define C             (ROWS * COLS)
#define M             1

#define TRAIN_STEPS   10000
#define BALANCED      100

FILE*                 f;


void InitializeApplication(NET* Net)
{
  INT i;
   
  for (i=0; i<Net->KohonenLayer->Units; i++) {
    Net->KohonenLayer->StepSize[i] = 1;
    Net->KohonenLayer->dScoreMean[i] = 0;
  }
  f = fopen("SOM.txt", "w");
}


void WriteNet(NET* Net)
{
  INT  r,c;
  REAL x,y,z;

  fprintf(f, "\n\n\n");
  for (r=0; r<ROWS; r++) {
    for (c=0; c<COLS; c++) {
      x = Net->KohonenLayer->Weight[r*ROWS+c][0];
      y = Net->KohonenLayer->Weight[r*ROWS+c][1];
      z = Net->OutputLayer->Weight[0][r*ROWS+c];
      fprintf(f, "([%5.1f°, %5.1f°], %5.1fN)    ", x, y, z);
    }
    fprintf(f, "\n");
  }
}


void FinalizeApplication(NET* Net)
{
  fclose(f);
}


/******************************************************************************
                     S I M U L A T I N G   T H E   P O L E
 ******************************************************************************/

                                     /* SIMULATION PARAMETERS FOR THE POLE:   */
#define L             1              /* - length of pole [m]                  */
#define Mc            1              /* - mass of cart [kg]                   */
#define Mp            1              /* - mass of pole [kg]                   */
#define G             9.81           /* - acceleration due to gravity [m/s²]  */
#define T             0.1            /* - time step [s]                       */
#define STEPS         10             /* - simulation steps in one time step   */


typedef struct {                     /* STATE VARIABLES OF A POLE:            */
        REAL          x;             /* - position of cart [m]                */
        REAL          xDot;          /* - velocity of cart [m/s]              */
        REAL          w;             /* - angle of pole [°]                   */
        REAL          wDot;          /* - angular velocity of pole [°/s]      */
        REAL          F;             /* - force applied to cart               */
} POLE;                              /*   during current time step [N]        */


void InitializePole(POLE* Pole)
{
  do {
    Pole->x    = 0;
    Pole->xDot = 0;
    Pole->w    = RandomEqualREAL(-30, 30);
    Pole->wDot = 0;
    Pole->F    = 0;
  } while (Pole->w == 0);
}


void SimulatePole(POLE* Pole)
{
  INT  s;
  REAL x, xDot, xDotDot;           
  REAL w, wDot, wDotDot;                   
  REAL F;                    

  x    = Pole->x;
  xDot = Pole->xDot;
  w    = (Pole->w    / 180) * PI;
  wDot = (Pole->wDot / 180) * PI;
  F    = Pole->F;
  for (s=0; s<STEPS; s++) {

    wDotDot = (G*sin(w) + cos(w) * ((-F - Mp*L*sqr(wDot)*sin(w)) / (Mc+Mp))) / 
              (L * ((REAL) 4/3 - (Mp*sqr(cos(w))) / (Mc+Mp)));

    xDotDot = (F + Mp*L * (sqr(wDot)*sin(w) - wDotDot*cos(w))) / (Mc+Mp);

    x    += (T / STEPS) * xDot;
    xDot += (T / STEPS) * xDotDot;
    w    += (T / STEPS) * wDot;
    wDot += (T / STEPS) * wDotDot;
  }
  Pole->x    = x;
  Pole->xDot = xDot;
  Pole->w    = (w    / PI) * 180;
  Pole->wDot = (wDot / PI) * 180;
}


BOOL PoleStillBalanced(POLE* Pole)
{
  return (Pole->w >= -60) AND (Pole->w <= 60);
}


REAL ScoreOfPole(POLE* Pole)
{
  return -sqr(Pole->w);
}


/******************************************************************************
                          I N I T I A L I Z A T I O N
 ******************************************************************************/


void GenerateNetwork(NET* Net)
{
  INT i;

  Net->InputLayer   = (LAYER*) malloc(sizeof(LAYER));
  Net->KohonenLayer = (LAYER*) malloc(sizeof(LAYER));
  Net->OutputLayer  = (LAYER*) malloc(sizeof(LAYER));

  Net->InputLayer->Units        = N;
  Net->InputLayer->Output       = (REAL*)  calloc(N, sizeof(REAL));
      
  Net->KohonenLayer->Units      = C;
  Net->KohonenLayer->Output     = (REAL*)  calloc(C, sizeof(REAL));
  Net->KohonenLayer->Weight     = (REAL**) calloc(C, sizeof(REAL*));
  Net->KohonenLayer->StepSize   = (REAL*)  calloc(C, sizeof(REAL));
  Net->KohonenLayer->dScoreMean = (REAL*)  calloc(C, sizeof(REAL));
      
  Net->OutputLayer->Units       = M;
  Net->OutputLayer->Output      = (REAL*)  calloc(M, sizeof(REAL));
  Net->OutputLayer->Weight      = (REAL**) calloc(M, sizeof(REAL*));
      
  for (i=0; i<C; i++) {
    Net->KohonenLayer->Weight[i] = (REAL*) calloc(N, sizeof(REAL));
  }
  for (i=0; i<M; i++) {
    Net->OutputLayer->Weight[i] = (REAL*) calloc(C, sizeof(REAL));
  }
}


void RandomWeights(NET* Net)
{
  INT i,j;
   
  for (i=0; i<Net->KohonenLayer->Units; i++) {
    for (j=0; j<Net->InputLayer->Units; j++) {
      Net->KohonenLayer->Weight[i][j] = RandomEqualREAL(-30, 30);
    }
  }
  for (i=0; i<Net->OutputLayer->Units; i++) {
    for (j=0; j<Net->KohonenLayer->Units; j++) {
      Net->OutputLayer->Weight[i][j] = 0;
    }
  }
}


void SetInput(NET* Net, REAL* Input)
{
  INT i;
   
  for (i=0; i<Net->InputLayer->Units; i++) {
    Net->InputLayer->Output[i] = Input[i];
  }
}


void GetOutput(NET* Net, REAL* Output)
{
  INT i;
   
  for (i=0; i<Net->OutputLayer->Units; i++) {
    Output[i] = Net->OutputLayer->Output[i];
  }
}


/******************************************************************************
                     P R O P A G A T I N G   S I G N A L S
 ******************************************************************************/


void PropagateToKohonen(NET* Net)
{
  INT  i,j;
  REAL Out, Weight, Sum, MinOut;

  for (i=0; i<Net->KohonenLayer->Units; i++) {
    Sum = 0;
    for (j=0; j<Net->InputLayer->Units; j++) {
      Out = Net->InputLayer->Output[j];
      Weight = Net->KohonenLayer->Weight[i][j];
      Sum += sqr(Out - Weight);
    }
    Net->KohonenLayer->Output[i] = sqrt(Sum);
  }
  MinOut = MAX_REAL;
  for (i=0; i<Net->KohonenLayer->Units; i++) {
    if (Net->KohonenLayer->Output[i] < MinOut)
      MinOut = Net->KohonenLayer->Output[Net->Winner = i];
  }
}


void PropagateToOutput(NET* Net)
{
  INT i;

  for (i=0; i<Net->OutputLayer->Units; i++) {
    Net->OutputLayer->Output[i] = Net->OutputLayer->Weight[i][Net->Winner];
  }
}


void PropagateNet(NET* Net)
{
  PropagateToKohonen(Net);
  PropagateToOutput(Net);
}


/******************************************************************************
                        T R A I N I N G   T H E   N E T
 ******************************************************************************/


REAL Neighborhood(NET* Net, INT i)
{
  INT  iRow, iCol, jRow, jCol;
  REAL Distance;

  iRow = i / COLS; jRow = Net->Winner / COLS;
  iCol = i % COLS; jCol = Net->Winner % COLS;

  Distance = sqrt(sqr(iRow-jRow) + sqr(iCol-jCol));

  return exp(-sqr(Distance) / (2*sqr(Net->Sigma)));
}


void TrainKohonen(NET* Net, REAL* Input)
{
  INT  i,j;
  REAL Out, Weight, Lambda, StepSize;

  for (i=0; i<Net->KohonenLayer->Units; i++) {
    for (j=0; j<Net->InputLayer->Units; j++) {
      Out = Input[j];
      Weight = Net->KohonenLayer->Weight[i][j];
      Lambda = Neighborhood(Net, i);
      Net->KohonenLayer->Weight[i][j] += Net->Alpha * Lambda * (Out - Weight);
    }
    StepSize = Net->KohonenLayer->StepSize[i];
    Net->KohonenLayer->StepSize[i] += Net->Alpha__ * Lambda * -StepSize;
  }
}


void TrainOutput(NET* Net, REAL* Output)
{
  INT  i,j;
  REAL Out, Weight, Lambda;

  for (i=0; i<Net->OutputLayer->Units; i++) {
    for (j=0; j<Net->KohonenLayer->Units; j++) {
      Out = Output[i];
      Weight = Net->OutputLayer->Weight[i][j];
      Lambda = Neighborhood(Net, j);
      Net->OutputLayer->Weight[i][j] += Net->Alpha_ * Lambda * (Out - Weight);
    }
  }
}


void TrainUnits(NET* Net, REAL* Input, REAL* Output)
{
  TrainKohonen(Net, Input);
  TrainOutput(Net, Output);
}


void TrainNet(NET* Net)
{
  INT  n,t;
  POLE Pole;
  REAL wOld, wNew, ScoreOld, ScoreNew, dScore, dScoreMean, StepSize;
  REAL Input[N];
  REAL Output[M];
  REAL Target[M];

  n = 0;
  while (n<TRAIN_STEPS) {
    t = 0;
    InitializePole(&Pole);
    fprintf(f, " Time     Angle     Force\n");
    fprintf(f, "%4.1fs    %5.1f°    %5.1fN\n", t * T, Pole.w, Pole.F);
    wOld = Pole.w;
    ScoreOld = ScoreOfPole(&Pole);
    SimulatePole(&Pole);
    wNew = Pole.w;
    ScoreNew = ScoreOfPole(&Pole);
    while (PoleStillBalanced(&Pole) AND (t<BALANCED)) {
      n++;
      t++;
      Net->Alpha   = 0.5 * pow(0.01, (REAL) n / TRAIN_STEPS);
      Net->Alpha_  = 0.5 * pow(0.01, (REAL) n / TRAIN_STEPS);
      Net->Alpha__ = 0.005;
      Net->Gamma   = 0.05;
      Net->Sigma   = 6.0 * pow(0.2, (REAL) n / TRAIN_STEPS);
      Input[0] = wOld;
      Input[1] = wNew;
      SetInput(Net, Input);
      PropagateNet(Net);
      GetOutput(Net, Output);
      Pole.F = Output[0];
      StepSize = Net->KohonenLayer->StepSize[Net->Winner];
      Pole.F += StepSize * RandomNormalREAL(0, 10);
      fprintf(f, "%4.1fs    %5.1f°    %5.1fN\n", t * T, Pole.w, Pole.F);
      wOld = Pole.w;
      ScoreOld = ScoreOfPole(&Pole);
      SimulatePole(&Pole);
      wNew = Pole.w;
      ScoreNew = ScoreOfPole(&Pole);
      dScore = ScoreNew - ScoreOld;
      dScoreMean = Net->KohonenLayer->dScoreMean[Net->Winner];
      if (dScore > dScoreMean) {
        Target[0] = Pole.F;
        TrainUnits(Net, Input, Target);
      }
      Net->KohonenLayer->dScoreMean[Net->Winner] += Net->Gamma * (dScore - dScoreMean);
    }
    if (PoleStillBalanced(&Pole))
      fprintf(f, "Pole still balanced after %0.1fs ...\n\n", t * T);
    else
      fprintf(f, "Pole fallen after %0.1fs ...\n\n", (t+1) * T);
  }
}


/******************************************************************************
                                    M A I N
 ******************************************************************************/


void main()
{
  NET Net;

  InitializeRandoms();
  GenerateNetwork(&Net);
  RandomWeights(&Net);
  InitializeApplication(&Net);
  TrainNet(&Net);
  WriteNet(&Net);
  FinalizeApplication(&Net);
}
