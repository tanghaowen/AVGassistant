/*------------------------------------------------------------
   HELLOWIN.C -- Displays "Hello, Windows 98!" in client area
                 (c) Charles Petzold, 1998
  ------------------------------------------------------------*/

#include <windows.h>
#include "resource.h"
#include <process.h>
#include <tchar.h>
#include <string.h>
#define HorizontalScreen 0  //横屏
#define VerticalScreen 1 //竖屏

#define UP 0
#define DOWN 1

int screenX=0,screenY=0;   //屏幕分辨率
int screenStatus=HorizontalScreen;

bool isWhile = false;
HWND windowHwnd;


static TCHAR windowClassName[200] = TEXT ("") ;
static TCHAR windowTitleName[200]=TEXT("无句柄");
static TCHAR szAppName[] = TEXT("GalAss");
static TCHAR szAppName2[] = TEXT("BackGround");

static bool ifNext=false;//是否下一步
static bool ifCompatible=false;
HINSTANCE hinstance;
static PROCESS_INFORMATION pi;

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
LRESULT CALLBACK BackgroundWndProc (HWND, UINT, WPARAM, LPARAM) ;
BOOL CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam) ;
bool removeWindowTitile( HWND );
bool sendKey( int key );
bool sendSinglKeyDown( int key );
bool sendSinglKeyUp( int key );
bool moveGameWindow( HWND );
void Thread (PVOID pvoid );
void Thread2 (PVOID pvoid );
bool mouseWheel( int stats );
bool sendSinglKeyDown( int key , int time );
bool sendSinglKeyUp( int key , int time );
bool InjectDLL( DWORD Pid );

