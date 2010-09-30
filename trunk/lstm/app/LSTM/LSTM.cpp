#include <stdlib.h>
#include <stdio.h>

#include <sstream> // stringstream
#include <iostream>
#include <fstream>
#include <iomanip>

//#include <new> // for memory error handling
//#include <ctype.h>
//#include <unistd.h> // sleep 
//#include <termios.h> // struct termios
#include <fcntl.h>
//#include <sys/param.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <float.h> // To have max values for imposible inits.

#include "LSTM.h"
#include "Error.h"

#define lrand48 rand

//// LSTM
LSTM::LSTM() 
{ 
  MemoBlock=NULL;
}

LSTM::~LSTM() 
{
  // Delete the MemoBlocks newed in LoadPar.
  delete[] MemoBlock; MemoBlock=NULL;
}

void LSTM::NewTs_d_Gate(Ts_d_Gate *&sd, unsigned int size) 
{
  sd = new Ts_d_Gate[size];
  for(unsigned int i=0;i<size;i++) {
    for(unsigned int o=0;o<ORDER_WEIGHT;o++) {
      sd[i].s_d[o] = 0;
#ifdef LOCAL_ALPHA
      sd[i].alpha[o] = AlphaBase;
      //    sd[i].h=0;
#endif
    }
  }
}

void LSTM::NewTs_d_Gate(Ts_d_Gate &sd) {
  Ts_d_Gate *psdTmp; NewTs_d_Gate(psdTmp,1);
  for(unsigned int o=0;o<ORDER_WEIGHT;o++) {
    sd.s_d[0]  = psdTmp[0].s_d[o];
#ifdef LOCAL_ALPHA
    sd.alpha[o] = psdTmp[0].alpha[o];
    //  sd.h     = psdTmp[0].h;
#endif
  }
  delete[] psdTmp;
}


char LSTM::Run() 
{
  cout << "LSTM::Run ..." << endl;

  //NeuralNetBase Init, also reads first part of par file.
  if( Init() ) { return 1; }

  // Trial loop.
  for( Tri=0; Tri < NbTrials; Tri++ ) 
    { 
      InitOrDeleteNet(true); // Also re-read complete par file.

      // Epo loop.
      do 
	{ 
	  Epo++;
	  // Skip rest of trial if matherr (nan) occured.
	  if( errno ) { continue; }
	  start_ctime=clock(); //ddd
	  //time_t  TimeNow = time(NULL);//ddd
	  //cout <<"Epo:"<<Epo
	  //<<" Train...time now:"<<ctime(&TimeNow); //ddd
#ifdef DO_ONLINE_PAT_PRODUCTION
	  // This Seq will be overwritten, 
	  // we init the Nb variables here.
      ONLINE_PAT_FUNCNAME(TrainData, true);
#else 
#ifdef PATTERN_GENERATION_FROM_DATA_SEQWISE
      // For Protein2D task to know how to sample from which data set.
      if(Generate_Pat) { Set_RefData(&TrainDataOrg,true,true,true); }
#else
      // This Seq will be overwritten, we init the Nb variables here.
      if(Generate_Pat && (Generate_Pat_Each_Epoch>0))
	{
	  if((unsigned int)(Epo % Generate_Pat_Each_Epoch) == 0)
	    {
	      GeneratePattern(TrainData, Pat_FuncName);
	    }
	}

      if( MixTrainSeqs ) { MixTrainSequences(); }
#endif
#endif

      //Clamping duration for iterated prediction.
      if((unsigned int)( Epo % AverageTrainBuf) == 0) 
	{
	  if(MeanPatCorrectTrain)	
	    MeanPatCorrectTrain/=MeanPatCorrectTrainCount;
	  else MeanPatCorrectTrain=0;
	  DebugFile<<Epo<<" "
		   <<MinPatCorrectTrain<<" "
		   <<MeanPatCorrectTrain<<" "
		   <<MaxPatCorrectTrain<<" "
		   <<endl; //ddd
	  MaxPatCorrectTrain=0; MinPatCorrectTrain=DBL_MAX; 
	  MeanPatCorrectTrain=0; MeanPatCorrectTrainCount=0; 
	}

      //
      ClassesWrong=0; MSEEpo=0; PatCorrect=0; PatCount=0;
      SeqOnline=0; SeqOnlinePatCount=0; AlphaDecayInSeq=1;
      // Seq loop.
      for(Seq=0;Seq<TrainData.NbSeq;FreezeSeqLoop ? Seq=0 : Seq++) {
	MSESeq=0; PatWrong=0; ClampOut=false; Clamping=false; 
	ClampPatStart=0; ClampPatStop=0;

	// Reset the network.
#ifndef DO_ONLINE_PAT_PRODUCTION
 	ResetNet(0,0);
#else
	if(!SeqOnline) ResetNet(0,0); //For sequential online
#endif

	// Generate a new seq (or not).
#ifdef DO_ONLINE_PAT_PRODUCTION
#if defined(NO_RESET_AFTER_ONLINE_SEQ) || defined(SETSTEPTARGET_BUT_COUNT_SEQUENCEWISE)
	if(SeqOnline) ONLINE_PAT_FUNCNAME(TrainData, false);
	else ONLINE_PAT_FUNCNAME(TrainData, true);
#else
	ONLINE_PAT_FUNCNAME(TrainData, true);
#endif
#else
#ifdef PATTERN_GENERATION_FROM_DATA_SEQWISE
	if(Generate_Pat) 
	  {
	    GeneratePattern(TrainData, Pat_FuncName);
	    // Fix NbPat of actual seq, 
	    // because TPatternManager does not know seq.
	    TrainData.NbPat[Seq] = TrainData.NbPat[0];
	}
#endif
#endif
	//	if(PatCorrectTest>10)
	//	  {cout<< "\n";cout.flush();}//Debug on-line.
	//DumpAll("dump.log.init");
	//DisplayNet(TrainData); DisplayWeights(); KeyCheck();
	//AlphaPredict = Alpha / TrainData.NbPat[Seq]; // AlphaPredict
	if(RandomPrediction) 
	  {
	    RandomPredictionStart=
	      (unsigned int)fmod((double)lrand48(),RandomPrediction);
	  }
	
	// Pattern loop.
	for( Pat = RandomPredictionStart; 
	     Pat < TrainData.NbPat[Seq]; Pat++ ) 
	  { 
	    // DisplayNet(TrainData); //ddd
	    // DisplayWeights(); KeyCheck();//ddd

#ifdef CLAMPING
	    // Stop train clamp when Pat errro to big 
	    // (assuming NbOut==1).
#ifdef STAT_OUT
	    if((MaxMSE && Clamping && (fabs(StatOut[0].e)>MaxMSE)) ||
	       (Pat==ClampPatStop && Clamping) ) {
	      if(Pat-ClampPatStart>MaxPatCorrectTrain)
		MaxPatCorrectTrain=Pat-ClampPatStart;
	      if(Pat-ClampPatStart<MinPatCorrectTrain)
		MinPatCorrectTrain=Pat-ClampPatStart;
	      MeanPatCorrectTrain+=Pat-ClampPatStart;
	      MeanPatCorrectTrainCount++;
	    Pat=ClampPatStop; 
	    }
#else	
	    if(MaxMSE && Clamping && (fabs(Out[0].e)>MaxMSE) ) {
	      Pat=ClampPatStop; 
	    }
#endif
	  // Predict ahead (clamp) steps.
	    if(ClampOutTrainPredictSteps && ClampOutPredictSteps) {
	      if(!Clamping) {
		Clamping=true; // Once with external input.
		ClampPatStart=Pat; // Set Start and Stop.
		ClampPatStop=
		  Pat + ClampOutTrainPredictSteps*PredictionOffset;
	    if(ClampPatStop>TrainData.NbPat[Seq]) {//Train set ends.
	      ClampPatStop=TrainData.NbPat[Seq]; }
	  } else if(!ClampOut) { // Second pass: Clamp now.
	    // Clamping already or start now.
	    ResetNet(1,0); //Save state.
	    ClampOut=true;
	  }
	  if(ClampOut || Clamping) {
	    if(Pat>=ClampPatStop) { // Relieve clamp and set back.
	      Clamping=false; ClampOut=false;
	      ResetNet(0,1); // Restore state.
	      Pat=ClampPatStart;// Set back.
	      continue; // finish and restart loop;
	    }
	  }
	}
#endif
	
	  // ForwardPass.
	  if(Reverse_Input) {
	    ResetNet(0,0); //Reset before each Pat.
	    PatReInStart=Pat;
	    if(Pat>Reverse_Input) PatReInStop=Pat-Reverse_Input;
	    else PatReInStop=0;
	    // Pat is unsigned!!
	    for(;(Pat>=PatReInStop)&&(Pat<=PatReInStart);Pat--) {
	      ForwardPass(TrainData,FreezeEndBlock,NbMemoBlocks);
	      //  	      cout<<"Train "<<Epo <<"-" //ddd
	      //  		  <<Seq<<"-"<<SeqOnline<<"-"<<Pat<<" :";
	      //  	      cout<<"["<<TrainData.NbPat[Seq]<< "] ";
	      //  	      for(unsigned int Val=0;Val<TrainData.NbVal;Val++) 
	      //            	    cout<<((TrainData.SeqList[Seq])[Pat])[Val]<<"=";
	      //  	      //<<GetInput(TrainData,Seq,Pat,Val)<<" ";
	      //  	      //cout<<" = "<<TrainData.NbClass[Seq];
	      //  	      cout<<" -> ("<<MSEPat<<") "<<PatCorrect;
	      //  	      for(unsigned int iO=0;iO<NbOut;iO++)
	      //  		cout<<"("<<Out[iO].y<<"->"<< Out[iO].e<<")";
	      //  	      cout<<"\n"; cout.flush();
	    }
	    Pat=PatReInStart;
	  } else {
	    ForwardPass(TrainData,FreezeEndBlock,NbMemoBlocks); 
	  }
	  //        if(PatCorrectTest>10) { // Debug on-line pat generation. // ddd
//   	  cout<<"Train "<<Epo <<"-"<<Seq<<"-"<<SeqOnline<<"-"<<Pat<<" :";
//  	  cout<<"["<<TrainData.NbPat[Seq]<< "] ";
//  	  for(unsigned int Val=0;Val<TrainData.NbVal;Val++) 
//  	    cout<<((TrainData.SeqList[Seq])[Pat])[Val]<<"=";
//  	  //<<GetInput(TrainData,Seq,Pat,Val)<<" ";
//  	  //cout<<" = "<<TrainData.NbClass[Seq];
//  	  cout<<" -> ("<<MSEPat<<") "<<PatCorrect;
//  	  for(unsigned int iO=0;iO<NbOut;iO++)
//  	    cout<<"("<<Out[iO].y<<"->"<< Out[iO].e<<")";
//  	  cout<<"\n"; cout.flush();
	  //	}
	  // Do BackwardPass if................................
#ifdef LOCAL_ALPHA_SEQUENCEWISE
	  BackwardPass(FreezeEndBlock,NbMemoBlocks);
#endif
	  if( ((Pat==TrainData.NbPat[Seq]-1) || SetStepTarget || 
	       AllTimeTarget || 
	       (NbPredictNextIn>0) || (NbPredictClass>0) )
#ifndef TARGET_777
	      && (((TrainData.SeqList[Seq])[Pat])[NbExtIn]!=-777)
#endif
	      ){
#ifdef ALPHA_DECAY_IN_SEQ
	    if(SetStepTarget) {
#ifdef ALPHA_DECAY_IN_SEQ_LINEAR
	      AlphaDecayInSeq = ALPHA_DECAY_IN_SEQ/
		(Pat+ALPHA_DECAY_IN_SEQ);
#elif ALPHA_DECAY_IN_SEQ
	      //if(Pat*ALPHA_DECAY_IN_SEQ>709) AlphaDecayInSeq=0;
	      //else AlphaDecayInSeq = EXP(-(Pat*ALPHA_DECAY_IN_SEQ));
	      AlphaDecayInSeq*=0.5;
#endif
	    } else {
#ifdef ALPHA_DECAY_IN_SEQ_LINEAR
	      AlphaDecayInSeq = ALPHA_DECAY_IN_SEQ/
		(SeqOnline+ALPHA_DECAY_IN_SEQ);
#elif ALPHA_DECAY_IN_SEQ
	      if(SeqOnline*ALPHA_DECAY_IN_SEQ>709) AlphaDecayInSeq=0;
	      else AlphaDecayInSeq =
		     EXP(-(SeqOnline*ALPHA_DECAY_IN_SEQ)); 
#endif
	    }
	    //     cout << Pat << "-"  << Seq <<  "-" << Epo << ":" 
	    // 	 << AlphaDecayInSeq << " ";
#endif
#ifndef LOCAL_ALPHA_SEQUENCEWISE
	    BackwardPass(FreezeEndBlock,NbMemoBlocks);
#endif
	    if(!AllTimeTarget || (Pat==TrainData.NbPat[Seq]-1))
	      if(!ClampOutTrainPredictSteps || 
		 // (Pat+PredictionOffset>=ClampPatStop))
		 (Pat==ClampPatStart))//Log SingleStep prediction.
		PatStatistics(TrainData);
#ifdef DO_ONLINE_PAT_PRODUCTION
	    if(SetStepTarget) {
#ifndef SETSTEPTARGET_BUT_COUNT_SEQUENCEWISE
	      if(!PatWrong) { 
		ONLINE_PAT_FUNCNAME(TrainData, false); 
	      } else break; // PatWrong.
#else
	      if(!PatWrong) { 
		; // Generate seq only before Pat loop.
	      } else break; // PatWrong.
#endif
	      //if(ReberGrammarState==-1) ResetNet(0,0);
	    }
#endif
	  } // end if go through back pass.

	  // Do some logging.
	  if( Epo == MaxEpochs && 0) // or not. 
	    if( Pat < 1000 )
	      Dump_Inner_States("Train", TrainData);

	  //WriteOutLogFile(OUT_LOGFILE);
	  //
	  //
	  //sprintf(cBuf, "w.Pat.%d",Pat); WriteWeightFile(cBuf); exit(0);
	  //sprintf(cBuf, "dump.log.Pat.%d",Pat);DumpAll(cBuf);
	  //Display after the back pass. //ddd
	  //DisplayNet(TrainData); 
	  //DisplayWeights(); 
	  //KeyCheck();//ddd
	  //if(cKey=='w') { DisplayWeights(); KeyCheck(); }
	  //if(cKey=='d'){ Dump_Inner_States("Train",TrainData);KeyCheck();}
	  //if((Pat==0 || Pat==1)&&(Epo==0 || Epo==1) )
	  //if(PatCorrectTest>90) 
	    //Dump_Inner_States("Train", TrainData);//ddd
	  //{ sprintf(cBuf, "dump.log.%d.%d",Epo,Pat);DumpAll(cBuf); }
	  ////ddd
//  	    //        if(PatCorrectTest>10) {
//    	    cout<<"Train "<<Epo <<"-"<<Seq<<"-"<<SeqOnline<<"-"<<Pat<<" :";
//    	    cout<<"["<<TrainData.NbPat[0]<< "] ";
//    	    for(unsigned int Val=0;Val<TrainData.NbVal;Val++) 
//    	      cout<<((TrainData.SeqList[Seq])[Pat])[Val]<<"=";
//    	    //<<GetInput(TrainData,Seq,Pat,Val)<<" ";
//    	    //cout<<" = "<<TrainData.NbClass[Seq];
//    	    cout<<" -> ("<<MSEPat<<") "<<PatCorrect;
//    	    for(unsigned int iO=0;iO<NbOut;iO++)
//    	      cout<<"("<<Out[iO].y<<"->"<< Out[iO].e<<")";
//    	    cout<<"\n"; cout.flush();
	    // 	  }
	  ////ddd
	  
#ifdef UPDATE_WEIGHTS_AFTER_SEQ
	if((update_weights_after_seq &&
	    ((double)((unsigned int) (Pat/update_weights_after_seq))*
	     update_weights_after_seq==Pat) ) ) {
	    ExecuteWeightChanges(FreezeEndBlock,NbMemoBlocks); 
	    // Set next stochastic update time.
	    update_weights_after_seq=UPDATE_WEIGHTS_AFTER_SEQ+
	      (unsigned int)fmod((double)lrand48(),
				 UPDATE_WEIGHTS_AFTER_SEQ);
	}
#endif
	} // End Pat loop.

#ifdef UPDATE_WEIGHTS_AFTER_SEQ
	// Update weights in any case at the end of a seq.
	    ExecuteWeightChanges(FreezeEndBlock,NbMemoBlocks);
#endif
	SeqStatistics(TrainData);
	//sprintf(cBuf, "w.Seq.%d",Seq); WriteWeightFile(cBuf); exit(0);
	//DisplayNet(TrainData); DisplayWeights(); KeyCheck();
#ifdef DO_ONLINE_PAT_PRODUCTION
#ifndef SETSTEPTARGET_BUT_COUNT_SEQUENCEWISE
	if(!SetStepTarget)
#endif
	  if(!PatWrong) { 
	    SeqOnline++; 
	    if(PatCorrect>=MaxOnlineStreamLengthTrain) break; 
	  } else break;
#endif
      } // End Seq loop. 

      //DumpSomeAlphas();
      // The last test is the final one if every epoch is tested.
      if( ( TestEach_Epo==1 ) && ( Epo==MaxEpochs) )
	{ 
	  OutputDebug=true;
	} 
      else 
	{ 
	  OutputDebug=false; 
	}

      EpoStatistics(TrainData);

      //ExecuteWeightChanges(FreezeEndBlock,NbMemoBlocks);
      //sprintf(cBuf, "w.Epo.%d",Epo); WriteWeightFile(cBuf);
      //sprintf(cBuf, "%s.%d", OUT_LOGFILE,Epo); rename(OUT_LOGFILE,cBuf);
      //if(Epo==7384) DumpAll("dump.log.end.log");
      //if((unsigned int)fmod(Epo,1000) == 0) {
      //sprintf(cBuf,"%s%d.%d",FINAL_W_LOGFILE,Tri,Epo);
      //WriteWeightFile(cBuf);}
      if(StopLern) EpochsAfterLearned--;
#ifdef DO_ONLINE_PAT_PRODUCTION
      //if((Epo>=MaxEpochs)||(PatCorrect>=MaxOnlineStreamLengthTrain)
      // ||(MSEEpo<MSEStop)) StopLern=true;
      // StopLearn when the taing set is leerand -> test.
      //if(StopLern && (Epo<MaxEpochs)) {
      //if(LastTestEpo+TestMaxEach_Epo<=Epo) // Not test too much.
      //  Test(); // If not just tested during the EpoStatistics.
      // Here StopLern only then trainig requirements met..
      //StopLern=((PatCorrectTest>=MaxOnlineStreamLengthTest) || 
      //	  (TestMSEEpo<MSEStop));
      //}
      // StopLearn also when the test is learnd but the trainig not.
      // The first or is for the case that we give up (MaxEpochs).
      // Test results may come from test during EpoStatistics.
      StopLern=((Epo>=MaxEpochs)||(TestMSEEpo<MSEStop));
      if(!NoClassesOnlyPredict) 
	StopLearn=StopLearn || (PatCorrectTest>=MaxOnlineStreamLengthTest);
      //       cout<<" StopLern:"<<StopLern //ddd
      //  	  <<" PatCorrect :"<<PatCorrect
      //  	  <<" PatCorrectTest :"<<PatCorrectTest
      //  	  <<endl;
#else
      if((Epo>=MaxEpochs)||(MSEEpo<MSEStop)) StopLern=true;
      if(!NoClassesOnlyPredict) if(!ClassesWrong)  StopLern=true;
      // Test when Traing set learned..
      //       if(LastTestEpo+TestMaxEach_Epo<=Epo) { // Not test too much
      // 	if(StopLern&&(Epo<MaxEpochs)) StopLern = !Test();
      //       } else StopLern = false;
#endif
      if((double)((unsigned int)
		  (Epo/AverageTrainBuf))*AverageTrainBuf==Epo) 
	{
	  //finish_ctime=clock(); // ddd . See how fast
	  //finish_time=time(NULL); // ddd
	  //cout<<"Train"<<" Tri:"<<Tri<<" Epo:"<<Epo<<" sec:"
	  //    <<((double)(finish_ctime-start_ctime)/CLOCKS_PER_SEC)
	  //    <<" total sec:"<<difftime(finish_time,start_time)
	  //    <<endl;//ddd
	  //WriteWeightLog(); //ddd !!! Discspace
	}
    } while (!StopLern || (EpochsAfterLearned>0));// End Epo loop.
    //if(Epo>=MaxEpochs) { Test();}// Final test if unsolved.
    if(TestEach_Epo!=1) {
      if(Epo>=MaxEpochs) {OutputDebug=true; Test();}// ddd
      if(Epo<MaxEpochs) {OutputDebug=true; Test();}//ddd
    }
    //if(!TimeWindowSize) FreeRunTest();///ddd
    OutputDebug=false; //ddd
#ifdef BUFFER_EPO_LOG
    // Flush the buffers to files.
    if(TrainMSEEpoLogBuf.NbVal) 
      FlushEpoLogBuf(MSELOGFILE, TrainMSEEpoLogBuf);
    if(!NoClassesOnlyPredict) {
      if(TrainErrEpoLogBuf.NbVal) 
	FlushEpoLogBuf(ERRORLOGFILE, TrainErrEpoLogBuf);
      if(TestErrEpoLogBuf.NbVal) 
	FlushEpoLogBuf(TEST_ERRORLOGFILE, TestErrEpoLogBuf);
    }
    if(TestMSEEpoLogBuf.NbVal) 
      FlushEpoLogBuf(TEST_MSELOGFILE, TestMSEEpoLogBuf);
#endif
    sprintf(cBuf, "%s%d.%d",FINAL_W_LOGFILE,Tri,Epo); 
    WriteWeightFile(cBuf);

    renameLogFilesAfterTrial( Tri );

    InitOrDeleteNet(false);

  } // End Trial loop.

  // Output running time.
  finish_time= time(NULL);
  double sec,min,h;
  sec= difftime(finish_time,start_time); 
  min=floor(sec/60); h=floor(min/60); 
  cout << "\nLSTM Done. Run time: "
       << sec << " sec = "
       << min << " min = "
       << h << " h "
       << endl;

  return 0;
}



