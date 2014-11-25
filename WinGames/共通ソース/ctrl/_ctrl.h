
#ifndef	__CTRL_
#define	__CTRL_


#define	CTRL_DIALOG_MAX		64					// ダイアログの最大数
#define	CTRL_CONTROL_MAX	64					// 各コントロール数
#define	CTRL_TAB_MAX		16					// タブの数
#define	CTRL_FONT_MAX		8					// フォントの数

#define	ID_TABCTRL			2000				// タブの識別子
#define	ID_LISTVIEW			4100				// リストビュー発生時の割り込み番号
#define	ID_STATIC			5200				// スタティックの割り込み番号

#define	ID_SLIDER			2020				// スライダー


//--- コントロールワーク --------------------------------------------
#if 1
	struct	CTRL_WORK
	{
		HWND		hWnd;								// ハンドル(これがマザーとなる)
		HWND		hCtrl[  CTRL_CONTROL_MAX];			// 各コントロールのハンドル
		WNDPROC		procOri[CTRL_CONTROL_MAX];			// 元々のスタティックのプロシージャ
	};
#endif

//--- コントロールマザー --------------------------------------------
// このワークは必ずひとつ作られ、CTRL_WORK は全てこのワークで管理される
struct	CTRL_MOTHER
{
	HWND		hSeparator;					// セパレーターハンドル
	WNDPROC		procSeparator;				// セパレーターの割り込み用
	ushort		usSeparator[4];				// セパレーターの位置(ウィンドウ上下左右の順)
	HWND		hCtrl[  CTRL_CONTROL_MAX];	// 各コントロールのハンドル
	HWND		hTips[  CTRL_CONTROL_MAX];	// 各コントロールのツールチップ
	WNDPROC		procOri[CTRL_CONTROL_MAX];	// 元々のスタティックのプロシージャ
	HWND		hMother;
};

//--- ツリービュー --------------------------------------------------
#define	CTRL_TREE_STR_MAX	256				// ツリービューの文字数
#define	CTRL_TREE_EXT_MAX	8				// 拡張子の総数(複数指定の場合は 8 個までの意味)
struct	CTRL_TREE_WORK
{
	HTREEITEM	hTree;						// ツリーのアイテムハンドル
	int			iObj[CTRL_TREE_EXT_MAX];		// アイコンの画像番号
	char		str[ CTRL_TREE_STR_MAX];		// 文字列の格納用
};

//--- 外部参照宣言 --------------------------------------------------
// フォント
	extern	void		ctrl_font_create(HFONT hFont);
	extern	HFONT		ctrl_font_create(int iSize, char *strFont);
	extern	void		ctrl_font_free(HFONT hFont);
// マザー
	extern	HWND		ctrl_create(HWND hMother, void *pProc, char *strResource);
	extern	HWND		ctrl_create(void *pProc, char *strResource);
	extern	HWND		ctrl_create(char *strDialog, void *pProc);
	extern	void		ctrl_debug_free(void);
	extern	HWND		ctrl_create(void *pProc);
	extern	void		ctrl_free(CTRL_MOTHER *w);
// デバック用
	extern	void		ctrl_debug_create(void);
	extern	void		ctrl_debug_view(bool bSw);
	extern	void		ctrl_debug_resize(int iX, int iY, int iW, int iH);
	extern	void		ctrl_debug_printf(char* Format,...);
// プッシュボタン
	extern	HWND		ctrl_push_button_create(HWND hWnd, uint menuID, char* Format,...);
	extern	void		ctrl_push_button_printf(HWND hWnd, char* Format,...);
// エディットボックス
	extern	HWND		ctrl_edit_create(HWND hWnd, DWORD dwFlag, HMENU hMenu);
	extern	HWND		ctrl_edit_create(HWND hWnd, DWORD dwFlag);
	extern	void		ctrl_edit_printf(HWND hWnd, char* Format,...);
	extern	void		ctrl_edit_get(HWND hWnd, char *strOut);
