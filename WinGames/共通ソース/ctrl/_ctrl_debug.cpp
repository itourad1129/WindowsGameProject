//
//	デバッグ用
//
//		※セパレーターの仕組みは、細く置いたスタティックのイベントである
//



#define	DEBUG_MONITOR_MAX	100				// モニター表示数
#define	DEBUG_MONITOR_W		400				// 横幅
#define	DEBUG_LIST_H		320				// リストビューの高さ
#define	INTERVAL_TIME		60				// 表示の更新間隔
#define	CONFIG_NAME			"debug.ini"

#define	SEPARATOR_H	4						// セパレーターのサイズ
#define	OFFSET_X	6						// リスト、エディットの余白
#define	OFFSET_Y	2


typedef	struct
{
	HWND	hwnd;
	BOOL	bEnd;
}PARAM_, *PPARAM_;

typedef	struct
{
	uchar	ucType;							// 0 : オフ
											// 1 : 文字
											// 2 : char
											// 3 : uchar
											// 4 : short
											// 5 : ushort
											// 6 : int
											// 7 : uint
											// 8 : float
											// 9 : double
	bool	bHex;							// true : 16 進数で表示
	void	*p;								// データの格納先
}DEBUG_WORK_;


static	DEBUG_WORK_	debug[DEBUG_MONITOR_MAX];


static	HWND	hDlgWnd;
static	HWND	hListView;					// 表示用のリストビュー
static	HWND	hEdit;						// エディットボックス
static	HWND	hSeparator;					// サイズ変更用のセパレーター
static	WNDPROC	procSeparator;				// セパレーターのイベント
static	int		iDebugMonitorCount;
static	PARAM_	data;
											// リストビューの高さ(この値で他のサイズも変更される)
static	int		iListViewH	  = DEBUG_LIST_H;
static	float	fListEditRate = 0.5f;
static	int		iSeparatorY;				// セパレーターのＹ座標
static	bool	bDrag;						// セパレーターをドラッグするのに使用
static	RECT	rectDebug;					// デバッグウィンドウのサイズ

//--- 関数宣言 ------------------------------------------------------
static	void	ConfigSave(void);
static	bool	ConfigLoad(void);
static	void	DebugMonitor(void);
static	void	DebugResize(float fRate);
static	BOOL	CALLBACK	DebugProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
static	LRESULT	CALLBACK	SeparatorProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);



