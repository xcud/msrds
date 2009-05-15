#include "stdafx.h"

#include "ghmmc_clr.h"

#include "../ghmm/ghmm.h"
#include "../ghmm/xmlreader.h"
#include "../ghmm/xmlwriter.h"

#include "../ghmm/ghmm_internals.h"



#pragma unmanaged

#include <math.h>

#pragma managed

namespace ghmmc_clr {

//#define MODEL_SIZE 10

void generateDigitModel (ghmm_dmodel *mo, int noStates,int noSimbol, unsigned int seed) {

  ghmm_dstate * states;
  int h, i, j, hsize;
  double rnd;

  /* flags indicating whether a state is silent */
  int *silent_array;

  /* init the random number generator */
  //srandom(seed);
  srand(seed);

  /*allocate memory for states and array of silent flags*/
  if (!(states =(ghmm_dstate *) malloc (sizeof (ghmm_dstate) * noStates)))
    {printf ("malloc failed in line %d", __LINE__); exit(1);}
  if (!(silent_array = (int *)calloc (sizeof (int), noStates))) 
    {printf ("malloc failed in line %d", __LINE__); exit(1);}

  mo->N = noStates;
  mo->M = noSimbol;
  mo->maxorder = 0;
  mo->prior = -1;

  /* Model has Higher order Emissions and labeled states*/
  mo->model_type = GHMM_kLabeledStates | GHMM_kDiscreteHMM;
  if (!(mo->label = (int*)malloc(sizeof(int) * mo->N))) 
    {printf ("malloc failed in line %d", __LINE__); exit(1);}
  if (mo->maxorder>0) {
    mo->model_type += GHMM_kHigherOrderEmissions;
    if (!(mo->order = (int*)malloc(sizeof(int) * mo->N))) 
      {printf ("malloc failed in line %d", __LINE__); exit(1);}
  }

  /* allocate memory for pow look-up table and fill it */
  if (!(mo->pow_lookup = (int*)malloc(sizeof(int) * (mo->maxorder+2)))) 
    {printf ("malloc failed in line %d", __LINE__); exit(1);}
  
  mo->pow_lookup[0] = 1;
  for (i=1; i<mo->maxorder+2; i++)
    mo->pow_lookup[i] =  mo->pow_lookup[i-1] * mo->M;

  /*initialize states*/
  for (i=0; i < mo->N; i++) {
	  states[i].desc = "state";

    states[i].pi = (0==i ? 1.0:0.0);
    states[i].fix = 0;

	// 
    states[i].out_states = ((mo->N-1) == i ? 1:2);
	// left right architecture
    states[i].in_states = (0 == i ? 1:2);
	//states[i].in_states = 2;
    mo->label[i] = i%4;
//    mo->order[i] = i%2;

    if (mo->model_type & GHMM_kHigherOrderEmissions)
      hsize = (int)pow((float)mo->M, mo->order[i]);
    else
      hsize = 1;

    /* allocate memory for the a, the out- and incoming States and b array for higher emmission order states*/
    states[i].b      = (double *)malloc(sizeof(double) * hsize*mo->M);
    states[i].out_id = (int*)malloc(sizeof(int)*states[i].out_states);
    states[i].in_id  = (int*)malloc(sizeof(int)*states[i].in_states);
    states[i].out_a  = (double*)malloc(sizeof(double)*states[i].out_states);
    states[i].in_a   = (double*)malloc(sizeof(double)*states[i].in_states);

    for (h=0; h<hsize; h++) {
      //rnd = rand()/(double)RAND_MAX;
      for (j=h*mo->M; j<(h*mo->M+mo->M); j++){
		states[i].b[j] = 1.0/mo->M;
      }
    }
    if (0==i) {
      states[i].in_id[0]  = i;
      states[i].in_a[0]  = 1.0;

    }
    else {
      states[i].in_id[1]  = i-1;
      states[i].in_id[0]  = i;
      states[i].in_a[0]  = 0.5;
	  states[i].in_a[1]  = 0.5;

    }

    if ((mo->N-1)==i) {
      states[i].out_id[0] = i;
      states[i].out_a[0] = 1.0;
    }
    else {
      states[i].out_id[0] = i;
      states[i].out_id[1] = i+1;
      states[i].out_a[0] = 0.5;
     states[i].out_a[1] = 0.5;

    }


#ifdef DEBUG
    printf("State %d goto    : %d, %d\n", i, states[i].out_id[0], states[i].out_id[1]);
    printf("State %d comefrom: %d, %d\n", i, states[i].in_id[0],  states[i].in_id[1]);
    printf("State %d goto    : %g, %g\n", i, states[i].out_a[0], states[i].out_a[1]);
    printf("State %d comefrom: %g, %g\n", i, states[i].in_a[0],  states[i].in_a[1]);
#endif
  }

  mo->s = states;
  mo->silent = silent_array;

#ifdef DEBUG
  for (i = 0; i < mo->N; i++) {
    printf("\n State %d:\n", i);
    for (j = 0; j < pow(mo->M,states[i].order+1); j++){
      printf("%g ",mo->s[i].b[j]);
    }
  }
#endif

  /* ghmm_dmodel_print(stdout, mo); */

}

// 

ghmm_dseq* gen_seq(int * seq,int len)
{

	int count  = (len +1) * 8;

	ghmm_dseq* seq_array = ghmm_dseq_calloc(count);
	
	
	seq_array->seq_number = count;
	seq_array->total_w = 0.0;
	seq_array->seq_w = (double*)malloc(count*sizeof(double));
	
	
	int index = 0;


	for(int y = 0;y < len+1;y++)
	{

		for(int s = 0;s < 8 ;s++)
		{
			seq_array->seq_len[index]=len +1;
			seq_array->seq_id[index]=101.0;
			seq_array->seq[index]=(int*)malloc(seq_array->seq_len[index]*sizeof(int));
			seq_array->seq_w[index] = 1.0;
			int i = 0;
			for(int n = 0;n < len+1;n++)
			{
					
							
				if( y == n)
				{
					seq_array->seq[index][n] = s;
				}else
				{
					seq_array->seq[index][n] = seq[i++];
				}

				
			}

			index++;
		}

	
	}	

	return seq_array;
}



ghmm_dmodel * ghmm::CreateDigitModel(System::Collections::Generic::List<int> ^ seq)
{
	ghmm_dmodel * mo_gen = NULL;
	ghmm_dseq * my_output = NULL;


	if (!(mo_gen = (ghmm_dmodel*)malloc (sizeof (ghmm_dmodel))))
	    {printf ("malloc failed in line %d", __LINE__); exit(1);}
	
	mo_gen->model_type = GHMM_kDiscreteHMM;
	mo_gen->name = "perpet";
	generateDigitModel(mo_gen,4  , 8,92304);

	//int * temp_seq = new int[seq->Count];
	
	int * ghmm_seq = (int*)malloc(seq->Count*sizeof(int));

	int i = 0;
	for each(int s in seq)
	{
		ghmm_seq[i] = s;
		i++;
		
	}

	my_output = gen_seq(ghmm_seq,i);

	 ghmm_dmodel_baum_welch(mo_gen, my_output);
	return mo_gen;
}

				
void ghmm::Test_digit_code()
{
int seqlen	 = 2700;

  int  error;
 
  ghmm_dmodel * mo_gen = NULL;
  ghmm_dmodel * mo_gen2 = NULL;
  ghmm_dmodel * mo_gen3 = NULL;

  ghmm_dseq * my_output = NULL;
  ghmm_dseq * my_output2 = NULL;
  ghmm_dseq * my_output3 = NULL;
	
  FILE * _File = fopen("label_higher_order_test2.txt","w");

  if (!(mo_gen = (ghmm_dmodel*)malloc (sizeof (ghmm_dmodel))))
    {printf ("malloc failed in line %d", __LINE__); exit(1);}
  if (!(mo_gen2 = (ghmm_dmodel*)malloc (sizeof (ghmm_dmodel))))
    {printf ("malloc failed in line %d", __LINE__); exit(1);}
  if (!(mo_gen3 = (ghmm_dmodel*)malloc (sizeof (ghmm_dmodel))))
    {printf ("malloc failed in line %d", __LINE__); exit(1);}
      

  mo_gen->alphabet = NULL;
	mo_gen->name = NULL;



  /* generate a model with variable number of states*/
  generateDigitModel(mo_gen,4  , 8,92304);
  generateDigitModel(mo_gen2,4  , 8,92304);
  generateDigitModel(mo_gen3,4  , 8,92304);

  //generateModel(mo_time, 5, 1704);
  //generateModel(mo_mem,  5, 1111);

  int seq[6] = {1,7,5,5,0,0};
	my_output = gen_seq(seq,6);

  //int seq2[6] = {1,7,6,5,5,0};
	int seq2[6] = {6,7,6,5,6,0};
	my_output2 = gen_seq(seq2,6);

  int seq3[6] = {1,7,7,5,5,0};
	my_output3 = gen_seq(seq3,6);

	/*generate a random sequence*/

  ghmm_dmodel_print(_File, mo_gen);

  error = ghmm_dmodel_baum_welch(mo_gen, my_output);
//  error = ghmm_dmodel_baum_welch(mo_gen2, my_output2);
 // error = ghmm_dmodel_baum_welch(mo_gen3, my_output3);

  //ghmm_dmodel_baum_welch_nstep(mo_gen,my_output,500,GHMM_EPS_ITER_BW);


  ghmm_dmodel_print(_File, mo_gen);

//fprintf(_File,"Distance between the two trained models2: %g\n", ghmm_dmodel_distance(mo_gen, mo_gen2));
//fprintf(_File,"Distance between the two trained models3: %g\n", ghmm_dmodel_distance(mo_gen, mo_gen3));


  fprintf(_File,"Log-Likelyhood generating1:    %g\n", ghmm_dmodel_likelihood (mo_gen, my_output));
  fprintf(_File,"Log-Likelyhood generating2:    %g\n", ghmm_dmodel_likelihood (mo_gen, my_output2));
  fprintf(_File,"Log-Likelyhood generating3:    %g\n", ghmm_dmodel_likelihood (mo_gen, my_output3));


	fclose(_File);

  /* freeing memory */
  ghmm_dmodel_free(&mo_gen);
  
  ghmm_dseq_free(&my_output);
}

}