char LSTM::LoadPar() 
{
#ifdef DEBUG
  cout<<"LSTM::LoadPar......."<<endl;//ddd
#endif
  char s[256];
  char dummy[256];
  int TmpEndSrcBlockNb; // To have the sign.

  int errStatus = 0;
  fstream pF; // = new ifstream(); 
  sprintf(cBuf,PARFILE);
  if (OpenFile(&pF,cBuf,ios::in)) return 1;
  if (ReadComment(&pF)) return 1;
  // Scan for the starting point of the LSTM part in the parameter file.
//    pF.scan("NbPredictNextIn:"); 
//    while(!pF.good()) { 
//      pF.seekg(1, pF.cur);
//      if(pF.eof()) { cout << "eof\n"; return 1;}
//      pF.clear();    
//      pF.scan("NbPredictNextIn:"); // First parameter of LSTM.
//    }
  while(strcmp("NbPredictNextIn:",s)&&!pF.eof()) pF>>s;
  if( pF.eof() ) 
    { 
      cerr << "eof in LSTM.par\n"; 
      return 1; 
    }
  pF>> NbPredictNextIn;
  //
  errStatus = loadOnePara( "NbPredictClass:", pF, 
			   NbPredictClass );
  errStatus = loadOnePara( "InternalBlockConect:", pF,
			   InternalBlockConect );
  errStatus = loadOnePara( "NbMemoBlocks:", pF,
			   NbMemoBlocks );
  if(NbMemoBlocks>MAX_BLOCKS) { 
    cerr << "NbMemoBlocks>MAX_BLOCKS\n"; exit(1); }
  //To assign sizes MemoBlock newed here, deleted in ~LSTM.
  if(!MemoBlock) MemoBlock = new TMemoBlock[NbMemoBlocks];
  //MemoBlock = new TMemoBlock[MAX_BLOCKS];
  errStatus = loadOnePara( "# parameters for each memo block", pF, 
			   dummy );
  // Compensate that parameter assumed one word.
  pF.ignore(1000, '\n'); 
  for(unsigned int iB=0;iB<NbMemoBlocks;iB++) 
    {
      errStatus = loadOnePara( "# memo block number", pF, dummy ); 
      // Compensate that parameter assumed one word.
      pF.ignore(1000, '\n'); 
      errStatus = loadOnePara( "MemoBlockSize:", pF, 
			       MemoBlock[iB].MemoBlockSize );
      errStatus = loadOnePara( "BegSrcBlockNb:", pF,
			       MemoBlock[iB].BegSrcBlockNb );
      errStatus = loadOnePara( "EndSrcBlockNb:", pF,
			       TmpEndSrcBlockNb );
      if(TmpEndSrcBlockNb==-1) MemoBlock[iB].EndSrcBlockNb = NbMemoBlocks;
      errStatus = loadOnePara( "InputGateBias:", pF,
			       MemoBlock[iB].InGate.W.Bias.w[0] );
      errStatus = loadOnePara( "OutputGateBias:", pF,
			       MemoBlock[iB].OutGate.W.Bias.w[0] );
#ifdef FORGET_GATES
      errStatus = loadOnePara( "ForgetGateBias:", pF,
			       MemoBlock[iB].FgGate.W.Bias.w[0] );
#else
      double NotUsed;
    errStatus = loadOnePara( "ForgetGateBias:", pF,
			     NotUsed );
#endif
    }
#ifdef DEBUG
  //cout<<"NbMemoBlocks:"<<NbMemoBlocks<<endl;//ddd
  cout<<"LSTM::LoadPar done."<<endl;//ddd
#endif

  if( errStatus ) 
    { 
      cout << "Parameter error\n"; 
    }
  CloseFile( &pF );

  return errStatus; 
}

char LSTM::InitOrDeleteNet(bool init) {
  if(init) {
    // Open a debug file.
    sprintf(cBuf,"%s.%d",DEBUG_LOGFILE,Tri);
    OpenFile(&DebugFile,cBuf, ios::out);
    // Seperate input Nb. for recurent output.
    NbExtIn=NbIn;
#ifdef RECURRENT_OUTPUT
    NbIn+=NbOut;
#endif
    // Read the complete par file again to restore values (e.g.Biases).
    // Also par file part of TNeuralNetBase (maybe for AlphaBase).
    if(TNeuralNetBase::LoadPar()) return 1;
    if(LoadPar()) return 1;
  } else {
    CloseFile(&DebugFile);   // Close a debug file.
  }
  InitOrDeleteMemoBlocks(init,0,NbMemoBlocks);
  // Init other units.
  // Current input.
  if(init) { 
    CurInput = new double *[NbIn];
    for(unsigned int iI=0;iI<NbIn;iI++)
      CurInput[iI] = new double[ORDER_WEIGHT];
  } else {
    for(unsigned int iI=0;iI<NbIn;iI++) delete CurInput[iI];
    delete[] CurInput; 
  }
  // Init Out units.
  if(init) Out = new TOut[NbOut];
  for(unsigned int iO=0;iO<NbOut;iO++) {
    if(init) NewWeight(Out[iO].W.Bias,OutUnitBiasInitWeight);
#ifndef NO_IN_OUT_SHORTCUTS
    if(init) {
      NewWeight(Out[iO].W.In,NbIn); 
      // Change the matching (direct 1to1, 2to2...) connections.
      if(InOutBiasForMatchingUnits) { 
	unsigned int MinNbInOut=NbIn; 
	if(MinNbInOut>NbOut) MinNbInOut=NbOut;
	for(unsigned int iM=0;iM<MinNbInOut;iM++)     
	  Out[iM].W.In[iM].w[0]=InOutBiasForMatchingUnits; }
      }
    else delete[] Out[iO].W.In;
#endif
#ifdef STAT_OUT
    if (init) StatOut = new TStatOut[NbStatOut];
    else if(StatOut) delete[] StatOut;
#endif
#ifdef USE_HIDDEN_UNITS
    if(init) NewWeight(Out[iO].W.Hidden,NbHidden);
    else delete[] Out[iO].W.Hidden;
#endif
    if(init) Out[iO].W.Cell = new TWeight *[MAX_BLOCKS];
    for(unsigned int iB=0;iB<NbMemoBlocks;iB++) 
      if(init) NewWeight(Out[iO].W.Cell[iB], MemoBlock[iB].MemoBlockSize);
      else delete[] Out[iO].W.Cell[iB];
    if(!init) delete[] Out[iO].W.Cell;
  }
  if(!init) delete[] Out;
#ifdef USE_HIDDEN_UNITS
  // Init Hidden units.
  if(init) Hidden = new THidden[NbHidden];
  for(unsigned int iH=0;iH<NbHidden;iH++) {
    if(init) NewWeight(Hidden[iH].W.Bias,HiddenUnitBiasInitWeight);
    if(CONNECT_HIDDEN2IN)
      if(init) NewWeight(Hidden[iH].W.In, NbIn);
      else delete[] Hidden[iH].W.In;
    if(CONNECT_HIDDEN2CELLS) {
      if(init) Hidden[iH].W.Cell = new TWeight *[MAX_BLOCKS];
      for(unsigned int iB=0;iB<NbMemoBlocks;iB++) 
	if(init) NewWeight(Hidden[iH].W.Cell[iB],MemoBlock[iB].MemoBlockSize);
	else delete[] Hidden[iH].W.Cell[iB];
      if(!init) delete[] Hidden[iH].W.Cell;
    }
  }
  if(!init) delete[] Hidden;
#endif
  // OutWindow.
  if(TimeWindowSize && ClampOutPredictSteps) {
    if(init) {
      OutWindow = new double[ClampOutPredictSteps];
      OutWindowPos=0; }
    else if(OutWindow) delete OutWindow;
  } else OutWindow=NULL;
  // Init NextIn predict.
  // Init PredictNextIn units.
//    if(init) PredictNextIn = new TPredictNextIn[NbPredictNextIn];
//    for(unsigned int iP=0;iP<NbPredictNextIn;iP++) {
//      if(init) NewWeight(PredictNextIn[iP].w_In, NbIn);
//      else delete[] PredictNextIn[iP].w_In;
//      if(init) NewWeight(PredictNextIn[iP].w_InClass, NbOut);
//      else delete[] PredictNextIn[iP].w_InClass;
//      if(init) PredictNextIn[iP].w_Cell = new TWeight *[MAX_BLOCKS];
//      for(unsigned int iB=0;iB<NbMemoBlocks;iB++) 
//        if(init) NewWeight(PredictNextIn[iP].w_Cell[iB], 
//  			 MemoBlock[iB].MemoBlockSize);
//        else delete[] PredictNextIn[iP].w_Cell[iB];
//      if(!init) delete[] PredictNextIn[iP].w_Cell;
//    }
//    if(!init) delete[] PredictNextIn;
//    // Init NextIn units.
//    if(NbPredictNextIn>0) {
//      if(init) NextIn = new TNextIn[NbIn];
//      for(unsigned int iO=0;iO<NbIn;iO++)
//        if(init) NewWeight(NextIn[iO].w_PredictNextIn, NbPredictNextIn);
//        else delete[] NextIn[iO].w_PredictNextIn;
//      if(!init) delete[] NextIn;
//    }
//    // Init Class predict.
//    // Init PredictClass units.
//    if(NbPredictClass) {
//      if(init) PredictClass = new TPredictClass[NbPredictClass];
//      for(unsigned int iP=0;iP<NbPredictClass;iP++) {
//        if(init) PredictClass[iP].w_Cell = new TWeight *[MAX_BLOCKS];
//        for(unsigned int iB=0;iB<NbMemoBlocks;iB++) 
//  	if(init) NewWeight(PredictClass[iP].w_Cell[iB], 
//  			   MemoBlock[iB].MemoBlockSize);
//  	else delete[] PredictClass[iP].w_Cell[iB];
//        if(!init) delete[] PredictClass[iP].w_Cell;
//      }
//      if(!init) delete[] PredictClass;
//    }
//    // Init PredictClassOut units.
//    if(NbPredictClass>0) {
//      if(init) PredictClassOut = new TPredictClassOut[NbOut];
//      for(unsigned int iO=0;iO<NbOut;iO++)
//        if(init) NewWeight(PredictClassOut[iO].w_PredictClass, NbPredictClass);
//        else delete[] PredictClassOut[iO].w_PredictClass;
//      if(!init) delete[] PredictClassOut;
//    }
  if(init) {
    // Init statistics and other working variables 
    errno=0;// Reset the math error.
    // (only what is not done in RUN).
    PatCorrectTest=0; TestMSEEpo=DBL_MAX; // First test result for StopLern.
    LastTestEpo=0;
    FreezeEndBlock=0; // Worm start.
    Clamping=false; ClampOut=false;
    Epo=0;
    LogErrRecMean=0; PartLogErrRecMean=0; NbLogedErr=0; 
    WeightMean=0; AlphaMean=0; AlphaStd=0; //WPC=0;
    StopLern=false; 
    MaxPatCorrectTrain=0; MinPatCorrectTrain=DBL_MAX; 
    MeanPatCorrectTrain=0; MeanPatCorrectTrainCount=0; 
    MinMSEEpoTrain=DBL_MAX; MinClassesWrong=UINT_MAX;
    MaxPatCorrectTrain=0;
    RandomPredictionStart=0;
#ifdef CELL_BIAS
    CellBias=true;
#else
    CellBias=false;
#endif
#ifndef NO_IN_OUT_SHORTCUTS
    IOShortcuts=true;
#else
    IOShortcuts=false;
#endif
#ifdef BUFFER_EPO_LOG
    TrainMSEEpoLogBuf.Reset(); 
    if(!NoClassesOnlyPredict) {
      TrainErrEpoLogBuf.Reset(); 
      TestErrEpoLogBuf.Reset(); 
    }
    TestMSEEpoLogBuf.Reset();
#endif
    //Alpha = AlphaBase + AlphaError;
    NbPredictOut =0;
    if(NbPredictNextIn>0) NbPredictOut +=NbIn;
    if(NbPredictClass>0)  NbPredictOut +=NbOut;
    SetTopoStatistics();
  }
  // Pattern switched to multiple seqs when iterated prediction.
  if(!TimeWindowSize) PredictionOffset=1;
  return 0;
}

void LSTM::InitOrDeleteMemoBlocks(bool init,
				   unsigned int BegBlock, 
				   unsigned int EndBlock) {
  // MemoBlocks (newed in LoadPar() and partial inited).
  // For easier indexing all arrays are newed from zero instead from
  // MemoBlock[iB].BegSrcBlockNb. For the **variables, never
  // used *var[] indices, are not newed (no difference in indexing).
  // BegSrcBlockNb will be zero(or small) in most cases anyway. 
  // But instead of newing until NbMemoBlocks we will use 
  // MemoBlock[iB].EndSrcBlockNb (not any more for full Add).
  for(unsigned int iB=BegBlock;iB<EndBlock;iB++) {
    // InGate.
    if(init) NewWeight(MemoBlock[iB].InGate.W.Bias,
		       MemoBlock[iB].InGate.W.Bias.w[0]);
    if(init) NewWeight(MemoBlock[iB].InGate.W.In, NbIn);
    else delete[] MemoBlock[iB].InGate.W.In;
#ifdef CONNECT_GATES_TO_S
    if(init) NewWeight(MemoBlock[iB].InGate.W.s, MemoBlock[iB].MemoBlockSize);
    else delete[] MemoBlock[iB].InGate.W.s;
#endif
    if(init) MemoBlock[iB].InGate.W.Cell = new TWeight *[MAX_BLOCKS];
    for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	iiB<MemoBlock[iB].EndSrcBlockNb;iiB++)
      if(init) NewWeight(MemoBlock[iB].InGate.W.Cell[iiB],
			 MemoBlock[iiB].MemoBlockSize);
      else delete[] MemoBlock[iB].InGate.W.Cell[iiB];
    if(!init) delete[] MemoBlock[iB].InGate.W.Cell;
#ifdef CONNECT_TO_GATES
    if(init) NewWeight(MemoBlock[iB].InGate.W.InGate, MAX_BLOCKS);
    else delete[] MemoBlock[iB].InGate.W.InGate;
    if(init) NewWeight(MemoBlock[iB].InGate.W.OutGate ,MAX_BLOCKS);
    else delete[] MemoBlock[iB].InGate.W.OutGate;
#endif
    // OutGate.
    if(init) NewWeight(MemoBlock[iB].OutGate.W.Bias,
		       MemoBlock[iB].OutGate.W.Bias.w[0]);
    if(init) NewWeight(MemoBlock[iB].OutGate.W.In ,NbIn);
    else delete[] MemoBlock[iB].OutGate.W.In;
#ifdef CONNECT_GATES_TO_S
    if(init) NewWeight(MemoBlock[iB].OutGate.W.s, 
		       MemoBlock[iB].MemoBlockSize);
    else delete[] MemoBlock[iB].OutGate.W.s;
#endif
    if(init) MemoBlock[iB].OutGate.W.Cell = 
	       new TWeight *[MAX_BLOCKS]; //[MemoBlock[iB].EndSrcBlockNb];
    for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) 
      if(init) NewWeight(MemoBlock[iB].OutGate.W.Cell[iiB], 
			 MemoBlock[iiB].MemoBlockSize);
      else delete[] MemoBlock[iB].OutGate.W.Cell[iiB];
    if(!init) delete[] MemoBlock[iB].OutGate.W.Cell;
#ifdef CONNECT_TO_GATES
    if(init) NewWeight(MemoBlock[iB].OutGate.W.InGate ,MAX_BLOCKS);
    else delete[] MemoBlock[iB].OutGate.W.InGate;
    if(init) NewWeight(MemoBlock[iB].OutGate.W.OutGate ,MAX_BLOCKS);
    else delete[] MemoBlock[iB].OutGate.W.OutGate;
#endif
#ifdef FORGET_GATES
    // FgGate.
    if(init) NewWeight(MemoBlock[iB].FgGate.W.Bias,
		       MemoBlock[iB].FgGate.W.Bias.w[0]);
    if(init) NewWeight(MemoBlock[iB].FgGate.W.In, NbIn);
    else delete[] MemoBlock[iB].FgGate.W.In;
#ifdef CONNECT_GATES_TO_S
    if(init) NewWeight(MemoBlock[iB].FgGate.W.s, MemoBlock[iB].MemoBlockSize);
    else delete[] MemoBlock[iB].FgGate.W.s;
#endif
    if(init) MemoBlock[iB].FgGate.W.Cell = new TWeight *[MAX_BLOCKS];
    for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) 
      if(init) NewWeight(MemoBlock[iB].FgGate.W.Cell[iiB],
			 MemoBlock[iiB].MemoBlockSize);
      else delete[] MemoBlock[iB].FgGate.W.Cell[iiB];
    if(!init) delete[] MemoBlock[iB].FgGate.W.Cell;
#ifdef CONNECT_TO_GATES
    if(init) NewWeight(MemoBlock[iB].FgGate.W.InGate, MAX_BLOCKS);
    else delete[] MemoBlock[iB].FgGate.W.InGate;
    if(init) NewWeight(MemoBlock[iB].FgGate.W.OutGate ,MAX_BLOCKS);
    else delete[] MemoBlock[iB].FgGate.W.OutGate;
#endif
#endif
    // Cells.
    if(init) MemoBlock[iB].Cell = new TCell[MemoBlock[iB].MemoBlockSize];
    for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++) {
#ifdef CELL_BIAS
      if(init) NewWeight(MemoBlock[iB].Cell[iC].W.Bias,0); // Random init.
#endif
      if(init) NewWeight(MemoBlock[iB].Cell[iC].W.In ,NbIn);
      else delete[] MemoBlock[iB].Cell[iC].W.In;
      if(init) MemoBlock[iB].Cell[iC].W.Cell = 
		 new TWeight *[MAX_BLOCKS];
      for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	  iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) 
	if(init) NewWeight(MemoBlock[iB].Cell[iC].W.Cell[iiB],
			   MemoBlock[iiB].MemoBlockSize);
	else delete[] MemoBlock[iB].Cell[iC].W.Cell[iiB];
      if(!init) delete[] MemoBlock[iB].Cell[iC].W.Cell;
#ifdef CONNECT_TO_GATES
      if(init) NewWeight(MemoBlock[iB].Cell[iC].W.InGate, MAX_BLOCKS);
      else delete[] MemoBlock[iB].Cell[iC].W.InGate;
      if(init) NewWeight(MemoBlock[iB].Cell[iC].W.OutGate, MAX_BLOCKS);
      else delete[] MemoBlock[iB].Cell[iC].W.OutGate;
#endif
    }
  }
  // Init the derivatives s_d.
  for(unsigned int iB=BegBlock;iB<EndBlock;iB++) {
    // For Cells.
    for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++) {
      if(init) NewTs_d_Gate(MemoBlock[iB].Cell[iC].s_d_In, NbIn);
      else delete[] MemoBlock[iB].Cell[iC].s_d_In;
      if(init) MemoBlock[iB].Cell[iC].s_d_Cell = 
		 new Ts_d_Gate *[MAX_BLOCKS];
      for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	  iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) 
        if(init)NewTs_d_Gate(MemoBlock[iB].Cell[iC].s_d_Cell[iiB],
		   MemoBlock[iiB].MemoBlockSize);
	else delete[] MemoBlock[iB].Cell[iC].s_d_Cell[iiB];
      if(!init) delete[] MemoBlock[iB].Cell[iC].s_d_Cell;
#ifdef CONNECT_TO_GATES
      if(init)NewTs_d_Gate(MemoBlock[iB].Cell[iC],MAX_BLOCKS);
      else delete[] MemoBlock[iB].Cell[iC].s_d_InGate;
      if(init)NewTs_d_Gate(MemoBlock[iB].Cell[iC],MAX_BLOCKS);
      else delete[] MemoBlock[iB].Cell[iC].s_d_OutGate;
#endif
      // For InGate.
      if(init) NewTs_d_Gate(MemoBlock[iB].Cell[iC].s_d_InGate_Bias);
      if(init) NewTs_d_Gate(MemoBlock[iB].Cell[iC].s_d_InGate_In,NbIn);
      else delete[] MemoBlock[iB].Cell[iC].s_d_InGate_In;
#ifdef CONNECT_GATES_TO_S
      if(init) NewTs_d_Gate(MemoBlock[iB].Cell[iC].s_d_InGate_s,
			    MemoBlock[iB].MemoBlockSize);
      else delete[] MemoBlock[iB].Cell[iC].s_d_InGate_s;
#endif
      if(init) MemoBlock[iB].Cell[iC].s_d_InGate_Cell = 
		 new Ts_d_Gate *[MAX_BLOCKS];
      for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	  iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) 
	if(init) NewTs_d_Gate(MemoBlock[iB].Cell[iC].s_d_InGate_Cell[iiB],
			      MemoBlock[iiB].MemoBlockSize);
	else delete[] MemoBlock[iB].Cell[iC].s_d_InGate_Cell[iiB];
      if(!init) delete[] MemoBlock[iB].Cell[iC].s_d_InGate_Cell;
#ifdef CONNECT_TO_GATES
      if(init) NewTs_d_Gate(MemoBlock[iB].Cell[iC].s_d_InGate_InGate,
			    MAX_BLOCKS);
      else delete[] MemoBlock[iB].Cell[iC].s_d_InGate_InGate;
      if(init) NewTs_d_Gate(MemoBlock[iB].Cell[iC].s_d_InGate_OutGate,
			    MAX_BLOCKS);
      else delete[] MemoBlock[iB].Cell[iC].s_d_InGate_OutGate;
#endif
#ifdef FORGET_GATES
      // For FgGate.
      if(init) NewTs_d_Gate(MemoBlock[iB].Cell[iC].s_d_FgGate_Bias);
      if(init) NewTs_d_Gate(MemoBlock[iB].Cell[iC].s_d_FgGate_In,NbIn);
      else delete[] MemoBlock[iB].Cell[iC].s_d_FgGate_In;
#ifdef CONNECT_GATES_TO_S
      if(init) NewTs_d_Gate(MemoBlock[iB].Cell[iC].s_d_FgGate_s,
			    MemoBlock[iB].MemoBlockSize);
      else delete[] MemoBlock[iB].Cell[iC].s_d_FgGate_s;
#endif
      if(init) MemoBlock[iB].Cell[iC].s_d_FgGate_Cell = 
		 new Ts_d_Gate *[MAX_BLOCKS];
      for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	  iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) 
	if(init) NewTs_d_Gate(MemoBlock[iB].Cell[iC].s_d_FgGate_Cell[iiB],
			      MemoBlock[iiB].MemoBlockSize);
	else delete[] MemoBlock[iB].Cell[iC].s_d_FgGate_Cell[iiB];
      if(!init) delete[] MemoBlock[iB].Cell[iC].s_d_FgGate_Cell;
#ifdef CONNECT_TO_GATES
      if(init) NewTs_d_Gate(MemoBlock[iB].Cell[iC].s_d_FgGate_InGate,
			    MAX_BLOCKS);
      else delete[] MemoBlock[iB].Cell[iC].s_d_FgGate_InGate;
      if(init) NewTs_d_Gate(MemoBlock[iB].Cell[iC].s_d_FgGate_OutGate,
			    MAX_BLOCKS);
      else delete[] MemoBlock[iB].Cell[iC].s_d_FgGate_OutGate;
#endif
#endif
    } // cell loop.
    if(!init) delete[] MemoBlock[iB].Cell;
  } // block loop.
}

