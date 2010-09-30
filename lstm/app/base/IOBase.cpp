//IOBase.cpp
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <string.h>
#include <sstream>
//#include <dirent.h>
//#include <unistd.h> // e.g. sleep 
//#include <termios.h> // struct termios
//#include <fcntl.h>
//#include <sys/param.h>
#include <assert.h>

#include "IOBase.h"

///////////////////TIOBase

char TIOBase::timeStamp[256] = ""; 

TIOBase::TIOBase() 
  : cKey( 0 )
{
  pFile = new fstream();
}

TIOBase::~TIOBase() {
  delete pFile; // callls fstream::close
}

char TIOBase::OpenFile(char* acFilename, int aiMode) {
  // The calling function should  take care of closing  pFile, e.g
  // with CloseFile(). But we check if it is open.
#ifdef DEBUG
  cout << " using pFile to open: " << acFilename
       << " rdstate: " << pFile->rdstate() << endl;
#endif
  if(!pFile) 
    { 
      cerr << "pFile does not exist!" << endl; return 1; 
    } 
  else 
    {
      fprintf( stdout, "Open file %s done.\n", acFilename ); //ddd
      // keep the actual filename for messages
      strcpy(cFilename, acFilename); 
      return OpenFile(pFile,acFilename,aiMode);
    }
}

char TIOBase::OpenFile(fstream *apF, char* acFilename, int aiMode) {
  // The calling function should  take care of closing  apF, e.g
  // with CloseFile(). But we check if it is open.
  // Mode, e.g., ios::in, ios::app, ios::out .
#ifdef DEBUG
  cout << " opening... " << acFilename
       << " rdstate: " << apF->rdstate() << endl;
#endif
  // Clear the file descriptor first.
  apF->clear();
  if(!apF) cerr << "apF does not exist!" << endl;
  //apF->tellg(); // try to access it and wait for error.
#ifdef  DEBUG
  cout << " Check " << acFilename << endl;
#endif
  if(apF->bad()) {
    cerr << "Warning: File bad (old file, maybe): " 
	 << cFilename << " (new file): " << acFilename<<"\n";
    // So we clear and close it.
    apF->clear(); apF->close();
  }
  // open the file. One can add an other parameter for the file-rights
  apF->open(acFilename, aiMode);
  //apF->open(acFilename, aiMode);
  if(!apF->good()) {
    cerr << " File not good : " << acFilename << " !\n";
    apF->clear(); apF->close();
    return 1;
  }
#ifdef DEBUG
  cout << acFilename << " open.\n";
#endif
  return 0;
}

char TIOBase::CloseFile() {
#ifdef DEBUG
  cout << " closing  " << cFilename
       << " rdstate: " << pFile->rdstate() << endl;
#endif
  if(pFile) { 
    strcpy(cFilename,"");
    return CloseFile(pFile); 
  } else { cerr<<"CloseFile: pFile does not exsist"<<endl; return 1; }
}

char TIOBase::CloseFile(fstream *apF) {
  // check file. We use the ios statefalgs.
#ifdef DEBUG
  cout << " closing apF " 
       << " rdstate: " << apF->rdstate() << endl;
#endif
  if(!apF) { cerr<<"Error in CloseFile (apF==NULL)."<<endl; exit(1); }
  iFileRdState = apF->rdstate();
  // Also a EOF should not happen, it indicates that a file is not complete.
  if(!apF->good()) { // && !apF->eof()) { // !apF->fail
    cerr << "File error before closing in " << "\n"; 
    iFileRdState = 1;
  }
  // Clear in case of error and close file
  apF->clear(); apF->close(); 
#ifdef DEBUG
  cout << "\n" << " closed regularly.\n"; 
#endif
  if(iFileRdState) return 1; else return 0;
}

char TIOBase::ReadComment() {
  if(pFile) { return ReadComment(pFile);
  } else { 
    cerr << "Error in ReadComment(). No File " 
	 << cFilename << " to read comment from.\n";
    return 1; }
}


char TIOBase::ReadComment( iostream * apF ) 
{
  // skip comment (normally at the beginning of a file)
  char c;
  if(!apF) {
    cerr << "No File " << cFilename << " to read comment from.\n";
    return 1; }
  *apF >> c;
  while((c == '#' || c == ' ' || c == '\n') && apF->good()) {
#ifdef DEBUG
    cout << c;
#endif    
    if(c == '#')
      if(c == '#') apF->ignore(1000, '\n');
    *apF >> c;
  }
  apF->putback(c);
  if(apF->eof()) { 
    cerr << "EOF while reading comment from " << cFilename << "\n"; 
    return 1; 
  }
  if(apF->fail()) { 
    cerr << "FAIL while reading comment from " << cFilename << "\n"; 
    return 1; 
  }
#ifdef DEBUG 
  cout << "\nComment read.\n";
#endif
  return 0;
}


