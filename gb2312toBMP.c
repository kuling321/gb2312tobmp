/* 
Copyright (c) 2017, allwinnertech.com
All rights reserved.

文件名称:gb2312toBMP.h
文件标识:
摘    要:将gb2312编码文字转换成bmp图片 

当前版本:1.0
作    者:K.L
完成日期：2017.2.8


 */

#include "gb2312toBMP.h"

#define ASCII     0
#define GB2312    1

static long g_offset_word= 0;
static unsigned char *g_file_p = NULL;

//生产bmp图片的一些信息
struct string_infor
{
    dword resolution_x; //最终图像宽的分辨率
    dword resolution_y; //最终图像高的分辨率
    dword picSize;      //bmp图像数据区大小
    int   gb2312Quan;   //字符中有多少个汉字
    int   asciiQuan;    //字符中有多少个ASCII字符
}stringInfor;


/***********************************
name:init_bmp_head(tagBITMAPFILEHEADER_t *_bmp_p, dword _width, dword _height, word _bitcount)
function:生成bmp文件头信息
parameter：bmp文件结构体_bmp_p,图像的宽_width， 高_height，和位深_bitcount
return:无
************************************/
void init_bmp_head(tagBITMAPFILEHEADER_t *_bmp_p, dword _width, dword _height, word _bitcount)
{

    _bmp_p-> biSizeImage = (_width * _height *_bitcount)/8;  //图像区数据的大小，以字节为单位(图像数据信息大小=（图像宽度*图像高度*记录像素的位数）/8)

    _bmp_p-> bfType = 0x4D42;    //文件类型
    _bmp_p-> bfSize = ( _bmp_p-> biSizeImage + 54);   //文件大小，字节为单位
    _bmp_p-> bfReserved1 = 0x0;   //保留，必须为0
    _bmp_p-> bfReserved2 = 0x0;   //保留，必须为0
    _bmp_p-> bfOffBits = 54;         //从文件头开始的偏移量
 
    //bmp info head
    _bmp_p-> biSize = 40;            //图像描述信息块的大小
    _bmp_p-> biWidth = _width;           //图像的宽度，以像素为单位
    _bmp_p-> biHeight = _height;          //图像的高度，以像素为单位
    _bmp_p-> biPlanes = 0x01;          //为目标设备说明位面数，其值总是设为1
    _bmp_p-> biBitCount = _bitcount;        //记录像素的位数，很重要的数值，图像的颜色数由该值决定。
    _bmp_p-> biCompression = 0;     //图像数据压缩类型
    // bmp_p-> biSizeImage = _size;  //图像区数据的大小，以字节为单位(图像数据信息大小=（图像宽度*图像高度*记录像素的位数）/8)
    _bmp_p-> biXPelsPerMeter = 0x0;  //水平分辨率，像素/米
    _bmp_p-> biYPelsPerMeter = 0x0;    //垂直分辨率，同上
    _bmp_p-> biClrUsed = 0;         //位图实际使用的彩色表中的颜色索引数
    _bmp_p-> biClrImportant = 0;    //对图像显示有重要影响的颜色索引的数目
}

/***********************************
name:find_out_dot_matrix(int _datatype, unsigned char *_pfontData , int _fonthight, FILE *_fd)
function:从字库中找出字的点阵信息
parameter：_datatype:找的是ASSCI还是GB2312 ， _pfontData:GB2312编码 ，_fonthight:字体大小 ， _fd:字库文件的文件指针
return:该字点阵信息内存的指针
************************************/
unsigned char *find_out_dot_matrix(int _datatype, unsigned char *_pfontData , int _fonthight, FILE *_fd)
{

    long offset = 0;
    unsigned char *pfile = NULL;
    int wordsize;

    switch(_datatype)
    {
        case ASCII: 
        {
            wordsize = _fonthight * _fonthight / 2 / 8;
            offset = ((*_pfontData ) - 0x20) * (_fonthight * _fonthight / 2) / 8;  //ASCII字库不包含控制字符，所以减去前面的0x20个
            break;
        }

        case GB2312:
        {
            wordsize = _fonthight * _fonthight / 8;
            if(_fonthight == 48 && *(_pfontData) >= 0xb0 )  //符号区为：0xa1~0xaf
                //40,48尺寸的字库把汉字库和全角字符库分开了，因为我们只需汉字库，所以在计算这两个汉字库的offset时，需要多减去0x0f    
                offset = ((*(_pfontData)-0xa1 - 0x0f)*94 + *(_pfontData+1)-0xa1)*wordsize ;

            else
                offset = ((*(_pfontData)-0xa1)*94 + *(_pfontData+1)-0xa1)*wordsize ;

            break;
        }
        default: printf("Datatype error\n"); return NULL;
    }

    pfile = (unsigned char *)malloc(sizeof(char)*wordsize);   //申请一段内存

     fseek(_fd, offset, SEEK_SET); //offset from start

     if(fread(pfile, wordsize, 1, _fd) != 1 )
     {

        printf("read the file error\n");
        return NULL;
     }

     return pfile;   
}

