#ifndef DECODE_H
#define DECODE_H


#include "types.h"// contain user defined datatype

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4
 

typedef struct _DecodeInfo
{
     char *magicstring;

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    int extn_secret_file_size;
    char extn_secret_file[MAX_FILE_SUFFIX];
     
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    

} DecodeInfo;



/* Read and validate Encode args from argv */

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
/* Decoding text data*/
Status do_decoding(DecodeInfo *decInfo);
/* open stego image*/
Status open_stego_image(DecodeInfo *decInfo ); 
/* checking magic string*/
Status checking_magic_string(char * magic_string ,DecodeInfo *decInfo  );
/*image to data  */
Status decode_image_to_data(char *data,int size, FILE *fptr_stego_image );
/*lsb to  byte  */
Status decode_lsb_to_byte( char *data,char *str);
/* Decoding extn size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo);
/*  lsb to size*/
Status decode_lsb_to_size(int *size, char *image_buffer);
/* Decoding extn */
Status decode_secret_file_extn(DecodeInfo * decInfo);
/*  open secret file */
Status open_secret_file(DecodeInfo *decInfo);
/*  decode file size */
Status decode_secret_file_size(DecodeInfo *decInfo);
 /* decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);



#endif
