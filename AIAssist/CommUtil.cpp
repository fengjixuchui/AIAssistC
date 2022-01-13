#include "pch.h"
#include "CommUtil.h"
#include "AssistWorker.h"
#include "opencv2/opencv.hpp"
#include<opencv2/imgproc.hpp>

using namespace cv;



UINT CommUtil::winSizeType = SIZE_MINIMIZED;

// MFC��Picture Control�ؼ���ʾͼ��
// src ����ʾ��ͼ�� (OpenCV��Mat����ṹ)
// hwnd MFC��ʾͼ��Ĵ��ھ��
void ShowImage(Mat& imgSrc, HWND hDlg, int IDC_IMG) {

	HWND hwnd = GetDlgItem(hDlg, IDC_IMG);

	CRect rect;
	//��ȡͼƬ�ؼ����ο�
	GetClientRect(hwnd, &rect);
	//����Mat������ӦͼƬ�ؼ���С
	//cv::resize(imgSrc, imgSrc, cv::Size(rect.Width(), rect.Height()));
	//cv::resize(imgSrc, imgSrc, cv::Size(300, 300));
	// 
	// ת����ʽ ,���ڻ�ȡBITMAPINFO
	Mat imgDst;
	switch (imgSrc.channels())
	{
	case 1:
		cv::cvtColor(imgSrc, imgDst, COLOR_GRAY2BGRA); // GRAY��ͨ��
		break;
	case 3:
		cv::cvtColor(imgSrc, imgDst, COLOR_BGR2BGRA);  // BGR��ͨ��
		break;
	default:
		break;
	}

	// ����һ�����ض��ٸ��ֽ�
	int pixelBytes = imgDst.channels() * (imgDst.depth() + 1);

	int width = (imgDst.cols < rect.Width()) ? imgDst.cols : rect.Width();
	int height = (imgDst.rows < rect.Height()) ? imgDst.rows : rect.Height();

	// ����bitmapinfo(����ͷ)
	BITMAPINFO bitInfo;
	bitInfo.bmiHeader.biBitCount = 8 * pixelBytes;
	bitInfo.bmiHeader.biWidth = width;
	bitInfo.bmiHeader.biHeight = -height;   //ע��"-"��(����ʱ���Ż���)
	bitInfo.bmiHeader.biPlanes = 1;
	bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo.bmiHeader.biCompression = BI_RGB;
	bitInfo.bmiHeader.biClrImportant = 0;
	bitInfo.bmiHeader.biClrUsed = 0;
	bitInfo.bmiHeader.biSizeImage = 0;
	bitInfo.bmiHeader.biXPelsPerMeter = 0;
	bitInfo.bmiHeader.biYPelsPerMeter = 0;

	HDC hDC = GetDC(hwnd);  //��ȡͼƬ�ؼ�DC

	//��ͼ
	::StretchDIBits(
		hDC,
		0, 0, width, height,
		0, 0, width, height,
		imgDst.data,
		&bitInfo,
		DIB_RGB_COLORS,
		SRCCOPY
	);

	imgDst.release();
	imgDst = NULL;

	ReleaseDC(hwnd, hDC);  //�ͷ�DC
}


//��ͼ�̺߳���
UINT DrawThreadProc(LPVOID pParam)
{
	DRAWTHREADPARAM* param = (DRAWTHREADPARAM*)pParam;

	if (param != NULL) {
		int IDC_IMG = param->idcImg;
		HWND hDlg = param->hDlg;
		AssistWorker* pAssistWorker = param->assistWorker;
		AssistConfig* pAssistConfig = param->assistConfig;
		while (true)
		{
			if (pAssistConfig->detectImg && CommUtil::winSizeType!= SIZE_MINIMIZED) {
				Mat imgSrc = pAssistWorker->PopDrawMat();
				ShowImage(imgSrc, hDlg, IDC_IMG);

				//�ֹ�����clone��mat����
				imgSrc.release();
				imgSrc = NULL;
			}
			else {
				Sleep(500);
			}
		}
		return 0;
	}
	else {
		return 1;
	}

	return 0;   // thread completed successfully
}