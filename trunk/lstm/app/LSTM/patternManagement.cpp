#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <string>
#include <iostream>
#include <iomanip>

#include "patternManagement.h"


#define lrand48 rand
#define drand48 drand
#define M_PI       3.14159265358979323846

double drand();
//{
//    union
//    {
//        double d;
//        unsigned char c[8];
//    } r;
//
//    int i;
//
//    for( i = 0 ; i < 7; ++i )
//    {
//        r.c[i] = rand() & 0xff; 
//    }
//
//    r.c[6] |= 0xf0; 
//    r.c[7] = 0x3f;
//
//    return r.d - 1.;
//}

///////////////////TPatternManager
TPatternManager::TPatternManager() {
  strcpy(cTrainPatFilename, TRAINPATFILE);
  strcpy(cTestPatFilename, TESTPATFILE);
  strcpy(cTrainPatFilenameResult, RESULTTRAINPATFILE);
  strcpy(cTestPatFilenameResult, RESULTTESTPATFILE);
  TrainData.SeqList = NULL;
  TestData.SeqList = NULL;
  TrainData.NbClass = NULL;
  TestData.NbClass = NULL;
  TrainData.NbPat = NULL;
  TestData.NbPat = NULL;
}

TPatternManager::~TPatternManager() {
  //cout<<"~TPatternManager."<<endl;//ddd
  DeletePattern(TrainData,0);
  DeletePattern(TestData,0);
}

void TPatternManager::DeletePattern(struct TPatData &apsPatData, 
				    unsigned int ConstNbPat) {
  unsigned int NbPat;
  if(apsPatData.SeqList) {
    // NULL pseudo PatData where only one seq or one Pat is newed.
    for(unsigned int seq=1;seq<apsPatData.NbSeq;seq++) // Seq loop.
      if(apsPatData.SeqList[seq]==apsPatData.SeqList[0])
	apsPatData.SeqList[seq]=NULL;
    if(ConstNbPat) NbPat=ConstNbPat; else NbPat=apsPatData.NbPat[0];
    for(unsigned int pat=1;pat<NbPat;pat++) // Pat loop.
      if((apsPatData.SeqList[0])[pat]==(apsPatData.SeqList[0])[0])
	(apsPatData.SeqList[0])[pat]=NULL;
    // Start real delete.
    for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) { // Seq loop.
      if(ConstNbPat) NbPat=ConstNbPat; else NbPat=apsPatData.NbPat[seq];
      if(apsPatData.SeqList[seq]) {
	for(unsigned int pat=0;pat<NbPat;pat++) // Pat loop.
	  // points to *double, which is an array of valuls
	  if((apsPatData.SeqList[seq])[pat])
	    delete[] ((apsPatData.SeqList[seq])[pat]);
	// delete pattern (array of values)
	delete[] apsPatData.SeqList[seq];
      }
    }
    delete[] apsPatData.SeqList;
    if(apsPatData.NbPat)delete[] apsPatData.NbPat;//Stored sequenses lengthes.
    if(apsPatData.NbClass) delete[] apsPatData.NbClass;
  }
  apsPatData.SeqList = NULL;
  apsPatData.NbSeq = 0;
  apsPatData.NbClass = NULL;
  apsPatData.NbPat = NULL;
  apsPatData.NbVal = 0;
}

char TPatternManager::NewPatData(struct TPatData &apsPatData, 
				 unsigned int ConstNbPat) {
  // Check if SeqList exists already.
  if(apsPatData.SeqList) return 1;
#ifdef DEBUG
  cerr<<"Create SeqList.\n";
#endif
  // New NbClass for logging to NbSeq (as default) and set it to zero.
  apsPatData.NbClass = new unsigned int[apsPatData.NbSeq];
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) 
    apsPatData.NbClass[seq] =0;
  // Check if all seqs suppose to have the same length or if they are preset.
  if(!apsPatData.NbPat || ConstNbPat) {
    apsPatData.NbPat = new unsigned int[apsPatData.NbSeq];
    for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) 
      apsPatData.NbPat[seq] = ConstNbPat;
  }
  // create the List (array of squences with different numbers of pattern)
  // Deleted in the destructor.
  apsPatData.SeqList = new double**[apsPatData.NbSeq]; 
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) {
    // number pattern
    apsPatData.SeqList[seq] // create the seqs wtih xxx pattern
      = new double*[apsPatData.NbPat[seq]];
    // create the pattern with const value number
    // Deleted in the destructor.
    for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++)
      (apsPatData.SeqList[seq])[pat] = new double[apsPatData.NbVal];
  }
  // Set all to zero.
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++)
    for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++)
      for(unsigned int val=0;val<apsPatData.NbVal;val++)
	(((apsPatData.SeqList[seq])[pat])[val])=0;
  return 0;
}

char TPatternManager::InitPatternClassification(unsigned int NbIn, 
						unsigned int NbOut) {
  InitPatternClassification(TrainData, NbIn, NbOut);
  InitPatternClassification(TestData, NbIn, NbOut);
  return 0;
}
  
char TPatternManager::InitPatternClassification(struct TPatData &apsPatData,
						unsigned int NbIn, 
						unsigned int NbOut) {
  if(!apsPatData.NbClass) 
    apsPatData.NbClass = new unsigned int[apsPatData.NbSeq];
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++)
    // get target class
    for(unsigned int iO=0;iO<NbOut;iO++) 
      if(apsPatData.SeqList[seq][(apsPatData.NbPat[seq])-1][NbIn+iO]) {
	apsPatData.NbClass[seq] = iO;
	break;
      }
  return 0;
}

void TPatternManager::CopyPatData(struct TPatData &apsPatDataDst, 
		       struct TPatData &apsPatDataSrc) {
  apsPatDataDst.NbSeq = apsPatDataSrc.NbSeq;
  apsPatDataDst.NbPat = apsPatDataSrc.NbPat;
  apsPatDataDst.NbPatTotal = apsPatDataSrc.NbPatTotal;
  apsPatDataDst.NbClass = apsPatDataSrc.NbClass;
  apsPatDataDst.NbVal = apsPatDataSrc.NbVal;
  apsPatDataDst.SeqList = apsPatDataSrc.SeqList;
}

char TPatternManager::LoadTrainPattern() {
  //DeletePattern(TrainData,0); // for reload 
  return LoadPattern(TrainData, cTrainPatFilename);
}

char TPatternManager::LoadTestPattern() {
  //DeletePattern(TestData,0); // for reload 
  return LoadPattern(TestData, cTestPatFilename);
}

char TPatternManager::LoadPattern(struct TPatData &apsPatData,
				  char* acPatFilename) {
  char c;
  unsigned int  NbValTemp; // to confirm file consistency
  char cScanned; // for scanning
  double dValue; 
  // Delete apsPatData if exsists.
  DeletePattern(apsPatData,0);
  // open file
  fstream pF;// = new ifstream(); 
  if(OpenFile(&pF,acPatFilename,ios::in)) return 1;
  if(ReadComment(&pF)) return 1;
  lFilePos = pF.tellg(); // save position for rescan
  // scan the PatFile for NbVal, NbSeq  (delimiters: \t or " ",\n,\n\n)
  NbValTemp = 0; // to verify constant pattern lenght
  // scan two times, one to get number of sequences (to new NbPat) 
  // and again to get the lenght of each sequence.
  cScanned = false;
  while (!cScanned) { // scan two times
#ifdef DEBUG
    cerr<<"Scan "<<acPatFilename<<"\n";
#endif
    while (!pF.eof()) {
      pF.setf(ios::skipws); // to skip preceeding white space
      pF>>dValue;
      // after the float scan to look for whitsspace
      // if the skipws flag is set even>>char skips whitespace.
      pF.unsetf(ios::skipws);
      if(!pF.fail()) apsPatData.NbVal++;
      else 
	if(!pF.eof()){ // if(PatFile->fail())
	  cerr<<"Value not valit in "<< acPatFilename<<"\n";
	  //delete pF; 
	  return 1;
	} else if(NbValTemp == 0 ) { // empty file
	  cerr<<"PatFile: "<<acPatFilename<<" empty.\n";
	  //delete pF; 
	  return 0;
 	} else if(apsPatData.NbVal ==0) break; // useless ws before eof
      // Skip invisible white space.
      for(pF>>c;c == ' ' || c == '\t' || c == '\n' 
	    || pF.eof();pF>>c) {
	if(c=='\t' || c==' ') continue; // seperate values
	if(c=='\n' || pF.eof()) // end of a pattern
	  // 2x'\n' or eof : new sequence
	  if(apsPatData.NbVal == 0) {
#ifdef DEBUG
	    cerr<<"NbSeq: "<<apsPatData.NbSeq<<"\t";
	    if(apsPatData.NbPat)
	      cerr<<"NbPat: " 
		  <<apsPatData.NbPat[apsPatData.NbSeq]<<"\t";
	    cerr<<"NbValTemp: "<<NbValTemp<<"\n";
#endif
	    apsPatData.NbSeq++;
	    break; // ignore further white space after sequence end
	  } else { // first '\n'
	    if(apsPatData.NbPat) { // exists only in the second scan
	      // Count the number of sequencec first
	      apsPatData.NbPat[apsPatData.NbSeq]++;
	      // check for constant pattern length
	      if((NbValTemp) && 
		  (NbValTemp != apsPatData.NbVal)) {
		cerr<<"Error in "<<acPatFilename
			   <<" pattern length not constant.\n";
		//delete pF; 
		return 1;
	      }
	    }
	    NbValTemp = apsPatData.NbVal; apsPatData.NbVal = 0;
	  }
      }
      // the next char belongs to the next float 
      // (or was useless white space)
      pF.putback(c);
    }
    if(apsPatData.NbPat) cScanned = true;
    else {
      // new NbPat to store the number of pattern in each sequnce.
      // Deleted in the destuctor.
      apsPatData.NbPat = new unsigned int[apsPatData.NbSeq];
      for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++)
	apsPatData.NbPat[seq] = 0;
      apsPatData.NbSeq = 0;
      apsPatData.NbVal = 0;
      pF.clear(); // unset eof
      pF.seekg(lFilePos); // rewind
    }
  }  
  // all pattern have the same length, but the number odValue pattern 
  // is not constant.
  apsPatData.NbVal = NbValTemp;
  // create the List
  NewPatData(apsPatData,0);
  // finally read in the actual flaots
#ifdef DEBUG
  cerr<<"Init SeqList.\n";
#endif
  pF.clear(); // unset eof
  pF.seekg(lFilePos); // rewind
  pF.setf(ios::skipws); // to skip preceeding white space
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) {
#if defined(DEBUG) && defined(DEBUG_PATTERN)
    cerr<<"\n";
#endif
    for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++) {
#if defined(DEBUG) && defined(DEBUG_PATTERN)
      cerr<<"\n";
#endif
      for(unsigned int val=0;val<apsPatData.NbVal;val++) {
	pF>>(((apsPatData.SeqList[seq])[pat])[val]); // not a cube
#if defined(DEBUG) && defined(DEBUG_PATTERN)
        cerr<<(((apsPatData.SeqList[seq])[pat])[val])<<"\t";
#endif
      } } }
#ifdef DEBUG
  cerr<<"LoadPattern done.\n";
#endif
  // Set global info.
  CountNbPatTotal(apsPatData);
  // check again and close
  return CloseFile(&pF);
}

char TPatternManager::SaveTrainPattern() {
  return SavePatternFile(TrainData, cTrainPatFilenameResult);
}

char TPatternManager::SaveTestPattern() {
  return SavePatternFile(TestData, cTestPatFilenameResult);
}

char TPatternManager::SavePatternFile(struct TPatData &apsPatData, 
				      char* acPatFilename) {
  // open file
  fstream pF;
  if(OpenFile(&pF,acPatFilename, ios::out)) return 1;
  //pF.setf(ios::showpos);
  // write File
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) {
    for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++) {
      for(unsigned int val=0;val<apsPatData.NbVal;val++) {
	pF<<(((apsPatData.SeqList[seq])[pat])[val])<<" ";
      } pF<<'\n'; } pF<<'\n'; }
  // check again and close
  //pF.unsetf(ios::showpos);
  return CloseFile(&pF);
}

char TPatternManager::SavePatternFileSeppFormat(struct TPatData &apsPatData, 
						char* acPatFilename, 
						unsigned int NbInUnits) {
  bool AllStepTarget=true;
  double tartmp;
  // openfile
  fstream pF;
  if(OpenFile(&pF,acPatFilename, ios::out)) return 1;
  pF.precision(3);
  pF.fill(' ');
  pF.setf(ios::showpoint);
  // write File
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) {
    for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++) {
      for(unsigned int val=0;val<NbInUnits;val++) // inputs
	pF<<setw(4)<<(((apsPatData.SeqList[seq])[pat])[val])<<" ";
      for(unsigned int val=NbInUnits;val<apsPatData.NbVal;val++) {// targets.
	if(pat!=apsPatData.NbPat[seq]-1 && !AllStepTarget) 
	  tartmp = 20.00; // Only Last target.
	else tartmp = (((apsPatData.SeqList[seq])[pat])[val]);	
	pF<<setw(5)<<tartmp<<" ";
      }
      pF<<"\n"; 
    } // end pat 
    for(unsigned int val=0;val<apsPatData.NbVal;val++) pF<<"20.00 "; 
    pF<<"\n"; 
  } // end seq
  for(unsigned int val=0;val<apsPatData.NbVal;val++) pF<<"20.00 "; 
  pF<<"\n";
  // check again and close
  return CloseFile(&pF);
}

void TPatternManager::MixTrainSequences() { MixSequences(TrainData); }

