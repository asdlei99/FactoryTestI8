// FactoryTestI8SN.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
    #error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CFactoryTestI8SNApp:
// �йش����ʵ�֣������ FactoryTestI8SN.cpp
//

class CFactoryTestI8SNApp : public CWinApp
{
public:
    CFactoryTestI8SNApp();

// ��д
    public:
    virtual BOOL InitInstance();

// ʵ��

    DECLARE_MESSAGE_MAP()
};

extern CFactoryTestI8SNApp theApp;