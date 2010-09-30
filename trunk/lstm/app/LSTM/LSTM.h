#ifndef KTB_LSTM_H
#define KTB_LSTM_H

#include <assert.h>
#include <sstream>
#include <iostream>
#include <fstream>

#include "neuralNetBase.h"

using namespace std;

//// defines
// switch for all debugging info
#define DEBUG
//#define DEBUG_ONLINE_TO_COUT

#define INNERSTATE_DUMP_LOGFILE "S_Dump.log"
#define GATE_Y_LOGFILE "Gate_y.log"
//#define HIDDEN_Y_LOGFILE "Hidden_y.log"
// Max number of MemoBlocks.
#define MAX_BLOCKS 2
// Max number of cells per block not used. //#define MAX_CELLS 1
//#define NO_IN_OUT_SHORTCUTS
// Peephole connections.#
#define CONNECT_GATES_TO_S
//#define PEEPHOLE_W_TERM
//#define CONNECT_TO_GATES
//#define CONNECT_GATES_OF_SAME_BLOCK
#define CELL_BIAS 
// Hidden units are feed-forward,
// they might see the cells and the input and feed into Out.
// Cells are never connected to hidden units, out units always.
//#define USE_HIDDEN_UNITS
// The next two are topo-fags they should always be set.
//#define CONNECT_HIDDEN2IN 1 
//#define CONNECT_HIDDEN2CELLS 0
// Increase number of input units and copy output back to input.
// For output self-recurrentcy IN_OUT_SHORTCUTS have to be in use.
//#define RECURRENT_OUTPUT
//#define SELF_RECURRENT_OUTPUT
//#define G_SIGMOID
//#define G_TANH
//#define G_RATIONAL_FUNC
#define G_LINEAR
//#define H_SIGMOID
//#define H_TANH
#define H_LINEAR
#define Yfg_SIGMOID 0
//#define Yfg_SHIFTEDPOLY
//#define OUTGATE_ERROR_SPLIT
#define FORGET_GATES

//// classes in this header
class LSTM;

////LSTM
class LSTM : public TNeuralNetBase 
{

public:

  LSTM();
  ~LSTM();
  virtual char Run();

private:

  virtual char LoadPar();
  //
  char InitOrDeleteNet(bool init);
  void InitOrDeleteMemoBlocks(bool Init,
			      unsigned int BegBlock, unsigned int EndBlock);
  //void DeleteNet();
  char AddMemoBlock(int BegSrcBlockNb, int EndSrcBlockNb, unsigned int MemoBlockSize);
  void AddMemoBlockToExistingBlocks(unsigned int BegBlock, 
				    unsigned int EndBlock);
  void ResetNet(bool SaveState, bool RestoreState);
  // For testing and freezing set the blocks for which we need derivatives.
  void ForwardPass(struct TPatData &PatData,
		   unsigned int BegBlockDeriv, unsigned int EndBlockDeriv);
  void BackwardPass(unsigned int BegBlock, unsigned int EndBlock);
#ifdef UPDATE_WEIGHTS_AFTER_SEQ 
  void ExecuteWeightChanges(unsigned int BegBlock, unsigned int EndBlock);
#endif
  void PatStatistics(struct TPatData &PatData);
  void SeqStatistics(struct TPatData &PatData);
  void EpoStatistics(struct TPatData &PatData);
  unsigned int Test();
  void FreeRunTest();
  char WriteWeightFile(char *FileName);
  char WriteWeightLog(); //char *FileName);
  // For display and fileIO.
  char WriteWeightStream(iostream *s, 
			 void(*f)(iostream *s, double &d),
			 bool WriteWhiteSpace, const char WhiteSpaceChar[2]);
  char LoadWeightFile(char *FileName);
  char DumpAll(char *FileName);
  void WriteNetStream(iostream *s, struct TPatData &PatData);
  void DisplayNet(struct TPatData &PatData);
  void DisplayWeights();
  char WriteOutLogFile(char *FileName);
  char Dump_Inner_States(char *FileName, TPatData &PatData);
  char DumpSomeAlphas(); // debugging local learning.
  void TopoCheck();
  void SetTopoStatistics();
  char GetOnlineProblemStatistics();

  // Rename log files after each trail.
  void renameLogFilesAfterTrial( int Tri );

