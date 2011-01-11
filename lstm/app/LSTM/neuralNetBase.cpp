// NeuralNetBase.cpp
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <iomanip>
#include <string>

//#include <new.h> // for memory error handling
//#include <ctype.h>
//#include <unistd.h> // sleep 
//#include <termios.h> // struct termios
#include <fcntl.h>
//#include <sys/param.h>
#include <math.h>
#include <errno.h>
#include <time.h>

#include "neuralNetBase.h"


#define lrand48 rand
#define drand48 drand
#define M_PI       3.14159265358979323846

double drand()
{
    union
    {
        double d;
        unsigned char c[8];
    } r;

    int i;

    for( i = 0 ; i < 7; ++i )
    {
        r.c[i] = rand() & 0xff; 
    }

    r.c[6] |= 0xf0; 
    r.c[7] = 0x3f;

    return r.d - 1.;
}


////TEpoLogBuf
TEpoLogBuf::TEpoLogBuf() {
  BufInUse=false; BufEpo=NULL; BufVal=NULL; BufMin=NULL; BufMax=NULL; 
}


TEpoLogBuf::~TEpoLogBuf() 
{
  if((!BufEpo || !BufVal) && BufInUse) {
    cerr << "NULL buffer to delete." << endl; exit(1); 
  } else {  delete[] BufEpo; delete[] BufVal; }
   if(BufMin) delete[] BufMin; if(BufMax) delete[] BufMax;
}


void TEpoLogBuf::Init( unsigned int MaxBuf, 
		       unsigned int aAverageBuf,
		       char* aBufName) 
{
  BufInUse=true;
  AverageBuf = aAverageBuf; strcpy(BufName, aBufName);
  if(AverageBuf>0) Max = (int)ceil((double)MaxBuf/AverageBuf); 
  else { cerr << "AverageBuf=0." << endl; exit(1); }
  BufEpo = new unsigned int[Max]; BufVal = new double[Max];
  if(!BufEpo || !BufVal) {cerr<<"NULL buffer after new."<<endl; exit(1);}
  // Inti the Min-Max buffers.
  if(AverageBuf>1) {
    BufMin = new double[Max]; BufMax = new double[Max];
    if(!BufMin || !BufMax){cerr<<"NULL buffer after new."<<endl; exit(1);}
  }

  Reset();
}


void TEpoLogBuf::Reset() {
  NbVal = 0; AverageCount=0;
  for(unsigned int i=0;i<Max;i++) { BufEpo[i]=0; BufVal[i]=0; }
}


void TEpoLogBuf::AddValue(unsigned int aEpo, double aVal) {
  // Here index shift Epo = 1,...MaxEpochs -> 0,...MaxEpochs-1
  if(AverageBuf>1) { // Update the min and max of averaged values.
    if(!AverageCount) { ValMin=aVal; ValMax=aVal; // First Val.
    } else { if(aVal<ValMin) ValMin=aVal; if(aVal>ValMax) ValMax=aVal; }
  }
  BufEpo[NbVal] = aEpo;
  BufVal[NbVal] += aVal;
  AverageCount++; 
  if(AverageCount==AverageBuf) {
    BufVal[NbVal]/=AverageBuf;
    if(AverageBuf>1) { BufMin[NbVal]=ValMin; BufMax[NbVal]=ValMax; }
    NbVal++; AverageCount=0;
  }
  if(NbVal>Max) { 
    cerr <<BufName<<": "
	 <<"Epo:"<<aEpo<<"("<<aVal<<") "
	 <<"NbVal("<<NbVal<<")>=Max("<<Max<<") in Buffer."
	 <<"AverageCount:"<<AverageCount<<" "
	 <<"AverageBuf:"<<AverageBuf<<" "
	 <<endl; exit(1); }
}

////TNeuralNetBase