// スタティックテキスト
	extern	HWND		ctrl_static_text_create(HWND hWnd, uint menuID, DWORD dwFlag, char* Format,...);
	extern	HWND		ctrl_static_text_create(HWND hWnd,              DWORD dwFlag, char* Format,...);
	extern	void		ctrl_static_text_printf(HWND hWnd, char* Format,...);
// スタティックピクチャー
	extern	HWND		ctrl_static_picture_create(HWND hWnd);
	extern	HWND		ctrl_static_picture_create(HWND hWnd, uint menuID);
// コンボボックス
	extern	HWND		ctrl_combo_create(HWND hWnd, uint hMenu);
	extern	HWND		ctrl_combo_create(HWND hWnd);
	extern	void		ctrl_combo_clear(HWND hWnd);
	extern	void		ctrl_combo_str_add(HWND	hWnd, char* Format,...);
	extern	void		ctrl_combo_str_get(char *strOut, HWND hWnd, int iSel);
	extern	void		ctrl_combo_sel_set(HWND	hWnd, int iSel);
	extern	int			ctrl_combo_sel_get(HWND	hWnd);
	extern	bool		ctrl_combo_strcmp(HWND hWnd, int iSel, char* Format,...);
	extern	int			ctrl_combo_max_get(HWND	hWnd);
// グループボックス
	extern	HWND		ctrl_group_box_create(HWND hWnd, char* Format,...);
	extern	void		ctrl_group_box_printf(HWND hWnd, char* Format,...);
// ツールチップ
	extern	HWND		ctrl_tool_tips_create(HWND hMother, HWND hWnd, char* Format,...);
	extern	HWND		ctrl_tool_tips_create(HWND hMother, uint uiControl, char* Format,...);
	//extern	void		ctrl_tool_tips_update_text(HWND	hWnd, char* Format,...);
	extern	void		ctrl_tool_tips_update_text(HWND	*hWnd, HWND hTarget, HWND hMother, char* Format,...);
// スライダー
	extern	HWND		ctrl_slider_create(HWND hWnd, DWORD dwFlag);
	extern	HWND		ctrl_slider_create(HWND hWnd);
	extern	void		ctrl_slider_range_set(HWND hWnd, int iMin, int iMax);
	extern	void		ctrl_slider_set(HWND hWnd, int iPos);
	extern	int			ctrl_slider_get(HWND hWnd);
// タブ
	#define	CTRL_TAB_HEIGHT		(22)						// タブの項目高さ
	#define	CTRL_TAB_RESIZE(       hWnd, x0, y0, x1, y1)	CTRL_RESIZE(hWnd, x0 + GetSystemMetrics(SM_CXEDGE), y0, x1, y1)
	#define	CTRL_TAB_DIALOG_RESIZE(hWnd, x0, y0, x1, y1)	CTRL_RESIZE(hWnd, x0 + GetSystemMetrics(SM_CXEDGE) + 2, y0 + 22, x1 - 2, y1 - 2)
	#define	CTRL_TAB_DIALOG_RESIZE_(hWnd, r,  w,  h)		CTRL_RESIZE(hWnd, (r.right - w) + GetSystemMetrics(SM_CXEDGE) + 2, r.top + 22, r.right - GetSystemMetrics(SM_CXEDGE) * 2 - 2, r.bottom - 2)
	#define	CTRL_TAB_SELECT_GET(h)	TabCtrl_GetCurSel(h)		// 選択タブの番号を得る
	extern	void		ctrl_tab_add(HWND hWnd,	uint uiID, char* Format,...);
	extern	HWND		ctrl_tab_create(HWND hWnd, uint uiID);
	extern	HWND		ctrl_tab_create(HWND hWnd);
	extern	void		ctrl_tab_dailog_resize(HWND hMother, int iXOfs, int iYOfs);
	extern	int			ctrl_tab_get(HWND hMother);				// 選択タブの番号を得る
	extern	void		ctrl_tab_set(HWND hMother, int iNum);	// タブの項目を選択する