void TPatternManager::MixSequences(struct TPatData &apsPatData) {
  double**  SeqTmp;
  unsigned int NbPatTmp, NbClassTmp;
  // We mix 10% (x2) of the sequences
  unsigned int NbMixSeq = (unsigned int) (apsPatData.NbSeq / 10);
  if(!NbMixSeq && (apsPatData.NbSeq>2)) NbMixSeq = 1;
  unsigned int SwitchSeq1, SwitchSeq2;
#ifdef RNN_RUNTIME_DEBUG
  cerr<<"NbSeq: "<<apsPatData.NbSeq <<'\n';
#endif
  for(unsigned int seq=0;seq<NbMixSeq;seq++) {
    // switch two sequence pointers
    SwitchSeq1 = (unsigned int)(lrand48() % apsPatData.NbSeq);
    SwitchSeq2 = (unsigned int)(lrand48() % apsPatData.NbSeq);
#ifdef RNN_RUNTIME_DEBUG
    cerr<<SwitchSeq1<< " <-> "<<SwitchSeq2<<'\n';
#endif
    // Switch data.
    SeqTmp = apsPatData.SeqList[SwitchSeq1];
    apsPatData.SeqList[SwitchSeq1] = apsPatData.SeqList[SwitchSeq2];
    apsPatData.SeqList[SwitchSeq2] = SeqTmp;
    // Switch NbPat.
    NbPatTmp = apsPatData.NbPat[SwitchSeq1];
    apsPatData.NbPat[SwitchSeq1] = apsPatData.NbPat[SwitchSeq2];
    apsPatData.NbPat[SwitchSeq2] = NbPatTmp;
    // Switch NbClass.
    NbClassTmp = apsPatData.NbClass[SwitchSeq1];
    apsPatData.NbClass[SwitchSeq1] = apsPatData.NbClass[SwitchSeq2];
    apsPatData.NbClass[SwitchSeq2] = NbClassTmp;
  }
}

void TPatternManager::CountNbPatTotal(struct TPatData &apsPatData) {
  apsPatData.NbPatTotal=0;
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++)
    apsPatData.NbPatTotal += apsPatData.NbPat[seq];
}

void TPatternManager::
TransPat2StepByStepPredict(unsigned int NbInUnits,
			      unsigned int NbOutUnits,
			      unsigned int NbPredictInputs,
			      unsigned int offset,
			      unsigned int PredictAllUntilOffset) {
  //  cout<<"PredictAllUntilOffset:"<<PredictAllUntilOffset<<endl;
  // We add the input pattern of a step to the output target of
  // the previous one. For test and train pattern.
  TransPat2StepByStepPredict(NbInUnits,NbOutUnits,
			     NbPredictInputs,
			     TrainData,offset,
			     PredictAllUntilOffset);
  TransPat2StepByStepPredict(NbInUnits,NbOutUnits,
			     NbPredictInputs,
			     TestData,offset,
			     PredictAllUntilOffset);
  // save for debug
//    sprintf(cBuf,"Train.Trans.%d.dat",offset); //ddd
//    SavePatternFile(TrainData, cBuf);
//    sprintf(cBuf,"Test.Trans.%d.dat",offset);
//    SavePatternFile(TestData, cBuf);
}		  
  
void TPatternManager::
TransPat2StepByStepPredict(unsigned int NbInUnits,
			   unsigned int NbOutUnits,		
			   unsigned int NbPredictInputs,	   
			   struct TPatData &apsPatData,
			   unsigned int offset, 
			   unsigned int PredictAllUntilOffset) {
  // Put input of pat+oftset into target of pat.
  // The seqences are cut when to offset is over seq end
  // and become empty when the offset>seqlength. 
  // THe units to be predicted are the first NbPredictInputs input units.
  long NewSeqLen;
  unsigned int  NbValNew = apsPatData.NbVal + NbPredictInputs;
  if(PredictAllUntilOffset) 
    NbValNew += NbPredictInputs * (PredictAllUntilOffset-1);
  // NbOutUnits must fit already to the new problem.
  if(NbOutUnits+NbInUnits<NbValNew) {
    cerr<<"TransPat2StepByStepPredict error!\n"
	<<"TrainData.NbVal: "<<TrainData.NbVal<<"\n"
	<<"NbValNew:"<<NbValNew<< "\n"
	<<"NbInUnits :"<<NbInUnits<<"\n"
	<<"NbOutUnits :"<<NbOutUnits<<"\n"
	<<"NbPredictInputs:"<<NbPredictInputs<<"\n"
	<<endl; exit(1); }
  double*** SeqListNew;
  // Create the new List with different NbVal.
  SeqListNew = new double**[apsPatData.NbSeq]; 
  // Create the seqs with NbPat-offset pattern (offset deduced).
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) {
    NewSeqLen = apsPatData.NbPat[seq]-offset; 
    if(NewSeqLen<0) NewSeqLen=0;
    apsPatData.NbPat[seq] = NewSeqLen;
    SeqListNew[seq] = new double*[apsPatData.NbPat[seq]];
    // Create the pattern with new NbVal
    for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++)
      (SeqListNew[seq])[pat] = new double[NbValNew];
  }
#ifdef DEBUG
  cerr<<"TransPat2StepByStepPredict : copy the pat values..."
      <<"TrainData.NbVal: "<<TrainData.NbVal<<"\n"
      <<"NbValNew:"<<NbValNew<< "\n"
      <<"NbInUnits :"<<NbInUnits<<"\n"
      <<"NbOutUnits :"<<NbOutUnits<<"\n"
      <<"NbPredictInputs:"<<NbPredictInputs<<"\n"
      <<endl;
#endif
  // Copy the old values, should be just inputs no targets.
  unsigned int val; // For use within these loops
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) {
    for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++) {
      for(val=0;val<apsPatData.NbVal;val++) // original pats
	(((SeqListNew[seq])[pat])[val])=
	  (((apsPatData.SeqList[seq])[pat])[val]);
      // For PredictAllUntilOffset
      if(!PredictAllUntilOffset) {
	//Add"input"targets.
	for(unsigned int iI=0;iI<NbPredictInputs;iI++,val++) {
	  //if(pat+1<apsPatData.NbPat[seq]) // except for the last pattern
	  (((SeqListNew[seq])[pat])[val])=
	    (((apsPatData.SeqList[seq])[pat+offset])[iI]);
	  //x	if(pat<10)
//  	  cout<<" seq:"<<seq<<" pat:"<<pat//ddd
//  	      <<" val:"<<val<<" iI:"<<iI
//  	      <<"target:"<<((apsPatData.SeqList[seq])[pat+offset])[iI]
//  	      <<endl;//ddd
	}
	// } else {
	//  (((SeqListNew[seq])[pat])[val]) = -777;
      } else {
	//cout<<"PredictAllUntilOffset:"<<PredictAllUntilOffset<<endl;
	// Add all the other targets form t+PredictOffset-1 to t+1.
	for(unsigned int p=PredictAllUntilOffset;p>0;p--)
	  //Add"input"targets
	  for(unsigned int iI=0;iI<NbPredictInputs;iI++,val++){
	    (((SeqListNew[seq])[pat])[val])=
	      (((apsPatData.SeqList[seq])[pat+p])[iI]);
	    //if(pat<10) cout<<" seq:"<<seq<<" pat:"<<pat//ddd
	    //<<" p:"<<p<<" val:"<<val<<" iI:"<<iI<<endl;//ddd
	  }
      }
    }
  }
#ifdef DEBUG
  cerr<<"TransPat2StepByStepPredict : delete the old list..."<<endl;
#endif
  // delete the old list
  if(apsPatData.SeqList) {
    for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) { // Seq loop.
      for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++) // Pat loop.
	// points to *double, which is an array of valuls
	delete[] (apsPatData.SeqList[seq])[pat];
      // delete pattern (array of values)
      delete[] apsPatData.SeqList[seq];
    }
    delete[] apsPatData.SeqList;
  }
  // set the new SeqList and NbVal
  apsPatData.NbVal = NbValNew;
  apsPatData.SeqList = SeqListNew;
}

double TPatternManager::Scale_IODIff(double max, unsigned int valNb) {
  double factor,maxval=0;
  // Determine maxval in train set val[valNb].
  for(unsigned int seq=0;seq<TrainData.NbSeq;seq++)
    for(unsigned int pat=0;pat<TrainData.NbPat[seq];pat++) {
      if(fabs(((TrainData.SeqList[seq])[pat])[valNb]) > maxval)
	maxval=fabs(((TrainData.SeqList[seq])[pat])[valNb]);
    }
  factor=max/maxval;
  // Rescale sets.
  TPatData *apsPatData = &TrainData;
  for(unsigned int d=0;d<2;d++) {
    for(unsigned int seq=0;seq<apsPatData->NbSeq;seq++)
      for(unsigned int pat=0;pat<apsPatData->NbPat[seq];pat++)
	((apsPatData->SeqList[seq])[pat])[valNb] *= factor;
    apsPatData = &TestData;
  }
  //cout<<"factor:"<<factor<<endl;//ddd
  return factor;
}

void TPatternManager::Add_IODiff(double factor) {
  Add_IODiff(factor, TrainData);
  Add_IODiff(factor, TestData);
  // save for debug 
#ifdef DEBUG
  char cTmp[101];
  strcpy(cTmp,cTrainPatFilename);
  SavePatternFile(TrainData, strcat(cTmp,".Trans.Classi"));
  strcpy(cTmp,cTestPatFilename);
  SavePatternFile(TestData, strcat(cTmp,".Trans.Classi"));
#endif
}
  
void TPatternManager::Add_IODiff(double factor,
				 struct TPatData &apsPatData) {
  // Only one dimentional data: add Out -In (then 3 dimentions), No check!
  if(apsPatData.NbVal!=2) {
      cerr<<"Add_IODiff error!\n"
      <<"apsPatData.NbVal (!=2): "<<apsPatData.NbVal<<"\n"
      <<endl; exit(1); 
  }
  unsigned int  NbValNew = apsPatData.NbVal + 1;
  double*** SeqListNew;
  // create the new List with different NbVal
  //cout<<"NbValNew:"<<NbValNew //ddd
  //  <<" apsPatData.NbSeq:"<<apsPatData.NbSeq<<endl;
  SeqListNew = new double**[apsPatData.NbSeq]; 
  //  cout<<"newed...:"<<NbValNew<<endl;//ddd
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) {
    // number pattern
    SeqListNew[seq] // create the seqs wtih xxx pattern
      = new double*[apsPatData.NbPat[seq]];
    // create the pattern with new NbVal
    for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++)
      (SeqListNew[seq])[pat] = new double[NbValNew];
  }
  // copy the values
  unsigned int val; // For use within these loops
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++)
    for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++) {
      for(val=0;val<apsPatData.NbVal;val++) // original pats
	(((SeqListNew[seq])[pat])[val])=
	  (((apsPatData.SeqList[seq])[pat])[val]);
      //plus new val: "output" targets: diff val[1]-val[0]
      ((SeqListNew[seq])[pat])[val] = factor *
	( ((SeqListNew[seq])[pat])[1]-((SeqListNew[seq])[pat])[0] );
    }
  // delete the old list
  if(apsPatData.SeqList) {
    for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) { // Seq loop.
      for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++) // Pat loop.
	// points to *double, which is an array of valuls
	delete[] (apsPatData.SeqList[seq])[pat];
      // delete pattern (array of values)
      delete[] apsPatData.SeqList[seq];
    }
    delete[] apsPatData.SeqList;
  }
  // set the new SeqList and NbVal
  apsPatData.NbVal = NbValNew;
  apsPatData.SeqList = SeqListNew;
}

void TPatternManager::Add_IODiff2Input() {
  Add_IODiff2Input(TrainData); Add_IODiff2Input(TestData); }
  
void TPatternManager::Add_IODiff2Input(struct TPatData &apsPatData) {
  // Expect y(t),y(t+T),dy(t) -> dy(t-1),y(t),y(t+T),dy(t) .
  // dy(0)=y(o) .
  if(apsPatData.NbVal!=3) {
      cerr<<"Add_IODiff2Input error!\n"
      <<"apsPatData.NbVal (!=3): "<<apsPatData.NbVal<<"\n"
      <<endl; exit(1); 
  }
  unsigned int  NbValNew = apsPatData.NbVal + 1;
  double*** SeqListNew;
  // create the new List with different NbVal
  //cout<<"NbValNew:"<<NbValNew //ddd
  //  <<" apsPatData.NbSeq:"<<apsPatData.NbSeq<<endl;
  SeqListNew = new double**[apsPatData.NbSeq]; 
  //  cout<<"newed...:"<<NbValNew<<endl;//ddd
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) {
    // number pattern
    SeqListNew[seq] // create the seqs wtih xxx pattern
      = new double*[apsPatData.NbPat[seq]];
    // create the pattern with new NbVal
    for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++)
      (SeqListNew[seq])[pat] = new double[NbValNew];
  }
  // copy the values: ,0->1,1->2,2->3,new->0.
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++)
    for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++) {
      for(int val=2;val>=0;val--) // original pats, no unsigned here.
	(((SeqListNew[seq])[pat])[val+1])=
	  (((apsPatData.SeqList[seq])[pat])[val]);
      //plus new input val[0]: dy(t-t).
	if(pat==0)// Just y(t) not dy(t-1).
	  ((SeqListNew[seq])[pat])[0] =
	    (((apsPatData.SeqList[seq])[pat])[0]);
	else
	  ((SeqListNew[seq])[pat])[0] =
	    (((apsPatData.SeqList[seq])[pat-1])[2]);
    }
  // delete the old list
  if(apsPatData.SeqList) {
    for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) { // Seq loop.
      for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++) // Pat loop.
	// points to *double, which is an array of valuls
	delete[] (apsPatData.SeqList[seq])[pat];
      // delete pattern (array of values)
      delete[] apsPatData.SeqList[seq];
    }
    delete[] apsPatData.SeqList;
  }
  // set the new SeqList and NbVal
  apsPatData.NbVal = NbValNew;
  apsPatData.SeqList = SeqListNew;
}


