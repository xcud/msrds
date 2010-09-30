#ifndef KTB_PATTERN_MANAGEMENT_H
#define KTB_PATTERN_MANAGEMENT_H

//#include <ctype>
#include <sstream>
#include <time.h>

#include "../base/MathMacros.h"
#include "../base/IOBase.h"

/*** defines ***/

// switch for all debugging info
//#define DEBUG
// file names
#define TRAINPATFILE "Train.pat"
#define TESTPATFILE  "Test.pat"
#define RESULTTRAINPATFILE "ResultTrain.pat"
#define RESULTTESTPATFILE "ResultTest.pat"

/*** classes in this header ***/
class TPatternManager;
class TPatternGenerator;

///////////////////TPatternManager
// a pattern has input output lists for one time step
// a sequece is a list of pattern
// TPatternManager manages a list of sequences
// the number of inputs and outputs are constant for all 
// pattern in all sequence,
// it has to be the same as the number of in- and output units in the net
// In the pattern file sequences are devided by "\n\n", pattern by "\n".
// and in-out values by "\t" or ' '. No global parameters. The number of
// values in a pattern must be constant for the whole file.
class TPatternManager : public TIOBase {
public:
  TPatternManager();
  ~TPatternManager();
  char cTrainPatFilename[256]; // training pattern
  char cTestPatFilename[256];  // test pattern
  char cTrainPatFilenameResult[256]; // mixed training pattern
  char cTestPatFilenameResult[256]; // mixed training pattern
  char LoadTrainPattern();
  char LoadTestPattern();
  // One class one Output classification.
  char InitPatternClassification(unsigned int NbIn, unsigned int NbOut);
  char SaveTrainPattern(); // for debugging and Id'tk
  char SaveTestPattern(); // for debugging and Id'tk
  void MixTrainSequences();
  // The PatData is public to give the UI direct access for faster dispaly.
  struct TPatData { // at least one for train and one for test
    unsigned int  NbSeq;
    unsigned int* NbPat; // number of pattern in each sequence[NbSeq]
    unsigned int  NbPatTotal;
    unsigned int* NbClass; // classification of sequence[NbSeq]
    unsigned int  NbVal; // number of values in each pattern
    double*** SeqList;//[NbSeq][NbPat[NbSeq]][NbVal] set NULL in constructor
    unsigned int Generalize; // 0 for train, form par for test.
    double Mean,Variance,StdDev;
  } TrainData, TestData;
protected:
  void CountNbPatTotal(struct TPatData &apsPatData);
  // If the *NbPat == NULL take the const lenght 
  char NewPatData(struct TPatData &apsPatData, unsigned int ConstNbPat);
  // For destructor and to reload pattern.
  void DeletePattern(struct TPatData &apsPatData, unsigned int ConstNbPat);
  char InitPatternClassification(struct TPatData &apsPatData,
				 unsigned int NbIn, 
				 unsigned int NbOut);  
  void CopyPatData(struct TPatData &apsPatDataDst, 
		   struct TPatData &apsPatDataSrc);
  void MixSequences(struct TPatData &apsPatData);
  // Transfrom ALL= Test and Train data.
  void TransPat2StepByStepPredict(unsigned int NbInUnits,
				     unsigned int NbOutUnits,
				     unsigned int NbPredictInputs,
				     unsigned int offset,
				     unsigned int PredictAllUntilOffset);
  void TransPat2StepByStepPredict(unsigned int NbInUnits,
				  unsigned int NbOutUnits,
				  unsigned int NbPredictInputs,
				  struct TPatData &apsPatData,
				  unsigned int offset,
				  unsigned int PredictAllUntilOffset);
  void TransPat2StepBySteplassi(unsigned int NbInUnits,
				   unsigned int NbOutUnits,
				   unsigned int PredictAllUntilOffset);
  void TransPat2StepByStepClassi(unsigned int NbInUnits,
			     struct TPatData &apsPatData);
  // Add IO diff to pat.
  void Add_IODiff(double factor); // ALL: test and train.
  void Add_IODiff(double factor, struct TPatData &apsPatData);
  // Scale IODiff in !!val[valNb]!! between +-max, Train set is reference.
  double Scale_IODIff(double max, unsigned int valNb);
  void Add_IODiff2Input();// ALL: test and train.
  void Add_IODiff2Input(struct TPatData &apsPatData);
  void Trans2IODiff(double factor); // ALL: test and train.
  void Trans2IODiff(double factor, struct TPatData &apsPatData);
  void FlipO1O2(unsigned int val1, unsigned int val2);//ALL:test and train.
  void FlipO1O2(struct TPatData &apsPatData,
		unsigned int val1, unsigned int val2);
  // Split into muliple sequneces with different offsets.
  void SplitPredictionOffset(unsigned int offset);
  void SplitPredictionOffset(unsigned int offset,
			     struct TPatData &apsPatData);
  //void TransPat2UseContextNet(unsigned int NbInUnits,
  //		      unsigned int NbOutUnits) {}
  void Add_TimeWindow(unsigned int InputDim, 
		      unsigned int Size, unsigned int StepSize);
  void Add_TimeWindow(unsigned int InputDim, 
		      unsigned int Size, unsigned int StepSize,
		      struct TPatData &apsPatData);
  char SavePatternFileSeppFormat(struct TPatData &apsPatData, 
				 char* acPatFilename, 
				 unsigned int NbInUnits);
  void CalcPatStatisticsAll(unsigned int NbInUnits,
			    unsigned int NbOutUnits);
  // Logging and debugging temporary variables.
  char cBuf[256];
private:
  char LoadPattern(struct TPatData &apsPatData, char* acPatFilename);
  char SavePatternFile(struct TPatData &apsPatData, char* acPatFilename);
  void CalcPatStatistics(struct TPatData &apsPatData,
			 unsigned int NbInUnits,
			 unsigned int NbOutUnits);
};

