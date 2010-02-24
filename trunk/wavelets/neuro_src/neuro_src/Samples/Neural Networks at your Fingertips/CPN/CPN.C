/******************************************************************************

                      ==========================
        Network:      Counterpropagation Network
                      ==========================

        Application:  Vision
                      Determination of the Angle of Rotation

        Author:       Karsten Kutza
        Date:         30.4.96

        Reference:    R. Hecht-Nielsen
                      Counterpropagation Networks
                      Proceedings of the IEEE International Conference on
                      Neural Networks, II, IEEE Press, New York, NY, pp. 19-32,
                      1987

 ******************************************************************************/




/******************************************************************************
                            D E C L A R A T I O N S
 ******************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <math.h>


typedef int           BOOL;
typedef char          CHAR;
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

#define LO            0
#define HI            1

#define PI            (2*asin(1))
#define sqr(x)        ((x)*(x))


typedef struct {                     /* A LAYER OF A NET:                     */
        INT           Units;         /* - number of units in this layer       */
        REAL*         Output;        /* - output of ith unit                  */
        REAL**        Weight;        /* - connection weights to ith unit      */
        BOOL*         Winner;        /* - marker for winning instar           */
} LAYER;

typedef struct {                     /* A NET:                                */
        LAYER*        InputLayer;    /* - input layer                         */
        LAYER*        InstarLayer;   /* - instar layer                        */
        LAYER*        OutstarLayer;  /* - outstar layer                       */
        INT           Winners;       /* - allowed number of winning instars   */
        REAL          Alpha;         /* - hidden layer learning rate          */
        REAL          Beta;          /* - output layer learning rate          */
} NET;


/******************************************************************************
        R A N D O M S   D R A W N   F R O M   D I S T R I B U T I O N S
 ******************************************************************************/


void InitializeRandoms()
{
  srand(4711);
}


INT RandomEqualINT(INT Low, INT High)
{
  return rand() % (High-Low+1) + Low;
}      


/******************************************************************************
               A P P L I C A T I O N - S P E C I F I C   C O D E
 ******************************************************************************/


#define NUM_DATA      8
#define X             11
#define Y             11

#define N             (X * Y)
#define C             NUM_DATA
#define M             2

CHAR                  Pattern[NUM_DATA][Y][X] =  { { "           ",
                                                     "           ",
                                                     "     O     ",
                                                     "     O     ",
                                                     "    OOO    ",
                                                     "    OOO    ",
                                                     "    OOO    ",
                                                     "   OOOOO   ",
                                                     "   OOOOO   ",
                                                     "           ",
                                                     "           "  },

                                                   { "           ",
                                                     "           ",
                                                     "        O  ",
                                                     "       O   ",
                                                     "     OOO   ",
                                                     "    OOO    ",
                                                     "   OOO     ",
                                                     " OOOOO     ",
                                                     "OOOOO      ",
                                                     "           ",
                                                     "           "  },

                                                   { "           ",
                                                     "           ",
                                                     "           ",
                                                     "  OO       ",
                                                     "  OOOOO    ",
                                                     "  OOOOOOO  ",
                                                     "  OOOOO    ",
                                                     "  OO       ",
                                                     "           ",
                                                     "           ",
                                                     "           "  },

                                                   { "           ",
                                                     "           ",
                                                     "OOOOO      ",
                                                     " OOOOO     ",
                                                     "   OOO     ",
                                                     "    OOO    ",
                                                     "     OOO   ",
                                                     "       O   ",
                                                     "        O  ",
                                                     "           ",
                                                     "           "  },

                                                   { "           ",
                                                     "           ",
                                                     "   OOOOO   ",
                                                     "   OOOOO   ",
                                                     "    OOO    ",
                                                     "    OOO    ",
                                                     "    OOO    ",
                                                     "     O     ",
                                                     "     O     ",
                                                     "           ",
                                                     "           "  },

                                                   { "           ",
                                                     "           ",
                                                     "      OOOOO",
                                                     "     OOOOO ",
                                                     "     OOO   ",
                                                     "    OOO    ",
                                                     "   OOO     ",
                                                     "   O       ",
                                                     "  O        ",
                                                     "           ",
                                                     "           "  },

                                                   { "           ",
                                                     "           ",
                                                     "           ",
                                                     "       OO  ",
                                                     "    OOOOO  ",
                                                     "  OOOOOOO  ",
                                                     "    OOOOO  ",
                                                     "       OO  ",
                                                     "           ",
                                                     "           ",
                                                     "           "  },

                                                   { "           ",
                                                     "           ",
                                                     "  O        ",
                                                     "   O       ",
                                                     "   OOO     ",
                                                     "    OOO    ",
                                                     "     OOO   ",
                                                     "     OOOOO ",
                                                     "      OOOOO",
                                                     "           ",
                                                     "           "  } };

