/*
**
**                      -+- rx -+-
**
**                 Mundane Graphics API.
**
**  *--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*
**  Copyright(C) Dayan Rodriguez, 2022, All Rights Reserved
**  *--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*
**
**        NOT PRODUCTION READY/NOT PRODUCTION QUALITY
**
**
**               HONI SOIT QUI MAL Y PENSE
**
**
**                 github.com/MicroRJ/rx
**
*/

/* todo: */
void rxwindow_init(LPCWSTR window_title)
{
  WNDCLASSW WindowClass;
  ZeroMemory(&WindowClass,sizeof(WindowClass));
  WindowClass.lpfnWndProc=rxwndmsg_callback_win32;
  WindowClass.hInstance=GetModuleHandleW(NULL);
  WindowClass.lpszClassName=window_title;
  RegisterClassW(&WindowClass);


  int WindowSizeX=_RX_DEFAULT_WINDOW_SIZE_X;
  int WindowSizeY=_RX_DEFAULT_WINDOW_SIZE_Y;

  if((WindowSizeX != CW_USEDEFAULT) ||
     (WindowSizeY != CW_USEDEFAULT) )
  {
    if(WindowSizeX == CW_USEDEFAULT) WindowSizeX = 720;
    if(WindowSizeY == CW_USEDEFAULT) WindowSizeY = 720;
  }

  if((WindowSizeX != CW_USEDEFAULT) &&
     (WindowSizeY != CW_USEDEFAULT) )
  {
    RECT WindowRect;
    WindowRect.  left=0;
    WindowRect.   top=0;
    WindowRect. right=_RX_DEFAULT_WINDOW_SIZE_X;
    WindowRect.bottom=_RX_DEFAULT_WINDOW_SIZE_Y;
    AdjustWindowRect(&WindowRect,WS_OVERLAPPEDWINDOW,FALSE);

    WindowSizeX = WindowRect. right - WindowRect.left;
    WindowSizeY = WindowRect.bottom - WindowRect. top;
  }

  rx.wnd.win32.obj = CreateWindowExW(
    WS_EX_NOREDIRECTIONBITMAP,
      WindowClass.lpszClassName,window_title,
      /* This makes the window not resizable */
      WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME,
        CW_USEDEFAULT,CW_USEDEFAULT,
          WindowSizeX,WindowSizeY,NULL,NULL,WindowClass.hInstance,NULL);
}

void rxwnd_show()
{
  if(rx.wnd.vis)
    return;

  rx.wnd.vis = TRUE;
  ShowWindow(rx.wnd.win32.obj,SW_SHOW);
}

/* XXX */
void rxwindow_resize(int size_x, int size_y)
{
  RECT Client;
  Client.  left=0;
  Client.   top=0;
  Client. right=size_x;
  Client.bottom=size_y;
  AdjustWindowRect(&Client,WS_OVERLAPPEDWINDOW,FALSE);

  SetWindowPos(rx.wnd.win32.obj,HWND_NOTOPMOST,0,0,Client.right,Client.bottom,
    SWP_NOMOVE|SWP_NOACTIVATE|SWP_FRAMECHANGED);

}

/* XXX */
void rxwindow_xy(int x, int y)
{
  SetWindowPos(rx.wnd.win32.obj,HWND_TOP,x,y,0,0,SWP_NOSIZE);
}

/* XXXX */
void rxwnd_poll()
{
  /* todo: */
  memcpy(rx.wnd.in.kbrd.key_lst,rx.wnd.in.kbrd.key,sizeof(rx.wnd.in.kbrd.key));
  memset(rx.wnd.in.kbrd.key,                     0,sizeof(rx.wnd.in.kbrd.key));
  rx.wnd.in.kbrd.chr = 0;

  rx.wnd.in.mice.yscroll = 0;
  rx.wnd.in.mice.xscroll = 0;


  MSG message;
  while(PeekMessage(&message,NULL,0,0,PM_REMOVE))
  { TranslateMessage(&message);
    DispatchMessageW(&message);
  }

  RECT client;
  GetClientRect(rx.wnd.win32.obj,&client);

  rx.wnd.size_x = client. right - client.left;
  rx.wnd.size_y = client.bottom - client. top;
  rx.wnd.center_x=rx.wnd.size_x>>1;
  rx.wnd.center_y=rx.wnd.size_y>>1;
}

LRESULT CALLBACK
rxwndmsg_callback_win32(HWND Window,UINT Message,WPARAM wParam,LPARAM lParam)
{
  LRESULT result = rxwndmsg_handler_win32(Message,wParam,lParam);

  if(result != TRUE)
  {
    result = DefWindowProcW(Window,Message,wParam,lParam);
  }

  return result;
}

