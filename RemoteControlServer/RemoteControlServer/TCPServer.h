#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

class CTCPServer
{
public:
  CTCPServer();
  ~CTCPServer();
  void CreateSocket();//创建socket
  void BindSocket();//绑定
  void ListenSocket();//监听,监听的最大连接数是值同步的最大连接值
  int AcceptSocket();//等待连接
  static void ReportError(const char* pError);//错误处理
  static void ShowMsg(const char* pLog);
private:
  int m_nSocket;

};

