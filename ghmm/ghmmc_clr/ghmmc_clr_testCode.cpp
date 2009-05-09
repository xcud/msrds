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


void generateModel (ghmm_dmodel *mo, int noStates, unsigned int seed) {

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
  mo->M = 4;
  mo->maxorder = 1;
  mo->prior = -1;

  /* Model has Higher order Emissions and labeled states*/
  mo->model_type = GHMM_kLabeledStates;
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
    states[i].pi = (0==i ? 1.0:0.0);
    states[i].fix = 0;
    states[i].out_states = 2;
    states[i].in_states = 2;
    mo->label[i] = i%4;
    mo->order[i] = i%2;

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
      rnd = rand()/(double)RAND_MAX;
      for (j=h*mo->M; j<(h*mo->M+mo->M); j++){
	states[i].b[j] = ( (0==((i+j)%mo->M)) ? rnd : (1-rnd) / (mo->M-1));
      }
    }

    if ((mo->N-1)==i) {
      states[i].out_id[0] = 0;
      states[i].out_id[1] = i;
    }
    else {
      states[i].out_id[0] = i;
      states[i].out_id[1] = i+1;
    }

    if (0==i) {
      states[i].in_id[0]  = i;
      states[i].in_id[1]  = mo->N-1;
    }
    else {
      states[i].in_id[1]  = i-1;
      states[i].in_id[0]  = i;
    }

    states[i].out_a[0] = 0.5;
    states[i].out_a[1] = 0.5;
    states[i].in_a[0]  = 0.5;
    states[i].in_a[1]  = 0.5;

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

void ghmm::label_higher_order_test(void)
{
	int seqlen	 = 2700;

  int  error;
 
  ghmm_dmodel * mo_gen = NULL;
  ghmm_dmodel * mo_time = NULL;
  ghmm_dmodel * mo_mem  = NULL;
  ghmm_dseq * my_output = NULL;
	
  FILE * _File = fopen("label_higher_order_test.txt","w");

  if (!(mo_gen = (ghmm_dmodel*)malloc (sizeof (ghmm_dmodel))))
    {printf ("malloc failed in line %d", __LINE__); exit(1);}
  if (!(mo_time = (ghmm_dmodel*)malloc (sizeof (ghmm_dmodel))))
    {printf ("malloc failed in line %d", __LINE__); exit(1);}
  if (!(mo_mem = (ghmm_dmodel*)malloc (sizeof (ghmm_dmodel))))
    {printf ("malloc failed in line %d", __LINE__); exit(1);}
      
  /* generate a model with variable number of states*/
  generateModel(mo_gen,  7, 92304);
  generateModel(mo_time, 5, 1704);
  generateModel(mo_mem,  5, 1111);

  /*generate a random sequence*/
  int NR_SEQUENCES = 50;
  my_output = ghmm_dmodel_label_generate_sequences(mo_gen, 0, seqlen, NR_SEQUENCES, seqlen);
  /* ghmm_dmodel_add_noise(mo_time, .499, 0); */
  /* randomize the second */
  /* ghmm_dmodel_add_noise(mo_mem, .499, 0); */

  ghmm_dmodel_print(_File, mo_time);
  ghmm_dmodel_print(_File, mo_mem);
  fprintf(_File,"Distance between the two models: %g\n\n", ghmm_dmodel_distance(mo_time, mo_mem));

  /* shifting both models in diffrent directions */
  /* train the first */	 
  /*ghmm_dmodel_label_baum_welch(mo_time, my_output);*/
  error = ghmm_dmodel_baum_welch(mo_time, my_output);

  /* train the second and hope they are equal */
  error = ghmm_dmodel_baum_welch(mo_mem, my_output);

  ghmm_dmodel_print(_File, mo_time);
  ghmm_dmodel_print(_File, mo_mem);
  fprintf(_File,"Distance between the two trained models: %g\n", ghmm_dmodel_distance(mo_time, mo_mem));

  fprintf(_File,"Log-Likelyhood generating:    %g\n", ghmm_dmodel_likelihood (mo_gen, my_output));
  fprintf(_File,"Log-Likelyhood fb-Baum-Welch: %g\n", ghmm_dmodel_likelihood (mo_time, my_output));
  fprintf(_File,"Log-Likelyhood me-Baum-Welch: %g\n", ghmm_dmodel_likelihood (mo_mem, my_output));

	fclose(_File);

  /* freeing memory */
  ghmm_dmodel_free(&mo_gen);
  ghmm_dmodel_free(&mo_time);
  ghmm_dmodel_free(&mo_mem);
  
  ghmm_dseq_free(&my_output);
}
void ghmm::libxml_print(void)
{

 char *docname, *writename;
  ghmm_xmlfile* f;
  int i;
  ghmm_set_loglevel(5+1);

  //if(argc <= 1) {
  //  printf("Usage: %s docname.xml", argv[0]);
  //  return(0);
  //}

  docname = "xml_example.xml";

	ghmm_dmodel ** m;
	int count;
	m = ghmm_dmodel_xml_read( docname, &count);

	ghmm_dmodel_xml_write(m,"xml_example2.xml",count);

  f = ghmm_xmlfile_parse(docname);
  /* simple test */
  if (f) {
    for (i=0;i<f->noModels; i++){
      switch (f->modelType & (GHMM_kDiscreteHMM + GHMM_kTransitionClasses
			      + GHMM_kPairHMM + GHMM_kContinuousHMM)) {
      case GHMM_kContinuousHMM:
        ghmm_cmodel_print(stdout, f->model.c[i]);
        break;
      case GHMM_kDiscreteHMM:
        ghmm_dmodel_print(stdout, f->model.d[i]);
      default:
        break;
      }
    }
  }

  
    
    ghmm_xmlfile_write(f, "simple_graph2.xml");
  
}

