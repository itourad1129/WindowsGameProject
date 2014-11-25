//
// アプリケーションの初期化
//
//

#define	SYSTEM_FILE			"system.ini"	// ウィンドウのサイズ保存用ファイル
#define	DEBUG_FILE			"DEBUG.TXT"		// デバッグ用のログファイル
#define	__URL				""				// バージョンダイアログの URL
											// 透過色
#define	__COLORKEY			D3DCOLOR_ARGB(255, 0, 0, 0)

#define	SYS_RESOURCE_MAX	8				// ユーザーが使用できるリソース数
#define	TIME_RATE_MAX		60				// FPS の計測するフレーム数
#define	FILE_BAK			8				// ファイル名の記憶数


//---- 以下の定数は書き換え不可 -------------------------------------------------------------------
#define	DIRECTINPUT_VERSION	0x0800			// DirectInput のバージョン（警告防止の為）

#if !WINVER
	#define	WINVER			0x040A			// Windows 98 &	Windows	2000以降　※IME	を禁止するのに必要
#endif

#define	DEVICE_MAKE_TIME			2		// リサイズ後、デバイス作成までの時間
#define	ID_MYTIMER					100		// タイマーイベント
#define	TIMER_COUNT					1000	// タイムスタンプを調べる間隔
#define	MONITORS_MAX				16		// サポートする最大モニター数

#define	_CRT_SECURE_NO_DEPRECATE	1		// VC8対策用
#pragma	warning(disable:4996)				// VC8対策用
#pragma warning(disable:4793)
#pragma warning(disable:4995)

#ifndef	__META
	#define		__META		__MQO			// メタセコイヤ変更
#endif


//--- ウィンドウスタイル --------------------------------------------------------------------------
#define	WINDOW_STYLE0		CS_DBLCLKS
							// CS_DBLCLKS      : ダブルクリックあり
							// CS_NOCLOSE      : 閉じるボタン無効

							// CS_HREDRAW	   : 横サイズが変わったとき，ウインドウ全体を再描画する
							// CS_VREDRAW	   : 縦サイズが変わったとき，ウインドウ全体を再描画する
							// CS_NOCLOSE	   : システムメニューの［閉じる］コマンドを使用不可にする
							// CS_CLASSDC	   : このクラスのウインドウ同士で１つのデバイスコンテキストを共有する
							// CS_OWNDC		   : ウインドウ毎にデバイスコンテキストを１つづつ持つ
							// CS_PARENTDC     : 親ウインドウのデバイスコンテキストを使用する
							// CS_GLOBALCLASS  : このクラスを作成したアプリケーションが実行されていれば，他のアプリケーションからでもこのクラスを利用できるようになる．一般的にDLLでカスタムコントロールを作成するときに使うらしい．

#define	WINDOW_STYLE1		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
							// WS_OVERLAPPED   : タイトルバー、境界線つきオーバーラップウインドウ
							// WS_CAPTION      : タイトルバー(WS_DLGFRAMEも一緒についてくる)
							// WS_SYSMENU      : タイトルバーにシステムメニュー(閉じるボタンとアイコン)つける
							// WS_THICKFRAME   : サイズ変更できる境界線
							// WS_MINIMIZEBOX  : システムメニューに最小化ボタン加える
							// WS_MAXIMIZEBOX  : システムメニューに最大化ボタン加える
							// WS_VISIBLE      : 初期状態で表示される

							// WS_CHILD		   : 子ウインドウ．WS_POPUPとは併用できない
							// WS_POPUP		   : ポップアップウインドウ．WS_CHILDとは併用できない
							// WS_CLIPCHILDREN : 親の描画で子ウインドウの部分をクリッピング
							// WS_CLIPSIBLINGS : 子の描画で兄弟ウインドウの部分をクリッピング
							// WS_BORDER	   : 細い境界線
							// WS_DLGFRAME	   : サイズ変更できない境界線
							// WS_HSCROLL	   : horizon  scroll(水平方向スクロール)
							// WS_VSCROLL	   : vertical scroll(垂直方向スクロール)
							// WS_DISABLED	   : 初期状態が使用不可
							// WS_MAXIMIZE	   : 初期状態で最大化
							// WS_MINIMIZE	   : 初期状態で最小化
							// WS_TABSTOP	   : ダイアログのコントロールにつかう．Tabでフォーカスを当てられるようにする．
							// WS_GROUP		   : 主にダイアログのラジオボタンのグループ設定に使う

