//#include "stdafx.h"
#include <windows.h>
#include <commctrl.h>
#include <windowsx.h>
#include <aygshell.h>

#include "ResourceContext.h"
#include "JabberAccount.h"

#include "DlgConfig.h"
#include "DlgUtils.h"
#include "config.h"

#include "..\vs2005\ui\resourceppc.h"

#include "utf8.hpp"
#include "stringutils.h"

//////////////////////////////////////////////////////////////////////////////////

INT_PTR CALLBACK DlgProcConfig(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam, int npage);
INT_PTR CALLBACK DlgProcConfigP1(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    return DlgProcConfig(hDlg, message, wParam, lParam, 0);
}
INT_PTR CALLBACK DlgProcConfigP2(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    return DlgProcConfig(hDlg, message, wParam, lParam, 1);
}
INT_PTR CALLBACK DlgProcConfigP3(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    return DlgProcConfig(hDlg, message, wParam, lParam, 2);
}
INT_PTR CALLBACK DlgProcConfigP4(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    return DlgProcConfig(hDlg, message, wParam, lParam, 3);
}
INT_PTR CALLBACK DlgProcConfigP5(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    return DlgProcConfig(hDlg, message, wParam, lParam, 4);
}

void DialogConfigMP(HINSTANCE g_hInst, HWND parent) {

    PROPSHEETPAGE pages[5];
    pages[0].dwSize=sizeof(PROPSHEETPAGE);
    pages[1].dwSize=sizeof(PROPSHEETPAGE);
    pages[2].dwSize=sizeof(PROPSHEETPAGE);
    pages[3].dwSize=sizeof(PROPSHEETPAGE);
	pages[4].dwSize=sizeof(PROPSHEETPAGE);

    pages[0].hInstance=g_hInst;
    pages[1].hInstance=g_hInst;
    pages[2].hInstance=g_hInst;
    pages[3].hInstance=g_hInst;
    pages[4].hInstance=g_hInst;

    pages[0].dwFlags=PSP_DEFAULT;
    pages[1].dwFlags=PSP_DEFAULT;
    pages[2].dwFlags=PSP_DEFAULT;
	pages[3].dwFlags=PSP_DEFAULT;
    pages[4].dwFlags=PSP_DEFAULT;

    pages[0].pszTemplate=(LPCTSTR)IDD_OPTIONS1;
    pages[1].pszTemplate=(LPCTSTR)IDD_OPTIONS2;
    pages[2].pszTemplate=(LPCTSTR)IDD_OPTIONS3;
    pages[3].pszTemplate=(LPCTSTR)IDD_OPTIONS4;
    pages[4].pszTemplate=(LPCTSTR)IDD_OPTIONS5;

    pages[0].pfnDlgProc=DlgProcConfigP1;
    pages[1].pfnDlgProc=DlgProcConfigP2;
    pages[2].pfnDlgProc=DlgProcConfigP3;
    pages[3].pfnDlgProc=DlgProcConfigP4;
    pages[4].pfnDlgProc=DlgProcConfigP5;

    pages[0].lParam=0;
    pages[1].lParam=1;
    pages[2].lParam=2;
    pages[3].lParam=3;
    pages[4].lParam=4;

    PROPSHEETHEADER psh;
    psh.dwSize=sizeof(PROPSHEETHEADER);
    psh.dwFlags=PSH_MAXIMIZE | PSH_PROPSHEETPAGE | PSH_USECALLBACK;
    psh.hwndParent=parent;
    psh.hInstance=g_hInst;
    psh.pszCaption=L"Options";
    psh.nPages=5;
    psh.nStartPage=0;
    psh.ppsp=pages;
	psh.pfnCallback = PropSheetCallback;

    PropertySheet(&psh);

    Config::getInstance()->save();
}


