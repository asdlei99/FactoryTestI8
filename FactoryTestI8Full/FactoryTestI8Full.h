// FactoryTestI8Full.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
    #error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CFactoryTestI8FullApp:
// �йش����ʵ�֣������ FactoryTestI8Full.cpp
//

class CFactoryTestI8FullApp : public CWinApp
{
public:
    CFactoryTestI8FullApp();

// ��д
public:
    virtual BOOL InitInstance();

// ʵ��

    DECLARE_MESSAGE_MAP()
};

extern CFactoryTestI8FullApp theApp;