//--- インクルード --------------------------------------------------------------------------------
#include	<stdlib.h>
#include	<stdio.h>

#include	<stddef.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<ctype.h>
#include	<stdarg.h>
#include	<string.h>
#include	<io.h>
#include	<fcntl.h>
#include	<direct.h>
#include	<windows.h>
#include	<windowsx.h>
#include	<time.h>
#include	<zmouse.h>						// WM_MOUSEWHEELをフックするために必要
#include	<imm.h>							// IME を禁止するのに必要
#include	<commctrl.h>					// コントロール
#include	<shlobj.h>
#include	<shlwapi.h>						// PathFileExists 用
#include	<process.h>						// スレッド

#ifdef	__MOVIE
	#include	<streams.h>
	#include	<atlbase.h>
#endif

#include	<d3d9.h>
#include	<d3dx9.h>

#define DWORD_PTR DWORD

#include	<dmusici.h>
#include	<dinput.h>
#include	<dshow.h>


//--- 置き換え ------------------------------------------------------------------------------------
// 変数
typedef	unsigned	int		ulong;
typedef	unsigned	int		uint;
typedef	unsigned	short	ushort;
typedef	unsigned	char	uchar;

typedef	D3DXVECTOR2		VEC2;				// ２Ｄベクトル
typedef	D3DXVECTOR3		VEC3;				// ３Ｄベクトル
typedef	D3DXVECTOR4		VEC4;				// ４Ｄベクトル
typedef	D3DXMATRIX		MTX;				// 行列 (D3DXMATRIX より高速に動作する)
typedef	D3DXQUATERNION	QTN;				// クォータニオン
typedef	D3DXPLANE		PLANE;				// 面
typedef	D3DMATERIAL9	MATERIAL;			// マテリアル
typedef	D3DXCOLOR		COL4;				// カラー
struct	COL									// ８ビットカラーの構造体
{
	uchar	r, g, b, a;
};

//--- 定数 ----------------------------------------------------------------------------------------
// ビット
#define	BIT0				1
#define	BIT1				2
#define	BIT2				4
#define	BIT3				8
#define	BIT4				16
#define	BIT5				32
#define	BIT6				64
#define	BIT7				128
#define	BIT8				256
#define	BIT9				512
#define	BIT10				1024
#define	BIT11				2048
#define	BIT12				4096
#define	BIT13				8192
#define	BIT14				16384
#define	BIT15				32768

//--- スイッチ ------------------------------------------------------------------------------------
#ifdef	__FULLSCREEN_2D_RESIZE
	#define	__2D_RESIZE
#endif
#ifdef	__2D_RESIZE
	#undef	__RENDER_FIXATION
	#define	__RENDER_FIXATION
#endif

#ifdef	__COLLISION
	#undef	__CALC
	#define	__CALC
#endif


//--- マクロいろいろ ------------------------------------------------------------------------------
#undef	SAFE_RELEASE
#define	SAFE_RELEASE(x)	{if(x){x->Release();x=NULL;}}

#undef	SAFE_DELETE
#define	SAFE_DELETE(p)	{if(p){delete(p); (p)=NULL;}}

#undef	SAFE_DELETE_ARRAY
#define	SAFE_DELETE_ARRAY(p) {if(p){delete[](p); (p)=NULL;}}

// 全角・半角判
#define	ISKANJI(c)	((uchar)(c)>=0x81 && (uchar)(c)<=0x9F || (uchar)(c)>=0xE0 && (uchar)(c)<=0xFC)

