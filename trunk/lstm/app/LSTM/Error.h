#ifndef KTB_ERROR_H
#define KTB_ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>

#include <iostream>

// define common error treatment for memory-full, matherr, etc.
//
///////////////////Global memory error management
// Does not work for LINUX.
// This function will be called by new in case of a memory error.
// new_handler has to be set to this: 
// set_new_handler(MemoryFull); // __new_handler = &MemoryFull;
void MemoryFull(void) {cerr << "Error. Memory FULL!\n"; exit(1);}
/*
// Math error
int matherr(struct exception *x) {
  cerr << "exception.type: " << x->type << endl
       << "exception.name: " << x->name << endl
       << "exception.arg1: " << x->arg1 << endl
       << "exception.arg2: " << x->arg2 << endl;
  cerr << " Matherror ..and exit!" << endl;
  exit(1);
}
*/

#endif // KTB_ERROR_H
