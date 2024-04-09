
/*
NAME: TANIA CHAKRABORTY
DATE: 17.07.2023
DESCRIPTION: project lsb stegography(encoding and decoding)
             encode text file in bmp image file
			 decode text file from bmp stego image

SAMPLE INPUT: bmp image file,secret data text file
*/



#include <stdio.h>
#include "encode.h"
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include "common.h"

int main(int argc , char **argv )
{  
   int operation;
  
   operation = check_operation_type(argv);

   if( operation == e_encode )
   {
	       
           printf("INFO : # Encoding Procedure Started #\n");
           
           EncodeInfo encInfo;
        if( read_and_validate_encode_args( argv , &encInfo)  == e_success )
		{         
                  printf("INFO : Read and validate encoding arguments success\n");
                  
		  if( do_encoding(&encInfo) == e_success )
		  {           
                      printf("INFO : Encoding Success\n");
                      
		  }
		  else
		  {           
                      printf("INFO : Encoding Failed\n");
					  
		  }
		}
		else
		{ 
          printf("INFO : Read and validate encoding arguments fail\n");
		  
		}
   }



   else if( operation == e_decode )
   {       
	       printf("INFO : # Decoding Procedure Started #\n");
           
           DecodeInfo decInfo;
		   
        if( read_and_validate_decode_args( argv , &decInfo)  == e_success )
		{         
                  printf("INFO : Read and validate decoding arguments success\n");
				   
		  if ( do_decoding(&decInfo) == e_success )
		  {  
             printf("INFO : Decoding is Success\n");
			 
		  }
		  else
		  {   
              printf("INFO : Decoding is Failed\n");
			  
		  }


		}
       

   }
   else 
   {   
	   printf("INFO : Pass correct argument\n");
       
   }

}

//defintion for check operation
OperationType check_operation_type(char *argv[])
{

   if( !strcmp(argv[1] , "-e" ))
   {
    return e_encode;
   }	 
   else if(!strcmp(argv[1] , "-d"))
   { 
    return e_decode;

   }
   else
	 return e_unsupported;

}



