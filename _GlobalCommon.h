//GlobalCommon.h
#include "afxwin.h"

char    *OpenBMPfile(CString strBmpFile);

BITMAPFILEHEADER *GetDIBHEADER(char *pFileBuffer);
BITMAPINFOHEADER *GetDIBINFO(char *pFileBuffer);
char    *GetDIBImageData(char *pFileBuffer);
RGBQUAD *GetDIBPaletteData(char *pFileBuffer,int nEntryNumber[1]);
int     GetImageWidth(char *pFileBuffer);
int     GetImageHeight(char *pFileBuffer);
int     GetColorBits(char *pFileBuffer);
long    GetUsedColors(char *pFileBuffer);
long    GetWidthBytes(char *pFileBuffer);

void    DisplayHeaderMessage(char *pBmpFileBuf);
void	DisplayImage(CDC *pDC,char *pBmpFileBuf,
		int disp_xL=0,int disp_yL=0,int disp_Width=0,int disp_Height=0,int mode=0 );

long    GetPixel(char *pFileBuffer,int x,int y,RGBQUAD rgb[1],bool bGray[1]=NULL);
void    SetPixel(char *pFileBuffer,int x,int y,RGBQUAD rgb);

BOOL    SaveDIB(char *pFileBuffer,CString strBmpFile);

char    *ImageInterpolation(char *pBmpFileBuf,int newWidth,int newHeight,int nMethod=0);

double* CreateGaussianKernel(int kernelSize=5, double sigma=1.0);
char    *GaussianSmooth(char* pBmpFileBuf, int kernelSize = 5, double sigma=1.0);
char    *MedianFilter(char* pBmpFileBuf, int kernelSize = 3);
char    * Bilateralfilter(char* pBmpFileBuf);
char    *Histoequalization(char* pBmpFileBuf);
char    *Sharpengrad(char* pBmpFileBuf);
char    *Cannyedge(char* pBmpFileBuf);
char    *Otsusegment(char* pBmpFileBuf);