CHAR                  Pattern_[NUM_DATA][Y][X] = { { "           ",
                                                     "           ",
                                                     "     O     ",
                                                     "     O     ",
                                                     "     O     ",
                                                     "    OOO    ",
                                                     "    OOO    ",
                                                     "    OOO    ",
                                                     "   OOOOO   ",
                                                     "           ",
                                                     "           "  },

                                                   { "           ",
                                                     "           ",
                                                     "     O     ",
                                                     "     O     ",
                                                     "    O O    ",
                                                     "    O O    ",
                                                     "    O O    ",
                                                     "   O   O   ",
                                                     "   O   O   ",
                                                     "           ",
                                                     "           "  },

                                                   { "           ",
                                                     "           ",
                                                     "           ",
                                                     "     O     ",
                                                     "    OOO    ",
                                                     "    OOO    ",
                                                     "    OOO    ",
                                                     "   OOOOO   ",
                                                     "           ",
                                                     "           ",
                                                     "           "  },

                                                   { "           ",
                                                     "           ",
                                                     "           ",
                                                     "           ",
                                                     "     O     ",
                                                     "     O     ",
                                                     "     O     ",
                                                     "    OOO    ",
                                                     "           ",
                                                     "           ",
                                                     "           "  },

                                                   { "           ",
                                                     "  O        ",
                                                     "     O     ",
                                                     "     O     ",
                                                     "    OOO    ",
                                                     "    OO     ",
                                                     "    OOO   O",
                                                     "    OOOO   ",
                                                     "   OOOOO   ",
                                                     "           ",
                                                     "       O   "  },

                                                   { "           ",
                                                     "           ",
                                                     "     O     ",
                                                     "     O     ",
                                                     "    OOO    ",
                                                     "    OOO    ",
                                                     "    OOO    ",
                                                     "   OOOOO   ",
                                                     "   OOOOO   ",
                                                     "           ",
                                                     "           "  },

                                                   { "           ",
                                                     "           ",
                                                     "       O   ",
                                                     "      O    ",
                                                     "    OOO    ",
                                                     "    OOO    ",
                                                     "   OOO     ",
                                                     "  OOOOO    ",
                                                     " OOOOO     ",
                                                     "           ",
                                                     "           "  },

                                                   { "           ",
                                                     "           ",
                                                     "        O  ",
                                                     "       O   ",
                                                     "     OOO   ",
                                                     "    OOO    ",
                                                     "   OOO     ",
                                                     " OOOOO     ",
                                                     "OOOOO      ",
                                                     "           ",
                                                     "           "  } };
                                                    
REAL                  Input [NUM_DATA][N];
REAL                  Input_[NUM_DATA][N];
REAL                  Output[NUM_DATA][M];

FILE*                 f;


void NormalizeInput()
{
  INT  n,i;
  REAL Length, Length_;

  for (n=0; n<NUM_DATA; n++) {
    Length  = 0;
    Length_ = 0;
    for (i=0; i<N; i++) {
      Length  += sqr(Input [n][i]);
      Length_ += sqr(Input_[n][i]);
    }
    Length  = sqrt(Length);
    Length_ = sqrt(Length_);
    for (i=0; i<N; i++) {
      Input [n][i] /= Length;
      Input_[n][i] /= Length_;
    }
  }
}


