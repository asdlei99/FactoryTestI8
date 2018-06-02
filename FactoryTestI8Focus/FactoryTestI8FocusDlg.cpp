// FactoryTestI8FocusDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FactoryTestI8Focus.h"
#include "FactoryTestI8FocusDlg.h"
#include "BenQGuruDll.h"
#include "fanplayer.h"
#include "log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ENABLE_MES_SYSTEM   TRUE
#define TIMER_ID_SET_FOCUS  2

static void get_app_dir(char *path, int size)
{
    HMODULE handle = GetModuleHandle(NULL);
    GetModuleFileNameA(handle, path, size);
    char  *str = path + strlen(path);
    while (*--str != '\\');
    *str = '\0';
}

static void parse_params(const char *str, const char *key, char *val)
{
    char *p = (char*)strstr(str, key);
    int   i;

    if (!p) return;
    p += strlen(key);
    if (*p == '\0') return;

    while (1) {
        if (*p != ' ' && *p != '=' && *p != ':') break;
        else p++;
    }

    for (i=0; i<32; i++) {
        if (*p == ',' || *p == ';' || *p == '\r' || *p == '\n' || *p == '\0') {
            val[i] = '\0';
            break;
        } else {
            val[i] = *p++;
        }
    }
}

static int load_config_from_file(char *user, char *passwd, char *res, char *gongdan, char *login, char *route, char *log)
{
    char  file[MAX_PATH];
    FILE *fp = NULL;
    char *buf= NULL;
    int   len= 0;

    // open params file
    get_app_dir(file, MAX_PATH);
    strcat(file, "\\factorytesti8focus.ini");
    fp = fopen(file, "rb");

    if (fp) {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        buf = (char*)malloc(len);
        if (buf) {
            fseek(fp, 0, SEEK_SET);
            fread(buf, len, 1, fp);
            parse_params(buf, "username"  , user   );
            parse_params(buf, "password"  , passwd );
            parse_params(buf, "resource"  , res    );
            parse_params(buf, "gongdan"   , gongdan);
            parse_params(buf, "loginmode" , login  );
            parse_params(buf, "routecheck", route  );
            parse_params(buf, "logfile"   , log    );
            free(buf);
        }
        fclose(fp);
        return 0;
    }

    return -1;
}

// CFactoryTestI8FocusDlg �Ի���

CFactoryTestI8FocusDlg::CFactoryTestI8FocusDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CFactoryTestI8FocusDlg::IDD, pParent)
    , m_strMesLoginState(_T(""))
    , m_strMesResource(_T(""))
    , m_strMesGongDan(_T(""))
    , m_strScanSN(_T(""))
    , m_strCurSN(_T(""))
    , m_strTestInfo(_T(""))
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFactoryTestI8FocusDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_TXT_MES_LOGIN, m_strMesLoginState);
    DDX_Text(pDX, IDC_TXT_MES_RESOURCE, m_strMesResource);
    DDX_Text(pDX, IDC_TXT_MES_GONGDAN, m_strMesGongDan);
    DDX_Text(pDX, IDC_EDT_SCAN_SN, m_strScanSN);
    DDX_Text(pDX, IDC_EDT_CUR_SN, m_strCurSN);
    DDX_Text(pDX, IDC_TXT_TEST_INFO, m_strTestInfo);
}

