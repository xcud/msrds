// rtrl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"



#include <math.h>
#include <stdio.h>
#include <stdlib.h>



#define max_units 15
#define max_inputs 5
#define max_outputs 3
#define max_sequence_length 100
#define max_training_size 200
#define max_test_size 200

FILE *fp1,*fp2;
char *outfile,*weightfile;
int 
  /*number inputs */
  in_mod,
  /*number outputs */
  out_mod,
  /*number hidden units */
  hid_mod, 
  /*number input and hidden units */
  hi_in_mod,
  /* number of all units */
  ges_mod,
  /* current sequence of the training set */
  examples,
  /* random number generator init */
  ran_sta,
  /* performing a test on a test  set? if ((epoch%test_aus==0)||(class_err<min_fehl)) */
  test_aus,
  min_fehl,
  /* write the weight matrix after w_out epochs */
  w_out,
  /* should the net still be learning --> learn=1 */
  learn,
  /* is stop to learn set? --> stop_learn=1 */
  stop_learn,
  /* max. number of trials to be performed */
  maxtrials,
  /* reset after each sequence? */
  sequ_reset,
  training_size,
  test_size,
  /* max. number of epochs before learning stop*/
  maxepoch,
  /* bias1==1 --> units biased ? */
  bias1,
  /* is target provided for the current input */
  targ,
  /* element in a sequence of the test set */
  element_t,
  /* number of sequences in test set */
  example_t,
  /* misclassifications per epoch */
  class_err,
  /* when to stop learning: wrong classifications per epoch < stop_class */
  stop_class,
  /* epochs to be learned after stop learning is set */
  ext_epochs,
  /* 1 if the current sequence has been processed correctly so far */
  seq_cor,
  /* 1 if end of the test set */
  test_end,
  /* weight_up == 1 then weight update */
  weight_up,
  /* w_up if w_up == 1 then weight update per sequence otherwise per epoch*/
  w_up,
  /* element in a sequence of the training set */
  element,
  /* number of sequences in training set */
  example,
  /* number of sequences seen by the net */
  numb_seq,
  /* number of epochs seen by the net */
  epoch,
  /* length of the training sequences */
  length[max_training_size],
  /* length of the test sequences */
  length_t[max_test_size];


double 
  /* weight matrix */
  W_mod[max_units][max_units],
  /* contribution to update of weight matrix */
  DW[max_units][max_units],
  /* new activation for all units */
  Yk_mod_new[max_units],
  /* old activation for all units */
  Yk_mod_old[max_units],
  /* dyk / dwij derivatives of units with respect to weights */
  Pk_ijm_mod[max_units][max_units][max_units], 
  /* old values of dyk / dwij derivatives of units with respect to weights */
  Pk_ijm_mod_o[max_units][max_units][max_units],
  /* learing rate */
  alpha, 
  /* interval of weight initialization is init_range*[-1,1] */
  init_range, 
  /* current target */
  target_a[max_units],
  /* matrix for storing the target per training element per
     seqence element per output component */
  tar[max_training_size][max_sequence_length][max_outputs],
  /* matrix for storing the input per training element per
seqence element per input component */
  inp[max_training_size][max_sequence_length][max_inputs],
  /* matrix for storing the target per test element per
seqence element per output component */
  tar_t[max_test_size][max_sequence_length][max_outputs],
  /* matrix for storing the input per test element per
seqence element per input component */
  inp_t[max_test_size][max_sequence_length][max_inputs],
  /* MSE per epoch */
  epoch_err,
  /* MSE per sequence */
  seq_err,
  /* when to stop learning: MSE per epoch < stop_mse */
  stop_mse,
  /* error for output units */
  error[max_units],
  /* max. allowed error per sequence for correct classification */
  seq_max; 