void LSTM::AddMemoBlockToExistingBlocks(unsigned int BegBlock, 
					 unsigned int EndBlock) {
  unsigned int Ml = NbMemoBlocks-1; // last Block shorthand.
  for(unsigned int iB=BegBlock;iB<EndBlock;iB++) {
    // Always fully connected.
    MemoBlock[iB].EndSrcBlockNb = NbMemoBlocks;
    // InGate.	
    NewWeight(MemoBlock[iB].InGate.W.Cell[Ml], MemoBlock[Ml].MemoBlockSize);
    // OutGate.
    NewWeight(MemoBlock[iB].OutGate.W.Cell[Ml], MemoBlock[Ml].MemoBlockSize);
#ifdef FORGET_GATES
    // FgGate.	
    NewWeight(MemoBlock[iB].FgGate.W.Cell[Ml], MemoBlock[Ml].MemoBlockSize);
#endif
    // Cells.
    for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++)
      NewWeight(MemoBlock[iB].Cell[iC].W.Cell[Ml],
		MemoBlock[Ml].MemoBlockSize);
  }
  // Init the derivatives s_d.
  for(unsigned int iB=BegBlock;iB<EndBlock;iB++) {
    // For Cells.
    for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++) {
      NewTs_d_Gate(MemoBlock[iB].Cell[iC].s_d_Cell[Ml],
	MemoBlock[Ml].MemoBlockSize);
      // For InGate.	
      NewTs_d_Gate(MemoBlock[iB].Cell[iC].s_d_InGate_Cell[Ml],
		   MemoBlock[Ml].MemoBlockSize);
#ifdef FORGET_GATES
      //  For FgGate.	
      NewTs_d_Gate(MemoBlock[iB].Cell[iC].s_d_FgGate_Cell[Ml],
		   MemoBlock[Ml].MemoBlockSize);
#endif
    }
  }
  // Set Init Weights different than in NewWeight().
  // Set MemoBlock weights.
  for(unsigned int iB=BegBlock;iB<EndBlock;iB++) {
    // InGate weights. The bias was already set in LoadPar.
#ifdef CONNECT_TO_GATES
    MemoBlock[iB].InGate.W.InGate[Ml].w[0] = 0;
    MemoBlock[iB].InGate.W.OutGate[Ml].w[0] = 0;
#endif
    for(unsigned int iiC=0;iiC<MemoBlock[Ml].MemoBlockSize;iiC++)
      MemoBlock[iB].InGate.W.Cell[Ml][iiC].w[0] = 0;
    // OutGate weights.
#ifdef CONNECT_TO_GATES
    MemoBlock[iB].OutGate.W.InGate[Ml].w[0] = 0;
    MemoBlock[iB].OutGate.W.OutGate[Ml].w[0] = 0;
#endif
    for(unsigned int iiC=0;iiC<MemoBlock[Ml].MemoBlockSize;iiC++)
      MemoBlock[iB].OutGate.W.Cell[Ml][iiC].w[0]= 0;
#ifdef FORGET_GATES
    // FgGate weights. The bias was already set in LoadPar.
#ifdef CONNECT_TO_GATES
    MemoBlock[iB].FgGate.W.InGate[Ml].w[0] = 0;
    MemoBlock[iB].FgGate.W.OutGate[Ml].w[0] = 0;
#endif
    for(unsigned int iiC=0;iiC<MemoBlock[Ml].MemoBlockSize;iiC++)
      MemoBlock[iB].FgGate.W.Cell[Ml][iiC].w[0] = 0;
#endif
    // Cells weights.
    for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++) {
#ifdef CONNECT_TO_GATES
      MemoBlock[iB].Cell[iC].W.InGate[Ml].w[0] =0;
      MemoBlock[iB].Cell[iC].W.OutGate[Ml].w[0] =0;
#endif
      for(unsigned int iiC=0;iiC<MemoBlock[Ml].MemoBlockSize;iiC++)
	MemoBlock[iB].Cell[iC].W.Cell[Ml][iiC].w[0] = 0;
    }
  }
}

char LSTM::AddMemoBlock(int BegSrcBlockNb, int EndSrcBlockNb, 
			 unsigned int MemoBlockSize) {
  if(NbMemoBlocks>=MAX_BLOCKS) return 1;
  // Add Block.
  if(FreezeAndGrow) FreezeEndBlock=NbMemoBlocks; // Only the new block.
  NbMemoBlocks++; 
  if(BegSrcBlockNb==-1) BegSrcBlockNb = NbMemoBlocks-1; // Only self.
  if(EndSrcBlockNb==-1) EndSrcBlockNb = NbMemoBlocks;
  MemoBlock[NbMemoBlocks-1].MemoBlockSize = MemoBlockSize;
  MemoBlock[NbMemoBlocks-1].BegSrcBlockNb = BegSrcBlockNb;
  MemoBlock[NbMemoBlocks-1].EndSrcBlockNb = EndSrcBlockNb;
  MemoBlock[NbMemoBlocks-1].InGate.W.Bias.w[0] = -1;
  MemoBlock[NbMemoBlocks-1].OutGate.W.Bias.w[0] = -1;
#ifdef FORGET_GATES
  MemoBlock[NbMemoBlocks-1].FgGate.W.Bias.w[0] = +1;
#endif
  InitOrDeleteMemoBlocks(true, NbMemoBlocks-1, NbMemoBlocks);
  if(GrowFullyNotCascade) AddMemoBlockToExistingBlocks(0,NbMemoBlocks-1);
  // Add new block the Out src.
  for(unsigned int iO=0;iO<NbOut;iO++)
    NewWeight(Out[iO].W.Cell[NbMemoBlocks-1],
	      MemoBlock[NbMemoBlocks-1].MemoBlockSize);
//    // Add new block the PredictNextIn src.
//    for(unsigned int iP=0;iP<NbPredictNextIn;iP++)
//      NewWeight(PredictNextIn[iP].W.Cell[NbMemoBlocks-1],
//  	      MemoBlock[NbMemoBlocks-1].MemoBlockSize);
//    // Add new block the PredictClass src.
//    for(unsigned int iP=0;iP<NbPredictClass;iP++)
//      NewWeight(PredictClass[iP].W.Cell[NbMemoBlocks-1],
//  	      MemoBlock[NbMemoBlocks-1].MemoBlockSize);
  // Refresh statistics.
  SetTopoStatistics();
  return 0;
}

void LSTM::ResetNet(bool SaveState, bool RestoreState) {
  // Save and restore not for partial and backpass variables.
  // So save and rstore can only be used during testing.
  // Reset MemoBlock varables.
  for(unsigned int iB=0;iB<NbMemoBlocks;iB++) {
    // Gate variables (In, Out, Fg).
    // Even though No connection form the gates.
    if(SaveState) {
      MemoBlock[iB].InGate.y_save=MemoBlock[iB].InGate.y;
      MemoBlock[iB].OutGate.y_save=MemoBlock[iB].OutGate.y;
#ifdef FORGET_GATES      
      MemoBlock[iB].FgGate.y_save=MemoBlock[iB].FgGate.y;
#endif    
    } else if(RestoreState) {
      MemoBlock[iB].InGate.y=MemoBlock[iB].InGate.y_save;
      MemoBlock[iB].OutGate.y=MemoBlock[iB].OutGate.y_save;
#ifdef FORGET_GATES      
      MemoBlock[iB].FgGate.y=MemoBlock[iB].FgGate.y_save;
#endif    
    } else { 
      MemoBlock[iB].InGate.y=0.0;
      MemoBlock[iB].OutGate.y=0.0;
#ifdef FORGET_GATES      
      MemoBlock[iB].FgGate.y=0.0;
#endif    
    }    
    // Cells.
    for(unsigned int o=0;o<ORDER_WEIGHT;o++) {
      for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++) {
      // Cell variables.
      if(SaveState) {
	MemoBlock[iB].Cell[iC].s_save[o]=MemoBlock[iB].Cell[iC].s[o];
	MemoBlock[iB].Cell[iC].y_save[o]=MemoBlock[iB].Cell[iC].y[o];    
      } else if(RestoreState) {
	MemoBlock[iB].Cell[iC].s[o]=MemoBlock[iB].Cell[iC].s_save[o];
	MemoBlock[iB].Cell[iC].y[o]=MemoBlock[iB].Cell[iC].y_save[o];    
      } else {
	MemoBlock[iB].Cell[iC].s[o]=0.0;
	MemoBlock[iB].Cell[iC].y[o]=0.0;
      }
      // s_d for cells.
      if(!SaveState && !RestoreState) {
#ifdef CELL_BIAS
	MemoBlock[iB].Cell[iC].s_d_Bias.s_d[o]=0;
#endif
      for(unsigned int iI=0;iI<NbIn;iI++)     
	MemoBlock[iB].Cell[iC].s_d_In[iI].s_d[o]=0;
      for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	  iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) {
	if(!InternalBlockConect && (iiB==iB)) continue;
#ifdef CONNECT_TO_GATES
	MemoBlock[iB].Cell[iC].s_d_InGate[iiB].s_d[o]=0;
	MemoBlock[iB].Cell[iC].s_d_OutGate[iiB].s_d[o]=0;
#endif
	for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++)
	  MemoBlock[iB].Cell[iC].s_d_Cell[iiB][iiC].s_d[o]=0;
      }
      // s_d for InGates.
      MemoBlock[iB].Cell[iC].s_d_InGate_Bias.s_d[o]=0;
      for(unsigned int iI=0;iI<NbIn;iI++)     
	MemoBlock[iB].Cell[iC].s_d_InGate_In[iI].s_d[o]=0;
#ifdef CONNECT_GATES_TO_S
      for(unsigned int iiC=0;iiC<MemoBlock[iB].MemoBlockSize;iiC++)
	MemoBlock[iB].Cell[iC].s_d_InGate_s[iiC].s_d[o]=0;
#endif
      for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	  iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) {
#ifdef CONNECT_TO_GATES
	MemoBlock[iB].Cell[iC].s_d_InGate_InGate[iiB].s_d[o]=0;
	MemoBlock[iB].Cell[iC].s_d_InGate_OutGate[iiB].s_d[o]=0;
#endif
	for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++)
	  MemoBlock[iB].Cell[iC].s_d_InGate_Cell[iiB][iiC].s_d[o]=0;
      }
#ifdef FORGET_GATES
      // s_d for FgGates.
      MemoBlock[iB].Cell[iC].s_d_FgGate_Bias.s_d[o]=0;
      for(unsigned int iI=0;iI<NbIn;iI++)     
	MemoBlock[iB].Cell[iC].s_d_FgGate_In[iI].s_d[o]=0;
#ifdef CONNECT_GATES_TO_S
      for(unsigned int iiC=0;iiC<MemoBlock[iB].MemoBlockSize;iiC++)
	MemoBlock[iB].Cell[iC].s_d_FgGate_s[iiC].s_d[o]=0;
#endif
      for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	  iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) {
#ifdef CONNECT_TO_GATES
	MemoBlock[iB].Cell[iC].s_d_FgGate_InGate[iiB].s_d[o]=0;
	MemoBlock[iB].Cell[iC].s_d_FgGate_OutGate[iiB].s_d[o]=0;
#endif
	for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++)
	  MemoBlock[iB].Cell[iC].s_d_FgGate_Cell[iiB][iiC].s_d[o]=0;
      }
#endif
    }
      }
    } // End o loop.
  } //end (!SaveState && !RestoreState) {
  // Reset Out units varables only for RECURRENT_OUTPUT. 
#if defined(RECURRENT_OUTPUT)
  for(unsigned int iO=0;iO<NbOut;iO++) {
    if(SaveState) Out[iO].y_save=Out[iO].y;
    else if(RestoreState) Out[iO].y=Out[iO].y_save;
    else Out[iO].y=0;
  }
#ifdef STAT_OUT
  for(unsigned int iO=0;iO<NbStatOut;iO++) {
    if(SaveState) StatOut[iO].y_save=StatOut[iO].y;
    else if(RestoreState) StatOut[iO].y=StatOut[iO].y_save;
    else StatOut[iO].y=0;
  }
#endif
#endif
#if defined(SELF_RECURRENT_OUTPUT)
  for(unsigned int iO=0;iO<NbOut;iO++) {
    if(SaveState) Out[iO].net_save=Out[iO].net;
    else if(RestoreState) Out[iO].net=Out[iO].net_save;
    else Out[iO].net=0;
  }
#endif
  // Reset Hidden units varables.
#ifdef USE_HIDDEN_UNITS
  for(unsigned int o=0;o<ORDER_WEIGHT;o++) {
    for(unsigned int iH=0;iH<NbHidden;iH++) {
      if(SaveState) Hidden[iH].y_save[o] = Hidden[iH].y[o];
      else if(RestoreState) Hidden[iH].y[o] = Hidden[iH].y_save[o];
      else Hidden[iH].y[o]=0; 
    }
  }
#endif
  // Only resst the OutWindow buffer.
  if(TimeWindowSize && OutWindow) {
    if(!SaveState && !RestoreState)
      for(unsigned int i=0;i<ClampOutPredictSteps;i++) OutWindow[i]=0;
    OutWindowPos=0;
  }
  // Reset PredictNextIn units varables.
  //  for(unsigned int iP=0;iP<NbPredictNextIn;iP++) PredictNextIn[iP].y=0; 
  // Reset NextIn units varables not necessary.
  // Reset PredictClass units varables.
  //  for(unsigned int iP=0;iP<NbPredictClass;iP++)  PredictClass[iP].y=0; 
  // Reset PredictClassOut units varables not necessary.
}


void LSTM::ForwardPass( struct TPatData &PatData,
			unsigned int BegBlockDeriv,
			unsigned int EndBlockDeriv) 
{
  // One step process: update MemoBlocks, Hidden, Predict than Out.
  // Temporal order within the MemoBlocks: 
  // i) net_InGate, net_FgGate, net_cell 
  // (gates see s(t-1)(if connected))
  // ii) y_InGate, y_FgGate -> s(t)
  // ii) net_OutGate (with s(t)(if connected)), y_OutGate -> y_cell.
  //
  // Set current input (external and form ouput units).
  // Time Window update during clamping (only for order 0),
  // Simple if 
  // TimeWindowStepSize < PredictionOffset*ClampOutPredictSteps)
  // otherwise predictions have to be stored. In the latter case 
  // we need: TimeWindowStepSize==n*PredictionOffset, n>0. 
  // We assume: TimeWindowStepSize==PredictionOffset !!
  // The newest value is In[0].

    if(ClampOut && TimeWindowSize && OutWindow) {
      ClampOut=false;
      for(unsigned int iI=1;iI<NbIn;iI++) {
	//for(unsigned int iI=NbIn-1;iI>0;iI--) {
	//CurInput[iI][0]=CurInput[iI-1][0];  // Shift the window.
	CurInput[iI][0]=GetInput(PatData,Seq,Pat,iI); 
      }
      // Replace the already predicted steps 
      // (the latest one further down).
      if((TimeWindowStepSize == 
	  (unsigned int)PredictionOffset) && OutWindow) 
	{ 
	  for(unsigned int iI=1;iI<NbIn;iI++) {

	    if( iI < ((unsigned int)((Pat-ClampPatStart) / 
				     PredictionOffset)))
	      CurInput[iI][0]=
		OutWindow[(unsigned int)
			  ((Pat-ClampPatStart)/PredictionOffset)-iI-1];
	  } 
      } else if(PredictionOffset==1) {
	for(unsigned int iI=0;iI<NbIn;iI++) {
	  OutWindowPos=(unsigned int)
	    (Pat-ClampPatStart)-1-iI*TimeWindowStepSize;
	  if(OutWindowPos>=0) CurInput[iI][0]=OutWindow[OutWindowPos];
	}
      }
      ClampOut=true;
      CurInput[0][0] = GetInput(PatData,Seq,Pat,0); //Get the new out.
    } else for(unsigned int iI=0;iI<NbIn;iI++) {
      CurInput[iI][0] = GetInput(PatData,Seq,Pat,iI);
      for(unsigned int o=1;o<ORDER_WEIGHT;o++) {
	CurInput[iI][o] = CurInput[iI][o-1] * CurInput[iI][0]; 
      }
    }
  // Input to MemoBlocks (net).
  for(unsigned int iB=0;iB<NbMemoBlocks;iB++) {
    // net InGate.
    NetInputSum(MemoBlock[iB].InGate.net,
		MemoBlock[iB].InGate.W,1,1,1,0);
#ifdef CONNECT_GATES_TO_S
    NetInputPeephole(MemoBlock[iB].InGate.net,MemoBlock[iB].InGate.W,iB);
#endif
    // net OutGate. Peephole (s) later.
    NetInputSum(MemoBlock[iB].OutGate.net,MemoBlock[iB].OutGate.W,1,1,1,0);
    // net FgGate.
#ifdef FORGET_GATES
    NetInputSum(MemoBlock[iB].FgGate.net,MemoBlock[iB].FgGate.W,1,1,1,0);
#ifdef CONNECT_GATES_TO_S
    NetInputPeephole(MemoBlock[iB].FgGate.net,MemoBlock[iB].FgGate.W,iB);
#endif
#endif
    // net Cells.
    for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++) {
#ifdef CELL_BIAS
      NetInputSum(MemoBlock[iB].Cell[iC].net,
		  MemoBlock[iB].Cell[iC].W,1,1,1,0);
#else
      NetInputSum(MemoBlock[iB].Cell[iC].net,
		  MemoBlock[iB].Cell[iC].W,1,0,1,0);
#endif
    }
  } // End first loop over MemoBlocks.
  // MemoBlocks (g,s,h,y).
  for(unsigned int iB=0;iB<NbMemoBlocks;iB++) {
    // y InGate.
    MemoBlock[iB].InGate.y_t1 = MemoBlock[iB].InGate.y;
    log_sig(MemoBlock[iB].InGate.net, MemoBlock[iB].InGate.y);
#ifdef FORGET_GATES
    // y FgGate.
    Yfg(MemoBlock[iB].FgGate.net, MemoBlock[iB].FgGate.y);
#endif
    // g,s,h,y Cell[iC].
    for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++) {
      for(unsigned int o=0;o<ORDER_WEIGHT;o++) {
	MemoBlock[iB].Cell[iC].y_t1[o] = MemoBlock[iB].Cell[iC].y[o]; }
      // G Input squashing.
      G(MemoBlock[iB].Cell[iC].net, MemoBlock[iB].Cell[iC].g);
      //MemoBlock[iB].Cell[iC].g = tanh(MemoBlock[iB].Cell[iC].net);
#ifdef FORGET_GATES
      // For the FgGate partials (and direct conection of the gates to s) 
      // we keep the last s.
      for(unsigned int o=0;o<ORDER_WEIGHT;o++) {
	MemoBlock[iB].Cell[iC].s_t1[o] = MemoBlock[iB].Cell[iC].s[o]; }
      MemoBlock[iB].Cell[iC].s[0] *= MemoBlock[iB].FgGate.y;
#endif
      //MemoBlock[iB].Cell[iC].s *= 0.9;
      MemoBlock[iB].Cell[iC].s[0] +=
	MemoBlock[iB].Cell[iC].g * MemoBlock[iB].InGate.y;
      for(unsigned int o=1;o<ORDER_WEIGHT;o++) {
	MemoBlock[iB].Cell[iC].s[o] =       
	  MemoBlock[iB].Cell[iC].s[o-1] * MemoBlock[iB].Cell[iC].s[0]; }
      // Do the overflow check for s (709 is max) in H() not here.
      // H Output squashing.
#ifdef USE_H
      H(MemoBlock[iB].Cell[iC].s[0], MemoBlock[iB].Cell[iC].h);
#else
      MemoBlock[iB].Cell[iC].h = MemoBlock[iB].Cell[iC].s[0];
#endif
      //MemoBlock[iB].Cell[iC].h = tanh(MemoBlock[iB].Cell[iC].s);
      // Now we finish the OutGate net input, because it should see
      // the actual s before it lets it out.
#ifdef CONNECT_GATES_TO_S
      NetInputPeephole(MemoBlock[iB].OutGate.net,
		       MemoBlock[iB].OutGate.W,iB);
#endif      
      // y OutGate (the outher gates'ys are updated before the s calc).
      MemoBlock[iB].OutGate.y_t1 = MemoBlock[iB].OutGate.y;
      log_sig(MemoBlock[iB].OutGate.net, MemoBlock[iB].OutGate.y);
      // Gating cell output.
      MemoBlock[iB].Cell[iC].y[0] = MemoBlock[iB].Cell[iC].h *
	MemoBlock[iB].OutGate.y;
      for(unsigned int o=1;o<ORDER_WEIGHT;o++) {
	MemoBlock[iB].Cell[iC].y[o] =       
	  MemoBlock[iB].Cell[iC].y[o-1] * MemoBlock[iB].Cell[iC].y[0]; }
    }
  }
  // Partial derivatives s_d.
  for(unsigned int iB=BegBlockDeriv;iB<EndBlockDeriv;iB++) {
    log_sig_d(MemoBlock[iB].InGate.y,InGate_df); //For the s_d for InGate.
#ifdef FORGET_GATES
    Yfg_d(MemoBlock[iB].FgGate.y,FgGate_df); //For the s_d for FgGate.
#endif
    for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++) {
      // For Cells.
#if defined(G_SIGMOID) || defined(G_TANH)
      G_d(MemoBlock[iB].Cell[iC].g, g_d);
#elif defined(G_RATIONAL_FUNC) || defined(G_LINEAR)
      G_d(g_d);
#endif
      g_d__y_in = g_d * MemoBlock[iB].InGate.y; // For all cells in block.
      //s_d cells.
#ifdef CELL_BIAS
      Update_Cell_s_d(MemoBlock[iB].Cell[iC].s_d_Bias.s_d,iB,1);
#endif
      for(unsigned int iI=0;iI<NbIn;iI++)
	Update_Cell_s_d(MemoBlock[iB].Cell[iC].s_d_In[iI].s_d,iB,
			CurInput[iI]);
      for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	  iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) {
	if(!InternalBlockConect && (iiB==iB)) continue;
#ifdef CONNECT_TO_GATES
	Update_Cell_s_d(MemoBlock[iB].Cell[iC].s_d_InGate[iiB].s_d,iB,
			MemoBlock[iiB].InGate.y_t1);
	Update_Cell_s_d(MemoBlock[iB].Cell[iC].s_d_OutGate[iiB].s_d,iB,
			MemoBlock[iiB].OutGate.y_t1);
#endif
	for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++)
	  Update_Cell_s_d(MemoBlock[iB].Cell[iC].s_d_Cell[iiB][iiC].s_d,iB,
			  MemoBlock[iiB].Cell[iiC].y_t1);
      }
      // s_d for InGate.
      y_in_d__g = InGate_df * MemoBlock[iB].Cell[iC].g;
      Update_InGate_s_d(MemoBlock[iB].Cell[iC].s_d_InGate_Bias.s_d,iB,1);
      for(unsigned int iI=0;iI<NbIn;iI++)
	Update_InGate_s_d(MemoBlock[iB].Cell[iC].s_d_InGate_In[iI].s_d,iB,
			  CurInput[iI]);
#ifdef CONNECT_GATES_TO_S
      //From s
      for(unsigned int iiC=0;iiC<MemoBlock[iB].MemoBlockSize;iiC++) { 
	Update_InGate_s_d(MemoBlock[iB].Cell[iC].s_d_InGate_s[iiC].s_d,iB,
			  MemoBlock[iB].Cell[iiC].s_t1);
#ifdef PEEPHOLE_W_TERM
	// Only for order 0.
      for(unsigned int iiiC=0;iiiC<MemoBlock[iB].MemoBlockSize;iiiC++)
	MemoBlock[iB].Cell[iC].s_d_InGate_s[iiC].s_d[0]+=
	  y_in_d__g*MemoBlock[iB].Cell[iiiC].s_t1[0]*
	  MemoBlock[iB].InGate.W.s[iiiC].w[0];
#endif
	}
