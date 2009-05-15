// ghmmc_clr.h

#pragma once

//#pragma managed(push,off)
//#include "myNativeClass.h"
//#pragma managed(pop)


#pragma unmanaged

#include "../win_config.h"


#include <stdio.h>
#include <stdlib.h>

#include <map>

#include "../ghmm/matrix.h"
#include "../ghmm/rng.h"
#include "../ghmm/sequence.h"
#include "../ghmm/model.h"
#include "../ghmm/viterbi.h"
#include "../ghmm/foba.h"

#include "../ghmm/obsolete.h"
#include "../ghmm/reestimate.h"

 
#pragma managed

//#include <cliext/vector>


using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

#define MAX_INDEX 8

namespace ghmmc_clr {
 

	public ref class model
	{
	public:

		model()
		{
			_m = NULL;
			//ghmm_dmodel **mo = NULL;
			//ARRAY_CALLOC (mo, sq->seq_number);

		}

		ghmm_dmodel * _m;


		void Print(System::String ^ str)
		{
			const char* str2 = (char*)(void*)Marshal::StringToHGlobalAnsi(str);
			
			FILE * _File = fopen(str2,"w");

			ghmm_dmodel_print(_File,_m);

			fclose(_File);

			Marshal::FreeHGlobal((System::IntPtr)(void*)str2);
		}

		void Save(System::String ^ str)
		{
			if( _m == NULL)
				return;

			const char* str2 = (char*)(void*)Marshal::StringToHGlobalAnsi(str);
			
			//FILE * _File = fopen(str2,"w");
			
			//pin_ptr<ghmm_dmodel**> m = &_m;
			
			
			_m->model_type = GHMM_kDiscreteHMM;// + GHMM_kLeftRight ;//+ GHMM_kTransitionClasses;

			ghmm_dmodel_xml_write2(_m, str2, 1);
			
			Marshal::FreeHGlobal((System::IntPtr)(void*)str2);

			//fclose(_File);
		}

		void Load(System::String ^ str)
		{

			

			const char* str2 = (char*)(void*)Marshal::StringToHGlobalAnsi(str);
			
			//FILE * _File = fopen(str2,"r");
			



			int count;
			ghmm_dmodel ** m;
			m = ghmm_dmodel_xml_read( str2, &count);

			Marshal::FreeHGlobal((System::IntPtr)(void*)str2);

			if( m != NULL)
				_m = m[0];
			
			//FILE * _File = fopen(str2,"r");

			//fclose(_File);
		}


	};