/* the output and weight files for 20 trials */
static char
*outf[] = {           "outa.txt",
		      "outb.txt",
		      "outc.txt",
		      "outd.txt",
		      "oute.txt",
		      "outf.txt",
		      "outg.txt",
		      "outh.txt",
		      "outi.txt",
		      "outj.txt",
		      "outk.txt",
		      "outl.txt",
		      "outm.txt",
		      "outn.txt",
		      "outo.txt",
		      "outp.txt",
		      "outq.txt",
		      "outr.txt",
		      "outs.txt",
		      "outt.txt"         },

  *weig[] = {           "weia.par",
			"weib.par",
			"weic.par",
			"weid.par",
			"weie.par",
			"weif.par",
			"weig.par",
			"weih.par",
			"weii.par",
			"weij.par",
			"weik.par",
			"weil.par",
			"weim.par",
			"wein.par",
			"weio.par",
			"weip.par",
			"weiq.par",
			"weir.par",
			"weis.par",
			"weit.par"         };

long random();
void srandom();

int seprand(int k)
{
  long l;
  int f;
  l = rand();
  f = l % k;
  return(f);
}

void reset_net()
{
  int i,j,v;

  for (i=0;i<ges_mod;i++)
    {
      Yk_mod_new[i]=0.5;
      Yk_mod_old[i]=0.5;
    }
  for (i=in_mod;i<ges_mod;i++)
    for (j=in_mod;j<ges_mod;j++)
      for (v=0;v<ges_mod;v++)
	{
	  Pk_ijm_mod[i][j][v]=0; 
	  Pk_ijm_mod_o[i][j][v]=0;
	}



}

void set_input_t()
{
  int i,j,k;
  double max;
  if (bias1==0)
    {
      for (i=0;i<in_mod;i++)
	{
	  Yk_mod_new[i]=inp_t[example_t][element_t][i];
	  Yk_mod_old[i]=Yk_mod_new[i];
	}
    }
  else
    {
      for (i=0;i<in_mod-1;i++)
	{
	  Yk_mod_new[i]=inp_t[example_t][element_t][i];
	  Yk_mod_old[i]=Yk_mod_new[i];
	}
      Yk_mod_new[in_mod-1]=1.0;
      Yk_mod_old[in_mod-1]=1.0;
    }

  max=0;
  for (k=hi_in_mod,j=0;k<ges_mod;k++,j++)
    {
      target_a[j]=tar_t[example_t][element_t][j];
      if (fabs(tar_t[example_t][element_t][j])>max)
	max=fabs(tar_t[example_t][element_t][j]);
    }
  targ=1;
  if (max>1.0)
    targ=0;
}


void execute_act_test()
{
  set_input_t();
  element_t++;
  if (element_t>length_t[example_t])
    {
      element_t=0;
      example_t++;
      seq_cor=1;
      seq_err=0;
      if (sequ_reset==1)
	reset_net();
      if (example_t>test_size-1)
	{
	  test_end=1;
	}
      set_input_t();
    }
}

void forward_pass()
{
  int i,j;
  double sum;
     
	for (i=in_mod;i<ges_mod;i++)
	{
		sum = 0;
		
		for (j=0;j<ges_mod;j++)
			sum += W_mod[i][j]*Yk_mod_old[j];

		Yk_mod_new[i] = 1/(1+exp(-sum));
	};
}



void comp_err() 
{
	int k,j,maxout;
	double err,max;

	/* MSE */
	for (k=hi_in_mod,j=0;k<ges_mod;k++,j++)
	{
		err=  error[j]*error[j];
	};
	seq_err+=err;
	epoch_err+=err;

	/* Maximal error output */

	max=0;
	for (k=hi_in_mod,j=0;k<ges_mod;k++,j++)
	{
		if (fabs(error[j])>max)
		{
			max=fabs(error[j]);
			maxout=j;
		}
	};

	if ((seq_cor==1)&&(max>seq_max))
	{
		seq_cor=0;
		class_err++;
	}
}

void test()
{
  int i,k,j;
  element_t=0;
  example_t=0;
  epoch_err=0;
  class_err=0;
  seq_cor=1;
  seq_err=0;
  test_end=0;

  while (test_end==0)
    {

      /* executing the environment
	 and setting the input
	 */
      execute_act_test();

      /* forward pass */

      forward_pass();


      if (targ==1) /* only if target for this input */
	{
	  /* compute error */

	  for (k=hi_in_mod,j=0;k<ges_mod;k++,j++)
	    {
	      error[j]=  target_a[j] - Yk_mod_new[k];
	    };

	  /* Training error */

	  comp_err();
	}

      /* set old activations */
      for (i=0;i<ges_mod;i++)
	{
	  Yk_mod_old[i] = Yk_mod_new[i];
        }

    }

  fp1=fopen(outfile, "a");
  fprintf(fp1,"TEST: epochs:%d sequences:%d\n",epoch+1,numb_seq);
  fprintf(fp1,"TEST: MSE:%.4f\n",epoch_err/(1.0*test_size));
  fprintf(fp1,"TEST: misclassifications:%d (out of %d test examples)\n",class_err,test_size);
  fprintf(fp1,"\n");
  fclose(fp1);

  
}