#endif
      for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	  iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) {
#ifdef CONNECT_TO_GATES
	Update_InGate_s_d(MemoBlock[iB].Cell[iC].s_d_InGate_InGate[iiB].s_d,
			  iB,MemoBlock[iiB].InGate.y_t1);
	Update_InGate_s_d(MemoBlock[iB].Cell[iC].s_d_InGate_OutGate[iiB].s_d,
			  iB,MemoBlock[iiB].OutGate.y_t1);
#endif
	for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++)
	  Update_InGate_s_d
	    (MemoBlock[iB].Cell[iC].s_d_InGate_Cell[iiB][iiC].s_d,
	     iB,MemoBlock[iiB].Cell[iiC].y_t1);
      }
#ifdef FORGET_GATES
      // s_d for FgGate.
      y__fg_d__s_t1 = FgGate_df * MemoBlock[iB].Cell[iC].s_t1[0];
      Update_FgGate_s_d(MemoBlock[iB].Cell[iC].s_d_FgGate_Bias.s_d,iB,1);
      for(unsigned int iI=0;iI<NbIn;iI++)
	Update_FgGate_s_d(MemoBlock[iB].Cell[iC].s_d_FgGate_In[iI].s_d,
			  iB,CurInput[iI]);
#ifdef CONNECT_GATES_TO_S
      //From s.
      for(unsigned int iiC=0;iiC<MemoBlock[iB].MemoBlockSize;iiC++) {
	Update_FgGate_s_d(MemoBlock[iB].Cell[iC].s_d_FgGate_s[iiC].s_d,iB,
			  MemoBlock[iB].Cell[iiC].s_t1);
#ifdef PEEPHOLE_W_TERM
	// Only for order 0.
      for(unsigned int iiiC=0;iiiC<MemoBlock[iB].MemoBlockSize;iiiC++)
	MemoBlock[iB].Cell[iC].s_d_FgGate_s[iiC].s_d[0]+=
	  y__fg_d__s_t1*MemoBlock[iB].Cell[iiiC].s_t1[0]*
	  MemoBlock[iB].FgGate.W.s[iiiC].w[0];
#endif
      }
#endif
      for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	  iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) {
#ifdef CONNECT_TO_GATES
	Update_FgGate_s_d(MemoBlock[iB].Cell[iC].s_d_FgGate_InGate[iiB].s_d,
			  iB,MemoBlock[iiB].InGate.y_t1);
	Update_FgGate_s_d
	  (MemoBlock[iB].Cell[iC].s_d_FgGate_OutGate[iiB].s_d,
	   iB,MemoBlock[iiB].OutGate.y_t1);
#endif
	for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++)
	  Update_FgGate_s_d
	    (MemoBlock[iB].Cell[iC].s_d_FgGate_Cell[iiB][iiC].s_d,
	     iB,MemoBlock[iiB].Cell[iiC].y_t1);
      }
#endif
    } // End iC loop.
  } // End iB loop.
  //
  //
#ifdef USE_HIDDEN_UNITS
  // net Hidden.
  for(unsigned int iH=0;iH<NbHidden;iH++)
    NetInputSum(Hidden[iH].net,
		Hidden[iH].W,CONNECT_HIDDEN2IN,1,
		CONNECT_HIDDEN2CELLS,0);
#endif
#ifdef USE_HIDDEN_UNITS
  // Update the Hidden units (y).
  for(unsigned int iH=0;iH<NbHidden;iH++) {
    f_Hidden(Hidden[iH].net,Hidden[iH].y[0]);
    for(unsigned int o=1;o<ORDER_WEIGHT;o++) {
      Hidden[iH].y[o] = Hidden[iH].y[o-1] * Hidden[iH].y[0]; }
  }
#endif
  //   // Input to the PredictNextIn units (net).
  //   for(unsigned int iP=0;iP<NbPredictNextIn;iP++) {
  //     PredictNextIn[iP].net=0;    
  //     for(unsigned int iI=0;iI<NbIn;iI++) // From Input.
  //       PredictNextIn[iP].net += PredictNextIn[iP].w_In[iI].w * 
  // 	CurInput[iI];
  //     for(unsigned int iI=0;iI<NbOut;iI++) // From constant InClass.
  //       PredictNextIn[iP].net += PredictNextIn[iP].w_InClass[iI].w *
  // 	PatData.SeqList,Seq,PatData.NbPat[Seq]-1,NbExtIn+iI);
  //     for(unsigned int iB=0;iB<NbMemoBlocks;iB++)
  //       for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++)
  // 	PredictNextIn[iP].net += PredictNextIn[iP].w_Cell[iB][iC].w * 
  // 	  MemoBlock[iB].Cell[iC].y;
  //   }
  //   // Input to the NextIn units (net).
  //   if(NbPredictNextIn>0) {
  //     for(unsigned int iO=0;iO<NbIn;iO++) {
  //       NextIn[iO].net=0;
  //       for(unsigned int iP=0;iP<NbPredictNextIn;iP++)
  // 	NextIn[iO].net +=NextIn[iO].w_PredictNextIn[iP].w * 
  // 	  PredictNextIn[iP].y;
  //     }
  //   }
  //   // Input to the PredictClass units (net).
  //   for(unsigned int iP=0;iP<NbPredictClass;iP++) {
  //     PredictClass[iP].net=0;    
  //     for(unsigned int iB=0;iB<NbMemoBlocks;iB++)
  //       for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++)
  // 	PredictClass[iP].net += PredictClass[iP].w_Cell[iB][iC].w * 
  // 	  MemoBlock[iB].Cell[iC].y;
  //   }
  //   // Input to the PredictClassOut units (net).
  //   if(NbPredictClass>0) {
  //     for(unsigned int iO=0;iO<NbOut;iO++) {
  //       PredictClassOut[iO].net=0;
  //       for(unsigned int iP=0;iP<NbPredictClass;iP++)
  // 	PredictClassOut[iO].net += PredictClassOut[iO].w_PredictClass[iP].w * 
  // 	  PredictClass[iP].y;
  //     }
  //   }
  //
  //   // Update the PredictNextIn units (y).
  //   for(unsigned int iP=0;iP<NbPredictNextIn;iP++)
  //     log_sig(PredictNextIn[iP].net, PredictNextIn[iP].y);
  //   // Update the NextIn units (y).
  //   if(NbPredictNextIn>0)
  //     for(unsigned int iO=0;iO<NbIn;iO++)
  //       log_sig(NextIn[iO].net, NextIn[iO].y);
  //   // Update the PredictClass units (y).
  //   for(unsigned int iP=0;iP<NbPredictClass;iP++)
  //     log_sig(PredictClass[iP].net, PredictClass[iP].y);
  //   // Update the PredictClassOut units (y).
  //   if(NbPredictClass>0) 
  //     for(unsigned int iO=0;iO<NbOut;iO++)
  //       log_sig(PredictClassOut[iO].net, PredictClassOut[iO].y);
  //
  // Input to and Update the out units (net,y).
  for(unsigned int iO=0;iO<NbOut;iO++) {
    //Out[iO].net=0; // Reset with bias.
#ifdef SELF_RECURRENT_OUTPUT
    if(!Out[iO].net) // Bias once.
      NetInputSum(Out[iO].net,Out[iO].W,IOShortcuts,1,1,1);
    else 
      NetInputSum(Out[iO].net,Out[iO].W,IOShortcuts,0,1,1);
#else
    NetInputSum(Out[iO].net,Out[iO].W,IOShortcuts,1,1,1);
    //    Out[iO].net=Out[iO].W.Bias.w;
#endif
    // Out y.
#ifndef LINEAR_OUTPUT
#ifdef OUTPUT_SIGMOID
    Output_Sig(Out[iO].net, Out[iO].y);
#else
    log_sig(Out[iO].net, Out[iO].y);
#endif
#else
    Out[iO].y = Out[iO].net;
#endif
  }
#ifdef STAT_OUT
  // Called after forward pass.
  // This is problem dependent.
  //
  // y(t+1) for Mackey Glass with one regular output: dy(t+1), input y(t).
  // In[0]=y(t), In[1]=dy(t) (maybe).
  // One dimension and order 0.
  //  for(unsigned int iO=0;iO<NbStatOut;iO++) {
  //    StatOut[iO].y=CurInput[1+iO][0]+Out[iO].y/PredictDiffFactor; 
  StatOut[0].y=CurInput[0][0]+Out[0].y/PredictDiffFactor; 
  //}
#endif
  // Store predictions for sliding time window.
  // Here one dimensional, order 0 for statouts.
  // This is problem dependent !!
#ifdef STAT_OUT
  if(TimeWindowSize && OutWindow) {
    if(ClampOut || Clamping) {
      if(TimeWindowStepSize==(unsigned int)PredictionOffset) {
	//cout<<"OutWindow[(unsigned int)
	//((Pat-ClampPatStart)/PredictionOffset)]: "
	//<<(int)((Pat-ClampPatStart)/PredictionOffset)<<endl;//ddd
	OutWindow[(unsigned int)((Pat-ClampPatStart)/PredictionOffset)]=
	  StatOut[0].y; 
      } else if(PredictionOffset==1) {
	OutWindowPos=(unsigned int)(Pat-ClampPatStart);
	if(OutWindowPos>=0) OutWindow[OutWindowPos]=StatOut[0].y; 
      }
    } 
  }
#endif
}



void LSTM::BackwardPass(unsigned int BegBlock, unsigned int EndBlock) {
  // Calculate the errors (e,delta).
  // e,delta out units.
  //cout<<"Start BackwardPass."<<endl;//ddd
  for(unsigned int iO=0;iO<NbOut;iO++) {
    if(AllTimeTarget || SetStepTarget || (Pat==TrainData.NbPat[Seq]-1)) { 
      Out[iO].e=((TrainData.SeqList[Seq])[Pat])[NbExtIn+iO] - Out[iO].y;
      //if(Pat<TrainData.NbPat[Seq]-1) // Not the last Pat in Seq.
      //Out[iO].e /= TrainData.NbPat[Seq];
#if !defined(LOG_LIKELIHOOD_OUTPUT) && !defined(LINEAR_OUTPUT)
#ifdef OUTPUT_SIGMOID
      Out[iO].delta = Output_Sig_d(Out[iO].y,Out[iO].df) * Out[iO].e;
#else 
      Out[iO].delta = log_sig_d(Out[iO].y,Out[iO].df) * Out[iO].e;
#endif
#else 
      Out[iO].delta = Out[iO].e; Out[iO].df=1;
#endif
    } else { 
      // Keep track for local learning rate adaptation.
      // If not defined the Backpass will not be called anyway.
      Out[iO].e=0; Out[iO].delta=0; 
#if !defined(LOG_LIKELIHOOD_OUTPUT) && !defined(LINEAR_OUTPUT)
#ifdef OUTPUT_SIGMOID
      Output_Sig_d(Out[iO].y,Out[iO].df); 
#else 
      log_sig_d(Out[iO].y,Out[iO].df); 
#endif
#else
      Out[iO].df=1; 
#endif
    }
  }
#ifdef STAT_OUT
  for(unsigned int iO=0;iO<NbStatOut;iO++) {
    StatOut[iO].t=((TrainData.SeqList[Seq])[Pat])[NbExtIn+NbOut+iO];
    StatOut[iO].e=StatOut[iO].t-StatOut[iO].y;
  }
#endif
#ifdef USE_HIDDEN_UNITS
  // e,delta Hidden units.
  for(unsigned int iH=0;iH<NbHidden;iH++) {
    Hidden[iH].e =0;
    for(unsigned int iO=0;iO<NbOut;iO++) { // From Out.
      Hidden[iH].e += Out[iO].delta * Out[iO].W.Hidden[iH].w[0];
      for(unsigned int o=1;o<ORDER_WEIGHT;o++) {
	// From dy_k/dy_c: Add derivative of y^o: o*y^(o-1).
	Hidden[iH].e += 
	  Out[iO].W.Hidden[iH].w[o] * Out[iO].delta
	  * (o+1) * Hidden[iH].y[o-1];
      }
    }
    // Only the derivative of y[0] is necessary.
    Hidden[iH].delta = 
      f_d_Hidden(Hidden[iH].y[0],Hidden[iH].df) * Hidden[iH].e;
  }
#endif
  //   // e,delta NextIn units.
  //   if(NbPredictNextIn>0)
  //     for(unsigned int iO=0;iO<NbIn;iO++) {
  //       if(Pat==TrainData.NbPat[Seq]-1) 
  //  NextIn[iO].e =0; // Last Pat in Seq.
  //       else NextIn[iO].e = // Difference to the next input.
  // 	     (GetInput(TrainData,Seq,Pat+1,iO) - NextIn[iO].y);
  //       NextIn[iO].delta = log_sig_d(NextIn[iO].y,NextIn[iO].df) 
  // 	* NextIn[iO].e;
  //     }
  //   // e,delta PredictNextIn units.
  //   for(unsigned int iP=0;iP<NbPredictNextIn;iP++) {
  //     PredictNextIn[iP].e =0;
  //     for(unsigned int iO=0;iO<NbIn;iO++) // Error From NextIn.
  //       PredictNextIn[iP].e += NextIn[iO].delta 
  // 	* NextIn[iO].w_PredictNextIn[iP].w;
  //     PredictNextIn[iP].delta *= 
  //       log_sig_d(PredictNextIn[iP].y,PredictNextIn[iP].df)
  //       * PredictNextIn[iP].e;
  //   }
  //   // e,delta PredictClassOut units.
  //   if(NbPredictClass>0)
  //     for(unsigned int iO=0;iO<NbOut;iO++) {
  //       PredictClassOut[iO].e = 
  // 	( ((TrainData.SeqList[Seq])[NbPat[Seq]-1])[NbExtIn+iO]
  // 	 - PredictClassOut[iO].y);
  //       PredictClassOut[iO].delta = 
  // 	log_sig_d(PredictClassOut[iO].y,PredictClassOut[iO].df)
  // 	* PredictClassOut[iO].e;
  //     }
  //   // e,delta PredictClass units.
  //   for(unsigned int iP=0;iP<NbPredictClass;iP++) {
  //     PredictClass[iP].e =0;
  //     for(unsigned int iO=0;iO<NbOut;iO++) // Error From PredictClassOut.
  //       PredictClass[iP].e += PredictClassOut[iO].delta * 
  // 	PredictClassOut[iO].w_PredictClass[iP].w;
  //     PredictClass[iP].delta = 
  //       log_sig_d(PredictClass[iP].y,PredictClass[iP].df)
  //       * PredictClass[iP].e;
  //   }
  // e_unscaled cells, the weighted sum of deltas from the output units.
  // Used for out gate error and e.
  for(unsigned int iB=BegBlock;iB<EndBlock;iB++)
    for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++) {
      MemoBlock[iB].Cell[iC].e_unscaled=0;
      // Delta from regular Out units.
      for(unsigned int iO=0;iO<NbOut;iO++) { 
	MemoBlock[iB].Cell[iC].e_unscaled += Out[iO].W.Cell[iB][iC].w[0] 
	  * Out[iO].delta;
	for(unsigned int o=1;o<ORDER_WEIGHT;o++) {
	  // From dy_k/dy_c: Add derivative of y^o: o*y^(o-1).
	  MemoBlock[iB].Cell[iC].e_unscaled += 
	    Out[iO].W.Cell[iB][iC].w[o] * Out[iO].delta
	    * (o+1) * MemoBlock[iB].Cell[iC].y[o-1];
	}
      }
#ifdef USE_HIDDEN_UNITS
      // Delta from Hidden units.
      if(CONNECT_HIDDEN2CELLS) {
	for(unsigned int iH=0;iH<NbHidden;iH++) {
	  MemoBlock[iB].Cell[iC].e_unscaled += 
	    Hidden[iH].W.Cell[iB][iC].w[0] 
	    * Hidden[iH].delta;
	  for(unsigned int o=1;o<ORDER_WEIGHT;o++) {
	    // From dy_k/dy_c: Add derivative of y^o: o*y^(o-1).
	    MemoBlock[iB].Cell[iC].e_unscaled += 
	      Hidden[iH].W.Cell[iB][iC].w[o] * Hidden[iH].delta
	      * (o+1) * MemoBlock[iB].Cell[iC].y[o-1];
	  }
	}
      }
#endif
      //
      //       for(unsigned int iP=0;iP<NbPredictNextIn;iP++) // From PredictNextIn units.
      // 	MemoBlock[iB].Cell[iC].e_unscaled +=
      // 	  PredictNextIn[iP].w_Cell[iB][iC].w *
      // 	  PredictNextIn[iP].delta / TrainData.NbPat[Seq];
      //       for(unsigned int iP=0;iP<NbPredictClass;iP++) // From PredictClass units.
      // 	MemoBlock[iB].Cell[iC].e_unscaled += 
      // 	  PredictClass[iP].w_Cell[iB][iC].w *
      // 	  PredictClass[iP].delta / TrainData.NbPat[Seq];
    }
  // e cells (=e InGates, also for the FgGates).
  // Internal state error.
  for(unsigned int iB=BegBlock;iB<EndBlock;iB++)
    for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++)
      MemoBlock[iB].Cell[iC].e = MemoBlock[iB].OutGate.y
#if (defined(H_SIGMOID) || defined(H_TANH)) && defined(USE_H)
	* H_d(MemoBlock[iB].Cell[iC].h) 
#endif
#ifdef OUTGATE_ERROR_SPLIT
	/(fabs(MemoBlock[iB].Cell[iC].h)+MemoBlock[iB].OutGate.y)
#endif
	* MemoBlock[iB].Cell[iC].e_unscaled;
  // end of H_d (!! defines inside expression !!)
  //
  // e,delta OutGates.
  for(unsigned int iB=BegBlock;iB<EndBlock;iB++) {
    MemoBlock[iB].OutGate.e=0;
    for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++)
      MemoBlock[iB].OutGate.e += MemoBlock[iB].Cell[iC].h
#ifdef OUTGATE_ERROR_SPLIT
	/(fabs(MemoBlock[iB].Cell[iC].h)+MemoBlock[iB].OutGate.y)
#endif
	* MemoBlock[iB].Cell[iC].e_unscaled;
    MemoBlock[iB].OutGate.delta =
      log_sig_d(MemoBlock[iB].OutGate.y,MemoBlock[iB].OutGate.df)
      * MemoBlock[iB].OutGate.e;
  }
  // Calcualte the local learning rate alpha and dw.
  // Three layes in one time step.
  // dw out units.
  for(unsigned int iO=0;iO<NbOut;iO++) {
    AdjustAlphaAndWeights(Out[iO].delta,
#ifdef LOCAL_ALPHA
			  //	  Out[iO].df,
#endif
			  &one,Out[iO].W.Bias,1);
#ifndef NO_IN_OUT_SHORTCUTS
    for(unsigned int iI=0;iI<NbIn;iI++) // From Input.
      AdjustAlphaAndWeights(Out[iO].delta,
#ifdef LOCAL_ALPHA
			    //	    Out[iO].df,
#endif
			    CurInput[iI],
			    Out[iO].W.In[iI],0);
#endif
    for(unsigned int iB=BegBlock;iB<EndBlock;iB++) // From Blocks.
      for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++)
	AdjustAlphaAndWeights(Out[iO].delta,
#ifdef LOCAL_ALPHA
			      //	      Out[iO].df,
#endif
			      MemoBlock[iB].Cell[iC].y,
			      Out[iO].W.Cell[iB][iC],0);
#ifdef USE_HIDDEN_UNITS
  for(unsigned int iH=0;iH<NbHidden;iH++)  // From Hidden.
	AdjustAlphaAndWeights(Out[iO].delta,
#ifdef LOCAL_ALPHA
			      //	      Out[iO].df,
#endif
			      Hidden[iH].y,
			      Out[iO].W.Hidden[iH],0);
#endif
  }
#ifdef USE_HIDDEN_UNITS
  // dw Hidden units.
  for(unsigned int iH=0;iH<NbHidden;iH++) {
    AdjustAlphaAndWeights(Hidden[iH].delta,
#ifdef LOCAL_ALPHA
			  //	  Hidden[iH].df,
#endif
			  &one,Hidden[iH].W.Bias,1);
    if(CONNECT_HIDDEN2IN)
      for(unsigned int iI=0;iI<NbIn;iI++) // From Input.
	AdjustAlphaAndWeights(Hidden[iH].delta,
#ifdef LOCAL_ALPHA
			      //	    Hidden[iH].df,
#endif
			      CurInput[iI],
			      Hidden[iH].W.In[iI],0);
    if(CONNECT_HIDDEN2CELLS)
      for(unsigned int iB=BegBlock;iB<EndBlock;iB++) // From Blocks.
	for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++)
	  AdjustAlphaAndWeights(Hidden[iH].delta,
#ifdef LOCAL_ALPHA
				//		      Hidden[iH].df,
#endif
				MemoBlock[iB].Cell[iC].y,
				Hidden[iH].W.Cell[iB][iC],0);
  }
#endif
  //   // dw NextIn units. 
  //   if(NbPredictNextIn>0)
  //     for(unsigned int iO=0;iO<NbIn;iO++)
  //       for(unsigned int iP=0;iP<NbPredictNextIn;iP++) 
  // 	AdjustAlphaAndWeights(NextIn[iO].delta,NextIn[iO].df,
  // 			      PredictNextIn[iP].y,
  // 			      NextIn[iO].w_PredictNextIn[iP],0);
  //   // dw PredicNextIn units.
  //   for(unsigned int iP=0;iP<NbPredictNextIn;iP++) {
  //     // PredictNextIn[iP].e *= AlphaPredict;
  //     for(unsigned int iI=0;iI<NbIn;iI++) // From Input.
  //       AdjustAlphaAndWeights(PredictNextIn[iP].delta,PredictNextIn[iP].df,
  // 			    CurInput[iI],
  // 			    PredictNextIn[iP].w_In[iI],0);
  //     for(unsigned int iI=0;iI<NbOut;iI++) // From const class.
  //       AdjustAlphaAndWeights(PredictNextIn[iP].delta,PredictNextIn[iP].df,
  // 			    ((TrainData.SeqList[Seq],0)
  //   			    [NbPat[Seq]-1])[NbIn+iI],
  // 			    PredictNextIn[iP].w_InClass[iI]);
  //     for(unsigned int iB=BegBlock;iB<EndBlock;iB++)
  //       for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++)
  // 	AdjustAlphaAndWeights(PredictNextIn[iP].delta,PredictNextIn[iP].df,
  // 			      MemoBlock[iB].Cell[iC].y_t1,
  // 			      PredictNextIn[iP].w_Cell[iB][iC],0);
  //   } 
  //   // dw PredictClassOut units. 
  //   if(NbPredictClass>0)
  //     for(unsigned int iO=0;iO<NbOut;iO++)
  //       for(unsigned int iP=0;iP<NbPredictClass;iP++)
  // 	AdjustAlphaAndWeights(PredictClassOut[iO].delta,
  // 			      PredictClassOut[iO].df,
  // 			      PredictClass[iP].y,
  // 			      PredictClassOut[iO].w_PredictClass[iP],0);