void TPatternManager::Trans2IODiff(double factor) {
  Trans2IODiff(factor, TrainData);
  Trans2IODiff(factor, TestData);
}

void TPatternManager::Trans2IODiff(double factor, 
				   struct TPatData &apsPatData) {
  // replace out y by out dy. In is y (one-dimensional).
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++)
    for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++)
      //new val: "output" targets: diff val[1]-val[0]
      ((apsPatData.SeqList[seq])[pat])[1] = factor *
	( ((apsPatData.SeqList[seq])[pat])[1]-
	  ((apsPatData.SeqList[seq])[pat])[0] );
}

void TPatternManager::FlipO1O2(unsigned int val1, unsigned int val2) {
  FlipO1O2(TrainData,val1,val2);
  FlipO1O2(TestData,val1,val2);
}

void TPatternManager::FlipO1O2(struct TPatData &apsPatData,
			       unsigned int val1, unsigned int val2) {
  // Flip val1<->val2 .
  double tmp;
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++)
    for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++) {
      tmp = ((apsPatData.SeqList[seq])[pat])[val1];
      ((apsPatData.SeqList[seq])[pat])[val1] =
	((apsPatData.SeqList[seq])[pat])[val2]; 
      ((apsPatData.SeqList[seq])[pat])[val2] = tmp;
    }
}

void TPatternManager::SplitPredictionOffset(unsigned int offset) {
  SplitPredictionOffset(offset, TrainData);
  SplitPredictionOffset(offset, TestData);
}

void TPatternManager::SplitPredictionOffset(unsigned int offset,
					    struct TPatData &apsPatData) {

  // We assume one sequence and split it into offset seqs 
  // of length ceil((apsPatData.NbPat[0]-offset)/offset).
  double*** SeqListNew;
  unsigned int *NbPatNew, NbPat;
  // create the new List with different NbSeq
  SeqListNew = new double**[offset]; 
  NbPatNew = new unsigned int[offset];
  // cout<<"apsPatData.NbPat[0]:"<<apsPatData.NbPat[0]
  //  <<" offset:"<<offset<<endl;//ddd
  for(unsigned int seq=0;seq<offset;seq++) { 
    // number pattern
    NbPat=(unsigned int)
      ceil(((double)apsPatData.NbPat[0]-seq)/(double)offset);
    NbPatNew[seq] = NbPat;
    //    cout<<"seq:"<<seq<<" NbPat:"<<NbPat<<endl;//ddd
    // create offset number of eqs with NbPat pattern
    SeqListNew[seq] = new double*[NbPat];
    // create the pattern with old NbVal
    for(unsigned int pat=0;pat<NbPat;pat++)
      (SeqListNew[seq])[pat] = new double[apsPatData.NbVal];
  }
  // split the pattern.
    for(unsigned int pat=0;pat<apsPatData.NbPat[0];pat+=offset) {
      NbPat=(unsigned int)(pat/offset);
      for(unsigned int seq=0;seq<offset;seq++) {
	if(pat+seq>=apsPatData.NbPat[0]) break;
	for(unsigned int val=0;val<apsPatData.NbVal;val++) {
//  	  cout<<" pat:"<<pat<<" seq:"<<seq<<" val:"<<val
//  	      <<" NbPat :"<<NbPat
//  	      <<" NbPatNew[seq] :"<<NbPatNew[seq]<<endl;//ddd
	  (((SeqListNew[seq])[NbPat])[val])=
	    (((apsPatData.SeqList[0])[pat+seq])[val]);
	}
      }
    }
  // partly delete the old list
  if(apsPatData.SeqList) {
    for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) { // Seq loop.
      for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++) // Pat loop.
	// points to *double, which is an array of valuls
	delete[] (apsPatData.SeqList[seq])[pat];
      // delete pattern (array of values)
      delete[] apsPatData.SeqList[seq];
    }
    delete[] apsPatData.SeqList;
    delete[] apsPatData.NbPat;
  }
  // Overwrite old PatData:  set new NbSeq, SeqList, NbPat
  apsPatData.NbSeq = offset;
  apsPatData.SeqList = SeqListNew;
  apsPatData.NbPat = NbPatNew;
}

void TPatternManager::Add_TimeWindow(unsigned int InputDim, 
				     unsigned int Size, 
				     unsigned int StepSize) {
  Add_TimeWindow(InputDim,Size,StepSize,TrainData); 
  Add_TimeWindow(InputDim,Size,StepSize,TestData);
}

void TPatternManager::Add_TimeWindow(unsigned int InputDim, 
				     unsigned int Size, 
				     unsigned int StepSize,
				     struct TPatData &apsPatData) {
  // Time window t,t-stepSize,t-2*stepSize,... . Size inputs including t!
  // Size = 1 -> nothing changes.
  // Expect InputDim input, rest outputs.
  // NbPats get reduced.
  // To keep the first TimeWindowSize-1 pattern of the test set
  // TimeWindowSize-1 pattern must be added at the begining of the test set,
  // e.g., from the end of the train set.
  unsigned int NewSeqLen=0;
  unsigned int  NbValNew = apsPatData.NbVal + InputDim*(Size-1);
  double*** SeqListNew;
  // Create the new List with different NbVal.
  SeqListNew = new double**[apsPatData.NbSeq]; 
  // Create the seqs with NbPat-Size*StepSize pattern.
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) {
    NewSeqLen = apsPatData.NbPat[seq]-(Size-1)*StepSize; 
    //OldSeqLen[] = apsPatData.NbPat[seq];
    if(NewSeqLen<0) NewSeqLen=0;
    apsPatData.NbPat[seq] = NewSeqLen;
    SeqListNew[seq] = new double*[apsPatData.NbPat[seq]];
    // Create the pattern with new NbVal
    for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++)
      (SeqListNew[seq])[pat] = new double[NbValNew];
  }
  // Copy the old values and add new ones skip first (Size-1)*StepSize pats.
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) {
    for(unsigned int pat=(Size-1)*StepSize;
	pat<apsPatData.NbPat[seq]+(Size-1)*StepSize;pat++){//old SeqLen.
      for(unsigned int s=0;s<Size;s++) //Inputs. s=0->original pats.
	for(unsigned int val=0;val<InputDim;val++)
	  (((SeqListNew[seq])[pat-(Size-1)*StepSize])[s*InputDim+val])=
	    (((apsPatData.SeqList[seq])[pat-s*StepSize])[val]);
      // Outputs.
      for(unsigned int val=0;val<apsPatData.NbVal-InputDim;val++)
	(((SeqListNew[seq])[pat-(Size-1)*StepSize])[Size*InputDim+val])=
	  (((apsPatData.SeqList[seq])[pat])[InputDim+val]);
    }
  }
  // delete the old list
  if(apsPatData.SeqList) {
    for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) { // Seq loop.
      for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++) // Pat loop.
	// points to *double, which is an array of valuls
	delete[] (apsPatData.SeqList[seq])[pat];
      // delete pattern (array of values)
      delete[] apsPatData.SeqList[seq];
    }
    delete[] apsPatData.SeqList;
  }
  // set the new SeqList and NbVal
  apsPatData.NbVal = NbValNew;
  apsPatData.SeqList = SeqListNew;
//    //ddd
//    for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) { 
//      for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++) {
//      cout<<pat<<" ";
//        for(unsigned int val=0;val<apsPatData.NbVal;val++) {
//  	cout<<((apsPatData.SeqList[seq])[pat])[val]<<" "; }
//        cout<<"\n"; }
//      cout<<"\n"; }
//    exit(0);//ddd
}

void TPatternManager::CalcPatStatisticsAll(unsigned int NbInUnits,
					   unsigned int NbOutUnits) {
  CalcPatStatistics(TrainData,NbInUnits,NbOutUnits);
  CalcPatStatistics(TestData,NbInUnits,NbOutUnits);
}

void TPatternManager::CalcPatStatistics(struct TPatData &apsPatData,
					unsigned int NbInUnits,
					unsigned int NbOutUnits) {
  apsPatData.Mean=0; apsPatData.Variance=0; apsPatData.StdDev=0;
  if(apsPatData.NbVal<NbInUnits) { 
    cerr<<"CalcPatStatistics: NbVal<NbInUnits\n"; exit(0); }
  CountNbPatTotal(apsPatData);
  // First pass: Calc mean.
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++)
    for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++)
      for(unsigned int val=NbInUnits;val<NbInUnits+NbOutUnits;val++) {
	apsPatData.Mean+=((apsPatData.SeqList[seq])[pat])[val]; }
  apsPatData.Mean/=(apsPatData.NbPatTotal*(NbOutUnits));
  // Second pass:
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++)
    for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++)
      for(unsigned int val=NbInUnits;val<NbInUnits+NbOutUnits;val++) {
	apsPatData.Variance+=
	  SQR(((apsPatData.SeqList[seq])[pat])[val]-apsPatData.Mean); }
  apsPatData.Variance/=(apsPatData.NbPatTotal*NbOutUnits-1);
  apsPatData.StdDev=sqrt(apsPatData.Variance);
//    cout<<" NbInUnits:"<<NbInUnits
//        <<" NbOutUnits:"<<NbOutUnits
//        <<" NbPat[0]:"<<apsPatData.NbPat[0] //ddd
//        <<" NbPatTotal:"<<apsPatData.NbPatTotal
//        <<" Mean:"<<apsPatData.Mean
//        <<" Variance:"<<apsPatData.Variance
//        <<" StdDev:"<<apsPatData.StdDev
//        <<endl;
}

///////////////////TPatternGenerator
TPatternGenerator::TPatternGenerator() { 
  RefData.PatData=NULL; ForONLINE = false;
  NbLoopAllSeqs = 1;
  MaxNbPat = 0; MaxNbPatTrain = 0;
  // Task specific variables.
  ReberGrammarSymbol_p1=B; SlidingNBitMemoryPointer=0;
  TrainDataOrg.SeqList = NULL;
  TestDataOrg.SeqList = NULL;
  TrainDataOrg.NbClass = NULL;
  TestDataOrg.NbClass = NULL;
  TrainDataOrg.NbPat = NULL;
  TestDataOrg.NbPat = NULL;
}

TPatternGenerator::~TPatternGenerator() {
  //cout<<"\n"<<MaxNbPat<< "~TPatternGenerator."<<endl;//ddd
  // For the case that we massed around with NbPat[].
  if(MaxNbPatTrain)
    DeletePattern(TrainData,MaxNbPatTrain);
  else
    DeletePattern(TrainData,MaxNbPat);
  DeletePattern(TestData,MaxNbPat);
  //cout<<"~TPatternGenerator regular data freed."<<endl;//ddd
  DeletePattern(TrainDataOrg,0);
  DeletePattern(TestDataOrg,0);
}

void TPatternGenerator::Init_Rand_Seed() {
  // init random generator with actual time.
  time_t t;
  long lseed = (long) time(&t);
  //srand48(lseed);
}

void TPatternGenerator::GeneratePattern(struct TPatData &apsPatData, 
					const char* FunktionName) {
  if(!strcmp(FunktionName,"LongVeryNoisyTimesSeries"))
    LongVeryNoisyTimesSeries(apsPatData);
  else if(!strcmp(FunktionName,"TempOrderEx6"))
    TempOrderEx6(apsPatData);
  else if(!strcmp(FunktionName,"TempXOR"))
    TempXOR(apsPatData);
  else if(!strcmp(FunktionName,"Adding"))
    Adding(apsPatData);
  else if(!strcmp(FunktionName,"Laufband"))
    Laufband(apsPatData);
  else if(!strcmp(FunktionName,"Sinus"))
    Sinus(apsPatData);
  else if(!strcmp(FunktionName,"SpaceToTime"))
    SpaceToTime(apsPatData);
  else if(!strcmp(FunktionName,"Protein2D"))
    Protein2D(apsPatData);
  else if(!strcmp(FunktionName,"TimerClass"))
    TimerClass(apsPatData);
  else if(!strcmp(FunktionName,"TimerClassMulti"))
    TimerClassMulti(apsPatData);
  else if(!strcmp(FunktionName,"AnBn"))
    AnBn(apsPatData);
  else if(!strcmp(FunktionName,"AnBnCn"))
    AnBnCn(apsPatData);
  else if(!strcmp(FunktionName,"a_nb_mB_mA_n"))
    a_nb_mB_mA_n(apsPatData);
  //  else if(!strcmp(FunktionName,"Osci_Class"))Osci_Class(apsPatData);
  else { cerr<<"GeneratePattern, no Function.\n"; exit(1); }
  CountNbPatTotal(apsPatData);
}

void TPatternGenerator::Set_RefData(TPatData *apsPatData, 
				    bool TrainNotTest, bool RandomSample,
				    bool Restart) {
  RefData.PatData=apsPatData;
  RefData.TrainNotTest=TrainNotTest;
  RefData.RandomSample=RandomSample;
  RefData.Restart=Restart;
  RefData.SampleSeq=0;
  RefData.SamplePat=0;
}

void TPatternGenerator::New_OnlinePatData(struct TPatData &apsPatData) {
    apsPatData.NbPatTotal = 1; // Not to divide by zero somewhere.
    // NewPatData by hand.
    // Create and update only one entry in NbPat (and NbClass for logging).
    apsPatData.NbSeq = 1;
    apsPatData.NbPat   = new unsigned int[apsPatData.NbSeq];
    apsPatData.NbPat[0] = MaxNbPat; // Will be changed dynamicly.
    apsPatData.NbClass = new unsigned int[apsPatData.NbSeq];
    apsPatData.NbClass[0] = 0; // Will be changed dynamicly.
    apsPatData.SeqList = new double**[apsPatData.NbSeq];
    // Create only one seq with MaxNbPat with NbVal.
    apsPatData.SeqList[0] = new double*[MaxNbPat];
    // Create Values for the pattern in the first seq.
    for(unsigned int pat=0;pat<MaxNbPat;pat++) {
      (apsPatData.SeqList[0])[pat] = new double[apsPatData.NbVal];
      // Set all to zero.
      for(unsigned int val=0;val<apsPatData.NbVal;val++)
	(((apsPatData.SeqList[0])[pat])[val])=0;
    }
}