struct ProcessWindow  
{  
    DWORD dwProcessId;  
    HWND hwndWindow;  
}; 

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{

     HWND         hwnd , hwnd2 ;
     MSG          msg ;
     WNDCLASS     wndclass ;

	 hinstance = hInstance;






     wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szAppName ;

     if (!RegisterClass (&wndclass))
     {
				  MessageBox (NULL, TEXT ("注册窗口类1失败!"), 
							  szAppName, MB_ICONERROR) ;
          return 0 ;
     }

     
     hwnd = CreateWindowEx (WS_EX_ACCEPTFILES,szAppName,                  // window class name
                          TEXT ("AVG assistant for windows tablet"), // window caption
						  WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME,        // window style
                          CW_USEDEFAULT,              // initial x position
                          CW_USEDEFAULT,              // initial y position
                          600,              // initial x size
                          250,              // initial y size
                          NULL,                       // parent window handle
                          NULL,                       // window menu handle
                          hInstance,                  // program instance handle
                          NULL) ;                     // creation parameters
     
     ShowWindow (hwnd, iCmdShow) ;
     UpdateWindow (hwnd) ;
     
     while (GetMessage (&msg, NULL, 0, 0))
     {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
     }
	 if ( ifNext )
	 {


		        wndclass.lpfnWndProc   = BackgroundWndProc ;
				wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH) ;
				wndclass.lpszClassName = szAppName2 ;

				if (!RegisterClass (&wndclass))
				{
							MessageBox (NULL, TEXT ("注册窗口类2失败!"), 
							  szAppName, MB_ICONERROR) ;
					return 0 ;
				}


				screenX = GetSystemMetrics ( SM_CXSCREEN );//获取屏幕分辨率
				screenY = GetSystemMetrics ( SM_CYSCREEN ); 

				 if ( screenX < screenY )  //判断横竖屏
				{
					screenStatus = VerticalScreen;
				}
				else
				{
					screenStatus = HorizontalScreen;
				}


				hwnd2 = CreateWindowEx ( WS_EX_NOACTIVATE, szAppName2,                  // window class name
                          TEXT ("Background Window"), // window caption
                          WS_POPUP,        // window style
                          0,              // initial x position
                          0,              // initial y position
                          screenX,              // initial x size
                          screenY,              // initial y size
                          NULL,                       // parent window handle
                          NULL,                       // window menu handle
                          hInstance,                  // program instance handle
                          NULL) ;                     // creation parameters

				ShowWindow (hwnd2, iCmdShow) ;
				UpdateWindow (hwnd2) ;
     
				while (GetMessage (&msg, NULL, 0, 0))
				{
					TranslateMessage (&msg) ;
					DispatchMessage (&msg) ;
				 }


		}








     return msg.wParam ;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
     HDC         hdc,hdcPic , hdcMem ;
     PAINTSTRUCT ps ;
     RECT        rect ;
	 HBITMAP hbitmapPic , hbitmapMem;
	 static LOGFONT lf;
	 static HFONT hfont;

     static bool ifClick=false;
	 POINT windowPoint;

	static TCHAR lpszFile[MAX_PATH] = {0};
	UINT uFile = 0;
	HDROP hDrop = (HDROP)wParam;  
	static STARTUPINFO si = {sizeof(si)}; 

	static bool stats;


     switch (message)
     {
     case WM_CREATE:


          return 0 ;
          
     case WM_PAINT:

		  lf.lfHeight = 20;
		  lf.lfItalic = 0;
		  lf.lfCharSet = DEFAULT_CHARSET;
		  			 wsprintf(lf.lfFaceName , TEXT("%s") ,  TEXT("微软雅黑"));

		  hfont = CreateFontIndirect( &lf );
          hdc = BeginPaint (hwnd, &ps) ;
		  GetClientRect (hwnd, &rect) ;





          hdcPic = CreateCompatibleDC( hdc );
          hbitmapPic =(HBITMAP) LoadImage( hinstance , MAKEINTRESOURCE( IDB_BITMAP1 ) , IMAGE_BITMAP , 0 , 0 ,NULL );
		  SelectObject( hdcPic , hbitmapPic );
		  
		  hdcMem = CreateCompatibleDC( hdc );

		  hbitmapMem = CreateCompatibleBitmap( hdc , 1920 , 1080 );
		  SelectObject( hdcMem , hbitmapMem );
		  SelectObject(hdcMem,(HBRUSH) GetStockObject (WHITE_BRUSH));
		  Rectangle(hdcMem,0,0,1920,1080);


		  

		  SelectObject(hdcMem , hfont );
		 
		  BitBlt( hdcMem , 5 , 45  , 29 ,22 , hdcPic , 0 , 0 ,SRCCOPY );
		  TextOut( hdcMem , 0 , 0 , TEXT("下の的をドラッグし、ゲームのウィンドウの上でリリースしてください") , lstrlen(TEXT("下の的をドラッグし、ゲームのウィンドウの上でリリースしてください ")));
		  TextOut( hdcMem , 0 , 25 , TEXT("  ↓") , lstrlen(TEXT("  ↓")));
		  TextOut( hdcMem , 0 , 70 , windowTitleName , lstrlen(windowTitleName));
		  TextOut( hdcMem , 0 , 95 , windowClassName , lstrlen(windowClassName));

		  SetTextColor(hdcMem , RGB( 98 , 98 , 98 ));

		  TextOut( hdcMem , 0 , 120 , TEXT("————————————————————————————————————————————————————————") , lstrlen( TEXT("————修复————————————————————————————————————————————————————")));
		  TextOut( hdcMem , 0 , 150 , TEXT("（もし本プログラムが不正確に中止し、タスクバーが消えたら下の文字をクリックして直してください）") , lstrlen(TEXT("（もし本プログラムが不正確に中止し、タスクバーが消えたら下の文字をクリックして直してください）")));
		  SetTextColor(hdcMem , RGB( 255 , 0 , 0 ));
		  SetBkColor( hdcMem , RGB( 0 , 0 , 255));
		  TextOut( hdcMem , 0 , 180 , TEXT("（Repair）") , lstrlen(TEXT("（Repair）")));

		  BitBlt( hdc , -1 , -1 , 1920 , 1080  , hdcMem , 0 , 0 ,SRCCOPY );


	//	  DrawText( hdc , TEXT("111111111111111111sdfd是打发的说法") , -1,&rect,DT_SINGLELINE | DT_CENTER | DT_VCENTER); 

          DeleteDC( hdcMem );
		  DeleteObject( hbitmapPic );
		  DeleteObject( hbitmapMem );

          EndPaint (hwnd, &ps) ;
		  DeleteObject( hfont);
          return 0 ;
	
	/* case WM_DROPFILES:

	


		uFile = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, NULL);
		if (uFile != 1){
			MessageBox (NULL, TEXT("不要丢多个文件进来"), TEXT("不要丢多个文件进来"), MB_ICONERROR);
			DragFinish(hDrop);
			break;
			}
		lpszFile[0] = '\0';
		DragQueryFile(hDrop, 0, lpszFile, MAX_PATH);

			
		DragFinish(hDrop);



		stats = CreateProcess(  lpszFile , NULL , NULL , NULL ,FALSE , CREATE_SUSPENDED , NULL , NULL, &si ,&pi );

		if ( !stats )
		{
			MessageBox(NULL , TEXT("创建进程失败") ,TEXT("创建进程失败"), MB_OK );
			break;
		}

		if ( InjectDLL( pi.dwProcessId ) )
		{

			ifCompatible = true;
			SendMessage( hwnd ,WM_DESTROY , NULL ,NULL);
			ifNext = true;
			break;
			

		}
		else
		{
			MessageBox(NULL , TEXT("HOOK失败") ,TEXT("HOOK失败"), MB_OK );
			break;

		}


			break;

          
		  */

	 case WM_LBUTTONDOWN:
		 if ( LOWORD(lParam)<70 && HIWORD( lParam)>170 && HIWORD( lParam)<210)
		 {
			 ShowWindow( FindWindow(TEXT("Shell_TrayWnd"), NULL) ,SW_SHOW ); 
			 return 0 ;
		 }

		 if (HIWORD( lParam)<120)
		 {
			 		 SetCapture( hwnd );
					 SetCursor( (HCURSOR) LoadImage( hinstance , MAKEINTRESOURCE( IDC_CURSOR1 ) , IMAGE_CURSOR , 0 , 0 ,0x1000));
					 ifClick = true;
		 }



		 return 0;
	 case WM_MOUSEMOVE:

		 if (ifClick)
		 {
			  SetCursor( (HCURSOR) LoadImage( hinstance , MAKEINTRESOURCE( IDC_CURSOR1 ) , IMAGE_CURSOR , 0 , 0 , 0x1000));  //设置鼠标图标为资源内的IDC_CURSOR1
			  GetCursorPos( &windowPoint);
			  windowPoint.x +=14;
			  windowPoint.y += 11;
			  windowHwnd = WindowFromPoint( windowPoint );   //获得鼠标当前所在位置的窗口hwnd
			  GetWindowText( windowHwnd , windowTitleName , 200);  //获得窗口的标题栏
			  GetClassName( windowHwnd , windowClassName , 200 );//获取ClassNane
			  if ( wcscmp(windowClassName, szAppName) )
			  {
				  
				  InvalidateRect(hwnd, NULL, FALSE);  //无效化客户区，绘制文本
				  UpdateWindow(hwnd);
			  }
			  else
			  {
				  wsprintf(windowTitleName , TEXT("本程序"));
				  wsprintf(windowClassName, TEXT("本程序"));

				  InvalidateRect(hwnd, NULL, FALSE);  //无效化客户区，绘制文本
				  UpdateWindow(hwnd);
			  }



		 }
		 return 0;
	 case WM_LBUTTONUP:
		 if (ifClick)
		 {
		 SetCursor( LoadCursor (NULL, IDC_ARROW) );
		 ifClick = false;
		 GetCursorPos( &windowPoint);
		 ReleaseCapture();
		 windowPoint.x +=14;
		 windowPoint.y += 11;
		 
		 windowHwnd = WindowFromPoint( windowPoint );
		 GetWindowText( windowHwnd , windowTitleName , 200);
		 GetClassName( windowHwnd , windowClassName , 200 );
		 if (wcscmp(windowClassName, szAppName))
		 {

			 InvalidateRect(hwnd, NULL, FALSE);  //无效化客户区，绘制文本
			 UpdateWindow(hwnd);
		 }
		 else
		 {
			 wsprintf(windowTitleName, TEXT("本程序"));
			 wsprintf(windowClassName, TEXT("本程序"));

			 InvalidateRect(hwnd, NULL, FALSE);  //无效化客户区，绘制文本
			 UpdateWindow(hwnd);
		 }
		// Sleep(1000);
		 if ( MessageBox( hwnd , TEXT("このウィンドウをキャッチしますか？") , TEXT("このウィンドウをキャッチしますか") , MB_YESNO ) == IDYES )
		 {
			 if (!wcscmp(windowClassName, TEXT("本程序")))
			 {
				 return 0;
			 }
			 if ( GetParent( windowHwnd ))
			 {
				 MessageBox( hwnd , TEXT("このウィンドウはチャイルドウィンドウです、タイトルバーでドロップしてください") , TEXT("ウィンドウキャッチ") , MB_OK );
				 return 0;

			 }


			 ShowWindow (hwnd, SW_HIDE) ;
			 ifNext = true;




			 SendMessage( hwnd ,WM_DESTROY , NULL ,NULL);
		 }		 





		 }
		 
		 
		 


		 return 0;
          
     case WM_DESTROY:
		  
          PostQuitMessage (0) ;
          return 0 ;
     }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
}



