#pragma once
#include<string>
#include<iostream>
#include<windows.h>
#include<winuser.h>
#include<opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// ��������ģ���࣬��UI������Ϣ��Ӧ������ģʽ̫�鷳��ֱ�Ӷ���һ����̬����
class AssistConfig
{
public:
    // ��ȡ��ʵ��
    static AssistConfig* GetInstance();

    //�����������������
    //����ʵ�ʼ������ֻ�����׼���ĵ�һС�����򣬱��ϼ���ٶ�
    void ReCalDetectionRect();

    //��ȡ��Ļ���ű���
    double static getScreensZoom();

    //������Ϸ��������������Ϸ�������
    void changGameName(string newGameName);


    //��������ģ��
    //ѡ�����Ϸ���̣���ͬ����Ϸ������������һ��
    //ע������������ڱ����Ǵ�һ���ո�ģ��е��
    string gameNames[4] = { "�������� ", "��ս", "��Խ����", "������Ϸ"};
    int maxModelWidths[4] = { 150, 200, 200, 200 };
    int playerCentXs[4] = { screenRect.width * 860 / 1920 - detectRect.x, screenRect.width / 2 - detectRect.x, 
        screenRect.width / 2 - detectRect.x, screenRect.width / 2 - detectRect.x };

    int gameIndex = 0; //��Ϸ����ID
    string gameName = "��ѡ����Ϸ"; //��Ϸ��������
    int maxModelWidth = maxModelWidths[0]; //��Ϸģ�������
    int playerCentX = playerCentXs[0]; //��Ϸ��ұ���ͼ���ټ�������ڵ�λ��,ȱʡ������������Ϸ����
    

    //�Ƿ�����ͼ����
    bool detectImg = true;
    //�û����õ�ʵ�ʼ���Ⱥ͸߶�
    int detectWidth = 320, detectHeight = 260;
    //��Ϸ����ռ�õ�ԭʼ��Ļ����
    cv::Rect screenRect = {0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN) };
    //�ü����ʵ��ͼ��������
    cv::Rect detectRect = { 0,0,detectWidth,detectHeight };
    //����������ĵ�
    int detectCentX = detectRect.x + detectRect.width / 2, detectCentY = detectRect.y + detectRect.height / 2;
    //������Ļ���ź�ģ��ü����ʵ��ͼ��������
    cv::Rect detectZoomRect = { 0,0,detectWidth,detectHeight };


    //�Ƿ��Զ�׷��
    bool autoTrace = false;
    //�Զ�׷�����ͣ�1�ǳ���׷�١�2������Ҽ���׼����׷��
    double mouseMoveSlow = 0.6; //�����ƶ�̫�죬��һ��������Ҫ�����ƶ��ٶȱ���,ֵҪ����ΪС��1
    int autoTraceType = 1;
    int autoTraceTimeOut = 0; //����Ҽ���׼����׷�ٵ���ʱ��ȱʡ��30������

    //�Ƿ��Զ�����
    bool autoFire = false;
    //ǹе�������,1�ǵ��㡢2��3���㡢3��6���㡢4�ǳ�����ǹ
    int gunType = 1;
    //������ǹʱ��
    int autoFireTime = 600;

    //�Ƿ��Զ�ѹǹ
    bool autoPush = false;


private:
    AssistConfig();
    ~AssistConfig();
    void test();

private:
    // Ψһ��ʵ������
    static AssistConfig* m_config;
};

