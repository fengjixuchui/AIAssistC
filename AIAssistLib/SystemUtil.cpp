#include "pch.h"
#include "SystemUtil.h"


//��̬��Ա��ʼ��
AssistConfig* SystemUtil::m_AssistConfig = AssistConfig::GetInstance();

/*
//�෽��ʵ��
void SystemUtil::findProcessWindowRect() {

    cv::Rect rect;

    //��ȡҪ���ҵĽ�������
    string processName = m_AssistConfig->processName;

    // �������̿��գ���������
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE != hSnapshot)
    {
        PROCESSENTRY32 pi;
        pi.dwSize = sizeof(PROCESSENTRY32);
        BOOL bRet = Process32First(hSnapshot, &pi);
        while (bRet)
        {
            //�ȽϽ�������
            string name = WChar2String(pi.szExeFile);
            if (processName.compare(name) == 0) {
                if (pi.th32ProcessID != 0) {
                    //���ݽ���ID�ҵ������ھ��
                    HWND hWnd = GetProcessMainWnd(pi.th32ProcessID);
                    if (INVALID_HANDLE_VALUE != hWnd) {
                        RECT r1,r2;
                        GetWindowRect(hWnd, &r1);
                        GetClientRect(hWnd, &r2);

                        //���������£��ж��´��ڴ�С�����������������������һ������
                        if ((r1.right - r1.left) > 600 && (r1.bottom - r1.top) > 500) {
                            //���Ǵ���ģʽ�£��б߿�Ŀ��,�ڼ�����Ļλ��ʱ���ų�
                            rect.x = r1.left + r2.left + ((r1.right-r1.left)-(r2.right-r2.left))/2;
                            rect.y = r1.top + r2.top + ((r1.bottom - r1.top) - (r2.bottom - r2.top));
                            rect.width = r2.right - r2.left;
                            rect.height = r2.bottom - r2.top;                    

                            //�ҵ����������Ĵ��ڣ��˳�ѭ��
                            m_AssistConfig->screenRect = rect;
                            break;
                        }
                    }
                }              
            }
            bRet = Process32Next(hSnapshot, &pi);
        }
    }

    CloseHandle(hSnapshot);
	return;
}
*/


//���ݴ������Ʋ��Ҵ��ھ��
void SystemUtil::findProcessWindowRect() {

    cv::Rect rect;
    //��ȡҪ���ҵĽ�������
    string gameName = m_AssistConfig->gameName;

    //���ݴ��ڱ��⣬���ҵ�һ�����ھ��
    CString title = StringToCString(gameName);
    HWND hWnd = FindWindow(NULL, title);
    while (NULL != hWnd) {
        RECT r1, r2;
        GetWindowRect(hWnd, &r1);
        GetClientRect(hWnd, &r2);

        //���������£��ж��´��ڴ�С�����������������������һ������
        if ((r1.right - r1.left) > 500 && (r1.bottom - r1.top) > 300) {
            //���Ǵ���ģʽ�£��б߿�Ŀ��,�ڼ�����Ļλ��ʱ���ų�
            //���ڱ߿���Ҫ�Ǳ�����ռ����λ��
            //��������խ�߿�ȼ�����һ����
            int thinBarWidth = ((r1.right - r1.left) - (r2.right - r2.left)) / 2;
            rect.x = r1.left + r2.left + thinBarWidth;
            rect.y = r1.top + r2.top + (((r1.bottom - r1.top) - (r2.bottom - r2.top)) - thinBarWidth);
            rect.width = r2.right - r2.left;
            rect.height = r2.bottom - r2.top;

            //�ҵ����������Ĵ��ڣ��˳�ѭ��
            m_AssistConfig->screenRect = rect;

            break;
        }
        
        //ѭ��������һ��ͬ����Ĵ���
        hWnd = FindWindowEx(NULL, hWnd, NULL, title);

    }
                  
    return;
}


// WChar����ת�ַ���
string SystemUtil::WChar2String(WCHAR   wchar[MAX_PATH]) {
    int len = WideCharToMultiByte(CP_ACP, 0, wchar, -1, NULL, 0, NULL, NULL);

    if (len <= 0)
        return string("");

    char* pChar = new char[len];
    WideCharToMultiByte(CP_ACP, 0, wchar, -1, pChar, len, NULL, NULL);
    pChar[len - 1] = 0;

    string str = string(pChar);
    delete[] pChar;

    return str;
}





