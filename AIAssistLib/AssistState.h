#pragma once
#include<string>
#include<iostream>

#include "AssistConfig.h"

using namespace std;


// ����״̬��Ϣģ���࣬ά����������״̬������ģʽ
class AssistState
{
public:
    // ���ö���
    static AssistConfig* m_AssistConfig;

    // ��ӡ������Ϣ
    static string getStatInf();

};