BEGIN_MESSAGE_MAP(CFactoryTestI8FocusDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_DRAWITEM()
    ON_WM_CTLCOLOR()
    ON_WM_QUERYDRAGICON()
    ON_WM_DESTROY()
    ON_WM_CLOSE()
    ON_WM_TIMER()
    ON_EN_CHANGE(IDC_EDT_SCAN_SN, &CFactoryTestI8FocusDlg::OnEnChangeEdtScanSn)
    ON_BN_CLICKED(IDC_BTN_TEST_RESULT1, &CFactoryTestI8FocusDlg::OnBnClickedBtnTestResult1)
    ON_BN_CLICKED(IDC_BTN_TEST_RESULT2, &CFactoryTestI8FocusDlg::OnBnClickedBtnTestResult2)
    ON_BN_CLICKED(IDC_BTN_TEST_RESULT3, &CFactoryTestI8FocusDlg::OnBnClickedBtnTestResult3)
    ON_BN_CLICKED(IDC_BTN_UPLOAD, &CFactoryTestI8FocusDlg::OnBnClickedBtnUpload)
END_MESSAGE_MAP()


// CFactoryTestI8FocusDlg ��Ϣ�������

BOOL CFactoryTestI8FocusDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
    // ִ�д˲���
    SetIcon(m_hIcon, TRUE);         // ���ô�ͼ��
    SetIcon(m_hIcon, FALSE);        // ����Сͼ��
//  m_fntTestResult.CreatePointFont(160, TEXT("����"), NULL);

    // �ڴ���Ӷ���ĳ�ʼ������
    strcpy(m_strUserName  , "username"      );
    strcpy(m_strPassWord  , "password"      );
    strcpy(m_strResource  , "resource"      );
    strcpy(m_strGongDan   , "gongdan"       );
    strcpy(m_strLoginMode , "alert_and_exit");
    strcpy(m_strRouteCheck, "yes"           );
    strcpy(m_strLogFile   , "DEBUGER"       );
    int ret = load_config_from_file(m_strUserName, m_strPassWord, m_strResource, m_strGongDan, m_strLoginMode, m_strRouteCheck, m_strLogFile);
    if (ret != 0) {
        AfxMessageBox(TEXT("�޷��򿪲��������ļ���"), MB_OK);
    }
    log_init(m_strLogFile);
    log_printf("username = %s\n", m_strUserName);
    log_printf("password = %s\n", m_strPassWord);
    log_printf("resource = %s\n", m_strResource);
    log_printf("gongdan  = %s\n", m_strGongDan );
    log_printf("logfile  = %s\n", m_strLogFile );

#if ENABLE_MES_SYSTEM
    CString strJigCode;
    CString strErrMsg;
    m_bMesLoginOK = MesDLL::GetInstance().CheckUserAndResourcePassed (CString(m_strUserName), CString(m_strResource), CString(m_strPassWord), strJigCode, strErrMsg);
    if (strcmp(m_strLoginMode, "alert_and_exit") == 0) {
        if (!m_bMesLoginOK) {
            AfxMessageBox(TEXT("\n��¼ MES ϵͳʧ�ܣ�\n\n�����������ú� MES ϵͳ��Ȼ�����ԡ�\n\nлл��"), MB_OK);
            EndDialog(IDCANCEL); return FALSE;
        }
    }
    m_strMesLoginState = m_bMesLoginOK ? "��¼ MES �ɹ�" : "��¼ MES ʧ��";
#endif

    m_strMesResource    = CString(m_strResource);
    m_strMesGongDan     = CString(m_strGongDan );
    m_strTestInfo       = TEXT("��ɨ������..." );
    m_bSnScaned         = FALSE;
    m_nFocusTestResult1 = -1;
    m_nFocusTestResult2 = -1;
    m_nFocusTestResult3 = -1;
    UpdateData(FALSE);

    SetTimer(TIMER_ID_SET_FOCUS, 1000, NULL);
    return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CFactoryTestI8FocusDlg::OnDestroy()
{
    CDialog::OnDestroy();

    log_done();

#if ENABLE_MES_SYSTEM
    CString strErrMsg;
    BOOL ret = MesDLL::GetInstance().ATELogOut(CString(m_strResource), strErrMsg);
    if (!ret) {
        log_printf("MesDLL ATELogOut failed !\n");
    }
#endif
}

// ���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
//
HCURSOR CFactoryTestI8FocusDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

// �����Ի��������С����ť������Ҫ����Ĵ���
// �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
// �⽫�ɿ���Զ���ɡ�

void CFactoryTestI8FocusDlg::OnPaint()
{
    if (IsIconic()) {
        CPaintDC dc(this); // ���ڻ��Ƶ��豸������

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // ʹͼ���ڹ��������о���
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // ����ͼ��
        dc.DrawIcon(x, y, m_hIcon);
    } else {
        CDialog::OnPaint();
    }
}

HBRUSH CFactoryTestI8FocusDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    switch (pWnd->GetDlgCtrlID()) {
    case IDC_TXT_MES_LOGIN:
        pDC->SetTextColor(m_bMesLoginOK ? RGB(0, 180, 0) : RGB(255, 0, 0));
        break;
    case IDC_TXT_TEST_INFO:
        if (m_strTestInfo.Find("�ɹ�") != -1) {
            pDC->SetTextColor(RGB(0, 180, 0));
        } else if (m_strTestInfo.Find("ʧ��") != -1) {
            pDC->SetTextColor(RGB(255, 0, 0));
        } else {
            pDC->SetTextColor(RGB(0, 120, 255));
        }
        break;
    }
    return hbr;
}

