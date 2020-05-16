enum COMMAND
{
  CMD_SCREEN,
  CMD_WM_MOUSEMOVE,
  CMD_WM_LBUTTONDOWN,
  CMD_WM_LBUTTONUP,
  CMD_WM_LBUTTONDBLCLK,
  CMD_WM_KEYDOWN,
  CMD_WM_KEYUP,
};

struct CMD_HEAD_INFO
{
  int m_nCmd;
  int m_Len;
};

struct CMD_SCREEN_INFO
{
  int m_nWidth;
  int m_nHeight;
  int m_nTotalSize;
};
struct CMD_MOUSE_INFO
{
  int m_nX;
  int m_nY;
};

struct CMD_KEYBOARY_INFO
{
  int m_nVirKey;
};