//---------------------------------------------------------------------------
// created:  20041013 by N.V.Shokhirev
// modified: 20041117 by N.V.Shokhirev
//---------------------------------------------------------------------------

#pragma hdrstop

#include "uIO.h"

#include <assert.h>
//---------------------------------------------------------------------------

#pragma package(smart_init)

void writevalue(ofstream& stream, char* name, int value)
{
  stream << name << "=" << value << endl;
};

void writevalue(ofstream& stream, char* name, double value)
{
  stream << name << "=" << value << endl;
};

void writevalue(ofstream& stream, char* name, char* value)
{
  stream << name << "=" << value << endl;
};

void writeFArr1D(ofstream& stream, char* name, FArr1D& v)
{
  stream << name << "=" << endl;
  stream << "Lo1=" << v.L1() << endl;
  stream << "Hi1=" << v.H1() << endl;
  for(int i=v.L1(); i<=v.H1(); i++) stream << " " << v(i);
  stream << endl;
};

void readint(ifstream& stream, int& v)
{
  char s[64];

  stream.getline(s, 64,'=');
  stream >> v;
};

void readFArr1D(ifstream& stream, FArr1D& v)
{
  int Lo1, Hi1;
  char s[4];
  readint(stream, Lo1);
  readint(stream, Hi1);
  assert( v.L1() == Lo1 && v.H1() == Hi1 );  // Check lim match.
  for(int i=Lo1; i<=Hi1; i++) stream >> v(i);
  stream.getline(s, 4);
//  stream.ignore(80, '\n');
};

void writeFArr2D(ofstream& stream, char* name, FArr2D& v)
{
  stream << name << "=" << endl;
  stream << "Lo1=" << v.L1() << endl;
  stream << "Hi1=" << v.H1() << endl;
  stream << "Lo2=" << v.L2() << endl;
  stream << "Hi2=" << v.H2() << endl;
  for(int i1=v.L1(); i1<=v.H1(); i1++)
  {
    for(int i2=v.L2(); i2<=v.H2(); i2++) stream << " " << v(i1,i2);
    stream << endl;
  };
};

void readFArr2D(ifstream& stream, FArr2D& v)
{
  int Lo1, Hi1, Lo2, Hi2;
  char s[4];

  readint(stream, Lo1);
  readint(stream, Hi1);
  readint(stream, Lo2);
  readint(stream, Hi2);
  assert( v.L1() == Lo1 && v.H1() == Hi1 && v.L2() == Lo2 && v.H2() == Hi2 );
  for(int i1 = Lo1; i1 <= Hi1; i1++)
  {
    for(int i2 = Lo2; i2 <= Hi2; i2++) stream >> v(i1,i2);
    if (i1 == Hi1) stream.getline(s, 4);
//    if (i1 == Hi1) stream.ignore(80, '\n');
  };
};

void writeDatetime(ofstream& stream, struct tm *tblock )
{
  stream << " " << tblock->tm_sec;
  stream << " " << tblock->tm_min;
  stream << " " << tblock->tm_hour;
  stream << " " << tblock->tm_mday;
  stream << " " << tblock->tm_mon;
  stream << " " << tblock->tm_year;
  stream << " " << tblock->tm_wday;
  stream << " " << tblock->tm_yday;
  stream << " " << tblock->tm_isdst << endl;
};

//void readDatetime(ifstream& stream, struct tm &tblock )
void readDatetime(ifstream& stream, struct tm *tblock )
{
  char s[4];

  stream >> tblock->tm_sec;
  stream >> tblock->tm_min;
  stream >> tblock->tm_hour;
  stream >> tblock->tm_mday;
  stream >> tblock->tm_mon;
  stream >> tblock->tm_year;
  stream >> tblock->tm_wday;
  stream >> tblock->tm_yday;
  stream >> tblock->tm_isdst;
  stream.getline(s, 4);
};

