// FactoryTestI8.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
    #error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CFactoryTestI8App:
// �йش����ʵ�֣������ FactoryTestI8.cpp
//

class CFactoryTestI8App : public CWinApp
{
public:
    CFactoryTestI8App();

// ��д
    public:
    virtual BOOL InitInstance();

// ʵ��

    DECLARE_MESSAGE_MAP()
};

extern CFactoryTestI8App theApp;