/*------------------------------------------------------------------------------*
| <<< 諸情報をセーブ >>>
*------------------------------------------------------------------------------*/
static	void	ConfigSave(void)
{

	HANDLE	handle;						// 読み込みハンドル
	DWORD	wr;							// ダミー
	char	str[256];

	GetWindowRect(hDlgWnd, &rectDebug);
	FOLDER_PUSH();
	FOLDER_SET();						// ディレクトリをＥＸＥ位置に
	remove(CONFIG_NAME);
	handle = CreateFile(CONFIG_NAME, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	FOLDER_POP();
	if(handle == INVALID_HANDLE_VALUE) return;

	sprintf(str, "%04d,%04d,%04d,%04d,%.4f\r\n",
			rectDebug.top,	  rectDebug.left,		// 右上
			rectDebug.bottom, rectDebug.right,		// 左下
			fListEditRate
			);
	WriteFile(handle, str, strlen(str),	&wr, NULL);
	CloseHandle(handle);
}



/*------------------------------------------------------------------------------*
| <<< システムファイルをロード >>>	※システムファイルにはウィンドウ位置が保存されている
*------------------------------------------------------------------------------*/
static	bool	ConfigLoad(void)
{

	HANDLE	handle;						// 読み込みハンドル
	DWORD	rs;							// ダミー
	int		size;
	char	str[256];

	FOLDER_PUSH();
	FOLDER_SET();
	handle = CreateFile(CONFIG_NAME, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	FOLDER_POP();
	if(handle == INVALID_HANDLE_VALUE)			return false;
	if((size = GetFileSize(handle, NULL)) == 0)	return false;
	memset(str, 0, sizeof(str));
	ReadFile(handle, str, size,	&rs, NULL);
	CloseHandle(handle);
	sscanf(str,	"%04d,%04d,%04d,%04d,%f\r\n",
				&rectDebug.top,	   &rectDebug.left,		// 右上
				&rectDebug.bottom, &rectDebug.right,	// 左下
				&fListEditRate
				);

	//--- 画面外のウィンドウを補正する ------------------------------
	if(rectDebug.left  < 0) rectDebug.right  -= rectDebug.left, rectDebug.left = 0;
	if(rectDebug.top   < 0) rectDebug.bottom -= rectDebug.top,  rectDebug.top = 0;
	if(rectDebug.right > SYS_DESKTOP_XSIZE())
	{
		int	a =	rectDebug.right - SYS_DESKTOP_XSIZE();
		if((rectDebug.left -= a) < 0) rectDebug.left = 0;
		rectDebug.right = SYS_DESKTOP_XSIZE();
	}
	int	y =	SYS_DESKTOP_YSIZE() + GetSystemMetrics(SM_CYCAPTION);
	if(rectDebug.bottom > y)
	{
		int	a =	rectDebug.bottom  - y;
		if((rectDebug.top -= a) < 0) rectDebug.top = 0;
		rectDebug.bottom = y;
	}
	return true;
}



/*------------------------------------------------------------------------------*
| <<< 変数モニター描画 >>>
*------------------------------------------------------------------------------*/
static	void	DebugMonitor(void)
{

	DEBUG_WORK_	*d = debug;

	for(int i = 0; i < iDebugMonitorCount; i++, d++)
	{
		switch(d->ucType)
		{
		//--- str ---------------------------------------------------
		case 1:
			ctrl_list_view_str_set(hListView, 1, i, (char*)d->p);
			break;
		//--- char --------------------------------------------------
		case 2:
			if(!d->bHex) ctrl_list_view_str_set(hListView, 1, i, "(char)     %d", *(char*)d->p);
			else		 ctrl_list_view_str_set(hListView, 1, i, "(char)   0x%x", *(char*)d->p);
			break;
		//--- uchar -------------------------------------------------
		case 3:
			if(!d->bHex) ctrl_list_view_str_set(hListView, 1, i, "(uchar)    %d", *(uchar*)d->p);
			else		 ctrl_list_view_str_set(hListView, 1, i, "(uchar)  0x%x", *(uchar*)d->p);
			break;
		//--- short -------------------------------------------------
		case 4:
			if(!d->bHex) ctrl_list_view_str_set(hListView, 1, i, "(short)    %d", *(short*)d->p);
			else		 ctrl_list_view_str_set(hListView, 1, i, "(short)  0x%x", *(short*)d->p);
			break;
		//--- ushort ------------------------------------------------
		case 5:
			if(!d->bHex) ctrl_list_view_str_set(hListView, 1, i, "(ushort)   %d", *(ushort*)d->p);
			else		 ctrl_list_view_str_set(hListView, 1, i, "(ushort) 0x%x", *(ushort*)d->p);
			break;
		//--- int ---------------------------------------------------
		case 6:
			if(!d->bHex) ctrl_list_view_str_set(hListView, 1, i, "(int)      %d", *(int*)d->p);
			else		 ctrl_list_view_str_set(hListView, 1, i, "(int)    0x%x", *(int*)d->p);
			break;
		//--- uint --------------------------------------------------
		case 7:
			if(!d->bHex) ctrl_list_view_str_set(hListView, 1, i, "(uint)     %d", *(uint*)d->p);
			else		 ctrl_list_view_str_set(hListView, 1, i, "(uint)   0x%x", *(uint*)d->p);
			break;
		//--- float -------------------------------------------------
		case 8:
			if(!d->bHex) ctrl_list_view_str_set(hListView, 1, i, "(float)    %f", *(float*)d->p);
			break;
		//--- double ------------------------------------------------
		case 9:
			if(!d->bHex) ctrl_list_view_str_set(hListView, 1, i, "(double)   %l", *(double*)d->p);
			break;
		}
	}
}



/*------------------------------------------------------------------------------*
| <<< デバッグダイアログプロシージャ >>>
*------------------------------------------------------------------------------*/
static	BOOL	CALLBACK	DebugProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

	switch(iMsg)
	{
	//--- リサイズ ------------------------------------------------------------
	case WM_SIZE:
		DebugResize(fListEditRate);
//		SetActiveWindow(sys.hWnd);
		break;
	//--- 最大時、最小時の制限 ------------------------------------------------
	case WM_GETMINMAXINFO:
		{
			int	x = SYS_XFRAME_GET();
			int	y = SYS_YFRAME_GET();
			if(x >= 0 && sys.cWindowStyle == SIZE_RESTORED)
			{
				((MINMAXINFO*)lParam)->ptMinTrackSize.x = 300 + x;
				((MINMAXINFO*)lParam)->ptMinTrackSize.y = 300 + y;
			}
		}
		break;
	//--- 変数監視のためのタイマーイベント ------------------------------------
	case WM_TIMER:
		DebugMonitor();
//		if(b1st) SetActiveWindow(sys.hWnd), b1st = false;
		break;
	//--- ダイアログボックスが閉じられるとき ----------------------------------
	case WM_CLOSE:
		ConfigSave();
		DestroyWindow(hDlgWnd);
		return TRUE;
	}
	return FALSE;  // DefWindowProc()ではなく、FALSEを返すこと！
}



/*------------------------------------------------------------------------------*
| <<< デバッグのレイアウト変更 >>>
|	入力	fRate = リストビューとエディットボックスの比率
|	※　iListViewH の値で全て決まります
*------------------------------------------------------------------------------*/
static	void	DebugResize(float fRate)
{

	RECT	r;

	GetWindowRect(hDlgWnd, &r);
											// フレームのサイズを引く
	r.top    += (GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYEDGE)) + GetSystemMetrics(SM_CYCAPTION);
	r.bottom -= (GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYEDGE));
	r.right  -= (GetSystemMetrics(SM_CXFIXEDFRAME) + GetSystemMetrics(SM_CXEDGE));
	r.left   += (GetSystemMetrics(SM_CXFIXEDFRAME) + GetSystemMetrics(SM_CXEDGE));

	r.right  -= r.left;						// bottom、right はサイズへ
	r.bottom -= r.top;

	//--- リストビュー --------------------------------------
	int	y = OFFSET_Y;						// 少し下へ下げる
	int	h = (int)((float)r.bottom * fRate) - SEPARATOR_H / 2 - OFFSET_Y;

	CTRL_RESIZE(hListView, OFFSET_X, y, r.right - OFFSET_X, h);
	
	//--- エディット ----------------------------------------
	y = (int)((float)r.bottom * fRate) + SEPARATOR_H;
	h = (int)((float)r.bottom * (1.0f - fRate)) - SEPARATOR_H / 2 - OFFSET_Y;
	CTRL_RESIZE(hEdit,     OFFSET_X, y, r.right - OFFSET_X, y + h);
	
	//--- セパレーター --------------------------------------
	h		    = SEPARATOR_H;
	y		    = (int)((float)r.bottom * fRate) - SEPARATOR_H / 2;
	iSeparatorY = y;
	CTRL_RESIZE(hSeparator, 0, y, r.right, y + h);
}