// 交換
#define	SWAP( __a_,	__b_)	{int   __c_; __c_ =	__a_; __a_ = __b_; __b_	= __c_;}
#define	SWAPF(__a_,	__b_)	{float __c_; __c_ =	__a_; __a_ = __b_; __b_	= __c_;}

// 乱数
#define	RAND(x)				(int)(	 rand()	% ((int)x))
#define	FRAND(x)			((float)(rand()	* (x)) / RAND_MAX)
#define	RAND2( a, b)		(((b) -	(a)) ==	0 ?	a :	(RAND((	b) - (a)) +	a))
#define	FRAND2(a, b)		(((b) -	(a)) ==	0 ?	a :	(FRAND((b) - (a)) +	a))

// a b 間の距離を c	の時間で移動するときの速度を返す
#define	VALUE_TO_SPEED(	____a, ____b, ____c)										\
		((____a) > (____b))	? (((____a)	- (____b)) / (____c)) :	(((____b) -	( ____a)) /	(____c))
		
// 値を目標値へ近づけるマクロ(a を b に c の速度で近づける)
#define	VALUE_TO_TARGET(____a, ____b, ____c)														\
		if(		(____a)	< (____b)){	(____a)	+= (____c);	if((____a) > (____b)) (____a) =	(____b);}	\
		else if((____a)	> (____b)){	(____a)	-= (____c);	if((____a) < (____b)) (____a) =	(____b);}

// 対象物との角度を得る
// 入力	p0		   = 元
//		p1		   = 先
// 出力	rho, pitch = 角度
inline	void	GET_ANGLE(float *yaw, float *pitch, VEC3 *p1, VEC3 *p0)
{
	VEC3	tmp	= *p1 -	*p0;
	D3DXVec3Normalize(&tmp,	&tmp);
	*yaw   = atan2f(tmp.x, tmp.z);
	*pitch = -tmp.y	* D3DX_PI /	2;
}

//--- システム用ワーク ----------------------------------------------------------------------------
struct	SYS_WORK
{
	char		strName[256];					// このゲームのキャプション
	char		strClass[32];					// クラス名
	bool		bActive;						// アクティブなら true
	bool		bMain;							// app_main	を実行中ならば true	※メニューでメイン処理を監視するのに使用
	char		cWindowStyle;					// ウィンドウスタイル(SIZE_MAXIMIZED or	SIZE_MINIMIZED or SIZE_RESTORED)
	char		cResizeCount;					// ウィンドウリサイズ用カウンター
	bool		bPause;							// システムを一時停止
	bool		bStartUp;						// true : システムの初期化中である

	int			iTimeCount;						// システムカウンター
	DWORD		dwTimeLast;						// 時間計測用
	DWORD		dwTimeMod;						// 時間計測用	余りをカウント
	bool		b30Frame;						// true なら30フレームになる
	bool		bFrameSkip;						// true	なら描画スキップ許可
	bool		bFramelateMessage;				// true	ならフレームレートをプロパティに表示゜

	#ifdef	__SHOW_FPS
		DWORD	dwTimeRate[TIME_RATE_MAX];
		int		iTimeRateCount;
	#endif

	HINSTANCE	hInst;
	HWND		hWnd;							// ウィンドウのハンドル
	HACCEL		hAccel;							// アクセラレーターテープルのハンドル
	HMODULE		hResource[SYS_RESOURCE_MAX];	// リソース
	HMENU		hMenu;							// メニューのハンドル
	bool		bMenu;							// true	: メニュー項目表示している
	bool		bScreenChange;					// スクリーンチェンジフラグ
	bool		bTopMost;						// メインループを非アクティブ時にも動作させ、スクリーンを常に前面に置く
	bool		bAlwayExec;						// メインループを非アクティブ時にも動作させる
	uchar		ucFileWatchReload;				// 0 != ファイル監視によるリロードが発生した	1 : テクスチャー、2 : メッシュ
	ushort		usFileWatchNum;					// ファイル監視によるリロードが発生したファイル番号

	bool		bMouseShow;						// マウスの状態
	int			iEscSkip;						// ESC キーをスキップする回数