// リストビュー
	//#define	CTRL_LIST_VIEW_RESIZE(hWnd, x0, y0, x1, y1)	CTRL_RESIZE(hWnd, (x0) + 6, (y0) + 6, (x1) - 12, (y1) - 32);
	extern	void		ctrl_list_view_draw(LPDRAWITEMSTRUCT lpDraw);
	extern	HWND		ctrl_list_view_create(HWND hWnd, bool bSw0,	bool bSw1);
	extern	void		ctrl_list_view_resize(HWND hMother, HWND hList);
	extern	void		ctrl_list_view_column_add(HWND hWnd, int iInsPoint,	char *str, int iSize);
	extern	void		ctrl_list_view_column_resize(HWND hWnd,	int	iNum);
	extern	int			ctrl_list_view_column_size_get(HWND hWnd, int iNum);
	extern	void		ctrl_list_view_reset(HWND hWnd);
	extern	void		ctrl_list_view_add(HWND	hWnd, int iSub, int iItem, char* Format,...);
	extern	bool		ctrl_list_view_dec(HWND hWnd, int iItem);
	extern	void		ctrl_list_view_str_get(char *strOut, HWND hWnd, int iSub, int iItem, int iSize);
	extern	void		ctrl_list_view_str_get(char *strOut, HWND hWnd, int iSub, int iItem);
	extern	void		ctrl_list_view_str_set(HWND hWnd, int iSub, int iItem, char* Format,...);
	extern	void		ctrl_list_view_swap(HWND hWnd, int a, int b);				// ドラッグ時、項目の入れ替え
	extern	int			ctrl_list_view_total_count_get(HWND	hWnd);
	extern	bool		ctrl_list_view_check_click_check(HWND hWnd);				// リストビュー選択時、チェックの部分が押されたか調べる
	extern	void		ctrl_list_view_check_set(HWND hWnd,	int	iItem, bool	bSw);	// リストビューのチェックボックスにチェックを付ける
	extern	bool		ctrl_list_view_check_get(HWND hWnd,	int	iItem);				// リストビューのチェックボックスにチェックの状態を得る
	extern	int			ctrl_list_view_focus_get(HWND hWnd);
	extern	void		ctrl_list_view_focus_set(HWND hWnd, int iSel);				// フォーカスを当てる(選択状態にする)
	extern	void		ctrl_list_view_focus_release(HWND hWnd, int iSel);			// フォーカスを解除する
	extern	int			ctlr_list_view_sel_get(HWND hListView, bool bSw);			// リストビューのマウスで選択した番号を得る
	extern	int			ctlr_list_view_sel_get(HWND hListView);
	extern	int			ctrl_list_view_next_select_get(HWND hWnd, int iItem);
																					// リストビュードラッグ開始(ドラッグイメージ作成)
	extern	int			ctrl_list_view_drag_begin(HIMAGELIST *phimageOut, HWND hList, HWND hMother, NMLISTVIEW *pNM);
																					// リストビュードラッグ中(ドラッグイメージの移動)
	extern	void		ctrl_list_view_drag(HWND hList, HWND hMother, int iX, int iY);
																					// リストビュードラッグ終了(ドラッグイメージの解放)
	extern	void		ctrl_list_view_drag_end(HWND hMother, HIMAGELIST *phimageListView);
	extern	int			ctrl_list_view_pos_get(HWND hList);							// マウスが指している、リストビューの番号を得る

// チェックボックス
	extern	HWND		ctrl_check_box_create(HWND hWnd, char* Format,...);
	extern	void		ctrl_check_box_set(HWND hWnd, bool bSw);
	extern	bool		ctrl_check_box_get(HWND hWnd);