void output_epoch()
{
  fp1=fopen(outfile, "a");
  fprintf(fp1,"epochs:%d sequences:%d\n",epoch+1,numb_seq);
  fprintf(fp1,"MSE:%.4f\n", epoch_err/(1.0*training_size));
  fprintf(fp1,"misclassifications:%d (out of %d training examples)\n",class_err,training_size);
  fprintf(fp1,"\n");
  fclose(fp1);
}

void weight_out()
{
  int i,j;
  fp2 = fopen(weightfile, "w");
  fprintf(fp2,"anz:%d\n",numb_seq);
  for (i=in_mod;i<ges_mod;i++)
    {
      for (j=0;j<ges_mod;j++)
	fprintf(fp2,"(%.2d,%.2d): %.3f ",i,j,W_mod[i][j]);
      fprintf(fp2,"\n");
    };
  fprintf(fp2,"\n");
  fclose(fp2);
}

void set_input()
{
  int i,j,k;
  double max;
	if (bias1==0)
	{
		for (i=0;i<in_mod;i++)
		{
			Yk_mod_new[i]=inp[example][element][i];
			Yk_mod_old[i]=Yk_mod_new[i];
		}
	}
	else
	{
		for (i=0;i<in_mod-1;i++)
		{
			Yk_mod_new[i]=inp[example][element][i];
			Yk_mod_old[i]=Yk_mod_new[i];
		}

		Yk_mod_new[in_mod-1]=1.0;
		Yk_mod_old[in_mod-1]=1.0;
	}

  max=0;
	for (k=hi_in_mod,j=0;k<ges_mod;k++,j++)
	{
		target_a[j]=tar[example][element][j];
		
		if (fabs(tar[example][element][j])>max)
			max=fabs(tar[example][element][j]);
	}
  /* is there a target for this input */
	targ=1;
	if (max>1.0)
		targ=0;
}

void execute_act()
{
  /* setting the net input */
  set_input();
  /* next element in the sequence */
  element++;
  /* sequence end */
  if (element>length[example])
    {
      /* first element in a sequence */
      element=0;
      /* counting the number of sequences in a epoch */
      example++;
      /* counting the number of sequences the net has seen */
      numb_seq++;
      /* resetting the error for the current sequence */
      seq_cor=1;
      /* resetting the error per sequence */
      seq_err=0;
      /* reset after each sequence? */
      if (sequ_reset==1)
	reset_net();
      /* weight update after each sequence? */
      if (w_up==1)
	weight_up=1;
      /* end of an epoch ? */
      if (example>training_size-1)
	{
	  /* MSE and misclassifications output for training set */
	  output_epoch();
	  /* when to stop learning */
	  if (stop_learn==0)
	    {
	      if ((epoch_err<stop_mse)||(class_err<stop_class))
		{
		  stop_learn=1;
		  maxepoch=epoch+ext_epochs;
		}
	    }
	  /* weight update after each epoch? */
	  if (w_up!=1)
	    weight_up=1;
	  /* performing a test on a test  set? */
	  if ((epoch%test_aus==0)||(class_err<min_fehl))
	    {
	      reset_net();
	      test();
	      reset_net();
	      seq_cor=1;
	      seq_err=0;
	    }
	  /* first sequence in the training set */
	  example=0;
	  /* counting the epochs */
	  epoch++;
	  /* resetting the error per epoch */
	  epoch_err=0;
	  /* resetting the misclassifications per epoch */
	  class_err=0;
	  /* Write weight matrix */
	  if (epoch%w_out==0)
	    weight_out();

	}
      set_input();
    }
}

