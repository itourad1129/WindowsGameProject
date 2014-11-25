//
//	ディスプレイ変更ダイアログ関係のプログラム
//
//


//#include	"main.h"


//--- 定数定義 ------------------------------------------------------
#undef	WIDTH_
#define	WIDTH_		435						// ウィンドウのサイズ
#undef	HEIGHT_
#define	HEIGHT_		(285 - 56)


static	void	ChangeSet(void);



/**************************************************************************************************
	[[[	ダイアログ関係 ]]]
**************************************************************************************************/

static	char	*strDisplay[] =				// fmtDisplay を文字化したもの
{
	"D3DFMT_X8R8G8B8", "D3DFMT_R5G6B5"
};
											// 頂点計算設定用
static	char	*strVP[] = {"HARDWARE_VP", "SOFTWARE_VP"};

static	D3_WORK		d3_bak;					// キャンセル時のバックアップ用
static	HWND		hDialog;				// デバイス変更ダイアログのハンドル



/*------------------------------------------------------------------------------*
| <<< 画面モードをコンボボックスに列挙する >>>
|	入力	iAdapter = アダプター番号（０がデフォルト）
|			fmt		 = 設定したいディスプレイフォーマット
|	戻り値	フルスクリーンの解像度を指す番号
*------------------------------------------------------------------------------*/
static	int		EnumMode(int iAdapter, D3DFORMAT fmt)
{
#if IDC_RESOLUTION_COMBO
	D3DDISPLAYMODE	mode;
	int				iRet	   = 0;
	HWND			hResoCombo = GetDlgItem(hDialog, IDC_RESOLUTION_COMBO);


	//SendMessage(hWnd, CB_RESETCONTENT, 0, 0);	// 消去
	ctrl_combo_clear(hResoCombo);

	int	iMax   = d3.d->GetAdapterModeCount(iAdapter, fmt);
	int	iCount = 0;
	for(int	i =	0; i < iMax; i++)
	{
		if(d3.d->EnumAdapterModes(iAdapter,	fmt, i,	&mode) == D3D_OK)
		{
			if(mode.RefreshRate	!= D3_HZ    ) continue;	// リフレッシュレートを	60MHz固定
			if(mode.Height		<  D3_REZO_H) continue;	// 解像度を縦 480 以下は無視
			//SendMessage(hWnd,	CB_ADDSTRING, 0, (LPARAM)buf);
			//#ifndef	__RENDER_FIXATION
			//	if(mode.Width == __SCR_W && mode.Height == __SCR_H)
			//#endif
			ctrl_combo_str_add(hResoCombo, "%d x %d", mode.Width, mode.Height);

			if(mode.Width  == (ushort)d3.sFullScreenRenderW
			&& mode.Height == (ushort)d3.sFullScreenRenderH) 
			{
				iRet = iCount;
			}
			iCount++;
		}
	}
	return iRet;
#else
	return 0;
#endif
}