void ghmm::sequence_alloc_print(void)
{
  ghmm_dseq* seq_array;
  int i;

  seq_array= ghmm_dseq_calloc(2);
  seq_array->seq_len[0]=10;
  seq_array->seq_id[0]=101.0;
  seq_array->seq[0]=(int*)malloc(seq_array->seq_len[0]*sizeof(int));

  for (i=0; i<seq_array->seq_len[0]; i++)
    seq_array->seq[0][i]=1;

    seq_array->seq_len[1]=15;
  seq_array->seq_id[1]=101.0;
  seq_array->seq[1]=(int*)malloc(seq_array->seq_len[1]*sizeof(int));

  for (i=0; i<seq_array->seq_len[1]; i++)
    seq_array->seq[1][i]=1;


  FILE * _File = fopen("sequence_alloc_print_xml.txt","w");

  ghmm_dseq_print_xml(seq_array, _File);
	fclose(_File);

  _File = fopen("sequence_alloc_print.txt","w");

  ghmm_dseq_print(seq_array, _File);
	fclose(_File);

  ghmm_dseq_free(&seq_array);
}


bool ghmm::Test_two_states_three_symbols ()
{
	ghmm_dmodel * my_model = new ghmm_dmodel();
	
	

	ghmm_dstate model_states[2];

			
	/* first state */
	/*  probability of emmission of 0,1 or 2 */
	double symbols_0_state[3]={0.5,0.5,0.0};
	/* transition to which state is given in the following arrays */
	int trans_id_0_state[2]={0,1};
	/* transition probability from here to 0-state (self) and 1-state */
	double trans_prob_0_state[2]={0.9,0.1};
	/* transition probability from 0-state (self) and 1-state to this state */
	double trans_prob_0_state_rev[2]={0.9,0.1};

	/* second state , comments see above */
	double symbols_1_state[3]={0.0,0.0,1.0};
	int trans_id_1_state[2]={0,1};
	double trans_prob_1_state[2]={0.1,0.9};
	double trans_prob_1_state_rev[2]={0.1,0.9};
	ghmm_dseq* my_output;

	int pow_look[2] = {1,3};

	/* flags indicating whether a state is silent */
	/*int silent_array[2] =  {0,0};*/

	/* initialise state 0 */
	/* start probability for this state */
	model_states[0].pi = 0.5;
	/* array with emission probabilities */
	model_states[0].b=symbols_0_state;
	/* number of fields in out_a and out_id */
	model_states[0].out_states=2;
	/* transition probability from this state */
	model_states[0].out_a=trans_prob_0_state;
	/* state ids belonging to the probability */
	model_states[0].out_id=trans_id_0_state;
	/* transition probability to this state */
	/* in_states,in_id and in_a have the same function as above*/
	model_states[0].in_states=2;
	model_states[0].in_id=trans_id_0_state;
	model_states[0].in_a=trans_prob_0_state_rev;
	/* should emission probabilities be changed during reestimation? 1: no, else: yes*/
	model_states[0].fix=0;

	/* initialise state 1 */
	/* same meaning as above */
	model_states[1].pi = 0.5;
	model_states[1].b=symbols_1_state;
	model_states[1].out_states=2;
	model_states[1].out_a=trans_prob_1_state;
	model_states[1].out_id=trans_id_1_state;
	model_states[1].in_states=2;
	model_states[1].in_id=trans_id_0_state;
	model_states[1].in_a=trans_prob_1_state_rev;
	model_states[1].fix=0;

	/* initialise model */
	my_model->N=2; /* number of states, dimension of model.s */
	my_model->M=3; /* number of symbols, dimension of states.b */
	my_model->s=model_states; /* array of states */
	my_model->prior=-1; /* probability of this model, used in a model array */

	/*my_model.silent = silent_array;*/
	my_model->pow_lookup = pow_look;
	my_model->maxorder = 0;
	my_model->model_type =0;


	char buf[1024];

	FILE * _File = fopen("Test_two_states_three_symbols.txt","w");

	/* consistency check */
	fprintf(_File,"checking model:\n");
	if (ghmm_dmodel_check(my_model))
	{
	  fprintf(stderr,"ghmm_dmodel_check failed!\n");
	  return 1;
	}


	fprintf(_File,"model is ok\n");
	

	/* print model parameters */
	fprintf(_File,"two_states_three_symbols model:\n");
	ghmm_dmodel_print(_File,my_model);

	/* generate sequences */
	fprintf(_File,"generating sequences:...");
	my_output=ghmm_dmodel_generate_sequences(my_model, /* model */
					 1,   /* random seed */
					 10, /* length of each sequence */
					 5, /* no of sequences */
	  				 8); /* maxT */ 
	fprintf(_File,"Done\n");

	ghmm_dseq_print(my_output,_File);

	/* slight change of emission probabilities in state 0 */
	symbols_0_state[0] = 0.6;
	symbols_0_state[1] = 0.4;
	symbols_0_state[2] = 0.0;

	/* reestimation */
	fprintf(_File,"reestimating with Baum-Welch-algorithm...");
	ghmm_dmodel_baum_welch(my_model,my_output);

	/* print the result */
	fprintf(_File,"Done\nthe result is:\n");
	ghmm_dmodel_print(_File,my_model);

	ghmm_dseq_free(&my_output);

	fclose(_File);



	ghmm_dmodel_xml_write2(my_model, "tsetrfs.xml", 1);

	//ghmm_dmodel_states * 
}
}