/*------------------------------------------------------------------------------*
| <<< セパレーターのプロシージャ >>>
*------------------------------------------------------------------------------*/
static	LRESULT	CALLBACK	SeparatorProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{

	switch(uiMsg)
	{
	//--- カーソル変更 ----------------------------------------------
	case WM_SETCURSOR:
		SetCursor(LoadCursor(NULL, IDC_SIZENS));
		return 0;

	//--- マウス処理 ------------------------------------------------
	// ここでドラックさせている
	// iSeparatorY の値が不正確なため、多少のズレを生じている
	case WM_LBUTTONDOWN:
		bDrag = true;
		SetCapture(hWnd);					// ウィンドウ外でもマウスの値が拾えるようにキャプチャー
		break;

	case WM_MOUSEMOVE:
		if(bDrag)
		{
			POINT	a;						// マウスのスクリーン座標
			GetCursorPos(&a);
			
			RECT	r;
			GetWindowRect(hDlgWnd, &r);
											// iY にデバックウィインドーの描画領域 Y 座標
			int	iTop    = r.top    +  GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYEDGE) + GetSystemMetrics(SM_CYCAPTION);
			int	iBottom = r.bottom - (GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYEDGE));

			if(a.y < iTop    + 100) break;	// 上限
			if(a.y > iBottom - 100) break;	// 下限
											// レート
			fListEditRate = (float)(a.y - iTop) / (float)(iBottom - iTop);
			DebugResize(fListEditRate);
		}
		break;

	case WM_LBUTTONUP:
		bDrag = false;
		ReleaseCapture();							// キャプチャーしたマウスを開放
		break;

	
	}
	return 	CallWindowProc(procSeparator, hWnd, uiMsg, wParam, lParam);
}