void initia()
{
  int i, j;

  example=0;
  epoch=0;
  epoch_err=0;
  class_err=0;
  numb_seq=0;
  seq_cor=1;
  seq_err=0;
  weight_up=0;
   

  /* weight initialization */
  for (i=in_mod;i<ges_mod;i++)
    {
      for (j=0;j<ges_mod;j++)
	{
	  W_mod[i][j] = (seprand(2000) - 1000);
	  W_mod[i][j] /= 1000.0;
	  W_mod[i][j]*=init_range;
	  DW[i][j]=0;
	};

    };

  /* reset activations and derivatives */

  reset_net();



}






void backward_pass()
{
	int k,i,j,l;
	double sum,kron_y;

	for (i=in_mod;i<ges_mod;i++)
		for (j=0;j<ges_mod;j++)
			for (k=in_mod;k<ges_mod;k++)
			{
				sum = 0;
				for (l=in_mod;l<ges_mod;l++)
					sum += W_mod[k][l]*Pk_ijm_mod_o[l][i][j];
				if (i==k)
					kron_y = Yk_mod_old[j];
				else
					kron_y = 0;

				Pk_ijm_mod[k][i][j] = Yk_mod_new[k]*(1-Yk_mod_new[k])*(sum + kron_y);
			};
  
    
	for (i=in_mod;i<ges_mod;i++)
	{
		for (j=0;j<ges_mod;j++)
		{
			sum = 0;
			for (k=hi_in_mod,l=0;k<ges_mod;k++,l++)
				sum += error[l]*Pk_ijm_mod[k][i][j];
	
			DW[i][j] += alpha*sum;
		};
	};
  
	for (i=in_mod;i<ges_mod;i++)
		for (j=0;j<ges_mod;j++)
			for (k=in_mod;k<ges_mod;k++)
				Pk_ijm_mod_o[k][i][j] = Pk_ijm_mod[k][i][j];
  
  
}

void weight_update()
{
  int i,j;


  for (i=in_mod;i<ges_mod;i++)
    {
      for (j=0;j<ges_mod;j++)
	{
	  W_mod[i][j] += DW[i][j];
	  DW[i][j] = 0;
	};
    }

}


