//
//	バージョン
//


//#include	"..\\main.h"

//--- 変数定義 ------------------------------------------------------
static	WNDPROC	Org_MyAboutStatic;
static	char	strText_[2][64];



/*------------------------------------------------------------------------------*
| <<< URL のコールバック >>>
*------------------------------------------------------------------------------*/
static	LRESULT	CALLBACK	MyAboutStaticProc(HWND	hWnd, UINT msg,	WPARAM wp, LPARAM lp)
{

	switch(msg)
	{
	//--- カーソル変更 ----------------------------------------------
	case WM_SETCURSOR:
		SetCursor(LoadCursor(sys.hInst,	"YUBI"));
		return 0;

	//--- IE 起動 ---------------------------------------------------
	case WM_LBUTTONDOWN:
		ShellExecute(hWnd, NULL, strText_[1], NULL, NULL, SW_SHOWNORMAL);
		return 0;

	//--- 下線、青文字にする ----------------------------------------
	case WM_PAINT:
		{
			HFONT		hFont;
			LOGFONT		lf;
			PAINTSTRUCT	ps;
			RECT		r;
			HDC			hdc	= BeginPaint(hWnd, &ps);

			memset(&lf,	0, sizeof(LOGFONT));	// 文字フォント設定
			strcpy(lf.lfFaceName , "system");
			lf.lfUnderline = TRUE;
			lf.lfHeight	   = 14;
			lf.lfWidth	   = 6;
			GetClientRect(hWnd,	&r);
			SetTextAlign(hdc, TA_CENTER);
			hFont =	CreateFontIndirect(&lf);
			SelectObject(hdc, (HGDIOBJ)hFont);
			SetBkColor(	 hdc, GetSysColor(COLOR_MENU));
			SetTextColor(hdc, RGB(0, 0,	255));
			SetBkMode(	 hdc, TRANSPARENT);
												// 文字描画
			TextOut(	 hdc, r.right /	2, 0, strText_[1], strlen(__URL));
			DeleteObject(hFont);
			EndPaint(hWnd, &ps);
			return 0;
		}
	}
	return CallWindowProc(Org_MyAboutStatic, hWnd, msg,	wp,	lp);
}



/*------------------------------------------------------------------------------*
| <<< ダイアログのイベント処理 >>>
*------------------------------------------------------------------------------*/
static	BOOL	CALLBACK	DlgProc(HWND hWnd, UINT	msg, WPARAM	wParam,	LPARAM lParam)
{
	static	HWND	hMyStatic;

	switch(msg)
	{
	case WM_INITDIALOG:
		//--- ダイアログ生成、ウィンドウの中央へ --------------------
		CTRL_WINDOW_RECT_SET(hWnd, CTRL_VER_WIDTH, CTRL_VER_HEIGHT);
			
		//--- URL のコントロールを得る ------------------------------
		hMyStatic =	GetDlgItem(hWnd, IDC_VER_URL);
		//if(!hMyStatic) MessageBox(NULL,	"Error", "OK", MB_OK);
		if(!hMyStatic){ sys_message_box("バージョンウィンドウが開けません", "OK", MB_OK);}
		Org_MyAboutStatic =	(WNDPROC)GetWindowLong(hMyStatic, GWL_WNDPROC);
		SetWindowLong(hMyStatic, GWL_WNDPROC, (LONG)MyAboutStaticProc);

		//--- メッセージ送信 ----------------------------------------
		//SetDlgItemText(hWnd, IDC_VER_MESSAGE,	strText_[1]);
		SetDlgItemText(hWnd, IDC_VER_TITLE,	strText_[0]);
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:							// OK ボタン
			SendMessage(hWnd, WM_CLOSE,	0, 0L);
			break;
		}
		break;
	case WM_CLOSE:
		//if(SetWindowLong(hMyStatic,	GWL_WNDPROC, (LONG)Org_MyAboutStatic) == 0)	MessageBox(hWnd, "Error", "OK",	MB_OK);
		if(SetWindowLong(hMyStatic,	GWL_WNDPROC, (LONG)Org_MyAboutStatic) == 0){ sys_message_box("バージョンウィンドウが閉じれません", "OK", MB_OK);}
		EndDialog(hWnd,	IDOK);
		d3.bRender = true;
		return TRUE;
	default:
		return FALSE;
	}
	return TRUE;
}



/*------------------------------------------------------------------------------*
| <<< バージョン情報オープン >>>
|	入力	strTitle = アプリ名
|			strUrl	 = ホムペのアドレス
*------------------------------------------------------------------------------*/
void	ctrl_version_dialog_open(char *strTitle, char *strUrl)
{
	strcpy(strText_[0], strTitle);
	strcpy(strText_[1], strUrl);
	DialogBox(sys.hInst, "VER",	sys.hWnd, DlgProc);
}
