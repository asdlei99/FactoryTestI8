// FactoryTestI8FocusDlg.h : ͷ�ļ�
//

#pragma once


// CFactoryTestI8FocusDlg �Ի���
class CFactoryTestI8FocusDlg : public CDialog
{
// ����
public:
    CFactoryTestI8FocusDlg(CWnd* pParent = NULL); // ��׼���캯��

// �Ի�������
    enum { IDD = IDD_FACTORYTESTI8FOCUS_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual void OnCancel();
    virtual void OnOK();

// ʵ��
protected:
    HICON m_hIcon;

    // ���ɵ���Ϣӳ�亯��
    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
    afx_msg void OnClose();
    afx_msg void OnPaint();
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    afx_msg HBRUSH  OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnEnChangeEdtScanSn();
    afx_msg void OnBnClickedBtnTestResult();
    afx_msg void OnBnClickedBtnUpload();
    DECLARE_MESSAGE_MAP()

private:
    CString m_strMesLoginState;
    CString m_strMesResource;
    CString m_strTestInfo;
    CString m_strScanSN;
    CString m_strCurSN;
    char    m_strUserName  [32];
    char    m_strPassWord  [32];
    char    m_strResource  [32];
    char    m_strGongDan   [32];
    char    m_strLoginMode [32];
    char    m_strRouteCheck[32];
    char    m_strLogFile   [32];
    BOOL    m_bMesLoginOK;
    BOOL    m_bSnScaned;
    int     m_nFocusTestResult;
    CFont   m_fntTestResult;

private:
    int GetBackColorByCtrlId(int id);
};
