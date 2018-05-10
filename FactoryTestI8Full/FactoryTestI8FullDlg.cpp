// FactoryTestI8FullDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FactoryTestI8Full.h"
#include "FactoryTestI8FullDlg.h"
#include "TestNetProtocol.h"
#include "BenQGuruDll.h"
#include "fanplayer.h"
#include "log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ENABLE_MES_SYSTEM  TRUE

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

static int load_config_from_file(char *user, char *passwd, char *res, char *ver, char *login, char *route, char *log)
{
    char  file[MAX_PATH];
    FILE *fp = NULL;
    char *buf= NULL;
    int   len= 0;

    // open params file
    get_app_dir(file, MAX_PATH);
    strcat(file, "\\factorytesti8full.ini");
    fp = fopen(file, "rb");

    if (fp) {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        buf = (char*)malloc(len);
        if (buf) {
            fseek(fp, 0, SEEK_SET);
            fread(buf, len, 1, fp);
            parse_params(buf, "username"  , user  );
            parse_params(buf, "password"  , passwd);
            parse_params(buf, "resource"  , res   );
            parse_params(buf, "version"   , ver   );
            parse_params(buf, "loginmode" , login );
            parse_params(buf, "routecheck", route );
            parse_params(buf, "logfile"   , log   );
            free(buf);
        }
        fclose(fp);
        return 0;
    }

    return -1;
}

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// �Ի�������
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CFactoryTestI8FullDlg �Ի���

CFactoryTestI8FullDlg::CFactoryTestI8FullDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CFactoryTestI8FullDlg::IDD, pParent)
    , m_strMesLoginState(_T(""))
    , m_strMesResource(_T(""))
    , m_strConnectState(_T(""))
    , m_strScanSN(_T(""))
    , m_strCurSN(_T(""))
    , m_strCurMac(_T(""))
    , m_strTestInfo(_T(""))
    , m_strSnMacVer(_T(""))
    , m_pTnpContext(NULL)
    , m_pFanPlayer(NULL)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFactoryTestI8FullDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_TXT_MES_LOGIN, m_strMesLoginState);
    DDX_Text(pDX, IDC_TXT_MES_RESOURCE, m_strMesResource);
    DDX_Text(pDX, IDC_TXT_CONNECT_STATE, m_strConnectState);
    DDX_Text(pDX, IDC_TXT_TEST_INFO, m_strTestInfo);
    DDX_Text(pDX, IDC_TXT_SN_MAC_VER, m_strSnMacVer);
    DDX_Text(pDX, IDC_EDT_SCAN_SN, m_strScanSN);
    DDX_Text(pDX, IDC_EDT_CUR_SN, m_strCurSN);
    DDX_Text(pDX, IDC_EDT_CUR_MAC, m_strCurMac);
}