/*------------------------------------------------------------------------------*
| <<< ダイアログ中のアダプターの設定を変更する >>>
|	入力	bWindow	= true	: ウィンドウで起動
*------------------------------------------------------------------------------*/
static	void	CtrlSet(bool bWindow)
{
#ifdef	IDC_ADAPTER_COMBO
	int		i, j;
	D3_WORK	*w		   = &d3;
	HWND	hAdpCombo  = GetDlgItem(hDialog, IDC_ADAPTER_COMBO);
	HWND	hAdpFCombo = GetDlgItem(hDialog, IDC_ADAPTERFORMAT_COMBO);
	HWND	hResoCombo = GetDlgItem(hDialog, IDC_RESOLUTION_COMBO);

	//--- アダプター ------------------------------------------------
	ctrl_combo_clear(hAdpCombo);
	for(i =	0; i < (int)d3.d->GetAdapterCount(); i++)
	{
		D3DADAPTER_IDENTIFIER9	Adapter;
		d3.d->GetAdapterIdentifier(i, 0, &Adapter);
		ctrl_combo_str_add(hAdpCombo, Adapter.Description);
	}
	ctrl_combo_sel_set(hAdpCombo, w->ucAdapter);

	//--- ウィンドウ ------------------------------------------------
	if(bWindow)
	{										// ウィンドウへスイッチを入れる
		CheckRadioButton(hDialog, IDC_WINDOW, IDC_FULLSCREEN, IDC_WINDOW);
		ctrl_combo_clear(hAdpFCombo);		// ディスプレイフォーマット
		for(i =	0; i < D3_FMT_DISPLAY_MAX; i++)
		{
			if(w->fmtDisplay ==	d3_display_format_tbl[i])
			{
				ctrl_combo_str_add(hAdpFCombo, strDisplay[i]);
				ctrl_combo_sel_set(hAdpFCombo, 0);
				break;
			}
		}
		EnableWindow(hAdpFCombo, false);	// ウィンドウの時はフォーマットの変更禁止
											// 解像度
		ctrl_combo_sel_set(hResoCombo, EnumMode(w->ucAdapter, w->fmtDisplay));
		EnableWindow(      hResoCombo, false);
	}
	//--- フルスクリーン --------------------------------------------
	else
	{										// フルスクリーンへスイッチを入れる
		CheckRadioButton(hDialog, IDC_WINDOW, IDC_FULLSCREEN, IDC_FULLSCREEN);
		ctrl_combo_clear(hAdpFCombo);		// ディスプレイフォーマット
		int	k =	0, l = 0;							// R5G6B5 で バックバッファが X8R8G8B8 というのもありえるのでこうしている
		for(i =	0; i < D3_FMT_DISPLAY_MAX; i++)
		{
			bool	flag = false;
			for(j =	0; j < D3_FMT_DISPLAY_MAX; j++)
			{
				if(SUCCEEDED(d3.d->CheckDeviceType(w->ucAdapter, w->deviceType,	d3_display_format_tbl[i], d3_display_format_tbl[j],	false))){ flag = true;}
			}
			if(w->fmtDisplay ==	d3_display_format_tbl[i]){ l = k;}
			if(flag){ ctrl_combo_str_add(hAdpFCombo, strDisplay[i]), k++;}
		}
		EnableWindow(      hAdpFCombo, true);	// フルスクリーン時はフォーマットの変更許可
		ctrl_combo_sel_set(hAdpFCombo, l);
												// 解像度
		ctrl_combo_sel_set(hResoCombo, EnumMode(w->ucAdapter, w->fmtDisplay));
		EnableWindow(      hResoCombo, true);
	}
	//--- 頂点の計算をハードでやるかソフトでやるか ------------------
	/*ctrl_combo_clear(hVertCombo);
	j =	0;
	if((d3.Caps.DevCaps	& D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0)
	{
		if(d3.Caps.TextureCaps & D3DPTEXTURECAPS_ALPHAPALETTE
		|| d3.Caps.TextureCaps & D3DPTEXTURECAPS_ALPHA)
		{									// D3DCREATE_HARDWARE_VERTEXPROCESSING を追加
			ctrl_combo_str_add(hVertCombo, strVP[0]);
			j++;
		}
	}										// D3DCREATE_SOFTWARE_VERTEXPROCESSING を追加 MIX は無視
	ctrl_combo_str_add(hVertCombo, strVP[1]);
	if(j !=	0)								// 項目がふたつある場合は現在選択しているものとする。
	{
		if(d3.ucVertexProcessing ==	D3DCREATE_HARDWARE_VERTEXPROCESSING) j = 0;
		else															 j = 1;
	}
	ctrl_combo_sel_set(hVertCombo, j);
	*/
#endif
}



/*------------------------------------------------------------------------------*
| <<< 指定のスイッチ、コンボボックスの状態を d3に返す>>>
*------------------------------------------------------------------------------*/
static	void	ComboSet(int iID)
{
#ifdef	IDC_ADAPTER_COMBO
	int		i, sel;
	HWND	h = GetDlgItem(hDialog, iID);

	switch(iID)
	{
	case IDC_ADAPTER_COMBO:					// アダプター
		if(d3.ucAdapter	!= ctrl_combo_sel_get(h))
		{
			d3.ucAdapter = ctrl_combo_sel_get(h);
											// アダプターの状態を得る
			d3.d->GetDeviceCaps(		d3.ucAdapter, d3.deviceType, &d3.caps);
											// ディスプレイの状態を得る
			d3.d->GetAdapterDisplayMode(d3.ucAdapter, &d3.displayMode);
		}
		break;
	case IDC_ADAPTERFORMAT_COMBO:			// アダプターの画像フォーマット
		sel	= ctrl_combo_sel_get(h);
		for(i =	0; i < D3_FMT_DISPLAY_MAX; i++)
		{
			if(ctrl_combo_strcmp(GetDlgItem(hDialog, iID), sel,	strDisplay[i]))
			{
				d3.fmtDisplay	 = d3_display_format_tbl[i];
				d3.fmtBackBuffer = d3_display_format_tbl[i];
				break;
			}
		}
		break;
	case IDC_RESOLUTION_COMBO:				// 描画サイズ
		{
			char	str[256];
			ctrl_combo_str_get(str, h, ctrl_combo_sel_get(h));
			sscanf(str,	"%d x %d", &d3.sFullScreenRenderW, &d3.sFullScreenRenderH);
		}
		break;
	case IDC_WINDOW:
		d3.bWindow    = IsDlgButtonChecked(hDialog, iID) == BST_CHECKED;
		break;
	}
#endif
}