void InitializeApplication(NET* Net)
{
  INT n,i,j;

  for (n=0; n<NUM_DATA; n++) {
    for (i=0; i<Y; i++) {
      for (j=0; j<X; j++) {
        Input [n][i*X+j] = (Pattern [n][i][j] == 'O') ? HI : LO;
        Input_[n][i*X+j] = (Pattern_[n][i][j] == 'O') ? HI : LO;
      }
    }
  }
  NormalizeInput();
  for (n=0; n<NUM_DATA; n++) {
    Output[n][0] = sin(n * 0.25 * PI);
    Output[n][1] = cos(n * 0.25 * PI);
  }
  f = fopen("CPN.txt", "w");
}


void WriteInput(NET* Net, REAL* Input)
{
  INT i;
   
  for (i=0; i<N; i++) {
    if (i%X == 0) {
      fprintf(f, "\n");
    }
    fprintf(f, "%c", (Input[i] != LO) ? 'O' : ' ');
  }
  fprintf(f, " -> ");
}


void WriteOutput(NET* Net, REAL* Output)
{
  REAL Angle;

  Angle = (atan2(Output[0], Output[1]) / PI) * 180;
  if (Angle < 0)
    Angle = Angle + 360;

  fprintf(f, "%0.0f°\n", Angle);
}


void FinalizeApplication(NET* Net)
{
  fclose(f);
}


/******************************************************************************
                          I N I T I A L I Z A T I O N
 ******************************************************************************/


void GenerateNetwork(NET* Net)
{
  INT i;

  Net->InputLayer   = (LAYER*) malloc(sizeof(LAYER));
  Net->InstarLayer  = (LAYER*) malloc(sizeof(LAYER));
  Net->OutstarLayer = (LAYER*) malloc(sizeof(LAYER));

  Net->InputLayer->Units    = N;
  Net->InputLayer->Output   = (REAL*)  calloc(N, sizeof(REAL));
      
  Net->InstarLayer->Units   = C;
  Net->InstarLayer->Output  = (REAL*)  calloc(C, sizeof(REAL));
  Net->InstarLayer->Weight  = (REAL**) calloc(C, sizeof(REAL*));
  Net->InstarLayer->Winner  = (BOOL*)  calloc(C, sizeof(BOOL));
      
  Net->OutstarLayer->Units  = M;
  Net->OutstarLayer->Output = (REAL*)  calloc(M, sizeof(REAL));
  Net->OutstarLayer->Weight = (REAL**) calloc(M, sizeof(REAL*));
      
  for (i=0; i<C; i++) {
    Net->InstarLayer->Weight[i] = (REAL*) calloc(N, sizeof(REAL));
  }
  for (i=0; i<M; i++) {
    Net->OutstarLayer->Weight[i] = (REAL*) calloc(C, sizeof(REAL));
  }

  Net->Winners = 1;
  Net->Alpha   = 0.1;
  Net->Beta    = 0.1;
}


void SetInput(NET* Net, REAL* Input, BOOL Protocoling)
{
  INT i;
   
  for (i=0; i<Net->InputLayer->Units; i++) {
    Net->InputLayer->Output[i] = Input[i];
  }
  if (Protocoling) {
    WriteInput(Net, Input);
  }
}


void GetOutput(NET* Net, REAL* Output, BOOL Protocoling)
{
  INT i;
   
  for (i=0; i<Net->OutstarLayer->Units; i++) {
    Output[i] = Net->OutstarLayer->Output[i];
  }
  if (Protocoling) {
    WriteOutput(Net, Output);
  }
}


/******************************************************************************
                     P R O P A G A T I N G   S I G N A L S
 ******************************************************************************/


void PropagateToInstars(NET* Net)
{
  INT  w,i,j;
  REAL Sum, SumWinners, MaxOut;
  INT  Winner;

  for (i=0; i<Net->InstarLayer->Units; i++) {
    Sum = 0;
    for (j=0; j<Net->InputLayer->Units; j++) {
      Sum += Net->InstarLayer->Weight[i][j] * Net->InputLayer->Output[j];
    }
    Net->InstarLayer->Output[i] = Sum;
    Net->InstarLayer->Winner[i] = FALSE;
  }
  SumWinners = 0;
  for (w=0; w<Net->Winners; w++) {
    MaxOut = MIN_REAL;
    for (i=0; i<Net->InstarLayer->Units; i++) {
      if (NOT Net->InstarLayer->Winner[i] AND Net->InstarLayer->Output[i] > MaxOut)
        MaxOut = Net->InstarLayer->Output[Winner = i];
    }
    Net->InstarLayer->Winner[Winner] = TRUE;
    SumWinners += Net->InstarLayer->Output[Winner];
  }
  for (i=0; i<Net->InstarLayer->Units; i++) {
    if (Net->InstarLayer->Winner[i])
      Net->InstarLayer->Output[i] = Net->InstarLayer->Output[i] / SumWinners;
    else
      Net->InstarLayer->Output[i] = 0;
  }
}


