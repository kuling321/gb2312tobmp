# gb2312tobmp
将文字的gb2312编码转换为bmp图片。

# 运行demo

sudo git clone https://github.com/kuling321/gb2312tobmp.git

cd gb2312tobmp

make

./a    #运行demo finsh后，图片保存在/tmp/bmp.bmp。


# 函数介绍
gb2312tobmp(tagBITMAPFILEHEADER_t *_bmp_p, unsigned char *_fontData_p , int _fonthight, word _bitcount);

参数

_bmp_p     ：bmp图片结构体指针

_fontdata_p :传入的GB2312文字编码

_fonthight  ：字体大小（16、32、48）

_bitcount   ：图片位深（16、24、32）
