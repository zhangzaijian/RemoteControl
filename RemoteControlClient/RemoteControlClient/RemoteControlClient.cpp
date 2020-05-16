// RemoteControlClient.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "RemoteControlClient.h"
#include "../../RemoteControlServer/RemoteControlServer/protocol.h"
#include <WinSock2.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO:  在此放置代码。
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);


	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_REMOTECONTROLCLIENT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_REMOTECONTROLCLIENT));

	// 主消息循环: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  函数:  MyRegisterClass()
//
//  目的:  注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_REMOTECONTROLCLIENT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_REMOTECONTROLCLIENT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数:  InitInstance(HINSTANCE, int)
//
//   目的:  保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


void ReportError(const char* pre)
{
  //#if win_32
  //#if linux
  LPVOID lpMsgBuf;
  FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    GetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR)&lpMsgBuf,
    0,
    NULL
    );

  MessageBox(NULL, (char*)lpMsgBuf, NULL, MB_OK);

  LocalFree(lpMsgBuf);

}

void ShowMsg(const char* msg)
{
  MessageBox(NULL, msg, "提示", MB_OK);
}

int g_nSocket = INVALID_SOCKET;

//连接
LRESULT OnConnect(HWND hWnd)
{
  if (g_nSocket > 0)
  {
    ShowMsg("服务器已连接");
    return 0;
  }

  g_nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (g_nSocket <= 0) 
  {
    ReportError("socket");
    return 0;
  }

  //连接
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(58888);
  if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) < 0) 
  {
    ReportError("inet_pton");
    return 0;
  }
  int nRet = connect(g_nSocket, (sockaddr*)&addr, sizeof(addr));
  if (nRet < 0)
  {
    ReportError("connect");
    return 0;
  }


  ShowMsg("connect ok");
  return 0;
}

//关闭连接
LRESULT OnDisConnect(HWND hWnd)
{
  if (g_nSocket > 0)
  {
    closesocket(g_nSocket);
    g_nSocket = INVALID_SOCKET;
  }
  else
  {
    ShowMsg("服务器已断开");
    return 0;
  }


  //showMsg("OnDisConnect");
  return 0;
}


//显示截图
void ShowScreen(HWND hWnd, int width, int height, char* buffer)
{
  HDC hdcWind = ::GetDC(hWnd);
  //创建内存dc
  HDC hdcCompatible = ::CreateCompatibleDC(hdcWind);
  if (hdcCompatible == NULL)
  {
    ReportError(_T("CreateCompatibleDC"));
    return;
  }
  //创建位图内存
  HBITMAP hbmScreen = ::CreateCompatibleBitmap(hdcWind, width, height);
  if (hbmScreen == NULL)
  {
    ReportError(_T("CreateCompatibleBitmap"));
    return;
  }

  //选择位图
  HGDIOBJ hdcOld = ::SelectObject(hdcCompatible, hbmScreen);
  if (hdcOld == NULL)
  {
    ReportError(_T("SelectObject"));
    return;
  }
  //填充位图
  LONG lRet = SetBitmapBits(hbmScreen, width * height * 4, buffer);
  if (lRet == 0)
  {
    ReportError(_T("SetBitmapBits"));
    return;
  }
  //拷贝位图到窗口
  bool bRet = ::BitBlt(hdcWind, 0, 0, width, height, hdcCompatible, 0, 0, SRCCOPY);
  if (!bRet)
  {
    ReportError(_T("BitBlt"));
    return;
  }

}
//处理所有的鼠标事件，移动，单机，双击都要发送消息，把最新的操作都更新过来显示

char* g_pDecomBuffer = NULL;
CMD_SCREEN_INFO* g_info = NULL;
typedef int(*PFNUNCOMPRESS)(void*, unsigned long*, void*, unsigned long);
BOOL bStartup = FALSE;

