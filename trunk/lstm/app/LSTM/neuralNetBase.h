#ifndef KTB_NEURALNETBASE_H
#define KTB_NEURALNETBASE_H

// Define to enable math macros, 
// do this here no to do this globally for all projects.
#define MATHMACROS_H_EXP
#define MATHMACROS_H_LOGC

#include <assert.h>

#include "patternManagement.h"
#include "..\base\MathMacros.h"

#define M_LN2      0.693147180559945309417


//// defines
//#define DEBUG

// Output file names. PARFILE should be redefined.
#define PARFILE  "LSTM.par"
#define ERRORLOGFILE  "TrainError.log"
#define TEST_ERRORLOGFILE  "TestError.log"
#define MSELOGFILE  "TrainMSE.log"
#define TEST_MSELOGFILE  "TestMSE.log"
#define GROWLOGFILE  "Grow.log"
#define WEIGHTLOGFILE  "Weight.log"
#define DUMP_FILE  "dump.log"
#define WEIGHTMEAN_LOGFILE  "WeightMean.log"
#define ALPHAMEAN_LOGFILE  "AlphaMean.log"
#define OUT_LOGFILE  "Out.log"
#define ONLINEPATTICKS_LOGFILE "OnlinePatTicks.log"
#define TEST_STATUS_LOGFILE  "TestStatus.log"
#define FINAL_W_LOGFILE "w.log.end"
#define DEBUG_LOGFILE "ddd.log"
// Algorithm.
// Polynomial order of weights (see Flake), min=1 (normal weights).
#define ORDER_WEIGHT 1
//#define LOCAL_ALPHA
//#define NICSNEWALGO
//#define LOCAL_ALPHA_SEQUENCEWISE
//#define MOMENTUM 0.99
//#define USE_MOMENTUM_CUT
//#define WEIGHT_DECAY
//#define RESTRICT_WEIGHT_RANGE 1000000
//#define ALPHA_DECAY_IN_SEQ 1
//#define ALPHA_DECAY_IN_SEQ_LINEAR
//#define DO_WEIGHT_STATISTICS
// Update at the end of Seqs or after (if>0) #Pats !!! not Seqs!.
#define UPDATE_WEIGHTS_AFTER_SEQ 50
//To save memory generate one seq at a time; for Protein2D task:
//Should not be defined if DO_ONLINE_PAT_PRODUCTION only if Generate_Pat.
//#define PATTERN_GENERATION_FROM_DATA_SEQWISE
//#define DO_ONLINE_PAT_PRODUCTION 
// For TempOrder, TempXOR task:
//#define NO_RESET_AFTER_ONLINE_SEQ
// For Timer task:
//#define SETSTEPTARGET_BUT_COUNT_SEQUENCEWISE
#define TEST_GENERALIZATION
#define ONLINE_PAT_FUNCNAME Online_ReberGrammar
//#define ONLINE_PAT_FUNCNAME Online_SlidingNBitMemory
//#define ONLINE_PAT_FUNCNAME Online_TempOrderEx6
//#define ONLINE_PAT_FUNCNAME Online_AddMulti
//#define ONLINE_PAT_FUNCNAME Online_TimerClass
//#define ONLINE_PAT_FUNCNAME Online_Oscillator
//#define ONLINE_PAT_FUNCNAME Online_Timer
//#define ONLINE_PAT_FUNCNAME Online_AnBn
#define PRECISION 10
#define BUFFER_EPO_LOG
//#define LOG_LIKELIHOOD_OUTPUT
//#define LINEAR_OUTPUT
#define OUTPUT_SIGMOID 1.0
//#define STAT_OUT
//#define TARGET_777
//#define SET_MIN_DERIVATIVE 0.01
//#define LEARN_MORE_FROM_WRONG_SYMBOLS
//#define RMSE
//#define NRMSE
//#define SCALE_DATA 0.01
//#define Offset_DATA -60.0
//#define CLAMPING


//// classes in this header
class TEpoLogBuf;
class TNeuralNetBase;

