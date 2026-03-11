#ifndef BITMAP_H
#define BITMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#pragma pack(push)
#pragma pack(1)
typedef struct tagBITMAPFILEHEADER_T{
    unsigned short bfType;
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;
}BITMAPFILEHEADER_T;

typedef struct tagBITMAPINFOHEADER_T{
    unsigned int biSize;
    unsigned int biWidth;
    unsigned int biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int biCompression;
    unsigned int biSizeImage;
    unsigned int biXPelsPerMeter;
    unsigned int biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;
}BITMAPINFOHEADER_T;

static void create_bitmap(char *pFileName, unsigned char* pbFrame, int cbFrame, int cx, int cy)
{
    //set the file deader for bitmap file
    BITMAPFILEHEADER_T bitMapFileHeader;
    memset(&bitMapFileHeader,0,sizeof(BITMAPFILEHEADER_T));

    bitMapFileHeader.bfType = 0x4D42;
    bitMapFileHeader.bfSize = cbFrame + 54;//  m_cx*m_cy*3+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)
    bitMapFileHeader.bfReserved1 = 0;
    bitMapFileHeader.bfReserved2 = 0;
    bitMapFileHeader.bfOffBits = 54;  //sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)

    //set the info header for Bitmap file
    BITMAPINFOHEADER_T bitMapInfoHeader;
    memset(&bitMapInfoHeader,0,sizeof(BITMAPINFOHEADER_T));

    bitMapInfoHeader.biSize = 40;//sizeof(BITMAPINFOHEADER)
    bitMapInfoHeader.biWidth = cx;//XI_CAPTURE_WIDTH
    bitMapInfoHeader.biHeight = cy;//XI_CAPTURE_HEIGHT
    bitMapInfoHeader.biPlanes = 1;
    bitMapInfoHeader.biBitCount = 24;
    bitMapInfoHeader.biCompression = 0;
    bitMapInfoHeader.biSizeImage = 0;
    bitMapInfoHeader.biXPelsPerMeter = 0;
    bitMapInfoHeader.biYPelsPerMeter = 0;
    bitMapInfoHeader.biClrUsed =0;
    bitMapInfoHeader.biClrImportant =0;

    //creae the BipMap file
    FILE *fd = fopen(pFileName,"wb");
    if(!fd)
        return;
    fwrite((const char *)&bitMapFileHeader, sizeof(BITMAPFILEHEADER_T), 1, fd);
    fwrite((const char *)&bitMapInfoHeader, sizeof(BITMAPINFOHEADER_T),1 ,fd);

    fwrite((const char *)pbFrame,cbFrame,1,fd);

    fclose(fd);
}

#pragma pack(pop)
#endif // BITMAP_H
