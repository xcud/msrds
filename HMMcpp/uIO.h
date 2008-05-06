//---------------------------------------------------------------------------
// created:  20041013 by N.V.Shokhirev
// modified: 20041117 by N.V.Shokhirev
//---------------------------------------------------------------------------

//#include <stdlib.h>
#ifndef uIOH
#define uIOH

#include <iostream>
#include <fstream>
#include "dynarrays.h"
#include <time.h>

//---------------------------------------------------------------------------
using namespace std;

void writevalue(ofstream& stream, char* name, int value);

void writevalue(ofstream& stream, char* name, double value);

void writevalue(ofstream& stream, char* name, char* value);

void readint(ifstream& stream, int& v);

void writeFArr1D(ofstream& stream, char* name, FArr1D& v);

void readFArr1D(ifstream& stream, FArr1D& v);

void writeFArr2D(ofstream& stream, char* name, FArr2D& v);

void readFArr2D(ifstream& stream, FArr2D& v);

void writeDatetime(ofstream& stream, struct tm *tblock );

void readDatetime(ifstream& stream, struct tm *tblock );
//void readDatetime(ifstream& stream, struct tm &tblock );

#endif
