/* 
Copyright (c) 2017, allwinnertech.com
All rights reserved.

文件名称:gb2312toBMP.h
文件标识:
摘    要:将gb2312编码文字转换成bmp图片头文件

当前版本:1.0
作    者:K.L
完成日期：2017.2.8


 */

#ifndef _GB2313TOBMP_H_
#define _GB2313TOBMP_H_

#include <stdio.h>
#include <stdlib.h>


typedef unsigned char byte;  //8bit
typedef unsigned short dbyte; //16bit
// typedef unsigned long int dword; //64-bit system long int has 64 bit
typedef int dword;  //32bit
typedef unsigned short word;    //16bit

/*
*定义bmp文件的头部数据结构
*/
#pragma pack(push,2)  //保持2字节对齐
typedef struct tagBITMAPFILEHEADER {
    //bmp file header
    dbyte bfType;        //文件类型
    dword bfSize;            //文件大小，字节为单位
    word bfReserved1;   //保留，必须为0
    word bfReserved2;   //保留，必须为0
    dword bfOffBits;         //从文件头开始的偏移量


    //bmp info head
    dword  biSize;            //图像描述信息块的大小
    dword  biWidth;           //图像的宽度，以像素为单位
    dword  biHeight;          //图像的高度，以像素为单位
    word biPlanes;          //为目标设备说明位面数，其值总是设为1
    word biBitCount;        //记录像素的位数，很重要的数值，图像的颜色数由该值决定。
    dword biCompression;     //图像数据压缩类型（数值位0：不压缩；1：8位压缩；2：4位压缩）。 
    dword biSizeImage;       //图像区数据的大小，以字节为单位(图像数据信息大小=（图像宽度*图像高度*记录像素的位数）/8)
    dword biXPelsPerMeter;   //水平每米有多少像素，在设备无关位图（.DIB）中，每字节以00H填写。
    dword biYPelsPerMeter;   //垂直每米有多少像素，在设备无关位图（.DIB）中，每字节以00H填写。
    dword biClrUsed;         //位图实际使用的彩色表中的颜色索引数.如果为0，则表示默认值(2^颜色位数)
    dword biClrImportant;    //对图像显示有重要影响的颜色索引的数目,如值为0，表示所有颜色一样重要。 
      
    //bmp rgb quad
     //对于16位，24位，32位的位图不需要色彩表
    //unsigned char rgbBlue;    //指定蓝色强度
    //unsigned char rgbGreen;   //指定绿色强度
    //unsigned char rgbRed;     //指定红色强度
    //unsigned char rgbReserved; //保留，设置为0 

    byte *picData;
}tagBITMAPFILEHEADER_t;  //BMPFILEHEADER
#pragma (pop)

//字体大小的枚举
enum  word_size
{
    Size16 = 16,
    Size32 = 32,
    Size48 = 48,
} wordsize;

//图片位深的枚举
enum  bit_count
{
    bit16 = 16,
    bit24 = 24,
    bit32 = 32,
} bitcount;


//函数声明
int gb2312tobmp(tagBITMAPFILEHEADER_t *_bmp_p, unsigned char *_fontData_p , int _fonthight, word _bitcount);


#endif