int CFactoryTestI8FocusDlg::GetBackColorByCtrlId(int id)
{
    int result = -1;
    switch (id) {
    case IDC_BTN_TEST_RESULT1: result = m_nFocusTestResult1; break;
    case IDC_BTN_TEST_RESULT2: result = m_nFocusTestResult2; break;
    case IDC_BTN_TEST_RESULT3: result = m_nFocusTestResult3; break;
    }

    switch (result) {
    case 0:  return RGB(255, 0, 0);
    case 1:  return RGB(0, 255, 0);
    default: return RGB(236, 233, 216);
    }
}

void CFactoryTestI8FocusDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    switch (nIDCtl) {
    case IDC_BTN_TEST_RESULT1:
    case IDC_BTN_TEST_RESULT2:
    case IDC_BTN_TEST_RESULT3:
        {
            RECT rect;
            CDC  dc;
            rect = lpDrawItemStruct->rcItem;
            dc.Attach(lpDrawItemStruct->hDC);
            dc.FillSolidRect(&rect, GetBackColorByCtrlId(nIDCtl));
            dc.Draw3dRect(&rect, RGB(255, 255, 255), RGB(0, 0, 0));
            dc.SetBkMode(TRANSPARENT);
            dc.SetTextColor(RGB(0 ,0, 0));
            TCHAR buffer[MAX_PATH] = {0};
            ::GetWindowText(lpDrawItemStruct->hwndItem, buffer, MAX_PATH);
//          dc.SelectObject(m_fntTestResult);
            dc.DrawText(buffer, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
            dc.Detach();
        }
        break;
    }
    CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CFactoryTestI8FocusDlg::OnEnChangeEdtScanSn()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
    UpdateData(TRUE);
    if (m_strScanSN.GetLength() >= 16) {
        m_strCurSN  = m_strScanSN.Trim();
        m_strScanSN = "";
        m_bSnScaned = TRUE;

#if ENABLE_MES_SYSTEM
        if (m_bMesLoginOK && stricmp(m_strRouteCheck, "yes") == 0) {
            CString strErrMsg;
            BOOL ret = MesDLL::GetInstance().CheckRoutePassed(m_strCurSN, CString(m_strResource), strErrMsg);
            if (!ret && strErrMsg.Compare("SN_Not_Exist") != 0) {
                AfxMessageBox(CString("�ù�λû�а���;��������\r\n") + strErrMsg, MB_OK);
                UpdateData(FALSE);
                return;
            }
        }
#endif

        m_strTestInfo = TEXT("������齹����...");
        UpdateData(FALSE);
    }
}

void CFactoryTestI8FocusDlg::OnCancel() {}
void CFactoryTestI8FocusDlg::OnOK()     {}

void CFactoryTestI8FocusDlg::OnClose()
{
    CDialog::OnClose();
    EndDialog(IDCANCEL);
}

BOOL CFactoryTestI8FocusDlg::PreTranslateMessage(MSG *pMsg)
{
    if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP) {
        switch (pMsg->wParam) {
        case 'Z'     : if (pMsg->message == WM_KEYDOWN) OnBnClickedBtnTestResult1(); return TRUE;
        case 'X'     : if (pMsg->message == WM_KEYDOWN) OnBnClickedBtnTestResult2(); return TRUE;
        case 'C'     : if (pMsg->message == WM_KEYDOWN) OnBnClickedBtnTestResult3(); return TRUE;
        case VK_SPACE: if (pMsg->message == WM_KEYDOWN) OnBnClickedBtnUpload     (); return TRUE;
        }
    }
    return CDialog::PreTranslateMessage(pMsg);
}

void CFactoryTestI8FocusDlg::OnBnClickedBtnTestResult1()
{
    GetDlgItem(IDC_EDT_SCAN_SN)->SetFocus();
    if (m_nFocusTestResult1 != 1) {
        m_nFocusTestResult1 = 1;
        GetDlgItem(IDC_BTN_TEST_RESULT1)->SetWindowText("PASS");
    } else {
        m_nFocusTestResult1 = 0;
        GetDlgItem(IDC_BTN_TEST_RESULT1)->SetWindowText("NG");
    }
    UpdateData(FALSE);
}