	char		strExeFolder[		 MAX_PATH +	1];	// インストールフォルダー
	char		strFile[			 MAX_PATH +	1];	// プロジェクト名（フルパス）
	char		strSelFile[			 MAX_PATH +	1];	// 選択中のファイル名
	char		strFileBak[FILE_BAK][MAX_PATH +	1];	// 過去の使用ファイル

	bool		bAfterLoad;						// true : 即時読み込み

	//--- モニター関係 ----------------------------------------------
	int			iWidth,	iHeight;				// ウィンドウのサイズ(横のタブも含む)
	float		fXZoom,	fYZoom;					// ウィンドウのズーム値
	void		*funcResize;					// リサイズ時に呼び出される関数を登録するところ
	RECT		rectWindow;						// ウィンドウのサイズ(現在の値が入っている)
	RECT		rectWindowRestore;				// ウィンドウのサイズ(最大化前の値が入っています)
	bool		bPrimary;						// true : プライマリー画面にウィンドウが作られている

	//--- コマンドライン --------------------------------------------
	PSTR		szCmdLine;

	//--- エラー用 --------------------------------------------------
	bool		bError;							// true	: エラー発生
	char		strErr[2048];					// エラーメッセージ
	void		*funcError;						// エラー時に呼び出される関数を登録するところ
};


//--- 外部参照宣言 --------------------------------------------------------------------------------
	extern	SYS_WORK	sys;
	extern	bool		sys_error(char*	Format,...);	// エラーメッセージ表示関数
	extern	void		sys_printf(char* Format,...);	// システムメッセージ表示関数
	extern	bool		sys_file_select(int	sw,	char *s, char *f);
	extern	uint		sys_window_style_get(void);
	extern	void		sys_mouse_show(bool	bSW);
	extern	bool		sys_timestamp_get(FILETIME *pOut, char *strFile);
	extern	void		sys_file_watch_main(void);
	extern	void		sys_file_watch_init(void);
	extern	void		sys_file_watch_free(void);
	extern	int			sys_message_box(char *pstrMes, char *pstrCaption, UINT uiType);	// かならずウィンドウの中央に表示されるメッセージボックス
	// システムの一時休止設定
	extern	void		sys_pause(void);

//--- インクルード --------------------------------------------------------------------------------
	// ここから移動させてはいけません
	#ifdef	__AFONT
		#include	"ex\\_afont.h"
	#endif

	#ifdef	__D3
		#include	"system\\_d3.h"
		#include	"system\\_d3_mesh.h"
	#else
		struct	D3_WORK
		{
			LPDIRECT3D9				d;						// Direct3D	の元
			LPDIRECT3DDEVICE9		dev;					// デバイス
			bool					bPaintPresent;			// WM_PAINT 時に画像の転送を行う(初期値true);
			RECT					*rectRender;			// レンダリング時の転送先範囲を示す変数のポインタ(NULLなら全画面)
			bool					bWindow;				// ウィンドウモードなら	true
			bool					bWindowBak;				// __EXIT_MENU 有効時に使用されます
			short					sRenderW, sRenderH;		// レンダリングサイズ
			bool					bDeviceChange;			// true : デバイス変更中
		};

		extern	D3_WORK	d3;

		// ダミー
		inline	void		d3_screen_change(bool bStyle){;}

	#endif


//--- ウィンドウのキャプション --------------------------------------------------------------------
	/*------------------------------------------------------------------------------*
	| <<< ウィンドウのキャプション設定 >>>
	|	入力	printf と同じです
	*------------------------------------------------------------------------------*/
	extern	void	sys_caption_printf(char *Format, ...);
	#define	SYS_CAPTION_PRINTF	sys_caption_printf


