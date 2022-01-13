// OpencvTest.cpp : ���ļ����� "main" ����������ִ�н��ڴ˴���ʼ��������
//

#include <iostream>
#include<string>
#include<fstream>
#include<cmath>
#include <windows.h>
#include <time.h>

#include <Tlhelp32.h>
#include<opencv.hpp>
#include<imgproc.hpp>

#include "include/cppflow/cppflow.h"



using namespace cv;
using namespace std;


/**
https://github.com/serizba/cppflow
https://serizba.github.io/cppflow/quickstart.html
https://github.com/dhiegomaga/cppflow/blob/master/examples/savedmodel/main.cpp
https://github.com/RoboSherlock/rs_tensorflow/blob/example/src/TensorFlowAnnotatorExample.cpp
**/


int main()
{
    HDC m_screenDC;
    HDC m_memDC;
    HBITMAP m_hBitmap;

    int width = 320, height = 320;

    // ��ȡ��Ļ DC
    m_screenDC = GetDC(HWND_DESKTOP);
    m_memDC = CreateCompatibleDC(m_screenDC);
    // ����λͼ
    m_hBitmap = CreateCompatibleBitmap(m_screenDC, width, height);
    SelectObject(m_memDC, m_hBitmap);


    cv::Mat mat, mat2;
    mat.create(height, width, CV_8UC4);
    //mat.create(height, width, CV_32FC4);

    //std::cout << "mat.type() = " << mat.type() << std::endl;
    //std::cout << "mat2.type() = " << mat2.type() << std::endl;

    //cv::namedWindow("opencvwindows", WINDOW_AUTOSIZE);

    //cv::imshow("opencvwindows", mat);
    //cout << "show empty mat\n";
    //waitKey(2000);




    double duration1, duration2;
    clock_t start1, finish1, start2, finish2;

    start1 = clock();


    // �õ�λͼ������
    // ʹ��BitBlt��ͼ�����ܽϵͣ������޸�ΪDXGI
    //Windows8�Ժ�΢��������һ���µĽӿڣ��С�Desktop Duplication API����Ӧ�ó��򣬿���ͨ������API�����������ݡ�
    //����Desktop Duplication API��ͨ��Microsoft DirectX Graphics Infrastructure (DXGI)���ṩ����ͼ��ģ��ٶȷǳ��졣
    bool opt = BitBlt(m_memDC, 0, 0, width, height, m_screenDC, 300, 250, SRCCOPY);

    int iBits = GetDeviceCaps(m_memDC, BITSPIXEL) * GetDeviceCaps(m_memDC, PLANES);
    WORD wBitCount;
    if (iBits <= 1)
        wBitCount = 1;
    else if (iBits <= 4)
        wBitCount = 4;
    else if (iBits <= 8)
        wBitCount = 8;
    else if (iBits <= 24)
        wBitCount = 24;
    else
        wBitCount = 32;
    BITMAPINFOHEADER bi = { sizeof(bi), width, -height, 1, wBitCount, BI_RGB };


    //int rows = GetDIBits(m_screenDC, m_hBitmap, 0, detectRect.height, m_mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
    int rows = GetDIBits(m_memDC, m_hBitmap, 0, height, mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    //cv::namedWindow("opencvwindows", WINDOW_AUTOSIZE);
    //cv::imshow("opencvwindows", mat);
    cout << "show GetDIBits mat\n";
    //waitKey(10000);

    // ����mat����
    //ע�����˳��Ϊ �С��С�ͼ���ʽ��bitmap����
    //Mat screenshot = Mat(detectRect.height, detectRect.width, CV_8UC4, m_screenshotData);
    //Mat screenshot(detectRect.width, detectRect.height, CV_8UC4, m_screenshotData);
    //Mat screenshot(detectRect.height, detectRect.width, CV_32FC4, m_screenshotData);

    //��Ļ��ͼ����Ƶ��ͼ��ʽ��һ������Ҫ����ͼ���ʽת��
    //ȥ��alpha ֵ(͸����)ͨ����ת��ΪCV_8UC3��ʽ
    cv::cvtColor(mat, mat2, COLOR_RGBA2RGB);

    finish1 = clock();
    duration1 = (double)(finish1 - start1) * 1000 / CLOCKS_PER_SEC;

    //std::cout << "mat.type() = " << mat.type() << std::endl;
    //std::cout << "mat2.type() = " << mat2.type() << std::endl;

    //���ͼ���ʽ
    //std::cout << "mat��ʽ��" << mat.depth() << "," << mat.type() << "," << mat.channels() << "," << std::endl;
    //std::cout << "mat2��ʽ��" << mat2.depth() << "," << mat2.type() << "," << mat2.channels() << "," << std::endl;

    cv::imshow("opencvwindows", mat2);
    cout << "show cvtColor mat\n";
    waitKey(10000);




    /**
    const string ConfigFile = "../../Data/model/efficientdet/d0.pbtxt";
    const string ModelFile = "../../Data/model/efficientdet/d0.pb";
    const string LabelFile = "../../Data/model/efficientdet/coco.names";
    vector<string> m_classLabels; //���ǩ
    const float MinConfidence = 0.7; //��С���Ŷ�
    const int PersonClassId = 0; //�����ǩ�б�����Ա��λ��
    vector<String> m_outputsNames; //ģ������������
   */


   //const string ConfigFile = "../../Data/model/mobilenet/ssd_mobilenet_v3.pbtxt";
   //const string ModelFile = "../../Data/model/mobilenet/ssd_mobilenet_v3.pb";
    //const string ConfigFile = "../../Data/model/mobilenet/ssd_mobilenet_v3_small.pbtxt";
    //const string ModelFile = "../../Data/model/mobilenet/ssd_mobilenet_v3_small.pb";

    //const string ModelFile = "../../Data/model/tensorflow/efficientdet_d0/";
    //const string ModelFile = "../../Data/model/efficientdet/d0.pb";

    const string LabelFile = "../../Data/model/mobilenet/coco.names";
    vector<string> m_classLabels; //���ǩ
    const float MinConfidence = 0.5; //��С���Ŷ�
    const int PersonClassId = 0; //�����ǩ�б�����Ա��λ��
    //cv::dnn::DetectionModel m_net;
    vector<String> m_outputsNames; //ģ������������



    //get the names of all the layers in the network
    //vector<String> layersNames = m_net.getLayerNames();

    //m_net.setPreferableBackend(dnn::DNN_BACKEND_OPENCV);
    //m_net.setPreferableTarget(dnn::DNN_TARGET_CPU);

    // ���ط����ǩ
    ifstream fin(LabelFile);
    if (fin.is_open())
    {
        string className = "";
        while (std::getline(fin, className))
            m_classLabels.push_back(className);
    }


    //gpu��Դ�������ã�ʹ��10%gpu�ڴ棬�����ڴ�������Ϸ
    // Read more to see how to obtain the serialized options
    std::vector<uint8_t> config{ 0x32,0x9,0x9,0x9a,0x99,0x99,0x99,0x99,0x99,0xb9,0x3f };
    // Create new options with your configuration
    TFE_ContextOptions* options = TFE_NewContextOptions();
    //TFE_ContextOptionsSetConfig(options, config.data(), config.size(), cppflow::context::get_status());
    TFE_ContextOptionsSetConfig(options, config.data(), config.size(), cppflow::context::get_status());
    // Replace the global context with your options
    cppflow::get_global_context() = cppflow::context(options);





    //����ģ��
    std::cout << "Current tensorflow version: " << TF_Version() << std::endl;
    //const string ModelFile = "../../Data/model/tensorflow/efficientdet-lite0/";
    const string ModelFile = "../../Data/model/tensorflow/ssd_mobilenet_v2/";
    cppflow::model model(ModelFile);
    cout << "tensorflowģ�ͼ��سɹ�" << "\n";

    //���������Щ����
    //cout << model.get_operations() << "\n";


    //ת��ͼƬ
    //int input_height = 512;
    //int input_width = 512;
    int input_height = 320;
    int input_width = 320;

    auto classIds = vector<float_t>();
    auto confidences = vector<float_t>();
    auto boxes_float = vector<float_t>();
    auto boxes = vector<Rect>();

    cv::Mat tempMat;
    std::vector<uint8_t > img_data;


    mat2.convertTo(tempMat, CV_8UC3, 1.0);
    img_data.assign(tempMat.data, tempMat.data + tempMat.total() * 3);
    cppflow::tensor input(img_data, { 1, input_height, input_width, 3 });


    //auto out = model({ {"serving_default_images:0", input} }, { "StatefulPartitionedCall:0", "StatefulPartitionedCall:1", "StatefulPartitionedCall:2" });
    //std::cout << "out shape: " << out[0] << std::endl;
    //std::cout << "out shape: " << out[1] << std::endl;
    //std::cout << "out shape: " << out[2] << std::endl;

    start2 = clock();

    for (int count = 0; count < 1; count++) {

        //double minValue, maxValue;    // ���ֵ����Сֵ
        //cv::Point  minIdx, maxIdx;    // ��Сֵ���꣬���ֵ����     
        //cv::Mat image_re = mat2.reshape(1);
        //cv::minMaxLoc(image_re, &minValue, &maxValue, &minIdx, &maxIdx);

        //cv::Mat tempMat(input_height, input_width, CV_8UC3);   //ע��ת����ͼ��Ϊ��ɫͼ���ǻҶ�ͼ

        //mat2.convertTo(tempMat, CV_8UC3, 255.0/(maxValue- minValue));
        mat2.convertTo(tempMat, CV_8UC3, 1.0);
        //resize(tempMat, tempMat, cv::Size(input_width, input_height));

        //cv::imshow("opencvwindows", tempMat);
        //cout << "show cvtColor mat\n";
        //waitKey(10000);

        img_data.assign(tempMat.data, tempMat.data + tempMat.total() * 3);

        cppflow::tensor input(img_data, { 1, input_height, input_width, 3 });
        //input = cppflow::expand_dims(input, 0);

        //efficientdet-lite0
        //auto out = model({ {"serving_default_images:0", input} }, { "StatefulPartitionedCall:0", "StatefulPartitionedCall:1", "StatefulPartitionedCall:2" });
        //ssd_mobilenet_v2
        auto out = model({ {"serving_default_input_tensor:0", input} }, { "StatefulPartitionedCall:1", "StatefulPartitionedCall:2", "StatefulPartitionedCall:4" });


        std::cout << "out shape: " << out.size() << std::endl;
        std::cout << "out shape: " << out[0].shape() << std::endl;
        std::cout << "out shape: " << out[1].shape() << std::endl;
        std::cout << "out shape: " << out[0] << std::endl;
        std::cout << "out shape: " << out[1] << std::endl;
        std::cout << "out shape: " << out[2] << std::endl;
     

        //efficientdet-lite0
        //classIds = out[2].get_data<float_t>();
        //confidences = out[1].get_data<float_t>();
        //boxes_float = out[0].get_data<float_t>();
        //ssd_mobilenet_v2
        classIds = out[1].get_data<float_t>();
        confidences = out[2].get_data<float_t>();
        boxes_float = out[0].get_data<float_t>();

        boxes = vector<Rect>();
        for (int i = 0; i < boxes_float.size(); i += 4) {
            //efficientdet-lite0
            //object_detection�����ʽΪ[ymin , xmin , ymax , xmax] 
            //Rect rect;
            //rect.y = boxes_float[i];
            //rect.x = boxes_float[i + 1];
            //rect.height = boxes_float[i + 2] - rect.y;
            //rect.width = boxes_float[i + 3] - rect.x;
            //boxes.push_back(rect);
            //ssd_mobilenet_v2
            //object_detection�����ʽΪ[ymin, xmin, ymax, xmax]
            Rect rect;
            rect.y = boxes_float[i] * input_height;
            rect.x = boxes_float[i + 1] * input_width;
            rect.height = boxes_float[i + 2] * input_height - rect.y;
            rect.width = boxes_float[i + 3] * input_width - rect.x;
            boxes.push_back(rect);
        }
    }

    finish2 = clock();
    duration2 = (double)(finish2 - start2) * 1000 / (CLOCKS_PER_SEC);


    string times = format("%.2f, %.2f", duration1, duration2);
    putText(mat2, times, Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 1);


    for (int i = 0; i < 5 && i < classIds.size(); i++) {
        rectangle(mat2, boxes[i], Scalar(255, 178, 50), 3);

        //Get the label for the class name and its confidence
        string label = format("%.2f", confidences[i]);
        if (!m_classLabels.empty())
        {
            //label = m_classLabels[classIds[i]-1] + ":" + label;
            label = to_string((int)classIds[i]) + "," + label;
        }

        //Display the label at the top of the bounding box
        int baseLine;
        Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
        int top = max(boxes[i].y, labelSize.height);
        putText(mat2, label, Point(boxes[i].x, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 1);
    }


    //cv::namedWindow("opencvwindows", WINDOW_AUTOSIZE);
    cv::imshow("opencvwindows", mat2);




    //cv::imshow("opencvwindows", show_image);
    cout << "show cvtColor mat\n";
    waitKey(10000);

    cv::namedWindow("opencvwindows", WINDOW_AUTOSIZE);
    cv::imshow("opencvwindows", mat2);


    waitKey(0);

    //cv::destroyAllWindows();

    return 0;
}

// ���г���: Ctrl + F5 ����� >����ʼִ��(������)���˵�
// ���Գ���: F5 ����� >����ʼ���ԡ��˵�

// ����ʹ�ü���: 
//   1. ʹ�ý��������Դ�������������/�����ļ�
//   2. ʹ���Ŷ���Դ�������������ӵ�Դ�������
//   3. ʹ��������ڲ鿴���������������Ϣ
//   4. ʹ�ô����б��ڲ鿴����
//   5. ת������Ŀ��>���������Դ����µĴ����ļ�����ת������Ŀ��>�����������Խ����д����ļ���ӵ���Ŀ
//   6. ��������Ҫ�ٴδ򿪴���Ŀ����ת�����ļ���>���򿪡�>����Ŀ����ѡ�� .sln �ļ�
