//GlobalCommon.cpp
#define _CRT_SECURE_NO_WARNINGS
#include "_GlobalCommon.h"
#include <vector>
#include <algorithm>
/*
@brief 打开位图文件
@param strBmpFile 位图文件路径
*/
char *OpenBMPfile(CString strBmpFile)
{
	CFile hFile;
	if( !hFile.Open(strBmpFile,CFile::modeRead|CFile::typeBinary) )
	{
		AfxMessageBox("Failed to open the BMP file");
		return( NULL );
	}
	long lFileSize = (long)hFile.Seek(0L, CFile::end);
	char *pFileBuf = new char [lFileSize+1];
	hFile.Seek(0L, CFile::begin);
	hFile.Read(pFileBuf, lFileSize);
	hFile.Close();
	BITMAPFILEHEADER *pBmpHead = (BITMAPFILEHEADER *)pFileBuf;
	BITMAPINFOHEADER *pBmpInfo = (BITMAPINFOHEADER *)(pFileBuf + sizeof(BITMAPFILEHEADER));
	if(	pBmpHead->bfType   != 0x4D42 ||		//"BM"=0x424D
	    pBmpInfo->biSize   != 0x28   ||		// 位图信息子结构长度(等于40,即0x28)
		pBmpInfo->biPlanes != 0x01 )		// 此域必须等于1
	{
		AfxMessageBox("It isn't a valid BMP file");
		return( NULL );
	}
	if( pBmpInfo->biCompression != BI_RGB )
	{
		AfxMessageBox("It is a compressed BMP file");
		return( NULL );
	}
	if( pBmpInfo->biBitCount != 8  &&
	    pBmpInfo->biBitCount != 24 )
	{
		AfxMessageBox("Only 8-bit and 24-bit BMP files are supported");
		return( NULL );
	}
	return( pFileBuf );
}

///////////////////////////////////////////////////////////////////////	
///////////////////////////////////////////////////////////////////////	

BITMAPFILEHEADER *GetDIBHEADER(char *pFileBuffer)
{
	char *p = pFileBuffer + 0;
	return( (BITMAPFILEHEADER *)p );
}

BITMAPINFOHEADER *GetDIBINFO(char *pFileBuffer)
{
	char *p = pFileBuffer + sizeof(BITMAPFILEHEADER);
	return( (BITMAPINFOHEADER *)p );
}

char *GetDIBImageData(char *pFileBuffer)
{
	const BITMAPFILEHEADER *pBmpHead = GetDIBHEADER(pFileBuffer);
	char *p = pFileBuffer + pBmpHead->bfOffBits;
	return( p );
}

//return NULL denoting no palette
RGBQUAD *GetDIBPaletteData(char *pFileBuffer,int nEntryNumber[1])
{
	char *pPaletteData = NULL;
	if( GetColorBits(pFileBuffer) <= 8 )
	{
		nEntryNumber[0] = 0;
		char *pDIBImageData = GetDIBImageData(pFileBuffer);
		pPaletteData = pFileBuffer + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		int  nNum = (DWORD)(pDIBImageData - pPaletteData) / sizeof(RGBQUAD);
		int  iUsedColors = (int)GetDIBINFO(pFileBuffer)->biClrUsed;
		if( nNum > 0 && (int)iUsedColors > 0 )
			nEntryNumber[0] = min(nNum,(int)iUsedColors);
		else
			pPaletteData = NULL;
	}
	return( (RGBQUAD *)pPaletteData );
}

int GetImageWidth(char *pFileBuffer)
{
	BITMAPINFOHEADER *pInfo = GetDIBINFO(pFileBuffer);
	return( pInfo->biWidth );
}

int GetImageHeight(char *pFileBuffer)
{
	BITMAPINFOHEADER *pInfo = GetDIBINFO(pFileBuffer);
	return( pInfo->biHeight );
}

int GetColorBits(char *pFileBuffer)
{
	BITMAPINFOHEADER *pInfo = GetDIBINFO(pFileBuffer);
	return( pInfo->biBitCount );
}

long GetUsedColors(char *pFileBuffer)
{
	BITMAPINFOHEADER *pInfo = GetDIBINFO(pFileBuffer);
	return( (long)pInfo->biClrUsed );
}

long GetWidthBytes(char *pFileBuffer)
{
	BITMAPINFOHEADER *pInfo = GetDIBINFO(pFileBuffer);
	long nBytesPerRow = 4 * ((pInfo->biWidth * pInfo->biBitCount + 31) / 32);
	return( nBytesPerRow );
}

