#include "pch.h"
#include "AssistWorker.h"


//��ʼ����̬��Ա����
AssistConfig* AssistWorker::m_AssistConfig = AssistConfig::GetInstance();
std::condition_variable AssistWorker::m_pushCondition = std::condition_variable();
std::atomic_bool AssistWorker::m_startPush = false;   ///�Ƿ�����ѹǹ������־
WEAPONINFO AssistWorker::m_weaponInfo = { 3,1,1 };

std::atomic_bool AssistWorker::m_startFire = false;   ///�Ƿ����ڿ�ǹ�����������˹���ǹʱ��ִ���Զ���ǹ����


LRESULT CALLBACK MouseHookProcedure(int nCode, WPARAM wParam, LPARAM lParam)
{
    MSLLHOOKSTRUCT* p = (MSLLHOOKSTRUCT*)lParam;
    BOOLEAN injected = p->flags & LLMHF_INJECTED || p->flags & LLMHF_LOWER_IL_INJECTED; // Checks if click was injected and not from a mouse
    if (nCode == HC_ACTION && !injected)
    {
        if (wParam == WM_LBUTTONDOWN) {
            //�������ڿ�ǹ��־
            AssistWorker::m_startFire = true;

            //�ж��û��Ƿ��������Զ�ѹǹ
            if (MouseKeyboard::m_AssistConfig->autoPush) {
                //��ʼѹǹ
                AssistWorker::m_startPush = true;
                AssistWorker::m_pushCondition.notify_all();
            }
        }
        else if (wParam == WM_LBUTTONUP) {
            //�������ڿ�ǹ��־
            AssistWorker::m_startFire = false;

            //������̧������ѹǹ
            AssistWorker::m_startPush = false;
            AssistWorker::m_pushCondition.notify_all();
        }
        else if (wParam == WM_RBUTTONDOWN) {
        }
        else if (wParam == WM_RBUTTONUP) {
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

LRESULT CALLBACK KeyboardHookProcedure(int nCode, WPARAM wParam, LPARAM lParam)
{
    // WH_KEYBOARD_LL uses the LowLevelKeyboardProc Call Back
    // wParam and lParam parameters contain information about the message.
    auto* p = (KBDLLHOOKSTRUCT*)lParam;
    if (nCode == HC_ACTION)
    {
        if (wParam == WM_SYSKEYDOWN || wParam == WM_KEYDOWN)
        {
            //����Ƿ��л�����
            switch (p->vkCode) {
                case 49: 
                    AssistWorker::m_weaponInfo.bag = 1;
                    break;
                case 50:
                    AssistWorker::m_weaponInfo.bag = 2;
                    break;
                case 51:
                    AssistWorker::m_weaponInfo.bag = 3;
                    break;
                case 52:
                    AssistWorker::m_weaponInfo.bag = 4;
                    break;
                case 53:
                    AssistWorker::m_weaponInfo.bag = 5;
                    break;
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}


AssistWorker::AssistWorker()
{
    //�����¼����������������
    m_AssistConfig->ReCalDetectionRect();

    //��������
    //���������г�������Ϊ1��Ŀ����ֻ�������¼������
    fireQueue = new BlockQueue<DETECTRESULTS>(1);
    moveQueue = new BlockQueue<DETECTRESULTS>(1);

    drawQueue = new BlockQueue<DRAWRESULTS>(10);
    outDrawQueue = new BlockQueue<Mat>(10);

    //�����߳�
    detectThread = new thread(std::bind(&AssistWorker::DetectWork, this));
    detectThread->detach();

    fireThread = new thread(std::bind(&AssistWorker::FireWork, this));
    fireThread->detach();

    moveThread = new thread(std::bind(&AssistWorker::MoveWork, this));
    moveThread->detach();

    drawThread = new thread(std::bind(&AssistWorker::DrawWork, this));
    drawThread->detach();

    mouseHookThread = new thread(std::bind(&AssistWorker::MouseHookWork, this));
    mouseHookThread->detach();

    keyboardHookThread = new thread(std::bind(&AssistWorker::KeyboardHookWork, this));
    keyboardHookThread->detach();

    pushThread = new thread(std::bind(&AssistWorker::PushWork, this));
    pushThread->detach();

    //����ͼƬ��������������
    //imageDetection = new ImageDetection();
    //imageDetection = new ImageDetectionTensorflow();
    imageDetection = NULL;
    mouseKeyboard = new MouseKeyboard();

    return;
}

AssistWorker::~AssistWorker()
{
    m_stopFlag = true;
    
    if (imageDetection != NULL)
        delete imageDetection;
    if (mouseKeyboard != NULL)
        delete mouseKeyboard;

    if (drawQueue != NULL)
        delete drawQueue;
    if (fireQueue != NULL)
        delete fireQueue;
    if (moveQueue != NULL)
        delete moveQueue;

    if (detectThread != NULL)
        delete detectThread;
    if (fireThread != NULL)
        delete fireThread;
    if (moveThread != NULL)
        delete moveThread;
    if (drawThread != NULL)
        delete drawThread;
    if (mouseHookThread != NULL)
        delete mouseHookThread;
    if (keyboardHookThread != NULL)
        delete keyboardHookThread;
    if (pushThread != NULL)
        delete pushThread;

    if (m_mouseHook) { 
        UnhookWindowsHookEx(m_mouseHook); 
        m_mouseHook = NULL;
    }
    if (m_keyboardHook) { 
        UnhookWindowsHookEx(m_keyboardHook); 
        m_keyboardHook = NULL;
    }

    return;
}

//�޸����ú���Ҫ���³�ʼ��һЩ����
void AssistWorker::ReInit() {
    
    //��ֹͣ���й����߳�
    Pause();
    Sleep(200);

    //�����¼����������������
    m_AssistConfig->ReCalDetectionRect();

    //������ж���
    drawQueue->Clear();
    outDrawQueue->Clear();
    fireQueue->Clear();
    moveQueue->Clear();

    //�ؽ���Ҫ�ؽ��Ķ���
    if (imageDetection != NULL) {
        imageDetection->ReInit();
    }
    else{
        //�½�����
        //imageDetection = new ImageDetection();
        imageDetection = new ImageDetectionTensorflow();
    }

    //���������߳�
    Start();

    return;
}

void AssistWorker::DetectWork()
{
    while (!m_stopFlag)
    {
        if (m_detectPauseFlag)
        {
            //�����̱߳�־�����߳�����
            unique_lock<mutex> locker(m_detectMutex);
            while (m_detectPauseFlag)
            {
                m_detectCondition.wait(locker); // Unlock _mutex and wait to be notified
            }
            locker.unlock();
        }
        else {
            //ͼ����
            double duration;
            clock_t start, finish;
            start = clock();

            //��Ļ����ͼ����
            imageDetection->getScreenshot();
            DETECTRESULTS detectResult = imageDetection->detectImg();

            finish = clock();
            duration = (double)(finish - start) * 1000 / CLOCKS_PER_SEC;

            if (detectResult.classIds.size() > 0) {
                //�м�鵽���࣬����ŵ�������,��֪ͨ�����߳����Ѽ����
                //��ǹ������ƶ��������ڲ�ͬ�̣߳����²������ѣ��ȷŻ�ͬһ���̴߳���
                //fireQueue->PushBackForce(detectResult);
                moveQueue->PushBackForce(detectResult);
            }

            //Ȼ����mat��������ǰ����ʾ
            Mat mat = imageDetection->getImg();

            DRAWRESULTS  drawResult{detectResult, mat, duration};
            bool push = drawQueue->PushBack(drawResult);

            //���������û�����ͳɹ������ֹ��ͷ�clone��mat����
            if (!push) {
                mat.release();
                mat = NULL;
            }
            
        }
    }

    return;
}

void AssistWorker::FireWork()
{
    while (!m_stopFlag)
    {
        if (m_firePauseFlag)
        {
            //�����̱߳�־�����߳�����
            unique_lock<mutex> locker(m_fireMutex);
            while (m_firePauseFlag)
            {
                m_fireCondition.wait(locker); // Unlock _mutex and wait to be notified
            }
            locker.unlock();
        }
        else {
            
            //��ȡ�����д�ŵļ����
            DETECTRESULTS detectResult;
            bool ret = fireQueue->PopFront(detectResult);
            if (ret) {
                //ִ���Զ���ǹ����
                //�ȼ���Ƿ��������Զ���ǹ��־
                if (m_AssistConfig->autoFire && !AssistWorker::m_startFire) {
                    //�ڼ���Ƿ��Ѿ���׼��
                    bool isInTarget = mouseKeyboard->IsInTarget(detectResult);
                    //����Ѿ���׼��ִ���Զ���ǹ����
                    if (isInTarget) {
                        //��ǹ������ƶ��������ڲ�ͬ�̣߳����²������ѣ��ȷŻ�ͬһ���̴߳���
                        //mouseKeyboard->AutoFire(detectResult);
                    }
                }
            }
            
        }
    }

    return;
}

void AssistWorker::MoveWork()
{
    while (!m_stopFlag)
    {
        if (m_movePauseFlag)
        {
            //�����̱߳�־�����߳�����
            unique_lock<mutex> locker(m_moveMutex);
            while (m_movePauseFlag)
            {
                m_moveCondition.wait(locker); // Unlock _mutex and wait to be notified
            }
            locker.unlock();
        }
        else {
            //��ȡ�����д�ŵļ����
            DETECTRESULTS detectResult;
            bool ret = moveQueue->PopFront(detectResult);
            if (ret) {
                //ִ��������
                //std::cout << to_string(detectResult.classIds.size());
                //�ȼ���Ƿ��������Զ�׷��
                //����������ֻ��ʹ�ñ���1��2(ʹ�ò�ǹ�;ѻ�ǹ��ʱ��)���Ž���׷�٣�ʹ������������׷��
                if (m_AssistConfig->autoTrace && (m_weaponInfo.bag==1 || m_weaponInfo.bag == 2)) {
                    //�ڼ���Ƿ��Ѿ���׼��
                    bool isInTarget = mouseKeyboard->IsInTarget(detectResult);
                    //û����׼������£���ִ�����׷�ٲ���
                    if (isInTarget) {
                        //��ǹ������ƶ��������ڲ�ͬ�̣߳����²������ѣ��ȷŻ�ͬһ���̴߳���
                        //����һ��������û���˹�����������������£���ִ���Զ���ǹ
                        if (m_AssistConfig->autoFire && !AssistWorker::m_startFire) {
                            mouseKeyboard->AutoFire(detectResult);
                        }
                    }
                    else {
                        mouseKeyboard->AutoMove(detectResult);
                    }
                }
            }
        }
    }

    return;
}

void AssistWorker::DrawWork()
{
    //cv::namedWindow("opencvwindows", WINDOW_AUTOSIZE);
    //cv::startWindowThread();

    while (!m_stopFlag)
    {
        if (m_drawPauseFlag)
        {
            //�����̱߳�־�����߳�����
            unique_lock<mutex> locker(m_drawMutex);
            while (m_drawPauseFlag)
            {
                m_drawCondition.wait(locker); // Unlock _mutex and wait to be notified
            }
            locker.unlock();
        }
        else {
            //��ȡ�����д�ŵļ����
            DrawResults drawResult;
            bool ret = drawQueue->PopFront(drawResult);
            if (ret) {
                //ִ�л�ͼ����
                DETECTRESULTS out = drawResult.out;
                Mat mat = drawResult.mat;
                double duration = drawResult.duration;
                if (!mat.empty()) {
                    string times = format("%.2f ms", duration);
                    putText(mat, times, Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 250), 1);

                    //ע����Ϸ��Ļ���ģ��ͼ�����������λ�ò�һ�������������������΢����1/10
                    Rect center = { mat.cols/2 -5,mat.rows / 2 + mat.rows /10 - 5,10,10 };
                    rectangle(mat, center, Scalar(0, 0, 250), 2);

                    if (out.classIds.size() > 0) {
                        Rect rect = out.boxes[0];
                        rectangle(mat, { rect.x + rect.width/2 -4, rect.y + rect.height / 2 - 4, 8, 8 }, Scalar(255, 178, 50), 2);
                    }

                    for (int i = 0; i < out.classIds.size(); i++) {
                        rectangle(mat, out.boxes[i], Scalar(255, 178, 50), 2);

                        //Get the label for the class name and its confidence
                        string label = format("%.2f", out.confidences[i]);
                        //label = m_classLabels[classIds[i]-1] + ":" + label;
                        label = "" + to_string(out.classIds[i]) + ", " + label;                      

                        //Display the label at the top of the bounding box
                        int baseLine;
                        Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 2, &baseLine);
                        int top = max(out.boxes[i].y, labelSize.height);
                        putText(mat, label, Point(out.boxes[i].x, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 250), 1);
                    }
                    
                    //�Ѵ�����õ�mat��������ⲿʹ�õĶ�����
                    bool push = outDrawQueue->PushBack(mat);
                    //�����������ʧ�ܣ��ֹ�����clone��mat����
                    if (!push) {
                        mat.release();
                        mat = NULL;
                    }
                   
                    //cv::imshow("opencvwindows", mat);
                    //waitKey(10);
                }
            }
        }
    }

    return;
}

void AssistWorker::MouseHookWork()
{
    //����hook���Ǹ��̹߳��ص�hook�������Ǹ��߳���ִ�лص�����
    if (!m_mouseHook) {
        m_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProcedure, GetModuleHandle(nullptr), NULL);
    }

    MSG Msg;
    while (!m_stopFlag && GetMessage(&Msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    if (m_mouseHook) {
        UnhookWindowsHookEx(m_mouseHook);
        m_mouseHook = NULL;
    }

    return;
}

void AssistWorker::KeyboardHookWork()
{
    //����hook���Ǹ��̹߳��ص�hook�������Ǹ��߳���ִ�лص�����
    if (!m_keyboardHook) {
        m_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProcedure, GetModuleHandle(nullptr), NULL);
    }

    MSG Msg;
    while (!m_stopFlag && GetMessage(&Msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    if (m_keyboardHook) {
        UnhookWindowsHookEx(m_keyboardHook);
        m_keyboardHook = NULL;
    }

    return;
}

void AssistWorker::PushWork()
{

    while (!m_stopFlag)
    {
        if (m_pushPauseFlag || !m_startPush)
        {
            //�����̱߳�־�����߳�����
            unique_lock<mutex> locker(m_pushMutex);
            while (m_pushPauseFlag || !m_startPush)
            {
                m_pushCondition.wait(locker); // Unlock _mutex and wait to be notified
            }
            locker.unlock();
        }
        else {
            //ִ��ѹǹ����
            mouseKeyboard->AutoPush(m_weaponInfo);
        }
    }

    return;
}

Mat AssistWorker::PopDrawMat() {
    Mat mat;
    outDrawQueue->PopFront(mat);

    //ע�ⷵ�ؿ�¡���󣬲��ܰ�ȫ�ش���ͼ������
    Mat mat2 = mat.clone();

    //�ͷ���mat
    mat.release();
    mat = NULL;

    return mat2;
}

void AssistWorker::Start()
{
    m_detectPauseFlag = false;
    m_detectCondition.notify_all();
    
    m_firePauseFlag = false;
    m_fireCondition.notify_all();

    m_movePauseFlag = false;
    m_moveCondition.notify_all();

    m_drawPauseFlag = false;
    m_drawCondition.notify_all();

    m_pushPauseFlag = false;
    m_pushCondition.notify_all();

    return;
}

void AssistWorker::Pause()
{
    m_detectPauseFlag = true;
    m_detectCondition.notify_all();

    m_firePauseFlag = true;
    m_fireCondition.notify_all();

    m_movePauseFlag = true;
    m_moveCondition.notify_all();

    m_drawPauseFlag = true;
    m_drawCondition.notify_all();

    m_pushPauseFlag = true;
    m_pushCondition.notify_all();

    return;
}
