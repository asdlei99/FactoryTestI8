// FactoryTestI8Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "tlhelp32.h"
#include "FactoryTestI8.h"
#include "FactoryTestI8Dlg.h"
#include "TestNetProtocol.h"
#include "BenQGuruDll.h"
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

static int load_config_from_file(char *user, char *passwd, char *res, char *ver, char *login, char *route, char *throughput, char *log)
{
    char  file[MAX_PATH];
    FILE *fp = NULL;
    char *buf= NULL;
    int   len= 0;

    // open params file
    get_app_dir(file, MAX_PATH);
    strcat(file, "\\factorytesti8sn.ini");
    fp = fopen(file, "rb");

    if (fp) {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        buf = (char*)malloc(len);
        if (buf) {
            fseek(fp, 0, SEEK_SET);
            fread(buf, len, 1, fp);
            parse_params(buf, "username"  , user      );
            parse_params(buf, "password"  , passwd    );
            parse_params(buf, "resource"  , res       );
            parse_params(buf, "version"   , ver       );
            parse_params(buf, "loginmode" , login     );
            parse_params(buf, "routecheck", route     );
            parse_params(buf, "throughput", throughput);
            parse_params(buf, "logfile"   , log       );
            free(buf);
        }
        fclose(fp);
        return 0;
    }

    return -1;
}

static float parse_iperf_log(CString path)
{
    CStdioFile file;
    CString    str;
    if (file.Open(path, CFile::modeRead)) {
        while (file.ReadString(str)) {
            if (str.Find(TEXT("sender"), 0) > 0) {
                int s = str.Find(TEXT("MBytes"), 0) + 6;
                int e = str.Find(TEXT("Mbits" ), 0);
                str = str.Mid(s, e - s).Trim();
                return (float)_tstof(str);
            }
        }
    }
    return 0;
}

static void kill_process_by_name(char *name)
{
    PROCESSENTRY32 pe32  = {0};
    HANDLE         hsnap = NULL;

    pe32.dwSize = sizeof(pe32);
    hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hsnap == INVALID_HANDLE_VALUE) return;

    BOOL ret = Process32First(hsnap, &pe32);
    while (ret) {
        if (stricmp(pe32.szExeFile, name) == 0) {
            HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
            TerminateProcess(h, 0);
            CloseHandle(h);
        }
        ret = Process32Next(hsnap, &pe32);
    }
    CloseHandle(hsnap);
}

static DWORD WINAPI DeviceTestThreadProc(LPVOID pParam)
{
    CFactoryTestI8Dlg *dlg = (CFactoryTestI8Dlg*)pParam;
    dlg->DoDeviceTest();
    return 0;
}

