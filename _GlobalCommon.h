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

/*
@brief 图像插值
@param pBmpFileBuf 位图文件缓冲区
@param newWidth 新的宽度
@param newHeight 新的高度
@param nMethod 插值方法
	-0: 最近邻插值
	-1: 双线性插值
@return 处理后的位图文件缓冲区
*/
char    *ImageInterpolation(char *pBmpFileBuf,int newWidth,int newHeight,int nMethod=0);
/*
@brief 创建高斯核
@param kernelSize 核大小
@param sigma 标准差
@return 高斯核
*/
double* CreateGaussianKernel(int kernelSize=5, double sigma=1.0);
/*
@brief 高斯平滑
@param pBmpFileBuf 位图文件缓冲区
@param kernelSize 核大小
@param sigma 标准差
*/
char    *GaussianSmooth(char* pBmpFileBuf, int kernelSize = 5, double sigma=1.0);
/*
@brief 中值滤波
@param pBmpFileBuf 位图文件缓冲区
@param kernelSize 核大小
*/
char    *MedianFilter(char* pBmpFileBuf, int kernelSize = 3);
/*
@brief 双边插值
@param pBmpFileBuf 位图文件缓冲区
@return 处理后的位图文件缓冲区
*/
char    * Bilateralfilter(char* pBmpFileBuf);
/*
@brief 直方图均衡化
@param pBmpFileBuf 位图文件缓冲区
@return 处理后的位图文件缓冲区
*/
char    *Histoequalization(char* pBmpFileBuf);
/*
@brief 梯度锐化
@param pBmpFileBuf 位图文件缓冲区
@return 处理后的位图文件缓冲区
*/
char    *Sharpengrad(char* pBmpFileBuf);
/*
@brief Canny边缘检测
@param pBmpFileBuf 位图文件缓冲区
@return 处理后的位图文件缓冲区
*/
char    *Cannyedge(char* pBmpFileBuf);
/*
@brief OTSU分割
@param pBmpFileBuf 位图文件缓冲区
@return 处理后的位图文件缓冲区
*/
char    *Otsusegment(char* pBmpFileBuf);