void getpars()
{
  int u,v,corr[50];
  char *wrong1[] = {
    "number inputs: ?",
    "number outputs: ?",
    "number hidden units: ?",
    "biased: ?",
    "learning rate: ?",
    "max. error for correct sequence: ?",
    "half interval length for intialization: ?",
    "performing test after ? epochs: ?",
    "performing test after fewer than ? wrong classifications on training set: ?",
    "write weight after ? epochs: ?",
    "max. number of trials: ?",
    "stop learning once MSE per epoch <: ?",
    "stop learning once wrong classifications per epoch <: ?",
    "epochs to be learned after stop learning is set: ?",
    "initialization of random generator: ?",
    "reset the net after each sequence?: ?",
    "weight update after sequence or epoch?: ?",
    "max. number of epochs: ?",
    "size of training set: ?",
    "size of test set: ?" } ;



  FILE *fp5;

  for (u=0;u<50;u++)
    {
      corr[u]=1;
    }

  v=0;
  fp5=fopen("rtrlpars.txt","r");
  corr[v]=fscanf(fp5,"number inputs: %d\n",&in_mod);
  /* are the maximal ranges correct? */
  if (in_mod>max_inputs)
    {
      printf("Program terminated!\n");
      printf("You have to set the constant max_inputs at begin\n");
      printf("of the program file greater or equal %d and then\n",in_mod);
      printf("compile the program again.\n");
      exit(0);
    }
  v++;
  corr[v]=fscanf(fp5,"number outputs: %d\n",&out_mod);
  /* are the maximal ranges correct? */
  if (out_mod>max_outputs)
    {
      printf("Program terminated!\n");
      printf("You have to set the constant max_outputs at begin\n");
      printf("of the program file greater or equal %d and then\n",out_mod);
      printf("compile the program again.\n");
      exit(0);
    }
  v++;
  corr[v]=fscanf(fp5,"number hidden units: %d\n",&hid_mod);
  v++;
  corr[v]=fscanf(fp5,"biased: %d\n",&bias1);
  v++;
  corr[v]=fscanf(fp5,"learning rate: %lf\n",&alpha);
  v++;
  corr[v]=fscanf(fp5,"max. error for correct sequence: %lf\n",&seq_max);
  v++;
  corr[v]=fscanf(fp5,"half interval length for intialization: %lf\n",&init_range);
  v++;
  corr[v]=fscanf(fp5,"performing test after ? epochs: %d\n",&test_aus);
  v++;
  corr[v]=fscanf(fp5,"performing test after fewer than ? wrong classifications on training set: %d\n",&min_fehl);
  v++;
  corr[v]=fscanf(fp5,"write weight after ? epochs: %d\n",&w_out);
  v++;
  corr[v]=fscanf(fp5,"max. number of trials: %d\n",&maxtrials);
  v++;
  corr[v]=fscanf(fp5,"stop learning once MSE per epoch <: %lf\n",&stop_mse);
  v++;
  corr[v]=fscanf(fp5,"stop learning once wrong classifications per epoch <: %d\n",&stop_class);
  v++;
  corr[v]=fscanf(fp5,"epochs to be learned after stop learning is set: %d\n",&ext_epochs);
  v++;
  corr[v]=fscanf(fp5,"initialization of random generator: %d\n",&ran_sta);
  v++;
  corr[v]=fscanf(fp5,"reset the net after each sequence?: %d\n",&sequ_reset);
  v++;
  corr[v]=fscanf(fp5,"weight update after sequence or epoch?: %d\n",&w_up);
  v++;
  corr[v]=fscanf(fp5,"max. number of epochs: %d\n",&maxepoch);
  v++;
  corr[v]=fscanf(fp5,"size of training set: %d\n",&training_size);
  /* are the maximal ranges correct? */
  if (training_size>max_training_size)
    {
      printf("Program terminated!\n");
      printf("You have to set the constant max_training_size at begin\n");
      printf("of the program file greater or equal %d and then\n",training_size);
      printf("compile the program again.\n");
      exit(0);
    }
  v++;
  corr[v]=fscanf(fp5,"size of test set: %d\n",&test_size);
  /* are the maximal ranges correct? */
  if (test_size>max_test_size)
    {
      printf("Program terminated!\n");
      printf("You have to set the constant max_test_size at begin\n");
      printf("of the program file greater or equal %d and then\n",test_size);
      printf("compile the program again.\n");
      exit(0);
    }
  v++;
  fclose(fp5);

  for (u=0;u<v;u++)
    {
      if (corr[u]==0)
	{
	  printf("Error in lstmpars.txt at line:\n");
	  printf("%s\n",wrong1[u]);
	  exit(0);
	}
    }
}

void write_info()
{

      printf("Perhaps an error occurred during reading the file\n");
      printf("lstmtest.txt ---> stopping the program.\n");
      printf("There should be %d input components and\n",in_mod);
      printf("%d target components per line. Makes %d real\n",out_mod,in_mod+out_mod);
      printf("values per line.\n");
      printf("An input sequence ends with a line\n");
      printf("where the first value (out of %d values in\n",in_mod+out_mod);
      printf("the line) is greater than 10.0\n");
      printf("The file ends with an extra line\n");
      printf("where the first value (out of %d values in\n",in_mod+out_mod);
      printf("the line) is greater than 10.0\n");
}