/*------------------------------------------------------------------------------*
| <<< ダイアログのイベント処理 >>>
*------------------------------------------------------------------------------*/
static	BOOL	CALLBACK	D3CtrlProc(HWND	hWnd, UINT msg,	WPARAM wParam, LPARAM lParam)
{
#ifdef	IDC_ADAPTER_COMBO
	switch(msg)
	{
	case WM_INITDIALOG:
		//static	bool	isLocked = false;		// 初期化時にはスイッチの動作を抑制
		//isLocked = true;
		CTRL_WINDOW_RECT_SET(hWnd);					// ウィンドウの中央へ
		hDialog	   = hWnd;
		d3.bWindow = true;							// 起動時は強制時にウィンドウとする
		CtrlSet(d3.bWindow);
		//isLocked	 = false;
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_ADAPTER_COMBO:				// アダプター
		case IDC_ADAPTERFORMAT_COMBO:		// アダプターの画像フォーマット
		case IDC_RESOLUTION_COMBO:			// 描画サイズ
		//case IDC_VERTEXPROCESSING_COMBO:	// 頂点の計算方法
			if(CBN_SELCHANGE ==	HIWORD(wParam))
			{
				ComboSet(LOWORD(wParam));	
				CtrlSet(IsDlgButtonChecked(hWnd, IDC_WINDOW) ==	BST_CHECKED);
			}
			break;
		case IDC_WINDOW:
			d3.fmtBackBuffer = d3.fmtDisplay = d3.deskTopDisplayMode.Format;
			ComboSet(LOWORD(wParam));
			CtrlSet(true);
			EnableWindow(GetDlgItem(hWnd, IDC_ADAPTERFORMAT_COMBO),	false);
			CheckRadioButton(hWnd, IDC_WINDOW, IDC_FULLSCREEN, IDC_WINDOW);
			break;
		case IDC_FULLSCREEN:
			d3.fmtBackBuffer = d3.fmtDisplay = d3.deskTopDisplayMode.Format;
			ComboSet(LOWORD(wParam));
			CtrlSet(false);
			EnableWindow(GetDlgItem(hWnd, IDC_ADAPTERFORMAT_COMBO),	true);
			CheckRadioButton(hWnd, IDC_WINDOW, IDC_FULLSCREEN, IDC_FULLSCREEN);
			break;
		case IDOK:							// OK ボタンを押した
			ChangeSet();
			SendMessage(hDialog, WM_CLOSE, 0, 0L);
			break;
		case IDCANCEL:						// CANCEL を押した
			memcpy(&d3,	&d3_bak, sizeof(D3_WORK));
			SendMessage(hDialog, WM_CLOSE, 0, 0L);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDialog, IDOK);
		hDialog	= NULL;
		break;
	default:
		return FALSE;
	}
#endif
	return TRUE;
}



/*------------------------------------------------------------------------------*
| <<< ダイアログオープン >>>
|	入力	bSw	= true	: ダイアログを開いて	 d3	を設定→アダプター初期化
|				  false	: ダイアログを開かないで d3	を設定→アダプター初期化
*------------------------------------------------------------------------------*/
void	ctrl_d3_open(void)
{
	memcpy(&d3_bak,	&d3, sizeof(D3_WORK));	// キャンセル時のバックアップ

	DialogBox(sys.hInst, "CHANGE_DEVICE", sys.hWnd,	D3CtrlProc);

	//--- エスケープが放されるまで待つ
	while(1)
	{
		if(!sys_key_get(VK_ESCAPE)){ break;}
	}
}



/*------------------------------------------------------------------------------*
| <<< 設定を更新 >>>
*------------------------------------------------------------------------------*/
void	ChangeSet(void)
{
#ifdef	IDC_ADAPTER_COMBO
	ComboSet(IDC_ADAPTER_COMBO);			// アダプター
	//ComboSet(IDC_VERTEXPROCESSING_COMBO);	// 頂点の計算方法
	ComboSet(IDC_WINDOW);					// ウィンドウ
											// ＯＫ時、HAL⇔REF、HARDWARE⇔SOFTWARE	の切り替えはデバイス新規作成
	if(d3.ucAdapter			 !=	d3_bak.ucAdapter
	|| d3.bWindow			 !=	d3_bak.bWindow
	|| d3.ucVertexProcessing !=	d3_bak.ucVertexProcessing)
	{
		while(sys.bMain);
		sys.cResizeCount = 0;
	}
#endif
}