INT_PTR CALLBACK DlgProcConfig(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam, int npage) {
    switch (message) {
    case WM_INITDIALOG:
        {
            // Create a Done button and size it.  
            SHINITDLGINFO shidi;
            shidi.dwMask = SHIDIM_FLAGS;
            shidi.dwFlags = SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN | SHIDIF_EMPTYMENU;
            shidi.hDlg = hDlg;
            //SHInitDialog(&shidi);

            Config::ref cfg=Config::getInstance();

            if (npage==0) {
                SetDlgCheckBox(hDlg, IDC_X_OFFLINES, cfg->showOfflines);
                SetDlgCheckBox(hDlg, IDC_X_GROUPS, cfg->showGroups);
                SetDlgCheckBox(hDlg, IDC_X_PRESENCESORT, cfg->sortByStatus);
				SetDlgCheckBox(hDlg, IDC_AUTO_LOG, cfg->autoLOG);
            }
            if (npage==1) {
                SetDlgCheckBox(hDlg, IDC_X_COMPOSING, cfg->composing);
                SetDlgCheckBox(hDlg, IDC_X_DELIVERY, cfg->delivered);
                SetDlgCheckBox(hDlg, IDC_X_HISTORY, cfg->history);
                SetDlgCheckBox(hDlg, IDC_X_SIP, cfg->raiseSIP);
            }
            if (npage==2) {
                SetDlgCheckBox(hDlg, IDC_X_VIBRA, cfg->vibra);
                SetDlgCheckBox(hDlg, IDC_X_SOUNDS, cfg->sounds);
            }
            if (npage==3) {
                SetDlgCheckBox(hDlg, IDC_X_AUTOCONNECT, cfg->connectOnStartup);
            }
			if (npage==4) {
					SetDlgCheckBox(hDlg, IDC_X_AUTO_MUC, cfg->autoMUC);
					SetDlgCheckBox(hDlg, IDC_AUTO_EDIT_FOCUS, cfg->autoFEdit);
					SetDlgCheckBox(hDlg, IDC_SAVE_CLOSED_MUC, cfg->saveClMUC);
					SetDlgCheckBox(hDlg, IDC_MUC_BOOK_SORT, cfg->autoMUCBS);
			}
            //finally
        }
        return (INT_PTR)TRUE;

    case WM_NOTIFY:
        {
            NMHDR* pnmh = (NMHDR*)lParam; 
            if (pnmh->code == PSN_APPLY) {

                Config::ref cfg=Config::getInstance();

                if (npage==0) {
                    GetDlgCheckBox(hDlg, IDC_X_OFFLINES, cfg->showOfflines);
                    GetDlgCheckBox(hDlg, IDC_X_GROUPS, cfg->showGroups);
                    GetDlgCheckBox(hDlg, IDC_X_PRESENCESORT, cfg->sortByStatus);
					GetDlgCheckBox(hDlg, IDC_AUTO_LOG, cfg->autoLOG);
                }
                if (npage==1) {
                    GetDlgCheckBox(hDlg, IDC_X_COMPOSING, cfg->composing);
                    GetDlgCheckBox(hDlg, IDC_X_DELIVERY, cfg->delivered);
                    GetDlgCheckBox(hDlg, IDC_X_HISTORY, cfg->history);
                    GetDlgCheckBox(hDlg, IDC_X_SIP, cfg->raiseSIP);
                }
                if (npage==2) {
                    GetDlgCheckBox(hDlg, IDC_X_VIBRA, cfg->vibra);
                    GetDlgCheckBox(hDlg, IDC_X_SOUNDS, cfg->sounds);
                }
                if (npage==3) {
                    GetDlgCheckBox(hDlg, IDC_X_AUTOCONNECT, cfg->connectOnStartup);
				}
				if (npage==4) {
					GetDlgCheckBox(hDlg, IDC_X_AUTO_MUC, cfg->autoMUC);
					GetDlgCheckBox(hDlg, IDC_AUTO_EDIT_FOCUS, cfg->autoFEdit);
					GetDlgCheckBox(hDlg, IDC_SAVE_CLOSED_MUC, cfg->saveClMUC);
					GetDlgCheckBox(hDlg, IDC_MUC_BOOK_SORT, cfg->autoMUCBS);
				}
                return TRUE;
            }
            return FALSE;
        }

    case WM_COMMAND:

        if (LOWORD(wParam) == IDCANCEL) {
            return TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