void TPatternGenerator::LongVeryNoisyTimesSeries(struct TPatData &apsPatData){
  bool DataExisted;
  unsigned int NbSeq = 8;
  unsigned int NbIn  = 8;
  unsigned int NbPat = 20;
  // New PatData in case it does not yet exist.
  if(apsPatData.SeqList) DataExisted = true;
  else {
    DataExisted = false;
    apsPatData.NbSeq = NbSeq*NbLoopAllSeqs;
    apsPatData.NbVal = NbIn+NbSeq; // NbIn inputs
    NewPatData(apsPatData, NbPat);
  }
  // Always rewrite everything except for the zero targets.
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) {  // Sequence loop.
    // Reset, Set output.
    for(unsigned int val=NbIn;val<apsPatData.NbVal;val++) 
      ((apsPatData.SeqList[seq])[apsPatData.NbPat[seq]-1])[val]=0;
    ((apsPatData.SeqList[seq])[apsPatData.NbPat[seq]-1])
      [NbIn+(unsigned int)(seq % NbSeq)]=1;
    // Set input.
    // Noise.
    for(unsigned int pat=0;pat<apsPatData.NbPat[seq]-1;pat++)
      for(unsigned int val=0;val<NbIn;val++) 
	((apsPatData.SeqList[seq])[pat])[val]=drand48();
    // Info in 3rd pattern.
    for(unsigned int val=0;val<NbIn;val++) 
      ((apsPatData.SeqList[seq])[2])[val]=0;
    ((apsPatData.SeqList[seq])[2])[(unsigned int)(seq % NbSeq)]=1;
  }  
}

void TPatternGenerator::TempOrderEx6(struct TPatData &apsPatData) {
  // IN: B E (Begin,End) X Y (the Info) a b c d (the noise)
  // OUT classes: YY XY YX XX ..
  bool DataExisted;
  bool Simplify = false;
  bool _3times = false;
  bool _4times = false;
  unsigned int NbSeq, NbIn, iClass;
  unsigned int t1Start, t2Start, t3Start, t4Start;
  unsigned int MinSeqLenght, Uncertainty;

  // Init to avoid warnings.
  t1Start=0; t2Start=0; t3Start=0; t4Start=0;
  if(!Simplify) {
    NbIn  = 8;
    NbSeq = 4; // ==NbOut.
    unsigned int ProblemStrech = 5;
    t1Start = ProblemStrech*10;//10
    t2Start = ProblemStrech*50;//50
    MinSeqLenght = ProblemStrech*100;//100
    Uncertainty = ProblemStrech*10; //  min 1
  } else {
    NbIn  = 8;
    NbSeq = 4; // ==NbOut.
    t1Start = 5;
    t2Start = 10;
    MinSeqLenght = 15;
    Uncertainty = 3; //  min 0.
  }
  if(_3times) {
    NbIn  = 8;
    NbSeq = 8; // ==NbOut.
    unsigned int ProblemStrech = 1;
    t1Start = ProblemStrech*10;
    t2Start = ProblemStrech*33;
    t3Start = ProblemStrech*66;
    MinSeqLenght = ProblemStrech*100;
    Uncertainty = ProblemStrech*10; //  min 0.
  }
  if(_4times) {
    NbIn  = 8;
    NbSeq = 16; // ==NbOut.
    t1Start = 10;
    t2Start = 30;
    t3Start = 50;
    t4Start = 70;
    MinSeqLenght = 100;
    Uncertainty = 10; //  min 0.
  }
  // unsigned int NbPat variable: MinSeqLenght+Uncertainty], hence set below.
  // New PatData in case it does not yet exist.
  if(apsPatData.SeqList) DataExisted = true;
  else {
    DataExisted = false;
    apsPatData.NbSeq = NbSeq*NbLoopAllSeqs;
    apsPatData.NbVal = NbIn+NbSeq; // NbIn inputs.
    // All newed with max length, so we don't have to renew them.
    NewPatData(apsPatData, MinSeqLenght+Uncertainty);
    // Set all to zero.
    for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++)
      for(unsigned int pat=0;pat<MinSeqLenght+Uncertainty;pat++)
	for(unsigned int val=0;val<apsPatData.NbVal;val++)
	  (((apsPatData.SeqList[seq])[pat])[val])=0;
  }
  // Always rewrite everything except for the zero targets, E, B.
  // Sequence loop.
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) {
    // Set changing values to zero (before seting new NbPat).
    for(unsigned int pat=1;pat<apsPatData.NbPat[seq];pat++)
      for(unsigned int val=2;val<NbIn;val++) 
	(((apsPatData.SeqList[seq])[pat])[val])=0;
    for(unsigned int val=NbIn;val<apsPatData.NbVal;val++)
      (((apsPatData.SeqList[seq])[apsPatData.NbPat[seq]-1])[val])=0;
    // Set NbPat.
    apsPatData.NbPat[seq] = 
      (unsigned int)(lrand48() % (Uncertainty+1))+MinSeqLenght;
    // Start with the Begin symbol.
    (((apsPatData.SeqList[seq])[0])[0])=1; 
    // Set XY.
    // If ForONLINE=false do all possible clsses ocne in a set.
    if(!ForONLINE) iClass = seq; 
    else { 
      iClass = (unsigned int)(lrand48() % NbSeq); 
      apsPatData.NbClass[seq]=iClass; 
    }
    XNotYt1 = (unsigned int)(iClass % 2);
    if(((unsigned int)(iClass % 4))>1) XNotYt2 =1; else XNotYt2 =0;
    if(((unsigned int)(iClass % 8))>3) XNotYt3 =1; else XNotYt3 =0;
    if(((unsigned int)(iClass % 16))>7) XNotYt4 =1; else XNotYt4 =0;
    t1 = (unsigned int)(lrand48() % (Uncertainty+1))+t1Start;
    t2 = (unsigned int)(lrand48() % (Uncertainty+1))+t2Start;
    t3 = (unsigned int)(lrand48() % (Uncertainty+1))+t3Start;
    t4 = (unsigned int)(lrand48() % (Uncertainty+1))+t4Start;
    (((apsPatData.SeqList[seq])[t1])[2])= XNotYt1;
    (((apsPatData.SeqList[seq])[t1])[3])= !XNotYt1;
    (((apsPatData.SeqList[seq])[t2])[2])= XNotYt2;
    (((apsPatData.SeqList[seq])[t2])[3])= !XNotYt2;
    if(_3times || _4times) {
      (((apsPatData.SeqList[seq])[t3])[2])= XNotYt3;
      (((apsPatData.SeqList[seq])[t3])[3])= !XNotYt3;
    } else XNotYt3=0;
    if(_4times) {
      (((apsPatData.SeqList[seq])[t4])[2])= XNotYt4;
      (((apsPatData.SeqList[seq])[t4])[3])= !XNotYt4;
    } else XNotYt4=0;
    // Set the noise (abcd) except in the first and last patterm
    // and also not then X or Y is set.
    if(!Simplify)
      for(unsigned int pat=1;pat<apsPatData.NbPat[seq]-1;pat++)
	if( (pat!=t1) && (pat!=t2) 
	    && (pat!=t3 || !_3times) 
	    && (pat!=t4 || !_4times) ) 
	  (((apsPatData.SeqList[seq])[pat])
	   [4+(unsigned int)(lrand48() % 4)]) =1;
    // Set the target class.
    (((apsPatData.SeqList[seq])[apsPatData.NbPat[seq]-1])
     [NbIn+XNotYt1+2*XNotYt2+4*XNotYt3+8*XNotYt4]) =1;
    // Set the End trigger.
    (((apsPatData.SeqList[seq])[apsPatData.NbPat[seq]-1])[1]) =1;
  }
}

void TPatternGenerator::TempXOR(struct TPatData &apsPatData) {
  unsigned int NbSeq, NbIn, Dim, In;
  char EndSymbol = 1; // 0 means no end symbol.
  NbIn = 2; // X and Y and end symbol not mentioned here.
  Dim = 2; // Dimension = Nb pat in sequence.
  unsigned int TargetDelay=1500; // Wait with presenting the target.
  bool BinNotLocalOut = false;
  bool RandomClass = true; // Cons Number of seq per class.
  if(BinNotLocalOut) NbSeq = Dim;
  else NbSeq = (unsigned int) pow((double)NbIn,(double)Dim); // ==NbOut
  if(!apsPatData.SeqList) { // New the Seq list.
    apsPatData.NbSeq = NbSeq*NbLoopAllSeqs;
    apsPatData.NbVal = NbIn+EndSymbol+NbSeq;
    NewPatData(apsPatData, Dim+TargetDelay); // Sets all to zero.
  }
  // Always rewrite everything except for the zero targets.
  // Sequence loop.
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) {
    // Set output (first set all values to zero),
    // already here for BinNotLocalOut.
    for(unsigned int val=NbIn;val<apsPatData.NbVal;val++)
      (((apsPatData.SeqList[seq])[apsPatData.NbPat[seq]-1])[val]) =0;
    // Reset class.
    apsPatData.NbClass[seq]=0;
    // Set XY input.
    //cout<<seq<<" : " ;
    for(unsigned int pat=0;pat<Dim;pat++) {
      if(NbSeq<apsPatData.NbSeq  && RandomClass) { // NbLoopAllSeqs>1
	In = (unsigned int)(lrand48() % NbIn); }// Ramdom class.
      else  { // Every class once.
	In = (unsigned int)
	  fmod((double)
	       (floor((double)seq / 
		      (unsigned int)
		      pow((double)NbIn,(double)pat))),(double)NbIn); }
      apsPatData.NbClass[seq] += 
	(unsigned int)(In*pow((double)NbIn,(double)pat));
      //cout<<In<<" ";
      // Reset input than set (Centered).
      for(unsigned int val=0;val<NbIn;val++) //!!!!! end symbol.
	((apsPatData.SeqList[seq])[pat])[val] = 0;//(double)-1/(NbIn-1);
      ((apsPatData.SeqList[seq])[pat])[In] = 1;
      // Set BinNotLocalOut target.
      if(BinNotLocalOut) 
	(((apsPatData.SeqList[seq])[apsPatData.NbPat[seq]-1])
	 [NbIn+EndSymbol+pat]) =In;
    }
    //cout<<" -> "<<apsPatData.NbClass[seq]<<endl;
    if(!BinNotLocalOut) 
      // Set the (locally encoded) target from class (set above).
      (((apsPatData.SeqList[seq])[apsPatData.NbPat[seq]-1])
       [NbIn+EndSymbol+apsPatData.NbClass[seq]]) =1;
    // Set the end symbol (never deleted as seq length stays constant).
    if(EndSymbol) 
       ((apsPatData.SeqList[seq])[apsPatData.NbPat[seq]-1])
       [NbIn] =1;   
  }
}

void TPatternGenerator::Adding(struct TPatData &apsPatData) {
  // Adding 2 numbers that are marked, at times 1-10 and
  // T/2-1. Pat T is marked with E=-1.
  unsigned int NbIn=2, NbOut=1, t1,t2;
  unsigned int T=5000, t2Max=(unsigned int)T/2-1;
  unsigned int Uncertainty = (unsigned int) T/10;
  if(!apsPatData.SeqList) { // New the Seq list.
    apsPatData.NbSeq = 1*NbLoopAllSeqs; // !!!!!
    apsPatData.NbVal = NbIn+NbOut;
    NewPatData(apsPatData, T+Uncertainty); // Sets all to zero.
  }
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) { // Seq loop.
    // Reset Output.
    (((apsPatData.SeqList[seq])[apsPatData.NbPat[seq]-1])[2])=0;
    // Set NbPat.
    apsPatData.NbPat[seq] =(unsigned int)(lrand48() % Uncertainty)+T;
    for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++) { // Pat loop.
      // Ramdom input on first channel.
      ((apsPatData.SeqList[seq])[pat])[0]=(drand48()*2-1);
      // Reset second channel.
      ((apsPatData.SeqList[seq])[pat])[1]=0;
    }
    // Choose t1, t2;
    t1 = (unsigned int)(lrand48() % 10); t2=t1;
    while(t2==t1) t2 = (unsigned int)(lrand48() % t2Max)+1;    
    // Set the markers on second channel.
    ((apsPatData.SeqList[seq])[t1])[1]=1;
    ((apsPatData.SeqList[seq])[t2])[1]=1;
    // No Begin but End symbol.
    //(((apsPatData.SeqList[seq])[0])[1])=-1; 
    (((apsPatData.SeqList[seq])[apsPatData.NbPat[seq]-1])[1])=-1; 
    // Set output.
    (((apsPatData.SeqList[seq])[apsPatData.NbPat[seq]-1])[2])=
      ( ((apsPatData.SeqList[seq])[t1])[0]+
      ((apsPatData.SeqList[seq])[t2])[0] )*0.4 + 0.5;
  }
}