BEGIN_MESSAGE_MAP(CFactoryTestI8FullDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_DRAWITEM()
    ON_WM_CTLCOLOR()
    ON_WM_QUERYDRAGICON()
    ON_WM_DESTROY()
    ON_WM_CLOSE()
    ON_EN_CHANGE(IDC_EDT_SCAN_SN, &CFactoryTestI8FullDlg::OnEnChangeEdtScanSn)
    ON_MESSAGE(WM_TNP_UPDATE_UI   , &CFactoryTestI8FullDlg::OnTnpUpdateUI   )
    ON_MESSAGE(WM_TNP_DEVICE_FOUND, &CFactoryTestI8FullDlg::OnTnpDeviceFound)
    ON_MESSAGE(WM_TNP_DEVICE_LOST , &CFactoryTestI8FullDlg::OnTnpDeviceLost )
    ON_BN_CLICKED(IDC_BTN_LED_RESULT, &CFactoryTestI8FullDlg::OnBnClickedBtnLedResult)
    ON_BN_CLICKED(IDC_BTN_CAMERA_RESULT, &CFactoryTestI8FullDlg::OnBnClickedBtnCameraResult)
    ON_BN_CLICKED(IDC_BTN_IR_RESULT, &CFactoryTestI8FullDlg::OnBnClickedBtnIrResult)
    ON_BN_CLICKED(IDC_BTN_SPKMIC_RESULT, &CFactoryTestI8FullDlg::OnBnClickedBtnSpkmicResult)
    ON_BN_CLICKED(IDC_BTN_IR_TEST, &CFactoryTestI8FullDlg::OnBnClickedBtnIrTest)
    ON_BN_CLICKED(IDC_BTN_SPKMIC_TEST, &CFactoryTestI8FullDlg::OnBnClickedBtnSpkmicTest)
    ON_BN_CLICKED(IDC_BTN_KEY_TEST, &CFactoryTestI8FullDlg::OnBnClickedBtnKeyTest)
    ON_BN_CLICKED(IDC_BTN_UPLOAD_REPORT, &CFactoryTestI8FullDlg::OnBnClickedBtnUploadReport)
END_MESSAGE_MAP()


// CFactoryTestI8FullDlg ��Ϣ�������

BOOL CFactoryTestI8FullDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // ��������...���˵�����ӵ�ϵͳ�˵��С�

    // IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL) {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty()) {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
    // ִ�д˲���
    SetIcon(m_hIcon, TRUE );        // ���ô�ͼ��
    SetIcon(m_hIcon, FALSE);        // ����Сͼ��

    m_fntTestInfo.CreatePointFont(220, TEXT("����"), NULL);
    GetDlgItem(IDC_TXT_TEST_INFO)->SetFont(&m_fntTestInfo);

    // �ڴ���Ӷ���ĳ�ʼ������
    strcpy(m_strUserName  , "username"      );
    strcpy(m_strPassWord  , "password"      );
    strcpy(m_strResource  , "resource"      );
    strcpy(m_strTnpVer    , "version"       );
    strcpy(m_strLoginMode , "alert_and_exit");
    strcpy(m_strRouteCheck, "yes"           );
    strcpy(m_strLogFile   , "DEBUGER"       );
    strcpy(m_strDeviceIP  , ""              );
    int ret = load_config_from_file(m_strUserName, m_strPassWord, m_strResource, m_strTnpVer, m_strLoginMode, m_strRouteCheck, m_strLogFile);
    if (ret != 0) {
        AfxMessageBox(TEXT("�޷��򿪲��������ļ���"), MB_OK);
    }
    log_init(m_strLogFile);
    log_printf("username = %s\n", m_strUserName);
    log_printf("password = %s\n", m_strPassWord);
    log_printf("resource = %s\n", m_strResource);
    log_printf("version  = %s\n", m_strTnpVer  );
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
    m_strConnectState   = "�ȴ��豸����...";
    m_strTestInfo       = "����豸���������ģʽ��\r\n";
    m_bConnectState     = FALSE;
    m_bSnScaned         = FALSE;
    m_bIrOnOffState     = FALSE;
    m_nLedTestResult    = -1;
    m_nCameraTestResult = -1;
    m_nIRTestResult     = -1;
    m_nSpkMicTestResult = -1;
    m_nKeyTestResult    = -1;
    m_nLSensorTestResult= -1;
    m_nSnTestResult     = -1;
    m_nMacTestResult    = -1;
    m_nVersionTestResult= -1;
    UpdateData(FALSE);

    m_pTnpContext = tnp_init(GetSafeHwnd());
    return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CFactoryTestI8FullDlg::OnDestroy()
{
    CDialog::OnDestroy();

    tnp_disconnect(m_pTnpContext);
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

void CFactoryTestI8FullDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    } else {
        CDialog::OnSysCommand(nID, lParam);
    }
}

// ���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
//
HCURSOR CFactoryTestI8FullDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

// �����Ի��������С����ť������Ҫ����Ĵ���
// �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
// �⽫�ɿ���Զ���ɡ�

void CFactoryTestI8FullDlg::OnPaint()
{
    if (IsIconic()) {
        CPaintDC dc(this); // ���ڻ��Ƶ��豸������

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // ʹͼ���ڹ��������о���
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width () - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // ����ͼ��
        dc.DrawIcon(x, y, m_hIcon);
    } else {
        CDialog::OnPaint();
    }
}

HBRUSH CFactoryTestI8FullDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    switch (pWnd->GetDlgCtrlID()) {
    case IDC_STATIC_VIDEO:
        return (HBRUSH)GetStockObject(NULL_BRUSH);
    case IDC_TXT_MES_LOGIN:
        pDC->SetTextColor(m_bMesLoginOK ? RGB(0, 180, 0) : RGB(255, 0, 0));
        break;
    }
    return hbr;
}

int CFactoryTestI8FullDlg::GetBackColorByCtrlId(int id)
{
    int result = -1;
    switch (id) {
    case IDC_BTN_LED_RESULT:    result = m_nLedTestResult;    break;
    case IDC_BTN_IR_RESULT:     result = m_nIRTestResult;     break;
    case IDC_BTN_CAMERA_RESULT: result = m_nCameraTestResult; break;
    case IDC_BTN_SPKMIC_RESULT: result = m_nSpkMicTestResult; break;
    case IDC_BTN_KEY_RESULT:    result = m_nKeyTestResult;    break;
    case IDC_BTN_LSENSOR_RESULT:result = m_nLSensorTestResult;break;
    case IDC_BTN_SN_RESULT:     result = m_nSnTestResult;     break;
    case IDC_BTN_MAC_RESULT:    result = m_nMacTestResult;    break;
    case IDC_BTN_VERSION_RESULT:result = m_nVersionTestResult;break;
    }

    switch (result) {
    case 0:  return RGB(255, 0, 0);
    case 1:  return RGB(0, 255, 0);
    default: return RGB(236, 233, 216);
    }
}

void CFactoryTestI8FullDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    switch (nIDCtl) {
    case IDC_BTN_LED_RESULT:
    case IDC_BTN_IR_RESULT:
    case IDC_BTN_CAMERA_RESULT:
    case IDC_BTN_SPKMIC_RESULT:
    case IDC_BTN_KEY_RESULT:
    case IDC_BTN_LSENSOR_RESULT:
    case IDC_BTN_SN_RESULT:
    case IDC_BTN_MAC_RESULT:
    case IDC_BTN_VERSION_RESULT:
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
            dc.DrawText(buffer, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
            dc.Detach();
        }
        break;
    }
    CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CFactoryTestI8FullDlg::OnEnChangeEdtScanSn()
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

        if (m_bMesLoginOK) {
            CString strErrMsg;
            CString strMAC;
            CString strBT ;
            CString strCode1;
            CString strCode2;
            CString strCode3;
            BOOL ret = MesDLL::GetInstance().GetAddressRangeByMO(m_strCurSN, strMAC, strBT, strCode1, strCode2, strCode3, strErrMsg);
            if (ret) {
                m_strCurMac = strMAC;
            } else {
                m_strTestInfo  += "�޷��� MES ϵͳ��ȡ MAC\r\n";
                return;
            }
        } else {
            m_strCurMac = "4637E68F43E5";
        }
#endif

        if (m_bConnectState) {
            m_bSnScaned = FALSE;
            char strsn [32];
            char strmac[32];
            char strver[32];
            strcpy(strsn , m_strCurSN );
            strcpy(strmac, m_strCurMac);
            strcpy(strver, m_strTnpVer);
            tnp_test_sensor_snmac_version(m_pTnpContext, strsn, strmac, strver,
                &m_nLSensorTestResult, &m_nSnTestResult, &m_nMacTestResult, &m_nVersionTestResult);
            m_strSnMacVer.Format("�豸ʵ�� SN�� %s\r\n�豸ʵ�� MAC��%s\r\n�豸ʵ�� VER��%s", strsn, strmac, strver);
            GetDlgItem(IDC_BTN_LSENSOR_RESULT)->Invalidate();
            GetDlgItem(IDC_BTN_SN_RESULT     )->Invalidate();
            GetDlgItem(IDC_BTN_MAC_RESULT    )->Invalidate();
            GetDlgItem(IDC_BTN_VERSION_RESULT)->Invalidate();
        } else {
            m_bSnScaned = TRUE;
            m_strTestInfo = "����豸���������ģʽ��\r\n";
        }
        UpdateData(FALSE);
    }
}

LRESULT CFactoryTestI8FullDlg::OnTnpUpdateUI(WPARAM wParam, LPARAM lParam)
{
    UpdateData(FALSE);
    return 0;
}

LRESULT CFactoryTestI8FullDlg::OnTnpDeviceFound(WPARAM wParam, LPARAM lParam)
{
    if (strcmp(m_strDeviceIP, "") != 0) {
        log_printf("already have a device connected !\n");
        return 0;
    }

    struct in_addr addr;
    addr.S_un.S_addr = (u_long)lParam;

    strcpy(m_strDeviceIP, inet_ntoa(addr));
    m_strConnectState.Format(TEXT("�����豸 %s ��"), CString(m_strDeviceIP));

    int ret = tnp_connect(m_pTnpContext, addr);
    if (ret == 0) {
        m_strConnectState.Format(TEXT("�豸���ӳɹ�����%s��"), CString(m_strDeviceIP));
        m_bConnectState = TRUE;
        if (m_bSnScaned) {
            m_bSnScaned = FALSE;
            char strsn [32];
            char strmac[32];
            char strver[32];
            strcpy(strsn , m_strCurSN );
            strcpy(strmac, m_strCurMac);
            strcpy(strver, m_strTnpVer);
            tnp_test_sensor_snmac_version(m_pTnpContext, strsn, strmac, strver,
                &m_nLSensorTestResult, &m_nSnTestResult, &m_nMacTestResult, &m_nVersionTestResult);
            m_strSnMacVer.Format("�豸ʵ�� SN�� %s\r\n�豸ʵ�� MAC��%s\r\n�豸ʵ�� VER��%s", strsn, strmac, strver);
            GetDlgItem(IDC_BTN_LSENSOR_RESULT)->Invalidate();
            GetDlgItem(IDC_BTN_SN_RESULT     )->Invalidate();
            GetDlgItem(IDC_BTN_MAC_RESULT    )->Invalidate();
            GetDlgItem(IDC_BTN_VERSION_RESULT)->Invalidate();
        } else {
            m_strTestInfo   = "�豸�����ӣ���ɨ������...\r\n";
        }

        // set timeout to 6s
        tnp_set_timeout(m_pTnpContext, 6000);

        //++ reopen fanplayer to play rtsp stream
        if (m_pFanPlayer) {
            player_close(m_pFanPlayer);
        }
        char url[MAX_PATH];
        sprintf(url, "rtsp://%s:8554//main", m_strDeviceIP);
        PLAYER_INIT_PARAMS params = {0};
        params.init_timeout = 10000;
        m_pFanPlayer = player_open(url, GetDlgItem(IDC_STATIC_VIDEO)->GetSafeHwnd(), NULL);
        //-- reopen fanplayer to play rtsp stream
    } else {
        m_strConnectState.Format(TEXT("�豸����ʧ�ܣ���%s��"), CString(m_strDeviceIP));
        m_strTestInfo   = "�豸����ʧ�ܣ��������豸��\r\n";
        m_bConnectState = FALSE;
    }
    UpdateData(FALSE);

    return 0;
}

LRESULT CFactoryTestI8FullDlg::OnTnpDeviceLost(WPARAM wParam, LPARAM lParam)
{
    struct in_addr addr;
    addr.S_un.S_addr = (u_long)lParam;

    if (strcmp(m_strDeviceIP, inet_ntoa(addr)) != 0) {
        log_printf("this is not current connected device lost !\n");
        return 0;
    }

    player_close(m_pFanPlayer);
    m_pFanPlayer = NULL;

    m_strConnectState   = "�ȴ��豸����...";
    m_strTestInfo       = "����豸���������ģʽ��\r\n";
    m_strSnMacVer       = "";
    m_strScanSN         = "";
    m_strCurSN          = "";
    m_strCurMac         = "";
    m_strDeviceIP[0]    = '\0';
    m_bConnectState     = FALSE;
    m_bSnScaned         = FALSE;
    m_bIrOnOffState     = FALSE;
    m_nLedTestResult    = -1;
    m_nCameraTestResult = -1;
    m_nIRTestResult     = -1;
    m_nSpkMicTestResult = -1;
    m_nKeyTestResult    = -1;
    m_nLSensorTestResult= -1;
    m_nSnTestResult     = -1;
    m_nMacTestResult    = -1;
    m_nVersionTestResult= -1;
    tnp_disconnect (m_pTnpContext);
    tnp_set_timeout(m_pTnpContext, 3000);
    UpdateData(FALSE);

    GetDlgItem(IDC_BTN_LED_RESULT    )->Invalidate();
    GetDlgItem(IDC_BTN_CAMERA_RESULT )->Invalidate();
    GetDlgItem(IDC_BTN_IR_RESULT     )->Invalidate();
    GetDlgItem(IDC_BTN_SPKMIC_RESULT )->Invalidate();
    GetDlgItem(IDC_BTN_KEY_RESULT    )->Invalidate();
    GetDlgItem(IDC_BTN_LSENSOR_RESULT)->Invalidate();
    GetDlgItem(IDC_BTN_SN_RESULT     )->Invalidate();
    GetDlgItem(IDC_BTN_MAC_RESULT    )->Invalidate();
    GetDlgItem(IDC_BTN_VERSION_RESULT)->Invalidate();

    return 0;
}

void CFactoryTestI8FullDlg::OnCancel() {}
void CFactoryTestI8FullDlg::OnOK()     {}

void CFactoryTestI8FullDlg::OnClose()
{
    CDialog::OnClose();
    EndDialog(IDCANCEL);
}