//处理回复的CMD_SCREEN消息
int OnCmdScreen(HWND hWnd, CMD_HEAD_INFO head, PFNUNCOMPRESS pfnUncompress)
{
  CMD_SCREEN_INFO info;
  int nRet = recv(g_nSocket, (char*)&info, sizeof(info), 0);
  if (nRet < 0)
  {
    ReportError(_T("recv CMD_SCREEN_INFO"));
    return -1;
  }
  g_info = new CMD_SCREEN_INFO;
  memcpy_s(g_info, sizeof(info), &info, sizeof(info));

  char *lpCompressBuff = new char[head.m_Len];
  int nEnSize = head.m_Len;
  int RecvBytes = 0;
  int bytes = 0;
  while (RecvBytes < nEnSize)
  {
    bytes = recv(g_nSocket, lpCompressBuff + RecvBytes, nEnSize - RecvBytes, 0);
    if (bytes < 0)
    {
      ReportError(_T("recv lpCompressBuff"));
      return -1;
    }
    RecvBytes += bytes;
  }
  //解压缩
  char *pDecomBuffer = new char[info.m_nTotalSize];
  nRet = pfnUncompress(pDecomBuffer, (unsigned long*)&info.m_nTotalSize, lpCompressBuff, nEnSize);
  if (nRet != 0)
  {
    ReportError(_T("pfnUncompress error"));
    return nRet;
  }
  g_pDecomBuffer = pDecomBuffer;
  //显示截图
  ShowScreen(hWnd, info.m_nWidth, info.m_nHeight, pDecomBuffer);
  delete[] lpCompressBuff;
  lpCompressBuff = NULL;
  delete[] pDecomBuffer;
  pDecomBuffer = NULL;
  delete g_info;
  g_info = NULL;
}
//处理所有服务器回复的消息
DWORD WINAPI workThread(LPVOID lpParameter)
{
  bStartup = TRUE;
  HWND hWnd = (HWND)lpParameter;
  if (g_nSocket < 0)
  {
    ShowMsg("请连接服务器");
    return 0;
  }

  //获取解压函数
  HMODULE hDll = ::LoadLibrary(_T("D:\\vs2013Code\\RemoteControlServer\\RemoteControlServer\\zlib1.dll"));
  if (hDll == NULL)
  {
    ReportError(_T("LoadLibrary"));
    return -1;
  }
  PFNUNCOMPRESS pfnUncompress = (PFNUNCOMPRESS)GetProcAddress(hDll, "uncompress");
  if (pfnUncompress == NULL)
  {
    ReportError(_T("GetProcAddress pfnUncompress"));
    return -1;
  }

  CMD_HEAD_INFO head;
  head.m_nCmd = CMD_SCREEN;
  head.m_Len = 0;
  int bytes;
  bytes = send(g_nSocket, (char*)&head, sizeof(head), 0);
  if (bytes < 0)
  {
    ReportError(_T("send CMD_HEAD_INFO"));
    return -1;
  }

  //接收消息
  while (true)
  {
	  int nRet = recv(g_nSocket, (char*)&head, sizeof(head), 0);
	  if (nRet < 0)
	  {
		  ReportError(_T("recv head"));
		  return -1;
	  }
	  switch (head.m_nCmd)
	  {
	  case CMD_SCREEN:
		  nRet = OnCmdScreen(hWnd, head, pfnUncompress);
		  if (nRet < 0)
		  {
			  ReportError(_T("recv head"));
			  return -1;
		  }
		  break;
	  }
  }
 
  return 0;
}
// 处理鼠标指针移动消息  
int WINAPI OnMyMouseMove(HWND hWnd, int x, int y, int nMark)
{
  //发送鼠标移动消息，服务器截图后返回
  CMD_MOUSE_INFO stMousePoint;
  CMD_HEAD_INFO head;
  head.m_nCmd = CMD_WM_MOUSEMOVE;
  head.m_Len = sizeof(CMD_MOUSE_INFO);
  int bytes;
  bytes = send(g_nSocket, (char*)&head, sizeof(head), 0);
  if (bytes < 0)
  {
    ReportError(_T("OnMouseMove send CMD_HEAD_INFO"));
    return -1;
  }
  stMousePoint.m_nX = x;
  stMousePoint.m_nY = y;
  bytes = send(g_nSocket, (char*)&stMousePoint, sizeof(stMousePoint), 0);
  if (bytes < 0)
  {
    ReportError(_T("OnMouseMove send CMD_MOUSE_INFO"));
    return -1;
  }
}

// 处理鼠标左键按下消息  
void WINAPI OnMouseLButtonDown(HWND hWnd, int x, int y, int nMark)
{
  CMD_MOUSE_INFO stMousePoint;
  CMD_HEAD_INFO head;
  head.m_nCmd = CMD_WM_LBUTTONDOWN;
  head.m_Len = sizeof(stMousePoint);
  int bytes;
  bytes = send(g_nSocket, (char*)&head, sizeof(head), 0);
  if (bytes < 0)
  {
    ReportError(_T("OnMouseMove send CMD_HEAD_INFO"));
    return;
  }
  stMousePoint.m_nX = x;
  stMousePoint.m_nY = y;
  bytes = send(g_nSocket, (char*)&stMousePoint, sizeof(stMousePoint), 0);
  if (bytes < 0)
  {
    ReportError(_T("OnMouseMove send CMD_MOUSE_INFO"));
    return;
  }
}

// 处理鼠标左键抬起消息  
void WINAPI OnMouseLButtonUp(HWND hWnd, int x, int y, int nMark)
{
  CMD_MOUSE_INFO stMousePoint;
  CMD_HEAD_INFO head;
  head.m_nCmd = CMD_WM_LBUTTONUP;
  head.m_Len = sizeof(stMousePoint);
  int bytes;
  bytes = send(g_nSocket, (char*)&head, sizeof(head), 0);
  if (bytes < 0)
  {
    ReportError(_T("OnMouseMove send CMD_HEAD_INFO"));
    return;
  }
  stMousePoint.m_nX = x;
  stMousePoint.m_nY = y;
  bytes = send(g_nSocket, (char*)&stMousePoint, sizeof(stMousePoint), 0);
  if (bytes < 0)
  {
    ReportError(_T("OnMouseMove send CMD_MOUSE_INFO"));
    return;
  }
}