///////////////////TPatternGenerator
// Collection of problem. Replaces TPatternManger for applications that
// that need data generation.
class TPatternGenerator : public TPatternManager {
public:
  TPatternGenerator();
  ~TPatternGenerator();
  void Init_Rand_Seed();
  void GeneratePattern(struct TPatData &apsPatData, const char* FunktionName);
  void Set_LoopAllSeqs(unsigned int ui) { NbLoopAllSeqs = ui; }
  // e.g. set the TrainData as reference to generate TestData (NULL to unset)
  void Set_RefData(TPatData *apsPatData, bool TrainNotTest, 
		   bool RandomSample, bool Restart); 
protected:
  void New_OnlinePatData(struct TPatData &apsPatData);
  // For Online pat by pat.
  void PatByPat_Reber(double out); // works on the train data.
  // We set the sizes of the PatData in the functions that generate them.
  void LongVeryNoisyTimesSeries(struct TPatData &apsPatData);
  void TempOrderEx6(struct TPatData &apsPatData);
  void TempXOR(struct TPatData &apsPatData);
  void Adding(struct TPatData &apsPatData);
  void Laufband(struct TPatData &apsPatData);
  void Sinus(struct TPatData &apsPatData);
  void SpaceToTime(struct TPatData &apsPatData);
  // Taskt the load data and then generate something else with it.
  void Protein2D(struct TPatData &apsPatData);
  void TimerClass(struct TPatData &apsPatData);
  void TimerClassMulti(struct TPatData &apsPatData);
  void AnBn(struct TPatData &apsPatData);
  void AnBnCn(struct TPatData &apsPatData);
  void a_nb_mB_mA_n(struct TPatData &apsPatData);
  // void Osci_Class(struct TPatData &apsPatData);
  // Online problems that generate one pattern or sequence at a time. 
  unsigned int MaxOnlineStreamLengthTrain,
    MaxOnlineStreamLengthTest; // target for online Pat generattion.
  unsigned int PatCount;// For online pat generation statistcs.
  // To delete correctly using dynamic NbPat:
  unsigned int MaxNbPat, MaxNbPatTrain;
  void Online_TempOrderEx6(struct TPatData &apsPatData);
  void Online_ReberGrammar(struct TPatData &apsPatData,
			   bool SetBackToSeqStart);
  // For on-line production, protected to reset the net.
  bool ForONLINE; // If online method uses (calls) generation method.
  char ReberGrammarState; 
  char EmbeddedReberGrammarState; 
  void Online_SlidingNBitMemory(struct TPatData &apsPatData,
				bool SetBackToSeqStart);
  void Online_AddMulti(struct TPatData &apsPatData,
				bool SetBackToSeqStart);
  void Online_Timer(struct TPatData &apsPatData,
				bool SetBackToSeqStart);
  void Online_TimerClass(struct TPatData &apsPatData,
				bool SetBackToSeqStart);
  void Online_Oscillator(struct TPatData &apsPatData,
				bool SetBackToSeqStart);
  void Online_OscillatorClass(struct TPatData &apsPatData,
				bool SetBackToSeqStart);
  void Online_AnBn(struct TPatData &apsPatData,
				bool SetBackToSeqStart);
  // Save the loaded data then transform it, etc.
  // Used for Protein2D.
  TPatData TrainDataOrg, TestDataOrg; 
private:
  // Used for Exp 6 and Protein2D.
  struct TRefData { 
    TPatData *PatData;
    bool TrainNotTest, RandomSample, Restart; // For Protein2D.
    unsigned int SampleSeq, SamplePat; // Seq=protein, pat=AA.
  } RefData;
  double AddMultiVal, AddMultiLastVal;
#define SlidingNBitMemorySize 4
  double SlidingNBitMemory[SlidingNBitMemorySize];
  char SlidingNBitMemoryPointer;
  // variables protected to log the Temp order (Exp 6).
  unsigned int t1,t2,t3,t4,tick;
  unsigned int XNotYt1,XNotYt2,XNotYt3,XNotYt4; 
  unsigned int NbLoopAllSeqs;
  // The next possible outputs.
  enum ReberGrammarSymbol {B,T,P,S,X,V,E} ReberGrammarSymbol_p1;
};


#endif // KTB_PATTERN_MANAGEMENT_H