// フォルダー選択
	extern	bool		ctrl_folder_select(HWND	hWnd, char *strDefaultFolder, char *strCaption);
	extern	bool		ctrl_folder_select(HWND	hWnd, char *strDefaultFolder);
// スクロールバー
	extern	HWND		ctrl_scrollbar_create(HWND hWnd, DWORD dwFlag, SCROLLINFO *sc);
	extern	bool		ctrl_scrollbar_proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp, SCROLLINFO *sc);
// ラジオボタン
	extern	HWND		ctrl_radio_create(HWND hWnd, char *str, bool bFirstGroup);
	extern	bool		ctrl_radio_get(HWND hWnd);
	extern	void		ctrl_radio_set(HWND hWnd, bool bSw);
// セパレーター
	extern	void		ctrl_separator(RECT *r);
// ツールバー
	//#define	CTRL_TOOLBAR_HEIGHT_GET(h)	(h + 22)
	extern	HWND		ctrl_toolbar_create(int iResourceNum, int iW, int iH, TBBUTTON *tbb, int iCount);
	extern	void		ctrl_toolbar_space_insert(HWND hMother, int iCount);
	extern	void		ctrl_toolbar_set(HWND hMother, TBBUTTON *tb, int iNum, bool bSw);
	extern	uint		ctrl_toolbar_height_get(HWND hMother);
	extern	uint		ctrl_toolbar_height_get(uint uiHeight);
// ツリービュー
	extern	HTREEITEM	ctrl_tree_add(HWND hWnd, HTREEITEM hItem, HTREEITEM hNext, int iImage, char* Format,...);
	extern	HTREEITEM	ctrl_tree_add(HWND hWnd, HTREEITEM hItem, int iImage, char* Format,...);
	extern	void		ctrl_tree_reset(HWND hWnd);
	extern	void		ctrl_tree_dir_all_view(HWND hWnd, CTRL_TREE_WORK *tree);
	extern	int			ctrl_tree_dir_sel_get(HWND hWnd, CTRL_TREE_WORK *tree);
	extern	void		ctrl_tree_dir_get(CTRL_TREE_WORK *treeOut, HWND hWnd, int iCount, HTREEITEM *hParent, LPCTSTR lpszFolder, LPCTSTR lpszWildCard, bool bSw);
	extern	void		ctrl_tree_dir_get(CTRL_TREE_WORK *treeOut, HWND hWnd, int iMax, char *strFolder, char *strWildCard, bool bSw);


//--- 便利なマクロたち ----------------------------------------------------------------------------

//	ダイアログorコントールの表示、非表示
	inline	void	CTRL_SHOW(HWND hWnd, bool bSw)
	{
		if(bSw){ ShowWindow(hWnd, SW_SHOW);}
		else   { ShowWindow(hWnd, SW_HIDE);}
	}

//	コントロールのサイズ変更要求
	inline	void	CTRL_RESIZE(HWND hWnd){		SendMessage(hWnd, WM_SIZE, 0, 0);}

//	コントロールのサイズ変更
//		入力	ウィンドウの四点を指定のこと
	inline	void	CTRL_RESIZE(HWND hWnd, uint x0, uint y0, uint x1, uint y1)
	{
		MoveWindow(hWnd, x0, y0, (x1) -	(x0), (y1) - (y0), TRUE);
	}

// ダイアログ生成、ウィンドウの中央へ
	inline	void	CTRL_WINDOW_RECT_SET(HWND hWnd)
	{
		RECT	r;
		GetWindowRect(hWnd,	&r);
		int		iWidth  = (r.right  - r.left);		// ダイアログのサイズを得る
		int		iHeight = (r.bottom - r.top );

		GetWindowRect(sys.hWnd,	&r);
		int		w  = (r.right  - r.left) / 2;
		int		h  = (r.bottom - r.top ) / 2;
		int		x0 =  r.left + w - iWidth  / 2;
		int		y0 =  r.top	 + h - iHeight / 2;
		MoveWindow(hWnd, x0, y0, iWidth, iHeight, TRUE);
	}