int
rxwndmsg_handler_win32(UINT Message, WPARAM wParam, LPARAM lParam)
{
  switch(Message)
  { case WM_CLOSE:
    case WM_QUIT:
    { PostQuitMessage(0);
      rx.wnd.off = TRUE;
    } break;
    case WM_MOUSEMOVE:
    {
      int xcursor=GET_X_LPARAM(lParam);
      int ycursor=GET_Y_LPARAM(lParam);

      rx.wnd.in.mice.xcursor=xcursor;
      rx.wnd.in.mice.ycursor=rx.wnd.size_y-ycursor;
    } break;
    case WM_MOUSEWHEEL:
    {
      rx.wnd.in.mice.yscroll = GET_WHEEL_DELTA_WPARAM(wParam)/WHEEL_DELTA;
      // rx.key[rx_kMVWHEEL] = TRUE;
    } break;
    case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK: case WM_LBUTTONUP:
    {
      rx.wnd.in.mice.btn_old  = rx.wnd.in.mice.btn_cur;
      rx.wnd.in.mice.btn_cur |= (Message!=WM_LBUTTONUP) << 1;
    } break;
    case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK: case WM_RBUTTONUP:
    {
      rx.wnd.in.mice.btn_old  = rx.wnd.in.mice.btn_cur;
      rx.wnd.in.mice.btn_cur |= (Message!=WM_RBUTTONUP) << 2;
    } break;
    case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK: case WM_MBUTTONUP:
    {
      rx.wnd.in.mice.btn_old  = rx.wnd.in.mice.btn_cur;
      rx.wnd.in.mice.btn_cur |= (Message!=WM_MBUTTONUP) << 3;
    } break;

    // case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
    // case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
    // case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
    // case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
    // { rx.key[rx_kLBUTTON]=1;
    //   if(!rx.ClickFocused)
    //   { rx.ClickFocused=TRUE;
    //     SetCapture((HWND)rx.wnd.win32.obj);
    //   }
    // } break;
    // case WM_LBUTTONUP: case WM_RBUTTONUP:
    // case WM_MBUTTONUP: case WM_XBUTTONUP:
    // { rx.key[rx_kLBUTTON]=0;

    //   rx.ClickFocused=FALSE;
    //   ReleaseCapture();
    // } break;

    case WM_CHAR:
    {
      /* todo: ensure this is safe */
      rx.wnd.in.kbrd.chr = 0;

      if(CCWITHIN(wParam,0,0x80))
      {
        rx.wnd.in.kbrd.chr = wParam;
      }

    } return TRUE;

    /* maybe rx should provide a flag on what to do here, by default I don't
      think most people care */
    case WM_SYSKEYUP:
    case WM_SYSKEYDOWN:
    {

    } return FALSE;
    case WM_KEYUP:
    case WM_KEYDOWN:
    {
      /* todo: there's probably a better way to do this */
      rx.wnd.in.kbrd.is_shft = (GetKeyState(VK_SHIFT)   & 0x8000) != 0;
      rx.wnd.in.kbrd.is_ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
      rx.wnd.in.kbrd.is_menu = (GetKeyState(VK_MENU)    & 0x8000) != 0;

      int key_map = 0;

      if(CCWITHIN(wParam,'a','z') ||
         CCWITHIN(wParam,'A','Z') ||
         CCWITHIN(wParam,'0','9'))
      {
        key_map = wParam;
      }

      /* what a waste of time */
      switch(wParam)
      {
        case VK_CONTROL:    key_map = rx_kLCTRL;     break;
        case VK_RETURN:     key_map = rx_kRETURN;    break;
        case VK_BACK:       key_map = rx_kBCKSPC;    break;
        case VK_DELETE:     key_map = rx_kDELETE;    break;
        case VK_HOME:       key_map = rx_kHOME;      break;
        case VK_END:        key_map = rx_kEND;       break;
        case VK_ESCAPE:     key_map = rx_kESCAPE;    break;

        case VK_LEFT:       key_map = rx_kKEY_LEFT;  break;
        case VK_RIGHT:      key_map = rx_kKEY_RIGHT; break;
        case VK_UP:         key_map = rx_kKEY_UP;    break;
        case VK_DOWN:       key_map = rx_kKEY_DOWN;  break;

        case VK_F1:  key_map = rx_kKEY_F1; break;
        case VK_F2:  key_map = rx_kKEY_F2; break;
        case VK_F3:  key_map = rx_kKEY_F3; break;
        case VK_F4:  key_map = rx_kKEY_F4; break;
        case VK_F5:  key_map = rx_kKEY_F5; break;
        case VK_F6:  key_map = rx_kKEY_F6; break;
        case VK_F7:  key_map = rx_kKEY_F7; break;
        case VK_F8:  key_map = rx_kKEY_F8; break;
        case VK_F9:  key_map = rx_kKEY_F9; break;
        case VK_F10: key_map = rx_kKEY_F10; break;
        case VK_F11: key_map = rx_kKEY_F11; break;
        case VK_F12: key_map = rx_kKEY_F12; break;
        /* waste of time */
        case VK_SPACE:      key_map =  ' ';          break;
        case VK_OEM_PLUS:   key_map =  '=';          break;
        case VK_OEM_PERIOD: key_map =  '.';          break;
        case VK_OEM_COMMA:  key_map =  ',';          break;
        case VK_OEM_1:      key_map =  ';';          break;
        case VK_OEM_2:      key_map =  '/';          break;
        case VK_OEM_3:      key_map =  '~';          break;
        case VK_OEM_4:      key_map =  '[';          break;
        case VK_OEM_5:      key_map = '\\';          break;
        case VK_OEM_6:      key_map =  ']';          break;
        case VK_OEM_7:      key_map = '\'';          break;
      }

      rx.wnd.in.kbrd.key[key_map] =
        (Message == WM_KEYDOWN)   ||
        (Message == WM_SYSKEYDOWN);

    } return TRUE;
    default:
    { return FALSE;
    } break;
  }
  return TRUE;
}