BOOL CFactoryTestI8FullDlg::PreTranslateMessage(MSG *pMsg)
{
    if (pMsg->message == WM_KEYDOWN) {
        GetDlgItem(IDC_EDT_SCAN_SN)->SetFocus();
    }
    return CDialog::PreTranslateMessage(pMsg);
}

void CFactoryTestI8FullDlg::OnBnClickedBtnLedResult()
{
    if (m_nLedTestResult != 1) {
        m_nLedTestResult = 1;
        GetDlgItem(IDC_BTN_LED_RESULT)->SetWindowText("PASS");
    } else {
        m_nLedTestResult = 0;
        GetDlgItem(IDC_BTN_LED_RESULT)->SetWindowText("NG");
    }
}

void CFactoryTestI8FullDlg::OnBnClickedBtnCameraResult()
{
    if (m_nCameraTestResult != 1) {
        m_nCameraTestResult = 1;
        GetDlgItem(IDC_BTN_CAMERA_RESULT)->SetWindowText("PASS");
    } else {
        m_nCameraTestResult = 0;
        GetDlgItem(IDC_BTN_CAMERA_RESULT)->SetWindowText("NG");
    }
}

void CFactoryTestI8FullDlg::OnBnClickedBtnIrResult()
{
    if (m_nIRTestResult != 1) {
        m_nIRTestResult = 1;
        GetDlgItem(IDC_BTN_IR_RESULT)->SetWindowText("PASS");
    } else {
        m_nIRTestResult = 0;
        GetDlgItem(IDC_BTN_IR_RESULT)->SetWindowText("NG");
    }
}

void CFactoryTestI8FullDlg::OnBnClickedBtnSpkmicResult()
{
    if (m_nSpkMicTestResult != 1) {
        m_nSpkMicTestResult = 1;
        GetDlgItem(IDC_BTN_SPKMIC_RESULT)->SetWindowText("PASS");
    } else {
        m_nSpkMicTestResult = 0;
        GetDlgItem(IDC_BTN_SPKMIC_RESULT)->SetWindowText("NG");
    }
}

void CFactoryTestI8FullDlg::OnBnClickedBtnIrTest()
{
    if (tnp_test_ir_and_filter(m_pTnpContext, !m_bIrOnOffState) == 0) {
        m_bIrOnOffState = !m_bIrOnOffState;
        GetDlgItem(IDC_BTN_IR_TEST)->SetWindowText(m_bIrOnOffState ? "�����-�ѿ�" : "�����-�ѹ�");
    }
}

void CFactoryTestI8FullDlg::OnBnClickedBtnSpkmicTest()
{
    tnp_test_spkmic_manual(m_pTnpContext);
}

void CFactoryTestI8FullDlg::OnBnClickedBtnKeyTest()
{
    tnp_test_button(m_pTnpContext, &m_nKeyTestResult);
    GetDlgItem(IDC_BTN_KEY_RESULT)->Invalidate();
}

void CFactoryTestI8FullDlg::OnBnClickedBtnUploadReport()
{
#if ENABLE_MES_SYSTEM
        CString strTestResult;
        CString strErrCode;
        CString strErrMsg;
        if (m_nLedTestResult != 1) {
            strErrCode += "L001,";
        }
        if (m_nCameraTestResult != 1) {
            strErrCode += "L002,";
        }
        if (m_nIRTestResult != 1) {
            strErrCode += "L003,";
        }
        if (m_nSpkMicTestResult != 1) {
            strErrCode += "L004,";
        }
        if (m_nKeyTestResult != 1) {
            strErrCode += "L005,";
        }
        if (m_nLSensorTestResult != 1) {
            strErrCode += "L006,";
        }
        if (m_nSnTestResult != 1) {
            strErrCode += "L007,";
        }
        if (m_nMacTestResult != 1) {
            strErrCode += "L008,";
        }
        if (m_nVersionTestResult != 1) {
            strErrCode += "L009,";
        }
        if (  m_nLedTestResult == 1 && m_nCameraTestResult == 1 && m_nIRTestResult == 1 && m_nSpkMicTestResult == 1
           && m_nKeyTestResult == 1 && m_nLSensorTestResult == 1 && m_nSnTestResult == 1 && m_nMacTestResult == 1 && m_nVersionTestResult == 1) {
            strTestResult = "OK";
        } else {
            strTestResult = "NG";
        }
        if (m_bMesLoginOK) {
            MesDLL::GetInstance().SetMobileData(m_strCurSN, CString(m_strResource), CString(m_strUserName), strTestResult, strErrCode, strErrMsg);
        }
#endif
}