	public ref class ghmm
	{
	public:

		//std::map<int,ghmm_dmodel *>* _ModelList;
		System::Collections::Generic::Dictionary<int,model^>^ _ModelList;
		System::Collections::Generic::Dictionary<int,XMLLib::Digit^>^ _DigitXML;

		void Init()
		{
			ghmm_rng_init();

			//_ModelList = new std::map<int,ghmm_dmodel *>;
			_ModelList = gcnew System::Collections::Generic::Dictionary<int,model^>;
			_DigitXML = gcnew System::Collections::Generic::Dictionary<int,XMLLib::Digit^>;
		}
		void sequence_alloc_print(void);
		void libxml_print(void);
		bool Test_two_states_three_symbols ();
		void label_higher_order_test(void);
		
		void Test_digit_code();
		
		ghmm_dmodel * CreateDigitModel(System::Collections::Generic::List<int> ^ seq);

		void Save()
		{
			for each( KeyValuePair<int, model^> pair in _ModelList)
			{
				pair.Value->Save("ghmm_" + pair.Key.ToString() + ".txt");
				pair.Value->Print("ghmm__" + pair.Key.ToString() + ".txt");
			}
		}


		
		void Load()
		{
			for(int i = 0;i < 10 ; i++)
			{
				model^ m = gcnew model;
				m->Load(String::Format("{0}_model.xml",i));
				if( m->_m != NULL)
				{
					_ModelList->Add(i,m);
				}
			}
		}

		int Estimate(System::Collections::Generic::List<int> ^ seq)
		{

			 int * state_seq ;

			


			int * ghmm_seq = (int*)malloc(seq->Count*sizeof(int));

				int i = 0;
				for each(int s in seq)
				{
					ghmm_seq[i] = s;
					i++;
					
				}

				int key = -1;

				ghmm_dseq* seq_array = ghmm_dseq_calloc(1);

				seq_array->seq_len[0]=seq->Count;
				seq_array->seq_id[0]=101.0;
				seq_array->seq[0]=(int*)malloc(seq_array->seq_len[0]*sizeof(int));
				
				
				i = 0;
				for each(int s in seq)
				{
					seq_array->seq[0][i] = s;
					i++;
				}
				
				
				
				double last_log_p = -99999999;


			for each( KeyValuePair<int, model^> pair in _ModelList)
			{
				
				//int result = ghmm_dmodel_logp(pair.Value->_m,ghmm_seq, seq->Count,&log_p);
				
				//viterbi
				 ghmm_dmodel_normalize(pair.Value->_m);
				//log_p = ghmm_dmodel_viterbi_logp(pair.Value->_m, ghmm_seq, seq->Count,state_seq); 
				double log_p = ghmm_dmodel_likelihood(pair.Value->_m,seq_array);
				
				//state_seq = ghmm_dmodel_viterbi_logp(pair.Value->_m, ghmm_seq, seq->Count,&log_p); 

				if( last_log_p < log_p)
				{
					key = pair.Key;
					last_log_p = log_p;
				}
			}
			
			ghmm_dseq_free(&seq_array);


			return key;
		}
		
		void Seq_Add(int model_id,System::Collections::Generic::List<int> ^ seq)
		{
			XMLLib::Digit ^ digit ;
			if( _DigitXML->TryGetValue(model_id,digit) == false)
			{
				digit = gcnew XMLLib::Digit;
				_DigitXML->Add(model_id,digit);
				digit->_NUM = model_id;
			}

			XMLLib::SIMBOLLIST ^ sl = gcnew XMLLib::SIMBOLLIST;
			sl->_SIMBOL = seq;
			
			digit->_SIMBOLLIST->Add(sl);
		}

		void Seq_Save(int model_id)
		{
			XMLLib::Xml::SaveXml<XMLLib::Digit^>(String::Format("{0}_seq.xml",model_id),_DigitXML[model_id]);
		}

		void Seq_Load(int model_id)
		{
			XMLLib::Digit ^ seq = XMLLib::Xml::LoadXml<XMLLib::Digit^>(String::Format("{0}_seq.xml",model_id));

			_DigitXML->Add(model_id,seq);
			
		}

		void Model_Load(int model_id)
		{
		}

		void Model_Save(int model_id)
		{
			_ModelList[ model_id]->Save(String::Format("{0}_model.xml",model_id));
		}


		void Model_Lean(int model_id)
		{
			model ^ m = gcnew model;
			
			static char * str[MAX_INDEX] = {"0","1","2","3","4","5","6","7"};

			

			m->_m = CreateDigitModel(_DigitXML[model_id]->_SIMBOLLIST[0]->_SIMBOL);
			
			m->_m->alphabet = new ghmm_alphabet;
			m->_m->alphabet->id = 0;
			m->_m->alphabet->size = MAX_INDEX;
			m->_m->alphabet->description = "test";
			m->_m->alphabet->symbols =  str;
		
			_ModelList->Add(model_id,m);

			FILE * _File = fopen("lean.txt","w");

			ghmm_dmodel_print(_File, m->_m);

			fclose(_File);
		}

			

		void Add_Lean(int model_id,System::Collections::Generic::List<int> ^ seq)
		{
			//_ModelList->is

			model ^ m ;

			if( _ModelList->TryGetValue(model_id,m) == false)
			{
				m = gcnew model;
				_ModelList->Add(model_id,m);
				int * ghmm_seq = (int*)malloc(seq->Count*sizeof(int));

				int i = 0;
				for each(int s in seq)
				{
					ghmm_seq[i] = s;
					i++;
					
				}

				m->_m = ghmm_dmodel_generate_from_sequence (ghmm_seq, seq->Count,MAX_INDEX);

				//ghmm_alphabet
				//m->_m->
				//static char * str[36] = {"0","1","2","3","4","5","6","7","8","9",
				//		"10","11","12","13","14","15","16","17","18","19",
				//		"20","21","22","23","24","25","26","27","28","29",
				//		"30","31","32","33","34","35"};

				static char * str[MAX_INDEX] = {"0","1","2","3","4","5","6","7"};
						
				 
				m->_m->alphabet = new ghmm_alphabet;
				m->_m->alphabet->id = 0;
				m->_m->alphabet->size = MAX_INDEX;
				m->_m->alphabet->description = "test";
				m->_m->alphabet->symbols =  str;

			}else
			{
				
				ghmm_dseq* seq_array = ghmm_dseq_calloc(1);

				seq_array->seq_len[0]=seq->Count;
				seq_array->seq_id[0]=101.0;
				seq_array->seq[0]=(int*)malloc(seq_array->seq_len[0]*sizeof(int));
				
				int i = 0;
				for each(int s in seq)
				{
					seq_array->seq[0][i] = s;
					i++;
				}
				
				ghmm_dmodel_baum_welch(m->_m,seq_array);
				
				ghmm_dseq_free(&seq_array);
			}
			

			//for (int i=0; i<seq_array->seq_len[0]; i++)
				
			

			
			

			

			//m->_m

			//ghmm_dmodel

//			ghmm_dseq * sq;

//			reestimate_baum_welch(m->_m,); 

		}


		// TODO: Add your methods for this class here.
	};
}