///////////////////////////////////////////////////////////////////////	
///////////////////////////////////////////////////////////////////////	

void DisplayHeaderMessage(char *pBmpFileBuf)
{
	BITMAPFILEHEADER *pBmpHead = GetDIBHEADER(pBmpFileBuf);
	BITMAPINFOHEADER *pBmpInfo = GetDIBINFO(pBmpFileBuf);
	/**/
	char msg[4096];
	sprintf(msg,
	"bfType (file type) = %4.4X \n"
	"bfSize (file length) = %ld \n"
	"bfOffBits (offset of bitmap data in bytes) = %ld \n"
	"biSize (header structure length should be 40 or 0x28) = %ld \n"
	"biWidth (image width)  = %ld \n"
	"biHeight (image height) = %ld \n"
	"biPlanes (must be eaual to 1) = %u \n"
	"biBitCount (color/pixel bits) = %u \n"
	"biCompression (compressed?) = %ld \n"
	"biSizeImage (length of bitmap data in bytes must be the times of 4) = %ld \n"
	"biXPelsPerMeter (horizontal resolution of target device in pixels/metre) = %ld \n"
	"biYPelsPerMeter (vertical resolution of target device in pixels/metre) = %ld \n"
	"biColorUsed (number of colors used in bitmap,0=2**biBitCount) = %ld \n"
	"biColorImportant (number of important colors,0=all colors are important) = %ld \n\n"
	"The following is additional information: \n"
	"Bytes per row in bitmap (nBytesPerRow) = %ld \n"
	"Total bytes of bitmap (nImageSizeInByte) = %ld \n"
	"Actual pixels per row in bitmap (nPixelsPerRow) = %ld \n"
	"Total rows of bitmap (nTotalRows) = %ld \n"
	"Total colors (2**biBitCount)(nTotalColors) = %ld \n"
	"Used colors (biColorUsed)(nUsedColors) = %ld ",
	pBmpHead->bfType,
	pBmpHead->bfSize,
	pBmpHead->bfOffBits,
	pBmpInfo->biSize,
	pBmpInfo->biWidth,
	pBmpInfo->biHeight,
	pBmpInfo->biPlanes,
	pBmpInfo->biBitCount,
	pBmpInfo->biCompression,
	pBmpInfo->biSizeImage,
	pBmpInfo->biXPelsPerMeter,
	pBmpInfo->biYPelsPerMeter,
	pBmpInfo->biClrUsed,
	pBmpInfo->biClrImportant,
	GetWidthBytes(pBmpFileBuf),
	GetWidthBytes(pBmpFileBuf) * GetImageHeight(pBmpFileBuf),
	GetImageWidth(pBmpFileBuf),
	GetImageHeight(pBmpFileBuf),
	1 << GetColorBits(pBmpFileBuf),
	GetUsedColors(pBmpFileBuf) );
	AfxMessageBox(msg);
}

//Mode = 0, normal display
//   1,2,3, display grayscale image in red, green, blue colors
void DisplayImage(CDC *pDC,char *pBmpFileBuf,int disp_xL,int disp_yL,int disp_Width,int disp_Height,int mode)
{
	ASSERT( pDC != NULL );
	HDC hDC = pDC->GetSafeHdc();
	ASSERT( hDC != 0 );
	/**/
	int imageWidth  = GetImageWidth(pBmpFileBuf);
	int imageHeight = GetImageHeight(pBmpFileBuf);
	if( disp_Width <= 0 || disp_Height <= 0 )
	{
		disp_Width  = imageWidth;
		disp_Height = imageHeight;
	}
	CRect rect;
	CWnd *pWnd = pDC->GetWindow();
	pWnd->GetClientRect(&rect);
	disp_Width = min(disp_Width, rect.right - disp_xL);
	disp_Height = min(disp_Height, rect.bottom - disp_yL);
	/**/
	BITMAPINFOHEADER *pBitmapInfo = GetDIBINFO(pBmpFileBuf);
	char *pDIBImageData = GetDIBImageData(pBmpFileBuf);
	/**/
	char buf[40+256*4];
	BITMAPINFO *pBitsInfo = (BITMAPINFO *)buf;
	memcpy(&pBitsInfo->bmiHeader,pBitmapInfo,sizeof(BITMAPINFOHEADER));
	/**/
	int palleteNum = 0;
	RGBQUAD *pallete = GetDIBPaletteData(pBmpFileBuf,&palleteNum);
	for(int c = 0; c < 256; c++)
	{
		if( mode == 0 )
		{
			(pBitsInfo->bmiColors[c]).rgbRed   = (pallete!=NULL && c<palleteNum? pallete[c].rgbRed   : c);
			(pBitsInfo->bmiColors[c]).rgbGreen = (pallete!=NULL && c<palleteNum? pallete[c].rgbGreen : c);
			(pBitsInfo->bmiColors[c]).rgbBlue  = (pallete!=NULL && c<palleteNum? pallete[c].rgbBlue  : c);
		}
		else
		{
			(pBitsInfo->bmiColors[c]).rgbRed   = (mode==1? c : 0); 
			(pBitsInfo->bmiColors[c]).rgbGreen = (mode==2? c : 0); 
			(pBitsInfo->bmiColors[c]).rgbBlue  = (mode==3? c : 0); 
		}
	}
	/**/
	SetStretchBltMode(hDC,COLORONCOLOR);
	StretchDIBits(hDC,disp_xL,disp_yL,disp_Width,disp_Height,
	0,0,imageWidth,imageHeight,pDIBImageData,pBitsInfo,DIB_RGB_COLORS,SRCCOPY );
	/**/
	return;
}

