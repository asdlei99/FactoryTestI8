#pragma once

#include <afxmt.h>
#include "stdafx.h"

#define FRAME_METHOD_EXPORTS
#ifdef FRAME_METHOD_EXPORTS
# define DLLExport _declspec(dllexport)
#else
# define DLLExport _declspec(dllimport)
#endif

class DLLExport MesDLL
{
private:
    MesDLL();
    MesDLL(const MesDLL &);
    MesDLL & operator = (const MesDLL &);

public:
    static MesDLL& GetInstance();
    ~MesDLL();

    // ���� SN �õ�������Ϣ
    BOOL GetRcardMOInfo(CString iSN, CString &oMoCode, CString &oErrMessage);

    // ����¼�����Ӧ����Ա�Ƿ���ȷ����Դ�Ƿ���ȷ����Ա�Ƿ�ӵ����ԴȨ��
    BOOL CheckUserAndResourcePassed(CString iUserCode, CString iResCode, CString iPassWord, CString iJigCode, CString &oErrMessage);

    // ��� SN �Ƿ���;������
    BOOL CheckRoutePassed(CString iSN, CString iResCode , CString &oErrMessage);

    // ���� MAC �� BT ��ַ
    BOOL GetAddressRangeByMO(CString iSN, CString &oWIFI, CString &oBT, CString &oCode1, CString &oCode2, CString &oCode3, CString &oErrMessage);

    // ���Թ��߻ش����ݣ�MES ��¼ Mac �� BT ��ַʹ����Ϣ
    BOOL SetAddressInfo(CString iSN, CString iWIFI, CString iBT, CString iCode1, CString iCode2, CString iCode3, CString &oErrMessage);

    // ��¼���Բ�����Ϣ
    BOOL SetTestDetail(CString iSN, CString iClass, CString iSubClass1, CString iSubClass2, CString iSubClass3, CString iMaxValue, CString iMinValue, CString iActualValue, CString iValue1, CString iValue2, CString iValue3, CString iTestResult, CString iResCode, CString &oErrMessage);

    // ���� IMEI �� NetCode����ȡ IMEI��NETCODE��PSN ��Ϣ��Check)
    BOOL GetMEIOrNetCodeRange(CString iSN, CString iIMEI, CString iNetCode, CString &oIMEI1, CString &oIMEI2, CString &oMEID, CString &oNetCode, CString &oPSN, CString &oID1, CString &oID2, CString &oID3, CString &oID4, CString &oID5, CString &oErrMessage);

    // �豸�ش����� IMEI��NETCODE��PSN ��Ϣ��MES ϵͳ��¼ʹ����Ϣ
    BOOL SetIMEIInfo(CString iSN, CString iIMEI, CString &oErrMessage);

    // ����ͨ����ȡ SN����ȡ MES ���м�¼��Ϣ��WIFI��BT��IMEI1��IMEI2��NETCODE��PSN��
    BOOL GetMobileAllInfo(CString iSN, CString &oWIFI, CString &oBT, CString &oCode1, CString &oCode2, CString &oCode3, CString &oIMEI1, CString &oIMEI2, CString &oMEID, CString &oNetCode, CString &oPSN, CString &oID1, CString &oID2, CString &oID3, CString &oID4, CString &oID5, CString &oErrMessage);

    // ��¼��ӡ��¼
    BOOL SetPrintRecord(CString iSN, CString iOperator, CString &oErrMessage);

    // MES ���������������ϣ���¼������Ϣ��
    BOOL SetMobileData(CString iSN, CString iResCode, CString iOperator, CString iResult, CString iErrCode, CString &oErrMessage);

    // MES ��Ʒ����Ʒ�ɼ����������������ܡ�
    BOOL SetMobileDataWithMO(CString iSN, CString iResCode, CString iOperator, CString iResult, CString iErrCode, CString iMO, CString &oErrMessage);

    // ���� IMEI �Լ� MACBT
    BOOL ImportImeiMacBt(CString strImputXml, CString iType, CString &oErrMessage);

    // �ʱ��ӡ DLL ��װ����
    BOOL CBLabelPrint(CString iRcard, CString iPrinter, CString iOperator, CString iResCode, CString &oErrMessage);

    BOOL ATELogOut(CString iResCode, CString &oErrMessage);
};