//--- メニューの表示管理 --------------------------------------------------------------------------
	/*------------------------------------------------------------------------------*
	| <<< フルスクリーンのメニュー表示管理 >>>
	|	入力	bSw = true  : 表示オン
	|				= false : 表示オフ
	*------------------------------------------------------------------------------*/
	extern	void	sys_menu_view_set(bool bSw);
	#define	MENU_VIEW	sys_menu_view_set

	//--- メニュースイッチ用のマクロ -------------------------------------
	
	// ※チェックタイプの項目はこれを使うこと
	/*------------------------------------------------------------------------------*
	| <<< メニュー調べて、チェックをつける >>>
	|	入力	idm = メニュー ID
	|			a   = チェック時に実行するマクロ
	|			b   = チェックを外す時に実行するマクロ
	*------------------------------------------------------------------------------*/
	#define	MENU_CHECK(type, a,	b)		 												\
			{																			\
				if(GetMenuState(sys.hMenu, (type), MF_BYCOMMAND) & MF_CHECKED)			\
				{																		\
					CheckMenuItem(sys.hMenu, (type), MF_BYCOMMAND | MF_UNCHECKED);		\
					(b);																\
				}																		\
				else																	\
				{																		\
					CheckMenuItem(sys.hMenu, (type), MF_BYCOMMAND | MF_CHECKED);		\
					(a);																\
				}																		\
			}

	/*------------------------------------------------------------------------------*
	| <<< メニューにチェックをつける >>>
	|	入力	idm = メニュー ID
	*------------------------------------------------------------------------------*/
	#define	MENU_CHECKED(idm)		CheckMenuItem(sys.hMenu, (idm), MF_BYCOMMAND | MF_CHECKED);

	/*------------------------------------------------------------------------------*
	| <<< メニューのチェックをはずす >>>
	|	入力	idm = メニュー ID
	*------------------------------------------------------------------------------*/
	#define	MENU_UNCHECKED(idm)	CheckMenuItem(sys.hMenu, (idm), MF_BYCOMMAND | MF_UNCHECKED);

	/*------------------------------------------------------------------------------*
	| <<< メニューにチェックをつけたり外したり >>>
	|	入力	idm = メニュー ID
	|			sw  = true  : チェックをつける
	|				= false : チェックを外す
	*------------------------------------------------------------------------------*/
	extern	void	sys_menu_set(uint uiIDM, bool bSw);
	#define	MENU_SET	sys_menu_set

	/*------------------------------------------------------------------------------*
	| <<< メニューを無効にする >>>
	|	入力	idm = メニュー ID
	*------------------------------------------------------------------------------*/
	extern	void	sys_menu_disable(uint uiIDM);
	#define	MENU_DISABLE	sys_menu_disable

	/*------------------------------------------------------------------------------*
	| <<< メニューを有効にする >>>
	|	入力	idm = メニュー ID
	*------------------------------------------------------------------------------*/
	extern	void	sys_menu_enable(uint uiIDM);
	#define	MENU_ENABLE	sys_menu_enable

	/*------------------------------------------------------------------------------*
	| <<< メニューの文字列を変更する >>>
	|	入力	idm = メニュー ID
	|			str = 変更文字列
	*------------------------------------------------------------------------------*/
	extern	void	sys_menu_str_chage(uint uiIDM, char *str);
	#define	MENU_STR_CHANGE	sys_menu_str_chage

	/*------------------------------------------------------------------------------*
	| <<< メニューにセパレーターを追加 >>>
	*------------------------------------------------------------------------------*/
	extern	void	sys_menu_separator_add(void);
	#define	MENU_SEPARATOR_ADD	sys_menu_separator_add

	/*------------------------------------------------------------------------------*
	| <<< メニューに最近使ったファイルを追加 >>>
	*------------------------------------------------------------------------------*/
	extern	void	sys_menu_file_make(void);
	#define	MENU_FILE_MAKE	sys_menu_file_make

	/*------------------------------------------------------------------------------*
	| <<< メニューに最近使ったファイルを追加 >>>
	|	入力	__file = ファイル名
	|			b      = true  : ファイルを追加
	|				   = false : 同項目を削除
	*------------------------------------------------------------------------------*/
	extern	bool	sys_menu_file_add(char *__file,	bool b);
	#define	MENU_FILE_ADD	sys_menu_file_add