  // Inline functions.
#ifdef USE_HIDDEN_UNITS  
  void  f_Hidden(double x, double &y) { y=2*LOGC(x)-1; }
  double f_d_Hidden(double y, double &dy) { dy=0.5*(1-y*y); return dy; }
#endif
  // G().
#ifdef G_SIGMOID
  void G(double x,double &g) { 
    //if(x>709) { x=709; } //cerr << Tri <<"- x>709 in G()\n"; g=2; return; }
    //else if(x<-709){ x=-709; } //cerr << Tri <<"- x<709 in G()\n"; g=-2; return; }
    g=2*LOGC(x)-1; }
  void G_d(double g,double &gd) { gd=0.5*(1-g*g); }//gd=1-g*g*0.25; }
#endif
#ifdef G_TANH
  void G(double x,double &g) { g=tanh(x); }
 void G_d(double g,double &gd) { gd=(1-g*g); } // tanh_d
#endif
#ifdef G_RATIONAL_FUNC
  double g__frac;
  void G(double x,double &g) { g__frac=1/(1+fabs(x)); g=x*g__frac; }
  void G_d(double &gd) { gd=SQR(g__frac); }
#endif
#ifdef G_LINEAR
  void G(double x,double &g) { g=x; }
  void G_d(double &gd) { gd=1; }
#endif
  // H().
#ifdef H_SIGMOID
  void H(double s,double &h) { 
    if(s>709) { s=709; } //cerr << Tri <<"- s>709 in H()\n"; h=1; return; }
    else if(s<-709){ s=-709; }//cerr << Tri <<"- s<709 in H()\n"; h=-1; return; }
    h=2*LOGC(s)-1; }
  double H_d(double h) { return (0.5*(1-h*h)); }
#endif
#ifdef H_TANH
  void H(double s,double &h) { h=tanh(s); }
  double H_d(double h) { return (1-h*h); } // tanh_d
#endif
#ifdef H_LINEAR
  void H(double s,double &h) { h=s; }
  double H_d() { return 1; }
#endif
  // Yfg
#ifdef Yfg_SHIFTEDPOLY
  double Yfg__frac;
  void Yfg(double x,double &y) { Yfg__frac=1/(1+fabs(x)); y=x*Yfg__frac+1; }
  double Yfg_d(double y,double &df) { df=Yfg__frac*Yfg__frac; return df; }
#endif
#ifdef Yfg_SIGMOID
  void Yfg(double x,double &y) { 
    if(x>709) { x=709; }//cerr << Tri <<"- x>709 in Yfg()\n"; y=1; return; }
    else if(x<-709){ x=-709; }//cerr << Tri <<"- x<709 in Yfg()\n"; y=-1; return; }
    //Strech the sig with Yfg_Sigmoid
    //y= (1+Yfg_SIGMOID) * LOGC(x) - Yfg_SIGMOID; }// y=1/(1+EXP(-x)); }
    y= LOGC(x); }
  double Yfg_d(double y,double &df) {  //y*(1-y);
    //df=1/(Yfg_SIGMOID+1)*(y+Yfg_SIGMOID)*(1-y); 
    df=y*(1-y); 
#ifdef SET_MIN_DERIVATIVE
    if(df<SET_MIN_DERIVATIVE) df=SET_MIN_DERIVATIVE;
#endif
    return df; }
#endif
#ifdef OUTPUT_SIGMOID
  void Output_Sig(double x,double &y) { 
    if(x>709) { x=709; } else if(x<-709){ x=-709; }
    //y=OUTPUT_SIGMOID * LOGC(x); - OUTPUT_SIGMOID*0.5; }
    y=LOGC(x); }
  double Output_Sig_d(double y,double &df) {   
    //df=1/OUTPUT_SIGMOID * ((OUTPUT_SIGMOID*OUTPUT_SIGMOID*0.25)-y*y);
    df=y*(1-y); 
#ifdef SET_MIN_DERIVATIVE
    if(df<SET_MIN_DERIVATIVE) df=SET_MIN_DERIVATIVE;
#endif
    return df; }
#endif
  struct Ts_d_Gate { 
    double s_d[ORDER_WEIGHT];
#ifdef LOCAL_ALPHA
    double alpha[ORDER_WEIGHT];
    //double h; 
#endif
  }; // Here for follow. func..
  void NewTs_d_Gate(Ts_d_Gate *&sd, unsigned int size);
  void NewTs_d_Gate(Ts_d_Gate &sd); // For the biases.
  // Function for the derivative update.
  void Update_dS(double &s_d, unsigned int iB, 
		 double y_t1, double d) {
#ifdef FORGET_GATES
      s_d *= MemoBlock[iB].FgGate.y;
#endif
      s_d += d*y_t1;
      //cerr<<s_d<<" "<<d<<" "<<y_t1<<endl;//ddd
  }
  // For bias connections only order 0.
  void Update_Cell_s_d(double *s_d,unsigned int iB, double *y_t1) {
    for(unsigned int o=0;o<ORDER_WEIGHT;o++)
      Update_dS(s_d[o],iB,y_t1[o],g_d__y_in); }
  void Update_Cell_s_d(double *s_d,unsigned int iB, double y_t1) {
    Update_dS(s_d[0],iB,y_t1,g_d__y_in); }
  void Update_InGate_s_d(double *s_d, unsigned int iB, double *y_t1) {
    for(unsigned int o=0;o<ORDER_WEIGHT;o++)
      Update_dS(s_d[o],iB,y_t1[o],y_in_d__g); }
  void Update_InGate_s_d(double *s_d, unsigned int iB, double y_t1) {
    Update_dS(s_d[0],iB,y_t1,y_in_d__g); }
#ifdef FORGET_GATES
  void Update_FgGate_s_d(double *s_d, unsigned int iB, double *y_t1) {
    for(unsigned int o=0;o<ORDER_WEIGHT;o++)
      Update_dS(s_d[o],iB,y_t1[o],y__fg_d__s_t1); }
  void Update_FgGate_s_d(double *s_d, unsigned int iB, double y_t1) {
    Update_dS(s_d[0],iB,y_t1,y__fg_d__s_t1); }
#endif
  // Two get input functions for security: 
  // if(val>=NbExtIn) tends to cover bugs.
#ifdef RECURRENT_OUTPUT
  double &GetInput(TPatData &PatData, 
		   unsigned int seq, unsigned int pat, unsigned int val){
    if((val>=NbExtIn)) return Out[val-NbExtIn].y; 
    else return ((PatData.SeqList[seq])[pat])[val]; }
#else
  double &GetInput(TPatData &PatData, 
		   unsigned int seq, unsigned int pat, unsigned int val){
    // The following is problem depedent!!!
#ifndef STAT_OUT
    if(ClampOut) return Out[val].y;
#else
    if(ClampOut) {
      ////if(val<NbOut) return Out[val].y;
      ////else return StatOut[val-NbOut].y;  
      return StatOut[val].y;
    }
#endif
    return ((PatData.SeqList[seq])[pat])[val]; }
#endif
  // Fields.
  struct TSrc {
    TWeight *In, **Cell, *s, Bias;   
#ifdef CONNECT_TO_GATES 
    TWeight *InGate, *OutGate;
#endif
#ifdef USE_HIDDEN_UNITS
    TWeight *Hidden;
#endif
  };
  inline void NetInputSum(double &net, TSrc &W,
			  bool Con2In, bool Con2Bias,
			  bool Con2Cell, bool Con2Hidden);