///////////////////////////////////////////////////////////////////////	
///////////////////////////////////////////////////////////////////////	
//   象素操作

//	 读象素颜色值
//	 返回: >=0 表示象素在位图数据中的偏移值
//		    <0 失败或参数无效
long GetPixel(char *pFileBuffer,int x,int y,RGBQUAD rgb[1],bool bGray[1])
{
	int  nColorBits   = GetColorBits(pFileBuffer);
	int  nImageHeight = GetImageHeight(pFileBuffer);
	int  nBytesPerRow = GetWidthBytes(pFileBuffer);
	/**/
	long nOffInImage  = (nImageHeight-1-y) * nBytesPerRow;
	char *p = GetDIBImageData(pFileBuffer) + nOffInImage;
	/**/
	if( bGray != NULL ) *bGray = true;
	if( nColorBits == 8 )
	{
		nOffInImage += x;
		rgb[0].rgbReserved = p[x];
		rgb[0].rgbRed      = p[x];
		rgb[0].rgbGreen    = p[x];
		rgb[0].rgbBlue     = p[x];
	}
	else if( nColorBits == 24 )
	{
		if( bGray != NULL ) *bGray = false;
		nOffInImage += 3 * x;
		p += (3 * x);
		rgb[0].rgbReserved = 0;
		rgb[0].rgbRed      = p[2];
		rgb[0].rgbGreen    = p[1];
		rgb[0].rgbBlue     = p[0];
	}
	else
	{
		AfxMessageBox("It is not an 8-bit or 24-bit image");
		return( -1L );
	}
	/**/
	return( nOffInImage );
}

//  设置像素(x,y)的颜色值
void SetPixel(char *pFileBuffer,int x,int y,RGBQUAD rgb)
{
	int  nColorBits   = GetColorBits(pFileBuffer);
	int  nImageHeight = GetImageHeight(pFileBuffer);
	int  nBytesPerRow = GetWidthBytes(pFileBuffer);
	/**/
	long nOffInImage  = (nImageHeight-1-y) * nBytesPerRow;
	char *p = GetDIBImageData(pFileBuffer) + nOffInImage;
	/**/
	if( nColorBits == 8 )
	{
		 p[ x ] = rgb.rgbReserved;
	}
	else if( nColorBits == 24 )
	{
		p += (3 * x);
		p[0] = rgb.rgbBlue;
		p[1] = rgb.rgbGreen;
		p[2] = rgb.rgbRed;
	}
	else
	{
		AfxMessageBox("It is not an 8-bit or 24-bit image");
	}
	/**/
	return;
}

///////////////////////////////////////////////////////////////////////	
///////////////////////////////////////////////////////////////////////	
// 保存为BMP文件

BOOL SaveDIB(char *pFileBuffer,CString strBmpFile)
{
	CFile hFile;
	if( !hFile.Open(strBmpFile,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary) )
	{
		AfxMessageBox("Failed to create the BMP file");
		return( FALSE );
	}
	/**/
	BITMAPFILEHEADER *pBmpHead = (BITMAPFILEHEADER *)pFileBuffer;
	long lFileSize = pBmpHead->bfSize;
	hFile.Write(pFileBuffer,lFileSize);
	hFile.Close();
	return( TRUE );
}

