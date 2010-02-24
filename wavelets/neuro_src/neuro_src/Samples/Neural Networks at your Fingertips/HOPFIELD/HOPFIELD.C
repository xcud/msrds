/******************************************************************************

                      ==============
        Network:      Hopfield Model
                      ==============

        Application:  Autoassociative Memory
                      Associative Recall of Images

        Author:       Karsten Kutza
        Date:         30.1.96

        Reference:    J.J. Hopfield
                      Neural Networks and Physical Systems
                      with Emergent Collective Computational Abilities
                      Proceedings of the National Academy of Sciences, 79,
                      pp. 2554-2558, 1982

 ******************************************************************************/




/******************************************************************************
                            D E C L A R A T I O N S
 ******************************************************************************/


#include <stdlib.h>
#include <stdio.h>


typedef int           BOOL;
typedef char          CHAR;
typedef int           INT;

#define FALSE         0
#define TRUE          1
#define NOT           !
#define AND           &&
#define OR            ||

#define LO            -1
#define HI            +1

#define BINARY(x)     ((x)==LO ? FALSE : TRUE)
#define BIPOLAR(x)    ((x)==FALSE ? LO : HI)


typedef struct {                     /* A NET:                                */
        INT           Units;         /* - number of units in this net         */
        INT*          Output;        /* - output of ith unit                  */
        INT*          Threshold;     /* - threshold of ith unit               */
        INT**         Weight;        /* - connection weights to ith unit      */
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


#define NUM_DATA      5
#define X             10
#define Y             10

#define N             (X * Y)

CHAR                  Pattern[NUM_DATA][Y][X]  = { { "O O O O O ",
                                                     " O O O O O",
                                                     "O O O O O ",
                                                     " O O O O O",
                                                     "O O O O O ",
                                                     " O O O O O",
                                                     "O O O O O ",
                                                     " O O O O O",
                                                     "O O O O O ",
                                                     " O O O O O"  },

                                                   { "OO  OO  OO",
                                                     "OO  OO  OO",
                                                     "  OO  OO  ",
                                                     "  OO  OO  ",
                                                     "OO  OO  OO",
                                                     "OO  OO  OO",
                                                     "  OO  OO  ",
                                                     "  OO  OO  ",
                                                     "OO  OO  OO",
                                                     "OO  OO  OO"  },

                                                   { "OOOOO     ",
                                                     "OOOOO     ",
                                                     "OOOOO     ",
                                                     "OOOOO     ",
                                                     "OOOOO     ",
                                                     "     OOOOO",
                                                     "     OOOOO",
                                                     "     OOOOO",
                                                     "     OOOOO",
                                                     "     OOOOO"  },

                                                   { "O  O  O  O",
                                                     " O  O  O  ",
                                                     "  O  O  O ",
                                                     "O  O  O  O",
                                                     " O  O  O  ",
                                                     "  O  O  O ",
                                                     "O  O  O  O",
                                                     " O  O  O  ",
                                                     "  O  O  O ",
                                                     "O  O  O  O"  },

                                                   { "OOOOOOOOOO",
                                                     "O        O",
                                                     "O OOOOOO O",
                                                     "O O    O O",
                                                     "O O OO O O",
                                                     "O O OO O O",
                                                     "O O    O O",
                                                     "O OOOOOO O",
                                                     "O        O",
                                                     "OOOOOOOOOO"  } };

CHAR                  Pattern_[NUM_DATA][Y][X] = { { "          ",
                                                     "          ",
                                                     "          ",
                                                     "          ",
                                                     "          ",
                                                     " O O O O O",
                                                     "O O O O O ",
                                                     " O O O O O",
                                                     "O O O O O ",
                                                     " O O O O O"  },

                                                   { "OOO O    O",
                                                     " O  OOO OO",
                                                     "  O O OO O",
                                                     " OOO   O  ",
                                                     "OO  O  OOO",
                                                     " O OOO   O",
                                                     "O OO  O  O",
                                                     "   O OOO  ",
                                                     "OO OOO  O ",
                                                     " O  O  OOO"  },

                                                   { "OOOOO     ",
                                                     "O   O OOO ",
                                                     "O   O OOO ",
                                                     "O   O OOO ",
                                                     "OOOOO     ",
                                                     "     OOOOO",
                                                     " OOO O   O",
                                                     " OOO O   O",
                                                     " OOO O   O",
                                                     "     OOOOO"  },

                                                   { "O  OOOO  O",
                                                     "OO  OOOO  ",
                                                     "OOO  OOOO ",
                                                     "OOOO  OOOO",
                                                     " OOOO  OOO",
                                                     "  OOOO  OO",
                                                     "O  OOOO  O",
                                                     "OO  OOOO  ",
                                                     "OOO  OOOO ",
                                                     "OOOO  OOOO"  },

                                                   { "OOOOOOOOOO",
                                                     "O        O",
                                                     "O        O",
                                                     "O        O",
                                                     "O   OO   O",
                                                     "O   OO   O",
                                                     "O        O",
                                                     "O        O",
                                                     "O        O",
                                                     "OOOOOOOOOO"  } };

INT                   Input [NUM_DATA][N];
INT                   Input_[NUM_DATA][N];

FILE*                 f;


void InitializeApplication(NET* Net)
{
  INT n,i,j;

  for (n=0; n<NUM_DATA; n++) {
    for (i=0; i<Y; i++) {
      for (j=0; j<X; j++) {
        Input [n][i*X+j] = BIPOLAR(Pattern [n][i][j] == 'O');
        Input_[n][i*X+j] = BIPOLAR(Pattern_[n][i][j] == 'O');
      }
    }
  }
  f = fopen("HOPFIELD.txt", "w");
}


void WriteNet(NET* Net)
{
  INT i,j;
   
  for (i=0; i<Y; i++) {
    for (j=0; j<X; j++) {
      fprintf(f, "%c", BINARY(Net->Output[i*X+j]) ? 'O' : ' ');
    }
    fprintf(f, "\n");
  }
  fprintf(f, "\n");
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

  Net->Units     = N;
  Net->Output    = (INT*)  calloc(N, sizeof(INT));
  Net->Threshold = (INT*)  calloc(N, sizeof(INT));
  Net->Weight    = (INT**) calloc(N, sizeof(INT*));

  for (i=0; i<N; i++) {
    Net->Threshold[i] = 0;
    Net->Weight[i]    = (INT*) calloc(N, sizeof(INT));
  }
}


void CalculateWeights(NET* Net)
{
  INT i,j,n;
  INT Weight;

  for (i=0; i<Net->Units; i++) {
    for (j=0; j<Net->Units; j++) {
      Weight = 0;
      if (i!=j) {
        for (n=0; n<NUM_DATA; n++) {
          Weight += Input[n][i] * Input[n][j];
        }
      }
      Net->Weight[i][j] = Weight;
    }
  }
}


void SetInput(NET* Net, INT* Input)
{
  INT i;
   
  for (i=0; i<Net->Units; i++) {
    Net->Output[i] = Input[i];
  }
  WriteNet(Net);
}


void GetOutput(NET* Net, INT* Output)
{
  INT i;
   
  for (i=0; i<Net->Units; i++) {
    Output[i] = Net->Output[i];
  }
  WriteNet(Net);
}


/******************************************************************************
                     P R O P A G A T I N G   S I G N A L S
 ******************************************************************************/


BOOL PropagateUnit(NET* Net, INT i)
{
  INT  j;
  INT  Sum, Out;
  BOOL Changed;

  Changed = FALSE;
  Sum = 0;
  for (j=0; j<Net->Units; j++) {
    Sum += Net->Weight[i][j] * Net->Output[j];
  }
  if (Sum != Net->Threshold[i]) {
    if (Sum < Net->Threshold[i]) Out = LO;
    if (Sum > Net->Threshold[i]) Out = HI;
    if (Out != Net->Output[i]) {
      Changed = TRUE;
      Net->Output[i] = Out;
    }
  }
  return Changed;
}


void PropagateNet(NET* Net)
{
  INT Iteration, IterationOfLastChange;

  Iteration = 0;
  IterationOfLastChange = 0;
  do {
    Iteration++;
    if (PropagateUnit(Net, RandomEqualINT(0, Net->Units-1)))
      IterationOfLastChange = Iteration;
  } while (Iteration-IterationOfLastChange < 10*Net->Units);
}


/******************************************************************************
                      S I M U L A T I N G   T H E   N E T
 ******************************************************************************/


void SimulateNet(NET* Net, INT* Input)
{
  INT Output[N];
   
  SetInput(Net, Input);
  PropagateNet(Net);
  GetOutput(Net, Output);
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
  CalculateWeights(&Net);
   
  for (n=0; n<NUM_DATA; n++) {
    SimulateNet(&Net, Input[n]);
  }
  for (n=0; n<NUM_DATA; n++) {
    SimulateNet(&Net, Input_[n]);
  }
   
  FinalizeApplication(&Net);
}
