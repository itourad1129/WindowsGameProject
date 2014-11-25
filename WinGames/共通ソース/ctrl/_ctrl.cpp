//
//	コントロール関係のプログラム
//

/*
	仕組みとしては　CTRL_MOTHER	が CTRL_WORK のメモリを確保、
	ダイアログを作成する。
	ダイアログはリソースを使い、リソースには各コントロールを
	貼り付けたり、プログラムで作成したりする。
*/

//	[[[ フォント ]]]
//	[[[ マザー ]]]
//	[[[ デバック用エディットボックス ]]]
//	[[[ プッシュボタン ]]]
//	[[[ エディットボックス ]]]
//	[[[ スタティックテキスト ]]]
//	[[[ スタティックピクチャー ]]]
//	[[[ コンボボックス ]]]
//	[[[ グループボックス ]]]
//	[[[ ツールチップ ]]]
//	[[[ タブコントロール ]]]
//	[[[ スライダー ]]]
//	[[[ リストビュー ]]]
//	[[[ チェックボックス ]]]
//	[[[ フォルダー選択 ]]]
//	[[[ スクロール ]]]
//	[[[ ラジオボタン ]]]
//	[[[ セパレーター ]]]



/*************************************************************************************************
	[[[ フォント ]]]
**************************************************************************************************/
static	HFONT	hFontDefault = NULL;

/*------------------------------------------------------------------------------*
| <<< 各コントロールのデフォルトフォント設定 >>>
*------------------------------------------------------------------------------*/
void	ctrl_font_create(HFONT hFont)
{
	hFontDefault = hFont;
}

HFONT	ctrl_font_create(int iSize, char *strFont)
{

	HFONT	hFont = CreateFont(iSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, strFont);
	hFontDefault  = hFont;
	return	hFont;
}



/*------------------------------------------------------------------------------*
| <<< フォント解放 >>>
*------------------------------------------------------------------------------*/
void	ctrl_font_free(HFONT hFont)
{
	if(hFont != NULL)
	{
		DeleteObject(hFont);
		hFontDefault = NULL;
	}
}



/*************************************************************************************************
	[[[ マザー ]]]
**************************************************************************************************/


/*------------------------------------------------------------------------------*
| <<< 特定のダイアログを作成 >>>
|	入力	hWnd		= 親のウィンドウハンドル
|			pProc       = 割り込み関数名
|			strResource = ダイアログリソース
*------------------------------------------------------------------------------*/
HWND	ctrl_create(HWND hMother, void *pProc, char *strResource)
{
	return CreateDialog(sys.hInst, strResource, hMother, (DLGPROC)pProc);
}


/*------------------------------------------------------------------------------*
| <<< 特定のダイアログを作成 >>>
|	入力	pProc       = 割り込み関数名
|			strResource = ダイアログリソース
*------------------------------------------------------------------------------*/
HWND	ctrl_create(void *pProc, char *strResource)
{
	return CreateDialog(sys.hInst, strResource, sys.hWnd, (DLGPROC)pProc);
}


/*------------------------------------------------------------------------------*
| <<< コントロールワーク確保 >>>		※こちらは何も無いダイアログを作成します。
|	入力	pProc	  =	イベント関数名
|	※メモリ確保します。ctrl_free で開放のこと
*------------------------------------------------------------------------------*/
HWND	ctrl_create(void *pProc)
{

	//CTRL_WORK	*w	  = (CTRL_WORK*)malloc(sizeof(CTRL_WORK));

	//memset(w, 0, sizeof(CTRL_WORK));
	return CreateDialog(sys.hInst, "DIALOG", sys.hWnd, (DLGPROC)pProc);
	//ShowWindow(w->hWnd,	SW_HIDE);									// 非表示で作成されます。
	//if(hFontDefault != NULL) CTRL_FONT_SET(w->hWnd, hFontDefault);	// フォント設定
	//return w;
}



/*------------------------------------------------------------------------------*
| <<< コントロールワーク開放 >>>
*------------------------------------------------------------------------------*/
void	ctrl_free(CTRL_MOTHER *w)
{

	//--- 各コントロールを閉じる ------------------------------------
	for(int i = 0; i < CTRL_CONTROL_MAX; i++)
	{
		if(w->hCtrl[i] != NULL)
		{
			DestroyWindow(w->hCtrl[i]);
			w->hCtrl[i] = NULL;
		}
		if(w->hTips[i] != NULL)
		{
			DestroyWindow(w->hTips[i]);
			w->hTips[i] = NULL;
		}
	}
	//--- 母体を消去 ------------------------------------------------
	if(w->hMother != NULL)
	{
		w->hMother = NULL;
		DestroyWindow(w->hMother);
	}
}



/*************************************************************************************************
	[[[ デバック用エディットボックス ]]]	※ ctrl_create を呼び出すことなく使用できます。
**************************************************************************************************/
static	HFONT	hDebugFont;				// デバック用フォント
static	HWND	hDebugEdit;				// デバック用のエディットボックス

void	ctrl_debug_create(void)
{
	hDebugEdit = CreateWindow("EDIT",						//エデイットコントロール
							"",								// 最初に表示される文字列
							WS_CHILD | WS_VISIBLE| WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | ES_WANTRETURN,
							0, 0, 0, 0,						// 表示位置、幅、高さ
							sys.hWnd,						// 親ウィンドウのハンドル
							(HMENU)1,						// エジットコントロールのＩＤ
							sys.hInst,						// インスタンス・ハンドル
							NULL);							// 通常ＮＵＬＬです

	hDebugFont = CreateFont(12, 0, 0, 0,
							FW_NORMAL, FALSE, FALSE, 0, SHIFTJIS_CHARSET,
							OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "ＭＳ ゴシック");

	SendMessage(hDebugEdit, WM_SETFONT, (WPARAM)hDebugFont, (LPARAM)MAKELPARAM(FALSE, 0));
}

void	ctrl_debug_free(void)
{
	DeleteObject( hDebugFont);
	DestroyWindow(hDebugEdit);
}

void	ctrl_debug_view(bool bSw)
{
	if(bSw) ShowWindow(hDebugEdit, SW_SHOW);
	else	ShowWindow(hDebugEdit, SW_HIDE);
}

void	ctrl_debug_resize(int iX0, int iY0, int iX1, int iY1)
{
	MoveWindow(hDebugEdit, iX0, iY0, iX1 - iX0, iY1 - iY0, TRUE);
}



/*------------------------------------------------------------------------------*
| <<< メッセージ表示用 >>>				※文字列は256文字以内のこと
*------------------------------------------------------------------------------*/
#define	__EDIT_SIZE	30000
void	ctrl_debug_printf(char* Format,...)
{

    char	strBuf[512];

	va_list	ap;
	va_start(ap,	 Format);
	vsprintf(strBuf, Format, ap);
	va_end(ap);

	//--- 末尾に改行がない場合はつける ------------------------------
	int	a = strlen(strBuf);
	if(strncmp(&strBuf[a - 2], "\r\n", 2) != 0) strcat(strBuf, "\r\n");

	//--- エディットボックスに文字を転送 ----------------------------
	char	*strPool = (char*)malloc(__EDIT_SIZE + 512);
	char	*strText = strPool;
	GetWindowText(hDebugEdit, strText, __EDIT_SIZE);
	strcat(strText, strBuf);

	a = strlen(strText);					// 文字のオーバーチェック
	while(strlen(strText) >= __EDIT_SIZE)
	{
		strText = strstr(strText, "\r\n") + 2;
	}

	SetWindowText(hDebugEdit, strText);
	free(strPool);
	
	//--- 最下行へスクロール ----------------------------------------
	a = SendMessage(hDebugEdit, EM_GETLINECOUNT, 0, 0);
	SendMessage(    hDebugEdit, EM_LINESCROLL, 0, a - 1);
}



/*************************************************************************************************
	[[[ プッシュボタン ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< プッシュボタンの作成 >>>
|	入力	hWnd	 = ウィンドゥハンドル
|			menuID   = メニューのID
|			str		 = 表示する文字列
|	戻り値	ボタンのハンドル
*------------------------------------------------------------------------------*/
HWND	ctrl_push_button_create(HWND hWnd, uint menuID, char* Format,...)
{
	HWND	hwnd;
	char	buf[256];

	vsprintf(buf, Format, (char	*)(&Format + 1));

	hwnd = CreateWindow("BUTTON", buf, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
					0, 0, 0, 0, hWnd, (HMENU)menuID, sys.hInst, NULL);
	if(hFontDefault != NULL) CTRL_FONT_SET(hwnd, hFontDefault);	// フォント設定
	return	hwnd;
}



/*------------------------------------------------------------------------------*
| <<< プッシュボタンのメッセージ変更 >>>
|	入力	hWnd	 = ウィンドゥハンドル
|			str		 = 表示する文字列
*------------------------------------------------------------------------------*/
void	ctrl_push_button_printf(HWND hWnd, char* Format,...)
{
	char	buf[256];

	vsprintf(buf, Format, (char	*)(&Format + 1));
	SetWindowText(hWnd, buf);
}



