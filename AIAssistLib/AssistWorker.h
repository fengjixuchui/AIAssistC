#pragma once
#include<string>
#include<iostream>
#include <thread>
#include<opencv2/opencv.hpp>

#include "AssistConfig.h"
#include "AssistState.h"
#include "ImageDetection.h"
#include "ImageDetectionTensorflow.h"
#include "MouseKeyboard.h"
#include "BlockQueue.h"

using namespace cv;
using namespace std;


//����������ͼ��ʾ�õĽṹ��
typedef struct DrawResults
{
    DETECTRESULTS out;
    Mat mat;
    double duration; //���ִ��ʱ��
}DRAWRESULTS;


// ���ֹ����࣬ʹ�ö��̷߳�ʽ���ͼ�����ִ�������̲���
class AssistWorker
{
public:

    //���캯������������
    AssistWorker();
    ~AssistWorker();

    //�޸����ú���Ҫ���³�ʼ��һЩ����
    void ReInit();

    //��⺯��������������
    void DetectWork();
    void FireWork();
    void MoveWork();
    void DrawWork();
    void MouseHookWork();
    void KeyboardHookWork();
    void PushWork();
    //������ͼmat���������ⲿ�̻߳�ͼ��������û������ʱ�����������ȴ�
    Mat PopDrawMat();

    //��������ֹͣ����
    void Start();
    void Pause();


public:
    // ���ö���
    static AssistConfig* m_AssistConfig;

    //ҵ�������
    //ImageDetection* imageDetection ;
    ImageDetectionTensorflow* imageDetection;
    MouseKeyboard* mouseKeyboard;

    //����̺߳��������̺߳ͻ�ͼ�߳�
    //��ǹ�̺߳��ƶ���׼�̺߳Ͷ��ж��ֿ�����֤��ǹ�ļ�ʹ�ԡ�
    thread* detectThread = NULL;
    thread* fireThread = NULL;
    thread* moveThread = NULL;
    thread* drawThread = NULL;

    //�������������hook�̺߳�ѹǹִ���߳�
    thread* mouseHookThread = NULL;
    thread* keyboardHookThread = NULL;
    thread* pushThread = NULL;

    //�߳̿��Ʊ���
    std::mutex m_detectMutex;
    std::condition_variable m_detectCondition;
    std::atomic_bool m_detectPauseFlag = true;   ///<��ͣ��ʶ

    std::mutex m_fireMutex;
    std::condition_variable m_fireCondition;
    std::atomic_bool m_firePauseFlag = true;   ///<��ͣ��ʶ

    std::mutex m_moveMutex;
    std::condition_variable m_moveCondition;
    std::atomic_bool m_movePauseFlag = true;   ///<��ͣ��ʶ

    std::mutex m_drawMutex;
    std::condition_variable m_drawCondition;
    std::atomic_bool m_drawPauseFlag = true;   ///<��ͣ��ʶ

    std::mutex m_pushMutex;
    static std::condition_variable m_pushCondition;
    std::atomic_bool m_pushPauseFlag = true;   ///<��ͣ��ʶ
    static std::atomic_bool m_startPush;   ///�Ƿ�����ѹǹ������־

    std::atomic_bool m_stopFlag = false;   ///<ֹͣ��ʶ

    static std::atomic_bool m_startFire;   ///�Ƿ����ڿ�ǹ�����������˹���ǹʱ��ִ���Զ���ǹ����

    //���������
    BlockQueue<DRAWRESULTS>* drawQueue;
    BlockQueue<DETECTRESULTS>* fireQueue;
    BlockQueue<DETECTRESULTS>* moveQueue;

    BlockQueue<Mat>* outDrawQueue;

    //������hook���
    HHOOK m_mouseHook = NULL;
    HHOOK m_keyboardHook = NULL;

    //��������
    static WEAPONINFO m_weaponInfo;

};


