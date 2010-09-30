#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "IOBase.h"
#include "scanDataFormat.h"

using namespace std;

ScanDataFormat::ScanDataFormat()
  : scanDataLine( NULL )
{ }


char * ScanDataFormat::allocIOBuf
( FILE * file, long & bufferSize, const long maxSize )
{
  // Get length of file buf in bytes.
  // We need one more char for string end if all file should
  // fit into buffer.
  bufferSize = TIOBase::getFileSize( file ) + 1;
  if( maxSize && (bufferSize > maxSize) )
    {
      bufferSize = maxSize ;
    }    
  printf("Input buffersize: %ld byte\n", bufferSize ); //ddd

  // Alloc buffer.
  // Add space in buf for one more \n in case it is 
  // missing in the data file.
  // We do not need the \0 at the end of the string buffer,
  // It may also end with \n for the scanf loop below.

  return new char[bufferSize];
}


int ScanDataFormat::fillBuffer
( FILE * file, char * buf, 
  const long & bufferSize, 
  long & validBytes )
{

  assert( buf && bufferSize > 2 );
  
  // Save one char for the end of STRING char '\0'
  long bytesRead = fread( buf, 1, bufferSize - 1, file);

  if ( ferror( file ) ) 
    { 
      fprintf( stderr, "ERROR reading  file into buffer\n");
      exit( -1 ); 
    }

  int status = 0; // Default: more to come from file.

  // Check if we reached eof.
  if( feof( file ) )
    { 
      status = 1;
    }
  else if ( bytesRead != bufferSize - 1 )  
    { 
      // not eof, thus buffer should be full.
      fprintf( stderr, "ERROR not reading file into buffer\n");
      exit( -1 ); 
    }
  else
    { 
      // Check if we will reach eof with the next read.
      if( EOF == getc( file ) ) { status = 1; }
      fseek( file, -1, SEEK_CUR );
    }
  
  if( status == 1)
    {
      // set end-string char
      buf[ bytesRead ] = '\0';
      // Include \0
      validBytes = bytesRead + 1;
      return status;
    }

  // Init p to the last char read.
  register char * p = buf + bytesRead - 1;

  // Set p to the last endline \n char
  // and correct file pointer accordingly. 
  //
  // p = (char *) memrchr( buf , '\n' , bytesRead ); 
  // The same handmade, because it is a GNU extension.
  // Set p to end of buffer.
  for( ; p >= buf; p-- )
    {
      if( *p == '\n' ) { break; }
    }

  if( p < buf ) 
    { 
      //       p = NULL; 
      //     } // No newline found.
      //    if( ! p )
      //     {
      fprintf( stderr, "ERROR line too long for IO buffer\n");
      exit( -1 ); 
    }
  assert( p >= buf );

  // validBytes without last newline
  validBytes = p - buf;

  assert( validBytes <= bytesRead);
  // set the filepointer back AFTER last \n
  fseek( file,  - ( bytesRead - validBytes - 1), SEEK_CUR);
  buf[ ++ validBytes ] = '\0'; // set end-string char

#ifdef __WIN32__
  // cariage-return \c  and line-feed \n are
  // one char when reading a file under dos. 
  // The filepointer will
  // not be at the correct position, if it
  // ends at \c, it will probably set to the next 
  // char and by reverting buffer with fseek one 
  // char will be lost.
  char tmpBuffer[1];
  fseek( file, -2 , SEEK_CUR);
  fread( tmpBuffer, 1, 1, file);  
#endif

//   debug:
//   //   fseek( file, -3 , SEEK_CUR);
//   printf( "\nfileBuf: -%d-%d-%d-%d-\n", 
// 	  debugBuffer[0], 
// 	  debugBuffer[1], 
// 	  debugBuffer[2], 
// 	  debugBuffer[3]);//ddd
//   printf( "\nendBuf: %s \n", buf+validBytes-20 );//ddd
//   printf( "endallBuf: %s \n", buf+bytesRead-4 );//ddd
//   printf( "endallBuf:-%d-%d-%d-\n", *(buf+bytesRead-2), 
// 	  *(buf+bytesRead-1), 
// 	  *(buf+bytesRead) 
// 	  );//ddd
//   printf( "endallBuf:+%c+%c+%c+\n", *(buf+bytesRead-2), 
// 	  *(buf+bytesRead-1), 
// 	  *(buf+bytesRead) 
// 	  );//ddd
//   printf( "validBytes: %d \n", validBytes );//ddd
//   printf( "bytesRead: %d\n", bytesRead );//ddd
  
  return status;
  }


int ScanDataFormat::bufferedReadDataLineByLine
( long expectedNbLine, FILE * file)
{ 
  // printf("ex %ld file %p\n",expectedNbLine, file); ddd
  assert( expectedNbLine && file );
  assert( scanDataLine );

  long bufferSize;
  // Nb lines in data file must match nb. datapoints.
  char * buf = allocIOBuf( file, bufferSize,
			   KTB_MAX_FILE_BUFFER  );

  printf("Buffered read data points " ); fflush( stdout );
  
  // Report some progress.
  register int progressCount = 0;  
  const long progressCountStep = expectedNbLine / 10;

  // Read buffered.
  register char * pNewLine;
  register char * p;
  register char * pMax;
  register long curBufSize;
  //
  long curLine = 0;
  bool eofReached = false;
  // loop over buffer read

  while( ! eofReached )
    {
      // Fill buffer and
      // check if all of file read.
      if( fillBuffer( file, buf, bufferSize, curBufSize ) ) 
	{ 
	  eofReached = true; 
	}

      // loop over datapoints till buffer is empty.
      p = buf;
      pMax = buf + curBufSize - 1 ; // last valid char.

      while( p < pMax ) // Last char is \0 or \n
	{
	  pNewLine = 
	    (char*) memchr( p, '\n', curBufSize - (p - buf) );

	  if( ! pNewLine ) // no more new lines.
	    {
	      pNewLine = pMax;
	    }

	  // Pass only one line to scanFunc (marc line end).
	  *pNewLine = '\0';

	  // Call the func pointer to scan current data line.
	  (this->*scanDataLine)( p, curLine );
	  curLine++; 

	  // Jump to next line.     
	  p = pNewLine + 1;
	  
 	  // Report some progress.
 	  if( ++progressCount == progressCountStep ) 
	    { 
 	      printf("."); 
 	      fflush( stdout );
 	      progressCount = 0;	      
 	    }
	}
    }

  assert( curLine == expectedNbLine );

  // Delete file buffer.
  delete[] buf; 

  printf(" done.\n"); fflush( stdout );

  return 0;
}
