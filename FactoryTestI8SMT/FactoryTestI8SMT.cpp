// FactoryTestI8SMT.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "FactoryTestI8SMT.h"
#include "FactoryTestI8SMTDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFactoryTestI8SMTApp

BEGIN_MESSAGE_MAP(CFactoryTestI8SMTApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CFactoryTestI8SMTApp ����

CFactoryTestI8SMTApp::CFactoryTestI8SMTApp()
{
    // TODO: �ڴ˴���ӹ�����룬
    // ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CFactoryTestI8SMTApp ����

CFactoryTestI8SMTApp theApp;


// CFactoryTestI8SMTApp ��ʼ��

BOOL CFactoryTestI8SMTApp::InitInstance()
{
    // ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
    // ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
    // ����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
    // �����ؼ��ࡣ
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    HWND hwnd = FindWindow("#32770", "FactoryTestI8SMT");
    if (hwnd) {
        SwitchToThisWindow(hwnd, TRUE);
        return FALSE;
    }

    if (!AfxSocketInit()) {
        AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
        return FALSE;
    }

    // ��׼��ʼ��
    // ���δʹ����Щ���ܲ�ϣ����С
    // ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
    // ����Ҫ���ض���ʼ������
    // �������ڴ洢���õ�ע�����
    // TODO: Ӧ�ʵ��޸ĸ��ַ�����
    // �����޸�Ϊ��˾����֯��
    SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

    CFactoryTestI8SMTDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
        // TODO: �ڴ˴����ô����ʱ�á�ȷ�������ر�
        // �Ի���Ĵ���
    } else if (nResponse == IDCANCEL) {
        // TODO: �ڴ˷��ô����ʱ�á�ȡ�������ر�
        // �Ի���Ĵ���
    }

    // ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
    // ����������Ӧ�ó������Ϣ�á�
    return FALSE;
}