// char TIOBase::ReadComment( fstream * apF ) 
// {
//   // skip comment (normally at the beginning of a file)
//   char c;
//   if(!apF) {
//     cerr << "No File " << cFilename << " to read comment from.\n";
//     return 1; }
//   *apF >> c;
//   while((c == '#' || c == ' ' || c == '\n') && apF->good()) {
// #ifdef DEBUG
//     cout << c;
// #endif    
//     if(c == '#')
//       if(c == '#') apF->ignore(1000, '\n');
//     *apF >> c;
//   }
//   apF->putback(c);
//   if(apF->eof()) { 
//     cerr << "EOF while reading comment from " << cFilename << "\n"; 
//     return 1; 
//   }
//   if(apF->fail()) { 
//     cerr << "FAIL while reading comment from " << cFilename << "\n"; 
//     return 1; 
//   }
// #ifdef DEBUG 
//   cout << "\nComment read.\n";
// #endif
//   return 0;
// }



bool TIOBase::DirContainsFile(bool SubSearch, char *fn) 
{
 /* DIR *dir;
  struct dirent *dr;
  dir = opendir(".");
  while((dr=readdir(dir))) {
    if(SubSearch) {
      if(strstr(dr->d_name, fn)) { closedir(dir); return 1; }
    } else
      if(!strcmp(fn, dr->d_name)) { closedir(dir); return 1; }
  }
  closedir(dir);*/
  return 0;
}

vector <string> &TIOBase::getFileNames(char *wildcard, 
				       const char *path, 
				       vector <string> &files,
				       bool recursive) {
//  char tmpPath[1024]; 
//  char curPath[1024]; 
//  strcpy(curPath, path);
//
//#ifdef _DIRENT_HAVE_D_TYPE
//      stringstream subPathBuf; 
//      char *subPath;
//#endif
//
//  struct dirent *dr; // An entry in the dir (filename).
//  DIR *dir = opendir(curPath);
//  // CurPath name should end with a /.
//  if(curPath[strlen(curPath)-1]!='/') strcat(curPath,"/");
//  //for(int i=0;i<strlen(curPath);i++) 
//  //{ cout << i<< ":::" << curPath[i] << endl; }
//  
//  // Read dir content.
//  while((dr=readdir(dir))) 
//    { 
//      // Read subdirs.
//#ifdef _DIRENT_HAVE_D_TYPE
//      if ( (dr->d_type==DT_DIR) && recursive ) 
//	{ 
//	//cout << "Recursive-> "<< dr->d_name <<" : " << endl;
//	if(!strcmp(dr->d_name,".") || !strcmp(dr->d_name,"..")) 
//	  {
//	    //cout << " _./,../_ " << endl; 
//	  } 
//	else 
//	  {
//	    subPathBuf.clear(); 
//	    subPathBuf.seekg(0); 
//	    subPathBuf.seekp(0);
//	    subPathBuf << curPath << dr->d_name << "/" << ends;
//	    subPathBuf << dr->d_name << ends;
//	    //cout << subPathBuf.str() << endl;
//	    subPath =  (char *)(subPathBuf.str()).c_str();
//	    getFileNames(wildcard, (const char *) 
//			 subPath, files, recursive);
//	    //delete subPath; // should be but causes a segfault.
//	  }
//      } 
//    else if(strstr(dr->d_name, wildcard)) 
//#endif
//      { // Select Files.
//	//cout << curPath << dr->d_name 
//	//<< " type:" << int(dr->d_type) <<" "<< endl;
//	strcpy(tmpPath,curPath); strcat(tmpPath,dr->d_name); 
//	files.push_back(tmpPath);
//      }
//    }
//  //  for(int i=0;i<files.size();i++) cout<<files[i]<<endl;
//  closedir(dir);
  return files;
}


int TIOBase::Keypressed_termIO(char &ac) {
  //struct termios term, termsave;
  //// We are not always in noncanonical mode without local echo, if
  //// a key is pressed "outside" this function (which is likely)
  //// it will be echoed.
  //// tty_noncanonical(STDIN_FILENO);
  //if (tcgetattr(STDIN_FILENO, &termsave) < 0) 
  //  cerr << "get error" << endl;
  //term = termsave;
  //term.c_lflag &= ~(ICANON | ECHO ); // noncanonical and no echo
  //term.c_cc[VMIN] = 0; // buffer size
  //term.c_cc[VTIME] = 0;
  //if (tcsetattr(STDIN_FILENO, TCSANOW, &term) < 0) 
  //  cerr << "set error" << endl;  
  //// Check for keypressed
  //int iReturnValue = read(STDIN_FILENO, &ac, 1);
  //// // tty_reset(STDIN_FILENO);
  //if (tcsetattr(STDIN_FILENO, TCSANOW, &termsave) < 0) 
  //  cerr << "set error" << endl;
  //// Done. Return nuber of chars read.
  //return iReturnValue;
	return 0;
}

void TIOBase::KeyCheck() {  KeyCheck_termIO(); }