LRESULT CALLBACK BackgroundWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND taskbar;
	static HMENU hmenu;
	HDC hdc , hdcMem , hdcPic;
    PAINTSTRUCT ps ;
    RECT        rect ;
	HBITMAP hbitmapMem , hbitmapPic;
	static int y =0;
	POINT point;
	static LOGFONT lf;
	static HFONT hfont;
	char tmpstr[20];
	static ProcessWindow procwin;  
	
	switch (message)
	{
	case WM_CREATE:
		  lf.lfHeight = 20;
		  lf.lfItalic = 0;
		  lf.lfCharSet = DEFAULT_CHARSET;
		  _tcscpy_s(lf.lfFaceName , TEXT("微软雅黑"));
		  hfont = CreateFontIndirect( &lf );



		SetTimer( hwnd , 1 , 1000 , NULL);//  测试用
		taskbar = FindWindow(TEXT("Shell_TrayWnd"), NULL);  //获得系统任务栏的句柄并隐藏。否则在使用中任务栏会被激活
		ShowWindow( taskbar , 0); 
		if ( !ifCompatible )
		{

		

		if ( !(windowHwnd = FindWindow( windowClassName , NULL)) ) 
		{
				MessageBox( hwnd , TEXT("获取窗口Hwnd失败") , TEXT("捕获窗口") , MB_OK );
				SendMessage( hwnd , WM_DESTROY  , NULL , NULL );
				return 0;
		}

		if (  hmenu = GetMenu( windowHwnd )  )  //如有菜单栏询问是否隐藏菜单栏
		{
			 if ( MessageBox (NULL, TEXT ("检测到游戏有菜单栏，是否尝试隐藏菜单栏"),TEXT ("是否隐藏菜单栏"), MB_YESNO) == IDYES )


			 {
				 DestroyMenu ( hmenu );
				 SetMenu( windowHwnd , NULL );
			 }

		}
		
		 SetWindowPos( windowHwnd, HWND_TOPMOST , 0 , 0 ,800,600 ,SWP_NOMOVE|SWP_NOSIZE);	//将黄油的窗口置顶	 
		 
		 SetWindowLong( windowHwnd , GWL_STYLE , WS_POPUP|WS_VISIBLE);  //修改黄油窗口样式，并调整窗口大小，定位到 （0,340）。平板竖屏的正中间
		 //MoveWindow(windowHwnd, 0 ,340 , 800, 600, TRUE);	  
		// SetWindowPos( windowHwnd, HWND_TOPMOST , 0 , 0 ,800,600 ,SWP_NOMOVE|SWP_NOSIZE);	//将黄油的窗口置顶	
		 moveGameWindow( windowHwnd);
		// SetWindowPos( windowHwnd, HWND_TOPMOST , 0 , 0 ,800,600 ,SWP_NOMOVE|SWP_ASYNCWINDOWPOS|SWP_NOCOPYBITS);

		}
		/*else
		{
			ResumeThread( pi.hThread );
			procwin.dwProcessId = pi.dwProcessId;  
			procwin.hwndWindow = NULL;  
			EnumWindows( EnumWindowCallBack, (LPARAM)&procwin);  

			procwin.hwndWindow = windowHwnd;
		    SetWindowPos( windowHwnd, HWND_TOPMOST , 0 , 0 ,800,600 ,SWP_NOMOVE|SWP_NOSIZE);	//将黄油的窗口置顶	 
			moveGameWindow( windowHwnd);


		}*/
		return 0;
		




	case WM_PAINT:
		  hdc = BeginPaint( hwnd , &ps );
		  GetClientRect (hwnd, &rect) ;

		  hbitmapMem = CreateCompatibleBitmap( hdc , 1300 , 1300 );
		  hdcMem= CreateCompatibleDC( hdc );
		  SelectObject( hdcMem , hbitmapMem );

		  hdcPic = CreateCompatibleDC( hdc );
		  hbitmapPic =(HBITMAP) LoadImage( hinstance , MAKEINTRESOURCE( IDB_BITMAP3 ) , IMAGE_BITMAP , 0 , 0 ,NULL );
		  SelectObject( hdcPic , hbitmapPic);

		  BitBlt(  hdcMem , 0 , screenY - 130 , 42 , 39 , hdcPic , 0 , 0 ,SRCCOPY );
		  DeleteObject( hbitmapPic );

		  hbitmapPic =(HBITMAP) LoadImage( hinstance , MAKEINTRESOURCE( IDB_BITMAP2 ) , IMAGE_BITMAP , 0 , 0 ,NULL );
		  SelectObject( hdcPic , hbitmapPic);

		  BitBlt(  hdcMem , 0 , screenY - 85 , 42 , 39 , hdcPic , 0 , 0 ,SRCCOPY );



		  SetTextColor(hdcMem , RGB( 98 , 98 , 98 ));
		  SetBkColor( hdcMem , RGB( 65 , 65 , 65));

		  

		  SelectObject(hdcMem , hfont );

		  TextOut( hdcMem , 0 , 0 , TEXT("  Exit") , lstrlen(TEXT("  Exit")));
		 // TextOut( hdcMem , screenX-500,0,TEXT("游戏中ALT + F4     ") , lstrlen(TEXT("游戏中ALT + F4     ")));

		  TextOut( hdcMem , 0 , 30 ,  TEXT("  ESC") , lstrlen(TEXT("  ESC")));

		  TextOut( hdcMem , 0 , screenY-40, TEXT("  Ctrl  ") , lstrlen(TEXT("  Ctrl  ")));
		  if (isWhile)
		  {
				TextOut( hdcMem , 50 , screenY-40 , TEXT("  Now Ctrl") , lstrlen(TEXT("  Now Ctrl")));
		  }
		  BitBlt( hdc , 0 , 0 ,1300 , 1300 ,hdcMem, 0 , 0 ,SRCCOPY);

          DeleteObject( hbitmapMem );
		  DeleteObject( hbitmapPic );
		  DeleteDC( hdcMem );
		  DeleteDC( hdcPic );

          EndPaint (hwnd, &ps) ;
		  

	
	case WM_DISPLAYCHANGE:

		screenX = GetSystemMetrics ( SM_CXSCREEN );//获取屏幕分辨率
		screenY = GetSystemMetrics ( SM_CYSCREEN ); 

		if ( screenX < screenY )  //判断横竖屏
		 {
			 screenStatus = VerticalScreen;
		 }
		 else
		 {
			 screenStatus = HorizontalScreen;
		 }
		MoveWindow( hwnd , 0 , 0 ,screenX , screenY , TRUE);
		moveGameWindow( windowHwnd);
		return 0;

	case WM_TIMER://检测窗口是否存在
		if (!(windowHwnd = FindWindow(windowClassName, NULL)))
			SendMessage(hwnd, WM_DESTROY, NULL, NULL);
		
		

		return 0;




	case WM_LBUTTONDOWN:
		 point.x = LOWORD(lParam) ;
		 point.y = HIWORD( lParam) ;

		if ( point.x <45 && point.y <20 ) //退出窗口
		{
			sendSinglKeyDown(VK_MENU);
			sendSinglKeyDown(VK_F4);

			sendSinglKeyUp(VK_F4);
			sendSinglKeyUp(VK_MENU);


			return 0;
		}

		/*if ( point.x> screenX-140 && point.y <20 )//ALT+F4
		{
			sendSinglKeyDown( VK_MENU);
			sendSinglKeyDown( VK_F4);

			sendSinglKeyUp( VK_F4);
			sendSinglKeyUp( VK_MENU);
			SendMessage(hwnd, WM_DESTROY, NULL, NULL);

			return 0;
		}*/

		if ( point.x >= 0 && point.x<= 40 && point.y >=30 && point.y <= 50)//ESC
		{
				sendKey( VK_ESCAPE);
				return 0;
		}

		if ( point.x >= 0 && point.x<= 45 && point.y >= screenY-40 && point.y <= screenY)//Ctrl
		{

			if ( !isWhile)
			{
				_beginthread( Thread , 0 , 0 );
				isWhile = true;
				InvalidateRect( hwnd , NULL , TRUE);
			}
			else
			{
				isWhile = false;
				InvalidateRect( hwnd , NULL , TRUE);
			}

				return 0;
		}

		if ( point.x <= 42 && point.y >= (screenY-130) && point.y<= (screenY-91) )//上滚轮
		{
			mouseWheel( UP );
			return 0;
		}

		if ( point.x <= 42 && point.y >= (screenY-85) && point.y<= (screenY-46) )//下滚轮
		{
			mouseWheel( DOWN );

			return 0;
		}

		sendKey( VK_RETURN);//默认回车



		return 0;
	
	case WM_DESTROY:
		isWhile = false;
		DeleteObject( hfont);
		Sleep(100);

		ShowWindow( taskbar, SW_SHOW); 	//恢复被隐藏的系统任务栏
		PostQuitMessage (0) ;  
				
		return 0 ;
	}
	     return DefWindowProc (hwnd, message, wParam, lParam) ;
}

