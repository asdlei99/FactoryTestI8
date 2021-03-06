#pragma once

#include <windows.h>

enum {
    WM_TNP_UPDATE_UI = WM_APP + 100,
    WM_TNP_DEVICE_FOUND,
    WM_TNP_DEVICE_LOST,
    WM_TNP_TYPE_CHANGED,
};

void* tnp_init(HWND hwnd);
void  tnp_free(void *ctxt);
int   tnp_connect    (void *ctxt, struct in_addr addr);
void  tnp_disconnect (void *ctxt);
int   tnp_connect_by_sn   (void *ctxt, char *sn, struct in_addr *paddr);
int   tnp_disconnect_by_sn(void *ctxt, char *sn);
void  tnp_set_timeout(void *ctxt, int timeout);
int   tnp_burn_snmac (void *ctxt, char *sn, char *mac, int *snrslt, int *macrslt);
int   tnp_test_spkmic(void *ctxt);
int   tnp_test_button(void *ctxt, int *btn);
int   tnp_test_ir_and_filter (void *ctxt, int onoff);
int   tnp_test_spkonly_manual(void *ctxt, int onoff);
int   tnp_test_sensor_snmac_version(void *ctxt, char *sn, char *mac, char *version, int *rsltkey, int *rsltsensor, int *rsltsn, int *rsltmac, int *rsltver);
int   tnp_test_smt   (void *ctxt, char *version, int *rsltsensor, int *rslspkmic, int *rsltver);
int   tnp_test_done  (void *ctxt);
void  tnp_test_cancel(void *ctxt, int cancel);