/*------------------------------------------------------------------------------*
| <<< デバッグダイアログオープン >>>
*------------------------------------------------------------------------------*/
void	ctrl_debug_dialog_open(void)
{

	if(hDlgWnd != NULL) return;
	hDlgWnd = CreateDialog(sys.hInst, "DEBUG", sys.hWnd, DebugProc);

	//--- ウィンドウの形を設定する ----------------------------------
	if(ConfigLoad())
	{
		MoveWindow(hDlgWnd, rectDebug.left, rectDebug.top, rectDebug.right - rectDebug.left, rectDebug.bottom - rectDebug.top, TRUE); 
	}
	else
	{
		RECT	r;
		GetWindowRect(sys.hWnd,	&r);
		MoveWindow(hDlgWnd, r.right, r.top, DEBUG_MONITOR_W, r.bottom - r.top, TRUE);
	}

	ShowWindow(  hDlgWnd, SW_SHOW);
	UpdateWindow(hDlgWnd);
											// リストビュー
	hListView     = ctrl_list_view_create(hDlgWnd, false, false);
											// メッセージ表示用エディットボックス
	hEdit         = ctrl_edit_create(hDlgWnd, WS_VSCROLL |  ES_MULTILINE | WS_HSCROLL | ES_READONLY | ES_AUTOVSCROLL);
			 								// セパレーター
	hSeparator	  = ctrl_static_text_create(hDlgWnd, NULL, "");
	procSeparator = CTRL_PROC_CHANGE(hSeparator, SeparatorProc);
											// 項目の追加
	ctrl_list_view_column_add(hListView, 0, "変数名", 150);
	ctrl_list_view_column_add(hListView, 1, "値",     232);

	//--- 監視用のタイマー起動 --------------------------------------
	// タイマーを使い、再度メインウィンドウをアクティブにすることで、パットが正常に動く
	SetTimer(hDlgWnd, ID_MYTIMER, INTERVAL_TIME, NULL);

	SendMessage(hDlgWnd, WM_SIZE, 0, 0L);

//	b1st = true;
}



/*------------------------------------------------------------------------------*
| <<< デバッグダイアログ閉じる >>>
*------------------------------------------------------------------------------*/
void	ctrl_debug_dialog_close(void)
{
	ConfigSave();
	SendMessage(hDlgWnd, WM_CLOSE, 0, 0L);
}



/*------------------------------------------------------------------------------*
| <<< デバッグダイアログ、変数モニター設定 >>>	※文字列
*------------------------------------------------------------------------------*/
void	ctrl_debug_str_monitor(char *strVar, char *str)
{
	ctrl_debug_dialog_open();
	if(hDlgWnd != NULL && hListView != NULL && iDebugMonitorCount < DEBUG_MONITOR_MAX)
	{
		int	iID = iDebugMonitorCount;
		debug[iID].ucType = 1;
		debug[iID].p	  = (void*)str;
		ctrl_list_view_add(hListView, 0, iID, strVar);
		ctrl_list_view_add(hListView, 1, iID, str);
		iDebugMonitorCount++;
	}
}



