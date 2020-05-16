#include "TCPServer.h"


CTCPServer::CTCPServer()
{
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
  CreateSocket();
}


CTCPServer::~CTCPServer()
{
}

void CTCPServer::CreateSocket()
{
  m_nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (m_nSocket <= 0) {
    ReportError(_T("socket"));
    return;
  }
  ShowMsg(_T("socket ok"));
}

void CTCPServer::BindSocket()
{
  printf("请输入服务器IP地址：");
  char szBufIp[100] = { 0 };
  scanf_s("%s",szBufIp,20);
  //绑定端口
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(58888);
  if (inet_pton(AF_INET, szBufIp, &addr.sin_addr) < 0) {
    ReportError(_T("inet_pton"));
    return;
  }


  if (bind(m_nSocket, (sockaddr*)&addr, sizeof(addr)) < 0) {
    ReportError(_T("bind"));
    return;
  }
  ShowMsg(_T("bind ok"));

}

void CTCPServer::ListenSocket()
{
  //监听
  if (listen(m_nSocket, SOMAXCONN) < 0) {
    ReportError(_T("listen"));
    return;
  }
  ShowMsg(_T("listen ok"));

}

int CTCPServer::AcceptSocket()
{
  ShowMsg(_T("accept..."));
  sockaddr_in stCaddr;
  int len = sizeof(stCaddr);
  int nClientSocket = accept(m_nSocket, (sockaddr*)&stCaddr, &len);
  if (nClientSocket < 0) 
  {
    return -1;
  }
  return nClientSocket;
}

void CTCPServer::ReportError(const char* pError)
{
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

  _tprintf(_T("%s:%s"), pError, lpMsgBuf);


  LocalFree(lpMsgBuf);
}

void CTCPServer::ShowMsg(const char* pLog)
{

    _tprintf(_T("%s\n"), pLog);
}
