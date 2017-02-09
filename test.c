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
    // unsigned char *pdotMatrix = NULL;
    unsigned char data[]={0xC8,0xAB,0xD6,0xBE,0xBF,0xC6,0xBC,0xBC,0x2D,0x41,0x6C,0x6C,0x57,0x69,0x6E,0x6E,0x65,
                            0x72,0x54,0x65,0x63,0x68,0x6E,0x6F,0x6C,0x6F,0x67,0x79,0x00}; //全志科技-AllWinnerTechnology (GB2312码)
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
    
    // free(pdotMatrix); //free the memory
    free(bmp_p.picData);

    printf("Finish!\n");
    return 0;

   }
}