//   // dw PredicClass units. 
//   for(unsigned int iP=0;iP<NbPredictClass;iP++)
//     for(unsigned int iB=BegBlock;iB<EndBlock;iB++)
//       for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++)
// 	AdjustAlphaAndWeights(PredictClass[iP].delta,PredictClass[iP].df,
// 			      MemoBlock[iB].Cell[iC].y_t1,
//			      PredictClass[iP].w_Cell[iB][iC],0);
  // dw OutGate.
  for(unsigned int iB=BegBlock;iB<EndBlock;iB++) {
    AdjustAlphaAndWeights(MemoBlock[iB].OutGate.delta,
#ifdef LOCAL_ALPHA
			  //	  MemoBlock[iB].OutGate.df,
#endif
			  &one,
			  MemoBlock[iB].OutGate.W.Bias,1);
    for(unsigned int iI=0;iI<NbIn;iI++) 
      AdjustAlphaAndWeights(MemoBlock[iB].OutGate.delta,
#ifdef LOCAL_ALPHA
			    //	    MemoBlock[iB].OutGate.df,
#endif
			    CurInput[iI],
			    MemoBlock[iB].OutGate.W.In[iI],0);
#ifdef CONNECT_GATES_TO_S
    for(unsigned int iiC=0;iiC<MemoBlock[iB].MemoBlockSize;iiC++) // From s.
      AdjustAlphaAndWeights(MemoBlock[iB].OutGate.delta,
#ifdef LOCAL_ALPHA
			    //MemoBlock[iB].OutGate.df,
#endif
			    // OutGate see one step ahead,
			    // it used already s(t) not s(t-1).
			    MemoBlock[iB].Cell[iiC].s, 
			    MemoBlock[iB].OutGate.W.s[iiC],0);
#endif
    for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) {
#ifdef CONNECT_TO_GATES
      AdjustAlphaAndWeights(MemoBlock[iB].OutGate.delta,
#ifdef LOCAL_ALPHA
			    
			    //MemoBlock[iB].OutGate.df,
#endif
			    MemoBlock[iiB].InGate.y_t1,
			    MemoBlock[iB].OutGate.W.InGate[iiB],0);
      AdjustAlphaAndWeights(MemoBlock[iB].OutGate.delta,
#ifdef LOCAL_ALPHA
			    
			    //MemoBlock[iB].OutGate.df,
#endif
			    MemoBlock[iiB].OutGate.y_t1,
			    MemoBlock[iB].OutGate.W.OutGate[iiB],0);
#endif
      for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++)
	AdjustAlphaAndWeights(MemoBlock[iB].OutGate.delta,
#ifdef LOCAL_ALPHA
			      //MemoBlock[iB].OutGate.df,
#endif
			      MemoBlock[iiB].Cell[iiC].y_t1,
			      MemoBlock[iB].OutGate.W.Cell[iiB][iiC],0);
    }
  }
  // dw cells.
  for(unsigned int iB=BegBlock;iB<EndBlock;iB++) {
    for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++) {
#ifdef CELL_BIAS
      RTRLAdjustAlphaAndWeights(MemoBlock[iB].Cell[iC].s_d_Bias.s_d,
				MemoBlock[iB].Cell[iC].e,
#ifdef LOCAL_ALPHA
				MemoBlock[iB].Cell[iC].W.Bias.alpha,
				//MemoBlock[iB].Cell[iC].W.Bias.h,
#endif
				MemoBlock[iB].Cell[iC].W.Bias,1);      
#endif
      for(unsigned int iI=0;iI<NbIn;iI++)     
	RTRLAdjustAlphaAndWeights(MemoBlock[iB].Cell[iC].s_d_In[iI].s_d,
				  MemoBlock[iB].Cell[iC].e,
#ifdef LOCAL_ALPHA
				  MemoBlock[iB].Cell[iC].W.In[iI].alpha,
				  //MemoBlock[iB].Cell[iC].W.In[iI].h,
#endif
				  MemoBlock[iB].Cell[iC].W.In[iI],0);
      for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	  iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) {
	if(!InternalBlockConect && (iiB==iB)) continue;
#ifdef CONNECT_TO_GATES
	RTRLAdjustAlphaAndWeights
	  (MemoBlock[iB].Cell[iC].s_d_InGate[iiB].s_d,
	   MemoBlock[iB].Cell[iC].e,
#ifdef LOCAL_ALPHA
	   MemoBlock[iB].Cell[iC].W.InGate[iiB].alpha,
	   //MemoBlock[iB].Cell[iC].W.InGate[iiB].h,
#endif
	   MemoBlock[iB].Cell[iC].W.InGate[iiB],0);
	RTRLAdjustAlphaAndWeights
	  (MemoBlock[iB].Cell[iC].s_d_OutGate[iiB].s_d,
	   MemoBlock[iB].Cell[iC].e,
#ifdef LOCAL_ALPHA
	   MemoBlock[iB].Cell[iC].W.OutGate[iiB].alpha,
	   //MemoBlock[iB].Cell[iC].W.OutGate[iiB].h,
#endif
	   MemoBlock[iB].Cell[iC].W.OutGate[iiB],0);
#endif
	for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++)
	  RTRLAdjustAlphaAndWeights
	    (MemoBlock[iB].Cell[iC].s_d_Cell[iiB][iiC].s_d,
	     MemoBlock[iB].Cell[iC].e,
#ifdef LOCAL_ALPHA
	     MemoBlock[iB].Cell[iC].W.Cell[iiB][iiC].alpha,
	     //MemoBlock[iB].Cell[iC].W.Cell[iiB][iiC].h,
#endif
	     MemoBlock[iB].Cell[iC].W.Cell[iiB][iiC],0);
      }
    }
  }
  // dw InGate.
  // Here we could do two runs. One to update alpha and 
  // than one for the weights, otherwise we get changing alphas during one
  // weight update, ..ok.
  for(unsigned int iB=BegBlock;iB<EndBlock;iB++) {
    for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++) {
      RTRLAdjustAlphaAndWeights(MemoBlock[iB].Cell[iC].s_d_InGate_Bias.s_d,
				MemoBlock[iB].Cell[iC].e,
#ifdef LOCAL_ALPHA
				MemoBlock[iB].Cell[iC].s_d_InGate_Bias.alpha,
				//MemoBlock[iB].Cell[iC].s_d_InGate_Bias.h,
#endif
				MemoBlock[iB].InGate.W.Bias,1);
      for(unsigned int iI=0;iI<NbIn;iI++)     
	RTRLAdjustAlphaAndWeights
	  (MemoBlock[iB].Cell[iC].s_d_InGate_In[iI].s_d,
	   MemoBlock[iB].Cell[iC].e,
#ifdef LOCAL_ALPHA
	   MemoBlock[iB].Cell[iC].s_d_InGate_In[iI].alpha,
	   //MemoBlock[iB].Cell[iC].s_d_InGate_In[iI].h,
#endif
	   MemoBlock[iB].InGate.W.In[iI],0);
#ifdef CONNECT_GATES_TO_S
      for(unsigned int iiC=0;iiC<MemoBlock[iB].MemoBlockSize;iiC++) // From s.
	RTRLAdjustAlphaAndWeights
	  (MemoBlock[iB].Cell[iC].s_d_InGate_s[iiC].s_d,
	   MemoBlock[iB].Cell[iC].e,
#ifdef LOCAL_ALPHA
	   MemoBlock[iB].Cell[iC].s_d_InGate_s[iiC].alpha,
	   //MemoBlock[iB].Cell[iC].s_d_InGate_s[iiC].h,
#endif
	   MemoBlock[iB].InGate.W.s[iiC],0);
#endif
      for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	  iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) {
#ifdef CONNECT_TO_GATES
	RTRLAdjustAlphaAndWeights
	  (MemoBlock[iB].Cell[iC].s_d_InGate_InGate[iiB].s_d,
	   MemoBlock[iB].Cell[iC].e,
#ifdef LOCAL_ALPHA
	   MemoBlock[iB].Cell[iC].s_d_InGate_InGate[iiB].alpha,
	   //MemoBlock[iB].Cell[iC].s_d_InGate_InGate[iiB].h,
#endif
	   MemoBlock[iB].InGate.W.InGate[iiB],0);
	RTRLAdjustAlphaAndWeights
	  (MemoBlock[iB].Cell[iC].s_d_InGate_OutGate[iiB].s_d,
	   MemoBlock[iB].Cell[iC].e,
#ifdef LOCAL_ALPHA
	   MemoBlock[iB].Cell[iC].s_d_InGate_OutGate[iiB].alpha,
	   //MemoBlock[iB].Cell[iC].s_d_InGate_OutGate[iiB].h,
#endif
	   MemoBlock[iB].InGate.W.OutGate[iiB],0);
#endif
	for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++)
	  RTRLAdjustAlphaAndWeights
	    (MemoBlock[iB].Cell[iC].s_d_InGate_Cell[iiB][iiC].s_d,
	     MemoBlock[iB].Cell[iC].e,
#ifdef LOCAL_ALPHA
	     MemoBlock[iB].Cell[iC].s_d_InGate_Cell[iiB][iiC].alpha,
	     //MemoBlock[iB].Cell[iC].s_d_InGate_Cell[iiB][iiC].h,
#endif
	     MemoBlock[iB].InGate.W.Cell[iiB][iiC],0);
      }
    }
  }
#ifdef FORGET_GATES
  // dw FgGate.
  for(unsigned int iB=BegBlock;iB<EndBlock;iB++) {
    for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++) {
      RTRLAdjustAlphaAndWeights(MemoBlock[iB].Cell[iC].s_d_FgGate_Bias.s_d,
				MemoBlock[iB].Cell[iC].e,
#ifdef LOCAL_ALPHA
				MemoBlock[iB].Cell[iC].s_d_FgGate_Bias.alpha,
				//MemoBlock[iB].Cell[iC].s_d_FgGate_Bias.h,
#endif
				MemoBlock[iB].FgGate.W.Bias,1);
      for(unsigned int iI=0;iI<NbIn;iI++)     
	RTRLAdjustAlphaAndWeights
	  (MemoBlock[iB].Cell[iC].s_d_FgGate_In[iI].s_d,
	   MemoBlock[iB].Cell[iC].e,
#ifdef LOCAL_ALPHA
	   MemoBlock[iB].Cell[iC].s_d_FgGate_In[iI].alpha,
	   //MemoBlock[iB].Cell[iC].s_d_FgGate_In[iI].h,
#endif
	   MemoBlock[iB].FgGate.W.In[iI],0);
#ifdef CONNECT_GATES_TO_S
      for(unsigned int iiC=0;iiC<MemoBlock[iB].MemoBlockSize;iiC++) // From s.
	RTRLAdjustAlphaAndWeights
	  (MemoBlock[iB].Cell[iC].s_d_FgGate_s[iiC].s_d,
	   MemoBlock[iB].Cell[iC].e,
#ifdef LOCAL_ALPHA
	   MemoBlock[iB].Cell[iC].s_d_FgGate_s[iiC].alpha,
	   //MemoBlock[iB].Cell[iC].s_d_FgGate_s[iiC].h,
#endif
	   MemoBlock[iB].FgGate.W.s[iiC],0);
#endif

      for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	  iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) {
#ifdef CONNECT_TO_GATES
	RTRLAdjustAlphaAndWeights
	  (MemoBlock[iB].Cell[iC].s_d_FgGate_InGate[iiB].s_d,
	   MemoBlock[iB].Cell[iC].e,
#ifdef LOCAL_ALPHA
	   MemoBlock[iB].Cell[iC].s_d_FgGate_InGate[iiB].alpha,
	   //MemoBlock[iB].Cell[iC].s_d_FgGate_InGate[iiB].h,
#endif
	   MemoBlock[iB].FgGate.W.InGate[iiB],0);
	RTRLAdjustAlphaAndWeights
	  (MemoBlock[iB].Cell[iC].s_d_FgGate_OutGate[iiB].s_d,
	   MemoBlock[iB].Cell[iC].e,
#ifdef LOCAL_ALPHA
	   MemoBlock[iB].Cell[iC].s_d_FgGate_OutGate[iiB].alpha,
	   //MemoBlock[iB].Cell[iC].s_d_FgGate_OutGate[iiB].h,
#endif
	   MemoBlock[iB].FgGate.W.OutGate[iiB],0);
#endif
	for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++)
	  RTRLAdjustAlphaAndWeights
	    (MemoBlock[iB].Cell[iC].s_d_FgGate_Cell[iiB][iiC].s_d,
	     MemoBlock[iB].Cell[iC].e,
#ifdef LOCAL_ALPHA
	     MemoBlock[iB].Cell[iC].s_d_FgGate_Cell[iiB][iiC].alpha,
	     //MemoBlock[iB].Cell[iC].s_d_FgGate_Cell[iiB][iiC].h,
#endif
	     MemoBlock[iB].FgGate.W.Cell[iiB][iiC],0);
      }
    }
  }
#endif
}

#ifdef UPDATE_WEIGHTS_AFTER_SEQ 
void LSTM::ExecuteWeightChanges(unsigned int BegBlock, 
				 unsigned int EndBlock) {
  // w out units.
  for(unsigned int iO=0;iO<NbOut;iO++) {
    ExecuteWeightChange(Out[iO].W.Bias,1); // From Bias.
#ifndef NO_IN_OUT_SHORTCUTS
    for(unsigned int iI=0;iI<NbIn;iI++) // From Input.
      ExecuteWeightChange(Out[iO].W.In[iI],0);
#endif
#ifdef USE_HIDDEN_UNITS
  for(unsigned int iH=0;iH<NbHidden;iH++)
    ExecuteWeightChange(Out[iO].W.Hidden[iH],0);
#endif
    for(unsigned int iB=BegBlock;iB<EndBlock;iB++) // From Blocks.
      for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++)
	ExecuteWeightChange(Out[iO].W.Cell[iB][iC],0);
  }
#ifdef USE_HIDDEN_UNITS
  // w Hidden units.
  for(unsigned int iH=0;iH<NbHidden;iH++) {
    ExecuteWeightChange(Hidden[iH].W.Bias,1); // From Bias.
    if(CONNECT_HIDDEN2IN)
      for(unsigned int iI=0;iI<NbIn;iI++) // From Input.
	ExecuteWeightChange(Hidden[iH].W.In[iI],0);
    if(CONNECT_HIDDEN2CELLS)
      for(unsigned int iB=BegBlock;iB<EndBlock;iB++) // From Blocks.
	for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++)
	  ExecuteWeightChange(Hidden[iH].W.Cell[iB][iC],0);
  }
#endif
  // w OutGate.
  for(unsigned int iB=BegBlock;iB<EndBlock;iB++) {
    ExecuteWeightChange(MemoBlock[iB].OutGate.W.Bias,1); // From Bias.
    for(unsigned int iI=0;iI<NbIn;iI++) 
      ExecuteWeightChange(MemoBlock[iB].OutGate.W.In[iI],0);
#ifdef CONNECT_GATES_TO_S
    for(unsigned int iiC=0;iiC<MemoBlock[iB].MemoBlockSize;iiC++) // From s.
      ExecuteWeightChange(MemoBlock[iB].OutGate.W.s[iiC],0);
#endif
    for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) {
#ifdef CONNECT_TO_GATES
      ExecuteWeightChange(MemoBlock[iB].OutGate.W.InGate[iiB],1);
      ExecuteWeightChange(MemoBlock[iB].OutGate.W.OutGate[iiB],1);
#endif
      for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++)
	ExecuteWeightChange(MemoBlock[iB].OutGate.W.Cell[iiB][iiC],0);
    }
  }
  // w cells.
  for(unsigned int iB=BegBlock;iB<EndBlock;iB++) {
    for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++) {
#ifdef CELL_BIAS
      ExecuteWeightChange(MemoBlock[iB].Cell[iC].W.Bias,1); // From Bias.
#endif
      for(unsigned int iI=0;iI<NbIn;iI++)     
	ExecuteWeightChange(MemoBlock[iB].Cell[iC].W.In[iI],0);
      for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	  iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) {
	if(!InternalBlockConect && (iiB==iB)) continue;
#ifdef CONNECT_TO_GATES
	ExecuteWeightChange(MemoBlock[iB].Cell[iC].W.InGate[iiB],1);
	ExecuteWeightChange(MemoBlock[iB].Cell[iC].W.OutGate[iiB],1);
#endif
	for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++)
	  ExecuteWeightChange(MemoBlock[iB].Cell[iC].W.Cell[iiB][iiC],0);
      }
    }
  }
  // w InGate.
  for(unsigned int iB=BegBlock;iB<EndBlock;iB++) {
    ExecuteWeightChange(MemoBlock[iB].InGate.W.Bias,1); // From Bias.
    for(unsigned int iI=0;iI<NbIn;iI++)     
      ExecuteWeightChange(MemoBlock[iB].InGate.W.In[iI],0);
#ifdef CONNECT_GATES_TO_S
    for(unsigned int iiC=0;iiC<MemoBlock[iB].MemoBlockSize;iiC++) // From s.
      ExecuteWeightChange(MemoBlock[iB].InGate.W.s[iiC],0);
#endif
    for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) {
#ifdef CONNECT_TO_GATES
      ExecuteWeightChange(MemoBlock[iB].InGate.W.InGate[iiB],1);
      ExecuteWeightChange(MemoBlock[iB].InGate.W.OutGate[iiB],1);
#endif
      for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++)
	ExecuteWeightChange(MemoBlock[iB].InGate.W.Cell[iiB][iiC],0);
    }
  }
#ifdef FORGET_GATES
  // w FgGate.
  for(unsigned int iB=BegBlock;iB<EndBlock;iB++) {
    ExecuteWeightChange(MemoBlock[iB].FgGate.W.Bias,1); // From Bias.
    for(unsigned int iI=0;iI<NbIn;iI++)     
      ExecuteWeightChange(MemoBlock[iB].FgGate.W.In[iI],0);
#ifdef CONNECT_GATES_TO_S
    for(unsigned int iiC=0;iiC<MemoBlock[iB].MemoBlockSize;iiC++) // From s.
      ExecuteWeightChange(MemoBlock[iB].FgGate.W.s[iiC],0);
#endif
    for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) {
#ifdef CONNECT_TO_GATES
      ExecuteWeightChange(MemoBlock[iB].FgGate.W.InGate[iiB],1);
      ExecuteWeightChange(MemoBlock[iB].FgGate.W.OutGate[iiB],1);
#endif   
      for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++)
	ExecuteWeightChange(MemoBlock[iB].FgGate.W.Cell[iiB][iiC],0);
    }
  }
#endif
}
#endif

void LSTM::PatStatistics(struct TPatData &PatData) {
  // Next-In PredictNextIn statistics.
  //   MSEPat=0;
  //   if(NbPredictNextIn>0)
  //     for(unsigned int iO=0;iO<NbIn;iO++) // Error From NextIn.
  //       MSEPat += NextIn[iO].e * NextIn[iO].e;
  //   if(NbPredictClass>0)
  //     for(unsigned int iO=0;iO<NbOut;iO++) // Error From PredictClassOut.
  // 	MSEPat += PredictClassOut[iO].e * PredictClassOut[iO].e;
  //   if(!SetStepTarget && NbPredictOut) MSEPat /= NbPredictOut;
  //   else {
  //     // Find the first two winners for RebberGrammar.
  //     // Find correct class.
  //     while((PatData.SeqList[Seq])[Pat])[NbExtIn+FirstClassNb]!=1) 
  //       FirstClassNb++; // At least one 1 should be there.
  //     SecondClassNb = FirstClassNb;
  //     do {
  //       SecondClassNb++; // Second winner.
  //       if(SecondClassNb>=NbOut) {
  // 	SecondClassNb=FirstClassNb; break; } // No second.
  //     } while((PatData.Seqlist[Seq])[Pat])[NbExtIn+SecondClassNb]!=1);
  //     // Find winner and second.
  //     for(unsigned int iO=1;iO<NbOut;iO++) 
  //       if(Out[iO].y>Out[SecondWinnerNb].y)
  // 	if(Out[iO].y>Out[FirstWinnerNb].y) {
  // 	  SecondWinnerNb=FirstWinnerNb; FirstWinnerNb=iO; 
  // 	} else SecondWinnerNb=iO;
  //     if(FirstClassNb==SecondClassNb) {
  //       if(FirstWinnerNb!=FirstClassNb) PatWrong++;
  //     } else if((FirstWinnerNb!=FirstClassNb || 
  //SecondWinnerNb!=SecondClassNb)&&
  // 	     ((FirstWinnerNb!=SecondClassNb) || 
  //(SecondWinnerNb!=FirstClassNb)))
  //       PatWrong++;
  if(!NoClassesOnlyPredict) {
    if(WinnerTakesAll) {
      // Find correct class (assuming exactly one, marked with 1 !!).
      ClassNb=0;
      while(((PatData.SeqList[Seq])[PatData.NbPat[Seq]-1])[NbExtIn+ClassNb]!=1) 
	if(++ClassNb==PatData.NbPat[Seq]) break;
      // Find winner.
      WinnerNb=0;
      for(unsigned int iO=1;iO<NbOut;iO++) 
	if(Out[iO].y>Out[WinnerNb].y) WinnerNb=iO;
      // Check correct.
      if(WinnerNb!=ClassNb) PatWrong++;
      //cout<<"WinnerNb:"<<WinnerNb<<" ClassNb:"<<ClassNb<<endl;//ddd
    } else { //else WinnerTakesAll
      // Pattern is wrongly classified when error for at least one out-unit
      // is over MaxMSE.
      ThisPatWrong=false;
      for(unsigned int iO=0;iO<NbOut;iO++)
	if(PatData.Generalize) {// Test.
	  if(!(fabs(Out[iO].e)<MaxMSEGenTest)) {ThisPatWrong=true; break;}
	} else {// Train.
	  if(!(fabs(Out[iO].e)<MaxMSE)) {ThisPatWrong=true; break;}
	}
      if(ThisPatWrong) PatWrong++;
#ifdef DO_ONLINE_PAT_PRODUCTION
      else PatCorrect++;
#endif
    } // end else WinnerTakesAll
  } // End if(!NoClassesOnlyPredict)
  //#ifndef DO_ONLINE_PAT_PRODUCTION
  // MSE
  //Do not include the additonal outputs, e.g. with diff.
#ifndef STAT_OUT
  if( (NbOut>1) && !PredictDiff && !PredictAllUntilOffset ) {
    MSEPat=0;
    for(unsigned int iO=0;iO<NbOut;iO++)
      MSEPat+=Out[iO].e * Out[iO].e;
    MSEPat /= NbOut; 
  } else {
#ifdef RMSE
    MSEPat=Out[0].e * Out[0].e; // MSE or MSE root.
#else
    MSEPat=fabs(Out[0].e); 
#endif
    //MSEEpo =sqrt(MSEEpo);// MSE or root MSE;
  }
#else
  //  RMSE from statistical Out units (one dimensioal).
  //cout<<Tri<<"-"<<Seq<<"-"<<Pat<<":"
  //<<"StatOut[0].e"<<StatOut[0].e<<"\n";//ddd
  MSEPat=StatOut[0].e * StatOut[0].e;
#endif
  //MSEPat=sqrt(MSEPat);  
  //  } // End else.
  MSESeq += MSEPat;
  //#endif
  // //    //ddd
  //             cout<<Epo <<"-"<<Seq<<"-"<<SeqOnline 
  //                 <<"-"<<Pat<<" : ";
  //             for(unsigned int Val=0;Val<PatData.NbVal;Val++)
  //               cout<<((PatData.SeqList[Seq])[Pat])[Val]<<" ";
  //             cout<<"->"<<PatCorrect<<"("<<PatWrong<<")";
  // 	    for(unsigned int iO=0;iO<NbOut;iO++)
  // 	      cout<<"("<<Out[iO].y<<"->"<< Out[iO].e<<")";
//    	    cout<<" MSEPat:"<<MSEPat
//                     <<" MSESeq:"<<MSESeq
//                     <<" MSEEpo:"<<MSEEpo;
//                 cout<<"\n"; cout.flush();
  //  	 ////if(SeqOnline>=2) exit(1);
  //ddd
}