void TPatternGenerator::Laufband(struct TPatData &apsPatData) {
  bool DataExisted;
  unsigned int StartClasss;
  unsigned int MinSeqLenght = 20;
  unsigned int Uncertainty =10;
  unsigned int NbIn = 8;
  unsigned int NbSeq = NbIn; // ==NbOut== number of classes.
  if(apsPatData.SeqList) DataExisted = true;
  else {
    DataExisted = false;
    apsPatData.NbSeq = NbSeq*NbLoopAllSeqs;
    apsPatData.NbVal = NbIn+NbSeq; // NbIn inputs.
    // All newed with max length, so we don't have to renew them.
    NewPatData(apsPatData, MinSeqLenght+Uncertainty);
    // Set all to zero.
    for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++)
      for(unsigned int pat=0;pat<MinSeqLenght+Uncertainty;pat++)
	for(unsigned int val=0;val<apsPatData.NbVal;val++)
	  (((apsPatData.SeqList[seq])[pat])[val])=0;
  }
  // Always rewrite everything except for the zero targets E, B.
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) {  // Sequence loop.
    // Random Start class.
    StartClasss = (unsigned int)(lrand48() % NbSeq);
    // Set changing values to zero (before seting new NbPat).
    for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++)  // Reset input.
      for(unsigned int val=0;val<NbIn;val++)
	(((apsPatData.SeqList[seq])[pat])[val])=0;
    for(unsigned int val=NbIn;val<apsPatData.NbVal;val++)//Reset Target class.
      (((apsPatData.SeqList[seq])[apsPatData.NbPat[seq]-1])[val])=0;
    // Set NbPat.
    apsPatData.NbPat[seq] = 
      (unsigned int)(lrand48() % (Uncertainty+1))+MinSeqLenght;
    // Set input.
    for(unsigned int pat=0;pat<apsPatData.NbPat[seq]-1;pat++)
      ((apsPatData.SeqList[seq])[pat])
	[(unsigned int)((StartClasss+pat) % NbSeq)]=1;
    // Set output.
    (((apsPatData.SeqList[seq])[apsPatData.NbPat[seq]-1])
     [NbIn+StartClasss])=1;
  } // End Sequence loop.
}

void TPatternGenerator::Sinus(struct TPatData &apsPatData) {
  // sin(NbSeq*2*M_PI/SeqLenght*pat+Offset) -> N-histogarmm [0,1] ; x=[0,1]
  bool DataExisted;
  double Offset = 0; // Shift the function.
  unsigned int MinSeqLenght = 100;
  unsigned int Uncertainty = 0;
  unsigned int NbIn = 3; // 3 directions to draw. north=0/(west)/east/south
  unsigned int NbSeq = 4; // ==NbOut== number of classes ==frequency.
  unsigned int NbY = 10;
  unsigned int frequency; double y; unsigned int iy; unsigned int iylast; 
  unsigned int direction = 0;
  if(apsPatData.SeqList) DataExisted = true;
  else {
    DataExisted = false;
    apsPatData.NbSeq = NbSeq*NbLoopAllSeqs;
    apsPatData.NbVal = NbIn+NbSeq; // NbIn inputs.
    // All newed with max length, so we don't have to renew them.
    NewPatData(apsPatData, MinSeqLenght+Uncertainty);
    // Set all to zero.
    for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++)
      for(unsigned int pat=0;pat<MinSeqLenght+Uncertainty;pat++)
	for(unsigned int val=0;val<apsPatData.NbVal;val++)
	  (((apsPatData.SeqList[seq])[pat])[val])=0;
  }
  // Always rewrite everything except for the zero targets E, B.
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) {  // Sequence loop.
    // Set changing values to zero (before seting new NbPat).
    for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++)  // Reset input.
      for(unsigned int val=0;val<NbIn;val++)
	(((apsPatData.SeqList[seq])[pat])[val])=0;
    for(unsigned int val=NbIn;val<apsPatData.NbVal;val++)//Reset Target class.
      (((apsPatData.SeqList[seq])[apsPatData.NbPat[seq]-1])[val])=0;
    // Set NbPat.
    if(Uncertainty>0) 
      apsPatData.NbPat[seq]=(unsigned int)(lrand48() % (Uncertainty+1))+MinSeqLenght;
    //cout<<apsPatData.NbPat[seq]<<endl;
    // Set output, i.e. set class.
    frequency = (unsigned int)(seq % NbSeq);
    (((apsPatData.SeqList[seq])[apsPatData.NbPat[seq]-1])
     [NbIn+frequency])=1;
    //// Set input.
    Offset = drand48()*2*M_PI; // Offset [0,2Pi].
    y=sin(0+Offset); y+=1; y*=NbY; iy=(unsigned int)floor(y); iylast=iy;
    for(unsigned int pat=0;pat<apsPatData.NbPat[seq];pat++) {
      y=sin((frequency+1)*2*M_PI*(pat+1)/apsPatData.NbPat[seq]+Offset);
      y+=1; y*=NbY; iy=(unsigned int)floor(y);
      if(iylast<iy) direction=0;
      else if(iylast>iy) direction=2;
      else if(iylast==iy) direction=1;
      iylast=iy;
      ((apsPatData.SeqList[seq])[pat])[direction]=1;
    }
  } // End Sequence loop.
}

void TPatternGenerator::SpaceToTime(struct TPatData &apsPatData) {
  unsigned int NbIn = 2+1; // One noise channel.
  unsigned int NbBits=8; unsigned int MaxVal=1<<NbBits; 
  unsigned int A_B;
  unsigned int NbOut =NbBits;
  unsigned int NbSeq = 256;
  if(!apsPatData.SeqList) {
    apsPatData.NbSeq = NbSeq*NbLoopAllSeqs;
    apsPatData.NbVal = NbIn+NbOut; 
    NewPatData(apsPatData,NbBits);
  }
  for(unsigned int seq=0;seq<MaxVal;seq++) { //Seq
     for(unsigned int pat=0,mask=1;pat<NbBits;pat++,mask+=1<<pat) { //Pat
      ((apsPatData.SeqList[seq])[pat])[0] = (bool)!(seq&(1<<pat)); // In
      ((apsPatData.SeqList[seq])[pat])[1] = (bool)(seq&(1<<pat)); // In
      if(drand48()<0.5)((apsPatData.SeqList[seq])[pat])[2] = 0;
      else ((apsPatData.SeqList[seq])[pat])[2] = 1; // Noise.
      for(unsigned int ib=0;ib<NbBits;ib++) { // Out
	A_B=(seq&(1<<ib));
	if(ib<=pat)
	  ((apsPatData.SeqList[seq])[pat])[NbIn+ib] = (bool)(A_B&mask);
	  else  
	    ((apsPatData.SeqList[seq])[pat])[NbIn+ib] = 0.5;
      }
    }
  }
}

void TPatternGenerator::Protein2D(struct TPatData &apsPatData) {
  //SampleSeq=protein , SamplePat=AA.
  // NbIn and NbOut form the original data file (in gets doubled, +clock).
  //const unsigned int NbIn=20, NbOut=3; // Protein2D task:20In , 3Out.
  const unsigned int NbIn=1 , NbOut=2; // debug task: 2In , 2Out.
  unsigned int NbClockIn=2; // Give pat, SQR(pat)
  // Sample always one sequence when called.
  if(!apsPatData.SeqList) {
    // Load train and test data from Pattern Files and put it away.
    if(!TrainDataOrg.SeqList) {
      if(LoadTrainPattern())exit(1); 
      CopyPatData(TrainDataOrg, TrainData); TrainData.SeqList=NULL; }
    if(!TestDataOrg.SeqList) {
      if(LoadTestPattern())exit(1); 
      CopyPatData(TestDataOrg, TestData); TestData.SeqList=NULL; }
    // Src date does not have to be deleted, because Org keeps pointers.
    // Detenmine longest sequence length.
    if(!MaxNbPat) {
      for(unsigned int seq=0;seq<TrainData.NbSeq;seq++)
	if(MaxNbPat<TrainData.NbPat[seq]) MaxNbPat=TrainData.NbPat[seq];
      for(unsigned int seq=0;seq<TestData.NbSeq;seq++)
	if(MaxNbPat<TestData.NbPat[seq]) MaxNbPat=TestData.NbPat[seq];
    }
    // Init data with one seq and MaxNbPat, all other seq are pointing down.
    TrainData.NbSeq = NbLoopAllSeqs; // Sample NbSeq times from train set.
    TestData.NbSeq = TestDataOrg.NbPatTotal;//Go throgh all AA in test test.
    //cout<<"TestData.NbSeq:"<<TestData.NbSeq<<endl;
    //cout<<"TestData.NbSeq"<<TestData.NbSeq<<endl;/ddd
    apsPatData.NbVal = NbIn+NbIn+NbClockIn+NbOut;
    // NewPatData by hand.
    apsPatData.NbPat = new unsigned int[apsPatData.NbSeq];
    for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) 
      apsPatData.NbPat[seq] = MaxNbPat; // Waste some space.
    apsPatData.SeqList = new double**[apsPatData.NbSeq];
    // Create only one seq with MaxNbPat.
    apsPatData.SeqList[0] = new double*[MaxNbPat];
    for(unsigned int pat=0;pat<MaxNbPat;pat++)
      (apsPatData.SeqList[0])[pat] = new double[apsPatData.NbVal];
    // All the other Seqs point to the first Seq.
    for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++)
      apsPatData.SeqList[seq] = apsPatData.SeqList[0];
    // NbClass not inited.
    apsPatData.NbClass=NULL;
    // Set to zero.
    for(unsigned int pat=0;pat<MaxNbPat;pat++)
      for(unsigned int val=0;val<apsPatData.NbVal;val++) 
	((apsPatData.SeqList[0])[pat])[val]=0;
    RefData.SampleSeq=0; RefData.SamplePat=0;
  }
  // Choooe a sample, for train random, for test one after the other.
  if(RefData.RandomSample) {
    RefData.SampleSeq=(unsigned int)(lrand48() % (*RefData.PatData).NbSeq);
    RefData.SamplePat=(unsigned int)
      (lrand48() % (*RefData.PatData).NbPat[RefData.SampleSeq]);
  } else {
    if(RefData.Restart){ 
      RefData.Restart=false; RefData.SampleSeq=0; RefData.SamplePat=0; 
    } else {
      RefData.SamplePat++;
      if(RefData.SamplePat>=(*RefData.PatData).NbPat[RefData.SampleSeq]) { 
	RefData.SamplePat=0; RefData.SampleSeq++;
	if(RefData.SampleSeq>=(*RefData.PatData).NbSeq) { 
	  //cout<<"Protein2D no more data."<<endl;//ddd
	  RefData.SampleSeq=0; } 
      }
    }
  }
  // Set sequence. First the AA in the center, then next neighbors (2).
  // Output always target of center.
  // Set NbPat.
  apsPatData.NbPat[0]=
    (*RefData.PatData).NbPat[RefData.SampleSeq]-
    RefData.SamplePat;//Closer to Seq start.
  if(apsPatData.NbPat[0]<=RefData.SamplePat) 
    apsPatData.NbPat[0]=RefData.SamplePat+1;//Closer to Seq end.
//   cout<<"RefData.SampleSeq:"<<RefData.SampleSeq
//       <<" RefData.SamplePat:"<<RefData.SamplePat
//       <<" apsPatData.NbPat[0]:"<<apsPatData.NbPat[0]<<endl;//ddd
  unsigned int PatPosDown=RefData.SamplePat, PatPosUp=RefData.SamplePat;
  bool PatPosDownCont=true, PatPosUpCont=true;
  for(unsigned int pat=0;pat<apsPatData.NbPat[0];pat++) { //Pat loop.
    for(unsigned int val=0;val<NbIn;val++) {// Set input.
      if(PatPosDownCont)// Left neighbor.
	((apsPatData.SeqList[0])[pat])[val]=
	  (((*RefData.PatData).SeqList[RefData.SampleSeq])[PatPosDown])[val];
      else ((apsPatData.SeqList[0])[pat])[val]=0;
      if(PatPosUpCont)// Right neightbor.
	((apsPatData.SeqList[0])[pat])[NbIn+val]=
	  (((*RefData.PatData).SeqList[RefData.SampleSeq])[PatPosUp])[val];
      else ((apsPatData.SeqList[0])[pat])[NbIn+val]=0;
      }
    if(PatPosDownCont)if(PatPosDown)PatPosDown--;else PatPosDownCont=false;
    if(PatPosUpCont)
      if(PatPosUp<(*RefData.PatData).NbPat[RefData.SampleSeq]-1)PatPosUp++;
      else PatPosUpCont=false;
    // Set ClockIn.
    ((apsPatData.SeqList[0])[pat])[2*NbIn+0]=pat;
    ((apsPatData.SeqList[0])[pat])[2*NbIn+1]=pat*pat;
    for(unsigned int val=0;val<NbOut;val++) // Set output.
      ((apsPatData.SeqList[0])[pat])[2*NbIn+NbClockIn+val]=
	(((*RefData.PatData).SeqList[RefData.SampleSeq])
	 [RefData.SamplePat])[NbIn+val];
  }
}

void TPatternGenerator::TimerClass(struct TPatData &apsPatData) {
  // Two classes on one output.
  const unsigned int NbOut=1, NbIn=1, T=20, DT=1, NbSeq=2;
  unsigned int dt;
  if(!apsPatData.SeqList) {
    MaxNbPat = T+DT;
    apsPatData.NbSeq = NbSeq*NbLoopAllSeqs;
    apsPatData.NbVal = NbIn+NbOut;
    NewPatData(apsPatData,MaxNbPat);
    // Set constant In values at seq start.
    for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++)
      ((apsPatData.SeqList[seq])[0])[0]=1;
  }
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) { // Sequence loop.
    for(unsigned int pat=T-1;pat<T+DT-1;pat++) // Reset.
      for(unsigned int val=0;val<apsPatData.NbVal;val++)   
	((apsPatData.SeqList[seq])[pat])[val]=0;
    dt = (unsigned int)(lrand48() % (DT+1));
    apsPatData.NbPat[seq] = dt*DT+T;
    ((apsPatData.SeqList[seq])[apsPatData.NbPat[seq]-1])[0]=1;// Input.
    ((apsPatData.SeqList[seq])[apsPatData.NbPat[seq]-1])[1]=dt;// Output.
  }
}

