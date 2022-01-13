#include "pch.h"
#include "ImageDetectionTensorflow.h"


//��̬��Ա��ʼ��
AssistConfig* ImageDetectionTensorflow::m_AssistConfig = AssistConfig::GetInstance();


//�෽��ʵ��
ImageDetectionTensorflow::ImageDetectionTensorflow()
{
    initImg();

    //����AIģ��
    initDnn();
}

ImageDetectionTensorflow::~ImageDetectionTensorflow()
{
    //ͼ����Դ�ͷ�
    releaseImg();

    //�ͷ���Դ����
    try {
        if (m_net != NULL) {
            delete m_net;
            m_net = NULL;
        }
    }
    catch (Exception ex) {
        string msg = "";
    }
}

//�޸����ú���Ҫ���³�ʼ��һЩ����
void ImageDetectionTensorflow::ReInit() {
    releaseImg();
    initImg();
}

//��ʼ��ͼ����Դ
void ImageDetectionTensorflow::initImg() {
    //ע����Ļ���ź�����
    //ע��ץȡ��Ļ��ʱ��ʹ�����ź�������������꣬ץȡ��������ʵ�����߼��ֱ�������
    cv::Rect detectRect = m_AssistConfig->detectRect;
    cv::Rect detectZoomRect = m_AssistConfig->detectZoomRect;


    // ��ȡ��Ļ DC
    m_screenDC = GetDC(HWND_DESKTOP);
    m_memDC = CreateCompatibleDC(m_screenDC);
    // ����λͼ
    m_hBitmap = CreateCompatibleBitmap(m_screenDC, detectRect.width, detectRect.height);
    SelectObject(m_memDC, m_hBitmap);

    //����λͼ��Ϣͷ
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
    m_Bitmapinfo = new BITMAPINFO{ {sizeof(BITMAPINFOHEADER), detectRect.width, -detectRect.height, 1, wBitCount, BI_RGB },{0,0,0,0} };

    //�������ͼ�����ݵ�mat
    //m_mat.create(detectRect.height, detectRect.width, CV_8UC4);
    //m_mat3.create(detectRect.height, detectRect.width, CV_8UC3);
}

//�ͷ�ͼ����Դ
void ImageDetectionTensorflow::releaseImg() {

    //��Դ�ͷ�
    try {
        m_mat_src.release();
        m_mat_3.release();
        img_mat.release();

        if (m_Bitmapinfo != NULL)
            delete m_Bitmapinfo;
        DeleteObject(m_hBitmap);
        DeleteDC(m_memDC);
        ReleaseDC(HWND_DESKTOP, m_screenDC);
    }
    catch (Exception ex) {
        string msg = "";
    }

    m_Bitmapinfo = NULL;
    m_hBitmap = NULL;
    m_memDC = NULL;
    m_screenDC = NULL;
}


/* ��ʼ��ģ�� */
void ImageDetectionTensorflow::initDnn() {
    //����gpu��Դ�������ã�ʹ��10%gpu�ڴ棬�����ڴ�������Ϸ
    /**/
   // Read more to see how to obtain the serialized options
    std::vector<uint8_t> config{ {0x32,0x9,0x9,0x9a,0x99,0x99,0x99,0x99,0x99,0xb9,0x3f} };
    // Create new options with your configuration
    TFE_ContextOptions* options = TFE_NewContextOptions();
    //TFE_ContextOptionsSetConfig(options, config.data(), config.size(), cppflow::context::get_status());
    TFE_ContextOptionsSetConfig(options, config.data(), config.size(), cppflow::context::get_status());
    // Replace the global context with your options
    cppflow::get_global_context() = cppflow::context(options);

    // ����ģ���ļ�
    //opencv��dnnģ��(NVIDIA GPU������ģ��)
    m_net = new cppflow::model(ModelFile);

    // ���ط����ǩ
    ifstream fin(LabelFile);
    if (fin.is_open())
    {
        string className = "";
        while (std::getline(fin, className))
            m_classLabels.push_back(className);
    }


    // ����CUDA����
    //(*m_net).setPreferableBackend(dnn::DNN_BACKEND_OPENCV);
    //(*m_net).setPreferableTarget(dnn::DNN_TARGET_CPU);

    return;
}