bool removeWindowTitile( HWND hwnd )
{

	 SetWindowLong( hwnd , GWL_STYLE , WS_POPUP|WS_VISIBLE);


		return true;
}

bool sendKey( int key )
{

	keybd_event( key , MapVirtualKey(0x0D, MAPVK_VK_TO_VSC) , NULL , GetMessageExtraInfo());
	Sleep(20);
	keybd_event( key , MapVirtualKey(0x0D, MAPVK_VK_TO_VSC) , KEYEVENTF_KEYUP , GetMessageExtraInfo());
	Sleep(20);
	return true;
}

bool sendSinglKeyDown( int key )
{
	keybd_event( key , MapVirtualKey(0x0D, MAPVK_VK_TO_VSC) , NULL , GetMessageExtraInfo());
	//Sleep(50);
	return true;
}

bool sendSinglKeyUp( int key )
{
	keybd_event( key , MapVirtualKey(0x0D, MAPVK_VK_TO_VSC) , KEYEVENTF_KEYUP , GetMessageExtraInfo());
	Sleep(50);
	return true;
}

bool sendSinglKeyDown( int key , int time )
{
	keybd_event( key , MapVirtualKey(0x0D, MAPVK_VK_TO_VSC) , NULL , GetMessageExtraInfo());
	Sleep(time);
	return true;
}