void TPatternGenerator::TimerClassMulti(struct TPatData &apsPatData) {
  // NbSeq classes and outputs, DT is the time delay between outputs.
  // One linear output unit for DT. 
  // (T=10,NbSeq=5,DT=1: 10,11,12,13,14) ; (T=10,NbSeq=4,DT=2: 10,14)
  const unsigned int NbIn=1, T=10, DT=1, NbSeq=12, NbOut=1;//NbSeq;
  unsigned int dt;
  if(!apsPatData.SeqList) {
    MaxNbPat = T+DT*(NbSeq-1)+1;
    //    apsPatData.NbSeq = NbSeq*NbLoopAllSeqs;
    if(NbLoopAllSeqs>1) apsPatData.NbSeq = NbLoopAllSeqs;//100; // Train.
    else apsPatData.NbSeq = NbSeq*NbLoopAllSeqs; // Test.
    apsPatData.NbVal = NbIn+NbOut;
    NewPatData(apsPatData,MaxNbPat);
    // Set constant In values at seq start.
    //for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++)
    //  ((apsPatData.SeqList[seq])[0])[0]=1;
  }
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) { // Sequence loop.
    for(unsigned int pat=T-1;pat<MaxNbPat-1;pat++) // Reset.
      for(unsigned int val=0;val<apsPatData.NbVal;val++)   
	((apsPatData.SeqList[seq])[pat])[val]=0;
    if(apsPatData.NbSeq==NbSeq) dt=seq;//(NbLoopAllSeqs-1)All seqes once.
    else dt = (unsigned int)(lrand48() % NbSeq);//Random Class.
    apsPatData.NbPat[seq] = dt*DT+T;
    ((apsPatData.SeqList[seq])[apsPatData.NbPat[seq]-1])[0]=1;// Input.
    //((apsPatData.SeqList[seq])[apsPatData.NbPat[seq]-1])[NbIn+dt]
    //  =1;//Output.
    ((apsPatData.SeqList[seq])[apsPatData.NbPat[seq]-1])[NbIn]=dt;//Output
  }
}

void TPatternGenerator::AnBn(struct TPatData &apsPatData) {
  // a_n b_n language. Set step taget: 0.5,...,1,1,..,1,0 ; 2n Pats
  // Input symbol, output predict prop(a=0,b=1). 
  // General(ize) during test.
  // Test_Generalize parameter should be set to 0 or 1 in LSTM.par.
  // local: input:a,b,S ; output:a,b,T ; set/notset=+1/-1;
  // Second input: (s)tart symbol; second output: (t)erminal symbol.
  const unsigned int NbIn=3, NbOut=3, N=20, NMax=21,dN=NMax-N,NG=1000;
  // dN is included into the set!!!!
  unsigned int n,dN1sqr; dN1sqr=(dN+1)*(dN+1);
  if(!apsPatData.SeqList) {
    if(apsPatData.Generalize) { 
      MaxNbPat = 2*NG+2+1; // +1 for S.
      apsPatData.NbSeq = NG; //Test.
    } else {
      //if(dN==1) {apsPatData.NbSeq = 2; // Time lag train set.
      //}else 
	{apsPatData.NbSeq = 1000; }//Train //!!! NbSeq*NbLoopAllSeqs;
      MaxNbPat = 2*NMax+1+1; MaxNbPatTrain=MaxNbPat; } // +1 for S.
    apsPatData.NbVal = NbIn+NbOut;
    NewPatData(apsPatData,MaxNbPat);
    // Reset. Set the begin symbol and target for all sequences only once.
    for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) {
      // Reset.
      for(unsigned int pat=0;pat<MaxNbPat;pat++)
	for(unsigned int val=0;val<apsPatData.NbVal;val++)
	  ((apsPatData.SeqList[seq])[pat])[val]=-1;
      ((apsPatData.SeqList[seq])[0])[2]=1; // S input.
      ((apsPatData.SeqList[seq])[0])[NbIn+0]=1; // a output.
      ((apsPatData.SeqList[seq])[0])[NbIn+2]=1; // T output.
    }
  } else if(apsPatData.Generalize) return; // Generate test set only once.
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) { // Sequence loop.
    if(apsPatData.Generalize) { n=1+seq;//Start N=1 uring a Test Run.
    } else {//During Training.
      if(seq<=dN) { n=N+seq; //Include every Seq at least once.
      //cout<<"seq:"<<seq<<" n="<<n<<endl;//ddd
      } else { 
	//n=(unsigned int)(lrand48() % (dN+1));// Random unifrom.
	//n=(unsigned int)sqrt((lrand48() % dN1sqr));// Random sqr.
	// Random valley, more/less short and long strings.:
	////double d=(lrand48() % (dN+1));// Random unifrom.
	n=(unsigned int)(lrand48() % (dN+1));
        //n=(unsigned int)
	//((double)dN/(1+exp(-(10.0/dN)*(n-((double)dN/2)))));//more.
	//n=(unsigned int)((4/(dN*dN))*d*d*d-(6/dN)*d*d+3*d);//less. 
	n+=N; }}
    apsPatData.NbPat[seq]=2*n+1;//Set NbPat. // +1 for S.
    //if(!apsPatData.Generalize) cout<<""<<n<<"\n";;//ddd
    for(unsigned int pat=1;pat<n+1;pat++) { // Set n times a.
      ((apsPatData.SeqList[seq])[pat])[0]=1; // Input a.
      ((apsPatData.SeqList[seq])[pat])[NbIn+0]=1; // a output.
      ((apsPatData.SeqList[seq])[pat])[NbIn+1]=1; // b output.
    }
    for(unsigned int pat=n+1;pat<2*n+1;pat++) { // Set n times b.
      ((apsPatData.SeqList[seq])[pat])[1]=1; // Input b.
      ((apsPatData.SeqList[seq])[pat])[NbIn+1]=1; // b output.
    }
      //For last b: output T.
      ((apsPatData.SeqList[seq])[2*n])[NbIn+1]=-1;
      ((apsPatData.SeqList[seq])[2*n])[NbIn+2]=1; // T output.
  }
  //if(!apsPatData.Generalize) exit(0); //else cout<<"test set\n";//ddd
}

void TPatternGenerator::AnBnCn(struct TPatData &apsPatData) {
  // a_n b_n c_n language. 
  // Set step taget: one-hot encode: in: a,b,c,S; out:a,b,c,T   ; 3n Pats
  // Input symbol, output predict prop. //General(ize) during test.
  // Test_Generalize parameter should be set to 0 or 1 in LSTM.par.
  // pattern:[N,...,NMax] ; dN is included into the set!!!!
  const unsigned int NbIn=4, NbOut=4, N=20, NMax=21, dN=NMax-N, NG=50;
  unsigned int n=0,dN1sqr; dN1sqr=(dN+1)*(dN+1);
  if(!apsPatData.SeqList) {
    if(apsPatData.Generalize) { 
      MaxNbPat = 3*NG+3+1; apsPatData.NbSeq = NG; //Test.//+1 for S.
    } else {
      //if(dN==1) {apsPatData.NbSeq = 2; // Time lag train set.
      //}else 
      {apsPatData.NbSeq = NbLoopAllSeqs; }//1000; }//Train //!!! 
      MaxNbPat = 3*NMax+1; MaxNbPatTrain=MaxNbPat; }//+1 for S.
    apsPatData.NbVal = NbIn+NbOut;
    NewPatData(apsPatData,MaxNbPat);
    // Set the begin symbol and target for all sequences only once.
    for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) {
      for(unsigned int val=0;val<apsPatData.NbVal;val++)
	((apsPatData.SeqList[seq])[0])[val]=-1;
      ((apsPatData.SeqList[seq])[0])[3]=1; // S input.
      ((apsPatData.SeqList[seq])[0])[NbIn+0]=1; // a output.
      ((apsPatData.SeqList[seq])[0])[NbIn+3]=1; // T output.
    }
  } else if(apsPatData.Generalize)
    return; //Generate test set only once.
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++){//Sequence loop.
    if(apsPatData.Generalize) { n=1+seq;//Start N=1 during a Test Run.
    } else {//During Training.
      if(seq<=dN) { n=N+seq; //Include every Seq at least once.
      //cout<<"seq:"<<seq<<" n="<<n<<endl;//ddd
      } else { // N random.
	//n=(unsigned int)(lrand48() % (dN+1));// Random uniform.
	//n=(unsigned int)sqrt((lrand48() % dN1sqr));// Random sqr.
	// Random valley, more/less short and long strings.:
	////double d=(lrand48() % (dN+1));// Random uniform.
	n=(unsigned int)(lrand48() % (dN+1)); //default!!!
        //n=(unsigned int)
	//((double)dN/(1+exp(-(10.0/dN)*(n-((double)dN/2)))));//more.
	//n=(unsigned int)((4/(dN*dN))*d*d*d-(6/dN)*d*d+3*d);//less. 
	n+=N; } } // n>=1.
    apsPatData.NbPat[seq]=3*n+1;//Set NbPat.//+1 for S.
    //if(!apsPatData.Generalize) cout<<""<<n<<"\n";;//ddd
    for(unsigned int pat=0;pat<3*n;pat++) { // Reset input.
      for(unsigned int val=0;val<NbIn;val++) {
	((apsPatData.SeqList[seq])[pat+1])[val]=-1; } }
    // Set new seq.
    for(unsigned int pat=0;pat<n;pat++) { // Set n times a.
      ((apsPatData.SeqList[seq])[pat+1])[0]=1;//Input a.
      ((apsPatData.SeqList[seq])[pat+1])[NbIn+0]=1; // Output a/b.
      ((apsPatData.SeqList[seq])[pat+1])[NbIn+1]=1; // Output a/b.
      ((apsPatData.SeqList[seq])[pat+1])[NbIn+2]=-1; // Output a/b.
      ((apsPatData.SeqList[seq])[pat+1])[NbIn+3]=-1; }// delete T.
      //      ((apsPatData.SeqList[seq])[pat+1])[NbIn+3]=1; }// Output a/b.
      //for(unsigned int val=NbIn;val<NbIn+NbOut;val++) {
      //((apsPatData.SeqList[seq])[pat+1])[val]=-777; }}// Output unknown.
    for(unsigned int pat=n;pat<2*n;pat++) { // Set n times b.
      ((apsPatData.SeqList[seq])[pat+1])[1]=1; // Input b.
      ((apsPatData.SeqList[seq])[pat+1])[NbIn+0]=-1; // Output b.
      ((apsPatData.SeqList[seq])[pat+1])[NbIn+1]=1; // Output b.
      ((apsPatData.SeqList[seq])[pat+1])[NbIn+2]=-1; // Output b.
      ((apsPatData.SeqList[seq])[pat+1])[NbIn+3]=-1; }// delete T.
      //      ((apsPatData.SeqList[seq])[pat+1])[NbIn+3]=-1; }// Output b.
    for(unsigned int pat=2*n;pat<3*n;pat++) { // Set n times c.
      ((apsPatData.SeqList[seq])[pat+1])[2]=1; // Input c.
      ((apsPatData.SeqList[seq])[pat+1])[NbIn+0]=-1;  // Output c.
      ((apsPatData.SeqList[seq])[pat+1])[NbIn+1]=-1;  // Output c.
      ((apsPatData.SeqList[seq])[pat+1])[NbIn+2]=1; // Output c.
      ((apsPatData.SeqList[seq])[pat+1])[NbIn+3]=-1; }// delete T.
      //      ((apsPatData.SeqList[seq])[pat+1])[NbIn+3]=-1; }// Output c.
    // Set the output exceptions.
    ((apsPatData.SeqList[seq])[2*n-1+1])[NbIn+1]=-1; // delete b.
    ((apsPatData.SeqList[seq])[2*n-1+1])[NbIn+2]=1; // Set c.
    ((apsPatData.SeqList[seq])[3*n-1+1])[NbIn+2]=-1; // delete c.
    ((apsPatData.SeqList[seq])[3*n-1+1])[NbIn+3]=1; // Set T.
  }// End seq loop.
  //if(!apsPatData.Generalize)exit(0);//else cout<<"test set\n";//ddd
}