void TIOBase::KeyCheck_cin() {
  while( true ) {
    cin.unsetf(ios::skipws); // read also white space
    cin.clear(); 
    if( !cin.eof() ) { cin >> cKey; return; } // step.
    if( (cKey == 'c') || (cKey =='C') ) return; // cont.
  }
}

void TIOBase::KeyCheck_termIO() {
  if((cKey != 'c') && (cKey != 'C')) {// step
    // while key down (long button press)
    while(Keypressed_termIO(cKey)); 
    // while key up
    while(!Keypressed_termIO(cKey));
  } else Keypressed_termIO(cKey);
}

void TIOBase::getDateAndTime( char * s ) {
  time_t t_now = time( NULL );
  tm *t = localtime( &t_now );
  sprintf( s, ".%d-%.2d-%.2d_%.2d_%.2d_%.2d", 
	   t->tm_year+1900, t->tm_mon+1, t->tm_mday,
	   t->tm_hour, t->tm_min, t->tm_sec );
  //printf("\n date and time: %s\n", timeStr );//ddd
}

void TIOBase::addDateAndTime( char *s ) {
  char dateAndTime[32];
  getDateAndTime( dateAndTime );
  sprintf( s, "%s.%s", s, dateAndTime);
}

void TIOBase::writeDateAndTimeComment( ofstream &of ) 
{
  char s[32]; getDateAndTime( s );
  of<<"#\n# Date and Time (yyyymmdd-hh_mm_ss) : "<<s<<"\n#\n";
}

long TIOBase::getFileSize( const char * filename )
{
  assert( filename );
  FILE * file = fopen( filename, "r" );
  if( ! file ) 
    { 
      printf("File open error: %s\n", filename ); 
      exit( -1 ); 
    }

  long size = getFileSize( file );
  
  fclose( file );

  return size;
}


long TIOBase::getFileSize( FILE * file )
{
  assert( file );

  if( fseek (file, 0L, SEEK_END) ) { exit( -1 ); }
  long size = ftell( file );
  if( fseek (file, 0L, SEEK_SET) ) { exit( -1 ); }
  
  //printf("bufferSize: %ld\n", size ); 

  return size;
}


long TIOBase::lineCount( const char * filename,
			 FILE * file,
			 const int bufferSize )
{
  char * buf = new char[bufferSize];
  if( ! buf ) { printf("Mem error.\n"); exit( -1 ); }

  long lines = lineCount( filename, file, buf, bufferSize );

  delete[] buf;

  return lines;
}
 
 
long TIOBase::lineCount( const char * filename,
			 FILE * file,
			 char * buf, 
			 const int bufferSize )
{
  assert( buf && bufferSize );
 
  assert( file || filename );
  assert( ! (file && filename) );

  long bytes_read;
  long lines = 0;
  //long bytes = 0;

  if( filename ) { file = fopen( filename, "r" ); }
  if( ! file ) 
    { 
      printf("File open error: %s\n", filename ); 
      exit( -1 ); 
    }
  else
    {
      rewind( file );
    }

  //char * buf = new char[bufferSize];
  //if( ! buf ) { printf("Mem error.\n"); exit( -1 ); }

  // Remember last char of current buffer block
  // to check if file ended on a newline.
  register char lc = 0;

  while ( ( bytes_read = fread ( buf, 1, bufferSize, file ) ) > 0)
    {
      // Remember last char of current buffer.
      lc = buf[ bytes_read - 1 ];

      register char *p = buf;
	  
      while ( ( p = (char *)
		// memchr would not count \0 at the end
		// so we would get a line less if file does
		// not end on newline.
		// strchr finds also \0 and is slower.
		memchr ( p, '\n', (buf + bytes_read) - p) ) )
	{
	  ++p;
	  ++lines;
	}
      //bytes += bytes_read;
    }

  if (bytes_read < 0)
    {
      printf( "Error in %s\n", filename ); 
      exit( -1 );
    }

  // Check if file ended on a newline
  // and add one line if not.
  if( lc != '\n' ) 
    { 
      // Assume that last data line is missing a newline.
      fprintf( stderr, 
	       "WARNING: newline missing in last data line\n");

      lines ++; 
    }
  
  //delete buf;
  
  // Check if file opened in this function.
  if( filename ) 
    { 
      fclose( file ); 
    }
  else
    {
      rewind( file );
    }

  //printf( "bytes: %d  \t lines: %d\n", bytes, lines );

  return lines;
}


int TIOBase::countWordsInLine( FILE * file )
{
  int words;
  char c = 0;
  bool inSeparation = true; // skip WS at begining of line.
  for( words = 0; c != '\n'; )
    {
      c = fgetc( file );
      if( ! isspace( c ) && inSeparation ) 
	{ 
	  words++;
	  inSeparation = false;  
	}
      if( isspace( c ) )
	{ 
	  inSeparation = true;  
	}
    }
  printf( "Count words: %d\n", words );//ddd  

  return words;
}