bool sendSinglKeyUp( int key , int time )
{
	keybd_event( key , MapVirtualKey(0x0D, MAPVK_VK_TO_VSC) , KEYEVENTF_KEYUP , GetMessageExtraInfo());
	Sleep(time);
	return true;
}

bool moveGameWindow( HWND hwnd )
{
	if ( screenStatus == VerticalScreen )
	{
		MoveWindow( hwnd , 0 , 340 , 800 , 600 , TRUE);
	}
	if (screenStatus == HorizontalScreen)
	{

		MoveWindow( hwnd , 240 , 100 , 800 , 600 ,TRUE);
	}


	return true;
}

void Thread (PVOID pvoid )
{
		sendSinglKeyDown( VK_CONTROL);
		Sleep(400);
		for (; isWhile; )
		{
			sendSinglKeyDown( VK_CONTROL);
			Sleep(50);
		}

		sendSinglKeyUp( VK_CONTROL);


		
		_endthread();
}

void Thread2 (PVOID pvoid )
{
		for (; isWhile; )
		{
			sendSinglKeyDown( VK_CONTROL , 10 );
			sendSinglKeyUp( VK_CONTROL , 10 );
		}


		_endthread();
}



bool mouseWheel( int stats )
{
	if ( stats == UP)
	{

		mouse_event( MOUSEEVENTF_WHEEL , 0 , 0  , 120 , GetMessageExtraInfo());
	}
	else
	{
		mouse_event( MOUSEEVENTF_WHEEL , 0 , 0  , -120 , GetMessageExtraInfo());

	}

	return true;
}