////TEpoLogBuf
// Buffer for the epo log files.
class TEpoLogBuf {
public:
  TEpoLogBuf();
  ~TEpoLogBuf();
  void Init(unsigned int MaxBuf, unsigned int aAverageBuf, char* aBufName);
  void Reset();
  void AddValue(unsigned int iEpo, double aVal);
  char BufName[64];
  bool BufInUse;
  unsigned int Max, NbVal, *BufEpo; 
  double *BufVal, *BufMin, *BufMax, ValMin, ValMax; 
  unsigned int AverageBuf, AverageCount;
};

////NeuralNetBase
class TNeuralNetBase : public TPatternGenerator {
public:
  TNeuralNetBase();
  virtual ~TNeuralNetBase();
  virtual char Run();
protected:
  char Init();

  // Scan for one parameter in parameter file.
  // Return 1 one error otherwise 0.
  // Also used by children to scan thier parameters.
  template <class ParaType> int loadOnePara
    ( const char * const paraName, fstream & pF, 
      ParaType & para ) 
    {
      assert( paraName );
      char buf[1024];
      pF >> buf; 
      if( strcmp( buf, paraName ) ) 
	{ 
	  if( buf[0] == '#' ) // Check for comment.
	    {
	      // cout << "Some comment. \n" //ddd
	      return 0; 
	    }
	  else
	    {
	      cerr << "WARNING: mismatch in LSTM.par\n";
	      cerr << "Scaned parameter " 
		   << paraName << " != " << buf << endl;
	      return 1; 
	    }
	}
      else
	{
#ifdef DEBUG	  
	  cout << "Scaned parameter " 
	       << paraName << "==" << buf;
#endif
	}
      pF >> para;
#ifdef DEBUG	  
      cout << para << endl;
#endif
      return 0;
    }
  //
  virtual char LoadPar();
  char WriteLogfileEpo(char *FileName, double val);
  char WriteLogfileEpo(char *FileName, double val0, double val1);
  char FlushEpoLogBuf(char *FileName, TEpoLogBuf &EpoLogBuf);
  char WriteOnlinePatTicks(char *FileName, double val);
  // Inline functions.
  void log_sig(double x,double &y) { 
    if(x>709){x=709;}//cerr<<Tri<<"- x>709 in log_sig()\n"; y=1; return;}
    else if(x<-709){x=-709;}//cerr<<Tri<<"- x<709 in log_sig()\n"; 
    //y=-1; return; }
    y = LOGC(x); }//y=1/(1+EXP(-x)); }
  double log_sig_d(double y,double &df) { 
#ifdef SET_MIN_DERIVATIVE
    if(df<SET_MIN_DERIVATIVE) df=SET_MIN_DERIVATIVE;
#endif
    df=y*(1-y); return df; }
  double log_sig_d(double y) { 
#ifdef SET_MIN_DERIVATIVE
    double df; df=(y*(1-y));
    if(df<SET_MIN_DERIVATIVE) df=SET_MIN_DERIVATIVE;
    return df;
#else
return(y*(1-y));
#endif
  }
  double tanh_d(double y,double &df) { df=1-y*y; return df; }
  double tanh_d(double y) { return (1-y*y); }
  //{df=EXP(y); df=df/(1+df)*(1+df); return df;}
  //{ df=y*(1-y); return df; }
  // Parameter
  unsigned int NbTrials;
  long RandomSeed;
  unsigned int MaxEpochs, EpochsAfterLearned, TestEpochs;
  unsigned int TestEach_Epo, TestMaxEach_Epo;  // 0 -> no tests.
  unsigned int TestMinEach_Epo, LastTestEpo;
  bool DumpBestNotLastTest;
  double Alpha, AlphaBase, AlphaError, AlphaPredict, AlphaDecayInSeq;
  double InitWeightRange, OutUnitBiasInitWeight, HiddenUnitBiasInitWeight;
  double InOutBiasForMatchingUnits;
  bool GrowNet, GrowFullyNotCascade, GrowFirstOrder, FreezeAndGrow;
  // For stochastic weight update within a sequence.
  unsigned int update_weights_after_seq;
  // SetStepTarget for stepwise prediction, AllTimeTarget sets target
  // for classification in any time step.
  bool SetStepTarget, AllTimeTarget;
  // Time series prediction: predict pat+PredictionOffset.
  bool NoClassesOnlyPredict;
  int PredictionOffset;
  // NbOut has to prefixed for that.
  unsigned int PredictAllUntilOffset;
  // Add an extra dimention to the patterns: val[1]-val[0]:Out-In
  // and multiply it with PredictDiff.
  double  PredictDiff, PredictDiffFactor, PredictOnlyDiff;
  // Starting point (random offset in pat) from sequence start.
  unsigned int RandomPrediction, RandomPredictionStart;
  // Time window of inputs (See PattenManager).
  unsigned int TimeWindowSize, TimeWindowStepSize;
  // Store old prediction resuts for sliding window.
  double *OutWindow; 
  int OutWindowPos;
  // Continue the prediction after training for test without reset.
  bool NoResetBeforeTest;
  // Parameter for topology.
  unsigned int NbIn, NbOut, NbHidden;
#ifdef STAT_OUT
  // Output unit caluculated from activation  of other output units,
  // serve for statisics and not to train the network.
  // The targets should be in the pats after the regular vals.           
  struct TStatOut { double y,y_save,e,t; } *StatOut;//t=target. See define.
#endif
  // Keep the number in any case to have the same par file.
  unsigned int NbStatOut;
  // Parameter for Data,
  // To generate the pattern instead of loading them from a file.
  bool MixTrainSeqs, Generate_Pat;
  unsigned int Generate_Pat_Each_Epoch;
  unsigned int Generate_Pat_NbLoopAllSeqs_Test;
  unsigned int Generate_Pat_NbLoopAllSeqs_Train;
  unsigned int Test_Generalize; // Bool and min of TrainSet.
  char Pat_FuncName[256];
  // Variables for statistics.
  bool WinnerTakesAll, ThisPatWrong;
  unsigned int PatWrong, ClassesWrong, TestClassesWrong;
  double PatCorrect, PatCorrectTest; // For continuous prediction.
  // To test only when improved.
  double MaxPatCorrectTrain, MinPatCorrectTrain, MeanPatCorrectTrain;
  unsigned int MeanPatCorrectTrainCount; 
  double MinMSEEpoTrain;
  unsigned int MinClassesWrong;
  bool NewBest;
  bool StopLern; 
  double ClassesWrongRel; 
  unsigned int ClassNb, WinnerNb;
  unsigned int LogErrRecSize, PartLogErrRecSize, NbLogedErr;
  double LogErrRecMean, PartLogErrRecMean;
  double MSEPat, MSESeq, MSEEpo, MSEStop,MaxMSE,MaxMSEGenTest,TestMSEEpo;
  double GeneralizationStaus;
  double WeightMean, AlphaMean, AlphaStd;
  double NbWeights;
  // Debug variables.
  double WeightsCount;// WPC;
  bool OutputDebug;
  // Global working variables (same as ).
  unsigned int Tri, Epo, TestEpo, Seq, SeqOnline, Pat, Val;
  double **CurInput;
  double one; // For bias activation.
  // Extra forward pass variables for REVERSE_INPUT.
  // Feed In(t) to In(t-REVERSE_INPUT), then output target t(t).
  unsigned int Reverse_Input, PatReInStart, PatReInStop;
  // For stepwise prediction. 
  // Output is clamped to input and external input is cut off.
  // Predict ClampOutPredictSteps times Prediction offset.
  // ClampOut sets the clamp, Clamping is for help.
  bool ClampOut, Clamping;
  unsigned int ClampOutTrainPredictSteps;
  // 0=none, one=one-step-ahead=useless to doing this way.
  unsigned int ClampOutPredictSteps; 
  unsigned int ClampPatStart, ClampPatStop;
  unsigned int SeqOnlinePatCount; // For Dump_Inner_States numbering.
  bool FreezeSeqLoop; // For sequencewise online pat generation.
  // Variables for display and user interaction.
  char cPrecision, cWidth;
  time_t  start_time, finish_time, tmp_time; // calender time
  time_t  start_ctime, finish_ctime; // calender time
  // File pointer for log files.
  fstream *TrainErr, *TrainMSE, *TestErr, *TestMSE;
  fstream DebugFile;
  // Buffer
#ifdef BUFFER_EPO_LOG
  TEpoLogBuf TrainMSEEpoLogBuf, TrainErrEpoLogBuf, 
    TestErrEpoLogBuf, TestMSEEpoLogBuf;
#endif
  unsigned int AverageTrainBuf; // Alwaus defined, for the .par file.
  // Defines for the local learning rate.
  double Alpha_ROH, Alpha_MUE;
  struct TWeight { 
    double w[ORDER_WEIGHT];
#ifdef UPDATE_WEIGHTS_AFTER_SEQ
    double Dw[ORDER_WEIGHT];
#endif
#if defined(MOMENTUM) || defined(NICSNEWALGO)
    double Dw_t1[ORDER_WEIGHT];
#endif    
#ifdef LOCAL_ALPHA
    double alpha[ORDER_WEIGHT];
    //    double h;  // For Nic's K1.
#endif
  };
  void NewWeight(TWeight *&pW, unsigned int size);
  void NewWeight(TWeight *&pW, unsigned int size, double InitWeight);
  void NewWeight(TWeight &W, double InitWeight);
  void NewWeight(double *&pW, unsigned int size);
  void AdjustAlphaAndWeights(double &delta,
#ifdef LOCAL_ALPHA
			     // double  df,
#endif
			     double *y_t1, TWeight &W, bool Order0) {
    double dw;
    // Bias weights are order 0 -> loop just once.
    for(unsigned int o=0;o<ORDER_WEIGHT;Order0 ? o=ORDER_WEIGHT : o++) {
      //df=df; // To avoid unused warning.
#ifndef LOCAL_ALPHA
      dw = AlphaBase * delta * y_t1[o];
      //  #ifdef LEARN_MORE_FROM_WRONG_SYMBOLS
      //      if(!ThisPatWrong) dw /= TrainData.NbPat[Seq];
      //  #endif
      //  #ifdef ALPHA_DECAY_IN_SEQ
      //      dw *= AlphaDecayInSeq;
      //  #endif
      //  #ifdef WEIGHT_DECAY
      //      // Weight decay.
      //      if(((W.w[o]>0)&&(dw>0))||((W.w[o]<0)&&(dw<0))) dw *=0.9;
      //  #endif
#ifdef UPDATE_WEIGHTS_AFTER_SEQ
      W.Dw[o] += dw;
#else
#ifdef MOMENTUM
#ifdef USE_MOMENTUM_CUT
      if(((dw>=0)&&(W.Dw_t1[o]<0)||(dw<=0)&&(W.Dw_t1[o]>0)))
	W.Dw_t1[o]=0;// Cut the momentum when sign changes.
#endif
      dw += MOMENTUM * W.Dw_t1[o]; W.Dw_t1[o] = dw;
#endif   
      W.w[o] += dw;
#endif
      //W.w[o] *= 0.999;
#else
      //    dw = W.alpha[o] * delta * y_t1[o];
#ifdef NICSNEWALGO
      W.alpha[o] += Alpha_MUE * W.alpha[o] * W.Dw_t1[o] * delta;
      dw = W.alpha[o] * delta * y_t1[o]; W.w[o] += dw;
      W.Dw_t1[o] = dw;
      if(W.alpha[o]<0) cout<<W.alpha[o]<<endl;//ddd
#endif
      //      double delta_y_t1[o] = delta*y_t1[o];
      //      double df_y_t1[o] = df*y_t1[o];
      //      // Calc alpha.
      //      double bracket = 1+Alpha_MUE*W.h[o]*delta_y_t1[o]; 
      //      if(bracket<Alpha_ROH) bracket=Alpha_ROH;
      //      W.alpha[o] *= bracket;
      //      // Update w.
      //      dw = W.alpha[o] * delta_y_t1[o]; W.w[o] += dw;
      //      // Calc h.
      //      double k = W.alpha[o] * df_y_t1[o];
      //      bracket = 1 - k * df_y_t1[o]; if(bracket<0) bracket=0;
      //      W.h[o] = W.h[o] * bracket + dw;
#endif
#ifdef DO_WEIGHT_STATISTICS
      // Some global statistics.
      WeightMean += fabs(W.w[o]);
      //AlphaMean += W.alpha[o]; AlphaStd += SQR(W.alpha[o]); //WPC++;
#endif
#ifdef RESTRICT_WEIGHT_RANGE
      if(fabs(W.w[o])>RESTRICT_WEIGHT_RANGE) W.w[o]=RESTRICT_WEIGHT_RANGE;
#endif
    } // Order o loop.
  }