void TPatternGenerator::a_nb_mB_mA_n(struct TPatData &apsPatData) {
  // a_n b_m.. language. Set step taget: 0.5,...,1,1,..,1,0 ; 2n Pats
  // 5 Input symbols (one hot +-1): a,b,A,B,S
  // 5 Output symbols (one hot +-1): a,b,A,B,T
  // Test_Generalize parameter should be set to 0 or 1 in LSTM.par.
  // NMax > n,m. Test all seqs with : n,m<NG
  const unsigned int NbIn=5, NbOut=5;
  unsigned int N=1, NMax=12, dN=NMax-N, NG=50;
  // dN is included into the set!!!!
  unsigned int n,m, seq=0, s_n, s_m;
  unsigned int UniqueN=((dN+1)*(dN+1)); if(UniqueN>1000) exit(1);
  if(!apsPatData.SeqList) {
    if(apsPatData.Generalize) { 
      NMax= NG; MaxNbPat = 4*NG+1; //+1 for S.
      apsPatData.NbSeq = (NG*NG); //Test.
    } else {
      {apsPatData.NbSeq = 1000; }//Train //!!!!! NbSeq*NbLoopAllSeqs;
      MaxNbPat = 4*NMax+1; MaxNbPatTrain=MaxNbPat; }//+1 for S.
    apsPatData.NbVal = NbIn+NbOut;
    NewPatData(apsPatData,MaxNbPat);
    // Set the begin symbol and target for all sequences only once.
    for(unsigned int seq_ini=0;seq_ini<apsPatData.NbSeq;seq_ini++) {
      for(unsigned int val=0;val<apsPatData.NbVal;val++)
	((apsPatData.SeqList[seq_ini])[0])[val]=-1;
      ((apsPatData.SeqList[seq_ini])[0])[4]=1; // S input.
      ((apsPatData.SeqList[seq_ini])[0])[NbIn+0]=1; // a output.
      ((apsPatData.SeqList[seq_ini])[0])[NbIn+4]=1; // T output.
    }
  } else if(apsPatData.Generalize) 
    return; //Generate test set only once.
  // Sequence loop.
  for(unsigned int seq_n=0;seq<apsPatData.NbSeq;seq_n++)
    for(unsigned int seq_m=0;(seq_m<=seq_n) //1000 no quaratic number.
    	  &&(seq<apsPatData.NbSeq);seq_m++) 
      for(unsigned int sym=0;(sym<=1)&&(seq<apsPatData.NbSeq);sym++,seq++) {
	if(seq_m==seq_n) sym++; // One go.
	if(!sym) { s_n=seq_n; s_m=seq_m; } else { s_n=seq_m; s_m=seq_n; }
	if(apsPatData.Generalize) {//Test: All combinations for test.
	  n=1+s_n; m=1+s_m;
//  	  cout<<"Test: seq:"<<seq
//  	      <<" seq_n:"<<seq_n<<" seq_m:"<<seq_m<<" sym:"<<sym
//  	      <<" n:"<<n<<" m:"<<m<<endl;//ddd
	} else { // Train: almost Rrandom training.
	  if (seq<UniqueN) { //Include every Seq at least once.
	    n=N+(unsigned int)(s_n % (dN+1));
	    m=N+(unsigned int)(s_m % (dN+1));
	  } else {
	    //cout<<"rand ";//ddd
	    n=N+(unsigned int)(lrand48() % (dN+1));
	    m=N+(unsigned int)(lrand48() % (dN+1));
	  }
	  // Wiles set n+m<=12
	  //  	  if(sym) { m=N+(unsigned int)fmod((m-N),dN+2-n);
	  //  } else { n=N+(unsigned int)fmod((n-N),dN+2-m); }
	  // End of Wiles set n+m<=12
//  	  cout<<"Train: seq:"<<seq
//    	      <<" seq_n:"<<seq_n<<" seq_m:"<<seq_m<<" sym:"<<sym
//    	      <<" n:"<<n<<" m:"<<m<<endl;//ddd
  	}
	apsPatData.NbPat[seq]=2*n+2*m+1;//Set NbPat.
	// Set values.
	for(unsigned int pat=0;pat<2*n+2*m;pat++) { // Reset all.
	  for(unsigned int val=0;val<NbIn+NbOut;val++) {
	    ((apsPatData.SeqList[seq])[pat+1])[val]=-1; } }
	//for(unsigned int pat=0;pat<n+m;pat++) { // Reset unknown target..
	//for(unsigned int val=NbIn;val<NbIn+NbOut;val++) {
	//  ((apsPatData.SeqList[seq])[pat+1])[val]=-777; } }
	for(unsigned int pat=0;pat<n;pat++) { // Set n times a.
	  ((apsPatData.SeqList[seq])[pat+1])[0]=1;  // Input a.
	  ((apsPatData.SeqList[seq])[pat+1])[NbIn+0]=1; // Output a/b.
	  ((apsPatData.SeqList[seq])[pat+1])[NbIn+1]=1; }// Output a/b.
	for(unsigned int pat=n;pat<n+m;pat++) { // Set m times b.
	  ((apsPatData.SeqList[seq])[pat+1])[1]=1; // Input b.
	  ((apsPatData.SeqList[seq])[pat+1])[NbIn+1]=1; // Output b/B.
	  ((apsPatData.SeqList[seq])[pat+1])[NbIn+3]=1; }// Output b/B.
	for(unsigned int pat=n+m;pat<n+2*m;pat++) { // Set m times B.
	  ((apsPatData.SeqList[seq])[pat+1])[3]=1; // Input B.
	  ((apsPatData.SeqList[seq])[pat+1])[NbIn+3]=1; }// Output B.
	for(unsigned int pat=n+2*m;pat<2*n+2*m;pat++) { // Set m times A.
	  ((apsPatData.SeqList[seq])[pat+1])[2]=1; // Input A.
	  ((apsPatData.SeqList[seq])[pat+1])[NbIn+2]=1; }// Output A.
	// Set the output exceptions.
	((apsPatData.SeqList[seq])[n+2*m-1+1])[NbIn+1]=-1; // delete b.
	((apsPatData.SeqList[seq])[n+2*m-1+1])[NbIn+3]=-1; // delete B.
	((apsPatData.SeqList[seq])[n+2*m-1+1])[NbIn+2]=1; // Set A.
	((apsPatData.SeqList[seq])[2*n+2*m-1+1])[NbIn+2]=-1; // delete A.
	((apsPatData.SeqList[seq])[2*n+2*m-1+1])[NbIn+4]=1; // Set T.
      }
}

//void TPatternGenerator::Osci_Class(struct TPatData &apsPatData) {
  /*
  // NbSeq classes of oszillating in seq, input=output.
  const unsigned int NbOut=1, NbIn=1; // Oszi input = output.
  const unsigned int NbSeq=4; // Sin, Tri, Squ, Pulse.
  const double Offset=0;// 0<Offset<MaxNbPat
  const unsigned int frequency=1;
  const unsigned int Cycles=8, StepsPerCyl=25;//discrete steps.
  MaxNbPat=Cycles*StepsPerCyl;
  //double  MP2=(double)MaxNbPat*0.5;
  if(!apsPatData.SeqList) {
    apsPatData.NbSeq = NbSeq*NbLoopAllSeqs;
    apsPatData.NbVal = NbIn+NbOut;
    NewPatData(apsPatData,MaxNbPat);
  }
  for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) { // Sequence loop.
    for(unsigned int pat=T-1;pat<MaxNbPat-1;pat++) // Reset.
      for(unsigned int val=0;val<apsPatData.NbVal;val++)   
	((apsPatData.SeqList[seq])[pat])[val]=0;
    dt = (unsigned int)(lrand48() % NbSeq);
    apsPatData.NbPat[seq] = dt*DT+T;
    ((apsPatData.SeqList[seq])[apsPatData.NbPat[seq]-1])[0]=1;// Input.
    ((apsPatData.SeqList[seq])[apsPatData.NbPat[seq]-1])[NbIn+dt]
      =1;//Output.
  }
  */
//}

void TPatternGenerator::Online_TempOrderEx6(struct TPatData &apsPatData) {
  if(!apsPatData.SeqList) {
    //bool ForONLINE = true;
    // Only for _3times.
    unsigned int NbVal = 16;
    MaxNbPat = 110;
    apsPatData.NbSeq = 1;
    apsPatData.NbVal = NbVal;
    apsPatData.NbPatTotal = 1; // Not to divide by zero somewhere.
    // NewPatData by hand.
    // Create and update only one entry in NbPat (and NbClass for logging).
    apsPatData.NbPat   = new unsigned int[apsPatData.NbSeq];
    apsPatData.NbPat[0] = MaxNbPat; // Will be changed dynamicly.
    apsPatData.NbClass = new unsigned int[apsPatData.NbSeq];
    apsPatData.NbClass[0] = 0; // Will be changed dynamicly.
    apsPatData.SeqList = new double**[apsPatData.NbSeq];
    // Create only one seq with MaxNbPat with NbVal.
    apsPatData.SeqList[0] = new double*[MaxNbPat];
    // Create Values for the pattern in the first seq.
    for(unsigned int pat=0;pat<MaxNbPat;pat++) {
      (apsPatData.SeqList[0])[pat] = new double[apsPatData.NbVal];
      // Set all to zero.
      for(unsigned int val=0;val<apsPatData.NbVal;val++)
	(((apsPatData.SeqList[0])[pat])[val])=0;
    }
  }
  TempOrderEx6(apsPatData); // Call the non-online function.
}

void TPatternGenerator::Online_ReberGrammar(struct TPatData &apsPatData,
					    bool SetBackToSeqStart) {
#define NO_B_E_SYMBOLS
#define EMBEDDED_REBER
  unsigned int NbIn = 7;
  unsigned int NbOut = 7;
  unsigned int NbVal = NbIn+NbOut;
  //unsigned int NbSeq = different for Test and Tain data, see below.
  MaxNbPat = MaxOnlineStreamLengthTrain;
  if(MaxOnlineStreamLengthTest>MaxNbPat)
    MaxNbPat = MaxOnlineStreamLengthTest;
  char &RGS = ReberGrammarState;
#ifdef EMBEDDED_REBER
  char &ERGS = EmbeddedReberGrammarState;
#endif
  // p1 is global, becaue it is the one taken in the last step, so now input.
  enum ReberGrammarSymbol &p1=ReberGrammarSymbol_p1 ,p2;// Shorter local name.
  p2=B; // Init to avoid warnings.
  PatCount++;// To do statistics (a service).
  if(!apsPatData.SeqList) {
    TrainData.NbSeq = 1;
    TestData.NbSeq = 10;
    apsPatData.NbVal = NbVal;
    apsPatData.NbPatTotal=1; // Not to divide by zero somewhere.
    // NewPatData by hand.
    apsPatData.NbPat = new unsigned int[apsPatData.NbSeq];
    for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) 
      apsPatData.NbPat[seq] = MaxNbPat;
    apsPatData.SeqList = new double**[apsPatData.NbSeq];
    // Create only one seq with MaxNbPat and  only one Pat with NbVal.
    apsPatData.SeqList[0] = new double*[MaxNbPat];
    (apsPatData.SeqList[0])[0] = new double[apsPatData.NbVal];
    // All the other Pats point to the first Pat.
    for(unsigned int pat=0;pat<MaxNbPat;pat++)
      (apsPatData.SeqList[0])[pat] = (apsPatData.SeqList[0])[0];
    // All the other Seqs point to the first Seq.
    for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++)
      apsPatData.SeqList[seq] = apsPatData.SeqList[0];
    // sTestDate points to the TrainData.
    // CopyPatData(TestData,TrainData);
  }
  // Set Pat to zero.
  for(unsigned int val=0;val<NbVal;val++) ((apsPatData.SeqList[0])[0])[val]=0;
#ifndef NO_B_E_SYMBOLS
  if(SetBackToSeqStart) {
#ifdef EMBEDDED_REBER
    ReberGrammarState=-1;
    EmbeddedReberGrammarState=0; // undefined.
#else
    ReberGrammarState=1; //  We utter the B with the first pat.
#endif
    p1=B; // The first input
  }
  // We number the grammar states beginnig with zero 
  // before B in math pos direction.
  // The utterances :  B T P S X V E
  ((apsPatData.SeqList[0])[0])[p1]=1; // In.
  switch(ReberGrammarState){ // Also called RGS.
#ifdef EMBEDDED_REBER
  case -2: {p1=B;p2=B; RGS=-1; break;}
  case -1: 
    {if(drand48()<0.5){p1=T;p2=P; RGS=0; ERGS=+1;}
    else{p1=P;p2=T; RGS=0; ERGS=-1;}break;}
#endif
  case 0: {p1=B;p2=B; RGS=1; break;}
  case 1: {if(drand48()<0.5){p1=T;p2=P; RGS=2;}else{p1=P;p2=T; RGS=6;}break;}
  case 2: {if(drand48()<0.5){p1=S;p2=X; RGS=2;}else{p1=X;p2=S; RGS=3;}break;}
  case 3: {if(drand48()<0.5){p1=S;p2=X; RGS=4;}else{p1=X;p2=S; RGS=6;}break;}
#ifdef EMBEDDED_REBER
  case 4: {p1=E;p2=E; RGS=7; break;}
#else
  case 4: {p1=E;p2=E; RGS=0; break;}
#endif
  case 5: {if(drand48()<0.5){p1=P;p2=V; RGS=3;}else{p1=V;p2=P; RGS=4;}break;}
  case 6: {if(drand48()<0.5){p1=T;p2=V; RGS=6;}else{p1=V;p2=T; RGS=5;}break;}
#ifdef EMBEDDED_REBER
  case 7: 
    {if(ERGS==+1){p1=T;p2=T; RGS=8; ERGS=0;}
    else{p1=P;p2=P; RGS=8; ERGS=0;}break;}
  case 8: {p1=E;p2=E; RGS=-2; break;}
#endif
  }
#else
  // Without E and B only for embedded. The utterances :  T P S X V
  if(SetBackToSeqStart) { // No input.
    ReberGrammarState=-1;
    EmbeddedReberGrammarState=0; // undefined.
  } else ((apsPatData.SeqList[0])[0])[p1]=1; // In.
  switch(ReberGrammarState){ // Also called RGS.
    //case -2: {p1=B;p2=B; RGS=-1; break;}
  case -1: 
    {if(drand48()<0.5){p1=T;p2=P; RGS=1; ERGS=+1;}
    else{p1=P;p2=T; RGS=1; ERGS=-1;}break;}
  //case 0: {p1=B;p2=B; RGS=1; break;}
  case 1: {if(drand48()<0.5){p1=T;p2=P; RGS=2;}else{p1=P;p2=T; RGS=6;}break;}
  case 2: {if(drand48()<0.5){p1=S;p2=X; RGS=2;}else{p1=X;p2=S; RGS=3;}break;}
  case 3: {if(drand48()<0.5){p1=S;p2=X; RGS=7;}else{p1=X;p2=S; RGS=6;}break;}
    //case 4: {p1=E;p2=E; RGS=7; break;}
  case 5: {if(drand48()<0.5){p1=P;p2=V; RGS=3;}else{p1=V;p2=P; RGS=7;}break;}
  case 6: {if(drand48()<0.5){p1=T;p2=V; RGS=6;}else{p1=V;p2=T; RGS=5;}break;}
  case 7: 
    {if(ERGS==+1){p1=T;p2=T; RGS=-1; ERGS=0;}
    else{p1=P;p2=P; RGS=-1; ERGS=0;}break;}
  //case 8: {p1=E;p2=E; RGS=-2; break;}
  }  
