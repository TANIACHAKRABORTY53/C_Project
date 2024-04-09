#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* Function Definitions */


// validation checking funtion
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
  // checking bmp passed or not
  if ( (argv[2] != NULL)  && (strchr(argv[2],'.') != NULL) ) 
  { if ( !strcmp ( strchr(argv[2], '.') ,".bmp") )
	  {
        encInfo -> src_image_fname = argv[2];
	  }
	  else
	  {
		  printf("INFO : Please pass the bmp file\n");
		  return e_failure;
	  }

  }
  else
  {  
       printf("INFO : Please  pass the bmp file\n");
	   return e_failure;
  }  

  // checking secreat file passed or not
 if ( argv[3] != NULL )
 {
       encInfo -> secret_fname = argv[3];
  strcpy(encInfo -> extn_secret_file , strchr( argv[3] , '.' ));

 }
 else
 {
	  printf("INFO : Please pass the secret file\n");
	  return e_failure;

 }

 // checking encode image name passed or not
 if( argv[4] != NULL )
 {
    if   ( !strcmp( strchr( argv[4] , '.' ) ,".bmp" )  )
         { 
           encInfo->stego_image_fname = argv[4];
          } 
   else
         {
            printf("INFO : %s format not .bmp\n", argv[4]);
            encInfo->stego_image_fname = "default_stego.bmp"; 
            printf("INFO : Creating %s as default name\n", encInfo->stego_image_fname);
          }    
 }
 else
 {  
      encInfo -> stego_image_fname = "default_stego.bmp";
 }

 return e_success;
}

// encoding function
Status do_encoding(EncodeInfo *encInfo)
{
	//open files

 if( open_files(encInfo) == e_success )
   { 
     printf("INFO : Open files success\n");
   }

   else
   {
     printf("INFO : Open files failed\n");
     return e_failure;
   }
   // checking secret file empy or not
   printf("INFO : Checking for %s size\n",encInfo ->secret_fname);
  if( encInfo ->size_secret_file != 0 )
  {
	  printf("INFO : Done .Non Empty\n");
  }
  else
  {
   printf("INFO : %s is Empty file",encInfo->secret_fname);
     return e_failure;
  }
   // checking capacity
       printf("INFO : Checking for %s to handle %s\n",encInfo -> src_image_fname,encInfo ->secret_fname);

        encInfo ->image_capacity =  get_image_size_for_bmp(encInfo->fptr_src_image);
		
  if( encInfo->size_secret_file * 8 < ( encInfo->image_capacity ) )
    {
      printf("INFO : Done.Found ok\n");
     }
     else
     {
       printf("INFO : %s not have capacity to handle %s",encInfo -> src_image_fname,encInfo ->secret_fname);
       return e_failure;
      }

   // 1 copy bmp header
 if(  copy_bmp_header( encInfo -> fptr_src_image ,  encInfo -> fptr_stego_image ) == e_success)
    {
      printf("INFO : Copied bmp header Success\n");
    }
 else
    {
      printf("INFO : Copied bmp header Failed/n");
      return e_failure;
    }

  // 2 encoding magicstring

if( encode_magic_string(MAGIC_STRING, encInfo) == e_success)
  {
    printf("INFO : Encode magic string Success\n");
  }
  else
  {
    printf("INFO : Encode magic string Failed\n");
     return e_failure;
  }
   // 3 encoding secret file extn size

  if( encode_secret_file_extn_size( strlen(encInfo -> extn_secret_file ), encInfo) == e_success)
   {
      printf("INFO : Encode secret file extn size Success\n");
   }
 else
  {
     printf("INFO : Encode secret file extn size Failed\n");
     return e_failure;
  }
  // 4 encoding secret file extn 

  if( encode_secret_file_extn(encInfo -> extn_secret_file  , encInfo) == e_success)
   { 
      printf("INFO : Encode secret file extn Success\n");
   }
  else
   {
      printf("INFO : Encode secret file extn Failed\n");
      return e_failure;
   }
  

  // 5 encoding  sectrt file size
  if(encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
   {
      printf("INFO : Encode secret file size Success\n");
              	//  printf("%d file size",encInfo -> size_secret_file);
   }
   else
   {
     printf("INFO : Encode secret file size failed\n");
     return e_failure;
   }
    
  // 6 encoding secret data
  if(encode_secret_file_data(encInfo) == e_success)
   {
      printf("INFO : Encode secret file data Success\n");
               
   }
   else
   {
     printf("INFO : Encode secret file data failed\n");
     return e_failure;
   }
  
  // 7 encoding remaing data
 if(copy_remaining_img_data( encInfo -> fptr_src_image ,  encInfo -> fptr_stego_image ) == e_success )
 {
    printf("INFO : Copying remaining data Success\n");
 }
 else
 {
   printf("INFO : Copying remaining data  Failed\n");
   return e_failure;
 }


return e_success;

   
}




// encoding-1 bmp header copying header function

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
  char bmp_header[51];
  rewind(fptr_src_image);
  rewind(fptr_dest_image);
  fread( bmp_header, 54, 1, fptr_src_image);
  fwrite( bmp_header, 54, 1, fptr_dest_image);
  return e_success;
}

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    // printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    // printf("height = %u\n", height);
	rewind(  fptr_image );

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }
    

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
    	return e_failure;
    }
                //finding file size
                rewind(encInfo->fptr_secret);
		        fseek(encInfo->fptr_secret,0,SEEK_END);
		        encInfo->size_secret_file = ftell(encInfo->fptr_secret);
                rewind(encInfo->fptr_secret);

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;


}


