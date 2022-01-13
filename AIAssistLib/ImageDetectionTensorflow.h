#pragma once
#include<string>
#include<iostream>
#include<fstream>
#include<cmath>
#include <windows.h>
#include <Tlhelp32.h>
#include<opencv2/opencv.hpp>
#include<opencv2/imgproc.hpp>

#include "include/cppflow/cppflow.h"

#include "AssistConfig.h"
#include "ImageDetection.h"

using namespace cv;
using namespace std;
using namespace cuda;


/**
�ο�����
https://github.com/serizba/cppflow
https://serizba.github.io/cppflow/quickstart.html
https://github.com/dhiegomaga/cppflow/blob/master/examples/savedmodel/main.cpp
https://github.com/RoboSherlock/rs_tensorflow/blob/example/src/TensorFlowAnnotatorExample.cpp

https://hub.tensorflow.google.cn/s?q=efficientdet
https://hub.tensorflow.google.cn/tensorflow/ssd_mobilenet_v2/2
**/
// ����Tensorflow��ͼ������
// ͼ��������װ�࣬ʹ��AIģ�ͽ�ȡ��Ļ������ж�����
class ImageDetectionTensorflow
{
public:
    ImageDetectionTensorflow();
    ~ImageDetectionTensorflow();
    void ReInit();

    void getScreenshot();
    DETECTRESULTS detectImg();
    cv::Mat getImg();

private:
    void initImg(); //��ʼ��ͼ����Դ
    void releaseImg(); //�ͷ�ͼ����Դ

    void initDnn(); //��ʼ��DNN��Դ

private:
    static AssistConfig* m_AssistConfig;

    //����ͼ�������������
    HDC m_screenDC;
    HDC m_memDC;
    HBITMAP m_hBitmap;
    BITMAPINFO* m_Bitmapinfo = NULL;

    //���ͼ�����ݵ�mat����
    //ע��opencv�ĺ���ʹ���������ʱ���Զ�����create�����������ݴ������
    //�������ط�ʹ��ʱ����Ҫ�ֶ�����create�����ڴ洴�����飻�ظ�ִ��create�������ᵼ���ظ��������ݴ�����飻
    cv::Mat m_mat_src; //���bitmap�л�ȡ��Դͼ
    cv::Mat m_mat_3; //���ת��Ϊ3ͨ����ͼ��

    cv::Mat img_mat;//ģ�ͼ��ͼ������


    //AI�����������
    /*
    const string ConfigFile = "../../Data/model/mobilenet/ssd_mobilenet_v3_small.pbtxt";
    const string ModelFile = "../../Data/model/mobilenet/ssd_mobilenet_v3_small.pb";
    const string LabelFile = "../../Data/model/mobilenet/coco.names";
    */

    //ģ��Ŀ¼
    //efficientdet-liteʹ��cpu���ʱԼ120ms,,����gpu����ʱ�仹����Ҫ40-50ms��������Щ��⻹��̫��
    const string ModelFile = "../../Data/model/tensorflow/efficientdet-lite0/";
    const string LabelFile = "../../Data/model/mobilenet/coco.names";

    //ssd_mobilenetʹ��cpu���ʱԼ40ms,����gpu���ּ��ʱ�������׼ȷ�ʵ�һЩ @#��%����&*
    //const string ModelFile = "../../Data/model/tensorflow/ssd_mobilenet_v2_fpnlite/";
    //const string LabelFile = "../../Data/model/mobilenet/coco.names";

    /*
    const string ConfigFile = "../../Data/model/efficientdet/d0.pbtxt";
    const string ModelFile = "../../Data/model/efficientdet/d0.pb";
    const string LabelFile = "../../Data/model/efficientdet/coco.names";
    */

    vector<string> m_classLabels; //���ǩ
    const float MinConfidence = 0.60; //��С���Ŷ�
    const int PersonClassId = 1; //�����ǩ�б��� ���� ��classid
    //cv::dnn::Net m_net;
    //ʹ��ר�ŵĶ�����ģ����
    cppflow::model* m_net = NULL;

};