/***********************************
name:toBMPdata(int _datatype, unsigned char *_data_p, int _piexlsize, int _fonthight)
function:将一个字点阵信息转化为BMP（RGB）数据，并连起来。
parameter：_datatype:找的是ASSCI还是GB2312 ，_data_p:点阵信息的指针  _piexlsize:每像素大小  ,_fonthight:字体大小 
return:无（因为存储在全局的指针内存中）
************************************/
void toBMPdata(int _datatype, unsigned char *_data_p, int _piexlsize, int _fonthight)
{
    int count_x,count_y,b;
    int offset = 0;
    int count = 0;
    int row = 0;
    int line = 0;
    int i=0;
    int width,wordSize;

    wordSize = _fonthight;

    switch(_datatype)
    { 
        case ASCII: 
        {
            if(wordSize == 48)
                width =_fonthight/2; //适用于48字库按行取值
            else
            {
                width =  _fonthight;
                _fonthight /=2;
            }
            break;
        }
        case GB2312: width =_fonthight; break;
        default: printf("Datatype error\n"); 
    }

    for(count_y=0;count_y<_fonthight;count_y++) 
    {
        //上下翻转  主要用于48字库是按行取值，16,24,32是按列取值
        if(wordSize == 16 || wordSize == 24 || wordSize == 32)
        row = count_y;
        //正相 
        else
        row = (_fonthight-1) - count_y;

        for(count_x=0; count_x<width; count_x++)  
        {
            //旋转+90度   主要用于48字库是按行取值，16,24,32是按列取值
            if(wordSize == 16 || wordSize == 24 || wordSize == 32)
            {
                line = (width-1) - count_x;
                offset = (row + line*(stringInfor.resolution_x) + g_offset_word)*_piexlsize;                 
            }
            //不旋转
            else
            {
                line = count_x;
                offset = (row*(stringInfor.resolution_x) + line + g_offset_word)*_piexlsize; 
            }


            if( *(_data_p+count/8) & 0x80 )
            {
                for(b=0; b<_piexlsize; b++)
                {
                  *(g_file_p + offset + b) = 0xff;    //white  frnotgound color is white                
                }
            }
            else 
            {
                 for(b=0; b<_piexlsize; b++)  
                {
                  *(g_file_p + offset + b) = 0x00;   //black  backgound color is black
                }   
            }
           *(_data_p+count/8) <<= 1;
            count++; 
        }       

    }

}

// //test  将获取的字库点阵信息输出到屏幕上
// void dot_matrix_disp(unsigned char *data)
// {
//     int i,j;
//     int count = 0;
//     for(i=0;i<16;i++)  //设置行
//     {
//         for(j=0; j<8; j++) //设置列
//         {
//             if( *(data+count/8) & 0x80 )
//             {
//                 printf("● ");
//             }
//             else 
//             {
//                 printf("○ ");
//             }
//            *(data+count/8) <<= 1;
//             count++;
//         }       
//     printf("\n");
//     }

// }


