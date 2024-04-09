#include <stdio.h>
#include<stdlib.h>
#include "decode.h"
#include <string.h>
#include "common.h"
#include "types.h"

/* Function Definitions */ 
// validation checking funtion
static int flag = 0;
static char secrt[20];

//validation
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
   {    
      if ( argv[3] !=NULL )
	  {   flag =1;
		  strcpy(secrt,argv[3]);
	  }
	  else
	  { 
		  strcpy(secrt,"decode");
	  }
	  
     

      if ( (argv[2] != NULL)  && (strchr(argv[2],'.') != NULL) ) 
         { 
           if ( !strcmp ( strchr(argv[2], '.') ,".bmp") )
           {  
			 decInfo -> stego_image_fname = argv[2];
             return e_success;
           }
          else
          {     
			  printf("INFO : Please pass the bmp file\n");
		   return e_failure;
           }
        }
    else
      {
         printf("INFO : Please pass the bmp file\n");
         return e_failure;   
      }
   }
// encoding
Status do_decoding(DecodeInfo *decInfo)
{ //1.open stego image
  if( open_stego_image(decInfo) == e_success )
   { 
    printf("INFO : Stegoimage opened Successfully\n");
     
   }
   else
    {
       printf("INFO : Stegoimage opened Failed\n");
       return e_failure;
     }
   
   //2. checking magic string
   if( checking_magic_string( MAGIC_STRING, decInfo  ) == e_success )
   { 
     printf("INFO : Magic string found\n");
     
   }
   else
   {
    printf("INFO : Magic string not found\n");
	return e_failure;
   }
    //3.decoding file extension size
  if ( decode_secret_file_extn_size(decInfo) == e_success )
  {   
      printf("INFO : Decode file extn size Success\n");
      
  }
 else
 {
    printf("INFO : Decode file extn size Fail");
    return e_failure;
 } 
 //3.decoding file extension
 if(decode_secret_file_extn(decInfo) == e_success )
  { 
    printf("INFO : Decode file extn Success\n");
      
  }
  else
   {
     printf("INFO : Decode file extn Fail\n");
     return e_failure;
   }
   //4.open secert data file
  if(open_secret_file(decInfo) == e_success ) 
  {         
	    printf("INFO : Opened %s Success\n",decInfo->secret_fname);
            
  }
 else
  {
	printf("INFO : Secert file opening Fail\n");
	return e_failure;
  }
  // 5.decode secret file size
  if( decode_secret_file_size(decInfo) == e_success )
  { 
    printf("INFO : Decode file size Success\n");
    
  }
  else
  {  
       printf("INFO : Decode file size Fail\n");
       return e_failure;
   } 


  if(decode_secret_file_data(decInfo) == e_success )
   {  
      printf("INFO : Decode file data Success\n");
      
   }
   else
    {
      printf("INFO : Decode file data Fail\n");
      return e_failure;
    }
  return e_success;
  }

// decode file size
  Status decode_secret_file_size(DecodeInfo *decInfo)
  {  
    int size;
    char str[32];
    fread(str ,1 ,32,decInfo->fptr_stego_image);
    decode_lsb_to_size(&size,str);
    decInfo->size_secret_file = size ;
        //	printf("size = %d\n", decInfo->size_secret_file);
    return e_success; 
  }


  // opening secret file

  Status open_secret_file(DecodeInfo *decInfo)
   { if(flag == 0)
	  {
		printf("INFO : Output file not mentioned.");
		strcat(secrt , decInfo->extn_secret_file);
	    decInfo->secret_fname = secrt;
	    printf("Creating %s as default\n",decInfo->secret_fname);
        decInfo->fptr_secret  = fopen( decInfo->secret_fname , "w");
         // Do Error handling
          if (decInfo->fptr_secret  == NULL)
           { perror("fopen");
             fprintf(stderr, "ERROR: Unable to open file %s\n",  decInfo->secret_fname );
             return e_failure;
            }
	}
	else
	{
		 strcat(secrt , decInfo->extn_secret_file);
	     decInfo->secret_fname = secrt;
         decInfo->fptr_secret  = fopen( decInfo->secret_fname , "w");
	      // Do Error handling
	      if (decInfo->fptr_secret  == NULL)
             { perror("fopen");
               fprintf(stderr, "ERROR: Unable to open file %s\n",  decInfo->secret_fname );
               return e_failure;
             }
	}

 return e_success;
}
//open stego image

  Status open_stego_image(DecodeInfo *decInfo )
 {   //stego_image
     decInfo->fptr_stego_image  = fopen( decInfo->stego_image_fname , "r");
    // Do Error handling
   if (decInfo->fptr_stego_image  == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n",  decInfo->stego_image_fname );

    	return e_failure;
    }
  return e_success;
 }
//checking magic string
  Status checking_magic_string(char * magic_string ,DecodeInfo *decInfo  )
 {   int len = strlen(magic_string);
	 char str[len+1];
     fseek(decInfo->fptr_stego_image ,54,SEEK_SET);
     decode_image_to_data(str,strlen(magic_string),decInfo->fptr_stego_image);
	 str[len]='\0';
	 if( strcmp(str,magic_string) == 0 )
	 { 
      return e_success;
	 }
	 else
    {
    	return e_failure;
	}

	
 }

// encode file data
  Status decode_secret_file_data(DecodeInfo *decInfo)
 { 
	int len = decInfo->size_secret_file;
	char str[len+1];
    decode_image_to_data(str,len,decInfo->fptr_stego_image);
	str[len]='\0';
     // printf("%s\n",str);
	fputs(str,decInfo->fptr_secret);
    return e_success;
 }

// image to data
  Status decode_image_to_data(char *data,int size,FILE *fptr_stego_image )
 {  
   	int j;
	char str[8];
  for(j = 0; j < size; j++)
  { 
	    fread(str, 8,sizeof(char),fptr_stego_image );
        decode_lsb_to_byte(&data[j],str);
    	
  }

  return e_success;
 }
//lsb to byte

  Status decode_lsb_to_byte(char *data ,char * str)
 {  
   	char ch = 0x00;int i=0;
	while(i<8){
    ch = (((str[i] & 0x01)<<i)|ch);
	i++; }
   *data = ch;
    return e_success;
 }

// decoding secretfile extn size

  Status decode_secret_file_extn_size(DecodeInfo *decInfo)
 {   int size;
    char str[32];
	fread(str ,1 ,32,decInfo->fptr_stego_image);
	decode_lsb_to_size(&size,str);
	decInfo->extn_secret_file_size = size ;
//	printf("size = %d\n", decInfo->extn_secret_file_size);
	return e_success;
 } 

// decoding secret file extn

  Status decode_secret_file_extn(DecodeInfo * decInfo)
 {   
	int len = decInfo->extn_secret_file_size ;//storing size of extn
	char str[len+1];
    decode_image_to_data(str,len,decInfo->fptr_stego_image);
	str[len]='\0';
    strcpy(decInfo->extn_secret_file, str );   
    //printf("%s\n", decInfo->extn_secret_file);
return e_success;
 }
// lsb to size

  Status decode_lsb_to_size(int * size, char *str )
 { 
  int a = 0x0000;// 4byte data
  int i = 0;
  while( i < 32 ){
  a = (((str[i] & 0x01)<<i) | a);
  i++;}
  *size = a;
  return e_success;
 } 
 