//encode  1byte to image array

Status encode_byte_to_lsb(char data, char *image_buffer)
{
   for( int i = 7; i >= 0; i-- )
    {
      image_buffer[i] = ((data & (1<<i)) >> i) | ((image_buffer[i] & 0XFE));
    }
}

//magic string encoding

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
  encode_data_to_image( magic_string, strlen(magic_string) , encInfo-> fptr_src_image, encInfo-> fptr_stego_image);
  return e_success;
}

//data to image

Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{  char str[8];
  for(int i = 0; i < size ; i++ )
   {
     fread( str ,8, 1,fptr_src_image);
     encode_byte_to_lsb(data[i],str); 
     fwrite( str ,8, 1,fptr_stego_image );
   }

}

//4byte size of extension to image array(32)
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char str[32];
    fread( str ,1, 32,encInfo->fptr_src_image);
    encode_size_to_lsb( size,str);
    fwrite( str ,1, 32,encInfo->fptr_stego_image );
    return e_success;

}

//4bytes size to image array
Status encode_size_to_lsb(int size, char *image_buffer)
{
	for(int i = 31; i >= 0; i--)
	{
		image_buffer[i] = ((size & (1<<i) )>> i) | ((image_buffer[i] & 0xFE));
	}
}

// encode file secert file extension to image array
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
     encode_data_to_image(file_extn,strlen(file_extn),encInfo-> fptr_src_image, encInfo-> fptr_stego_image);
     return e_success;

}

// encode secret file size  

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char str[32];
    fread(str , 1, 32, encInfo -> fptr_src_image);
    encode_size_to_lsb(file_size, str);
    fwrite(str, 1, 32, encInfo -> fptr_stego_image);
    return e_success;	
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
        char str[encInfo -> size_secret_file];
	fread(str, 1, encInfo -> size_secret_file, encInfo -> fptr_secret);// read secret filed data
        encode_data_to_image(str, encInfo -> size_secret_file, encInfo -> fptr_src_image, encInfo-> fptr_stego_image);
        return e_success;
}


Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
	char ch;
	while(fread(&ch, 1, 1, fptr_src) > 0)
	{
		fwrite(&ch, 1, 1, fptr_dest);
	}
	return e_success;
}