TNeuralNetBase::TNeuralNetBase() 
{
  // Remove files and clear screen.
  if(DirContainsFile(true, ERRORLOGFILE)) remove(ERRORLOGFILE);
  if(DirContainsFile(true, TEST_ERRORLOGFILE)) 
    remove(TEST_ERRORLOGFILE);
  if(DirContainsFile(true, TEST_MSELOGFILE)) remove(TEST_ERRORLOGFILE);
  if(DirContainsFile(true, MSELOGFILE)) remove(MSELOGFILE);
  if(DirContainsFile(true, TEST_STATUS_LOGFILE))
    remove(TEST_STATUS_LOGFILE);
  if(DirContainsFile(true, GROWLOGFILE)) remove(GROWLOGFILE);
  if(DirContainsFile(true, DUMP_FILE)) remove(DUMP_FILE);
  if(DirContainsFile(true, WEIGHTMEAN_LOGFILE)) 
    remove(WEIGHTMEAN_LOGFILE);
  if(DirContainsFile(true, ALPHAMEAN_LOGFILE)) 
    remove(ALPHAMEAN_LOGFILE);
  if(DirContainsFile(true, OUT_LOGFILE)) 
    remove(OUT_LOGFILE);

  // system("clear"); // !!Causes stop in bg job!!
  //printf("\033[0;H\007"); // position curser (0,0) (row,colm)
  //printf("\033[0J\007"); // Clear Below

  // Init user interaction.
  cKey = ' ';
  //  pF.precision(PRECISION);
  one=1;

  // Init debug and watch variables.
  OutputDebug=false;
}

TNeuralNetBase::~TNeuralNetBase() {}


char TNeuralNetBase::Run() { if( Init() ) return 1; return 0; }


char TNeuralNetBase::Init() 
{
#ifdef DEBUG
  cout << "TNeuralNetBase::Init........." << endl;
#endif
  // See how fast we are in sec.
  start_time= time(NULL);
  // Read first part of par file already here to know NbTrials.
  if(TNeuralNetBase::LoadPar()) { 
    cerr<<"Error in NeuralNetBase::Init : LoadPar."<<endl; return 1; }
  // Init random generator
  if(RandomSeed==0) { time_t t; RandomSeed = (long) time(&t); }
//  srand48(RandomSeed);
  // Init EpoLogBuf
#ifdef BUFFER_EPO_LOG
  TrainMSEEpoLogBuf.Init(MaxEpochs+EpochsAfterLearned, AverageTrainBuf,
			 "TrainMSEEpoLogBuf"); 
    if(!NoClassesOnlyPredict)
      TrainErrEpoLogBuf.Init(MaxEpochs+EpochsAfterLearned, AverageTrainBuf,
			     "TrainErrEpoLogBuf");
  unsigned int TME; // Test buffer one longer for the final test.
  if(TestEach_Epo) TME=TestEach_Epo; else TME=TestMaxEach_Epo;
  if(!NoClassesOnlyPredict)
    TestErrEpoLogBuf.
      Init((int) 
	   ceil((double)(MaxEpochs+EpochsAfterLearned)/TME)+1,1,
	   "TestErrEpoLogBuf");
  TestMSEEpoLogBuf.
    Init((int) ceil((double)(MaxEpochs+EpochsAfterLearned)/TME)+1,1,
	 "TestMSEEpoLogBuf");
#endif
  // Init Pattern.
  TrainData.Generalize=0;
  TestData.Generalize=Test_Generalize;
  FreezeSeqLoop=false;
#if !defined(DO_ONLINE_PAT_PRODUCTION)
  FreezeSeqLoop=false;
  if(Generate_Pat) {
    // Test.
    Set_RefData(&TestDataOrg,false,false,true);
    Set_LoopAllSeqs(Generate_Pat_NbLoopAllSeqs_Test);
    GeneratePattern(TestData, Pat_FuncName);
    // Train.
    Set_RefData(&TrainDataOrg,true,true,true);
    Set_LoopAllSeqs(Generate_Pat_NbLoopAllSeqs_Train); // Stays set.
    GeneratePattern(TrainData, Pat_FuncName);
  } else {
    if(LoadTrainPattern()) return 1; // from PatternFile
    if(LoadTestPattern()) return 1; // from PatternFile
    if(PredictionOffset>0) 
      //For prediction task:f*dy(t-1),y(t),f*dy(t),y(t+T)
      // start with y(t).
      //->y(t),y(t+T)
      TransPat2StepByStepPredict(
				 NbIn,NbOut,
				 1, //NbPredictInputs, task dependent.
				 PredictionOffset,
				 PredictAllUntilOffset);
    if(PredictDiff) {
      //->y(t),y(t+T),dy(t-1)
      Add_IODiff(1);
      //->y(t),y(t+T),f*dy(t-1)
      PredictDiffFactor = Scale_IODIff(PredictDiff,2);
      //->f*dy(t-1),y(t),y(t+T),f*dy(t-1)
      //Add_IODiff2Input();// Then: FlipO1O2(2,3); 
      //->f*dy(t-1),y(t),f*dy(t),y(t+T)
      FlipO1O2(1,2); 
      //if(PredictOnlyDiff) 
      //Trans2IODiff(PredictDiff);
      //else Add_IODiff(PredictDiff);
    }
    // Split the series into PredictionOffset sequences dep. on offset.
    if((PredictionOffset>1) && !TimeWindowSize) { 
      SplitPredictionOffset(PredictionOffset); 
    }
    if(TimeWindowSize) {
      //      Add_IODiff(1);
      //PredictDiffFactor = Scale_IODIff(PredictDiff,2);
      Add_TimeWindow(NbIn/TimeWindowSize,
		     TimeWindowSize,TimeWindowStepSize); 
//    //ddd
//      for(unsigned int seq=0;seq<TrainData.NbSeq;seq++) { 
//        for(unsigned int pat=0;pat<TrainData.NbPat[seq];pat++) {
//          cout<<pat<<" ";
//          for(unsigned int val=0;val<TrainData.NbVal;val++) {
//    	cout<<((TrainData.SeqList[seq])[pat])[val]<<" "; }
//          cout<<"\n"; }
//        cout<<"\n"; }
//      exit(0);//ddd
      //->y(t),...,dy(t-1),y(t+T)
      //FlipO1O2(NbIn,NbIn+1); 
    }
    // Modify the NbIn not to include the Diff outputs.
    unsigned int NbOutMSE=NbOut; 
    // Detemine which units contribute to the MSE:
    if(PredictDiff && !PredictOnlyDiff) NbOutMSE/=2;
#ifndef STAT_OUT
    CalcPatStatisticsAll(NbIn,NbOutMSE);
#else
    // Considder only the stat outs.
    CalcPatStatisticsAll(NbIn+NbOut,NbOutMSE);
#endif
  }
  //  exit(0);//ddd
#else
  if(!SetStepTarget) FreezeSeqLoop=true;
#ifdef SETSTEPTARGET_BUT_COUNT_SEQUENCEWISE
  FreezeSeqLoop=true;
#endif
#endif
  //SaveTrainPattern(); exit(0); // For debug.
  // Init general network stuff.
  ClampOut=false;
#ifdef UPDATE_WEIGHTS_AFTER_SEQ
  update_weights_after_seq=UPDATE_WEIGHTS_AFTER_SEQ;
#endif
  // Init Done.
#ifdef DEBUG
  cout << "TNeuralNetBase::Init done." << endl;
#endif
  return 0;
}