  void RTRLAdjustAlphaAndWeights(double *s_d, double e,
#ifdef LOCAL_ALPHA	 
				 double *alpha,
				 // double &h,
#endif				 
				 TWeight &W, bool Order0) {
    double dw;
    for(unsigned int o=0;o<ORDER_WEIGHT;Order0 ? o=ORDER_WEIGHT : o++) {
      //h=h; // To avoid unused warning.
#ifndef LOCAL_ALPHA
      dw = AlphaBase * s_d[o] * e;
      //  #ifdef LEARN_MORE_FROM_WRONG_SYMBOLS
      //      if(!ThisPatWrong) dw /= TrainData.NbPat[Seq];
      //  #endif
      //  #ifdef ALPHA_DECAY_IN_SEQ
      //      dw *= AlphaDecayInSeq;
      //  #endif
      //  #ifdef WEIGHT_DECAY
      //      // Weight decay.
      //      if(((W.w[o]>0)&&(dw>0))||((W.w[o]<0)&&(dw<0))) dw *=0.9;
      //  #endif
#ifdef UPDATE_WEIGHTS_AFTER_SEQ
      W.Dw[o] += dw;
#else
#ifdef MOMENTUM
#ifdef USE_MOMENTUM_CUT
      if(((dw>=0)&&(W.Dw_t1[o]<0)||(dw<=0)&&(W.Dw_t1[o]>0)))
	W.Dw_t1[o]=0;// Cut the momentum when sign changes.
#endif
      dw += MOMENTUM * W.Dw_t1[o]; W.Dw_t1[o] = dw;
#endif
  W.w[o] += dw;
#endif
#else
  //dw = alpha[o] * s_d[o] * e;
#ifdef NICSNEWALGO
  W.alpha[o] += Alpha_MUE * W.alpha[o] * W.Dw_t1[o] * e;
  dw = alpha[o] * s_d[0] * e; W.w[o] += dw;
  W.Dw_t1[o] = dw;
#endif
  //      // If LOCAL_ALPHA. Calc alpha.
  //      double bracket = 1+Alpha_MUE*h*e*s_d[o];
  //      if(bracket<Alpha_ROH) bracket=Alpha_ROH;
  //      alpha[o] *= bracket;
  //      // Update w.
  //      double k = alpha[o] * s_d[o];
  //      dw = e * k; W.w[o] += dw;
  //      // Calc h.
  //      bracket = 1 - k * s_d[o]; if(bracket<0) bracket=0;
  //      h = h * bracket + dw;
#endif
#ifdef DO_WEIGHT_STATISTICS
  // Some global statistics.
  WeightMean += fabs(W.w[o]);
  //AlphaMean += W.alpha[o]; AlphaStd += SQR(W.alpha[o]); //WPC++;
#endif
#ifdef RESTRICT_WEIGHT_RANGE
  if(fabs(W.w[o])>RESTRICT_WEIGHT_RANGE) W.w[o]=RESTRICT_WEIGHT_RANGE;
#endif
    } // Order o loop.
  }
  
#ifdef UPDATE_WEIGHTS_AFTER_SEQ  
void ExecuteWeightChange(TWeight &W, bool Order0) {
  // Bias weights are order 0 -> loop just once.
  for(unsigned int o=0;o<ORDER_WEIGHT;Order0 ? o=ORDER_WEIGHT : o++) {
#ifdef MOMENTUM
#ifdef USE_MOMENTUM_CUT
    if(((W.Dw[o]>=0)&&(W.Dw_t1[o]<0)||(W.Dw[o]<=0)&&(W.Dw_t1[o]>0)))
      W.Dw_t1[o]=0;// Cut the momentum when sign changes.
#endif
    W.Dw[o] += MOMENTUM * W.Dw_t1[o]; W.Dw_t1[o] = W.Dw[o];
#endif    
    W.w[o] += W.Dw[o]; W.Dw[o]=0;
  } // Order o loop.
}
#endif

};


#endif // KTB_NEURALNETBASE_H