void CFactoryTestI8Dlg::DoDeviceTest()
{
    m_bResultDone = FALSE;

    // set timeout to 6s
    tnp_set_timeout(m_pTnpContext, 6000);

    if (!m_bTestCancel) {
        m_strTestInfo   = "����д�� ...\r\n";
        m_strTestResult = "����д��";

        tnp_burn_snmac(m_pTnpContext, m_strCurSN.GetBuffer(), m_strCurMac.GetBuffer(), &m_bResultBurnSN, &m_bResultBurnMac);
        m_strCurSN .ReleaseBuffer();
        m_strCurMac.ReleaseBuffer();
        PostMessage(WM_TNP_UPDATE_UI);
    }

    if (!m_bTestCancel) {
        m_strTestInfo  += "���ڲ��������� ...\r\n";
        m_strTestResult = "���ڲ���";
        PostMessage(WM_TNP_UPDATE_UI);

        CString cmd;
        cmd.Format(TEXT("cmd /c iperf3 -c %s > iperf.log"), CString(m_strDeviceIP));
        STARTUPINFO         si = {0};
        PROCESS_INFORMATION pi = {0};
        si.dwFlags     = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;
        CreateProcess(NULL, cmd.GetBuffer(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
        cmd.ReleaseBuffer();
        CloseHandle(pi.hThread);
        while (1) {
            DWORD ret1 = WaitForSingleObject(pi.hProcess, 100);
            if (ret1 == WAIT_OBJECT_0) {
                break;
            } else if (m_bTestCancel) {
                kill_process_by_name("iperf3.exe");
            }
        }
        CloseHandle(pi.hProcess);

        float wifi = parse_iperf_log(TEXT("iperf.log"));
        int   throughput = atoi(m_strThroughPut);
        if (!throughput) throughput = 25;
        m_strWiFiThroughPut.Format(TEXT("%.1f MBytes/sec"), wifi);
        PostMessage(WM_TNP_UPDATE_UI);
        if (wifi > throughput) {
            m_bResultTestNet = TRUE;
        } else {
            m_bResultTestNet = FALSE;
        }
        PostMessage(WM_TNP_UPDATE_UI);
    }

    if (!m_bTestCancel) {
        m_strTestInfo  += "���ڲ���������ͷ ...\r\n\r\n";
        m_strTestResult = "���ڲ���";
        PostMessage(WM_TNP_UPDATE_UI);
        if (tnp_test_spkmic(m_pTnpContext) == 0) {
            m_bResultTestSpkMic = TRUE;
        } else {
            m_bResultTestSpkMic = FALSE;
        }
        PostMessage(WM_TNP_UPDATE_UI);
    }

    if (!m_bTestCancel) {
        if (m_bResultBurnSN && m_bResultBurnMac && m_bResultTestSpkMic && m_bResultTestNet) {
            m_strTestResult = "OK";
        } else {
            m_strTestResult = "NG";
        }

        m_strTestInfo += m_bResultBurnSN    ? "д��     -  OK\r\n" : "д��     -  NG\r\n";
        m_strTestInfo += m_bResultBurnMac   ? "MAC      -  OK\r\n" : "MAC      -  NG\r\n";
        m_strTestInfo += m_bResultTestSpkMic? "������ͷ -  OK\r\n" : "������ͷ -  NG\r\n";
        m_strTestInfo += m_bResultTestNet   ? "������   -  OK\r\n" : "������   -  NG\r\n";

        m_bResultDone = TRUE;
        PostMessage(WM_TNP_UPDATE_UI);

#if ENABLE_MES_SYSTEM
        CString strErrCode;
        CString strErrMsg;
        if (!m_bResultBurnSN) {
            strErrCode += "L001,";
        }
        if (!m_bResultBurnMac) {
            strErrCode += "L002,";
        }
        if (!m_bResultTestSpkMic) {
            strErrCode += "L004,L005,";
        }
        if (!m_bResultTestNet) {
            strErrCode += "L007,";
        }
        if (m_bMesLoginOK) {
            BOOL ret = MesDLL::GetInstance().SetMobileData(m_strCurSN, CString(m_strResource), CString(m_strUserName), m_strTestResult, strErrCode, strErrMsg);
            if (!ret) {
                if (strErrMsg.Find("CS_RepeatCollect_OnOneOP") != -1) {
                    m_strTestResult = "�ظ��ɼ�";
                } else {
                    m_strTestResult = "�ϴ�ʧ��";
                }
                log_printf("MES SetMobileData failed !\nstrErrMsg = %s\n", strErrMsg);
                PostMessage(WM_TNP_UPDATE_UI);
            }
        }
#endif
    }

    // set timeout to 3s
    tnp_set_timeout(m_pTnpContext, 3000);

    CloseHandle(m_hTestThread);
    m_hTestThread = NULL;
}

void CFactoryTestI8Dlg::StartDeviceTest()
{
    if (m_hTestThread) {
        log_printf("device test is running, please wait test done !\n");
        return;
    }

    m_bTestCancel = FALSE;
    tnp_test_cancel(m_pTnpContext, FALSE);
    m_hTestThread = CreateThread(NULL, 0, DeviceTestThreadProc, this, 0, NULL);
}

void CFactoryTestI8Dlg::StopDeviceTest()
{
    m_bTestCancel = TRUE;
    tnp_test_cancel(m_pTnpContext, TRUE);
    if (m_hTestThread) {
        WaitForSingleObject(m_hTestThread, -1);
    }
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


// CFactoryTestI8Dlg �Ի���

CFactoryTestI8Dlg::CFactoryTestI8Dlg(CWnd* pParent /*=NULL*/)
    : CDialog(CFactoryTestI8Dlg::IDD, pParent)
    , m_strMesLoginState(_T(""))
    , m_strMesResource(_T(""))
    , m_strConnectState(_T(""))
    , m_strScanSN(_T(""))
    , m_strCurSN(_T(""))
    , m_strCurMac(_T(""))
    , m_strWiFiThroughPut(_T(""))
    , m_strTestResult(_T(""))
    , m_strTestInfo(_T(""))
    , m_hTestThread(NULL)
    , m_pTnpContext(NULL)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFactoryTestI8Dlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_TXT_MES_LOGIN, m_strMesLoginState);
    DDX_Text(pDX, IDC_TXT_MES_RESOURCE, m_strMesResource);
    DDX_Text(pDX, IDC_TXT_CONNECT_STATE, m_strConnectState);
    DDX_Text(pDX, IDC_TXT_TEST_INFO, m_strTestInfo);
    DDX_Text(pDX, IDC_TXT_TEST_RESULT, m_strTestResult);
    DDX_Text(pDX, IDC_EDT_SCAN_SN, m_strScanSN);
    DDX_Text(pDX, IDC_EDT_CUR_SN, m_strCurSN);
    DDX_Text(pDX, IDC_EDT_CUR_MAC, m_strCurMac);
    DDX_Text(pDX, IDC_EDT_IPREF_RESULT, m_strWiFiThroughPut);
}

BEGIN_MESSAGE_MAP(CFactoryTestI8Dlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_CTLCOLOR()
    ON_WM_QUERYDRAGICON()
    ON_WM_DESTROY()
    ON_WM_CLOSE()
    ON_EN_CHANGE(IDC_EDT_SCAN_SN, &CFactoryTestI8Dlg::OnEnChangeEdtScanSn)
    ON_MESSAGE(WM_TNP_UPDATE_UI   , &CFactoryTestI8Dlg::OnTnpUpdateUI   )
    ON_MESSAGE(WM_TNP_DEVICE_FOUND, &CFactoryTestI8Dlg::OnTnpDeviceFound)
    ON_MESSAGE(WM_TNP_DEVICE_LOST , &CFactoryTestI8Dlg::OnTnpDeviceLost )
END_MESSAGE_MAP()


// CFactoryTestI8Dlg ��Ϣ�������

BOOL CFactoryTestI8Dlg::OnInitDialog()
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

    m_fntResult.CreatePointFont(350, TEXT("����"), NULL);
    GetDlgItem(IDC_TXT_TEST_RESULT)->SetFont(&m_fntResult);
    m_strTestResult = "�������豸";

    // �ڴ���Ӷ���ĳ�ʼ������
    strcpy(m_strUserName  , "username"      );
    strcpy(m_strPassWord  , "password"      );
    strcpy(m_strResource  , "resource"      );
    strcpy(m_strTnpVer    , "version"       );
    strcpy(m_strLoginMode , "alert_and_exit");
    strcpy(m_strRouteCheck, "yes"           );
    strcpy(m_strLogFile   , "DEBUGER"       );
    strcpy(m_strDeviceIP  , ""              );
    int ret = load_config_from_file(m_strUserName, m_strPassWord, m_strResource, m_strTnpVer, m_strLoginMode, m_strRouteCheck, m_strThroughPut, m_strLogFile);
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

    m_strMesResource   = CString(m_strResource);
    m_strConnectState  = "�ȴ��豸����...";
    m_strTestInfo      = "����豸���������ģʽ��\r\n";
    m_bConnectState    = FALSE;
    m_bSnScaned        = FALSE;
    m_bResultBurnSN    = FALSE;
    m_bResultBurnMac   = FALSE;
    m_bResultTestSpkMic= FALSE;
    m_bResultTestNet   = FALSE;
    m_bResultDone      = FALSE;
    UpdateData(FALSE);

    m_pTnpContext = tnp_init(GetSafeHwnd());
    return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CFactoryTestI8Dlg::OnDestroy()
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

void CFactoryTestI8Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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
HCURSOR CFactoryTestI8Dlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

// �����Ի��������С����ť������Ҫ����Ĵ���
// �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
// �⽫�ɿ���Զ���ɡ�

void CFactoryTestI8Dlg::OnPaint()
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

HBRUSH CFactoryTestI8Dlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    switch (pWnd->GetDlgCtrlID()) {
    case IDC_TXT_TEST_RESULT:
        if (!m_bConnectState || !m_bResultDone) {
            pDC->SetTextColor(RGB(0, 120, 255));
        } else if (m_bResultBurnSN && m_bResultBurnMac && m_bResultTestSpkMic && m_bResultTestNet) {
            pDC->SetTextColor(RGB(0, 200, 0));
        } else {
            pDC->SetTextColor(RGB(255, 0, 0));
        }
        break;
    case IDC_TXT_MES_LOGIN:
        pDC->SetTextColor(m_bMesLoginOK ? RGB(0, 180, 0) : RGB(255, 0, 0));
        break;
    }
    return hbr;
}

void CFactoryTestI8Dlg::OnEnChangeEdtScanSn()
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
            StartDeviceTest();
        } else {
            m_bSnScaned = TRUE;
            m_strTestResult = "�������豸";
        }
        UpdateData(FALSE);
    }
}