void LSTM::SeqStatistics(struct TPatData &PatData) {
#ifndef DO_ONLINE_PAT_PRODUCTION
  //    // MSE
  //     if((NbPredictNextIn>0) || (NbPredictClass>0))
  //     MSESeq /= PatData.NbPat[Seq]; // Last pattern for free.
  //     else {
  //     for(unsigned int iO=0;iO<NbOut;iO++) MSESeq += Out[iO].e * Out[iO].e;
  //     MSESeq /= NbOut; MSESeq = sqrt(MSESeq); 
  //     }
  if(!NoClassesOnlyPredict) {
    ClassesWrong+=PatWrong;//if(PatWrong) ClassesWrong++;
  }
#endif
  if(SetStepTarget) MSESeq /=(PatData.NbPat[Seq]-RandomPredictionStart); 
#if defined(RMSE) || defined(NRMSE)
  MSESeq =sqrt(MSESeq);// MSE or root MSE;
#endif
#ifdef NRMSE
  MSESeq /=PatData.StdDev;
#endif
  MSEEpo += MSESeq;
//       cout<<" MSEPat:"<<MSEPat
//            <<" MSESeq:"<<MSESeq
//            <<" PatData.StdDev :"<<PatData.StdDev
//            <<" MSEEpo:"<<MSEEpo<<endl;//ddd
}

void LSTM::EpoStatistics(struct TPatData &PatData) {
  NewBest=0;
  if(!NoClassesOnlyPredict) {
#ifdef DO_ONLINE_PAT_PRODUCTION
    if(SetStepTarget) PatCorrect/=PatData.NbSeq;
#ifdef BUFFER_EPO_LOG
    TrainErrEpoLogBuf.AddValue(Epo, PatCorrect);
#else
    WriteLogfileEpo(ERRORLOGFILE,PatCorrect);
#endif
    if(PatCorrect>MaxPatCorrectTrain) {
      MaxPatCorrectTrain=PatCorrect; NewBest=1; }
#else
    if(ClassesWrong<MinClassesWrong) { 
      MinClassesWrong=ClassesWrong; NewBest=1; }
#ifdef BUFFER_EPO_LOG
    TrainErrEpoLogBuf.AddValue(Epo, ClassesWrong);
#else
    WriteLogfileEpo(ERRORLOGFILE, ClassesWrong);
#endif
  }// end !NoClassesOnlyPredict
  //ClassesWrongRel = ClassesWrong/PatData.NbSeq;
  // Alpha = AlphaBase + AlphaError * ClassesWrongRel;
  // Record the dLogError to decide when to add an other MemoBlock.
  if(GrowNet) {
    NbLogedErr++;
    unsigned int devisor;
    if(NbLogedErr>LogErrRecSize) devisor = LogErrRecSize; 
    else devisor = NbLogedErr;
    LogErrRecMean = ((devisor-1)*LogErrRecMean+ClassesWrong)/devisor;
    if(NbLogedErr>PartLogErrRecSize) devisor = PartLogErrRecSize; 
    else devisor = NbLogedErr;
    PartLogErrRecMean =((devisor-1)*PartLogErrRecMean+ClassesWrong)/devisor;
    if((NbLogedErr>LogErrRecSize) && (NbLogedErr>LogErrRecSize)) 
      // Decide when to add an other MemoBlock.
      if(PartLogErrRecMean>=LogErrRecMean) {
	// Add memoblock, all same size.
	if(GrowFirstOrder) 
	  AddMemoBlock(-1,-1,MemoBlock[0].MemoBlockSize);
	else // Grow cascate or fully connected.
	  AddMemoBlock(0,-1,MemoBlock[0].MemoBlockSize);
	LogErrRecMean=0; PartLogErrRecMean=0; NbLogedErr=0;
      }
    WriteLogfileEpo(GROWLOGFILE, LogErrRecMean, PartLogErrRecMean);
  }
#endif
  // MSE for the training has to be compleated before a test is run.
#ifdef DO_ONLINE_PAT_PRODUCTION
  MSEEpo /= (SeqOnline+1);
#else
  if(PatData.NbSeq>1) MSEEpo /= PatData.NbSeq;   
#endif
  if(MSEEpo<MinMSEEpoTrain) {
    MinMSEEpoTrain=MSEEpo; NewBest=1; }
#ifdef BUFFER_EPO_LOG
  TrainMSEEpoLogBuf.AddValue(Epo, MSEEpo);
#else
  WriteLogfileEpo(MSELOGFILE, MSEEpo);
#endif
  // Some more statistics.
#ifdef DO_WEIGHT_STATISTICS
#ifdef DO_ONLINE_PAT_PRODUCTION
  WeightMean/=PatCount*NbWeights;
#else
  WeightMean/=PatData.NbPatTotal*NbWeights;
#endif
  //AlphaMean/=PatData.NbPatTotal*NbWeights;
  //AlphaStd/=PatData.NbPatTotal*NbWeights; 
  //AlphaStd-=SQR(AlphaMean); AlphaStd=sqrt(AlphaStd);
  WriteLogfileEpo(WEIGHTMEAN_LOGFILE, WeightMean);
  WriteLogfileEpo(ALPHAMEAN_LOGFILE, AlphaMean, AlphaStd);//ddd
  WeightMean=0; AlphaMean=0; AlphaStd=0;
#endif
  // Test the net on the Test-set. At the end not to disturb statistics.
  if(TestEach_Epo>0) {
    if((unsigned int)(Epo % TestEach_Epo) == 0) Test(); }
  else {
#ifdef DO_ONLINE_PAT_PRODUCTION
    if((PatCorrect==MaxPatCorrectTrain) && // Better/Best in training.
       (LastTestEpo+TestMaxEach_Epo<=Epo))  { // Not test too much.
      Test(); }
#else
    if((LastTestEpo+TestMinEach_Epo<=Epo) || 
       //       (NewBest && (ClassesWrong==MinClassesWrong) || 
	((NewBest)
	)&&//Better/Best train.
       (LastTestEpo+TestMaxEach_Epo<=Epo))  { // Not test too much.
      Test(); 
      //cout<<"Tested!"<<endl;//ddd
    }
#endif	 
//        	cout << " Test: Epo:" << Epo 
//  	     << " NewBest:" << NewBest
//        	     << " LastTestEpo=Epo:" << LastTestEpo
//        	     << " TestMaxEach_Epo:" << TestMaxEach_Epo
//        	     << " TestMinEach_Epo:" << TestMinEach_Epo << endl; //ddd
    // Write the weight file for good test results.
    // 	if(PatCorrectTest>500) { //ddd
    // 	  sprintf(cBuf, "w.Test%d.%d-%d",
    // 		  Tri,Epo,(unsigned int)PatCorrectTest); 
    // 	  WriteWeightFile(cBuf);
    // 	}
  }
}


unsigned int LSTM::Test() 
{
  // For clamp loging.
  //cout<<"Test...."<<endl;
  char tmpBuf[64];//ddd
  // Save network state after training.
  if(NoResetBeforeTest) { ResetNet(1,0); } //Save state. 
  // Do the test ClampOutPredictSteps times 
  // to calc the MSE for each step with clamp.
  unsigned int clampEpo=0;
#ifdef CLAMPING
  for(clampEpo=0;(clampEpo<=ClampOutPredictSteps);
      clampEpo+=ClampOutPredictSteps){
    //clampEpo++){
    // Leave loop when not clamping at all.
    if(!ClampOutPredictSteps && clampEpo) {
	cout<<ClampOutPredictSteps<<"break"
	    <<"Train "<<Epo <<"-" //ddd
	    <<Seq<<"-"<<SeqOnline<<"-"<<Pat<<" :"<<clampEpo<<endl;//ddd;
	break;
      } else {
	//cerr<<ClampOutPredictSteps<<"go"<<endl;//ddd
      }
      //NoClampingLoop=true;
    //
#endif
  //  start_ctime=clock();
  //  tmp_time = time(NULL);
//    time_t  TimeNow = time(NULL);//ddd
    //cout <<"Epo:"<<Epo<<" Test....time now:"<<ctime(&TimeNow)<<endl; //ddd
    if (TestEpochs==0) return 0;
    LastTestEpo=Epo; RandomPredictionStart=0;
    unsigned int ClassesWrongTmp = ClassesWrong; // Save the train value.
    double PatCorrectTmp = PatCorrect; // Save the train value.
    unsigned int SeqOnlineTmp = SeqOnline;
    unsigned int SeqOnlinePatCountTmp = SeqOnlinePatCount;
    double MSEEpoTmp = MSEEpo; 
    ClassesWrong=0; TestMSEEpo=0; PatCorrectTest=0;
    // Test Epo loop.
    for(TestEpo=0;TestEpo<TestEpochs;TestEpo++) {
      PatCorrect=0; SeqOnline=0; SeqOnlinePatCount=0; MSEEpo=0;
      GeneralizationStaus=-1;
#ifdef DO_ONLINE_PAT_PRODUCTION
      // This Seq will be overwritten, we init the Nb variables here.
      ONLINE_PAT_FUNCNAME(TestData, true);
#else
#ifdef PATTERN_GENERATION_FROM_DATA_SEQWISE
      if(Generate_Pat) Set_RefData(&TestDataOrg,false,false,true);
#endif
#endif
      for(Seq=0;Seq<TestData.NbSeq;FreezeSeqLoop ? Seq=0 : Seq++) {//Seq loop.
	MSESeq=0; PatWrong=0; ClampOut=false; Clamping=false; 
	ClampPatStart=0; ClampPatStop=0;
	// Restore state saved after training.
	// This only work if no intermediate train epos with clamping are run.
	if(NoResetBeforeTest) { ResetNet(0,1); } // Restore state.
	else  // Reset network.
#ifndef DO_ONLINE_PAT_PRODUCTION
	  ResetNet(0,0);
#else
	if(!SeqOnline) ResetNet(0,0); //For sequential online
#endif
	// Generate a new seq (or not).
#ifdef DO_ONLINE_PAT_PRODUCTION
#if defined(NO_RESET_AFTER_ONLINE_SEQ) || defined(SETSTEPTARGET_BUT_COUNT_SEQUENCEWISE)
	if(SeqOnline) ONLINE_PAT_FUNCNAME(TestData, false);
	else ONLINE_PAT_FUNCNAME(TestData, true);
#else
	ONLINE_PAT_FUNCNAME(TestData, true);
#endif
#else
#ifdef PATTERN_GENERATION_FROM_DATA_SEQWISE
	if(Generate_Pat) {
	  GeneratePattern(TestData, Pat_FuncName);
	  // Fix NbPat of actual seq,
	  // because TPatternManager does not know seq.
	  TestData.NbPat[Seq] = TestData.NbPat[0];
	}
#endif
#endif
	for(Pat=0;Pat<TestData.NbPat[Seq];
	    clampEpo ? Pat+=PredictionOffset : Pat++) { //Pat loop.
	  // Dump before the forward pass.//ddd
	  //if(clampEpo==ClampOutPredictSteps)
	  //sprintf(tmpBuf,"Test.C%d",clampEpo);//ddd
	  //	  Dump_Inner_States("Test", TestData); //ddd
	  //Dump_Inner_States(tmpBuf,TestData); //ddd 
	  //if(Reverse_Input) {
	  //  	  ResetNet(0,0); //Reset before each Pat.
	  //  	  PatReInStart=Pat;
	  //  	  if(Pat>Reverse_Input) PatReInStop=Pat-Reverse_Input;
	  //  	  else PatReInStop=0;
	  //  	  // Pat is unsigned!!
	  //  	  for(;(Pat>=PatReInStop)&&(Pat<=PatReInStart);Pat--) {
//  	    ForwardPass(TestData,0,0); // Pat loop, don't calc s_d.
	  //  	  }
	  //  	  Pat=PatReInStart;
	  //	} else
#ifdef CLAMPING
	  if(clampEpo && ClampOutPredictSteps) {
	    if(!Clamping) {
	      Clamping=true; // Once with external input.
	      ClampPatStart=Pat; // Set Start and Stop.
	      ClampPatStop=Pat+clampEpo*PredictionOffset;
	      if(ClampPatStop>TestData.NbPat[Seq]) {//Train set ends.
		ClampPatStop=TestData.NbPat[Seq]; }
	    } else if(!ClampOut) { // Second pass: Clamp now.
	      // Clamping already or start now.
	      ResetNet(1,0); //Save state.
	      ClampOut=true;
	    }
	    if(ClampOut || Clamping) {
	      if(Pat>=ClampPatStop) { // Relieve clamp and set back.
	      Clamping=false; ClampOut=false;
	      ResetNet(0,1); // Restore state.
	      
	      // Set back if not one free run test clamp (laser dat set A).
	      if(ClampOutPredictSteps<TestData.NbPat[Seq]) 
		Pat=ClampPatStart;
	      //if(clampEpo==ClampOutPredictSteps)//ddd
	      //if(Pat>2) Pat--;
	      //cout<<Pat<<endl;//ddd 
	      continue; // finish and restart loop;
	      }
	    }
	}
#endif
	  ForwardPass(TestData,0,0); // Pat loop, don't calc s_d.
	  // Calc output units' error instead of doing the Backpass, if.....
	  if(((Pat==TestData.NbPat[Seq]-1) || SetStepTarget ||
	      (NbPredictNextIn>0) || (NbPredictClass>0)) 
#ifndef TARGET_777
	   && (((TestData.SeqList[Seq])[Pat])[NbExtIn]!=-777)
#endif
	   ){
	  for(unsigned int iO=0;iO<NbOut;iO++)
	    Out[iO].e=
	      ((TestData.SeqList[Seq])[Pat])[NbExtIn+iO] - Out[iO].y;
#ifdef STAT_OUT
	  for(unsigned int iO=0;iO<NbStatOut;iO++) {
	    StatOut[iO].t=((TestData.SeqList[Seq])[Pat])[NbExtIn+NbOut+iO];
	    StatOut[iO].e=StatOut[iO].t-StatOut[iO].y;
	  }
#endif
	  if(!clampEpo  //ddd Laser data.
	     || (clampEpo==ClampOutPredictSteps)
	     //|| (clampEpo==100) //ddd Laser data.
	    || (Pat+PredictionOffset>=ClampPatStop)
	    || (ClampOutPredictSteps>=TestData.NbPat[Seq])
	       )
	    PatStatistics(TestData);
	  //ddd
	  //     	  cout <<"Test "<<Epo<<"-"<<TestEpo<<"-"<<Seq<<"-"
	  //   	       <<SeqOnline<<"-"<<Pat<<":"; 
	  //     	  for(unsigned int Val=0;Val<TestData.NbVal;Val++) 
	  //     	    cout << ((TestData.SeqList[Seq])[Pat])[Val] << " ";
	  //     	  cout << "->" << PatCorrect << "(" << PatWrong << ")"
	  //   	       << "PCT:" << PatCorrectTest
	  //     	       << "MSEPat:" << MSEPat << "TestMSEEpo:" << TestMSEEpo;
	  //     	  cout << "\n"; //cout.flush();
	  //ddd
	  //if(/*(Epo<MaxEpochs) && StopLern /*&& (Seq<10*/) { //ddd
	  //if(OutputDebug && !TestEpo) { 
	  // Dump_Inner_States("Train", TrainData); //ddd
	  //sprintf(cBuf,"DumpAll.log.%d-%d-%d-%d-%d",//ddd
	  //    Tri,Epo,Seq,SeqOnline,Pat);//ddd
	  //DumpAll(cBuf); //ddd
	  //} //ddd
#ifdef DO_ONLINE_PAT_PRODUCTION
	  if(SetStepTarget) {
#ifndef SETSTEPTARGET_BUT_COUNT_SEQUENCEWISE
	    if(!PatWrong) 
	      ONLINE_PAT_FUNCNAME(TestData, false); 
	    else break; // PatWrong.
#else
	    if(!PatWrong) 
	      ; // Generate seq only before Pat loop.
	    else break; // PatWrong.
#endif
	    //if(ReberGrammarState==-1) ResetNet(0,0);
	  }
#endif
	} // end of PatStatistics loop (replaces the backward pass).
	//if(OutputDebug/*(Epo<MaxEpochs) && StopLern /*&& (Seq<10)*/) { //ddd
	//   Dump_Inner_States("Train", TestData); //ddd
	if((OutputDebug || (DumpBestNotLastTest && NewBest)) && !TestEpo) { 
	  //if(!PatWrong || (GeneralizationStaus!=-1) )//ddd
	  // Remove test dumps from the same trial.
	  if((Pat==0)&&(DumpBestNotLastTest)) {
	    sprintf(cBuf,"rm Test.*.log.%d-*",Tri);
	    //cout <<cBuf<<" Epo:"<<Epo<<endl; //ddd
	    system(cBuf);
	  }
	  if(Seq==5) {
	    if((clampEpo==ClampOutPredictSteps)
	       || (clampEpo==0) ) {
	      
	      sprintf(tmpBuf,"Test.C%d",clampEpo);
	      Dump_Inner_States(tmpBuf, TestData);
	      //Dump_Inner_States("Test", TestData);
	      //	  Dump_Inner_States("Test", TestData); //ddd 
	      //Dump_Inner_States(tmpBuf,TestData); //ddd 
	      
	      // 	  sprintf(cBuf,"DumpAll.log.%d-%d-%d-%d-%d",//ddd
	      // 		  Tri,Epo,Seq,SeqOnline,Pat);//ddd
	      // 	  DumpAll(cBuf); //ddd
	    }
	  }
       	}
//  	if(ClampOutPredictSteps) {
//  	  ClampOut=false;
//  	  ResetNet(0,1); // Restore state.
//  	  Pat=ClampPatStart;
//  	}
//  	if(clampEpo) {//ddd
//  	  DisplayNet(TestData); //ddd
//  	  //DisplayWeights(); 
//  	  KeyCheck();//ddd
//  	}//ddd
      } // End Pat loop.
      SeqStatistics(TestData);
#ifdef DO_ONLINE_PAT_PRODUCTION
#ifndef SETSTEPTARGET_BUT_COUNT_SEQUENCEWISE
      if(!SetStepTarget)
#endif
	if(!PatWrong) {
	  SeqOnline++;
	  if(PatCorrect>=MaxOnlineStreamLengthTest) break; 
	} else break;
#else
#ifdef TEST_GENERALIZATION
      //cout<<"TEST_GENERALIZATION...."<<endl;
      // Set the Test_Generalize in LSTM.par to start of trainset.
      //    	cout<<" before: "
      //  	    << "Generate_Pat_NbLoopAllSeqs_Test:"
      //  	    <<Generate_Pat_NbLoopAllSeqs_Test
      //    	    <<" GeneralizationStaus:"<<GeneralizationStaus
      //    	    <<" Test_Generalize:"<<Test_Generalize
      //  	    <<" Seq:"<<Seq
      //  	    <<" PatWrong:"<<PatWrong
      //  	    <<" ClassesWrong:"<<ClassesWrong
      //    	    <<endl;//ddd
      // Note (end) region of correct seqs.

      if(Generate_Pat) {
	if( (PatWrong && (GeneralizationStaus==-1) && 
	     (Seq>Test_Generalize) ) // Give up when Trainset not right.
	    || (Seq==TestData.NbSeq-1) ) {
	  if(GeneralizationStaus!=-1) { //Too good.
	    Seq=TestData.NbSeq; PatWrong=1;
	  } else {
	    ClassesWrong=0; break; } }//Stop trying(ClassesWrong reverse).
	  
	if( (!PatWrong && (GeneralizationStaus==-1)) || 
	    (PatWrong && (GeneralizationStaus!=-1)) ) 
	  {
	    // Open LogFile.
	    sprintf(cBuf,"%s.%d", TEST_STATUS_LOGFILE, Tri );
	    fstream pF;
	    if( OpenFile( &pF,cBuf, ios::app ) ) { return 1; }

	    if(!PatWrong) { // Start region.
	      GeneralizationStaus=Seq; }
	    else { //PatWrong: End region.
	      if(GeneralizationStaus!=-1) { //(ClassesWrong if seqs right)
	      if(
		 //Start new if region under testset and now incorrect.
		 (GeneralizationStaus < Test_Generalize) &&  
		 (Seq-1<Test_Generalize)) 
		// Continue if just n=1 incorrect.
		//		   (GeneralizationStaus==0) && (Seq-1==0)) 
		{ 
		  GeneralizationStaus=-1; 
		} else {
		  ClassesWrong=(unsigned int)(Seq-GeneralizationStaus); 
		  pF<<Epo<<" "<<GeneralizationStaus<<" "<<Seq-1<<"\n"; } }
	    CloseFile(&pF); break; }
	  CloseFile(&pF);
	}
	//    	  cout<<" after: "
	//    	      <<" GeneralizationStaus:"<<GeneralizationStaus
	//    	      <<" Test_Generalize:"<<Test_Generalize
	//  	      <<" Seq:"<<Seq
	//  	      <<" PatWrong:"<<PatWrong
	//  	      <<" ClassesWrong:"<<ClassesWrong
	//    	      <<endl;//ddd
      } else {}
#endif
#endif
    } // End Seq loop.
#ifdef DO_ONLINE_PAT_PRODUCTION
    MSEEpo /= (SeqOnline+1);
#else
    if(TestData.NbSeq>1) MSEEpo /= TestData.NbSeq;   
#endif
    TestMSEEpo += MSEEpo;
    PatCorrectTest+=PatCorrect; 
  } // End TestEpochs loop.
  //cout << "End of Test." << endl; //ddd
  if(TestEpochs>1) TestMSEEpo /= TestEpochs;
#ifdef DO_ONLINE_PAT_PRODUCTION
  PatCorrectTest/=TestData.NbSeq*TestEpochs;
  //PatCorrectTest=PatCorrect; // Remember the test value.
#ifdef BUFFER_EPO_LOG
  if(!NoClassesOnlyPredict)
    TestErrEpoLogBuf.AddValue(Epo,PatCorrectTest);
  TestMSEEpoLogBuf.AddValue(Epo,TestMSEEpo);
#else
  if(!NoClassesOnlyPredict)
    WriteLogfileEpo(TEST_ERRORLOGFILE,PatCorrectTest);
  if(clampEpo) {
    sprintf(cBuf,"%s.%d.C%d",TEST_MSELOGFILE,Tri,clampEpo);// Open LogFile.
    WriteLogfileEpo(cBuf,TestMSEEpo);
  } else
    WriteLogfileEpo(TEST_MSELOGFILE,TestMSEEpo);
#endif
#else // We leave out the TestEpochs for the non online case.
  TestClassesWrong = ClassesWrong; // Remember the test value.
#ifdef BUFFER_EPO_LOG
  if(!NoClassesOnlyPredict)
    TestErrEpoLogBuf.AddValue(Epo, TestClassesWrong);
  TestMSEEpoLogBuf.AddValue(Epo,TestMSEEpo);
#else
  if(!NoClassesOnlyPredict)
    WriteLogfileEpo(TEST_ERRORLOGFILE, TestClassesWrong);
  if(clampEpo) {
    sprintf(cBuf,"%s.%d.C%d",TEST_MSELOGFILE,Tri,clampEpo);// Open LogFile.
    WriteLogfileEpo(cBuf,TestMSEEpo);
  } else
    WriteLogfileEpo(TEST_MSELOGFILE, TestMSEEpo);
#endif
#endif
  // Reconstruct the train values.
  ClassesWrong = ClassesWrongTmp; // Reconstruct the train value for stop.
  PatCorrect = PatCorrectTmp; // Reconstruct the train value for stop.
  SeqOnline = SeqOnlineTmp;
  SeqOnlinePatCount = SeqOnlinePatCountTmp;
  MSEEpo = MSEEpoTmp;
  //ddd
  //finish_ctime=clock(); // ddd . See how fast
  //cout<<"Test Epo:"<<Epo<<" sec:"
    //      <<difftime(finish_time,tmp_time)<<endl;//ddd
  //  <<((double)(finish_ctime-start_ctime)/CLOCKS_PER_SEC)
  //  <<endl;//ddd
  //TimeNow = time(NULL);//ddd
  //cout<<"Epo:"<<Epo<<" Test done, time now:"<<ctime(&TimeNow)<<endl; //ddd
  //ddd
#ifdef CLAMPING
  } //End clampEpo.
#endif
  cout<<"Test done."<<endl;
#ifdef DO_ONLINE_PAT_PRODUCTION
  return((unsigned int) PatCorrectTest);
#else
  return TestClassesWrong;
#endif
}


