
#ifndef	__TABLET_
#define	__TABLET_

//
//	タブレット処理
//


#include	<wintab.h>


#define	TABLET_PRESSURE_RESOLUTION	1024			// 筆圧の分解能

struct	TABLET_WORK						// タブレットワーク
{
	bool	bPush;							// true : タブレットにペンがついている
	int		iX, iY;							// タブレットのＸＹ座標
	int		iPressure;						// 筆圧

	// 以下、カット
	//bool	bClick;							// true : タブレットにペンをつけた瞬間
	//bool	bRelease;						// true : タブレットからペンを離した瞬間
	//bool	bTabletBak;						// bTabletClick 計算用
	//int	iTabletMoveX, iTabletMoveY;		// ペンの移動量
	//int	iTabletXBak,  iTabletYBak;		// iTabletMoveX, iTabletMoveY 計算用
};


extern	TABLET_WORK	tablet;


/*------------------------------------------------------------------------------*
| <<< タブレット初期化 >>>
|	戻り値	タブレットコンテクストのハンドル
*------------------------------------------------------------------------------*/
extern	HCTX	TabletInit(void);



/*------------------------------------------------------------------------------*
| <<< タブレットのイベント >>>
|	入力	hWnd		   = ウィンドウハンドル
|			uiMsg		   = メッセージ
|			wParam, lParam = パラメーター
*------------------------------------------------------------------------------*/
extern	void	TabletProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);



//extern	HCTX	TabletInit2(void);
//extern	void	TabletReceive(WPARAM wParam, LPARAM lParam);
//extern	void	TabletMouse(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);


#endif	// __TABLET_