///////////////////////////////////////////////////////////////////////	
///////////////////////////////////////////////////////////////////////	
// 图像插值

/**
	 功能: 图像插值
		   nMethod  插值算法
					0 = 最临近插值法
					1 = (双)线性插值法
	 返回: 新图像的BMP文件缓冲区首地址
           NULL 表示失败（内存不足）
**/
char *ImageInterpolation(char *pBmpFileBuf,int newWidth,int newHeight,int nMethod)
{
	BITMAPFILEHEADER *pFileHeader = (BITMAPFILEHEADER *)pBmpFileBuf;
	BITMAPINFOHEADER *pDIBInfo = (BITMAPINFOHEADER *)(pBmpFileBuf + sizeof(BITMAPFILEHEADER));
//	char *pDIBData = pBmpFileBuf + pFileHeader->bfOffBits;
	int  orgWidth  = pDIBInfo->biWidth;
	int  orgHeight = pDIBInfo->biHeight;
	int  colorBits = pDIBInfo->biBitCount;
	/**/
	long bytesPerRow = 4 * ((newWidth * colorBits + 31) / 32);
	long newBmpFileSize = pFileHeader->bfOffBits + bytesPerRow * newHeight;
	char *pNewBmpFileBuf = new char [newBmpFileSize];
	memcpy(pNewBmpFileBuf, pBmpFileBuf, pFileHeader->bfOffBits);
	BITMAPFILEHEADER *pNewFileHeader = (BITMAPFILEHEADER *)pNewBmpFileBuf;
	BITMAPINFOHEADER *pNewDIBInfo = (BITMAPINFOHEADER *)(pNewBmpFileBuf + sizeof(BITMAPFILEHEADER));
	pNewFileHeader->bfSize = newBmpFileSize;
	pNewDIBInfo->biWidth = newWidth;
	pNewDIBInfo->biHeight = newHeight;
	pNewDIBInfo->biSizeImage = bytesPerRow * newHeight;
//	char *pNewDIBData = pNewBmpFileBuf + pFileHeader->bfOffBits;
	/**/
	/**/
	float xScale  = (float)orgWidth  / (float)newWidth;
	float yScale  = (float)orgHeight / (float)newHeight;
	for(int y = 0; y < newHeight; y++)
	{
		float fy = y * yScale;
		for(int x = 0; x < newWidth; x++)
		{
			RGBQUAD rgb;
			float fx = x * xScale;
			if( nMethod == 0 )		//最临近插值法
			{
				int xx = min( (int)(fx+0.5), orgWidth - 1 );
				int yy = min( (int)(fy+0.5), orgHeight - 1 );
				GetPixel(pBmpFileBuf, xx, yy, &rgb);
			}
			else
			{						//(双)线性插值法
				RGBQUAD rgbLT,rgbRT,rgbLB,rgbRB;
				int   x1 = (int)fx;
				int   x2 = min(x1+1, orgWidth-1);
				float dx = fx - (float)x1;
				int   y1 = (int)fy;
				int   y2 = min(y1+1, orgHeight-1);
				float dy = fy - (float)y1;
				GetPixel(pBmpFileBuf, x1, y1, &rgbLT);
				GetPixel(pBmpFileBuf, x2, y1, &rgbRT);
				GetPixel(pBmpFileBuf, x1, y2, &rgbLB);
				GetPixel(pBmpFileBuf, x2, y2, &rgbRB);
				for(int N = 0; N < 4; N++)
				{
					float v1 = ((BYTE *)&rgbLT)[N] + dy * (((BYTE *)&rgbLB)[N] - ((BYTE *)&rgbLT)[N]);
					float v2 = ((BYTE *)&rgbRT)[N] + dy * (((BYTE *)&rgbRB)[N] - ((BYTE *)&rgbRT)[N]);
					((BYTE *)&rgb)[N] = (int)(v1 + dx * (v2 - v1) + 0.5);
				}
			}
			SetPixel(pNewBmpFileBuf, x, y, rgb);
		}
	}
	/**/
	return( pNewBmpFileBuf );
}

