// FactoryTestI8Focus.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
    #error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CFactoryTestI8Focus:
// �йش����ʵ�֣������ FactoryTestI8Focus.cpp
//

class CFactoryTestI8FocusApp : public CWinApp
{
public:
    CFactoryTestI8FocusApp();

// ��д
public:
    virtual BOOL InitInstance();

// ʵ��

    DECLARE_MESSAGE_MAP()
};

extern CFactoryTestI8FocusApp theApp;