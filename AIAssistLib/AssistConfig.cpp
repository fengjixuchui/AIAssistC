#include "pch.h"
#include "AssistConfig.h"


//��ʼ����̬��Ա����
AssistConfig* AssistConfig::m_config = new AssistConfig();


AssistConfig* AssistConfig::GetInstance()
{
    return m_config;
}

AssistConfig::AssistConfig()
{
    ReCalDetectionRect();
    
    return;
}

AssistConfig::~AssistConfig()
{
    //ֻnew��һ�εľ�ָ̬�룬�����ֶ��ͷţ�ϵͳ���Զ��ͷ�
    //if (m_config != NULL)
    //    delete m_config;

    return;
}

void AssistConfig::changGameName(string newGameName) {
   
    //������Ϸ������ȱʡ����Ϊ���������һ������������Ϸ��
    int findIndex = gameLength - 1;
    for (int i = 0; i < gameLength; i++) {
        if (gameNames[i] == newGameName) {
            findIndex = i;
            break;
        }
    }

    //������Ϸ��ز���
    gameName = newGameName;
    gameIndex = findIndex;
    maxModelWidth = maxModelWidths[findIndex];
    playerCentX = playerCentXs[findIndex];

    pushHeight[0] = pushHeights[findIndex][0];
    pushHeight[1] = pushHeights[findIndex][1];
}

//����ʵ�ʼ������ֻ�����׼���ĵ�һС�����򣬱��ϼ���ٶ�
void AssistConfig::ReCalDetectionRect() {
    //�û����õ�ʵ�ʼ���Ⱥ͸߶�
    int w = detectWidth, h = detectHeight;

    if (w > 0 && h > 0 && screenRect.width > 0 && screenRect.height > 0)
    {
        //�ü����ڣ����㷨������ʵ�ʼ������ֻ�����׼���ĵ�һС�����򣬱��ϼ���ٶ�
        int centX = screenRect.x + screenRect.width / 2;
        int centy = screenRect.y + screenRect.height / 2;

        detectRect.x = centX - w / 2;
        detectRect.width = w;
        //�����������ĵ���΢����ƫ�� 1/10
        detectRect.y = centy - (h / 2 + h / 10);
        detectRect.height = h;

        //������Ϸ��������
        detectCentX = centX;
        detectCentY = centy;

        //������Ļ����
        //��Ļ���ź�ģ��ü����ʵ��ͼ��������
        //ע��ץȡ��Ļ��ʱ��ʹ�����ź�������������꣬ץȡ��������ʵ�����߼��ֱ�������
        double zoom = getScreensZoom();
        detectZoomRect.x = int(detectRect.x * zoom);
        detectZoomRect.y = int(detectRect.y * zoom);
        detectZoomRect.width = int(detectRect.width * zoom);
        detectZoomRect.height = int(detectRect.height * zoom);

        //������Ϸ��ұ���λ�ã�pubg��
        //������ʵ�ʼ��������������λ��
        playerCentX = screenRect.width * 860 / 1920 - detectRect.x;

        //��Ϸģ�������
        int maxModelWidth = detectWidth / 5;
    }
}


/* ��ȡ��Ļ����ֵ */
double AssistConfig::getScreensZoom()
{
    // ��ȡ���ڵ�ǰ��ʾ�ļ�����
    HWND hWnd = GetDesktopWindow();
    HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

    // ��ȡ�������߼����
    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfo(hMonitor, &monitorInfo);
    int cxLogical = (monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left);

    // ��ȡ������������
    DEVMODE dm;
    dm.dmSize = sizeof(dm);
    dm.dmDriverExtra = 0;
    EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &dm);
    int cxPhysical = dm.dmPelsWidth;

    return cxPhysical * 1.0 / cxLogical;
}

void AssistConfig::test()
{
    return;
}