//--- ウエイト処理 --------------------------------------------------------------------------------
	/*------------------------------------------------------------------------------*
	| <<< タイマーリセット処理 >>>
	*------------------------------------------------------------------------------*/
	extern	void	sys_wait_reset(void);
	#define	SYS_WAIT_RESET	sys_wait_reset

	/*------------------------------------------------------------------------------*
	| <<< タイマー処理 >>>
	*------------------------------------------------------------------------------*/
	extern	void	sys_wait(void);
	#define	SYS_WAIT		sys_wait

	/*------------------------------------------------------------------------------*
	| <<< スリープ処理 >>>
	*------------------------------------------------------------------------------*/
	extern	void	sys_sleep(void);
	#define	SYS_SLEEP	sys_sleep


//--- 半角大文字を半角小文字へ --------------------------------------------------------------------
	/*------------------------------------------------------------------------------*
	| <<< strlwr と同じ >>>
	|	入力	src = 大文字を小文字へ
	|	出力	src = 変換文字列
	*------------------------------------------------------------------------------*/
	extern	void	sys_strlwr(char *src);
	#define	STRLWR	sys_strlwr


//--- ALT キー押下時にメニューが選択状態になるが、それを解除 --------------------------------------
	/*------------------------------------------------------------------------------*
	| <<< ALT 押下解除 >>>
	*------------------------------------------------------------------------------*/
	inline	void	SYS_ALT_KEY_OFF(void){ SendMessage(sys.hWnd, WM_SYSKEYUP, 0, 0L);}


//--- メッセージボックス --------------------------------------------------------------------------
	/*------------------------------------------------------------------------------*
	| <<< 終了メニュー >>>
	|	戻り値	true or false
	*------------------------------------------------------------------------------*/
	extern	bool	sys_exit_menu(void);
	#define	SYS_EXIT_MENU	sys_exit_menu

	/*------------------------------------------------------------------------------*
	| <<< はい、いいえを問う >>>
	|	入力	str = メッセージ
	|	戻り値	true or false
	*------------------------------------------------------------------------------*/
	extern	bool	sys_yes_no(char	*str);
	#define	SYS_YES_NO	sys_yes_no(char	*str)

	/*------------------------------------------------------------------------------*
	| <<< はい、いいえ、キャンセルを問う >>>
	|	入力	str = メッセージ
	|	戻り値	true or false
	*------------------------------------------------------------------------------*/
	extern	bool	sys_yes_no(char	*str);
	#define	SYS_YES_NO_CANCEL	sys_yes_no


//--- デスクトップのサイズを返す ------------------------------------------------------------------

	/*------------------------------------------------------------------------------*
	| <<< タスクバーを除くサイズですデスクトップのＸサイズを得る >>>
	*------------------------------------------------------------------------------*/
	extern	int		sys_desktop_xsize(void);
	#define	SYS_DESKTOP_XSIZE	sys_desktop_xsize

	/*------------------------------------------------------------------------------*
	| <<< タスクバーを除くサイズですデスクトップのＹサイズを得る >>>
	*------------------------------------------------------------------------------*/
	extern	int		sys_desktop_ysize(void);
	#define	SYS_DESKTOP_YSIZE	sys_desktop_xsize


//--- ウィンドウ処理 ------------------------------------------------------------------------------

	/*------------------------------------------------------------------------------*
	| <<< フレームのＸサイズを得る >>>
	|	入力	hWnd = ウィンドウハンドル
	*------------------------------------------------------------------------------*/
	extern	int		sys_xframe_get(void);
	extern	int		sys_xframe_get(HWND hWnd);
	#define	SYS_XFRAME_GET	sys_xframe_get

	/*------------------------------------------------------------------------------*
	| <<< フレームのＹサイズを得る >>>		※メインウィンドウ限定
	|	入力	hWnd = ウィンドウハンドル
	*------------------------------------------------------------------------------*/
	extern	int		sys_yframe_get(void);
	extern	int		sys_yframe_get(HWND hWnd);
	#define	SYS_YFRAME_GET	sys_yframe_get

	/*------------------------------------------------------------------------------*
	| <<< フレームのトップより、エッジ、メニュー、キャプションを引いた値を得る >>>
	*------------------------------------------------------------------------------*/
	extern	int		sys_topframe_get(void);
	#define	SYS_TOPFRAME_GET	sys_topframe_get

	/*------------------------------------------------------------------------------*
	| <<< ウィンドウサイズを設定する >>>
	*------------------------------------------------------------------------------*/
	extern	void	sys_window_size_set(void);
	#define	SYS_WINDOW_SIZE_SET	sys_window_size_set