  inline void NetInputPeephole(double &net, TSrc &W, 
				     unsigned int &iB);
  struct TCell 
  {
    double net;
    double y[ORDER_WEIGHT];
    double y_save[ORDER_WEIGHT];
    double y_t1[ORDER_WEIGHT];//s_t1 for the forget gates.
    // Order for s for peepholes
    double g,
      s[ORDER_WEIGHT],s_save[ORDER_WEIGHT],s_t1[ORDER_WEIGHT],h;
    TSrc W;
    //TWeight *w_In, **w_Cell, *w_InGate, *w_OutGate, w_Bias;   
    Ts_d_Gate *s_d_In, **s_d_Cell, *s_d_InGate, *s_d_OutGate, s_d_Bias;
    // Partials for the InGate.
    Ts_d_Gate  *s_d_InGate_In, *s_d_InGate_s, **s_d_InGate_Cell, 
      *s_d_InGate_InGate, *s_d_InGate_OutGate, s_d_InGate_Bias;
#ifdef FORGET_GATES
    Ts_d_Gate  *s_d_FgGate_In,  *s_d_FgGate_s, **s_d_FgGate_Cell, 
      *s_d_FgGate_InGate, *s_d_FgGate_OutGate, s_d_FgGate_Bias;
#endif
    double e_unscaled,e;
  };
  double g_d,g_d__y_in,y_in_d__g,InGate_df,FgGate_df; // For calculations.
#ifdef FORGET_GATES
  double y__fg_d__s_t1;
#endif
  struct TMemoBlock {
    unsigned int MemoBlockSize;
    // For not fully connected growing net.
    // InputBeginBlockNb<= b < InputEndBlockNb (if==-1 -> NbMemoBlocks).
    unsigned int BegSrcBlockNb, EndSrcBlockNb;
    struct {
      double net,y,y_save,y_t1;
      TSrc W;
      //TWeight *w_In, *w_s, **w_Cell, *w_InGate, *w_OutGate, w_Bias; 
    } InGate;
    struct {
      double net,y,y_save,y_t1;
      double e,df,delta;
      TSrc W;
      //TWeight *w_In, *w_s, **w_Cell, *w_InGate, *w_OutGate, w_Bias; 
    } OutGate;
#ifdef FORGET_GATES
    // No connections from the FgGate, so no y_t1 and also no self connection.
    struct {
      double net,y,y_save;//,y_t1;
      TSrc W;
      //TWeight *w_In, *w_s, **w_Cell, *w_InGate, *w_OutGate, w_Bias; 
    } FgGate;
#endif
    TCell *Cell;
  } *MemoBlock;
  struct TOut { 
    double net,y,y_save,e,df,delta;
    TSrc W;
    //TWeight *w_In, **w_Cell, *w_Hidden, w_Bias;
  } *Out;
#ifdef USE_HIDDEN_UNITS
  struct THidden { // BP units.
    double net,y[ORDER_WEIGHT],y_save[ORDER_WEIGHT];
    //,y_t1[ORDER_WEIGHT];
    //double net,y,y_save,
    double e,df,delta;
    TSrc W;
    //TWeight *w_In, **w_Cell, w_Bias;
  } *Hidden;
#endif
//    struct TPredictNextIn { // BP units.
//      double net,y,e,df,delta;
//      TWeight *w_In, *w_InClass, **w_Cell;
//    } *PredictNextIn;
//    struct TNextIn { // Out units for PredictNextIn.
//      double net,y,e,df,delta; 
//      TWeight *w_PredictNextIn;
//    } *NextIn;
//    struct TPredictClass { // BP units.
//      double net,y,e,df,delta;
//      TWeight **w_Cell;
//    } *PredictClass;
//    struct TPredictClassOut { // Out units for PredictClass.
//      double net,y,e,df,delta;
//      TWeight *w_PredictClass;
//    } *PredictClassOut;
  // Parameter for topology.
  // for RECURRENT_OUTPUT
  bool CellBias, IOShortcuts;
  unsigned int NbExtIn;
  unsigned int NbPredictNextIn;
  unsigned int NbPredictClass;
  unsigned int NbPredictOut; // To calculate MSE.
  // Use cell-cell, gate-to-cell  connection inside block.
  bool InternalBlockConect;
  unsigned int NbMemoBlocks; // The block size is stored in TMemoBlock.
  unsigned int FreezeEndBlock; // Lower Block number are frozen.
  // LSTM statistics.
  unsigned int NbCells, NbMemUnits;
  // debugging.
  //double ddd;
};