char TNeuralNetBase::LoadPar() 
{
#ifdef DEBUG
  cout<<"TNeuralNetBase::LoadPar......."<<endl;//ddd
#endif
  int errStatus = 0;
  fstream pF;// = new ifstream(); 
  sprintf(cBuf,PARFILE);
  if (OpenFile(&pF,cBuf,ios::in)) return 1;
  if (ReadComment(&pF)) return 1;

  errStatus = loadOnePara( "NbTrials:", pF, NbTrials );
  errStatus = loadOnePara( "RandomSeed:", pF, RandomSeed );
  errStatus = loadOnePara( "MaxEpochs:", pF, MaxEpochs );
  errStatus = loadOnePara( "AverageTrainBuf:", pF, AverageTrainBuf );
  errStatus = loadOnePara( "MaxOnlineStreamLengthTrain:", pF, MaxOnlineStreamLengthTrain );
  errStatus = loadOnePara( "MaxOnlineStreamLengthTest:", pF, MaxOnlineStreamLengthTest );
  errStatus = loadOnePara( "EpochsAfterLearned:", pF, EpochsAfterLearned );
  errStatus = loadOnePara( "MaxMSE:", pF, MaxMSE );
  errStatus = loadOnePara( "MaxMSEGenTest:", pF, MaxMSEGenTest );
  errStatus = loadOnePara( "MSEStop:", pF, MSEStop );
  errStatus = loadOnePara( "WinnerTakesAll:", pF, WinnerTakesAll );
  errStatus = loadOnePara( "TestEach_Epo:", pF, TestEach_Epo );
  errStatus = loadOnePara( "TestMaxEach_Epo:", pF, TestMaxEach_Epo );
  errStatus = loadOnePara( "TestMinEach_Epo:", pF, TestMinEach_Epo );
  errStatus = loadOnePara( "TestEpochs:", pF, TestEpochs );
  errStatus = loadOnePara( "DumpBestNotLastTest:", pF, DumpBestNotLastTest );
  errStatus = loadOnePara( "AlphaBase:", pF, AlphaBase );
  errStatus = loadOnePara( "Alpha_ROH:", pF, Alpha_ROH );
  errStatus = loadOnePara( "Alpha_MUE:", pF, Alpha_MUE );
  errStatus = loadOnePara( "AlphaError:", pF, AlphaError );
  errStatus = loadOnePara( "SetStepTarget:", pF, SetStepTarget );
  errStatus = loadOnePara( "AllTimeTarget:", pF, AllTimeTarget );
  errStatus = loadOnePara( "NoClassesOnlyPredict:", pF, NoClassesOnlyPredict );
  errStatus = loadOnePara( "PredictionOffset:", pF, PredictionOffset );
  errStatus = loadOnePara( "PredictAllUntilOffset:", pF, PredictAllUntilOffset );
  errStatus = loadOnePara( "PredictDiff:", pF, PredictDiff );
  errStatus = loadOnePara( "PredictOnlyDiff:", pF, PredictOnlyDiff );
  errStatus = loadOnePara( "RandomPrediction:", pF, RandomPrediction );
  errStatus = loadOnePara( "Reverse_Input:", pF, Reverse_Input );
  errStatus = loadOnePara( "ClampOutPredictSteps:", pF, ClampOutPredictSteps );
  errStatus = loadOnePara( "ClampOutTrainPredictSteps:", pF, ClampOutTrainPredictSteps );
  errStatus = loadOnePara( "TimeWindowSize:", pF, TimeWindowSize );
  errStatus = loadOnePara( "TimeWindowStepSize:", pF, TimeWindowStepSize );
  errStatus = loadOnePara( "NoResetBeforeTest:", pF, NoResetBeforeTest );
  errStatus = loadOnePara( "GrowNet:", pF, GrowNet );
  errStatus = loadOnePara( "FreezeAndGrow:", pF, FreezeAndGrow );
  errStatus = loadOnePara( "GrowFullyNotCascade:", pF, GrowFullyNotCascade );
  errStatus = loadOnePara( "GrowFirstOrder:", pF, GrowFirstOrder );
  errStatus = loadOnePara( "LogErrRecSize:", pF, LogErrRecSize );
  errStatus = loadOnePara( "PartLogErrRecSize:", pF, PartLogErrRecSize );
  errStatus = loadOnePara( "TRAINPATFILE:", pF, cTrainPatFilename );
  errStatus = loadOnePara( "TESTPATFILE:", pF, cTestPatFilename );
  errStatus = loadOnePara( "MixTrainSeqs:", pF, MixTrainSeqs );
  errStatus = loadOnePara( "Generate_Pat:", pF, Generate_Pat );
  errStatus = loadOnePara( "Generate_Pat_Each_Epoch:", pF, Generate_Pat_Each_Epoch );
  errStatus = loadOnePara( "Generate_Pat_NbLoopAllSeqs_Test:", pF, Generate_Pat_NbLoopAllSeqs_Test  );
  errStatus = loadOnePara( "Generate_Pat_NbLoopAllSeqs_Train:", pF, Generate_Pat_NbLoopAllSeqs_Train );
  errStatus = loadOnePara( "Test_Generalize:", pF, Test_Generalize );
  errStatus = loadOnePara( "Pat_FuncName:", pF, Pat_FuncName );
  errStatus = loadOnePara( "NbIn:", pF, NbIn );
  errStatus = loadOnePara( "NbOut:", pF, NbOut );
  errStatus = loadOnePara( "NbStatOut:", pF, NbStatOut );
  errStatus = loadOnePara( "NbHidden:", pF, NbHidden );
  errStatus = loadOnePara( "InitWeightRange:", pF, InitWeightRange );
  errStatus = loadOnePara( "OutUnitBiasInitWeight:", pF, OutUnitBiasInitWeight );
  errStatus = loadOnePara( "InOutBiasForMatchingUnits:", pF, InOutBiasForMatchingUnits );
  errStatus = loadOnePara( "HiddenUnitBiasInitWeight:", pF, HiddenUnitBiasInitWeight );

  // cout<<"HiddenUnitBiasInitWeight:"<<HiddenUnitBiasInitWeight<<endl;//ddd
//    cout<<"NbOut:"<<NbOut<<endl;//ddd
//    cout<<"NbStatOut:"<<NbStatOut<<endl;//ddd
//    cout<<"TNeuralNetBase::LoadPar done.\n"<<endl;//ddd

  CloseFile(&pF);
  return errStatus;
}