//--- 最前面の処理	-------------------------------------------------------------------------------

	/*------------------------------------------------------------------------------*
	| <<< sys.bTopMost = true 時アプリを最前面する >>>
	*------------------------------------------------------------------------------*/
	extern	void	sys_window_topmost_set(void);
	#define	SYS_WINDOW_TOPMOST_SET	sys_window_topmost_set


//--- フォルダー操作 ------------------------------------------------------------------------------

	#define	__FOLDER_MAX	8					// 入れ子数
	#define	EXT_MAX			8					// 拡張子の総数(複数指定の場合は 8 個までの意味)

	/*----------------------------------------------------------------------*
	| <<< ファイル名をパスとファイル名に分割する >>>
	|	※	拡張子が無いとフォルダー名がファイル扱いになるので、
	|		拡張子がない場合は、末尾に\\を足してください。										
	|	入力	*strFile		= フルパスのファイル名
	|	出力	*strPathOut		= パス
	|			*strFileNameOut	= ファイル名
	|			*strExtOut		= 拡張子名
	*----------------------------------------------------------------------*/
	extern	void	sys_div_path(char *strPathOut, char	*strFileNameOut, char *strExtOut, char *strFile);

	/*----------------------------------------------------------------------*
	| <<< 現在のフォルダー位置を保存する >>>
	*----------------------------------------------------------------------*/
	extern	bool	sys_folder_push(void);

	/*----------------------------------------------------------------------*
	| <<< SysMakePath 実行前のパスへ戻る >>>
	*----------------------------------------------------------------------*/
	extern	bool	sys_folder_pop(void);

	/*------------------------------------------------------------------------------*
	| <<< フォルダー位置を設定 >>>
	|	入力	*strDir	= フォルダー名
	|			bErr	= true : エラーメッセージ表示
	|	戻り値	false :	フォルダー変更失敗
	*------------------------------------------------------------------------------*/
	extern	bool	sys_folder_set(char	*strDir, bool bErr);
	extern	bool	sys_folder_set(char	*strDir = NULL, bool bErr = false);
	#define	FOLDER_SET	sys_folder_set

	/*------------------------------------------------------------------------------*
	| <<< 現在のフォルダーを保存 >>>
	*------------------------------------------------------------------------------*/
	#define	FOLDER_PUSH()	sys_folder_push()

	/*------------------------------------------------------------------------------*
	| <<< FOLDER_PUSH で保存した情報を戻す >>>
	*------------------------------------------------------------------------------*/
	#define	FOLDER_POP()	sys_folder_pop()

	/*------------------------------------------------------------------------------*
	| <<< ディレクトリを得る >>>
	|	入力	lpszFolder	 = 調べるフォルダー名（絶対パス）
	|			lpszWildCard = ワイルドカード（*.txtとか）
	|			bSw			 = true	: サブフォルダーも調べる
	|	出力	strOut		 = 絶対パスを返す
	*------------------------------------------------------------------------------*/
	extern	void	sys_get_dir(char *strOut, LPCTSTR lpszFolder, LPCTSTR lpszWildCard,	bool bSw);