/*------------------------------------------------------------------------------*
| <<< デバッグダイアログ、変数モニター設定 >>>	※ char 型
*------------------------------------------------------------------------------*/
void	ctrl_debug_monitor(char *strVar, char *cVar, bool bHex)
{
	ctrl_debug_dialog_open();
	if(hDlgWnd != NULL && hListView != NULL && iDebugMonitorCount < DEBUG_MONITOR_MAX)
	{
		int	iID = iDebugMonitorCount;
		debug[iID].ucType = 2;
		debug[iID].p	  = (void*)cVar;
		debug[iID].bHex	  = bHex;
		ctrl_list_view_add(hListView, 0, iID, strVar);
		ctrl_list_view_add(hListView, 1, iID, "wait...");
		iDebugMonitorCount++;
	}
}

void	ctrl_debug_monitor(char *strVar, char *cVar)
{
	ctrl_debug_monitor(strVar, cVar, false);
}



/*------------------------------------------------------------------------------*
| <<< デバッグダイアログ、変数モニター設定 >>>	※ uchar 型
*------------------------------------------------------------------------------*/
void	ctrl_debug_monitor(char *strVar, uchar *ucVar, bool bHex)
{
	ctrl_debug_dialog_open();
	if(hDlgWnd != NULL && hListView != NULL && iDebugMonitorCount < DEBUG_MONITOR_MAX)
	{
		int	iID = iDebugMonitorCount;
		debug[iID].ucType = 3;
		debug[iID].p	  = (void*)ucVar;
		debug[iID].bHex	  = bHex;
		ctrl_list_view_add(hListView, 0, iID, strVar);
		ctrl_list_view_add(hListView, 1, iID, "wait...");
		iDebugMonitorCount++;
	}
}

void	ctrl_debug_monitor(char *strVar, uchar *ucVar)
{
	ctrl_debug_monitor(strVar, ucVar, false);
}



/*------------------------------------------------------------------------------*
| <<< デバッグダイアログ、変数モニター設定 >>>	※ short 型
*------------------------------------------------------------------------------*/
void	ctrl_debug_monitor(char *strVar, short *sVar, bool bHex)
{
	ctrl_debug_dialog_open();
	if(hDlgWnd != NULL && hListView != NULL && iDebugMonitorCount < DEBUG_MONITOR_MAX)
	{
		int	iID = iDebugMonitorCount;
		debug[iID].ucType = 4;
		debug[iID].p	  = (void*)sVar;
		debug[iID].bHex	  = bHex;
		ctrl_list_view_add(hListView, 0, iID, strVar);
		ctrl_list_view_add(hListView, 1, iID, "wait...");
		iDebugMonitorCount++;
	}
}

void	ctrl_debug_monitor(char *strVar, short *sVar)
{
	ctrl_debug_monitor(strVar, sVar, false);
}

	
	
/*------------------------------------------------------------------------------*
| <<< デバッグダイアログ、変数モニター設定 >>>	※ ushort 型
*------------------------------------------------------------------------------*/
void	ctrl_debug_monitor(char *strVar, ushort *usVar, bool bHex)
{
	ctrl_debug_dialog_open();
	if(hDlgWnd != NULL && hListView != NULL && iDebugMonitorCount < DEBUG_MONITOR_MAX)
	{
		int	iID = iDebugMonitorCount;
		debug[iID].ucType = 5;
		debug[iID].p	  = (void*)usVar;
		debug[iID].bHex	  = bHex;
		ctrl_list_view_add(hListView, 0, iID, strVar);
		ctrl_list_view_add(hListView, 1, iID, "wait...");
		iDebugMonitorCount++;
	}
}

void	ctrl_debug_monitor(char *strVar, ushort *usVar)
{
	ctrl_debug_monitor(strVar, usVar, false);
}