void getsets()
{
  FILE *fp3,*fp4;
  int end_seq,elm,end,trains,i;


  fp3=fopen("rtrltrain.txt","r");
  end=0;
  trains=0;
  while (end==0)
    {
      end_seq=0;
      elm=0;
      while (end_seq==0)
	{
	  for (i=0;i<in_mod;i++)
	    {
	      fscanf(fp3,"%lf ",&inp[trains][elm][i]);
	    }
	  for (i=0;i<out_mod;i++)
	    {
	      fscanf(fp3,"%lf ",&tar[trains][elm][i]);
	    }
	  fscanf(fp3,"\n");
	  /* sequence ends with first input component greater 10.0 */
	  if (fabs(inp[trains][elm][0])>10.0)
	    {
	      /* if 2 successive sequences have a first input component greater
		 than 10.0, then training set is finished */ 
	      if (elm==0)
		{
		  end=1;
		}
	      else
		{
		  elm--;
		}
	      end_seq=1;
	    }
	  elm++;
	  if (elm>max_sequence_length)
	    {
	      printf("Program terminated!\n");
	      printf("You have to set the constant max_sequence_length at begin\n");
	      printf("of the program file greater or equal maximal sequence\n");
	      printf("length (>= %d) and then compile the program again.\n",elm);
	      exit(0);
	    }
	}
      length[trains]=elm;
      trains++;
      if (end==1)
	trains--;
    }
  fclose(fp3);
  if (trains!=training_size)
    {
      printf("Training set size in parameter file: %d.\n",training_size);
      printf("Training set size detected: %d.\n",trains);
      write_info();
      exit(0);
    }

  fp4=fopen("rtrltest.txt","r");
  end=0;
  trains=0;
  while (end==0)
    {
      end_seq=0;
      elm=0;
      while (end_seq==0)
	{
	  for (i=0;i<in_mod;i++)
	    {
	      fscanf(fp4,"%lf ",&inp_t[trains][elm][i]);
	    }
	  for (i=0;i<out_mod;i++)
	    {
	      fscanf(fp4,"%lf ",&tar_t[trains][elm][i]);
	    }
	  fscanf(fp3,"\n");
	  if (fabs(inp_t[trains][elm][0])>10.0)
	    {
	      if (elm==0)
		{
		  end=1;
		}
	      else
		{
		  elm--;
		}
	      end_seq=1;
	    }
	  elm++;

	  if (elm>max_sequence_length)
	    {
	      printf("Program terminated!\n");
	      printf("You have to set the constant max_sequence_length at begin\n");
	      printf("of the program file greater or equal maximal sequence\n");
	      printf("length (>= %d) and then compile the program again.\n",elm);
	      exit(0);
	    }
	}
      length_t[trains]=elm;
      trains++;
      if (end==1)
	trains--;
    }
  fclose(fp4);
  if (trains!=test_size)
    {
      printf("Test set size in parameter file: %d.\n",test_size);
      printf("Test set size detected: %d.\n",trains);
      write_info();
      exit(0);
    }
}

void main()
{

  int i, j, k,
    trialnr;

    
  /* input pars */

  getpars();

  /* input training set and test set */

  getsets();

  if (maxtrials>20)
    maxtrials=20;

  if (bias1==1)
    in_mod++;
  hi_in_mod = in_mod+hid_mod;
  ges_mod = hi_in_mod+out_mod;

  if (ges_mod>max_units)
    {
      printf("Program terminated!\n");
      printf("You have to set the constant max_units at begin\n");
      printf("of the program file greater or equal %d and then\n",ges_mod);
      printf("compile the program again.\n");
      exit(0);
    }



	srand(ran_sta);
	for (trialnr=0;trialnr<maxtrials;trialnr++)
    {

		outfile = outf[trialnr];

		weightfile = weig[trialnr];



		fp1 = fopen(outfile, "w");
		fprintf(fp1,"Trial Nr.:%.1d\n",trialnr);
		fclose(fp1);

		fp2 = fopen(weightfile, "w");
		fprintf(fp2,"Trial Nr.:%.1d\n",trialnr);
		fclose(fp2);


		initia();

		examples=0;
		epoch=0;



		stop_learn=0;
		learn = 1;

		while (learn == 1)
		{
			/* executing the environment
				and setting the input
				*/
			execute_act();

			/* forward pass */

			forward_pass();


			if (targ==1) /* only if target for this input */
			{
				/* compute error */

				for (k=hi_in_mod,j=0;k<ges_mod;k++,j++)
				{
					error[j]=  target_a[j] - Yk_mod_new[k];
				};

				/* Training error */
				comp_err();
			}

			/* backward pass */
			backward_pass();

			/* set old activations */
			for (i=0;i<ges_mod;i++)
			{
				Yk_mod_old[i] = Yk_mod_new[i];
			}
			
			/* update weights */

			if (weight_up==1)
			{
				weight_up=0;
				weight_update();
			}

			/* stop if maxepoch reached */
			if (epoch>maxepoch)
			learn=0;

		}

		weight_out();
		test();
    }

  exit(0);
}










