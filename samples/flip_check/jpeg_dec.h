#ifndef _JPEG_DEC_H_
#define _JPEG_DEC_H_
int jpg_decoder_init(int width,int heigth,int ratio);
void jpg_decoder_deinit(void);
int jpg_decoder_fun(unsigned char *jpg_buf,unsigned char ** Y_buf,int pic_size);
void jpg_get_pic_size(unsigned int *width,unsigned int *heigth);
#endif