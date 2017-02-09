/* 
Copyright (c) 2017, allwinnertech.com
All rights reserved.

文件名称:test.c
文件标识:
摘    要:测试gb2312tobmp()函数。并将它生产的BMP图片保存到磁盘

当前版本:1.0
作    者:K.L
完成日期：2017.2.8


 */
#include "gb2312toBMP.h"


int main(void)
{
    tagBITMAPFILEHEADER_t bmp_p;

    unsigned char data[]={0xC4,0xE3,0xBA,0xC3,0xA1,0xA3,0x48,0x65,0x6C,0x6C,
                            0x6F,0x20,0x77,0x6F,0x72,0x6C,0x64,0x21,0x00}; //你好。Hello world!  必须有个结束符号 0x00
    FILE *fd;

    wordsize = Size48;  //选择字体大小 。
    bitcount = bit16;   //选择bmp图片的位数


    if(gb2312tobmp(&bmp_p, data, wordsize, bitcount) != 0 )
   {
        printf("change fail\n");
        return -1;
   } 

   else
   {

    fd = fopen("/tmp/bmp.bmp", "w");
    if( fd == NULL)
    {
        printf("file open fail\n");
        return -1;
    } 

    fwrite(&bmp_p, 54, 1, fd);  //write head
    fwrite(bmp_p.picData, bmp_p.biSizeImage, 1, fd); //write the data
    fclose(fd);
    
    free(bmp_p.picData);

    printf("Finish!\n");
    return 0;

   }
}