double* CreateGaussianKernel(int kernelSize, double sigma)
{
	int kernelRadius = kernelSize / 2;
	double* kernel = new double[kernelSize * kernelSize];
	double sum = 0.0;

	for (int y = -kernelRadius; y <= kernelRadius; y++)
	{
		for (int x = -kernelRadius; x <= kernelRadius; x++)
		{
			double value = exp(-(x * x + y * y) / (2 * sigma * sigma));
			kernel[(y + kernelRadius) * kernelSize + (x + kernelRadius)] = value;
			sum += value;
		}
	}

	// Normalize the kernel
	for (int i = 0; i < kernelSize * kernelSize; i++)
	{
		kernel[i] /= sum;
	}

	return kernel;
}

	char* GaussianSmooth(char* pBmpFileBuf, int kernelSize, double sigma)
	{
		BITMAPFILEHEADER* pFileHeader = (BITMAPFILEHEADER*)pBmpFileBuf;
		BITMAPINFOHEADER* pDIBInfo = (BITMAPINFOHEADER*)(pBmpFileBuf + sizeof(BITMAPFILEHEADER));
		int orgWidth = pDIBInfo->biWidth;
		int orgHeight = pDIBInfo->biHeight;
		int colorBits = pDIBInfo->biBitCount;
		long bytesPerRow = 4 * ((orgWidth * colorBits + 31) / 32);
		long newBmpFileSize = pFileHeader->bfOffBits + bytesPerRow * orgHeight;
		char* pNewBmpFileBuf = new char[newBmpFileSize];
		memcpy(pNewBmpFileBuf, pBmpFileBuf, pFileHeader->bfOffBits);
		BITMAPFILEHEADER* pNewFileHeader = (BITMAPFILEHEADER*)pNewBmpFileBuf;
		BITMAPINFOHEADER* pNewDIBInfo = (BITMAPINFOHEADER*)(pNewBmpFileBuf + sizeof(BITMAPFILEHEADER));
		pNewFileHeader->bfSize = newBmpFileSize;
		pNewDIBInfo->biWidth = orgWidth;
		pNewDIBInfo->biHeight = orgHeight;
		pNewDIBInfo->biSizeImage = bytesPerRow * orgHeight;

		int kernelRadius = kernelSize / 2;
		double* kernel = CreateGaussianKernel(kernelSize, sigma);

		for (int y = 0; y < orgHeight; y++)
		{
			for (int x = 0; x < orgWidth; x++)
			{
				RGBQUAD rgb = { 0, 0, 0, 0 };
				double sum = 0.0;
				double weightSum = 0.0;

				for (int ky = -kernelRadius; ky <= kernelRadius; ky++)
				{
					for (int kx = -kernelRadius; kx <= kernelRadius; kx++)
					{
						int srcX = max(0, min(orgWidth - 1, x + kx));
						int srcY = max(0, min(orgHeight - 1, y + ky));
						int kernelIndex = (ky + kernelRadius) * kernelSize + (kx + kernelRadius);
						double weight = kernel[kernelIndex];

						RGBQUAD srcRGB;
						GetPixel(pBmpFileBuf, srcX, srcY, &srcRGB);

						rgb.rgbBlue += static_cast<BYTE>(weight * srcRGB.rgbBlue);
						rgb.rgbGreen += static_cast<BYTE>(weight * srcRGB.rgbGreen);
						rgb.rgbRed += static_cast<BYTE>(weight * srcRGB.rgbRed);

						weightSum += weight;
					}
				}

				rgb.rgbBlue = static_cast<BYTE>(rgb.rgbBlue / weightSum);
				rgb.rgbGreen = static_cast<BYTE>(rgb.rgbGreen / weightSum);
				rgb.rgbRed = static_cast<BYTE>(rgb.rgbRed / weightSum);

				SetPixel(pNewBmpFileBuf, x, y, rgb);
			}
		}

		delete[] kernel;
		return pNewBmpFileBuf;
	}

