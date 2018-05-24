// FactoryTestI8FocusDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FactoryTestI8Focus.h"
#include "FactoryTestI8FocusDlg.h"
#include "TestNetProtocol.h"
#include "BenQGuruDll.h"
#include "log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ENABLE_MES_SYSTEM   TRUE
#define TIMER_ID_SCAN_NEXT  1

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
    , m_strScanSN(_T(""))
    , m_strCurSN(_T(""))
    , m_strTestInfo(_T(""))
    , m_strDeviceIP(_T("no device"))
    , m_pTnpContext(NULL)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFactoryTestI8FocusDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_TXT_MES_LOGIN, m_strMesLoginState);
    DDX_Text(pDX, IDC_TXT_MES_RESOURCE, m_strMesResource);
    DDX_Text(pDX, IDC_EDT_SCAN_SN, m_strScanSN);
    DDX_Text(pDX, IDC_EDT_CUR_SN, m_strCurSN);
    DDX_Text(pDX, IDC_TXT_TEST_INFO, m_strTestInfo);
    DDX_Text(pDX, IDC_TXT_DEVICE_IP, m_strDeviceIP);
}

BEGIN_MESSAGE_MAP(CFactoryTestI8FocusDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_DRAWITEM()
    ON_WM_CTLCOLOR()
    ON_WM_QUERYDRAGICON()
    ON_WM_DESTROY()
    ON_WM_CLOSE()
    ON_EN_CHANGE(IDC_EDT_SCAN_SN, &CFactoryTestI8FocusDlg::OnEnChangeEdtScanSn)
    ON_BN_CLICKED(IDC_BTN_TEST_RESULT, &CFactoryTestI8FocusDlg::OnBnClickedBtnTestResult)
    ON_BN_CLICKED(IDC_BTN_UPLOAD, &CFactoryTestI8FocusDlg::OnBnClickedBtnUpload)
    ON_MESSAGE(WM_TNP_DEVICE_FOUND, &CFactoryTestI8FocusDlg::OnTnpDeviceFound)
    ON_MESSAGE(WM_TNP_DEVICE_LOST , &CFactoryTestI8FocusDlg::OnTnpDeviceLost )
END_MESSAGE_MAP()


// CFactoryTestI8FocusDlg ��Ϣ�������

BOOL CFactoryTestI8FocusDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
    // ִ�д˲���
    SetIcon(m_hIcon, TRUE);         // ���ô�ͼ��
    SetIcon(m_hIcon, FALSE);        // ����Сͼ��
    m_fntTestResult.CreatePointFont(255, TEXT("����"), NULL);

    // �ڴ���Ӷ���ĳ�ʼ������
    strcpy(m_strUserName  , "username"      );
    strcpy(m_strPassWord  , "password"      );
    strcpy(m_strResource  , "resource"      );
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
    m_strTestInfo       = TEXT("��ɨ������...");
    m_bSnScaned         = FALSE;
    m_nFocusTestResult  = -1;
    UpdateData(FALSE);

    m_pTnpContext = tnp_init(GetSafeHwnd());
    return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CFactoryTestI8FocusDlg::OnDestroy()
{
    CDialog::OnDestroy();

    tnp_free(m_pTnpContext);
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
    }
    return hbr;
}