// 处理鼠标左键双击消息  
void WINAPI OnMouseLButtonDoubleClick(HWND hWnd, int x, int y, int nMark)
{
  CMD_MOUSE_INFO stMousePoint;
  CMD_HEAD_INFO head;
  head.m_nCmd = CMD_WM_LBUTTONDBLCLK;
  head.m_Len = sizeof(stMousePoint);
  int bytes;
  bytes = send(g_nSocket, (char*)&head, sizeof(head), 0);
  if (bytes < 0)
  {
    ReportError(_T("OnMouseMove send CMD_HEAD_INFO"));
    return;
  }
  stMousePoint.m_nX = x;
  stMousePoint.m_nY = y;
  bytes = send(g_nSocket, (char*)&stMousePoint, sizeof(stMousePoint), 0);
  if (bytes < 0)
  {
    ReportError(_T("OnMouseMove send CMD_MOUSE_INFO"));
    return;
  }
}

//处理键盘消息
void OnKeyDown(WPARAM wparam)
{
  CMD_KEYBOARY_INFO stVirKey;
  CMD_HEAD_INFO head;
  head.m_nCmd = CMD_WM_KEYDOWN;
  head.m_Len = sizeof(stVirKey);
  int bytes;
  bytes = send(g_nSocket, (char*)&head, sizeof(head), 0);
  if (bytes < 0)
  {
    ReportError(_T("OnMouseMove send CMD_HEAD_INFO"));
    return;
  }
  stVirKey.m_nVirKey = wparam;
  bytes = send(g_nSocket, (char*)&stVirKey, sizeof(stVirKey), 0);
  if (bytes < 0)
  {
    ReportError(_T("OnMouseMove send CMD_HEAD_INFO"));
    return;
  }
}
void OnKeyUp(WPARAM wparam)
{
  CMD_KEYBOARY_INFO stVirKey;
  CMD_HEAD_INFO head;
  head.m_nCmd = CMD_WM_KEYUP;
  head.m_Len = sizeof(stVirKey);
  int bytes;
  bytes = send(g_nSocket, (char*)&head, sizeof(head), 0);
  if (bytes < 0)
  {
    ReportError(_T("OnMouseMove send CMD_HEAD_INFO"));
    return;
  }
  stVirKey.m_nVirKey = wparam;
  bytes = send(g_nSocket, (char*)&stVirKey, sizeof(stVirKey), 0);
  if (bytes < 0)
  {
    ReportError(_T("OnMouseMove send CMD_HEAD_INFO"));
    return;
  }
}

//  函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
    case IDM_CONNECT:
      OnConnect(hWnd);
      break;
    case IDM_CLOSE:
      OnDisConnect(hWnd);
      break;
	case IDM_SCREEN:
		CreateThread(NULL, 0, workThread,(LPVOID)hWnd, 0, NULL);
      break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO:  在此添加任意绘图代码...
    //显示截图
    if (g_info != NULL && g_pDecomBuffer != NULL)
    {
      ShowScreen(hWnd, g_info->m_nWidth, g_info->m_nHeight, g_pDecomBuffer);
    }
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
  case WM_MOUSEMOVE:
	  if (g_nSocket > 0 && bStartup == TRUE)
	  {
		  OnMyMouseMove(hWnd, LOWORD(lParam), HIWORD(lParam), (int)wParam); (hWnd, LOWORD(lParam), HIWORD(lParam), (int)wParam);
		  break;
	  }
    else
    {
		  break;
	  }
  case WM_LBUTTONDOWN:
    if (g_nSocket > 0&&bStartup==TRUE)
    {
		  OnMouseLButtonDown(hWnd, LOWORD(lParam), HIWORD(lParam), (int)wParam); (hWnd, LOWORD(lParam), HIWORD(lParam), (int)wParam);
		  break;
    }
    else
    {
		  break;
	  }
  case WM_LBUTTONUP:
    if (g_nSocket > 0 && bStartup == TRUE)
    {
      OnMouseLButtonUp(hWnd, LOWORD(lParam), HIWORD(lParam), (int)wParam); (hWnd, LOWORD(lParam), HIWORD(lParam), (int)wParam);
      break;
    }
    else
    {
      break;
    }
  case WM_LBUTTONDBLCLK:
    if (g_nSocket > 0 && bStartup == TRUE)
    {
      OnMouseLButtonDoubleClick(hWnd, LOWORD(lParam), HIWORD(lParam), (int)wParam); (hWnd, LOWORD(lParam), HIWORD(lParam), (int)wParam);
      break;
    }
    else
    {
      break;
    }
  case WM_KEYDOWN:
    if (g_nSocket > 0 && bStartup == TRUE)
    {
      OnKeyDown(wParam);
      break;
    }
    else
    {
      break;
    }
  case WM_KEYUP:
    if (g_nSocket > 0 && bStartup == TRUE)
    {
      OnKeyUp(wParam);
      break;
    }
    else
    {
      break;
    }
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
