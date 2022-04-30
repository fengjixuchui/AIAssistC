#pragma once
#include <string>
#include "AssistWorker.h"
#include "opencv2/opencv.hpp"
#include<opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

class CommUtil
{
public:
    static string CStringToString(CString cs) {
#ifdef _UNICODE

        //�����unicode����
        USES_CONVERSION;
        std::string str(W2A(cs));
        return str;
#else
        //����Ƕ��ֽڹ��� 
        std::string str(cs.GetBuffer());
        cs.ReleaseBuffer();
        return str;

#endif // _UNICODE 
    }

    static CString StringToCString(string str) {
#ifdef _UNICODE
        //�����unicode����
        USES_CONVERSION;
        CString ans(str.c_str());
        return ans;
#else
        //����Ƕ��ֽڹ��� 
        CString ans;
        ans.Format("%s", str.c_str());
        return ans;
#endif // _UNICODE  
    }

    static LPCWSTR stringToLPCWSTR(std::string orig)
    {
        size_t origsize = orig.length() + 1;
        const size_t newsize = 100;
        size_t convertedChars = 0;
        wchar_t* wcstring = (wchar_t*)malloc(sizeof(wchar_t) * (orig.length() - 1));
        mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);

        return wcstring;
    }

    static string WCharToMByte(LPCWSTR lpcwszStr)
    {
        string str;
        DWORD dwMinSize = 0;
        LPSTR lpszStr = NULL;
        dwMinSize = WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, NULL, 0, NULL, FALSE);
        if (0 == dwMinSize)
        {
            return FALSE;
        }
        lpszStr = new char[dwMinSize];
        WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, lpszStr, dwMinSize, NULL, FALSE);
        str = lpszStr;
        delete[] lpszStr;
        return str;
    }


    static UINT winSizeType; //�Ի��򴰿�״̬��״̬Ϊ��С��ʱ�����ڻ�ͼ����Լϵͳ��Դ


};



//���ݲ������ͼ�̵߳Ľṹ��
typedef struct DrawThreadParam
{
    int idcImg; //��ͼ�ؼ�id
    HWND hDlg; //�Ի���Ĵ��ھ��
    AssistWorker* assistWorker;
    AssistConfig* assistConfig;
    //Ԥ������������

} DRAWTHREADPARAM;

//��ʾ�����ͼ��Ƭ
void ShowImage(Mat& imgSrc, HWND hDlg, int IDC_IMG);

//��ʾ�����ͼ��Ƭ���̣߳������������˶Ի�����Ϣ
UINT DrawThreadProc(LPVOID pParam);