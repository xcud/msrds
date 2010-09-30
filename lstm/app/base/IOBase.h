#ifndef IOBASE_H
#define IOBASE_H

//IOBase.h
#include <fstream>
#include <vector>
#include <string>

using namespace std;

/*** defines ***/
// switch for all debugging info
//#define DEBUG


// classes in this header 
class TIOBase; 


///////////////////TTIOBase
// a container class for trival common variables and functions.
// Handle the fileIO.
class TIOBase
{
public:
  // We leave the standart constructor empty, and do all the initializations
  // in the other one. The empty one is necessairy for TTopoCreator and
  // TPatternManager, becase their constructors are empty too. But as thier
  // classes exist only virtually for TRNN, the empty constructor of 
  // TIOBase will never be called. So no one will realize that it doesn't
  // do its work properly.
  TIOBase();
  ~TIOBase();
  // Functions for C code including.
  static vector <string> &getFileNames(char *wildcard, 
				       const char *path, 
				       vector <string> & files, 
				       bool recursive);
  char ReadComment();
  char ReadComment(iostream *apF);
  // Depricated.
  // char ReadComment( fstream *apF );
  //
  int Keypressed_termIO(char &ac);
  void KeyCheck();
  void KeyCheck_cin();
  void KeyCheck_termIO();
  // Last (and current)  char read by keypressed.
  char cKey;

  // The following functions take the current time,
  // they do not change static timeStamp variable.
  //
  // Stores current time in parameter s.
  void getDateAndTime( char * s );
  // Add current time to string s.
  void addDateAndTime( char * s );
  void writeDateAndTimeComment( ofstream &of );
  //
  // The following functions work on static timeStamp variable.
  //
  void setTimeStampToCurrentTime() { getDateAndTime( timeStamp ); }
  // Does not change static timeStamp variable.
  void addTimeStamp( char * s ) { strcat( s, timeStamp); } 
  void getTimeStamp( char * s ) { strcpy( s, timeStamp); } 

  // Returns file size in bytes.
  static long getFileSize( const char * filename );
  static long getFileSize( FILE * file );

  // Linecount functions return nb of lines in file.
  // It count lines (and bytes) of a file,
  // if the doea not end with a newline it returns 
  // nb of line + 1.
  //
  // If FILE * is NULL, file with filename is opened.
  static long lineCount( const char * filename,
			 FILE * file,
			 const int bufferSize  = 8388608 ); //8M
  // Alloc read buf in calling function.
  static long lineCount( const char * filename,
			 FILE * file,
			 char * buf,
			 const int bufferSize );

  // Count WS separated wordscols in line 
  // after pos of file pointer (no rewind, line is consumed).
  static int countWordsInLine( FILE * file );

protected:
  // possible Mode parameters are defined in clsss ios (ios::out,in,...)
  // The childs are responsible closing and deleting the file again. 
  char OpenFile(char* acFilename, int aiMode);
  char OpenFile(fstream *apF, char* acFilename, int aiMode);
  char CloseFile();
  char CloseFile(fstream *apF);
  bool DirContainsFile( bool SubSearch, char *fn );
  // Double to stream and the other way round.
  static void d2s(iostream *s, double &d) { *s << d; }
  static void s2d(iostream *s, double &d) { *s >> d; }
  // For long doubles.
  static void ld2s(iostream *s, long double &d) {
    double dd; dd=(double)d; *s << dd; } 
  static void s2ld(iostream *s, long double &d) {
    double dd; *s >> dd; d=dd; } 
  // to rescan the file begining after the global section
  long lFilePos; 
private:
  // TO keep only one fstream object alive and open 
  //- close the files instead of new - 
  // delete an fstream object every time we read a file.
  fstream *pFile; // common stream pointer for all friend-childs
  char cFilename[256];
  int iFileRdState; // the status of the file.

  // Time stamp to add to all filenames that are saved.
  // This may be the starting time of the process.
  static char timeStamp[256];

};

#endif // IOBASE_H
 