char TNeuralNetBase::WriteLogfileEpo(char *FileName, double val) {
  fstream pF;
  if (OpenFile(&pF,FileName, ios::app)) return 1;
  pF << Epo << " " << val << "\n";
  return CloseFile(&pF);
}

char TNeuralNetBase::WriteLogfileEpo(char *FileName, 
				     double val0, double val1) {
  fstream pF;
  if (OpenFile(&pF,FileName, ios::app)) return 1;
  pF << Epo << " " << val0 << " " << val1 << "\n";
  return CloseFile(&pF);
}

char TNeuralNetBase::FlushEpoLogBuf(char *FileName, TEpoLogBuf &EpoLogBuf) {
  fstream pF;
  if (OpenFile(&pF,FileName, ios::app)) return 1;
  // Get the last (maybe incomplete) average right.
  if(EpoLogBuf.AverageCount>0) {
    EpoLogBuf.BufVal[EpoLogBuf.NbVal] /= EpoLogBuf.AverageCount;
    if(EpoLogBuf.AverageBuf>1) { 
      EpoLogBuf.BufMin[EpoLogBuf.NbVal]=EpoLogBuf.ValMin;
      EpoLogBuf.BufMax[EpoLogBuf.NbVal]=EpoLogBuf.ValMax; }
    EpoLogBuf.NbVal++;
  }
  for(unsigned int i=0;i<EpoLogBuf.NbVal;i++){
    pF << EpoLogBuf.BufEpo[i] << " " << EpoLogBuf.BufVal[i] << "\n";
    if(EpoLogBuf.AverageBuf>1) { 
      pF << EpoLogBuf.BufEpo[i] << " " << EpoLogBuf.BufMin[i] << "\n";
      pF << EpoLogBuf.BufEpo[i] << " " << EpoLogBuf.BufMax[i] << "\n";
      // Again for gnuplots linespoints.
      pF << EpoLogBuf.BufEpo[i] << " " << EpoLogBuf.BufVal[i] << "\n";
    }
  }
  return CloseFile(&pF);
}