bool InjectDLL( DWORD Pid )
 {
    LPCSTR lpDllName = "API hooker.dll"; 
    DWORD dwSize = strlen(lpDllName)+1;  
    DWORD dwHasWrite;  
	LPVOID lpRemoteBuf;	
	
	HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION|PROCESS_VM_WRITE,FALSE,Pid); 
 
	if(hProcess == NULL) 
    { 
		MessageBox( NULL , TEXT("get hProcess error!") ,  TEXT("get hProcess error!") ,MB_OK);
            return false; 
    } 

    lpRemoteBuf = VirtualAllocEx(hProcess,NULL,dwSize,MEM_COMMIT,PAGE_READWRITE); 

    if(WriteProcessMemory(hProcess,lpRemoteBuf,lpDllName,dwSize,&dwHasWrite)) 
    { 
        if(dwHasWrite != dwSize) 
        { 
            VirtualFreeEx(hProcess,lpRemoteBuf,dwSize,MEM_COMMIT); 
            CloseHandle(hProcess); 
            return false; 
        }     
    }
	else 
    { 
		MessageBox( NULL , TEXT("写入远程进程内存空间出错!") ,  TEXT("写入远程进程内存空间出错!") 

,MB_OK); 
        CloseHandle(hProcess); 
        return false; 
    } 


     
   DWORD dwNewThreadId; 
   LPVOID lpLoadDll = LoadLibraryA; 
   HANDLE hNewRemoteThread = CreateRemoteThread(hProcess,NULL,0,(LPTHREAD_START_ROUTINE)lpLoadDll,lpRemoteBuf,0,&dwNewThreadId); 

   if(hNewRemoteThread == NULL) 
   {
	    MessageBox( NULL , TEXT("建立远程线程失败!") ,  TEXT("建立远程线程失败!") ,MB_OK); 
        CloseHandle(hProcess); 
        return false; 
    } 

   MessageBox(NULL, TEXT("建立远程线程成功!"), TEXT("建立远程线程成功!"), MB_OK);

   return true;

}

BOOL CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam)  
{  
    ProcessWindow *pProcessWindow = (ProcessWindow *)lParam;  
  
    DWORD dwProcessId;  
    GetWindowThreadProcessId(hWnd, &dwProcessId);  
  
    // 判断是否是指定进程的主窗口  
    if (pProcessWindow->dwProcessId == dwProcessId && IsWindowVisible(hWnd) && GetParent(hWnd) == NULL)  
    {  
        pProcessWindow->hwndWindow = hWnd;  
  
        return FALSE;  
    }  
  
    return TRUE;  
}  