void CFactoryTestI8FocusDlg::OnBnClickedBtnTestResult2()
{
    GetDlgItem(IDC_EDT_SCAN_SN)->SetFocus();
    if (m_nFocusTestResult2 != 1) {
        m_nFocusTestResult2 = 1;
        GetDlgItem(IDC_BTN_TEST_RESULT2)->SetWindowText("PASS");
    } else {
        m_nFocusTestResult2 = 0;
        GetDlgItem(IDC_BTN_TEST_RESULT2)->SetWindowText("NG");
    }
    UpdateData(FALSE);
}

void CFactoryTestI8FocusDlg::OnBnClickedBtnTestResult3()
{
    GetDlgItem(IDC_EDT_SCAN_SN)->SetFocus();
    if (m_nFocusTestResult3 != 1) {
        m_nFocusTestResult3 = 1;
        GetDlgItem(IDC_BTN_TEST_RESULT3)->SetWindowText("PASS");
    } else {
        m_nFocusTestResult3 = 0;
        GetDlgItem(IDC_BTN_TEST_RESULT3)->SetWindowText("NG");
    }
    UpdateData(FALSE);
}

void CFactoryTestI8FocusDlg::OnBnClickedBtnUpload()
{
    GetDlgItem(IDC_EDT_SCAN_SN)->SetFocus();
    if (!m_bSnScaned) return;

#if ENABLE_MES_SYSTEM
    CString strTestResult;
    CString strErrCode;
    CString strMO;
    CString strErrMsg;

    if (m_nFocusTestResult1 == 1 && m_nFocusTestResult2 == 1 && m_nFocusTestResult3 == 1) {
        strTestResult = "OK";
    } else {
        strTestResult = "NG";
    }

    if (m_nFocusTestResult1 != 1) {
        strErrCode    = "L017";
    }
    if (m_nFocusTestResult2 != 1) {
        strErrCode    = "L015";
    }
    if (m_nFocusTestResult3 != 1) {
        strErrCode    = "L016";
    }

    strMO = m_strGongDan;
    if (m_bMesLoginOK) {
        BOOL ret = MesDLL::GetInstance().SetMobileDataWithMO(m_strCurSN, CString(m_strResource), CString(m_strUserName), strTestResult, strErrCode, strMO, strErrMsg);
        if (!ret) {
            if (strErrMsg.Find("CS_Route_Failed_FirstOP") != -1) {
                m_strTestInfo = "��Դ�����";
            }
            else if (strErrMsg.Find("CS_ID_Has_Already_Belong_To_This_MO") != -1) {
                m_strTestInfo = "�ظ��ɼ���";
            } else if (strErrMsg.Find("CS_MO_NOT_EXIST") != -1) {
                m_strTestInfo = "��������";
            } else {
                m_strTestInfo = "�ϴ����Խ��ʧ�ܣ�";
            }
        } else {
            m_strTestInfo = "�ϴ����Խ���ɹ���";
        }
    } else {
        m_strTestInfo = "�ϴ�ʧ�ܣ�MES ϵͳδ��¼��";
    }
    AfxMessageBox(m_strTestInfo + "\r\n" + strErrMsg);
#endif

    m_strTestInfo = "��ɨ������...";
    m_bSnScaned   = FALSE;
    m_nFocusTestResult1 = -1;
    m_nFocusTestResult2 = -1;
    m_nFocusTestResult3 = -1;
    GetDlgItem(IDC_BTN_TEST_RESULT1)->SetWindowText("NG");
    GetDlgItem(IDC_BTN_TEST_RESULT2)->SetWindowText("NG");
    GetDlgItem(IDC_BTN_TEST_RESULT3)->SetWindowText("NG");
    UpdateData(FALSE);
}

void CFactoryTestI8FocusDlg::OnTimer(UINT_PTR nIDEvent)
{
    switch (nIDEvent) {
    case TIMER_ID_SET_FOCUS:
        GetDlgItem(IDC_EDT_SCAN_SN)->SetFocus();
        break;
    }
    CDialog::OnTimer(nIDEvent);
}