LRESULT CFactoryTestI8Dlg::OnTnpUpdateUI(WPARAM wParam, LPARAM lParam)
{
    UpdateData(FALSE);
    return 0;
}

LRESULT CFactoryTestI8Dlg::OnTnpDeviceFound(WPARAM wParam, LPARAM lParam)
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
            StartDeviceTest();
        } else {
            m_strTestInfo   = "�豸�����ӣ���ɨ�����롣\r\n";
            m_strTestResult = "��ɨ������";
        }
    } else {
        m_strConnectState.Format(TEXT("�豸����ʧ�ܣ���%s��"), CString(m_strDeviceIP));
        m_strTestInfo   = "�豸����ʧ�ܣ��������豸��\r\n";
        m_bConnectState = FALSE;
    }
    UpdateData(FALSE);
    return 0;
}

LRESULT CFactoryTestI8Dlg::OnTnpDeviceLost(WPARAM wParam, LPARAM lParam)
{
    struct in_addr addr;
    addr.S_un.S_addr = (u_long)lParam;
    if (strcmp(m_strDeviceIP, inet_ntoa(addr)) != 0) {
        log_printf("this is not current connected device lost !\n");
        return 0;
    }

    StopDeviceTest(); // stop test
    m_strConnectState   = "�ȴ��豸����...";
    m_strTestResult     = "�������豸";
    m_strTestInfo       = "����豸���������ģʽ��\r\n";
    m_strScanSN         = "";
    m_strCurSN          = "";
    m_strCurMac         = "";
    m_strWiFiThroughPut = "";
    m_strDeviceIP[0]    = '\0';
    m_bConnectState     = FALSE;
    m_bSnScaned         = FALSE;
    m_bResultDone       = FALSE;
    tnp_disconnect(m_pTnpContext);
    UpdateData(FALSE);
    return 0;
}

void CFactoryTestI8Dlg::OnCancel() {}
void CFactoryTestI8Dlg::OnOK()     {}

void CFactoryTestI8Dlg::OnClose()
{
    CDialog::OnClose();
    EndDialog(IDCANCEL);
}

BOOL CFactoryTestI8Dlg::PreTranslateMessage(MSG *pMsg)
{
    if (pMsg->message == WM_KEYDOWN) {
        GetDlgItem(IDC_EDT_SCAN_SN)->SetFocus();
    }
    return CDialog::PreTranslateMessage(pMsg);
}