#endif
  ((apsPatData.SeqList[0])[0])[NbIn+p1]=1; // Out.
  ((apsPatData.SeqList[0])[0])[NbIn+p2]=1; // Out.
}


void TPatternGenerator::Online_SlidingNBitMemory(struct TPatData &apsPatData,
						 bool SetBackToSeqStart) {
  unsigned int NbIn = 2;
  unsigned int NbOut = 1;
  unsigned int NbVal = NbIn+NbOut;
  //unsigned int NbSeq = differetn for Test and Tain data, see below.
  MaxNbPat = MaxOnlineStreamLengthTrain;
  if(MaxOnlineStreamLengthTest>MaxNbPat)
    MaxNbPat = MaxOnlineStreamLengthTest;
  double (&SM)[SlidingNBitMemorySize] = SlidingNBitMemory; // Shorthand.
  char &SMP = SlidingNBitMemoryPointer; // Shorthand.
  char NewBit;
  PatCount++;// To do statistics (a service).
  if(!apsPatData.SeqList) {
    TrainData.NbSeq = 1;
    TestData.NbSeq = 10;
    apsPatData.NbVal = NbVal;
    apsPatData.NbPatTotal=1; // Not to divide by zero somewhere.
    // NewPatData by hand.
    apsPatData.NbPat = new unsigned int[apsPatData.NbSeq];
    for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++) 
      apsPatData.NbPat[seq] = MaxNbPat;
    apsPatData.SeqList = new double**[apsPatData.NbSeq];
    // Create only one seq with MaxNbPat but only one Pat wiht NbVal.
    apsPatData.SeqList[0] = new double*[MaxNbPat];
    (apsPatData.SeqList[0])[0] = new double[apsPatData.NbVal];
    // All the other Pats point to the first Pat.
    for(unsigned int pat=0;pat<MaxNbPat;pat++)
      (apsPatData.SeqList[0])[pat] = (apsPatData.SeqList[0])[0];
    // All the other Seqs point to the first Seq.
    for(unsigned int seq=0;seq<apsPatData.NbSeq;seq++)
      apsPatData.SeqList[seq] = apsPatData.SeqList[0];
    // sTestDate points to the TrainData.
    //CopyPatData(TestData,TrainData);
  }
  // Set Pat to zero.
  for(unsigned int val=0;val<NbVal;val++) ((apsPatData.SeqList[0])[0])[val]=0;
  if(SetBackToSeqStart) {
    SMP=0; for(unsigned int i=0;i<SlidingNBitMemorySize;i++) 
      SM[i]=0.5;// Undefined.
  }
  // In.
  if(drand48()<0.5) NewBit=0; else NewBit=1;
  ((apsPatData.SeqList[0])[0])[(int)NewBit]=1;
  // Out.
  ((apsPatData.SeqList[0])[0])[(int)NbIn]=SM[(int)SMP];
  // Actualize memory.
  SM[(int)SMP]=NewBit; SMP=(char)(++SMP % SlidingNBitMemorySize);
}

void TPatternGenerator::Online_AddMulti
(struct TPatData &apsPatData, bool SetBackToSeqStart) 
{
  // One noisy channel:0, 3 markers:
  // (adding(a):cannnel1, multipliction(m):channel2,target(t):channel3).
  // (a or m) and (t) markers oscillate (t1,t2),
  // all T+Uncertainty time steps (seq length>2T).
  unsigned int NbIn=4;
  const unsigned int NbOut=1,
    T=10, Uncertainty = (unsigned int) ceil((double)T/5.0);//min 0.
  //const double AddProp=0.0, ValueRange=2.0;
  const double AddProp=1.0, ValueRange=1;
  const bool ChangeSign=true, GlidingAverge=true, TmpMem=true;
  if(TmpMem) NbIn=3;
  unsigned int In;
  if(!apsPatData.SeqList) {
    MaxNbPat=2*T+(Uncertainty+1);//See Choose t1, t2 and set NbPat.
    apsPatData.NbSeq = 1;
    apsPatData.NbVal = NbIn+NbOut;
    apsPatData.NbPatTotal = 1; // Not to divide by zero somewhere.
    // NewPatData by hand.
    // Create and update only one entry in NbPat (and NbClass for logging).
    apsPatData.NbPat   = new unsigned int[apsPatData.NbSeq];
    apsPatData.NbPat[0] = MaxNbPat; // Will be changed dynamicly.
    apsPatData.NbClass = new unsigned int[apsPatData.NbSeq];
    apsPatData.NbClass[0] = 0; // Will be changed dynamicly.
    apsPatData.SeqList = new double**[apsPatData.NbSeq];
    // Create only one seq with MaxNbPat with NbVal.
    apsPatData.SeqList[0] = new double*[MaxNbPat];
    // Create Values for the pattern in the first seq.
    for(unsigned int pat=0;pat<MaxNbPat;pat++) {
      (apsPatData.SeqList[0])[pat] = new double[apsPatData.NbVal];
      // Set all to zero.
      for(unsigned int val=0;val<apsPatData.NbVal;val++)
	(((apsPatData.SeqList[0])[pat])[val])=0;
    }
  }
  // Start a new stream.
  if(SetBackToSeqStart) { t1=0; t2=0; AddMultiVal=0; AddMultiLastVal=0; }
  // Choose t1, t2.
  t1 = T; if(Uncertainty) t1-=(unsigned int)(lrand48() % (Uncertainty+1));
  t2 = t1+T; if(Uncertainty) t2+=(unsigned int)(lrand48() % (Uncertainty+1));
  // Set NbPat.
  apsPatData.NbPat[0] = t2+1; // Target marker comes on t2.
  for(unsigned int pat=0;pat<apsPatData.NbPat[0];pat++) {   // Pat loop.
    // Ramdom input on channel 0.
    ((apsPatData.SeqList[0])[pat])[0]=(drand48()*2*ValueRange-ValueRange);
    // Reset markers 1,2,(3).
    for(unsigned int val=1;val<NbIn;val++) 
      (((apsPatData.SeqList[0])[pat])[val])=0;
  }
  // Choose Addition or mutiplication.
  if(AddMultiVal && AddProp<1){ In=1; if(drand48()>AddProp) In=2; }
  else In=1; // Add at seq start.
  // Set the markers on channel 1,2,(3).
  ((apsPatData.SeqList[0])[t1])[In]=1; 
  ((apsPatData.SeqList[0])[t2])[NbIn-1]=1; // Target marker.
  // Set output.
  if(TmpMem) {
    AddMultiVal = ((apsPatData.SeqList[0])[t1])[0];
  } else if(GlidingAverge) {
    AddMultiVal = AddMultiLastVal + ((apsPatData.SeqList[0])[t1])[0];
    AddMultiLastVal = ((apsPatData.SeqList[0])[t1])[0];
  } else {
    if(In==1) AddMultiVal += ((apsPatData.SeqList[0])[t1])[0];
    else {
      if(ChangeSign) ((apsPatData.SeqList[0])[t1])[0] =
		       fabs(((apsPatData.SeqList[0])[t1])[0]);
      AddMultiVal *= ((apsPatData.SeqList[0])[t1])[0];
    }
  }
  (((apsPatData.SeqList[0])[t2])[NbIn]) = AddMultiVal;
}

void TPatternGenerator::Online_Timer(struct TPatData &apsPatData,
				     bool SetBackToSeqStart) {
  // Set the target all T time steps (or with uncertainty).
  // Input: 2 ways: T as input or (Doya) input all T steps,
  // then take the external input away and connnect to ouput (selfloop).
  // No markers. Timertick at times tick.
  // And we want the StepTarget, not constant output=1.
  SetBackToSeqStart=SetBackToSeqStart;//Not uesed, to avoid warnings use here.
  const unsigned int NbOut=1, T=50, Uncertainty=0;
  unsigned int NbIn;
  if(Uncertainty) NbIn=1; else NbIn=0; //NbIn=1;//ddd
  if(!apsPatData.SeqList) {
    MaxNbPat =  T+Uncertainty;
    apsPatData.NbVal = NbIn+NbOut;
    New_OnlinePatData(apsPatData); // Pass  MaxNbPat.
  }
  // Start a new stream. 
  // Reset output.
  //if(SetBackToSeqStart) { tick=0; }
  // Delete old timer tick in the output.
  //((apsPatData.SeqList[0])[tick])[1]=0;
  // Chose tick. Set NbPat. Timelenght(duration) of a Seq changes every time.
  tick=T-1+(unsigned int)(lrand48() % (Uncertainty+1)); 
  apsPatData.NbPat[0]=tick+1;
  // Start a new stream.  if(SetBackToSeqStart) {}
  for(unsigned int pat=0;pat<apsPatData.NbPat[0];pat++) { // Pat loop.
    if(NbIn)(((apsPatData.SeqList[0])[pat])[0])=tick-T+1; // Set input.
    ((apsPatData.SeqList[0])[pat])[NbIn]=0; // Reset output.
  }
  ((apsPatData.SeqList[0])[tick])[NbIn] = 1; // Set output.
}

void TPatternGenerator::Online_Oscillator(struct TPatData &apsPatData,
				     bool SetBackToSeqStart) {
  SetBackToSeqStart=
    SetBackToSeqStart;//Not uesed, to avoid warnings use here.
  const unsigned int NbOut=1, NbIn=0; // Frequency input.
  //const double Offset=0;// 0<Offset<MaxNbPat
  const unsigned int frequency=1; 
  MaxNbPat=10;  //discrete steps.
  double  MP2=(double)MaxNbPat*0.5;
  if(!apsPatData.SeqList) {
    apsPatData.NbVal = NbIn+NbOut;
    New_OnlinePatData(apsPatData); // Pass  MaxNbPat.
  }
  for(unsigned int pat=0;pat<MaxNbPat;pat++) { // Pat loop.
    if(NbIn)(((apsPatData.SeqList[0])[pat])[0])=frequency; // Set input.
    ((apsPatData.SeqList[0])[pat])[NbIn]= // Set Output
      // 0.5*(1-cos(frequency*2*M_PI*(pat+Offset)/(MaxNbPat-1)));//Cos [0-1]
    (pat-MP2>0); // Rectangular.
    //(double)((pat-2*(pat-MP2)*(pat-MP2>0)))/MP2; // Triangular
  }
}

void TPatternGenerator::Online_TimerClass(struct TPatData &apsPatData,
				     bool SetBackToSeqStart) {
  // Two classes on one output.
  // Input: spike train, output: last time delay.
  const unsigned int NbOut=1, NbIn=1, T=10, DT=2;
  unsigned int dt, NbPat;
  SetBackToSeqStart=SetBackToSeqStart;//Not uesed, here to avoid warnings.
  if(!apsPatData.SeqList) {
    MaxNbPat = T+DT;
    apsPatData.NbVal = NbIn+NbOut;
    New_OnlinePatData(apsPatData); // Pass  MaxNbPat.
    // Set constant In values at seq start.
    //((apsPatData.SeqList[0])[0])[0]=1;
  }
  //if(SetBackToSeqStart) ;// Set constant In values at seq start.
  //((apsPatData.SeqList[0])[0])[0]=1;
  //else ((apsPatData.SeqList[0])[0])[0]=0; // Reset.    
  //Timelenght (duration) of a Seq changes every time.
  dt = (unsigned int)(lrand48() % (DT+1)); NbPat=T+dt;
  apsPatData.NbPat[0] = NbPat;
  // Reset.
  for(unsigned int pat=T-1;pat<NbPat;pat++) {
    ((apsPatData.SeqList[0])[pat])[0]=0;
    ((apsPatData.SeqList[0])[pat])[1]=0;
  }
  // Set.
  ((apsPatData.SeqList[0])[NbPat-1])[0]=1;  // Input.
  ((apsPatData.SeqList[0])[NbPat-1])[1]=dt; // Output.
}

void TPatternGenerator::Online_AnBn(struct TPatData &apsPatData,
				     bool SetBackToSeqStart) {
  // a_n b_n language. Set step taget: 0.5,...,1,1,..,1,0 ; 2n Pats
  // Input symbol, output predict prop(a=0,b=1). General(ize) during test.
  SetBackToSeqStart=SetBackToSeqStart;//Not uesed, here to avoid warnings.
  const unsigned int NbIn=1, NbOut=1, N=5, dN=5;
  unsigned int n, dn=dN+apsPatData.Generalize;
  if(!apsPatData.SeqList) {
    MaxNbPat =  2*N+2*(dN+TestData.Generalize);//Delete save also for Train.
    apsPatData.NbVal = NbIn+NbOut; 
    New_OnlinePatData(apsPatData); //Pass MaxNbPat.
  }
  n=N+(unsigned int)(lrand48() % (dn+1));// Include dn!!
  apsPatData.NbPat[0]=2*n;
  for(unsigned int pat=0;pat<n;pat++) { // Set n times a.
    ((apsPatData.SeqList[0])[pat])[0]=0; // Input a.
    ((apsPatData.SeqList[0])[pat])[NbIn]=0.5; }// Output unknown.
  for(unsigned int pat=n;pat<2*n;pat++) { // Set n times b.
    ((apsPatData.SeqList[0])[pat])[0]=1; // Input b.
    ((apsPatData.SeqList[0])[pat])[NbIn]=1; }// Output b.
  ((apsPatData.SeqList[0])[2*n-1])[NbIn]=0;//Except: for last b: output a.
}









