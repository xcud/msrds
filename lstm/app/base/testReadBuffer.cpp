//#include<stdio.h>
//
//#include "IOBase.h"
//#include "scanDataFormat.h"
//
//
//int main() 
//{
//
//  ScanDataFormat scanDataFormat;
//
//  FILE * inFile = fopen("in.txt", "r");			 
//  FILE * outFile = fopen("out.txt", "w");			 
//  int status; 
//
//  long lines = TIOBase::lineCount( NULL, inFile );
//  printf( "lines: %ld\n", lines );
//
//  // 8388608 / 8; // 1 MB
//  const long size = 100;//TIOBase::getFileSize( inFile ) + 1;
//  long validBytes;
//  printf("size:%ld\n", size);
//  char buf[size];
//
//  do
//    {
//      status = 
//	scanDataFormat.fillBuffer( inFile, buf, size, validBytes);
//      fprintf(outFile,"%s", buf);
//    }
//  while( !status );
//    
//  fclose(inFile);
//  fclose(outFile);
//
//  return 0;
//}
