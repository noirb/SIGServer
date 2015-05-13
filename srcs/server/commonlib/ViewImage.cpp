/* $Id: ViewImage.cpp,v 1.12 2011-10-25 07:39:58 okamoto Exp $ */
#ifdef WIN32
#include <windows.h>
#define _USE_MATH_DEFINES
#endif

#include <math.h>
#include "ViewImage.h"
#include <stdio.h>
#include "Logger.h"

int ViewImage::getWidthBytes(int width, int bytesPerOnePixel)
{
	int widthByteSize = width * bytesPerOnePixel;

	// width must be aligned 32bit boundary
	if ((widthByteSize & 0x03) > 0) {
		widthByteSize = ((widthByteSize >> 2) + 1) << 2;
	}

	return widthByteSize;
}

int ViewImage::calcBufferSize(const ViewImageInfo &info)
{
	return getWidthBytes(info.getWidth(), info.getBytesPerOnePixel())*info.getHeight();
}


#ifndef WIN32

struct BITMAPFILEHEADER
{
	unsigned short bfType;
	unsigned       bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned       bfOffBits;
} __attribute__((packed));

struct BITMAPINFOHEADER
{
	unsigned       biSize;
	int            biWidth;
	int            biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned       biCompression;
	unsigned       biSizeImage;
	int            biXPixPerMeter;
	int            biYPixPerMeter;
	unsigned       biClrUsed;
	unsigned       biClrImportant;
};

#endif


bool ViewImage::saveAsWindowsBMP(const char *fname)
{
#ifndef WIN32
	assert(m_info.getDataType() == IMAGE_DATA_WINDOWS_BMP);
	
	FILE *fp = fopen(fname, "wb");
	if (!fp) { return false; }

	assert(sizeof(BITMAPFILEHEADER) == 14);
	assert(sizeof(BITMAPINFOHEADER) == 40);
	
	BITMAPINFOHEADER bi = {0};
	
	char tmp[m_buflen*3];
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = getWidth();
	bi.biHeight = -getHeight();
	bi.biPlanes = 1;

	switch(m_info.getColorBitType()) {
		case COLORBIT_24:
		{
			bi.biBitCount = 24; break;
		}
		case DEPTHBIT_8:
		{
			bi.biBitCount = 24;

			int height = getHeight();
			int width = getWidth();

			/*
			  double fov = getFOVy();
			  double fl = 120/tan(fov/2*M_PI/180);
			*/

			for(int i = 0; i < height; i++){
				for(int j = 0; j < width; j++){

					int tmp_i = i*width+j;
					//int distance = (unsigned char)m_buf[i*width+j]/cos(theta);
					char distance = m_buf[i*width+j];
		
					tmp[tmp_i*3]   = distance;
					tmp[tmp_i*3+1] = distance;
					tmp[tmp_i*3+2] = distance;
				}
			}
			break;
		}
		//bi.biBitCount = 8;
		//bi.biClrUsed = 0; break;
		default:
			assert(0);
	}
	
	int size = getBufferLength();

	bi.biCompression = 0;
	bi.biSizeImage = size;

	BITMAPFILEHEADER bf = {0};

	bf.bfType = *(unsigned short*)"BM";
	bf.bfReserved1 = 0;
	bf.bfReserved2 = 0;
	bf.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bf.bfSize = bf.bfOffBits + bi.biSizeImage;

	fwrite((char*)&bf, sizeof(bf), 1, fp);
	fwrite((char*)&bi, sizeof(bi), 1, fp);

	if(m_info.getColorBitType() == DEPTHBIT_8) 
	{
		fwrite(tmp, size*3, 1, fp);
	}
	else
	{
		fwrite(getBuffer(), size, 1, fp);
	}
	fclose(fp);
#endif
	return true;
}

#ifdef WIN32
void ViewImage::setBitImageAsWindowsBMP(unsigned char *bitImage)
{
	if (!bitImage) return;

	int width, height;
	int widthByteSize, imageByteSize;
	int x, y;

	width = getWidth();
	height = getHeight();

	double fov = getFOVy();
	double fl = (height/2)/tan(fov/2*M_PI/180);

	// width must be aligned 32bit boundary
	widthByteSize = getWidthBytes(width, m_info.getBytesPerOnePixel());

	// clear buffer
	memset(m_buf, 0, m_buflen);

	int nbyte = m_info.getBytesPerOnePixel();

	// Note: be cautious to difference of y-axis order and pixel format.
	// (src) RGBARGBARGBA....
	// (dst) BGRBGRBGR....
	for (y=0; y<height; y++) {
		for (x=0; x<width; x++) {
			unsigned char *pFrom = bitImage + ((height-(y+1))*width + x)*4;
			for(int i = 0;i<nbyte;i++){
				//modified by okamoto@tome (2011/9/16)
				m_buf[y*widthByteSize + x*nbyte + nbyte-(i+1)] = pFrom[i];
			}
		}
	}
}



#endif

#if (defined _DEBUG || defined UNIT_TEST || defined IRWAS_TEST_CLIENT)
ViewImage *ViewImage::createSample()
{
	int w = 320;
	int h = 240;
	ViewImageInfo info(IMAGE_DATA_WINDOWS_BMP, COLORBIT_24, w, h);
	ViewImage *img = new ViewImage(info);
	int len = img->getBufferLength();
	char *buf = img->getBuffer();

	char b = 0;
	char g = 0;
	char *p = buf;

	// gradation : black(bottom) -> red(top)
	for (int i=0; i<h; i++) {
		char r = i;
		for (int j=0; j<w; j++) {
			*p = b; p++;
			*p = g; p++;
			*p = r; p++;
		}
	}
	return img;
}


#endif


#ifdef ViewImage_test
#include <string.h>

int main()
{
	ViewImage *img = ViewImage::createSample();
	img->saveAsWindowsBMP("foo.bmp");
	return 0;
}

#endif

