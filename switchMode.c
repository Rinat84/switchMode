#include <windows.h>
#include <Commctrl.h>
#include <wininet.h>

#include <minicrt.h>

#include "resource\resource.h"

#define  hBmode     GetDlgItem(hWnd, IDB_MODE)
#define  hBdebug    GetDlgItem(hWnd, IDB_DEBUG)
#define  hBproject  GetDlgItem(hWnd, IDB_PROJECT)
#define  hSip       GetDlgItem(hWnd, IDS_IP)
#define  hEip       GetDlgItem(hWnd, IDE_IP)
#define  hSvid      GetDlgItem(hWnd, IDS_VID)
#define  hSpid      GetDlgItem(hWnd, IDS_PID)
#define  hSrev      GetDlgItem(hWnd, IDS_REV)
#define  hEvid      GetDlgItem(hWnd, IDE_VID)
#define  hEpid      GetDlgItem(hWnd, IDE_PID)
#define  hErev      GetDlgItem(hWnd, IDE_REV)

VOID MobileBrServ(HWND hWnd, WPARAM wParam)
{
  HKEY hKey;
  CHAR vid[5], pid[5], rev[5], cKey[55];
  UCHAR osvc[] = {0x01, 0xEE};
  UCHAR SkipContainerIdQuery[] = {0x01, 0x00, 0x00, 0x00};

  GetWindowText(hEvid, vid, 5);
  GetWindowText(hEpid, pid, 5);
  GetWindowText(hErev, rev, 5);
  sprintf_s(cKey, 55 ,"SYSTEM\\CurrentControlSet\\Control\\usbflags\\%s%s%s", vid, pid, rev);
  if(!RegCreateKeyEx(HKEY_LOCAL_MACHINE, cKey, 0, NULL, REG_OPTION_NON_VOLATILE,
                    KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
  {
    MessageBox(hWnd, "Ошибка", "Ошибка", MB_OK | MB_ICONERROR);
    return;
  }
  if(LOWORD(wParam) == IDB_DEBUG)
  {
    RegSetValueEx(hKey, "osvc", 0, REG_BINARY, (LPBYTE)osvc, sizeof(osvc));
    RegSetValueEx(hKey, "SkipContainerIdQuery", 0, REG_BINARY, (LPBYTE)SkipContainerIdQuery, sizeof(SkipContainerIdQuery));
    MessageBox(hWnd, "Выполнено", "Выполнено", MB_OK | MB_ICONINFORMATION);
  }
  else
  {
    RegDeleteKey(HKEY_LOCAL_MACHINE, cKey);
    MessageBox(hWnd, "Выполнено", "Выполнено", MB_OK | MB_ICONINFORMATION);
  }
  RegCloseKey(hKey);
}

VOID SendPost(HWND hWnd, WPARAM wParam)
{
  HINTERNET hInternet, hConnect, hRequest;
  CHAR ip[16];
  CHAR post[] = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
"<api version=\"1.0\">\n"
"  <header>\n"
"    <function>switchMode</function>\n"
"  </header>\n"
"  <body>\n"
"    <request>\n"
"      <switchType>0</switchType>\n"
"    </request>\n"
"  </body>\n"
"</api>";

  if(LOWORD(wParam) == IDB_DEBUG) post[160] = '1';
  GetWindowText(hEip, ip, 16);

  // инициализируем WinInet
  hInternet = InternetOpen("switchMode", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
  if(hInternet  && GetLastError() == 0) {
    // открываем HTTP сессию
    hConnect = InternetConnect(hInternet, ip, INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if(hConnect  && GetLastError() == 0) {
      // открываем запрос
      hRequest = HttpOpenRequest(hConnect, "POST", "/CGI", NULL, NULL, 0, INTERNET_FLAG_KEEP_CONNECTION, 0);
      if(hRequest  && GetLastError() == 0) {
        // посылаем запрос
        HttpSendRequest(hRequest, NULL, 0, post, 250);
        if(GetLastError() == 0) MessageBox(hWnd, "Выполнено", "Выполнено", MB_OK | MB_ICONINFORMATION);
        else MessageBox(hWnd, "Ошибка", "Ошибка", MB_OK | MB_ICONERROR);
      }
      // закрываем запрос
      InternetCloseHandle(hRequest);
    }
    // закрываем сессию
    InternetCloseHandle(hConnect);
  }
  // закрываем WinInet
  InternetCloseHandle(hInternet);

}


//  Основная функция окна.
BOOL CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch(message)
  {
    case WM_INITDIALOG:
    {
      HICON hIcon;

      hIcon = LoadIcon((HINSTANCE)lParam, MAKEINTRESOURCE(IDI_ICON));
      SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

      SendDlgItemMessage(hWnd, IDE_IP, IPM_SETADDRESS, 0, MAKEIPADDRESS(192, 168, 8, 1));
      SendDlgItemMessage(hWnd, IDE_VID, EM_SETLIMITTEXT, (WPARAM)4, TRUE);
      SendDlgItemMessage(hWnd, IDE_PID, EM_SETLIMITTEXT, (WPARAM)4, TRUE);
      SendDlgItemMessage(hWnd, IDE_REV, EM_SETLIMITTEXT, (WPARAM)4, TRUE);

      SendDlgItemMessage(hWnd, IDE_VID, WM_SETTEXT, FALSE, (LPARAM)"12D1"); 
      SendDlgItemMessage(hWnd, IDE_PID, WM_SETTEXT, FALSE, (LPARAM)"1F01"); 
      SendDlgItemMessage(hWnd, IDE_REV, WM_SETTEXT, FALSE, (LPARAM)"0102"); 
      break;
    }

    case WM_COMMAND:
    {
      BOOL mode;
      if(IsWindowVisible(hEip)) mode = FALSE;
      else mode = TRUE;
      switch(LOWORD(wParam))
      {
        case IDB_MODE:
          if(mode == FALSE)
          {
            ShowWindow(hSip, SW_HIDE);
            ShowWindow(hEip, SW_HIDE);
            ShowWindow(hSvid, SW_SHOW);
            ShowWindow(hSpid, SW_SHOW);
            ShowWindow(hSrev, SW_SHOW);
            ShowWindow(hEvid, SW_SHOW);
            ShowWindow(hEpid, SW_SHOW);
            SendDlgItemMessage(hWnd, IDB_MODE, WM_SETTEXT, FALSE, (LPARAM)"Назад"); 
            ShowWindow(hErev, SW_SHOW);
          }
          else
          {
            ShowWindow(hSip, SW_SHOW);
            ShowWindow(hEip, SW_SHOW);
            ShowWindow(hSvid, SW_HIDE);
            ShowWindow(hSpid, SW_HIDE);
            ShowWindow(hSrev, SW_HIDE);
            ShowWindow(hEvid, SW_HIDE);
            ShowWindow(hEpid, SW_HIDE);
            ShowWindow(hErev, SW_HIDE);
            SendDlgItemMessage(hWnd, IDB_MODE, WM_SETTEXT, FALSE, (LPARAM)"MobileBrServ"); 
          }
          break;

        case IDB_DEBUG:
          if(mode == FALSE) SendPost(hWnd, wParam);
          else MobileBrServ(hWnd, wParam);
          break;

        case IDB_PROJECT:
          if(mode == FALSE) SendPost(hWnd, wParam);
          else MobileBrServ(hWnd, wParam);
          break;

        case IDCANCEL:
          EndDialog(hWnd, 0);
          break;

        default:
          return FALSE;
      }
      break;
    }
    case WM_CLOSE:
      EndDialog(hWnd, 0);
      break;

    default:
      return FALSE;
  }
  return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
  DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG), NULL, 
                (DLGPROC)DialogProc, (LPARAM)hInstance);
  return 0;
}