void PropagateToOutstars(NET* Net)
{
  INT  i,j;
  REAL Sum;

  for (i=0; i<Net->OutstarLayer->Units; i++) {
    Sum = 0;
    for (j=0; j<Net->InstarLayer->Units; j++) {  
      Sum += Net->OutstarLayer->Weight[i][j] * Net->InstarLayer->Output[j];
    }
    Net->OutstarLayer->Output[i] = Sum;
  }
}


void PropagateNet(NET* Net)
{
  PropagateToInstars(Net);
  PropagateToOutstars(Net);
}


/******************************************************************************
                        T R A I N I N G   T H E   N E T
 ******************************************************************************/


INT Winner(NET* Net)
{
  INT i;

  for (i=0; i<Net->InstarLayer->Units; i++) {
    if (Net->InstarLayer->Winner[i])
      return i;
  }
}


void TrainInstars(NET* Net, INT Epochs)
{
  INT n,m,i,j;
   
  for (i=0; i<Net->InstarLayer->Units; i++) {
    for (j=0; j<Net->InputLayer->Units; j++) {
      Net->InstarLayer->Weight[i][j] = Input[i][j];
    }
  }
  Net->Winners = 1;
  for (m=0; m<Epochs*NUM_DATA; m++) {
    n = RandomEqualINT(0, NUM_DATA-1);
    SetInput(Net, Input[n], FALSE);
    PropagateToInstars(Net);
    i = Winner(Net);
    for (j=0; j<Net->InputLayer->Units; j++) {
      Net->InstarLayer->Weight[i][j] +=
        Net->Alpha * (Input[n][j] - Net->InstarLayer->Weight[i][j]);
    }
  }
}


void TrainOutstars(NET* Net, INT Epochs)
{
  INT n,m,i,j;
   
  for (i=0; i<Net->OutstarLayer->Units; i++) {
    for (j=0; j<Net->InstarLayer->Units; j++) {
      Net->OutstarLayer->Weight[i][j] = Output[j][i];
    }
  }
  Net->Winners = 1;
  for (m=0; m<Epochs*NUM_DATA; m++) {
    n = RandomEqualINT(0, NUM_DATA-1);
    SetInput(Net, Input[n], FALSE);
    PropagateToInstars(Net);
    j = Winner(Net);
    for (i=0; i<Net->OutstarLayer->Units; i++) {
      Net->OutstarLayer->Weight[i][j] +=
        Net->Beta * (Output[n][i] - Net->OutstarLayer->Weight[i][j]);
    }
  }
}


/******************************************************************************
                      S I M U L A T I N G   T H E   N E T
 ******************************************************************************/


void SimulateNet(NET* Net, REAL* Input)
{
  REAL Output[M];

  SetInput(Net, Input, TRUE);
  PropagateNet(Net);
  GetOutput(Net, Output, TRUE);
}


/******************************************************************************
                                    M A I N
 ******************************************************************************/


void main()
{
  NET Net;
  INT n;

  InitializeRandoms();
  GenerateNetwork(&Net);
  InitializeApplication(&Net);
  TrainInstars(&Net, 0);         /* weights are computed for this application */
  TrainOutstars(&Net, 0);        /* weights are computed for this application */

  Net.Winners = 2;
  for (n=0; n<NUM_DATA; n++) {
    SimulateNet(&Net, Input[n]);
  }
  for (n=0; n<NUM_DATA; n++) {
    SimulateNet(&Net, Input_[n]);
  }

  FinalizeApplication(&Net);
}