void LSTM::FreeRunTest() {
  // Let the net run free without input or with 
  // the output clamped to the input. For TestData.NbPat[0] steps.
  // Dump_logs, but no statistics and errors...
  ResetNet(0,0); ClampOut=true; Clamping=true; Seq=0;
  ClampPatStart=0; ClampPatStop=0;
  for(Pat=0;Pat<TestData.NbPat[Seq];Pat+=PredictionOffset) {
//      if((unsigned int)((Pat-ClampPatStart)/PredictionOffset)
//         >=ClampOutPredictSteps ) {
//        Pat=ClampPatStart;
//      }
    ClampPatStart=Pat+PredictionOffset;//hackhackhack.
    ClampPatStop=Pat+3*PredictionOffset;//hackhackhack.
    ForwardPass(TestData,0,0); 
#ifdef STAT_OUT
    for(unsigned int iO=0;iO<NbStatOut;iO++) {
      StatOut[iO].t=((TestData.SeqList[Seq])[Pat])[NbExtIn+NbOut+iO]; }
#endif
    //Dump_Inner_States("FreeRun", TestData); //ddd
  }
    ClampOut=false; Clamping=false;
    ClampPatStart=0; ClampPatStop=0;
}

char LSTM::WriteWeightFile(char *FileName) {
  fstream pF;
  if (OpenFile(&pF,FileName, ios::out)) return 1;
  pF.precision(PRECISION); 
  WriteWeightStream(&pF,(void (*)(iostream *,double &))&d2s,
		    true,"\n");
  return CloseFile(&pF);
}

char LSTM::WriteWeightLog() { //char *FileName) {
  sprintf(cBuf, "w.log.%d",Tri);
  fstream pF;
  if(OpenFile(&pF,cBuf, ios::app)) return 1;
  pF.precision(PRECISION); 
  pF<<Epo<<" ";
  WriteWeightStream(&pF,(void (*)(iostream *,double &))&d2s,true," ");
  return CloseFile(&pF);
}

char LSTM::LoadWeightFile(char *FileName) {
  fstream pF;
  if (OpenFile(&pF,FileName, ios::in)) return 1;
  WriteWeightStream(&pF,(void (*)(iostream *, double &))&s2d,
		    false,"\n");
  return CloseFile(&pF);
}

char LSTM::DumpAll(char *FileName) {
  fstream pF;// = new ifstream(); 
  if (OpenFile(&pF,FileName, ios::out)) return 1;
  pF.precision(PRECISION);
  pF << "weights\n\n";
  WriteWeightStream(&pF,(void (*)(iostream *, double &))&d2s,
		    true,"\n");
  WriteNetStream(&pF,TrainData);
  return CloseFile(&pF);
}
 
char LSTM::WriteWeightStream(iostream *s,
			      void(*f)(iostream *s, double &d),
			      bool WriteWhiteSpace, 
			      const char WhiteSpaceChar[2]) {
  //  cout<<"WriteWeightStream..."<<endl;//ddd
#if defined(CONNECT_TO_GATES) || !defined(CELL_BIAS)
  double zero=0; //Dummy zeros to fill the weight matrix with not exist. w..
#endif
  // Freds Weight Matrix(row=to;column=from): 
  // 1xBias(1 bias cell to all),(s),In,Hidden,
  // MemoBlocks(Ingate,Outgate,FgGate,cells),Out.
  // Zero rows or columns are left out.
  // To/from Bias. (nothing)
  // To/from In. (nothing)
  // To/from s. (nothing)
  // To/from all Hidden. (!!!nothing from the Gates)
  // Without CONNECT_TO_GATES defined  these weights are not printed.
  // Forget gates never have outgoing connections, so never printed.
  for(unsigned int o=0;o<ORDER_WEIGHT;o++) {
    // To Hidden.
#ifdef USE_HIDDEN_UNITS
    for(unsigned int iH=0;iH<NbHidden;iH++) {
      f(s,Hidden[iH].W.Bias.w[o]);if(WriteWhiteSpace) *s<<" ";//From Bias.
      if(CONNECT_HIDDEN2IN)
	for(unsigned int iI=0;iI<NbIn;iI++) { // From In.
	  f(s,Hidden[iH].W.In[iI].w[o]); if(WriteWhiteSpace) *s << " "; }
    if(CONNECT_HIDDEN2CELLS)
      for(unsigned int iB=0;iB<NbMemoBlocks;iB++) {// From Blocks.
	for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++){//From Cells.
#ifdef CONNECT_TO_GATES
	  // Add zeros for the missing connections from the gates.
	  f(s,zero); if(WriteWhiteSpace) *s << " "; // From InGate.
	  f(s,zero); if(WriteWhiteSpace) *s << " "; // From OutGate.
#ifdef FORGET_GATES
	  f(s,zero); if(WriteWhiteSpace) *s << " "; // From FgGate.
#endif
#endif
	  f(s,Hidden[iH].W.Cell[iB][iC].w[o]); 
	  if(WriteWhiteSpace) *s << " "; }
      }
    } //if(WriteWhiteSpace) *s << WhiteSpaceChar ;
#endif
    // To MemoBlocks.
    for(unsigned int iB=0;iB<NbMemoBlocks;iB++) {
      // To InGate.
      f(s,MemoBlock[iB].InGate.W.Bias.w[o]); 
      if(WriteWhiteSpace) *s << " "; // From Bias. 
      for(unsigned int iI=0;iI<NbIn;iI++) {
	f(s,MemoBlock[iB].InGate.W.In[iI].w[o]);  
	if(WriteWhiteSpace) *s << " "; } // From In.
#ifdef CONNECT_GATES_TO_S
      for(unsigned int iiC=0;iiC<MemoBlock[iB].MemoBlockSize;iiC++) { // From s.
	f(s,MemoBlock[iB].InGate.W.s[iiC].w[o]);  
	if(WriteWhiteSpace) *s << " "; } 
#endif
      for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	  iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) { // From Blocks.
#ifdef CONNECT_TO_GATES
	f(s,MemoBlock[iB].InGate.W.InGate[iiB].w[o]);  
	if(WriteWhiteSpace) *s << " ";  // From InGate.
	f(s,MemoBlock[iB].InGate.W.OutGate[iiB].w[o]);  
	if(WriteWhiteSpace) *s << " "; // From OutGate.
#endif
	for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++) {
	  f(s,MemoBlock[iB].InGate.W.Cell[iiB][iiC].w[o]);  
	  if(WriteWhiteSpace) *s << " "; }// From Cells.
      } if(WriteWhiteSpace) *s << WhiteSpaceChar;
      // OutGate weights.
      f(s,MemoBlock[iB].OutGate.W.Bias.w[o]);  
      if(WriteWhiteSpace) *s << " "; // From Bias. 
      for(unsigned int iI=0;iI<NbIn;iI++) {     
	f(s,MemoBlock[iB].OutGate.W.In[iI].w[o]);  
	if(WriteWhiteSpace) *s << " "; } // From In.
#ifdef CONNECT_GATES_TO_S
      for(unsigned int iiC=0;iiC<MemoBlock[iB].MemoBlockSize;iiC++) { // From s.
	f(s,MemoBlock[iB].OutGate.W.s[iiC].w[o]);  
	if(WriteWhiteSpace) *s << " "; } 
#endif
      for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	  iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) { // From Blocks.
#ifdef CONNECT_TO_GATES
	f(s,MemoBlock[iB].OutGate.W.InGate[iiB].w[o]);  
	if(WriteWhiteSpace) *s << " "; // From InGate.
	f(s,MemoBlock[iB].OutGate.W.OutGate[iiB].w[o]);  
	if(WriteWhiteSpace) *s << " "; // From OutGate.
#endif
	for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++) {
	  f(s,MemoBlock[iB].OutGate.W.Cell[iiB][iiC].w[o]);  
	  if(WriteWhiteSpace) *s << " "; } // From Cells.
      } if(WriteWhiteSpace) *s << WhiteSpaceChar;
#ifdef FORGET_GATES
      // To FgGate.
      f(s,MemoBlock[iB].FgGate.W.Bias.w[o]); 
      if(WriteWhiteSpace) *s << " "; // From Bias. 
      for(unsigned int iI=0;iI<NbIn;iI++) {
	f(s,MemoBlock[iB].FgGate.W.In[iI].w[o]);  
	if(WriteWhiteSpace) *s << " "; } // From In.
#ifdef CONNECT_GATES_TO_S
      for(unsigned int iiC=0;iiC<MemoBlock[iB].MemoBlockSize;iiC++) { // From s.
	f(s,MemoBlock[iB].FgGate.W.s[iiC].w[o]);  
	if(WriteWhiteSpace) *s << " "; } 
#endif
      for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	  iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) { // From Blocks.
#ifdef CONNECT_TO_GATES
	f(s,MemoBlock[iB].FgGate.W.InGate[iiB].w[o]);  
	if(WriteWhiteSpace) *s << " ";  // From InGate.
	f(s,MemoBlock[iB].FgGate.W.OutGate[iiB].w[o]);  
	if(WriteWhiteSpace) *s << " "; // From OutGate.
#endif
	for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++) {
	  f(s,MemoBlock[iB].FgGate.W.Cell[iiB][iiC].w[o]);  
	  if(WriteWhiteSpace) *s << " "; }// From Cells.
      } if(WriteWhiteSpace) *s << WhiteSpaceChar;
#endif   
      // To Cells.
      for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++) {
	// Cell weights.
#ifdef CELL_BIAS
	f(s,MemoBlock[iB].Cell[iC].W.Bias.w[o]);  
#else
	f(s,zero); 
#endif
	if(WriteWhiteSpace) *s << " "; // From Bias. 
	for(unsigned int iI=0;iI<NbIn;iI++) {     
	  f(s,MemoBlock[iB].Cell[iC].W.In[iI].w[o]);  
	  if(WriteWhiteSpace) *s << " "; }// From In.
	for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	    iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) {// From Blocks.
#ifdef CONNECT_TO_GATES
	  f(s,MemoBlock[iB].Cell[iC].W.InGate[iiB].w[o]);  
	  if(WriteWhiteSpace) *s << " ";// From InGate.
	  f(s,MemoBlock[iB].Cell[iC].W.OutGate[iiB].w[o]);  
	  if(WriteWhiteSpace) *s << " ";// From OutGate.
#endif
	  for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++) {
	    f(s,MemoBlock[iB].Cell[iC].W.Cell[iiB][iiC].w[o]);  
	    if(WriteWhiteSpace) *s << " "; } //From Cells
	} if(WriteWhiteSpace) *s << WhiteSpaceChar;
      } //if(WriteWhiteSpace) *s << WhiteSpaceChar;
    }
    // TO Out units.
    for(unsigned int iO=0;iO<NbOut;iO++) { 
      //if(WriteWhiteSpace) *s << WhiteSpaceChar;
      f(s,Out[iO].W.Bias.w[o]); if(WriteWhiteSpace) *s << " "; // From Bias.
#ifndef NO_IN_OUT_SHORTCUTS
      for(unsigned int iI=0;iI<NbIn;iI++) {     
	f(s,Out[iO].W.In[iI].w[o]); if(WriteWhiteSpace) *s << " "; }// From In.
#endif
#ifdef USE_HIDDEN_UNITS
      for(unsigned int iH=0;iH<NbHidden;iH++) { // From hidden.
	f(s,Out[iO].W.Hidden[iH].w[o]); if(WriteWhiteSpace) *s << " "; }
#endif
      for(unsigned int iB=0;iB<NbMemoBlocks;iB++) { // From Blocks.
#ifdef CONNECT_TO_GATES
	// Add zeros for the missing connections to the gates.
	f(s,zero); if(WriteWhiteSpace) *s << " "; // From InGate.
	f(s,zero); if(WriteWhiteSpace) *s << " "; // From OutGate.   
#endif
	for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++) {
	  f(s,Out[iO].W.Cell[iB][iC].w[o]);  
	  if(WriteWhiteSpace) *s << " "; }//From Cells
      }if(WriteWhiteSpace) *s << WhiteSpaceChar;
    }
    //For matrix2X tool.In any case also in log.
    if(WriteWhiteSpace) *s << "\n"; 
  } // End o loop.
  return 0;
}

void LSTM::DisplayNet(struct TPatData &PatData) {
  WriteNetStream(NULL,PatData);
}

void LSTM::WriteNetStream(iostream *s, struct TPatData &PatData) {
  stringstream DisplayBuf;
  // Flags for DisplayBuf
  DisplayBuf.precision(PRECISION);
  cWidth = cPrecision+6;
  if(!s) {
    DisplayBuf.fill(' ');
    DisplayBuf.setf(ios::showpos);
    DisplayBuf.setf(ios::left, ios::adjustfield);
  }
    // Start writing into the buffer.
    DisplayBuf<<" Tri:" << Tri << "/" << NbTrials
	       << " Epo:" << Epo << "/" << MaxEpochs
	       << " Seq:" << Seq << "/" << PatData.NbSeq
	       << " Pat:" << Pat << "/" << PatData.NbPat[Seq] 
	      << "\n"
	       << " ClassesWrong:" << ClassesWrong
	       << " PatWrong: " << PatWrong
	       << " MSEEpo: " << MSEEpo
	       << " MSESeq: " << setw(cWidth) << MSESeq
	       << " MSEPat: " << setw(cWidth) << MSEPat
	      << "\n"
	      <<" Clamping: "<<Clamping
	      <<" ClampOut: "<<ClampOut
	      <<" ClampPatStart: "<<ClampPatStart
	      <<" ClampPatStop: "<<ClampPatStop
	       << "\n";
    //if(Pat>PatData.NbPat[Seq]-1) Pat=PatData.NbPat[Seq]-1;
    DisplayBuf<<"Targets: "; 
    for(unsigned int iO=0;iO<NbOut;iO++) // Targets
      DisplayBuf<<((PatData.SeqList[Seq])[Pat])[NbExtIn+iO]<<" "; 
#ifdef STAT_OUT
    for(unsigned int iO=0;iO<NbStatOut;iO++) 
      DisplayBuf<<StatOut[iO].t<<" "; 
#endif
  // Dump the buffert output for the input window.
  if(TimeWindowSize  && OutWindow && (ClampOut || Clamping) 
     //&& (TimeWindowStepSize==(unsigned int)PredictionOffset) 
     ) {
    DisplayBuf<<"\n";
    DisplayBuf<<"OutWindow ("
	      <<(unsigned int)((Pat-ClampPatStart)/PredictionOffset)<<"): ";
    for(unsigned int i=0;i<ClampOutPredictSteps;i++) 
      DisplayBuf<<OutWindow[i]<<" ";
    
  } else DisplayBuf<<"?";
  DisplayBuf<<"\n";
  DisplayBuf<<"Inputs  : ";
  for(unsigned int iI=0;iI<NbIn;iI++) {
    DisplayBuf<<((PatData.SeqList[Seq])[Pat])[iI]<<" ";
  }
  DisplayBuf<<"\n";
    DisplayBuf<<"CurInput: ";
    for(unsigned int iI=0;iI<NbIn;iI++) {
	for(unsigned int o=0;o<ORDER_WEIGHT;o++) // ORDER_WEIGHT loop.
	  DisplayBuf<<CurInput[iI][o]<<" ";
    }
    DisplayBuf<<"\n";
    DisplayBuf<<"\t net input\n"; // net input.
#ifdef USE_HIDDEN_UNITS
    for(unsigned int iH=0;iH<NbHidden;iH++) 
	DisplayBuf<<Hidden[iH].net << " ";  // Hidden.net
    DisplayBuf<<"\t Hidden.net\n";
#endif

    for(unsigned int iB=0;iB<NbMemoBlocks;iB++) {
      //DisplayBuf<<setw(cWidth); 
      DisplayBuf<<MemoBlock[iB].InGate.net << " "; // InGate.net
      //DisplayBuf<<setw(cWidth); 
      DisplayBuf<<MemoBlock[iB].OutGate.net << " "; // OutGate.net
#ifdef FORGET_GATES
      //DisplayBuf<<setw(cWidth); 
      DisplayBuf<<MemoBlock[iB].FgGate.net << " "; // FgGate.net
#endif
      for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++){//Cell.net
	//DisplayBuf<<setw(cWidth); 
	DisplayBuf<<MemoBlock[iB].Cell[iC].net << " "; 
      DisplayBuf<<"\n";
      }
    }
    //Out.net
    for(unsigned int iO=0;iO<NbOut;iO++) DisplayBuf<<Out[iO].net << " ";
    DisplayBuf<<"\n";
    DisplayBuf<<"\t activations\n"; // activations.
#ifdef USE_HIDDEN_UNITS
    //Hidden.
    for(unsigned int iH=0;iH<NbHidden;iH++) {
      for(unsigned int o=0;o<ORDER_WEIGHT;o++) // ORDER_WEIGHT loop.
	DisplayBuf<<Hidden[iH].y[o] << " "; } // Hidden.y
    DisplayBuf<<"\t Hidden.y\n";
#endif
    // Memo blocks.
    for(unsigned int iB=0;iB<NbMemoBlocks;iB++) {
      //DisplayBuf<<setw(cWidth); 
      DisplayBuf<<MemoBlock[iB].InGate.y << " "; // InGate.y
      //DisplayBuf<<setw(cWidth); 
      DisplayBuf<<MemoBlock[iB].OutGate.y << " "; // OutGate.y
#ifdef FORGET_GATES
      //DisplayBuf<<setw(cWidth); 
      DisplayBuf<<MemoBlock[iB].FgGate.y << " "; // FgGate.y
#endif
      for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++) { // Cell.y
	//DisplayBuf<<setw(cWidth); 
	for(unsigned int o=0;o<ORDER_WEIGHT;o++) // ORDER_WEIGHT loop.
	  DisplayBuf<<MemoBlock[iB].Cell[iC].y[o] << " "; 
      }
      DisplayBuf<<"\n";
    }
    //Out.y
    for(unsigned int iO=0;iO<NbOut;iO++) DisplayBuf<<Out[iO].y << " ";
    DisplayBuf<<"\n";
#ifdef STAT_OUT
    for(unsigned int iO=0;iO<NbStatOut;iO++) 
      DisplayBuf<<StatOut[iO].y<< " ";
    DisplayBuf<<"\n";
#endif
    DisplayBuf<<"\t net  g  (h)  s[o]\n"; // cellstates.
    for(unsigned int iB=0;iB<NbMemoBlocks;iB++) {
      for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++) { // Cell.s
	//DisplayBuf<<setw(cWidth); 
	DisplayBuf<< MemoBlock[iB].Cell[iC].net << " "
		  << MemoBlock[iB].Cell[iC].g << " "
#ifndef H_LINEAR
		  << MemoBlock[iB].Cell[iC].h << " "
#endif
	  ; 
	for(unsigned int o=0;o<ORDER_WEIGHT;o++) // ORDER_WEIGHT loop.
	  DisplayBuf<<MemoBlock[iB].Cell[iC].s[0] << " ";
      }
      DisplayBuf<<"\n";
    }
  // Errors.
  DisplayBuf<<"\t outerror Out.e\n";
  for(unsigned int iO=0;iO<NbOut;iO++)
    DisplayBuf<<Out[iO].e << " "; // Out.e
#ifdef STAT_OUT
    for(unsigned int iO=0;iO<NbStatOut;iO++) 
      DisplayBuf<<StatOut[iO].e<< " ";  // StatOut.e
#endif
  DisplayBuf<<"\n";
  DisplayBuf<<"\t outdelta Out.delta\n";
  for(unsigned int iO=0;iO<NbOut;iO++)
    DisplayBuf<<Out[iO].delta << " "; // Out.delta
  DisplayBuf<<"\n";
#ifdef USE_HIDDEN_UNITS
  //Hidden.delta
  DisplayBuf<<"\t Hidden.delta\n";
  for(unsigned int iH=0;iH<NbHidden;iH++)
    DisplayBuf<<Hidden[iH].delta << " "; // Hidden.y
  DisplayBuf<<"\n";
  DisplayBuf<<"\t Hidden.e\n";
  for(unsigned int iH=0;iH<NbHidden;iH++)
    DisplayBuf<<Hidden[iH].e << " "; // Hidden.y
  DisplayBuf<<"\n";