//--- ファイルデバッグ処理 ------------------------------------------------------------------------
	/*------------------------------------------------------------------------------*
	| <<< デバッグファイル削除 >>>
	*------------------------------------------------------------------------------*/
	extern	void	sys_debug_clear(void);
	#define	DEBUG_CLEAR	sys_debug_clear
	
	/*------------------------------------------------------------------------------*
	| <<< デバッグ情報出力 >>>
	|	入力	printf と同じフォーマット
	*------------------------------------------------------------------------------*/
	extern	void	sys_debug_printf(char *Format,...);
	#undef	DEBUG
	#define	DEBUG	sys_debug_printf



//--- 終了処理 ------------------------------------------------------------------------------------
	/*------------------------------------------------------------------------------*
	| <<< 強制終了 >>>
	*------------------------------------------------------------------------------*/
	extern	void	sys_end(char *Format, ...);
	#define	SYS_END	sys_end

	/*------------------------------------------------------------------------------*
	| <<< アプリに終了を通知する >>>
	*------------------------------------------------------------------------------*/
	extern	void	sys_exit(void);
	#define	SYS_EXIT	sys_exit


//--- ドラッグ＆ドロップ --------------------------------------------------------------------------

	/*------------------------------------------------------------------------------*
	| <<< ドラッグ＆ドロップ前処理 >>>	※sys.strSelFile にフルパスが入ります。
	|	入力	wParam = イベント時の引数
	|			strExt = 対象拡張子(.*とすれば全てのファイル)
	|			pProc  = 拡張処理(ファイルが複数の時使用のこと)
	|				   = NULL(未処理)
	|	出力	sys.strSelFile = フルパスが入ります。
	|	戻り値	true : ドロップ成功
	*------------------------------------------------------------------------------*/
	extern	bool	sys_drag_drop_begin(WPARAM wParam, char *strExt, void *pProc);
	extern	bool	sys_drag_drop_begin(WPARAM wParam, char	*strExt);
	#define	DRAG_DROP_BEGIN	sys_drag_drop_begin


//--- キー処理 ------------------------------------------------------------------------------------

	/*------------------------------------------------------------------------------*
	| <<< キー入力 >>>							※DirectInput不要
	*------------------------------------------------------------------------------*/
	extern	int	sys_key_get(int	iCode);


//--- 以下に *.h をインクルードしよう！	-----------------------------------------------------------

#include	"ctrl\\_ctrl.h"
#include	"ctrl\\_ctrl_d3.h"
#include	"ctrl\\_ctrl_debug.h"

#ifdef	__D3
	#include	"system\\_skin.h"
	#include	"system\\_shader.h"
#endif

#include	"system\\_pad.h"
#include	"system\\_snd.h"

#ifdef	__TSK
	#include	"ex\\_tsk.h"
	#include	"ex\\_tsk_move.h"
#endif
#ifdef	__NET
	#include	"ex\\_net.h"
#endif
#ifdef	__CAMERA
	#include	"ex\\_camera.h"
#endif
#ifdef	__MQO
	#include	"ex\\_mqo.h"
#endif
#ifdef	__MOVIE
	#include	"ex\\_movie_to_texture.h"
#endif
#ifdef	__PRINTF
	#include	"ex\\_printf.h"
#endif
#ifdef	__IME
	#include	"ex\\_ime.h"
#endif
#ifdef	__TABLET
	#include	"ex\\_tablet.h"
#endif
#ifdef	__CALC
	#include	"ex\\_calc.h"
#endif
#ifdef	__COLLISION
	#include	"ex\\_collision.h"
#endif
#ifdef	__MESH_BUF
	#include	"ex\\_mesh_buf.h"
#endif
#ifdef	__ASORT
	#include	"ex\\_asort.h"
#endif
#ifdef	__FONT_TO_TEXTURE
	#include	"ex\\_font_to_texture.h"
#endif
#ifdef	__FRUSTUM
	#include	"ex\\_frustum.h"
#endif

//--- 調整中 --------------------------------------------------------
#ifdef	__SCRIPT
	#include	"ex\\_script.h"
	#include	"ex\\_menu.h"
#endif
//-------------------------------------------------------------------

#ifdef	__VERSION
	#include	"ctrl\\_ctrl_version_dialog.h"
#endif