// ダイアログ生成、ウィンドウの中央へ
	inline	void	CTRL_WINDOW_RECT_SET(HWND hWnd, int iWidth, int iHeight)
	{
		RECT	r;
		GetWindowRect(sys.hWnd,	&r);
		int		w  = (r.right  - r.left) / 2;
		int		h  = (r.bottom - r.top ) / 2;
		int		x0 =  r.left + w - iWidth  / 2;
		int		y0 =  r.top	 + h - iHeight / 2;
		MoveWindow(hWnd, x0, y0, iWidth, iHeight, TRUE);
	}

// 各コントロールのフォント変更
	inline	void	CTRL_FONT_SET(HWND hWnd, HFONT hFont)
	{
		SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0)); 
	}

// 割り込みイベントを書き換える
	inline	WNDPROC	CTRL_PROC_CHANGE(HWND hWnd, WNDPROC newProc)
	{
												// 元々のメッシュリストビューのプロシージャを保存しイベントをのっとる
		WNDPROC	proc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
		SetWindowLong(hWnd, GWL_WNDPROC, (LONG)newProc);

		return proc;
	}

// リストビューのダミーイベント
//	※リストビュー外で、マウスのホイールをまわしてもリストをスクロールさせない処理
	#define	CTRL_LIST_VIEW_DUMMY_PROC(FuncName, OriProc)									\
		static	LRESULT	CALLBACK	FuncName(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)		\
		{																					\
			switch(msg)																		\
			{																				\
			case WM_KEYDOWN: return NULL; /* キーを無効にする */							\
			case WM_MOUSEMOVE:																\
			case WM_MOUSEWHEEL:																\
			if(pad_mosue_render_range_check()){ return NULL;}								\
				break;																		\
			}																				\
			return CallWindowProc(OriProc, hWnd, msg, wp, lp);								\
		}

// リストビュードラッグイベント
	#define	CTRL_LIST_VIEW_DRAG_EVENT(Num, FuncName, hListWnd, FuncCall)						\
		static	void		FuncCall(bool bCancel);												\
		static	ushort		usSel_##Num;														\
		static	HANDLE		hCapture_##Num;														\
		static	HIMAGELIST	hImage_##Num;														\
		void	FuncName(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)					\
		{																						\
			switch(uiMsg)																		\
			{																					\
			case WM_LBUTTONUP:				/* リストビュードラッグ終了			*/				\
			case IDCANCEL:																		\
				if(hCapture_##Num  != NULL)														\
				{																				\
					if(ctrl_list_view_drag_end(&hCapture_##Num, &hImage_##Num, hWnd, hListWnd))	\
					{						/* LBUTTONUP のときは配置する */					\
						FuncCall(uiMsg == IDCANCEL);											\
					}																			\
				}																				\
				break;																			\
			case WM_MOUSEMOVE:																	\
				if(hCapture_##Num != NULL)														\
				{																				\
					ctrl_list_view_drag_on(hWnd, hListWnd, LOWORD(lParam), HIWORD(lParam));		\
				}																				\
				break;																			\
			}																					\
		}

// リストビュードラッグ開始		※CTRL_LIST_VIEW_DRAG_EVENT と対応する番号で呼び出しのこと
	#define	CTRL_LIST_VIEW_DRAG_BEGIN(Num, hMotherWnd, hListWnd, FuncCall)												\
		{																												\
			if(ctlr_list_view_sel_get(hListWnd) != -1)																	\
			{																											\
				FuncCall;																								\
				ctrl_list_view_drag_begin(&hCapture_##Num, &hImage_##Num, hMotherWnd, hListWnd, (NMLISTVIEW*)lParam);	\
			}																											\
		}

#endif										// end of __CTRL_