#endif
  DisplayBuf<<"\t outgateerror OutGate.delta\n";
  for(unsigned int iB=0;iB<NbMemoBlocks;iB++) { 
    //DisplayBuf<<setw(cWidth); 
    //DisplayBuf<< MemoBlock[iB].OutGate.e << " "; // OutGate.e
    //DisplayBuf<<setw(cWidth);
    DisplayBuf<<MemoBlock[iB].OutGate.delta; // OutGate.delta
    DisplayBuf<<"\n";
  }
  DisplayBuf<<"\t cellerror\n";
  for(unsigned int iB=0;iB<NbMemoBlocks;iB++) { 
    // Cells.e_unscaled
    for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++) {
      //DisplayBuf<<setw(cWidth); 
      DisplayBuf<<MemoBlock[iB].Cell[iC].e_unscaled << " ";
    }
    DisplayBuf<<"\n";
  } 
  DisplayBuf<<"\t stateerror\n";
  for(unsigned int iB=0;iB<NbMemoBlocks;iB++) { 
    for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++) { // Cells.e
      //DisplayBuf<<setw(cWidth); 
      DisplayBuf<<MemoBlock[iB].Cell[iC].e << " "; 
    }
    DisplayBuf<<"\n";
  } 
 
  //#ifdef DEBUG
  for(unsigned int o=0;o<ORDER_WEIGHT;o++) { // ORDER_WEIGHT loop.
  // s_d for the InGate.
  DisplayBuf<<"\t ingatepartials\n";  
  for(unsigned int iB=0;iB<NbMemoBlocks;iB++) { // MemoBlocks
    for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++) {
      DisplayBuf<<MemoBlock[iB].Cell[iC].s_d_InGate_Bias.s_d[o] << " ";
      for(unsigned int iI=0;iI<NbIn;iI++)     
	DisplayBuf<<MemoBlock[iB].Cell[iC].s_d_InGate_In[iI].s_d[o] << " ";
#ifdef CONNECT_GATES_TO_S
      for(unsigned int iiC=0;iiC<MemoBlock[iB].MemoBlockSize;iiC++) // From s.
	DisplayBuf<<MemoBlock[iB].Cell[iC].s_d_InGate_s[iiC].s_d[o] << " ";
#endif
      for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	  iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) {
#ifdef CONNECT_TO_GATES
	DisplayBuf<<
	  MemoBlock[iB].Cell[iC].s_d_InGate_InGate[iiB].s_d[o] << " ";
	DisplayBuf<<
	  MemoBlock[iB].Cell[iC].s_d_InGate_OutGate[iiB].s_d[o] << " ";
#endif
	for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++)
	  DisplayBuf 
	    << MemoBlock[iB].Cell[iC].s_d_InGate_Cell[iiB][iiC].s_d[o] << " ";
      }
      DisplayBuf<<"\n";
    }
  }  
#ifdef FORGET_GATES
  // s_d for the FgGate.
  DisplayBuf<<"\t FgGatepartials\n";  
  for(unsigned int iB=0;iB<NbMemoBlocks;iB++) { // MemoBlocks
    for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++) {
      DisplayBuf<<MemoBlock[iB].Cell[iC].s_d_FgGate_Bias.s_d[o] << " ";
      for(unsigned int iI=0;iI<NbIn;iI++)     
	DisplayBuf<<MemoBlock[iB].Cell[iC].s_d_FgGate_In[iI].s_d[o] << " ";
#ifdef CONNECT_GATES_TO_S
      for(unsigned int iiC=0;iiC<MemoBlock[iB].MemoBlockSize;iiC++) // From s.
	DisplayBuf<<MemoBlock[iB].Cell[iC].s_d_FgGate_s[iiC].s_d[o] << " ";
#endif
      for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	  iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) {
#ifdef CONNECT_TO_GATES
	DisplayBuf<<
	  MemoBlock[iB].Cell[iC].s_d_FgGate_InGate[iiB].s_d[o] << " ";
	DisplayBuf<<
	  MemoBlock[iB].Cell[iC].s_d_FgGate_OutGate[iiB].s_d[o] << " ";
#endif
	for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++)
	  DisplayBuf 
	    << MemoBlock[iB].Cell[iC].s_d_FgGate_Cell[iiB][iiC].s_d[o] << " ";
      }
      DisplayBuf<<"\n";
    }
  }  
#endif
  // s_d for the cells.
  DisplayBuf<<"\t cellpartials\n";  
  for(unsigned int iB=0;iB<NbMemoBlocks;iB++) { // MemoBlocks
    for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++) {
      DisplayBuf<<MemoBlock[iB].Cell[iC].s_d_Bias.s_d[o] << " ";
      for(unsigned int iI=0;iI<NbIn;iI++)     
	DisplayBuf<<MemoBlock[iB].Cell[iC].s_d_In[iI].s_d[o] << " ";
      for(unsigned int iiB=MemoBlock[iB].BegSrcBlockNb;
	  iiB<MemoBlock[iB].EndSrcBlockNb;iiB++) {
#ifdef CONNECT_TO_GATES
	DisplayBuf<<MemoBlock[iB].Cell[iC].s_d_InGate[iiB].s_d[o] << " ";
	DisplayBuf<<MemoBlock[iB].Cell[iC].s_d_OutGate[iiB].s_d[o] << " ";
#endif
	for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++)
	  DisplayBuf<<MemoBlock[iB].Cell[iC].s_d_Cell[iiB][iiC].s_d[o] << " ";
      }
      DisplayBuf<<"\n";
    }
  }
  DisplayBuf<<"\n";
  } // End ORDER_WEIGHT loop.
  //#endif

  DisplayBuf << ends;

  // Print the buffer.
  //char DisplayStr[1024];
  //string DisplayString = DisplayBuf.str();
  //strcpy( DisplayStr, DisplayString.c_str() );
  //DisplayStr = DisplayString.c_str();
  // DisplayStr = DisplayBuf.str();

  if(!s) { //  Print to stdout.
    printf("\033[0;H\007"); // position curser (0,0) (row,colm)
    printf("\033[0J\007"); // Clear Below
    cout << DisplayBuf; cout.flush(); // Str to Buf (2005-04)
  } else *s << DisplayBuf; // Str to Buf (2005-04)

  //free(DisplayStr);
  //DisplayBuf.clear(); DisplayBuf.seekg(0); DisplayBuf.seekp(0);
}


void LSTM::DisplayWeights() 
{
  stringstream WeightBuf; WeightBuf.precision(8);
  WriteWeightStream(&WeightBuf,(void (*)(iostream *, double &))&d2s,
		    true,"\n");
  // Print the buffer.
  WeightBuf << ends;

  //char * WeightBufStr = WeightBuf.str(); 
  //free(WeightBufStr);
  
  // This should work, but is too cumbersome:
//   char WeightBufStr[1024];
//   string WeightBufString = WeightBuf.str();
//   strcpy( WeightBufStr, WeightBufString.c_str() );
//   cout << WeightBufStr; 

  cout << WeightBuf; 


  cout.flush(); // Finally flush all out (including the buffers).
  //WeightBuf.clear(); WeightBuf.seekg(0); WeightBuf.seekp(0);
}

char LSTM::Dump_Inner_States(char *FileName, TPatData &PatData) {
  bool FileCloseStatus=false; 
  fstream pF;
#ifdef DO_ONLINE_PAT_PRODUCTION
  // Log seq beginnings and endings.
  //if(!SetStepTarget) {
  //if(Pat==0) {
  //sprintf(cBuf,"SeqBegEnd.log.%d-%d-%d",Tri,Epo,Seq); 
  //fstream pF;
  //if(OpenFile(&pF,cBuf, ios::app)) return 1;
  // ddd TempOrderEx6
  //       pF << SeqOnlinePatCount << " " <<  PatData.NbClass[0] << "\n";
  //       pF << SeqOnlinePatCount+t1 << " " <<  XNotYt1 << "\n";
  //       pF << SeqOnlinePatCount+t2 << " " <<  XNotYt2 << "\n";
  //       pF << SeqOnlinePatCount+t3 << " " <<  XNotYt3 << "\n";
  //CloseFile(&pF);
  //}
  //}
#endif
  // Dump file name
  sprintf(cBuf,"%s.%s.%d-%d-%d",FileName,OUT_LOGFILE,Tri,Epo,Seq); 
  if(OpenFile(&pF,cBuf, ios::app)) return 1;
  pF.precision(4);
  // Dump the activation of the out units.
#ifndef DO_ONLINE_PAT_PRODUCTION
  pF << Pat << " ";
#else
#ifndef SETSTEPTARGET_BUT_COUNT_SEQUENCEWISE
  if(SetStepTarget) pF << Pat << " "; 
  else  pF << SeqOnlinePatCount << " ";
#else
  if(SetStepTarget) pF << PatCorrect << " "; 
#endif
#endif
  // Dump Input.
  for(unsigned int iI=0;iI<NbIn;iI++)
  if(!ClampOut || 1) {  pF<<CurInput[iI][0] << " "; 
  } else { pF<<"? "; }// missing, do not print recurrent output.
  //    } else { // Dump also last External input before free run.
  //      for(unsigned int iI=0;iI<NbIn;iI++) { // Dump last External Input.
  //        if(Pat>=ClampOutPredictSteps-1) pF<<
  //  	((PatData.SeqList[Seq])[Pat-ClampOutPredictSteps+1])[iI]<<" ";}
  //  }
  // Dump targets.
  for(unsigned int iO=0;iO<NbOut;iO++){ 
#ifndef TARGET_777
    if(((PatData.SeqList[Seq])[Pat])[NbExtIn+iO]!=-777)
     if(ClampOut && 0) pF<<"? "; 
     else 
       pF 
	 //<<"TAR"//ddd
	 << ((PatData.SeqList[Seq])[Pat])[NbExtIn+iO] << " "; 
    else pF << "? "; 
#else 
  pF << ((PatData.SeqList[Seq])[Pat])[NbExtIn+iO] << " "; 
#endif
  }
#ifdef STAT_OUT
   for(unsigned int iO=0;iO<NbStatOut;iO++) { 
     if(ClampOut && 0) pF<<"? "; 
     else 
       pF 
	 //<<"SOUT Target"//ddd
	 << StatOut[iO].t << " "; }
#endif
   // Dump Out.
  for(unsigned int iO=0;iO<NbOut;iO++) {
    pF 
      //<<"OUT"//ddd
      << Out[iO].y << " "; }
#ifdef STAT_OUT
  for(unsigned int iO=0;iO<NbStatOut;iO++) { 
    pF
      //<<"SOUT"//ddd
      << StatOut[iO].y << " "; }
#endif
  pF << "\n"; 
  // After clamping: write one separation line.
  if((ClampOut || Clamping) && (Pat==ClampPatStop-PredictionOffset)) {
    pF << "\n"; 
    //pF <<Pat<< " ";
    // Repeat the last in and tar.
    //for(unsigned int i=0;i<NbIn+1*(NbOut+NbStatOut);i++) pF<<"? ";
    //pF << "\n";
  }
  FileCloseStatus = FileCloseStatus || CloseFile(&pF);
  //
  // Dump the cells' inner states s.
  sprintf(cBuf,"%s.%s.%d-%d-%d",FileName,
	  INNERSTATE_DUMP_LOGFILE,Tri,Epo,Seq); 
  //sprintf(cBuf,"%s.%s.%d-%d",FileName, INNERSTATE_DUMP_LOGFILE,Tri,Seq); 
  //  sprintf(cBuf,"%s.%s.%d-%d",FileName, INNERSTATE_DUMP_LOGFILE,Tri,Seq); 
  //sprintf(cBuf,"%s.%s.%d-%d",FileName, INNERSTATE_DUMP_LOGFILE,Tri,Epo); 
  if(OpenFile(&pF,cBuf, ios::app)) return 1;
  pF.precision(4);
#ifndef DO_ONLINE_PAT_PRODUCTION
  pF << Pat << " ";
#else
#ifndef SETSTEPTARGET_BUT_COUNT_SEQUENCEWISE
  if(SetStepTarget) pF << Pat << " "; 
  else  pF << SeqOnlinePatCount++ << " ";
#else
  if(SetStepTarget) pF << PatCorrect << " "; 
#endif
#endif
  for(unsigned int iB=0;iB<NbMemoBlocks;iB++)
    for(unsigned int iC=0;iC<MemoBlock[iB].MemoBlockSize;iC++) {
      for(unsigned int o=0;o<ORDER_WEIGHT;o++) { // ORDER_WEIGHT loop.
	pF << MemoBlock[iB].Cell[iC].s[o] << " "; // s.
	pF << MemoBlock[iB].Cell[iC].y[o] << " "; // y_c.
      }
    }
  pF << "\n";
  // After clamping: write one separation line.
  if((ClampOut || Clamping) && (Pat==ClampPatStop-PredictionOffset)) { 
    pF << "\n"; }
  FileCloseStatus = FileCloseStatus || CloseFile(&pF);
  // Dump the gate activations.
  //  sprintf(cBuf, "%s.%s.%d-%d",FileName,GATE_Y_LOGFILE,Tri,Seq); 
  sprintf(cBuf, "%s.%s.%d-%d-%d",FileName,GATE_Y_LOGFILE,Tri,Epo,Seq); 
  if(OpenFile(&pF,cBuf, ios::app)) return 1;
  pF.precision(6);
#ifndef DO_ONLINE_PAT_PRODUCTION
  pF << Pat << " ";
#else
#ifndef SETSTEPTARGET_BUT_COUNT_SEQUENCEWISE
  if(SetStepTarget) pF << Pat << " "; 
  else  pF << SeqOnlinePatCount << " ";
#else
  if(SetStepTarget) pF << PatCorrect << " "; 
#endif
#endif
  for(unsigned int iB=0;iB<NbMemoBlocks;iB++) { // Gates.
    pF << MemoBlock[iB].InGate.y << " "; // In.
    pF << MemoBlock[iB].OutGate.y << " "; // Out.
#ifdef FORGET_GATES
    pF << MemoBlock[iB].FgGate.y << " "; // Forget.
#endif
  }
  pF << "\n";
  // After clamping: write one separation line.
  if((ClampOut || Clamping) && (Pat==ClampPatStop-PredictionOffset)) { 
    pF << "\n"; }
  FileCloseStatus = FileCloseStatus || CloseFile(&pF);
#ifdef USE_HIDDEN_UNITS
  // Dump the Hidden activations.
  //  sprintf(cBuf, "%s.%s.%d-%d",FileName,HIDDEN_Y_LOGFILE,Tri,Seq); 
  sprintf(cBuf, "%s.%s.%d-%d-%d",FileName,HIDDEN_Y_LOGFILE,Tri,Epo,Seq); 
  if(OpenFile(&pF,cBuf, ios::app)) return 1;
  pF.precision(6);
#ifndef DO_ONLINE_PAT_PRODUCTION
  pF << Pat << " ";
#else
#ifndef SETSTEPTARGET_BUT_COUNT_SEQUENCEWISE
  if(SetStepTarget) pF << Pat << " "; 
  else  pF << SeqOnlinePatCount << " ";
#else
  if(SetStepTarget) pF << PatCorrect << " "; 
#endif
#endif
  for(unsigned int iH=0;iH<NbHidden;iH++) {
    for(unsigned int o=0;o<ORDER_WEIGHT;o++) // ORDER_WEIGHT loop.
      pF << Hidden[iH].y[o] << " "; }
  pF << "\n";
  FileCloseStatus = FileCloseStatus || CloseFile(&pF);
#endif
  return FileCloseStatus;
}

char LSTM::DumpSomeAlphas() {
  sprintf(cBuf,"AlphaDump.log");
  fstream pF;
  if (OpenFile(&pF,cBuf, ios::app)) return 1;
  pF.precision(8);
  pF << Epo << " ";
#ifdef LOCAL_ALPHA
  // Alpha for the InGate.
  for(unsigned int iC=0;iC<MemoBlock[1].MemoBlockSize;iC++) {
    pF << MemoBlock[1].Cell[iC].s_d_InGate_Bias.alpha << " ";
    for(unsigned int iI=0;iI<NbIn;iI++)     
      pF << MemoBlock[1].Cell[iC].s_d_InGate_In[iI].alpha << " ";
    for(unsigned int iiB=0;iiB<1;iiB++) {
      for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++)
	pF 
	  << MemoBlock[1].Cell[iC].s_d_InGate_Cell[iiB][iiC].alpha << " ";
    }
  }
  // OutGate weights alphas.
  pF << MemoBlock[1].OutGate.W.Bias.alpha;  
  pF << " "; // From Bias. 
  for(unsigned int iI=0;iI<NbIn;iI++) {     
    pF << MemoBlock[1].OutGate.W.In[iI].alpha;  
    pF << " "; } // From In.
  // Cells.
  for(unsigned int iC=0;iC<MemoBlock[1].MemoBlockSize;iC++) {
    // Cell weights alphas.
    pF << " "; // From Bias. 
    for(unsigned int iI=0;iI<NbIn;iI++) {     
      pF << MemoBlock[1].Cell[iC].W.In[iI].alpha;  
      pF << " "; }// From In.
    for(unsigned int iiB=0;iiB<1;iiB++) {// From Blocks.
      for(unsigned int iiC=0;iiC<MemoBlock[iiB].MemoBlockSize;iiC++) {
	pF << MemoBlock[1].Cell[iC].W.Cell[iiB][iiC].alpha;  
	pF << " "; } //From Cells
    }
  }
#endif
  pF << "\n";
  return CloseFile(&pF);
}


char LSTM::WriteOutLogFile(char *FileName) {
  fstream pF;
  if (OpenFile(&pF,FileName, ios::app)) return 1;
  pF << Pat << " ";
  for(unsigned int iO=0;iO<NbOut;iO++) 
    pF << Out[iO].y << " "; 
#ifdef STAT_OUT
    for(unsigned int iO=0;iO<NbStatOut;iO++) 
      pF << StatOut[iO].y << " ";
#endif
  pF << "\n";
  return CloseFile(&pF);
}

void LSTM::TopoCheck() {
  cout << " "<< NbCells << " " << NbMemUnits << " " << NbWeights << endl;
  for(unsigned int iB=0;iB<NbMemoBlocks;iB++) { // MemoBlocks
    cout << iB << " : "
	 << MemoBlock[iB].BegSrcBlockNb << "-" 
	 << MemoBlock[iB].EndSrcBlockNb << endl;
  }
  cout << FreezeEndBlock << endl;
}

void LSTM::SetTopoStatistics() {
  // Count the number of gates.
  // For the input gates we count NbCells*Weights, 
  // so a weight to the gate is counted NbCell times.
  unsigned int NbGates=NbMemoBlocks*2;
  NbWeights=0; NbCells=0;
  for(unsigned int iB=0;iB<NbMemoBlocks;iB++) 
    NbCells+=MemoBlock[iB].MemoBlockSize;
  NbMemUnits = NbCells+NbGates;
  // Bias.
  NbWeights += NbMemUnits+NbOut;
#ifndef CELL_BIAS
  NbWeights -= NbCells;
#endif
#ifdef CONNECT_GATES_TO_S
  for(unsigned int iB=0;iB<NbMemoBlocks;iB++) {
    NbWeights += MemoBlock[iB].MemoBlockSize *2;
#ifdef FORGET_GATES
    NbWeights += MemoBlock[iB].MemoBlockSize;
#endif
  }
#endif
  // hidden to out.
  NbWeights += NbCells * NbOut;
  // Start with fully connected memblocks then substract.
  NbWeights += SQR(NbMemUnits);
#ifdef CONNECT_TO_GATES
  NbWeights-=NbGates*NbMemUnits;
#endif
  if(!InternalBlockConect) 
    for(unsigned int iB=0;iB<NbMemoBlocks;iB++) 
      NbWeights-=SQR(MemoBlock[iB].MemoBlockSize);
  // Connetion from the input.
  NbWeights+=(NbMemUnits+NbOut)*NbIn;
  // Now count the weight to the InGate * NbCells as extra weights.
  unsigned int NbWeightsToInGate = NbIn+NbCells+1; // 1 for bias.
#ifdef CONNECT_TO_GATES
  NbWeightsToInGate += NbGates;
#endif
  for(unsigned int iB=0;iB<NbMemoBlocks;iB++)
    NbWeights+=(MemoBlock[iB].MemoBlockSize-1)*NbWeightsToInGate;
#ifdef FORGET_GATES
  // Same as to the InGates only that they are not yet counted at all.
  for(unsigned int iB=0;iB<NbMemoBlocks;iB++)
    NbWeights+=(MemoBlock[iB].MemoBlockSize)*NbWeightsToInGate;
#endif
}

char LSTM::GetOnlineProblemStatistics() { // Not used.
  //unsigned int NbCyl, CylLength;
  sprintf(cBuf,"ONLINE_PAT_FUNCNAME.log");
  fstream pF;
  if(OpenFile(&pF,cBuf, ios::out)) return 1;
  ONLINE_PAT_FUNCNAME(TrainData, true);
  //NbCyl=0;CylLength=0;
  //while(NbCyl<0) {
  ONLINE_PAT_FUNCNAME(TrainData, false);
  //CylLength++;
  //     if(ReberGrammarState==0) { //ddd
  //       NbCyl++;
  //       pF << CylLength  << "\n";
  //       CylLength=0;
  //     }
  //}
  return CloseFile(&pF);
}


void LSTM::renameLogFilesAfterTrial( int Tri ) 
{
  char cBuf[1024];

  if(NbTrials>1) 
    {
      if(DirContainsFile(true, MSELOGFILE)) {
	sprintf(cBuf, "%s.%d", MSELOGFILE,Tri); 
	rename(MSELOGFILE,cBuf); }
      if(DirContainsFile(true, ERRORLOGFILE)) {
	sprintf(cBuf, "%s.%d", ERRORLOGFILE,Tri); 
	rename(ERRORLOGFILE,cBuf); } 
      if(DirContainsFile(true, TEST_ERRORLOGFILE)) {
	sprintf(cBuf, "%s.%d", TEST_ERRORLOGFILE,Tri); 
	rename(TEST_ERRORLOGFILE,cBuf); }
      if(DirContainsFile(true, TEST_MSELOGFILE)) {
	sprintf(cBuf, "%s.%d", TEST_MSELOGFILE,Tri); 
	rename(TEST_MSELOGFILE,cBuf); }
      if(DirContainsFile(true, TEST_STATUS_LOGFILE)) {
	sprintf(cBuf, "%s.%d", TEST_STATUS_LOGFILE,Tri); 
	rename(TEST_STATUS_LOGFILE,cBuf); }
      if(DirContainsFile(true, GROWLOGFILE)) {
	sprintf(cBuf, "%s.%d", GROWLOGFILE,Tri); 
	rename(GROWLOGFILE,cBuf); }
      if(DirContainsFile(true, WEIGHTMEAN_LOGFILE)) {
	sprintf(cBuf, "%s.%d", WEIGHTMEAN_LOGFILE,Tri); 
	rename(WEIGHTMEAN_LOGFILE,cBuf); }
      if(DirContainsFile(true, ALPHAMEAN_LOGFILE)) {
	sprintf(cBuf, "%s.%d", ALPHAMEAN_LOGFILE,Tri); 
	rename(ALPHAMEAN_LOGFILE,cBuf); }
      if(DirContainsFile(true, INNERSTATE_DUMP_LOGFILE)) {
	sprintf(cBuf, "%s.%d", INNERSTATE_DUMP_LOGFILE,Tri); 
	rename(INNERSTATE_DUMP_LOGFILE,cBuf); }
      if(DirContainsFile(true, "AlphaDump.log")) {
	sprintf(cBuf, "%s.%d", "AlphaDump.log",Tri); 
	rename("AlphaDump.log",cBuf); }
    }
}


///////////////////main
//
int main(int argc, char **argv) 
{
  char ExitStatus;
  //Global memory error management called by new
  //set_new_handler(MemoryFull); // __new_handler = &MemoryFull;
  // Log stderr (cerr) into a file.
  //dup2( open( "cerr.log", 
  // O_WRONLY | O_CREAT | O_TRUNC, 0777), STDERR_FILENO );
//  int fdCerr = open("cerr.log", O_WRONLY | O_CREAT | O_TRUNC, 0666);
  //dup2(fdCerr, STDERR_FILENO);
  // The User Interface (UI) is the only static object.
  LSTM lstm;
  ExitStatus = lstm.Run();
  // clean up
  //close(fdCerr);

  return ExitStatus;
}