inline void LSTM::NetInputSum(double &net, TSrc &W,
			       bool Con2In, bool Con2Bias,
			       bool Con2Cell, bool Con2Hidden) {
  // Peepholes not included.
  if(Con2Bias) net = W.Bias.w[0]; else net=0; // From Bias.
  for(unsigned int o=0;o<ORDER_WEIGHT;o++) {
    if(Con2In) for(unsigned int iI=0;iI<NbIn;iI++) // From Input.
      net += W.In[iI].w[o] * CurInput[iI][o];
    //  for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
    //  iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) {
    for(unsigned int iiB=0;iiB<NbMemoBlocks;iiB++) {
#ifdef CONNECT_TO_GATES
      // ORDER_WEIGHT is yet missing here.
      net += W.InGate[iiB].w[o] * MemoBlock[iiB].InGate.y; // From InGate.
      net += W.OutGate[iiB].w[o] * MemoBlock[iiB].OutGate.y; // From OutGate.
#endif
      // From Cells.
      if(Con2Cell)
	for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++)
	  net += W.Cell[iiB][iiC].w[o] * MemoBlock[iiB].Cell[iiC].y[o]; 
    }
#ifdef USE_HIDDEN_UNITS
    if(Con2Hidden)
      for(unsigned int iH=0;iH<NbHidden;iH++)
	net += W.Hidden[iH].w[o] * Hidden[iH].y[o];
#endif
  }// End order loop.
}

inline void LSTM::NetInputPeephole(double &net, TSrc &W, unsigned int &iB) {
  for(unsigned int o=0;o<ORDER_WEIGHT;o++) {
  for(unsigned int iiC=0;iiC<MemoBlock[iB].MemoBlockSize;iiC++) // From s.
    net += W.s[iiC].w[o] * MemoBlock[iB].Cell[iiC].s[o];
  }
}

//  inline void LSTM::dS_UpdateLoop() {
//  }



#endif // KTB_LSTM_H