/* ��ȡ���������Ļ��ͼ */
void ImageDetectionTensorflow::getScreenshot()
{
    //������Ļ���ź�ģ��ü����ʵ��ͼ��������
    //ע��ץȡ��Ļ��ʱ��ʹ�����ź�������������꣬ץȡ��������ʵ�����߼��ֱ�������
    cv::Rect detectRect = m_AssistConfig->detectRect;
    cv::Rect detectZoomRect = m_AssistConfig->detectZoomRect;


    // �õ�λͼ������
    // ʹ��BitBlt��ͼ�����ܽϵͣ������޸�ΪDXGI
    //Windows8�Ժ�΢��������һ���µĽӿڣ��С�Desktop Duplication API����Ӧ�ó��򣬿���ͨ������API�����������ݡ�
    //����Desktop Duplication API��ͨ��Microsoft DirectX Graphics Infrastructure (DXGI)���ṩ����ͼ��ģ��ٶȷǳ��졣
    bool opt = BitBlt(m_memDC, 0, 0, detectRect.width, detectRect.height, m_screenDC, detectZoomRect.x, detectZoomRect.y, SRCCOPY);

    //ע���ڷ�opencv������ʹ��matʱ����Ҫ�ֶ�����create�����ڴ洴�����飻�ظ�ִ��create�������ᵼ���ظ��������ݴ�����飻
    m_mat_src.create(detectRect.height, detectRect.width, CV_8UC4);
    //int rows = GetDIBits(m_screenDC, m_hBitmap, 0, detectRect.height, m_mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
    int rows = GetDIBits(m_memDC, m_hBitmap, 0, detectRect.height, m_mat_src.data, (BITMAPINFO*)m_Bitmapinfo, DIB_RGB_COLORS);

    //cv::namedWindow("opencvwindows", WINDOW_AUTOSIZE);
    //cv::imshow("opencvwindows", m_mat_src);
    //waitKey(2000);

    //��Ļ��ͼ����Ƶ��ͼ��ʽ��һ������Ҫ����ͼ���ʽת��
    //ȥ��alpha ֵ(͸����)ͨ����ת��ΪCV_8UC3��ʽ
    cv::cvtColor(m_mat_src, m_mat_3, COLOR_RGBA2RGB);

    //���ݸ�ʽת��
    m_mat_3.convertTo(img_mat, CV_8UC3, 1.0);

    return;
}


