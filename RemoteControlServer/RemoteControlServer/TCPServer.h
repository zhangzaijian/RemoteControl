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
  void CreateSocket();//����socket
  void BindSocket();//��
  void ListenSocket();//����,�����������������ֵͬ�����������ֵ
  int AcceptSocket();//�ȴ�����
  static void ReportError(const char* pError);//������
  static void ShowMsg(const char* pLog);
private:
  int m_nSocket;

};