/***********************************
name:gb2312tobmp(tagBITMAPFILEHEADER_t *_bmp_p, unsigned char *_fontData_p , int _fonthight, word _bitcount)
function:将GB2312编码的字符串转换为BMP图片。
parameter：_bmp_p:BMP图片结构体指针，_fontData_p:GB2312编码字符串，_fonthight:字体大小，_bitcount:BMP图片位深
return:成功返回0 ，失败返回-1
************************************/
int gb2312tobmp(tagBITMAPFILEHEADER_t *_bmp_p, unsigned char *_fontData_p , int _fonthight, word _bitcount)
{
    unsigned char *pWordData = NULL;
    int i=0;
    int piexlSize;
    FILE *ascii_fd = NULL;
    FILE *gb2312_fd = NULL;
    FILE *gb2312t_fd = NULL;   
    stringInfor.gb2312Quan = 0;  //the number of gb2312
    stringInfor.asciiQuan = 0;  //the number of ascii
    g_offset_word = 0;

    //compute the quantity of string 计算一共多少个汉字和字符
    for(;;)  
    {
        if(*(_fontData_p+i) > 0xA0 && *(_fontData_p+i)  < 0xff)  //gb2312
        {
            stringInfor.gb2312Quan += 1;
            i += 2;
        }
        else if (*(_fontData_p+i) > 0x1f && *(_fontData_p+i) < 0x80)  //ascii
        {
            stringInfor.asciiQuan += 1;
            i += 1;            
        }
        else break;
    }

    //open HZK file 打开相应的字库文件
    if(stringInfor.gb2312Quan > 0)
    {
       switch(_fonthight)
        {
            case 16:
            {
                gb2312_fd = fopen("font_lib/HZK16","r");
                if(NULL == gb2312_fd)
                {
                    printf("open gb2312.hzk file error\n");
                    return -1;
                }
                break;
            }
            case 32:
            {
                gb2312_fd = fopen("font_lib/HZK32","r");
                if(NULL == gb2312_fd)
                {
                    printf("open gb2312.hzk file error\n");
                    return -1;
                }
                break;
            }
            case 48:
            {
                gb2312_fd = fopen("font_lib/HZK48","r");
                gb2312t_fd = fopen("font_lib/HZK48T","r");
                if(NULL == gb2312_fd || NULL == gb2312t_fd)
                {
                    printf("open gb2312.hzk file error\n");
                    return -1;
                }
                break;                
            }
            default:return -1;
        }
    }
    //open ASCII.HZK file 打开相应的字库文件
    if(stringInfor.asciiQuan > 0)
    {
       switch(_fonthight)
        {
            case 16:
            {
                ascii_fd = fopen("font_lib/ASC16_8","r");
                if(NULL == ascii_fd)
                {
                    printf("open ascii.hzk file error\n");
                    return -1;
                }
                break;
            }  
             
            case 32:
            {
                ascii_fd = fopen("font_lib/ASC32_16","r");
                if(NULL == ascii_fd)
                {
                    printf("open ascii.hzk file error\n");
                    return -1;
                }
                break;
            }
            case 48:
            {
                ascii_fd = fopen("font_lib/ASC48_24","r");
                if(NULL == ascii_fd)
                {
                    printf("open ascii.hzk file error\n");
                    return -1;
                }
                break;
            }
            default:return -1;
        }
    }


    //计算需要多大的空间存储
                //(_fonthight*_fonthight*stringInfor.gb2312Quan + _fonthight*_fonthight/2*stringInfor.asciiQuan) * 16 /8  (16 is countbit)
    stringInfor.picSize = (_fonthight*_fonthight) * 
                            (stringInfor.gb2312Quan * _bitcount + stringInfor.asciiQuan * _bitcount / 2) /8;
    //GB2312 is 16*16 or  48*48
    //ASCII  is 8*16  or  24*48


    //总分辨率 Resolution
    stringInfor.resolution_x = stringInfor.gb2312Quan * _fonthight+ stringInfor.asciiQuan* _fonthight/2;
    stringInfor.resolution_y = _fonthight; 

    
    i = 0;
    piexlSize = _bitcount/8;  // per pixel size
    //申请 bmp data 总内存大小
    g_file_p = (unsigned char *)malloc(sizeof(char)*stringInfor.picSize);
    //find out gb2312 dot matrix and change to bmp data
    for(;;) 
    {
        //GB2312 Chinese character 处理 GB2312汉字并转化为bmp
        if(*(_fontData_p+i) > 0xA0 && *(_fontData_p+i)  < 0xff)  //gb2312
        {
            if(_fonthight == 48 && *(_fontData_p+i)  < 0xb0) //48字号的全角符号字库
            {
                pWordData = find_out_dot_matrix(GB2312, (_fontData_p+i), _fonthight, gb2312t_fd);
            }
            //从hzk文件获取点阵数据
            else
            pWordData = find_out_dot_matrix(GB2312, (_fontData_p+i), _fonthight, gb2312_fd);
            if(NULL == pWordData)
            {
                  printf("ERROR\n");
                  return -1;
            }
            //将点阵数据转换为bmp数据
            toBMPdata(GB2312, pWordData, piexlSize, _fonthight); //start to change bmp data

            g_offset_word += _fonthight; //next word offset
            free(pWordData); //free the pWordData  
            pWordData = NULL;         
            i += 2;
        }
        //ASCII character 处理ASCII字符并转化为bmp
        else if (*(_fontData_p+i) > 0x1f && *(_fontData_p+i) < 0x80)  //ascii
        {
            //从hzk文件获取点阵数据
            pWordData = find_out_dot_matrix(ASCII, (_fontData_p+i), _fonthight, ascii_fd);
            // dot_matrix_disp((_fontData_p+i));
            if(NULL == pWordData)
            {
                  printf("ERROR\n");
                  return -1;
            }
            //将点阵数据转换为bmp数据
            toBMPdata(ASCII, pWordData, piexlSize, _fonthight); //start to change bmp data

            g_offset_word += _fonthight/2; //next word offset
            free(pWordData); //free the pWordData  
            pWordData = NULL;  
            i += 1;  
        }


        else break;
    }

    //close file 
    if(NULL != gb2312_fd)
        fclose(gb2312_fd);
    if(NULL != gb2312t_fd)
        fclose(gb2312t_fd);
    if(NULL != ascii_fd)
        fclose(ascii_fd);

    init_bmp_head(_bmp_p,stringInfor.resolution_x,stringInfor.resolution_y,_bitcount);
   _bmp_p->picData =  g_file_p;

   return 0;
}