char* MedianFilter(char* pBmpFileBuf, int kernelSize)
{
	BITMAPFILEHEADER* pFileHeader = (BITMAPFILEHEADER*)pBmpFileBuf;
	BITMAPINFOHEADER* pDIBInfo = (BITMAPINFOHEADER*)(pBmpFileBuf + sizeof(BITMAPFILEHEADER));
	int orgWidth = pDIBInfo->biWidth;
	int orgHeight = pDIBInfo->biHeight;
	int colorBits = pDIBInfo->biBitCount;

	long bytesPerRow = 4 * ((orgWidth * colorBits + 31) / 32);
	long newBmpFileSize = pFileHeader->bfOffBits + bytesPerRow * orgHeight;
	char* pNewBmpFileBuf = new char[newBmpFileSize];
	memcpy(pNewBmpFileBuf, pBmpFileBuf, pFileHeader->bfOffBits);
	BITMAPFILEHEADER* pNewFileHeader = (BITMAPFILEHEADER*)pNewBmpFileBuf;
	BITMAPINFOHEADER* pNewDIBInfo = (BITMAPINFOHEADER*)(pNewBmpFileBuf + sizeof(BITMAPFILEHEADER));
	pNewFileHeader->bfSize = newBmpFileSize;
	pNewDIBInfo->biWidth = orgWidth;
	pNewDIBInfo->biHeight = orgHeight;
	pNewDIBInfo->biSizeImage = bytesPerRow * orgHeight;

	int kernelRadius = kernelSize / 2;
	for (int y = 0; y < orgHeight; y++)
	{
		for (int x = 0; x < orgWidth; x++)
		{
			std::vector <RGBQUAD> neighbors;
			for (int ky = -kernelRadius; ky <= kernelRadius; ky++)
			{
				for (int kx = -kernelRadius; kx <= kernelRadius; kx++)
				{
					int srcX = max(0, min(orgWidth - 1, x + kx));
					int srcY = max(0, min(orgHeight - 1, y + ky));
					RGBQUAD rgb;
					GetPixel(pBmpFileBuf, srcX, srcY, &rgb);
					neighbors.push_back(rgb);
				}
			}

			sort(neighbors.begin(), neighbors.end(), [](const RGBQUAD& a, const RGBQUAD& b) {
				return a.rgbBlue < b.rgbBlue || (a.rgbBlue == b.rgbBlue && a.rgbGreen < b.rgbGreen) || (a.rgbBlue == b.rgbBlue && a.rgbGreen == b.rgbGreen && a.rgbRed < b.rgbRed);
				});

			int medianIndex = (int)neighbors.size() / 2;
			RGBQUAD medianRGB = neighbors[medianIndex];
			SetPixel(pNewBmpFileBuf, x, y, medianRGB);
		}
	}
	return pNewBmpFileBuf;
}

char* Bilateralfilter(char* pBmpFileBuf)
{
	BITMAPFILEHEADER* pFileHeader = (BITMAPFILEHEADER*)pBmpFileBuf;
	BITMAPINFOHEADER* pDIBInfo = (BITMAPINFOHEADER*)(pBmpFileBuf + sizeof(BITMAPFILEHEADER));
	int orgWidth = pDIBInfo->biWidth;
	int orgHeight = pDIBInfo->biHeight;
	int colorBits = pDIBInfo->biBitCount;
	long bytesPerRow = 4 * ((orgWidth * colorBits + 31) / 32);
	long newBmpFileSize = pFileHeader->bfOffBits + bytesPerRow * orgHeight;
	char* pNewBmpFileBuf = new char[newBmpFileSize];
	memcpy(pNewBmpFileBuf, pBmpFileBuf, pFileHeader->bfOffBits);
	BITMAPFILEHEADER* pNewFileHeader = (BITMAPFILEHEADER*)pNewBmpFileBuf;
	BITMAPINFOHEADER* pNewDIBInfo = (BITMAPINFOHEADER*)(pNewBmpFileBuf + sizeof(BITMAPFILEHEADER));
	pNewFileHeader->bfSize = newBmpFileSize;
	pNewDIBInfo->biWidth = orgWidth;
	pNewDIBInfo->biHeight = orgHeight;
	pNewDIBInfo->biSizeImage = bytesPerRow * orgHeight;


	return nullptr;
}