int CFactoryTestI8FocusDlg::GetBackColorByCtrlId(int id)
{
    int result = -1;
    switch (id) {
    case IDC_BTN_TEST_RESULT: result = m_nFocusTestResult; break;
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
    case IDC_BTN_TEST_RESULT:
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
            dc.SelectObject(m_fntTestResult);
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
    if (m_strScanSN.GetLength() >= 15) {
        m_strCurSN  = m_strScanSN;
        m_strScanSN = "";

#if ENABLE_MES_SYSTEM
        if (m_bMesLoginOK && stricmp(m_strRouteCheck, "yes") == 0) {
            CString strErrMsg;
            BOOL ret = MesDLL::GetInstance().CheckRoutePassed(m_strCurSN, CString(m_strResource), strErrMsg);
            if (!ret) {
                AfxMessageBox(TEXT("�ù�λû�а���;��������"), MB_OK);
                return;
            }
        }
#endif

        m_bSnScaned   = TRUE;
        m_strTestInfo = TEXT("����е���������...");
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
    if (pMsg->message == WM_KEYDOWN) {
        GetDlgItem(IDC_EDT_SCAN_SN)->SetFocus();
    }
    return CDialog::PreTranslateMessage(pMsg);
}


LRESULT CFactoryTestI8FocusDlg::OnTnpDeviceFound(WPARAM wParam, LPARAM lParam)
{
    if (strcmp(m_strDeviceIP, "no device") != 0) {
        log_printf("already have a device connected !\n");
        return 0;
    }

    struct in_addr addr;
    addr.S_un.S_addr = (u_long)lParam;
    m_strDeviceIP = inet_ntoa(addr);
    UpdateData(FALSE);
    return 0;
}

LRESULT CFactoryTestI8FocusDlg::OnTnpDeviceLost(WPARAM wParam, LPARAM lParam)
{
    struct in_addr addr;
    addr.S_un.S_addr = (u_long)lParam;
    if (strcmp(m_strDeviceIP, inet_ntoa(addr)) != 0) {
        log_printf("this is not current connected device lost !\n");
        return 0;
    }

    m_strDeviceIP = "no device";
    m_bSnScaned   = FALSE;
    m_nFocusTestResult = -1;
    GetDlgItem(IDC_BTN_TEST_RESULT)->SetWindowText("NG");
    UpdateData(FALSE);
    return 0;
}

void CFactoryTestI8FocusDlg::OnBnClickedBtnTestResult()
{
    if (!m_bSnScaned) return;
    if (m_nFocusTestResult != 1) {
        m_nFocusTestResult = 1;
        GetDlgItem(IDC_BTN_TEST_RESULT)->SetWindowText("PASS");
    } else {
        m_nFocusTestResult = 0;
        GetDlgItem(IDC_BTN_TEST_RESULT)->SetWindowText("NG");
    }
    m_strTestInfo = TEXT("���ϴ����Խ��...");
    UpdateData(FALSE);
}

void CFactoryTestI8FocusDlg::OnBnClickedBtnUpload()
{
#if ENABLE_MES_SYSTEM
    CString strTestResult;
    CString strErrCode;
    CString strMO;
    CString strErrMsg;

    if (m_nFocusTestResult == 1) {
        strTestResult = "OK";
    } else {
        strTestResult = "NG";
        strErrCode    = "L008";
    }
    strMO = m_strGongDan;
    if (m_bMesLoginOK) {
        BOOL ret = MesDLL::GetInstance().SetMobileDataWithMO(m_strCurSN, CString(m_strResource), CString(m_strUserName), strTestResult, strErrCode, strMO, strErrMsg);
        if (!ret) {
            if (strErrMsg.Find("CS_Route_Failed_FirstOP") != -1) {
                m_strTestInfo = "�ظ��ɼ�";
            } else {
                m_strTestInfo = "�ϴ����Խ��ʧ�ܣ�";
            }
            AfxMessageBox(m_strTestInfo);
        } else {
            m_strTestInfo = "�ϴ����Խ���ɹ���";
            SetTimer(TIMER_ID_SCAN_NEXT, 2000, NULL);
        }
    }
#endif

    m_bSnScaned = FALSE;
    m_nFocusTestResult = -1;
    GetDlgItem(IDC_BTN_TEST_RESULT)->SetWindowText("NG");
    UpdateData(FALSE);
}

void CFactoryTestI8FocusDlg::OnTimer(UINT_PTR nIDEvent)
{
    switch (nIDEvent) {
    case TIMER_ID_SCAN_NEXT:
        KillTimer(TIMER_ID_SCAN_NEXT);
        m_strTestInfo = "��ɨ������...";
        UpdateData(FALSE);
        break;
    }
    CDialog::OnTimer(nIDEvent);
}

