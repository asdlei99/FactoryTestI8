// FactoryTestI8FullDlg.h : ͷ�ļ�
//

#pragma once


// CFactoryTestI8FullDlg �Ի���
class CFactoryTestI8FullDlg : public CDialog
{
// ����
public:
	CFactoryTestI8FullDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FACTORYTESTI8FULL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
