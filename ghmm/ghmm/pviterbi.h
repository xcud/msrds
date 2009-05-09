/*******************************************************************************
*
*       This file is part of the General Hidden Markov Model Library,
*       GHMM version __VERSION__, see http://ghmm.org
*
*       Filename: ghmm/ghmm/viterbi.h
*       Authors:  Bernhard Knab, Benjamin Georgi
*
*       Copyright (C) 1998-2004 Alexander Schliep 
*       Copyright (C) 1998-2001 ZAIK/ZPR, Universitaet zu Koeln
*	Copyright (C) 2002-2004 Max-Planck-Institut fuer Molekulare Genetik, 
*                               Berlin
*                                   
*       Contact: schliep@ghmm.org             
*
*       This library is free software; you can redistribute it and/or
*       modify it under the terms of the GNU Library General Public
*       License as published by the Free Software Foundation; either
*       version 2 of the License, or (at your option) any later version.
*
*       This library is distributed in the hope that it will be useful,
*       but WITHOUT ANY WARRANTY; without even the implied warranty of
*       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*       Library General Public License for more details.
*
*       You should have received a copy of the GNU Library General Public
*       License along with this library; if not, write to the Free
*       Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
*
*       This file is version $Revision: 1713 $ 
*                       from $Date: 2006-10-16 23:06:28 +0900 (월, 16 10 2006) $
*             last change by $Author: grunau $.
*
*******************************************************************************/

#ifndef GHMM_PVITERBI_H
#define GHMM_PVITERBI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pmodel.h"
#include "psequence.h"


/**@name Viterbi-Algorithmus */
/*@{ (Doc++-Group: viterbi) */

/**
  Viterbi algorithm. Calculates the Viterbi path (the optimal path trough
  the model) and the Viterbi probability to a given model and a given 
  sequence. The matrices in the local_store struct are allocated using
  stat_matrix_d_alloc.
  @return Viterbi path
  @param mo:    model
  @param o:     sequence
  @param len:   length of the sequence
  @param log_p: probability of the sequence in the Viterbi path
  */
int *ghmm_dpmodel_viterbi(ghmm_dpmodel *mo, ghmm_dpseq * X, ghmm_dpseq * Y, double *log_p,
		     int *path_length);

int *ghmm_dpmodel_viterbi_variable_tb(ghmm_dpmodel *mo, ghmm_dpseq * X, ghmm_dpseq * Y,
				 double *log_p, int *path_length,
				 int start_traceback_with);

int *ghmm_dpmodel_viterbi_test(ghmm_dpmodel *mo, ghmm_dpseq * X, ghmm_dpseq * Y,
			  double *log_p, int *path_length);

/**
  Calculates the logarithmic probability to a given path through the 
  states (does not have to be the Viterbi path), given sequence and
  a model.
  @param mo:        model
  @param o:         sequence
  @param len:       length of the sequence
  @param state_seq: path through the states
  @return log P
  */

double ghmm_dpmodel_viterbi_logp(ghmm_dpmodel *mo, ghmm_dpseq * X, ghmm_dpseq * Y,
			    int *state_seq, int state_seq_len);


#ifdef __cplusplus
}
#endif


#endif

/*@} (Doc++-Group: viterbi) */
