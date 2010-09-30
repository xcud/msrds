#ifndef SCANDATAFORMAT_H
#define SCANDATAFORMAT_H

// Buffer size for reading data.
// x MB, e.g.,  16777216 = 2^24 = 16 MB
static const long KTB_MAX_FILE_BUFFER  = 16777216; 

class ScanDataFormat;

// Class to define function pointer to function that
// scans data file, e.g., with a sscanf().
// This function is inherited by IOBase and 
// any data reading class that uses IOBase for buffered read.
// It must define a scan function and set a pointer to it
// in the common base class ScanDataFormat.
// It is than called to from TIOBase::bufferedReadData().
//
typedef void (ScanDataFormat::*ScanDataLine)
  ( char * lineStart, const long & curLine );
//
class ScanDataFormat
{
 public:

  ScanDataFormat();

  //! read to buffer from file up to last endline or endof. 
  //! Set the filepointer to the char 
  //! after the last endline or at endoffile.
  //! return nb of bytes in buffer before \0 (valid bytes)(strlen) 
  //! return -1 on error.
  // validBytes contains the bytes until the last newline
  // in buf, at pos validBytes a \0 is set in buffer.
  // Calling function must open the file 
  // and alloc mem for buf (and, of cause call fclose and free).
  //
  // IF whole file should fit into buffer, the buffer must be
  // of filesize + 1 for the termination \0, which is written 
  // as finalizing char into the buffer to use it later in sscanf.
  static int fillBuffer
    ( FILE * file, char * buf, 
      const long & bufferSize, 
      long & validBytes );

  // Read data from file.
  // The file format is defined by the function that is
  // given as a function pointer (and does some sscanf).
  // Para expectedNbLine is only for the progess count.
  // Callback to scanDataLine must be set before calling this func.
  int bufferedReadDataLineByLine
    ( long expectedNbLine, FILE * file );
  //! \todo int bufferedReadDataBlockByBlock

 protected:

  // The following pointer can be called like: (this->*fp)(); }
  ScanDataLine scanDataLine; 

 private:

  // Returns a pointer to an IO buf, 
  // which must be deleted by caller.
  // If maxSize = 0 , buf will be of file size (+1, see def.).
  // file pointer is used determine file size.
  static char * allocIOBuf
    ( FILE * file, long & bufferSize, const long maxSize = 0 );


};


#endif // SCANDATAFORMAT_H