char TNeuralNetBase::WriteOnlinePatTicks(char *FileName, double val) {
  fstream pF;
  if (OpenFile(&pF,FileName, ios::app)) return 1;
  pF << Pat << " " << val << "\n";
  return CloseFile(&pF);
}

void TNeuralNetBase::NewWeight(TWeight *&pW, unsigned int size) {
  pW = new TWeight[size];
  for(unsigned int i=0;i<size;i++) { 
    for(unsigned int o=0;o<ORDER_WEIGHT;o++) {
      pW[i].w[o]     = InitWeightRange*(drand48()*2-1);
#ifdef UPDATE_WEIGHTS_AFTER_SEQ
      pW[i].Dw[o]    = 0;
#endif
#ifdef MOMENTUM
      pW[i].Dw_t1[o] = 0;
#endif    
#ifdef LOCAL_ALPHA
      pW[i].alpha[o] = AlphaBase;
      //pW[i].h     = 0;
#endif
    }
  }
}

void TNeuralNetBase::NewWeight(TWeight *&pW, unsigned int size, 
			       double InitWeight) {
  NewWeight(pW,size);
  if(InitWeight) for(unsigned int i=0;i<size;i++) 
    for(unsigned int o=0;o<ORDER_WEIGHT;o++)
      pW[i].w[o] = InitWeight;
}

void TNeuralNetBase::NewWeight(TWeight &W, double InitWeight) {
  // For bias weights.
  TWeight *pWtmp; NewWeight(pWtmp,1);
  for(unsigned int o=0;o<ORDER_WEIGHT;o++) {
    W.w[o]     = pWtmp[0].w[o]; 
#ifdef UPDATE_WEIGHTS_AFTER_SEQ
    W.Dw[o]    = pWtmp[0].Dw[o]; 
#endif
#ifdef MOMENTUM
    W.Dw_t1[o]   = 0;
#endif
#ifdef LOCAL_ALPHA
    W.alpha = pWtmp[0].alpha;
    //W.h     = pWtmp[0].h;
#endif
    if(InitWeight) 
		W.w[o] = InitWeight; // else stay random.
  }
  delete[] pWtmp;
}
void TNeuralNetBase::NewWeight(double *&pW, unsigned int size) {
  pW = new double [size];
  for(unsigned int i=0;i<size;i++) pW[i]= InitWeightRange*(drand48()*2-1);
}













