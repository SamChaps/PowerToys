#include "pch.h"
#include "FileUtils.h"

#include <common/logger/logger.h>

#include <shlobj.h>

namespace FileUtils
{
    HRESULT GetSelectedFile(std::vector<String>& selectedFiles)
    {
        HRESULT hr = S_FALSE;

        // The window handle the user interacted with
        HWND hwndFind = GetForegroundWindow();

        // To find the file the user is focused on in FE:
        //  1. Find all active explorer windows
        //  2. See if we can find the user interacted window within it
        //  3. Find the focused file in that explorer window
        IShellWindows* psw;
        if (SUCCEEDED(CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_ALL, IID_IShellWindows, (void**)&psw)))
        {
            VARIANT v;
            V_VT(&v) = VT_I4;
            IDispatch* pdisp;

            BOOL isExplorerWindow = FALSE;
            for (V_I4(&v) = 0; !isExplorerWindow && psw->Item(v, &pdisp) == S_OK; V_I4(&v)++)
            {
                IWebBrowserApp* pwba;
                if (SUCCEEDED(pdisp->QueryInterface(IID_IWebBrowserApp, (void**)&pwba)))
                {
                    HWND hwndWBA;
                    if (SUCCEEDED(pwba->get_HWND((LONG_PTR*)&hwndWBA)) && hwndWBA == hwndFind)
                    {
                        isExplorerWindow = TRUE;
                        IServiceProvider* psp;

                        if (SUCCEEDED(pwba->QueryInterface(IID_IServiceProvider, (void**)&psp)))
                        {
                            IShellBrowser* psb;
                            if (SUCCEEDED(psp->QueryService(SID_STopLevelBrowser, IID_IShellBrowser, (void**)&psb)))
                            {
                                IShellView* psv;
                                if (SUCCEEDED(psb->QueryActiveShellView(&psv)))
                                {
                                    IFolderView* pfv;
                                    if (SUCCEEDED(psv->QueryInterface(IID_IFolderView, (void**)&pfv)))
                                    {
                                        IFolderView2* pfv2;
                                        if (SUCCEEDED(psv->QueryInterface(IID_IFolderView2, (void**)&pfv2)))
                                        {
                                            IPersistFolder2* ppf2;
                                            if (SUCCEEDED(pfv->GetFolder(IID_IPersistFolder2, (void**)&ppf2)))
                                            {
                                                LPITEMIDLIST pidlFolder;
                                                if (SUCCEEDED(ppf2->GetCurFolder(&pidlFolder)))
                                                {
                                                    TCHAR szPath[MAX_PATH];
                                                    if (!SHGetPathFromIDList(pidlFolder, szPath))
                                                    {
                                                        Logger::info(L"Could not resolve path to a valid directory");
                                                    }
                                                    else
                                                    {
                                                        // loop through all files and get selection state
                                                        IShellItemArray* psiItems;
                                                        pfv2->GetSelection(false, &psiItems);

                                                        DWORD selectedCount = 0;
                                                        psiItems->GetCount(&selectedCount);

                                                        for (DWORD i = 0; i < selectedCount; i++)
                                                        {
                                                            IShellItem* shellItem;
                                                            psiItems->GetItemAt(i, &shellItem);

                                                            LPWSTR itemName;
                                                            if (SUCCEEDED(shellItem->GetDisplayName(SIGDN_FILESYSPATH, &itemName)))
                                                            {
                                                                selectedFiles.push_back(itemName);
                                                            }
                                                        }

                                                        if (selectedFiles.size() > 0)
                                                        {
                                                            hr = S_OK;
                                                        }
                                                    }

                                                    CoTaskMemFree(pidlFolder);
                                                }
                                                ppf2->Release();
                                            }
                                            pfv2->Release();
                                        }
                                        pfv->Release();
                                    }
                                    psv->Release();
                                }
                                psb->Release();
                            }
                            psp->Release();
                        }
                    }
                    pwba->Release();
                }
                pdisp->Release();
            }
            psw->Release();
        }

        return hr;
    }
}