/*------------------------------------------------------------------------------*
| <<< デバッグダイアログ、変数モニター設定 >>>	※ int 型
*------------------------------------------------------------------------------*/
void	ctrl_debug_monitor(char *strVar, int *iVar, bool bHex)
{
	ctrl_debug_dialog_open();
	if(hDlgWnd != NULL && hListView != NULL && iDebugMonitorCount < DEBUG_MONITOR_MAX)
	{
		int	iID = iDebugMonitorCount;
		debug[iID].ucType = 6;
		debug[iID].p	  = (void*)iVar;
		debug[iID].bHex	  = bHex;
		ctrl_list_view_add(hListView, 0, iID, strVar);
		ctrl_list_view_add(hListView, 1, iID, "wait...");
		iDebugMonitorCount++;
	}
}

void	ctrl_debug_monitor(char *strVar, int *iVar)
{
	ctrl_debug_monitor(strVar, iVar, false);
}



/*------------------------------------------------------------------------------*
| <<< デバッグダイアログ、変数モニター設定 >>>	※ uint 型
*------------------------------------------------------------------------------*/
void	ctrl_debug_monitor(char *strVar, uint *uiVar, bool bHex)
{
	ctrl_debug_dialog_open();
	if(hDlgWnd != NULL && hListView != NULL && iDebugMonitorCount < DEBUG_MONITOR_MAX)
	{
		int	iID = iDebugMonitorCount;
		debug[iID].ucType = 7;
		debug[iID].p	  = (void*)uiVar;
		debug[iID].bHex	  = bHex;
		ctrl_list_view_add(hListView, 0, iID, strVar);
		ctrl_list_view_add(hListView, 1, iID, "wait...");
		iDebugMonitorCount++;
	}
}

void	ctrl_debug_monitor(char *strVar, uint *uiVar)
{
	ctrl_debug_monitor(strVar, uiVar, false);
}



/*------------------------------------------------------------------------------*
| <<< デバッグダイアログ、変数モニター設定 >>>	※ float 型
*------------------------------------------------------------------------------*/
void	ctrl_debug_monitor(char *strVar, float *fVar)
{
	ctrl_debug_dialog_open();
	if(hDlgWnd != NULL && hListView != NULL && iDebugMonitorCount < DEBUG_MONITOR_MAX)
	{
		int	iID = iDebugMonitorCount;
		debug[iID].ucType = 7;
		debug[iID].p	  = (void*)fVar;
		ctrl_list_view_add(hListView, 0, iID, strVar);
		ctrl_list_view_add(hListView, 1, iID, "wait...");
		iDebugMonitorCount++;
	}
}



/*------------------------------------------------------------------------------*
| <<< デバッグダイアログ、変数モニター設定 >>>	※ double 型
*------------------------------------------------------------------------------*/
void	ctrl_debug_monitor(char *strVar, double *dVar)
{
	ctrl_debug_dialog_open();
	if(hDlgWnd != NULL && hListView != NULL && iDebugMonitorCount < DEBUG_MONITOR_MAX)
	{
		int	iID = iDebugMonitorCount;
		debug[iID].ucType = 8;
		debug[iID].p	  = (void*)dVar;
		ctrl_list_view_add(hListView, 0, iID, strVar);
		ctrl_list_view_add(hListView, 1, iID, "wait...");
		iDebugMonitorCount++;
	}
}



/*------------------------------------------------------------------------------*
| <<< デバッグダイアログ、エディットボックスに文字列を送る >>>
*------------------------------------------------------------------------------*/
void	ctrl_debug_monitor_printf(char* Format,...)
{
	ctrl_debug_dialog_open();
	if(hDlgWnd != NULL && hEdit != NULL)
	{
		char	strBuf[512];

		va_list	ap;
		va_start(ap,	 Format);
		vsprintf(strBuf, Format, ap);
		va_end(ap);
		ctrl_edit_printf(hEdit, strBuf);
	}
}