char* Histoequalization(char* pBmpFileBuf)
{
	BITMAPFILEHEADER* pFileHeader = (BITMAPFILEHEADER*)pBmpFileBuf;
	BITMAPINFOHEADER* pDIBInfo = (BITMAPINFOHEADER*)(pBmpFileBuf + sizeof(BITMAPFILEHEADER));
	int orgWidth = pDIBInfo->biWidth;
	int orgHeight = pDIBInfo->biHeight;
	int colorBits = pDIBInfo->biBitCount;

	if (colorBits != 24 && colorBits != 32)
	{
		// Only support 24-bit and 32-bit BMP
		return nullptr;
	}

	long bytesPerRow = 4 * ((orgWidth * colorBits + 31) / 32);
	long bmpFileSize = pFileHeader->bfOffBits + bytesPerRow * orgHeight;
	char* pNewBmpFileBuf = new char[bmpFileSize];
	memcpy(pNewBmpFileBuf, pBmpFileBuf, bmpFileSize);

	// Calculate histogram
	int histogram[256] = { 0 };
	for (int y = 0; y < orgHeight; y++)
	{
		for (int x = 0; x < orgWidth; x++)
		{
			RGBQUAD rgb;
			GetPixel(pBmpFileBuf, x, y, &rgb);
			int gray = (rgb.rgbRed + rgb.rgbGreen + rgb.rgbBlue) / 3;
			histogram[gray]++;
		}
	}

	// Calculate cumulative distribution function (CDF)
	int cdf[256] = { 0 };
	cdf[0] = histogram[0];
	for (int i = 1; i < 256; i++)
	{
		cdf[i] = cdf[i - 1] + histogram[i];
	}

	// Normalize the CDF
	int cdfMin = 0;
	for (int i = 0; i < 256; i++)
	{
		if (cdf[i] != 0)
		{
			cdfMin = cdf[i];
			break;
		}
	}

	int totalPixels = orgWidth * orgHeight;
	int lookupTable[256];
	for (int i = 0; i < 256; i++)
	{
		lookupTable[i] = (cdf[i] - cdfMin) * 255 / (totalPixels - cdfMin);
		lookupTable[i] = max(0, min(255, lookupTable[i]));
	}

	// Apply histogram equalization
	for (int y = 0; y < orgHeight; y++)
	{
		for (int x = 0; x < orgWidth; x++)
		{
			RGBQUAD rgb;
			GetPixel(pBmpFileBuf, x, y, &rgb);
			int gray = (rgb.rgbRed + rgb.rgbGreen + rgb.rgbBlue) / 3;
			int equalizedGray = lookupTable[gray];
			rgb.rgbRed = rgb.rgbGreen = rgb.rgbBlue = equalizedGray;
			SetPixel(pNewBmpFileBuf, x, y, rgb);
		}
	}

	return pNewBmpFileBuf;
}


// Gradient sharpening function
char* Sharpengrad(char* pBmpFileBuf)
{
	BITMAPFILEHEADER* pFileHeader = (BITMAPFILEHEADER*)pBmpFileBuf;
	BITMAPINFOHEADER* pDIBInfo = (BITMAPINFOHEADER*)(pBmpFileBuf + sizeof(BITMAPFILEHEADER));
	int orgWidth = pDIBInfo->biWidth;
	int orgHeight = pDIBInfo->biHeight;
	int colorBits = pDIBInfo->biBitCount;
	long bytesPerRow = 4 * ((orgWidth * colorBits + 31) / 32);
	long newBmpFileSize = pFileHeader->bfOffBits + bytesPerRow * orgHeight;
	char* pNewBmpFileBuf = new char[newBmpFileSize];
	memcpy(pNewBmpFileBuf, pBmpFileBuf, pFileHeader->bfOffBits);
	char* pOrgBmpFileBuf = new char[newBmpFileSize];
	memcpy(pOrgBmpFileBuf, pBmpFileBuf, newBmpFileSize);

	for (int y = 1; y < orgHeight - 1; y++)
	{
		for (int x = 1; x < orgWidth - 1; x++)
		{
			RGBQUAD rgbL, rgbR, rgbU, rgbD;
			GetPixel(pOrgBmpFileBuf, x - 1, y, &rgbL);
			GetPixel(pOrgBmpFileBuf, x + 1, y, &rgbR);
			GetPixel(pOrgBmpFileBuf, x, y - 1, &rgbU);
			GetPixel(pOrgBmpFileBuf, x, y + 1, &rgbD);

			RGBQUAD rgbNew;
			rgbNew.rgbBlue = min(255, max(0, abs((int)rgbR.rgbBlue - (int)rgbL.rgbBlue) + abs((int)rgbD.rgbBlue - (int)rgbU.rgbBlue)));
			rgbNew.rgbGreen = min(255, max(0, abs((int)rgbR.rgbGreen - (int)rgbL.rgbGreen) + abs((int)rgbD.rgbGreen - (int)rgbU.rgbGreen)));
			rgbNew.rgbRed = min(255, max(0, abs((int)rgbR.rgbRed - (int)rgbL.rgbRed) + abs((int)rgbD.rgbRed - (int)rgbU.rgbRed)));
			SetPixel(pNewBmpFileBuf, x, y, rgbNew);
		}
	}

	delete[] pOrgBmpFileBuf;
	return pNewBmpFileBuf;
}