/* AI���� */
DETECTRESULTS ImageDetectionTensorflow::detectImg()
{
    //������Ļ���ź�ģ��ü����ʵ��ͼ��������
    //ע��ץȡ��Ļ��ʱ��ʹ�����ź�������������꣬ץȡ��������ʵ�����߼��ֱ�������
    cv::Rect detectRect = m_AssistConfig->detectRect;
    cv::Rect detectZoomRect = m_AssistConfig->detectZoomRect;
    int gameIndex = m_AssistConfig->gameIndex;
    int playerCentX = m_AssistConfig->playerCentX;

    auto classIds = vector<float_t>();
    auto confidences = vector<float_t>();
    auto boxes_float = vector<float_t>();
    auto boxes = vector<Rect>();
    DETECTRESULTS out;

    std::vector<uint8_t > img_data;

    try
    {
        img_data.assign(img_mat.data, img_mat.data + detectRect.width * detectRect.height * 3);
        cppflow::tensor input(img_data, { 1, detectRect.height, detectRect.width, 3 });

        //ִ��ģ������
        auto model = *m_net;
        //efficientdet-lite0
        auto result = model({ {"serving_default_images:0", input} }, { "StatefulPartitionedCall:0", "StatefulPartitionedCall:1", "StatefulPartitionedCall:2" });
        //ssd_mobilenet_v2
        //auto result = model({ {"serving_default_input_tensor:0", input} }, { "StatefulPartitionedCall:1", "StatefulPartitionedCall:2", "StatefulPartitionedCall:4" });


        //����������
        //efficientdet-lite0
        classIds = result[2].get_data<float_t>();
        confidences = result[1].get_data<float_t>();
        boxes_float = result[0].get_data<float_t>();
        //ssd_mobilenet_v2
        //classIds = result[1].get_data<float_t>();
        //confidences = result[2].get_data<float_t>();
        //boxes_float = result[0].get_data<float_t>();

        //efficientdetģ�͵��������Ѿ��ź�����
        out.maxPersonConfidencePos = 0; //������Ŷ�����λ��
        for (int i = 0; i < classIds.size() && i < 100; i++) {

            //��������������͡����ŶȺ�����
            int classid = (int)classIds.at(i);
            float confidence = confidences.at(i);
            if (classid == PersonClassId && confidence > MinConfidence) {
                //�������������ļ�����
                //efficientdet-lite object_detection���box�������ʽΪ[ymin , xmin , ymax , xmax] 
                Rect box;
                //efficientdet-lite0
                box.y = boxes_float[i*4];
                box.x = boxes_float[i*4 + 1];
                box.height = boxes_float[i*4 + 2] - box.y;
                box.width = boxes_float[i*4 + 3] - box.x;
                //ssd_mobilenet_v2
                //box.y = boxes_float[i * 4] * detectRect.height;
                //box.x = boxes_float[i * 4 + 1] * detectRect.width;
                //box.height = boxes_float[i * 4 + 2] * detectRect.height - box.y;
                //box.width = boxes_float[i * 4 + 3] * detectRect.width - box.x;

                //Ϊ������Ŀ���ų�̫�����̫С��ģ��
                if (box.width <= 220 && box.width >= 10 && box.height <= 280 && box.height >= 10)
                {
                    //�ж��Ƿ�����Ϸ���߱���,ģ��λ��Ϊ��Ļ��Ϸ��λ��
                    //��Ϸ�ߵ�λ������Ļ�·�����һ�㣬��� 860/1920��
                    //������Ϸ������ҡ�ڷ��Ƚϴ�����x��ļ���ֵҪ���ô�һЩ��   
                    //ע��box������ʵ�ʼ������������������
                    /*
                    if (gameIndex == 0 &&  //����������Ϸ����Ҫ���⴦��
                        box.width > 70 && box.width < 160 &&   //ģ�Ϳ�ȴ���60С��150
                        box.height < detectRect.height / 2 &&   //ģ�͸߶�С�ڼ������Ķ���֮һ
                        abs((box.y + box.height) - (detectRect.height)) <= 16 &&  //ģ�Ϳ�����������·�
                        abs((box.x + box.width / 2) - (playerCentX)) <= 160  // ģ�����ĵ�ӽ����λ�����ĵ�                      
                        )
                    {
                        //�ų���Ϸ���Լ�
                        //var testi = 0;
                    }
                    else
                    {
                        //���������⵽�Ķ���
                        out.classIds.push_back(classid);
                        out.confidences.push_back(confidence);
                        out.boxes.push_back(box);
                    }
                    */

                    //���������⵽�Ķ���
                    out.classIds.push_back(classid);
                    out.confidences.push_back(confidence);
                    out.boxes.push_back(box);
                }
            }
        }

    }
    catch (Exception ex) {
        string msg = "";
    }

    return out;
}


cv::Mat ImageDetectionTensorflow::getImg() {
    //��¡mat���ݽ��ⲿ����ʹ�ã����������ֻ��������mat�������ǵ������ڴ���
    Mat mat = m_mat_3.clone();
    return mat;
}