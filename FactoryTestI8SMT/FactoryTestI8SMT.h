// FactoryTestI8SMT.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
    #error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CFactoryTestI8SMTApp:
// �йش����ʵ�֣������ FactoryTestI8SMT.cpp
//

class CFactoryTestI8SMTApp : public CWinApp
{
public:
    CFactoryTestI8SMTApp();

// ��д
public:
    virtual BOOL InitInstance();

// ʵ��

    DECLARE_MESSAGE_MAP()
};

extern CFactoryTestI8SMTApp theApp;