char* Cannyedge(char* pBmpFileBuf) {
	BITMAPFILEHEADER* pFileHeader = (BITMAPFILEHEADER*)pBmpFileBuf;
	BITMAPINFOHEADER* pDIBInfo = (BITMAPINFOHEADER*)(pBmpFileBuf + sizeof(BITMAPFILEHEADER));
	int orgWidth = pDIBInfo->biWidth;
	int orgHeight = pDIBInfo->biHeight;
	int colorBits = pDIBInfo->biBitCount;
	long bytesPerRow = 4 * ((orgWidth * colorBits + 31) / 32);
	long newBmpFileSize = pFileHeader->bfOffBits + bytesPerRow * orgHeight;
	char* pNewBmpFileBuf = new char[newBmpFileSize];
	memcpy(pNewBmpFileBuf, pBmpFileBuf, pFileHeader->bfOffBits);
	char* pOrgBmpFileBuf = new char[newBmpFileSize];
	memcpy(pOrgBmpFileBuf, pBmpFileBuf, newBmpFileSize);

	return nullptr;
}

uint8_t RGBToGrayscale(RGBQUAD color) {
	return static_cast<uint8_t>(0.3 * color.rgbRed + 0.59 * color.rgbGreen + 0.11 * color.rgbBlue);
}

char* Otsusegment(char* pBmpFileBuf){
	BITMAPFILEHEADER* pFileHeader = (BITMAPFILEHEADER*)pBmpFileBuf;
	BITMAPINFOHEADER* pDIBInfo = (BITMAPINFOHEADER*)(pBmpFileBuf + sizeof(BITMAPFILEHEADER));
	int orgWidth = pDIBInfo->biWidth;
	int orgHeight = pDIBInfo->biHeight;
	int colorBits = pDIBInfo->biBitCount;
	long bytesPerRow = 4 * ((orgWidth * colorBits + 31) / 32);
	long newBmpFileSize = pFileHeader->bfOffBits + bytesPerRow * orgHeight;
	char* pNewBmpFileBuf = new char[newBmpFileSize];
	memcpy(pNewBmpFileBuf, pBmpFileBuf, pFileHeader->bfOffBits);
	char* pOrgBmpFileBuf = new char[newBmpFileSize];
	memcpy(pOrgBmpFileBuf, pBmpFileBuf, newBmpFileSize);
	std::vector<uint8_t> grayscale(orgWidth * orgHeight);

	// Convert image to grayscale
	for (int y = 0; y < orgHeight; ++y) {
		for (int x = 0; x < orgWidth; ++x) {
			RGBQUAD color;
			GetPixel(pBmpFileBuf, x, y, &color);
			grayscale[y * orgWidth + x] = RGBToGrayscale(color);
		}
	}

	// Calculate histogram
	int histogram[256] = { 0 };
	for (int i = 0; i < orgWidth * orgHeight; ++i) {
		histogram[grayscale[i]]++;
	}

	// Compute Otsu's threshold
	int total = orgWidth * orgHeight;
	float sum = 0;
	for (int t = 0; t < 256; ++t) sum += t * histogram[t];
	float sumB = 0, wB = 0, wF = 0, varMax = 0;
	int threshold = 0;

	for (int t = 0; t < 256; ++t) {
		wB += histogram[t];
		if (wB == 0) continue;
		wF = total - wB;
		if (wF == 0) break;

		sumB += t * histogram[t];
		float mB = sumB / wB;
		float mF = (sum - sumB) / wF;
		float varBetween = wB * wF * (mB - mF) * (mB - mF);

		if (varBetween > varMax) {
			varMax = varBetween;
			threshold = t;
		}
	}

	// Apply the threshold to segment the image
	for (int y = 0; y < orgHeight; ++y) {
		for (int x = 0; x < orgWidth; ++x) {
			uint8_t gray = grayscale[y * orgWidth + x];
			RGBQUAD color = { gray, gray, gray, 0 };
			if (gray > threshold) {
				color.rgbBlue = color.rgbGreen = color.rgbRed = 255; // White
			}
			else {
				color.rgbBlue = color.rgbGreen = color.rgbRed = 0; // Black
			}
			SetPixel(pNewBmpFileBuf, x, y, color);
		}
	}

	return pNewBmpFileBuf;
}