/*************************************************************************************************
	[[[ エディットボックス ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< エディットボックスの作成 >>>
|	入力	hWnd   = ウィンドゥハンドル
|			dwFlag = フラグ
|			menuID = メニューのID
|	戻り値	エディットのハンドル
*------------------------------------------------------------------------------*/
HWND	ctrl_edit_create(HWND hWnd, DWORD dwFlag, HMENU hMenu)
{
	HWND		hOut;
	HINSTANCE	hInst;

	hInst =	(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

	hOut  = CreateWindow("EDIT",				//エデイットコントロール
				"",								// 最初に表示される文字列
				WS_CHILD | WS_VISIBLE | /*WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | /*ES_READONLY |*/ ES_WANTRETURN | WS_BORDER | dwFlag,
				0, 0, 0, 0,						// 表示位置、幅、高さ
				hWnd,							// 親ウィンドウのハンドル
				hMenu,							// エディットコントロールのＩＤ
				hInst,							// インスタンス・ハンドル
				NULL);							//通常ＮＵＬＬです

	if(hFontDefault != NULL){ CTRL_FONT_SET(hOut, hFontDefault);}	// フォント設定
	return hOut;
}



/*------------------------------------------------------------------------------*
| <<< エディットボックスの作成 >>>
|	入力	hWnd   = ウィンドゥハンドル
|			dwFlag = フラグ
|	戻り値	エディットのハンドル
*------------------------------------------------------------------------------*/
HWND	ctrl_edit_create(HWND hWnd, DWORD dwFlag)
{
	return ctrl_edit_create(hWnd, dwFlag, (HMENU)1);
}



/*------------------------------------------------------------------------------*
| <<< メッセージ表示用 >>>				※文字列は256文字以内のこと
|	入力	hWnd	 = ハンドル
|			Format	 = printfと同じ
*------------------------------------------------------------------------------*/
void	ctrl_edit_printf(HWND hWnd, char* Format,...)
{

    char	strBuf[512];

	va_list	ap;
	va_start(ap,	 Format);
	vsprintf(strBuf, Format, ap);
	va_end(ap);

	//--- 末尾に改行がない場合はつける ------------------------------
	int	a = strlen(strBuf);
	if(strncmp(&strBuf[a - 2], "\r\n", 2) != 0){ strcat(strBuf, "\r\n");}

	//--- エディットボックスに文字を転送 ----------------------------
	char	*strPool = (char*)malloc(__EDIT_SIZE + 512);
	char	*strText = strPool;
	GetWindowText(hWnd, strText, __EDIT_SIZE);
	strcat(strText, strBuf);

	a = strlen(strText);					// 文字のオーバーチェック
	while(strlen(strText) >= __EDIT_SIZE)
	{
		strText = strstr(strText, "\r\n") + 2;
	}
	SetWindowText(hWnd, strText);
	free(strPool);

	//--- 最下行へスクロール ----------------------------------------
//	a = SendMessage(hDebugEdit, EM_GETLINECOUNT, 0, 0);
//	SendMessage(hDebugEdit, EM_LINESCROLL, 0, a - 1);
	a = SendMessage(hWnd, EM_GETLINECOUNT, 0, 0);
	SendMessage(    hWnd, EM_LINESCROLL,   0, a - 1);
}



/*------------------------------------------------------------------------------*
| <<< メッセージを得る >>>
|	入力	hWnd	 = ハンドル
|			*strOut	 = printfと同じ
*------------------------------------------------------------------------------*/
void	ctrl_edit_get(HWND hWnd, char *strOut)
{
	GetWindowText(hWnd, strOut, __EDIT_SIZE);
}



/*************************************************************************************************
	[[[ スタティックテキスト ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< スタティックテキストの作成 >>>
|	入力	hWnd	 = ウィンドゥハンドル
|			menuID   = メニューのID
|			x,y, w,h = ボタンの位置とサイズ
|			dwFlag	 = ウィンドウフラグ(くぼみなどはここでつけてください)
|			str		 = 表示する文字列
|	戻り値	ボタンのハンドル
*------------------------------------------------------------------------------*/
HWND	ctrl_static_text_create(HWND hWnd, uint menuID, DWORD dwFlag, char* Format,...)
{
	if(hWnd	== NULL) return NULL;

	HWND	hwnd;
	char	buf[256];

	vsprintf(buf, Format, (char	*)(&Format + 1));
																// ツールチップで使う通知
																//       ↓			くぼみ
	hwnd = CreateWindow(TEXT("STATIC"), buf, WS_CHILD | WS_VISIBLE | SS_NOTIFY /*| SS_SUNKEN*/ | dwFlag,
					0, 0, 0, 0, hWnd, (HMENU)menuID, sys.hInst, NULL);

	if(hFontDefault != NULL) CTRL_FONT_SET(hwnd, hFontDefault);	// フォント設定
	return	hwnd;
}



/*------------------------------------------------------------------------------*
| <<< スタティックテキストの作成 >>>		※ツールチップを表示するときは上の関数を使用のこと
|	入力	hWnd	 = ハンドル
|			x,y, w,h = ボタンの位置とサイズ
|			dwFlag	 = ウィンドウフラグ
|			str		 = 表示する文字列
|	戻り値	ボタンのハンドル
*------------------------------------------------------------------------------*/
HWND	ctrl_static_text_create(HWND hWnd, DWORD dwFlag, char* Format,...)
{
	char	buf[256];

	vsprintf(buf, Format, (char	*)(&Format + 1));
	HWND	hwnd;
																// ツールチップで使う通知
																//       ↓			くぼみ
	hwnd = CreateWindow(TEXT("STATIC"), buf, WS_CHILD | WS_VISIBLE | SS_NOTIFY /*| SS_SUNKEN*/ | dwFlag,
					0, 0, 10, 10, hWnd, (HMENU)1, sys.hInst, NULL);
	if(hFontDefault != NULL) CTRL_FONT_SET(hwnd, hFontDefault);	// フォント設定
	return	hwnd;
}



/*------------------------------------------------------------------------------*
| <<< スタティックテキストにメッセージを送る >>>
|	入力	hWnd	= ウィンドゥハンドル
|			Format	= 文字列
*------------------------------------------------------------------------------*/
void	ctrl_static_text_printf(HWND hWnd, char* Format,...)
{
	if(hWnd	== NULL){ return;}
	char	buf[512];
	vsprintf(buf, Format, (char	*)(&Format + 1));
	if(strlen(buf) == 0){ memset(buf, ' ', sizeof(buf)),	buf[511] = NULL;}
	SetWindowText(hWnd, buf);
}



/*************************************************************************************************
	[[[ スタティックピクチャー ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< ピクチャーボックスの作成 >>>
|	入力	hWnd   = マザーとなるハンドル
|			menuID = メニューのID
|	※ 大きさは	CTRL_RESIZE	で設定のこと
|	   スタティックのイベントは STN_CLICKED と STN_DBLCLK しか通知されないので注意。
|								マウスクリックとダブルクリック
*------------------------------------------------------------------------------*/
HWND	ctrl_static_picture_create(HWND hWnd, uint menuID)
{
	HWND		hOut;
	HINSTANCE	hInst;

	hInst =	(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	hOut = CreateWindow(
				TEXT("static"),	NULL,
				WS_CHILD | WS_VISIBLE |	WS_BORDER | SS_NOTIFY,
				0, 0, 0, 0,
				hWnd, (HMENU)menuID, hInst,	NULL);
	if(hFontDefault != NULL) CTRL_FONT_SET(hOut, hFontDefault);	// フォント設定
	return hOut;
}
/*　以下のようなイベントになります。
	//ctrl_static_picture_create(hWnd, ID_STATIC);で作成後
	case WM_COMMAND:
		if(LOWORD(wParam) == ID_STATIC)
		{
			switch(HIWORD(wParam))
			{
			case STN_CLICKED:
				// クリックされたときの処理
				break ;

			case STN_DBLCLK:
				// ダブルクリックされたときの処理
				break ;
			}
		}
*/



/*------------------------------------------------------------------------------*
| <<< ピクチャーボックスの作成 >>>
|	入力	hWnd = マザーとなるハンドル
|	※ 大きさは	CTRL_RESIZE	で設定のこと
*------------------------------------------------------------------------------*/
HWND	ctrl_static_picture_create(HWND hWnd)
{
	return	ctrl_static_picture_create(hWnd, 1);
}



/*************************************************************************************************
	[[[ コンボボックス ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< コンボボックス作成 >>>
|	入力	hWnd = マザーとなるハンドル
|	※ 大きさは	CTRL_RESIZE	で設定のこと
*------------------------------------------------------------------------------*/
HWND	ctrl_combo_create(HWND hWnd, uint hMenu)
{

	if(hWnd == NULL) return NULL;

	HWND		hOut;
	HINSTANCE	hInst;
	
	hInst =	(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	hOut  = CreateWindow(
				TEXT("COMBOBOX"), NULL, 
				WS_CHILD | WS_VISIBLE | CBS_SIMPLE | CBS_DROPDOWN,
				0, 0, 0, 0,
				hWnd, (HMENU)hMenu, hInst,	NULL);
	if(hFontDefault != NULL) CTRL_FONT_SET(hOut, hFontDefault);	// フォント設定
	return hOut;
}



/*------------------------------------------------------------------------------*
| <<< コンボボックス作成 >>>
|	入力	hWnd = マザーとなるハンドル
|	※ 大きさは	CTRL_RESIZE	で設定のこと
*------------------------------------------------------------------------------*/
HWND	ctrl_combo_create(HWND hWnd)
{
	if(hWnd == NULL) return NULL;
	return	ctrl_combo_create(hWnd, 1);
}



/*------------------------------------------------------------------------------*
| <<< コンボボックスクリアー >>>
|	入力	hWnd = ハンドル
*------------------------------------------------------------------------------*/
void	ctrl_combo_clear(HWND hWnd)
{
	if(hWnd == NULL) return;
	ComboBox_ResetContent(hWnd);
}



/*------------------------------------------------------------------------------*
| <<< コンボボックスに項目を追加 >>>
|	入力	hWnd   = ハンドル
|			Format = 文字列
*------------------------------------------------------------------------------*/
void	ctrl_combo_str_add(HWND	hWnd, char* Format,...)
{
	if(hWnd == NULL) return;
    char	strBuf[512];

	va_list	ap;
	va_start(ap,	 Format);
	vsprintf(strBuf, Format, ap);
	va_end(ap);

	SendMessage(hWnd, CB_ADDSTRING,	0, (LPARAM)strBuf);
}



/*------------------------------------------------------------------------------*
| <<< コンボボックスの指定の項目の文字を得る >>>
|	入力	hWnd = ハンドル
|			iNum = 項目番号
|	出力	strOut : 文字列
*------------------------------------------------------------------------------*/
void	ctrl_combo_str_get(char *strOut, HWND hWnd, int iSel)
{
	if(hWnd == NULL) return;
	SendMessage(hWnd, CB_GETLBTEXT,	iSel, (LPARAM)strOut);
}



/*------------------------------------------------------------------------------*
| <<< コンボボックスの何番目を表示するか指定 >>>
|	入力	hWnd = ハンドル
|			iSel = 項目番号
*------------------------------------------------------------------------------*/
void	ctrl_combo_sel_set(HWND	hWnd, int iSel)
{
	if(hWnd == NULL) return;
	SendMessage(hWnd, CB_SETCURSEL,	iSel, 0);
}



/*------------------------------------------------------------------------------*
| <<< コンボボックスの選択中の番号を得る >>>
|	入力	hWnd = ハンドル
*------------------------------------------------------------------------------*/
int		ctrl_combo_sel_get(HWND	hWnd)
{
	return (int)SendMessage(hWnd, CB_GETCURSEL,	0L,	0L);
}



/*------------------------------------------------------------------------------*
| <<< コンボボックスの項目と文字列を比較する >>>
|	入力	hWnd = ハンドル
*------------------------------------------------------------------------------*/
bool	ctrl_combo_strcmp(HWND hWnd, int iSel, char* Format,...)
{
    char	strBuf0[512], strBuf1[512];

	va_list	ap;
	va_start(ap,	  Format);
	vsprintf(strBuf0, Format, ap);
	va_end(ap);

	ctrl_combo_str_get(strBuf1, hWnd, iSel);
	if(strcmp(strBuf0, strBuf1) == 0) return true;
	return false;
}



/*------------------------------------------------------------------------------*
| <<< コンボボックスの項目数を返す >>>
|	入力	hWnd = ハンドル
*------------------------------------------------------------------------------*/
int		ctrl_combo_max_get(HWND	hWnd)
{
	return (int)SendMessage(hWnd, CB_GETCOUNT, 0, 0);
}



/*************************************************************************************************
	[[[ グループボックス ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< グループボックスの作成 >>>
|	入力	hWnd		  =	ウィンドゥハンドル
|			Format... 	  =	メッセージ
*------------------------------------------------------------------------------*/
HWND	ctrl_group_box_create(HWND hWnd, char* Format,...)
{
	HWND		hOut;
	HINSTANCE	hInst;
	char		buf[256];

	vsprintf(buf, Format, (char	*)(&Format + 1));

	hInst =	(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	hOut = CreateWindow(
				TEXT("BUTTON"),	TEXT(buf),
				WS_CHILD | WS_VISIBLE |	BS_GROUPBOX,
				0, 0, 0, 0,
				hWnd, (HMENU)1,	hInst, NULL);
	if(hFontDefault != NULL) CTRL_FONT_SET(hOut, hFontDefault);	// フォント設定
	return hOut;
}



/*------------------------------------------------------------------------------*
| <<< グループボックスのテキスト変更 >>>
|	入力	hWnd		  =	ウィンドゥハンドル
|			Format... 	  =	メッセージ
*------------------------------------------------------------------------------*/
void	ctrl_group_box_printf(HWND hWnd, char* Format,...)
{
	if(hWnd == NULL) return;

	char	strBuf[512];

	va_list	ap;
	va_start(ap,	 Format);
	vsprintf(strBuf, Format, ap);
	va_end(ap);

	SetWindowText(hWnd, strBuf);
}



/*************************************************************************************************
	[[[ ツールチップ ]]]
**************************************************************************************************/
// ツールチップの既知のバグとしては以下のようなものがあります
// セカンドディスプレイでツールチップが表示された瞬間に、
// DirectX 面が真っ黒になる。
// プライマリーではこのような現象は起きない(2009_10_15)。

/*------------------------------------------------------------------------------*
| <<< ツールチップの作成 >>>
|	※現状でリサイズ時にちらつきます
|	入力	hMother  = 母体となるウィンドウ
|			hWnd	 = ウィンドゥハンドル
|			str		 = 表示する文字列
*------------------------------------------------------------------------------*/
HWND	ctrl_tool_tips_create(HWND hMother, HWND hWnd, char* Format,...)
{

	HWND		hTool;
	TOOLINFO	ti;
	char		buf[256];


	vsprintf(buf, Format, (char	*)(&Format + 1));

	//--- ツールチップを作る ----------------------------------------
	ti.hwnd	 = hWnd;
	ti.hinst = sys.hInst;
	hTool	 = CreateWindowEx(0, TOOLTIPS_CLASS, NULL, TTS_ALWAYSTIP,
								CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
								hMother, NULL, sys.hInst, NULL);

	GetClientRect(ti.hwnd, &ti.rect);
	ti.cbSize	= sizeof(TOOLINFO);
	ti.uFlags	= TTF_SUBCLASS;
	ti.uId		= 1;
	ti.lpszText	= TEXT(buf);
	SendMessage(hTool, TTM_ADDTOOL,	0, (LPARAM)&ti);
	//SendMessage(hTool, TB_SETTOOLTIPS, (WPARAM)&ti, 0); 

	if(hFontDefault != NULL){ CTRL_FONT_SET(hTool, hFontDefault);}	// フォント設定
	
	return	hTool;
}



/*------------------------------------------------------------------------------*
| <<< ツールチップの作成 >>>
|	入力	hMother   = 母体となるウィンドウ
|			uiControl = コントロールID
|			str		  = 表示する文字列
*------------------------------------------------------------------------------*/
HWND	ctrl_tool_tips_create(HWND hMother, uint uiControl, char* Format,...)
{

	char	buf[256];

	vsprintf(buf, Format, (char	*)(&Format + 1));
	return	ctrl_tool_tips_create(hMother, GetDlgItem(hMother, uiControl), buf);
}



/*------------------------------------------------------------------------------*
| <<< ツールチップの文字設定 >>>
|	入力	hWnd	 = ウィンドゥハンドル
|			str		 = 表示する文字列
*------------------------------------------------------------------------------*/
void	ctrl_tool_tips_update_text(HWND	*hWnd, HWND hTarget, HWND hMother, char* Format,...)
{

/*	以下、うまく動作しません
	TOOLINFO	ti;
	char	buf[256];
	vsprintf(buf, Format, (char	*)(&Format + 1));
	ti.lpszText	= TEXT(buf);
	SendMessage(hWnd, TTM_UPDATETIPTEXT, 0,	(LPARAM)&ti);
	*/

	char	buf[256];
	vsprintf(buf, Format, (char	*)(&Format + 1));

	if(*hWnd){ DestroyWindow(*hWnd);}
	*hWnd = ctrl_tool_tips_create(hMother, hTarget, buf);
}



/*************************************************************************************************
	[[[ タブコントロール ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< タブコントロールに項目追加 >>>
|	入力	hWnd = ウィンドゥハンドル
|			iID	 = 識別番号
|			str	 = タグの項目
*------------------------------------------------------------------------------*/
void	ctrl_tab_add(HWND hWnd,	uint uiID, char* Format,...)
{
	TC_ITEM	tcItem;
	char	buf[256];

	vsprintf(buf, Format, (char	*)(&Format + 1));

	tcItem.mask	   = TCIF_TEXT;
	tcItem.pszText = TEXT(buf);
	SendMessage(hWnd, TCM_INSERTITEM, (WPARAM)uiID, (LPARAM)&tcItem);
}



/*------------------------------------------------------------------------------*
| <<< タブコントロールの作成 >>>
|	入力	hWnd = マザーとなるハンドル
|			uiID = 管理番号
|	※ 大きさは	CTRL_RESIZE	で設定のこと
*------------------------------------------------------------------------------*/
HWND	ctrl_tab_create(HWND hWnd, uint uiID)
{

	HWND	hTab;							// ハンドル

	hTab  =	CreateWindowEx(
			0,										// 拡張スタイルなし
			WC_TABCONTROL,							// クラスネーム
			NULL,									// ウィンドウネーム
			WS_CHILD | WS_VISIBLE |	WS_CLIPSIBLINGS,// ウィンドウスタイル
			0, 0, 0, 0,								// 位置、大きさ（初期状態では極端に小さくしておく）
			hWnd,									// 親ウィンドウのハンドル
			(HMENU)ID_TABCTRL,						// 子供ウィンドウ識別子
			sys.hInst,								// インスタンスハンドル
			NULL);									// WM_CREATEのパラメーターなし
	if(hFontDefault != NULL) CTRL_FONT_SET(hTab, hFontDefault);	// フォント設定
	return	hTab;
}



/*------------------------------------------------------------------------------*
| <<< タブコントロールの作成 >>>
|	入力	hWnd = マザーとなるハンドル
|	※ 大きさは	CTRL_RESIZE	で設定のこと
*------------------------------------------------------------------------------*/
HWND	ctrl_tab_create(HWND hWnd)
{
	return ctrl_tab_create(hWnd, ID_TABCTRL);
}



/*------------------------------------------------------------------------------*
| <<< タブに格納されるダイアログのサイズ変更 >>>
|	入力	hMother     = ダイアログのハンドル
|			iXOfs iYOfs = オフセット位置
*------------------------------------------------------------------------------*/
void	ctrl_tab_dailog_resize(HWND hMother, int iXOfs, int iYOfs)
{

	RECT	r;
	GetClientRect(sys.hWnd, &r);			// ウィンドウのサイズを得る

	/*int	w = 4, h0 = 22, h1 = 10;
	CTRL_RESIZE(hMother, r.right + iXOfs - (__SCR_W - __RENDER_X1) + SYS_XFRAME_GET() / 2 + w  - 2, 
						 r.top   + iYOfs + h0,
						 r.right  -  w + 1, 
						 r.bottom - h1 + 5);
	*/
	int	w = 3;
	int	b = CTRL_TAB_HEIGHT;				// タブの項目の高さ(どうやって算出するか不明なので直で書いてます)
	CTRL_RESIZE(hMother, r.right + iXOfs - (__SCR_W - __RENDER_X1) + SYS_XFRAME_GET() / 2 + w, 
						 r.top   + iYOfs						   + b,
						 r.right  - w * 2 + 1, 
						 r.bottom - 4);
}



/*------------------------------------------------------------------------------*
| <<< タブで選択されている項目番号を返す >>>
|	入力	hMother = ダイアログのハンドル
|	戻り値	項目番号
*------------------------------------------------------------------------------*/
int		ctrl_tab_get(HWND hMother)
{
	return	TabCtrl_GetCurSel(hMother);		// 選択タブの番号を得る
}



/*------------------------------------------------------------------------------*
| <<< タブの項目を選択する >>>
|	入力	hMother = ダイアログのハンドル
|			iNum    = 項目番号
*------------------------------------------------------------------------------*/
void	ctrl_tab_set(HWND hMother, int iNum)
{
	TabCtrl_SetCurSel(hMother, iNum);
}



/*************************************************************************************************
	[[[ スライダー ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< スライダーの作成 >>>
|	入力	hWnd   = マザーとなるハンドル
|	※ 大きさは	CTRL_RESIZE	で設定のこと
*------------------------------------------------------------------------------*/
HWND	ctrl_slider_create(HWND hWnd, DWORD dwFlag)
{
	HWND		hOut;
	HINSTANCE	hInst;

	hInst =	(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

	hOut = CreateWindowEx( 
				0,															// no extended styles
				TRACKBAR_CLASS,												// class name
				"Trackbar Control",											// title (caption)
				WS_CHILD | WS_VISIBLE | /*TBS_AUTOTICKS |*/ TBS_ENABLESELRANGE,	// style
										// ↑メモリを自動で表示するスイッチ
				0, 0, 0, 0,													// position & size
				hWnd,														// parent window
				(HMENU)ID_SLIDER,											// control identifier
				hInst,														// instance
				NULL);														// no WM_CREATE parameter
	if(hFontDefault != NULL) CTRL_FONT_SET(hOut, hFontDefault);	// フォント設定
	return hOut;
};


HWND	ctrl_slider_create(HWND hWnd)
{
	return ctrl_slider_create(hWnd, 0);
}



/*------------------------------------------------------------------------------*
| <<< スライダーのレンジ設定 >>>
|	入力	hWnd = スライダーのハンドル
*------------------------------------------------------------------------------*/
void	ctrl_slider_range_set(HWND hWnd, int iMin, int iMax)
{
	SendMessage(hWnd, TBM_SETRANGE,	0, MAKELONG(iMin, iMax));
}



/*------------------------------------------------------------------------------*
| <<< スライダーの値を設定 >>>
|	入力	hWnd = スライダーのハンドル
*------------------------------------------------------------------------------*/
void	ctrl_slider_set(HWND hWnd, int iPos)
{
	SendMessage(hWnd, TBM_SETPOS, TRUE, iPos);
}


/*------------------------------------------------------------------------------*
| <<< スライダーの値を得る >>>
|	入力	hWnd = スライダーのハンドル
|	戻り値	スライダーの値
*------------------------------------------------------------------------------*/
int		ctrl_slider_get(HWND hWnd)
{
	return SendMessage(hWnd, TBM_GETPOS, 0, 0);
}



/*************************************************************************************************
	[[[ リストビュー ]]]
**************************************************************************************************/



/*------------------------------------------------------------------------------*
| <<< リストビューの文字列描画 >>>
|	※フォーカスが他へ移ると、選択項目が分からなくなるので、こうしています
*------------------------------------------------------------------------------*/
static	void	ListViewTextDraw(HDC hdc, char *Text, RECT *rc,	int	fmt)
{
	
	DRAWTEXTPARAMS	para;
	int				Justify;
	
	// 隣り合う文字列がくっつかないように左右にマージンをつける。
	// ただし、右マージンは文字の配置を右寄せにしたときだけ付ける。
	para.cbSize		   = sizeof(para);
	para.iLeftMargin   = 2;
	para.iRightMargin  = 0;
	para.iTabLength	   = 0;
	para.uiLengthDrawn = 0;

	// サブアイテムの文字の配置を調べて、DrawTextEx用のスタイルに直す。
	if(		(fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT )
	{
		Justify	= DT_LEFT;
	}
	else if((fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
	{
		Justify			  =	DT_RIGHT;
		para.iRightMargin =	6;
	}
	else
	{
		Justify	= DT_CENTER;
	}

	// 文字列の表示　左右マージンをつけるのでDrawTextExを使っている。
	DrawTextEx(hdc,	Text, lstrlen(Text), rc, Justify | DT_VCENTER |	DT_SINGLELINE |	DT_END_ELLIPSIS, &para);
}



/*------------------------------------------------------------------------------*
| <<< リストビューアイテムを描画する >>>
*------------------------------------------------------------------------------*/
void	ctrl_list_view_draw(LPDRAWITEMSTRUCT lpDraw)
{

	HWND		hList =	lpDraw->hwndItem;	// リストビューのハンドル
	HDC			hDC	  =	lpDraw->hDC;		// デバイスコンテキスト
	RECT		rc,	rcAll, rcClient;
	HBRUSH		hBrush;						// 背景描画用のブラシハンドル
	COLORREF	Color;
	char		Text[256];
	int			SubItem;
	LVCOLUMN	LvColumn;					// 列項目取得用の構造体
	LVITEM		LvItem;						// アイテム情報取得用の構造体
	HIMAGELIST	hImage;						// イメージリストのハンドル
	int			SubItemNum;
	
	
	// デバイスコンテキストを保存
	SaveDC(hDC);
	
	// 文字の背景の表示モードを非透過に設定する。
	SetBkMode(hDC, OPAQUE);

	// ODS_SELECTEDフラグがあるときには、強調表示する。
	if(lpDraw->itemState & ODS_SELECTED)
	{
		// 背景色の取得
		hBrush = CreateSolidBrush (GetSysColor(COLOR_HIGHLIGHT));
		SetBkColor(	 hDC, GetSysColor(COLOR_HIGHLIGHT));
		SetTextColor(hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
	}
	else
	{
		Color =	ListView_GetTextBkColor(hList);
		
		// ListView_SetTextBkColorが一度もされてないときには、ff000000という色になるので
		// その時には、システムの色を使う。これをしないと黒になってしまう。
		if(Color & 0xff000000) Color = GetSysColor(COLOR_WINDOW);
		hBrush = CreateSolidBrush(Color);
	}

	// 列見出しのフォーマット情報を取得する。
	ZeroMemory(&LvColumn, sizeof(LvColumn));
	LvColumn.mask =	LVCF_FMT;
	ListView_GetColumn(hList, 0, &LvColumn);

	// 背景を埋める
	ListView_GetItemRect(hList,	lpDraw->itemID,	&rcAll,	LVIR_BOUNDS);
	ListView_GetItemRect(hList,	lpDraw->itemID,	&rc,	LVIR_LABEL);
	rcAll.left = rc.left;

	GetClientRect(hList, &rcClient);

	// アイテムの情報を取得します。表示するイメージのインデックスもいるのでListView_GetItemを使う。
	ZeroMemory(&LvItem,	sizeof(LvItem));
	LvItem.iItem      = lpDraw->itemID;
	LvItem.iSubItem   = 0;
	LvItem.mask       = LVIF_TEXT |	LVIF_IMAGE;
	LvItem.pszText    = Text;
	LvItem.cchTextMax = sizeof(Text);
	ListView_GetItem(hList, &LvItem);

	// イメージリストのハンドルを取得する
	hImage = ListView_GetImageList(hList, LVSIL_SMALL);

	// イメージリストが登録されているときだけ、イメージを表示する。
	// イメージ番号は、iImageメンバにある。
	if(hImage)
	{
		ListView_GetItemRect(hList, lpDraw->itemID, &rc, LVIR_ICON);
																	// イメージを強調表示するかどうかを設定する。
		ImageList_Draw(hImage, LvItem.iImage, hDC, rc.left, rc.top, ((lpDraw->itemState & ODS_SELECTED) ? ILD_FOCUS : ILD_NORMAL) | ILD_TRANSPARENT);
	}

	// 文字列を表示する。ListView_GetItemRectを使うと表示する領域が分かる。
	ListView_GetItemRect(hList, lpDraw->itemID, &rc, LVIR_LABEL);
	ListViewTextDraw(hDC, Text, &rc, LvColumn.fmt);

	// サブアイテムの数を取得する
	SubItemNum = Header_GetItemCount(ListView_GetHeader(hList)) - 1;

	// サブアイテムも同様に表示する。
	for(SubItem = 1; SubItem <= SubItemNum; SubItem++)
	{
		// 背景のクリア
		ListView_GetSubItemRect(hList, lpDraw->itemID, SubItem, LVIR_BOUNDS, &rc);
		FillRect(hDC, &rc, hBrush);

		// 列項目のフォーマットを得ます。構造体はさっき設定したのでそのまま使う。
		ListView_GetColumn(     hList, SubItem, &LvColumn);
		ListView_GetSubItemRect(hList, lpDraw->itemID, SubItem, LVIR_LABEL, &rc);

		// アイテムの情報を取得します。文字列だけなので今度はListView_GetItemTextを使います。
		ListView_GetItemText(hList, lpDraw->itemID, SubItem, Text, sizeof(Text));
		ListViewTextDraw(hDC, Text,	&rc, LvColumn.fmt);
	}

	if(lpDraw->itemState & ODS_FOCUS)
	{
		//DrawFocusRect(hDC, &rcAll);				// ここ暫定的にコメントにしています
	}
	
	// 作ったブラシの削除
	DeleteObject(hBrush);

	// デバイスコンテキストの復元
	RestoreDC(hDC, -1);
}



/*------------------------------------------------------------------------------*
| <<< リストビュー作成 >>>
|	入力	hWnd = おやのハンドル
|			bSw0 = true	: 複数選択可
|			bSw1 = true	: チェックボタン有
|	戻り値	作成したリストビューのハンドル
|	※リストビューを使う場合は、親ウィンドウのイベントで 
|			case WM_DRAWITEM:
|				ctrl_list_view_draw((LPDRAWITEMSTRUCT)lParam);
|	  としてください。
*------------------------------------------------------------------------------*/
HWND	ctrl_list_view_create(HWND hWnd, bool bSw0,	bool bSw1)
{

	HWND		hList;
	DWORD		dwStyle;
	HINSTANCE	hInst =	(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);


	if(bSw0) dwStyle = WS_CHILD	| WS_VISIBLE | LVS_REPORT;					// 複数選択可
	else	 dwStyle = WS_CHILD	| WS_VISIBLE | LVS_REPORT |	LVS_SINGLESEL;	// 単数選択可

	hList =	CreateWindowEx(
					0,
					WC_LISTVIEW, "", 
					WS_BORDER | dwStyle | LVS_OWNERDRAWFIXED,
					0, 0, 0, 0,	hWnd, (HMENU)ID_LISTVIEW, sys.hInst, NULL);
	dwStyle	 = ListView_GetExtendedListViewStyle(hList);

	if(bSw1) dwStyle |=	LVS_EX_CHECKBOXES |	LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES	| LVS_EX_HEADERDRAGDROP;
	else	 dwStyle |=						LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES	| LVS_EX_HEADERDRAGDROP ;

	ListView_SetExtendedListViewStyle(hList, dwStyle);

	if(hFontDefault != NULL) CTRL_FONT_SET(hList, hFontDefault);	// フォント設定

	return hList;
}



/*------------------------------------------------------------------------------*
| <<< リストビューサイズ変更 >>>
|	入力	hMother = 母体となるダイアログのハンドル
|			hList   = リストビューのハンドル
*------------------------------------------------------------------------------*/
void	ctrl_list_view_resize(HWND hMother, HWND hList)
{
	RECT	r0, r1;
	GetWindowRect(hMother, &r0);
	GetWindowRect(hList,   &r1);
	const	int		iOfs = - 8;
	MoveWindow(   hList,   r1.left - r0.left, r1.top - r0.top, r1.right - r1.left, r0.bottom - r0.top - (r1.top - r0.top) + iOfs, TRUE);
}



/*------------------------------------------------------------------------------*
| <<< コラムを追加する >>>
|	入力	hWnd	  =	ウィンドゥハンドル
|			iInsPoint =	追加する位置
|			str		  =	追加する列の文字列
|			iSize	  =	コラムのサイズ
*------------------------------------------------------------------------------*/
void	ctrl_list_view_column_add(HWND hWnd, int iInsPoint,	char *str, int iSize)
{
	LV_COLUMN	lvcol;
	lvcol.mask	   = LVCF_FMT |	LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvcol.fmt	   = LVCFMT_LEFT;
	lvcol.cx	   = iSize;
	lvcol.pszText  = str;
	lvcol.iSubItem = 0;
	ListView_InsertColumn(hWnd,	iInsPoint, &lvcol);
}



/*------------------------------------------------------------------------------*
| <<< コラムの幅を自動調節 >>>
|	入力	hWnd = ウィンドゥハンドル
|			iNum = コラム番号
*------------------------------------------------------------------------------*/
void	ctrl_list_view_column_resize(HWND hWnd,	int	iNum)
{
	ListView_SetColumnWidth(hWnd, iNum,	LVSCW_AUTOSIZE_USEHEADER);
}



/*------------------------------------------------------------------------------*
| <<< コラムの幅を得る >>>
|	入力	hWnd = ウィンドゥハンドル
|			iNum = コラム番号
*------------------------------------------------------------------------------*/
int	ctrl_list_view_column_size_get(HWND hWnd, int iNum)
{
	return ListView_GetColumnWidth(hWnd, iNum);
}



/*------------------------------------------------------------------------------*
| <<< アイテムを追加 >>>
|	入力	hWnd   = ウィンドゥハンドル
|			iSub   = サブアイテム番号( 0 で項目を追加していきます)
|			iItem  = アイテム番号
|			Format = 追加する列の文字列
*------------------------------------------------------------------------------*/
void	ctrl_list_view_add(HWND	hWnd, int iSub, int iItem, char* Format,...)
{

	char	buf[256];

	vsprintf(buf, Format, (char	*)(&Format + 1));

	if(iSub	== 0)
	{
		LVITEM	item;
		//item.mask	  =	LVIF_TEXT;
		item.mask	  =	LVIF_TEXT |	LVIF_PARAM;
 		item.iItem	  =	iItem;
		item.iSubItem =	iSub;
		item.pszText  =	buf;
		item.lParam	  =	0;
		ListView_InsertItem(hWnd, &item);
	}
	else
	{
		ListView_SetItemText(hWnd, iItem, iSub,	buf);
	}
}



/*------------------------------------------------------------------------------*
| <<< 指定の項目を削除 >>>
|	入力	hWnd   = ウィンドゥハンドル
|			iSub   = サブアイテム番号( 0 で項目を追加していきます)
|			iItem  = アイテム番号
|	戻り値	true : 開放した
*------------------------------------------------------------------------------*/
bool	ctrl_list_view_dec(HWND hWnd, int iItem)
{
	BOOL	bRet = ListView_DeleteItem(hWnd, iItem);
	if(bRet) return true;
	else	 return false;
}



/*------------------------------------------------------------------------------*
| <<< リストから文字を得る >>>
|	入力	hWnd   = ウィンドゥハンドル
|			iSub   = サブアイテム番号
|			iItem  = アイテム番号
|			str	   = 追加する列の文字列
|			iSize  = 文字列のサイズ
|	出力	strOut = 
*------------------------------------------------------------------------------*/
void	ctrl_list_view_str_get(char *strOut, HWND hWnd, int iSub, int iItem, int iSize)
{
	ListView_GetItemText(hWnd, iItem, iSub,	strOut,	iSize);
}



/*------------------------------------------------------------------------------*
| <<< リストに文字をセット >>>
|	入力	hWnd   = ウィンドゥハンドル
|			iSub   = サブアイテム番号( 0 で項目を追加していきます)
|			iItem  = アイテム番号
|			Format = 追加する列の文字列
*------------------------------------------------------------------------------*/
void	ctrl_list_view_str_set(HWND	hWnd, int iSub, int iItem, char* Format,...)
{

	char	buf[256];

	vsprintf(buf, Format, (char	*)(&Format + 1));
	ListView_SetItemText(hWnd, iItem, iSub,	buf);
}



/*------------------------------------------------------------------------------*
| <<< 指定の項目を交換 >>>
|	入力	hWnd   = ウィンドゥハンドル
|			a, b   = a と b を交換
*------------------------------------------------------------------------------*/
void	ctrl_list_view_swap(HWND hWnd, int a, int b)
{

	char	str0[256], str1[256];

	for(int i = 0; i < 64; i++)
	{
		str0[0] = NULL;
		str1[0] = NULL;
		ctrl_list_view_str_get(str0, hWnd, i, a, sizeof(str0));
		if(str0[0] == NULL) break;
		ctrl_list_view_str_get(str1, hWnd, i, b, sizeof(str1));
		if(str1[0] == NULL) break;
		ctrl_list_view_str_set(      hWnd, i, a, str1);
		ctrl_list_view_str_set(      hWnd, i, b, str0);
	}
}



/*------------------------------------------------------------------------------*
| <<< リストビュー選択時、チェックの部分が押されたか調べる >>>
|	入力	hWnd   = ハンドル
|	戻り値	true : チェック部分を押した
*------------------------------------------------------------------------------*/
bool	ctrl_list_view_check_click_check(HWND hWnd)
{
	LVHITTESTINFO	lvHitInfo;

	GetCursorPos(&lvHitInfo.pt);
	ScreenToClient(hWnd, &lvHitInfo.pt);

	if(lvHitInfo.pt.x < 18)	return true;	// チェックの部分を選択した
	return false;
}



/*------------------------------------------------------------------------------*
| <<< アイテムのチェックスイッチオン・オフ >>>
|	入力	hWnd  =	ウィンドゥハンドル
|			iItem =	アイテム番号
|			bSw	  =	チェックスイッチの状態
*------------------------------------------------------------------------------*/
#ifndef ListView_SetCheckState
   #define ListView_SetCheckState(hwndLV, i, fCheck) ListView_SetItemState(hwndLV, i, INDEXTOSTATEIMAGEMASK((fCheck)+1), LVIS_STATEIMAGEMASK)
#endif
void	ctrl_list_view_check_set(HWND hWnd,	int	iItem, bool	bSw)
{
//	ListView_SetItemState(hWnd,	iItem, INDEXTOOVERLAYMASK((int)bSw), LVIS_OVERLAYMASK);
	ListView_SetCheckState(hWnd, iItem, (int)bSw);
}



/*------------------------------------------------------------------------------*
| <<< アイテムのチェックスイッチの状態を得る >>>
|	入力	hWnd  =	ウィンドゥハンドル
|			iItem =	アイテム番号
|	戻り値	TRUE  :	オン
|			FALSE :	オフ
*------------------------------------------------------------------------------*/
bool	ctrl_list_view_check_get(HWND hWnd,	int	iItem)
{
	return (ListView_GetCheckState(hWnd, iItem)	== TRUE);
}



/*------------------------------------------------------------------------------*
| <<< リストの内容を全消去 >>>
|	入力	hWnd = ウィンドゥハンドル
*------------------------------------------------------------------------------*/
void	ctrl_list_view_reset(HWND hWnd)
{
	ListView_DeleteAllItems(hWnd);
}



/*------------------------------------------------------------------------------*
| <<< リストのアイテム数を調べる >>>
|	入力	hWnd = ウィンドゥハンドル
|	戻り値	全行数が返る
*------------------------------------------------------------------------------*/
int		ctrl_list_view_total_count_get(HWND	hWnd)
{
	return ListView_GetItemCount(hWnd);
}



/*------------------------------------------------------------------------------*
| <<< リストのフォーカスを返す >>>			※選択状態にする
|	入力	hWnd = ウィンドゥハンドル
|	戻り値	-1	 = 何も選択されていない
|			-1	!= 選択番号
*------------------------------------------------------------------------------*/
int		ctrl_list_view_focus_get(HWND hWnd)
{
	for(int	i =	0; i < ctrl_list_view_total_count_get(hWnd); i++)
	{
		UINT	uiState	= ListView_GetItemState(hWnd, i, LVIS_FOCUSED);
		//UINT	uiState	= ListView_GetItemState(hWnd, i, LVIS_SELECTED | LVIS_FOCUSED);
		/*if(uiState & LVIS_SELECTED)
		{
			// 選択されている
		}
		else*/
		if(uiState & LVIS_FOCUSED) return i;
	}
	return -1;
}



/*------------------------------------------------------------------------------*
| <<< リストのフォーカスをセットする >>>	※選択されている番号を返す
|	入力	hWnd = ウィンドゥハンドル
|			iSel = フォーカスをするリスト番号
*------------------------------------------------------------------------------*/
void	ctrl_list_view_focus_set(HWND hWnd, int iSel)
{
	//ListView_SetItemState(hWnd,	-1,	  0,				LVIS_DROPHILITED | LVIS_FOCUSED	| LVIS_SELECTED	| LVIS_CUT);
	//ListView_SetItemState(hWnd,	iSel, LVIS_DROPHILITED,	LVIS_DROPHILITED);
	//SetFocus(hWnd);

	ListView_SetItemState(hWnd, iSel, LVIS_SELECTED, LVIS_SELECTED);
}



/*------------------------------------------------------------------------------*
| <<< リストのフォーカスを解除する >>>
|	入力	hWnd = ウィンドゥハンドル
|			iSel = フォーカスを解除するリスト番号
*------------------------------------------------------------------------------*/
void	ctrl_list_view_focus_release(HWND hWnd, int iSel)
{
	//ListView_SetItemState(hWnd,	-1,	0,  LVIS_FOCUSED);	// -1番目を選択状態にするという方法で回避
	ListView_SetItemState(hWnd,	iSel,	0,  LVIS_SELECTED);	// -1番目を選択状態にするという方法で回避
}



/*------------------------------------------------------------------------------*
| <<< リストビューのマウスで選択した番号を得る >>>			※ドラッグしたときなどに使う
|	入力	hWnd = ウィンドゥハンドル
|			bSw  = true : そこになにもセットされてなくても選択番号を返す
|	戻り値	-1	 = 何も選択されていない
|			-1	!= 選択番号
*------------------------------------------------------------------------------*/
int		ctlr_list_view_sel_get(HWND hListView, bool bSw)
{
	LVHITTESTINFO	lvHitInfo;

	GetCursorPos(					   &lvHitInfo.pt);
	ScreenToClient(         hListView, &lvHitInfo.pt);
	ListView_SubItemHitTest(hListView, &lvHitInfo);

	char	str[MAX_PATH];
	ctrl_list_view_str_get(str, hListView, 0, (int)lvHitInfo.iItem, MAX_PATH);

	//--- 絶対値を得る ----------------------------------------------
	if(bSw) return (lvHitInfo.pt.y - 16) / 14;

	//--- 普通に値を返す --------------------------------------------
	if(str[0] == NULL) return -1;
	else			   return lvHitInfo.iItem;
}

int		ctlr_list_view_sel_get(HWND hListView)
{
	return ctlr_list_view_sel_get(hListView, false);
}



/*------------------------------------------------------------------------------*
| <<< アイテムが複数選択されている場合、次に選択されている番号を返す >>>
|	入力	hWnd  = ウィンドゥハンドル
|			iItem = アイテム番号
*------------------------------------------------------------------------------*/
int		ctrl_list_view_next_select_get(HWND hWnd, int iItem)
{

	return	ListView_GetNextItem(hWnd, iItem, LVNI_SELECTED);

	//※下のように使うこと
	//int	iItem = -1;
	//while((iItem = ctrl_list_next_select_get(Ctrl.pWork[0]->hListView[0], iItem)) != -1)
	//{
	//		選択されている項目すべてがここを通る
	//}
}




/*------------------------------------------------------------------------------*
| <<< リストビュードラッグ開始(ドラッグイメージ作成) >>>
|	入力	hList      = リストビューのハンドル
|			hMother    = マザーとなるウィンドウのハンドル
|			pNM      　= リストビューの表示通知メッセージ
|	出力	phimageOut = ドラッグイメージのハンドル
|	戻り値	選択した項目番号
*------------------------------------------------------------------------------*/
int		ctrl_list_view_drag_begin(HIMAGELIST *phimageOut, HWND hList, HWND hMother, NMLISTVIEW *pNM)
{

	// ドラッグイメージ取得
	POINT	pointPos;
	*phimageOut = ListView_CreateDragImage(hList, pNM->iItem, &pointPos);

	// リストビューよりドラッグ位置を計算して、開始
	POINT	pointCursor;
	GetCursorPos(  &pointCursor);
	ScreenToClient(hMother, &pointCursor);

	int	iX = pointCursor.x - pointPos.x;
	int	iY = pointCursor.y - pointPos.y;
	ImageList_BeginDrag(*phimageOut, 0, iX, iY);
	ImageList_DragEnter(hMother,    0, 0);

	// マウスキャプチャ
	SetCapture(hMother);

	return pNM->iItem;
}



/*------------------------------------------------------------------------------*
| <<< リストビュードラッグ中(ドラッグイメージの移動) >>>
|	入力	hList   = リストビューのハンドル
|			hMother = マザーとなるウィンドウのハンドル
|			iX, iY  = マウス座標
*------------------------------------------------------------------------------*/
void	ctrl_list_view_drag(HWND hList, HWND hMother, int iX, int iY)
{

	POINT	pointPos = {iX, iY};
	ClientToScreen(hMother, &pointPos);

	RECT	r0, r1;
	GetWindowRect(hMother, &r0);
	GetWindowRect(hList,   &r1);
	ImageList_DragMove(pointPos.x - r0.left + (r1.left - r0.left), pointPos.y - r0.top + (r1.top - r0.top));
}



/*------------------------------------------------------------------------------*
| <<< リストビュードラッグ終了(ドラッグイメージの解放) >>>
|	入力	hMother    = マザーとなるウィンドウのハンドル
|			phimageOut = ドラッグイメージのハンドル
*------------------------------------------------------------------------------*/
void	ctrl_list_view_drag_end(HWND hMother, HIMAGELIST *phimageListView)
{

	ImageList_DragLeave(hMother);
	ImageList_EndDrag();

	// ドラッグイメージの解放
	ImageList_Destroy(*phimageListView);
	*phimageListView = NULL;

	// マウスキャプチャ解除
	ReleaseCapture();
}



/*------------------------------------------------------------------------------*
| <<< マウスが指している、リストビューの番号を得る >>>
|	入力	hList   = リストビューのハンドル
|	戻り値	-1 == 無効なところ
|			-1 != 項目番号
*------------------------------------------------------------------------------*/
int		ctrl_list_view_pos_get(HWND hList)
{
	// カーソル位置を取得
	POINT	pointCursor;
	GetCursorPos(&pointCursor);

	// クリックされた位置のアイテムのインデックスを取得
	LVHITTESTINFO	lvHit;
	lvHit.pt.x = pointCursor.x;
	lvHit.pt.y = pointCursor.y;
	ScreenToClient(hList, &lvHit.pt);

	return ListView_HitTest(hList, &lvHit);
}



/*************************************************************************************************
	[[[ チェックボックス ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< チェックボックスの作成 >>>
|	入力	hWnd   = マザーとなるハンドル
|	※ 大きさは	CTRL_RESIZE	で設定のこと
*------------------------------------------------------------------------------*/
HWND	ctrl_check_box_create(HWND hWnd, char* Format,...)
{
	if(hWnd == NULL) return NULL;

	HWND		hOut;
	HINSTANCE	hInst;
	char		buf[256];

	vsprintf(buf, Format, (char	*)(&Format + 1));

	hInst =	(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

	hOut = CreateWindow(
				TEXT("BUTTON") , TEXT(buf),
				WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,// | BS_LEFTTEXT,
				0, 0, 0, 0,
				hWnd, (HMENU)1, hInst, NULL);
	if(hFontDefault != NULL) CTRL_FONT_SET(hOut, hFontDefault);	// フォント設定
	return hOut;
}



/*------------------------------------------------------------------------------*
| <<< チェックボックスに値を設定 >>>
|	入力	hWnd = コントロールのハンドル
|			bSw  = true or false;
*------------------------------------------------------------------------------*/
void	ctrl_check_box_set(HWND hWnd, bool bSw)
{
	if(hWnd == NULL) return;
	if(bSw) SendMessage(hWnd, BM_SETCHECK, BST_CHECKED,   0);
	else    SendMessage(hWnd, BM_SETCHECK, BST_UNCHECKED, 0);
}



/*------------------------------------------------------------------------------*
| <<< チェックボックスの状態を得る >>>
|	入力	hWnd = コントロールのハンドル
*------------------------------------------------------------------------------*/
bool	ctrl_check_box_get(HWND hWnd)
{
	if(hWnd == NULL) return false;
	return	(SendMessage(hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED);
}



/*************************************************************************************************
	[[[ フォルダー選択 ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< SHBrowseForFolder()用コールバック	>>>
*------------------------------------------------------------------------------*/
static	int	CALLBACK SHBrowseProc(HWND hWnd, UINT uMsg,	LPARAM lParam, LPARAM lpData)
{
	if(uMsg	== BFFM_INITIALIZED	&& lpData)
	{										// デフォルトで選択させるパスの指定
		SendMessage(hWnd, BFFM_SETSELECTION, TRUE, lpData);
	}
	return 0;
}



/*------------------------------------------------------------------------------*
| <<< システムが確保したITEMIDLISTを開放しなければならない >>>
*------------------------------------------------------------------------------*/
static	void	_SHFree(ITEMIDLIST *pidl)
{
	IMalloc	*pMalloc;
	SHGetMalloc(&pMalloc);
	if(pMalloc)
	{
		pMalloc->Free(pidl);
		pMalloc->Release();
	}
}



/*------------------------------------------------------------------------------*
| <<< フォルダ選択ダイアログの起動 >>>
|	入力	hWnd			  =	ウィンドウハンドル
|			lpszDefaultFolder =	デフォルトフォルダー
|			lpszBuffer		  =	選択したフォルダー名
|			strMessage		  =	メッセージ
*------------------------------------------------------------------------------*/
static	UINT	GetOpenFolderName(HWND hWnd, LPCTSTR lpszDefaultFolder,	LPTSTR lpszBuffer, char	*strMessage)
{
	BROWSEINFO	bi;
	ITEMIDLIST	*pidl;
	char		szSelectedFolder[MAX_PATH];
											// フックを掛ける
//	hookFolderHandle = SetWindowsHookEx(WH_CBT, ProcFolderHook, NULL, GetCurrentThreadId()); 
//	return NULL;//  MessageBox(sys.hWnd, str1, str2, uiType);

	ZeroMemory(&bi,	sizeof(BROWSEINFO));
	bi.hwndOwner = hWnd;
	bi.lpfn		 = SHBrowseProc;				// コールバック関数を指定
	bi.lParam	 = (LPARAM)lpszDefaultFolder;	// デフォルトで選択させておくフォルダを指定
	bi.lpszTitle = strMessage;					// タイトルの指定
	pidl		 = SHBrowseForFolder(&bi);		// フォルダダイアログの起動
	if(pidl)
	{											// 選択されたフォルダ名を取得
		SHGetPathFromIDList(pidl, szSelectedFolder);
		_SHFree(pidl);
		if((DWORD)lstrlen(szSelectedFolder)	< MAX_PATH)	lstrcpy(lpszBuffer,	szSelectedFolder);
		return IDOK;							// フォルダが選択された
	}
	return IDCANCEL;							// フォルダは選択されなかった
}



/*------------------------------------------------------------------------------*
| <<< フォルダー選択 >>>
|	入力	hWnd			 = ウィンドウハンドル
|			strDefaultFolder = デフォルトフォルダー名
|			strMessage		 = メッセージ
|	出力	sys.strSelFile	= フォルダー名
|	戻り値	true			 = フォルダー選択した
*------------------------------------------------------------------------------*/
bool	ctrl_folder_select(HWND	hWnd, char *strDefaultFolder, char *strMessage)
{
	char	dir[MAX_PATH];
	char	str[500];

	//--- デフォルトフォルダーを調べる ----------------------------
	if(strlen(strDefaultFolder)	== 0)		// 指定が無い場合は	c:\	で
	{
		sprintf(dir, "c:\\");
	}
	else									// ファイル名は切り離す
	{
		sys_div_path(dir, NULL,	NULL, strDefaultFolder);
	}
	//--- メッセージ設定 ------------------------------------------
	if(strMessage == NULL)
	{
		strcpy(str,	"フォルダを選択してください");
	}
	else
	{
		strcpy(str,	strMessage);
	}
	
	if(GetOpenFolderName(hWnd, dir,	sys.strSelFile,	str) ==	IDOK)
	{
		//MessageBox(hWnd, szFolder, "選択されたフォルダ", MB_OK);
		return true;
	}
	return false;
}



/*------------------------------------------------------------------------------*
| <<< フォルダー選択 >>>
|	入力	hWnd			 = ウィンドウハンドル
|			strDefaultFolder = デフォルトフォルダー名
|	出力	sys.strSelFile	 = フォルダー名
|	戻り値	true			 = フォルダー選択した
*------------------------------------------------------------------------------*/
bool	ctrl_folder_select(HWND	hWnd, char *strDefaultFolder)
{
	if(strDefaultFolder == NULL) return ctrl_folder_select(hWnd, sys.strExeFolder, NULL); 
	else						 return ctrl_folder_select(hWnd, strDefaultFolder, NULL);
}



/*************************************************************************************************
	[[[ スクロール ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< スクロールバーの作成 >>>
|	入力	hWnd   = ウィンドゥハンドル
|			dwFlag = ウィンドウフラグ		SBS_VERT(縦) or SBS_HORZ(横)
|			sc     = スクロールバー情報
|	戻り値	スクロールバーのハンドル
*------------------------------------------------------------------------------*/
HWND	ctrl_scrollbar_create(HWND hWnd, DWORD dwFlag, SCROLLINFO *sc)
{

	HINSTANCE	hInst =	(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

	HWND	h =  CreateWindow(TEXT("SCROLLBAR"), TEXT(""), WS_CHILD | WS_VISIBLE | SBS_HORZ, 0, 0, 0, 0, hWnd, (HMENU)1, hInst, NULL);

	SetScrollInfo(h, SB_CTL, sc, true);
	sc->fMask = SIF_POS;

	return	h;
}



/*------------------------------------------------------------------------------*
| <<< スクロールバーのイベント処理 >>>
|	入力	hWnd   = ウィンドゥハンドル
|			dwFlag	 = ウィンドウフラグ		SBS_VERT(縦) or SBS_HORZ(横)
|			sc     = スクロールバー情報
|	戻り値	true : 更新した
*------------------------------------------------------------------------------*/
bool	ctrl_scrollbar_proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp, SCROLLINFO *sc)
{

	bool	bRet = false;

	switch(msg)
	{
	case WM_HSCROLL:
		switch(LOWORD(wp))
		{
		case SB_LEFT:			sc->nPos = sc->nMin;					  bRet = true; break;
		case SB_RIGHT:			sc->nPos = sc->nMax;					  bRet = true; break;
		case SB_LINELEFT:		if(sc->nPos               ){ sc->nPos--;} bRet = true; break;
		case SB_LINERIGHT:		if(sc->nPos < sc->nMax - 9){ sc->nPos++;} bRet = true; break;
		case SB_PAGELEFT:		sc->nPos -= sc->nPage;					  bRet = true; break;
		case SB_PAGERIGHT:		sc->nPos += sc->nPage;					  bRet = true; break;
		case SB_THUMBPOSITION:	sc->nPos  = HIWORD(wp);					  bRet = true; break;
		}
		SetScrollInfo(hWnd, SB_CTL, sc, true);
		break;
	}

	return	bRet;
}



/*************************************************************************************************
	[[[ ラジオボタン ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< ラジオボタンの作成 >>>
|	入力	hWnd		= ウィンドゥハンドル
|			str			= ボタンに表示する文字列
|			bFirstGroup = グループの先頭時に true のこと
|	戻り値	スクロールバーのハンドル
*------------------------------------------------------------------------------*/
HWND	ctrl_radio_create(HWND hWnd, char *str, bool bFirstGroup)
{

	HINSTANCE	hInst =	(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

	DWORD	d = 0;
	if(bFirstGroup) d = WS_GROUP;

	HWND	h =  CreateWindow(TEXT("BUTTON"), TEXT(str), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | d, 0, 0, 0, 0, hWnd, NULL, hInst, NULL);

	if(hFontDefault != NULL) CTRL_FONT_SET(h, hFontDefault);	// フォント設定
	return	h;
}



/*------------------------------------------------------------------------------*
| <<< ラジオボタンの状態を得る >>>
|	入力	hWnd   = ウィンドゥハンドル
|	戻り値	true or false
*------------------------------------------------------------------------------*/
bool	ctrl_radio_get(HWND hWnd)
{
	if(SendMessage(hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED) return true;
	else													return false;
}



/*------------------------------------------------------------------------------*
| <<< ラジオボタンを設定する >>>
|	入力	hWnd   = ウィンドゥハンドル
|	戻り値	true or false
*------------------------------------------------------------------------------*/
void	ctrl_radio_set(HWND hWnd, bool bSw)
{

	// BST_CHECKED 			ボタンのチェックをオンにします。
	// BST_INDETERMINATE 	ボタンをグレイ表示（ 不確定）の状態にします。ボタンが、BS_3STATE スタイルまたは BS_AUTO3STATE スタイルを持つときにだけ使用してください。
	// BST_UNCHECKED		ボタンのチェックをオフにします。

	uint	 d = BST_CHECKED;
	if(!bSw) d = BST_UNCHECKED;
	SendMessage(hWnd, BM_SETCHECK, d, 0);
}



/*************************************************************************************************
	[[[ セパレーター ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< セパレーターのプロシージャ >>>
*------------------------------------------------------------------------------*/
static	CTRL_MOTHER	*ctrlMother;			// ctrl マザーのポインタ
static	void		*pCtrlSetFunc;			// CtrlSet関数の登録用
static	bool		bCtrlSeparatorDrag;		// セパレーターをドラッグするのに使用
static	LRESULT	CALLBACK	CtrlSeparatorProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{

	switch(uiMsg)
	{
	//--- カーソル変更 ----------------------------------------------
	case WM_SETCURSOR:
		SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		return 0;

	//--- マウス処理 ------------------------------------------------
	// ここでドラックさせている
	case WM_LBUTTONDOWN:
		bCtrlSeparatorDrag = true;
		SetCapture(hWnd);					// ウィンドウ外でもマウスの値が拾えるようにキャプチャー
		break;

	case WM_MOUSEMOVE:
		if(bCtrlSeparatorDrag)
		{
			POINT	a;						// マウスのスクリーン座標
			GetCursorPos(&a);
			
			RECT	r;
			GetWindowRect(sys.hWnd, &r);
											// iY にデバックウィインドーの描画領域 Y 座標
			int	iLeft = r.left + GetSystemMetrics(SM_CXFIXEDFRAME) + GetSystemMetrics(SM_CXEDGE);
			//int	iRight = r.right - (GetSystemMetrics(SM_CXFIXEDFRAME) + GetSystemMetrics(SM_CXEDGE));

			int	w = a.x - iLeft;
			//if(w <         256) break;		// 上限
			//if(w > SCR_W - 256) break;		// 下限

			if(w <              256){ break;}		// 上限
			if(w > sys.iWidth - 256){ break;}		// 下限
			
			ctrlMother->usSeparator[0] = (ushort)(a.x				  - iLeft);
			ctrlMother->usSeparator[1] = (ushort)(sys.iWidth		  - ctrlMother->usSeparator[0]);
			d3.sRenderW			       = (ushort)(a.x				  - iLeft);
			d3.rectRender->right       = (ushort)(d3.rectRender->left + d3.sRenderW);
			//CtrlSet();
			void( *func)(void);
			func = (void(*)())pCtrlSetFunc;
			(*func)();
		}
		break;

	case WM_LBUTTONUP:
		sys.cResizeCount   = 0;				// このカウンターの値が一定の値になるまでデバイスを作成しない
		bCtrlSeparatorDrag = false;
		ReleaseCapture();					// キャプチャーしたマウスを開放
		break;
	}
	return 	CallWindowProc(ctrlMother->procSeparator, hWnd, uiMsg, wParam, lParam);
}



/*------------------------------------------------------------------------------*
| <<< セパレーター作成 >>>
|	入力	*r = ウィンドウの範囲
*------------------------------------------------------------------------------*/
void	ctrl_separator(CTRL_MOTHER *c, void *pCtrlSet, RECT *r)
{

	if(c->hSeparator == NULL)
	{
		ctrlMother                = c;
		pCtrlSetFunc			  = pCtrlSet;
		ctrlMother->hSeparator	  = ctrl_static_text_create(sys.hWnd, NULL, "");
		ctrlMother->procSeparator = CTRL_PROC_CHANGE(ctrlMother->hSeparator, CtrlSeparatorProc);
	}

	ctrlMother->usSeparator[0] = sys.iWidth - ctrlMother->usSeparator[1];
	CTRL_RESIZE(ctrlMother->hSeparator, ctrlMother->usSeparator[0], r->top, ctrlMother->usSeparator[0] + 2, r->bottom);
	r->right += 4;
}



/*************************************************************************************************
	[[[ ツールバー ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< ツールバー作成 >>>
|	入力	iResourceNum = 画像のリソース番号(画像は連番にすること)
|			iW, iH       = ボタンのサイズ(ひとつあたりのサイズです)
|			tbb			 = 
|			iCount		 = ボタンの数
*------------------------------------------------------------------------------*/
#define		ID_TOOLBAR		500

HWND	ctrl_toolbar_create(int iResourceNum, int iW, int iH, TBBUTTON *tbb, /*TBBUTTON *tb,*/ int iCount)
{

	HWND	hTool = CreateToolbarEx(
						sys.hWnd,							// 親ウィンドウ
															// ウィンドウスタイル
						WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
						ID_TOOLBAR,							// コントロールID
						iCount,								// イメージの数
						sys.hInst,
						iResourceNum,
						tbb,
						iCount,								// ボタンの数
						0,									// ボタンの幅
						0,									// ボタンの高さ
						iW,									// イメージの幅
						iH,									// イメージの高さ
						sizeof(TBBUTTON));
	return hTool;
}



/*------------------------------------------------------------------------------*
| <<< ツールバーにスペースを挿入 >>>
|	入力	hMother = ツールバーハンドル
|			iCount  = スペースを入れるところ
*------------------------------------------------------------------------------*/
void	ctrl_toolbar_space_insert(HWND hMother, int iCount)
{

	TBBUTTON	tbSpace = {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0}; 

	SendMessage(hMother, TB_INSERTBUTTON, iCount, (LPARAM)&tbSpace);
}



/*------------------------------------------------------------------------------*
| <<< ツールバーのボタンの状態を設定 >>>
|	入力	hMother = ツールバーハンドル
|			tb      = ツールバーのテーブル
|			iNum    = 対象のボタン番号(左は端が０)
|			bSw     = true or false
*------------------------------------------------------------------------------*/
void	ctrl_toolbar_set(HWND hMother, TBBUTTON *tb, int iNum, bool bSw)
{

	SendMessage(hMother, TB_CHECKBUTTON, tb[iNum].idCommand, MAKELONG(bSw, 0));
}



/*------------------------------------------------------------------------------*
| <<< ツールバーの高さを得る >>>
|	入力	hMother = ツールバーハンドル
*------------------------------------------------------------------------------*/
/*uint	ctrl_toolbar_height_get(HWND hMother)
{
	return CTRL_TOOLBAR_H + 16;
}*/



/*------------------------------------------------------------------------------*
| <<< ツールバーの高さを得る >>>
|	入力	uiHeight = ツールバーの絵の縦サイズ
*------------------------------------------------------------------------------*/
uint	ctrl_toolbar_height_get(uint uiHeight)
{
	return uiHeight + 12;
}


