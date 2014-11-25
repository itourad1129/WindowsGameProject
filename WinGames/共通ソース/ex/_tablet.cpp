//
//	タブレット処理
//
//
//	タブレット処理には タブレットドライバー『Wintab32.dll』が必要になります。
//	タブレットが動作しない環境下で、タブレット処理を呼び出すとエラーが発生します。
//	『Wintab32.dll』が『Widows\\System32』または、実行ファイルと同じフォルダーに置けばエラーは回避できます。
//	『Wintab32.dll』は再配布可能な DLL ファイルです。


//#include	"main.h"


//--- 定数定義 ------------------------------------------------------------------------------------
#define	PACKETDATA		(PK_CURSOR | PK_X | PK_Y | PK_BUTTONS | PK_NORMAL_PRESSURE | PK_ORIENTATION)
#define	PACKETMODE		PK_BUTTONS
#include	<pktdef.h>


//--- ライプラリィ --------------------------------------------------------------------------------
#pragma comment(lib,"wintab32.lib")
#pragma comment(lib,"wntab32x.lib")

//--- 変数定義 ------------------------------------------------------------------------------------
#define	NPACKETQSIZE	32			// タブレットのデータバッファサイズ

static	HCTX	hctxTablet;
static	PACKET	pacektBuf[NPACKETQSIZE];	// パケット受信用
static	POINT	ptNew;						// ペンの座標値格納用
static	POINT	ptOld;						// 直前の座標値格納用

		TABLET_WORK	tablet;					// タブレットワーク
static	TABLET_WORK	*tb = &tablet;




/*------------------------------------------------------------------------------*
| <<< タブレット初期化 >>>
|	※ マウスイベントに連動するタイプ
|	　 タブレットのイベントは、マウスのイベントされる。
|	   タブレットの情報は WndProc のマウスイベントより得られる
*------------------------------------------------------------------------------*/
HCTX	TabletInit(void)
{

	// デフォルト設定の取得
	LOGCONTEXT	lcMine;
	WTInfo(WTI_DEFSYSCTX, 0, &lcMine);
  
	// 設定値の修正
	wsprintf(lcMine.lcName, "tablet program %x", GetCurrentProcessId());
	lcMine.lcPktData   = PACKETDATA;
	lcMine.lcPktMode   = PACKETMODE;
	lcMine.lcMoveMask  = PACKETDATA;
	lcMine.lcBtnUpMask = lcMine.lcBtnDnMask;

	// マウスモード用の設定
	lcMine.lcOutOrgX = 0;
	lcMine.lcOutExtX = GetSystemMetrics(SM_CXSCREEN);
	lcMine.lcOutOrgY = 0;
	lcMine.lcOutExtY = -GetSystemMetrics(SM_CYSCREEN);
  
	// パケット受信開始
	hctxTablet = WTOpen(sys.hWnd, &lcMine, TRUE);

	// 受信バッファの設定(本来は「バッファあふれのエラー処理が必要」だそうです)
	WTQueueSizeSet(hctxTablet, NPACKETQSIZE);

	return	hctxTablet;
}



/*------------------------------------------------------------------------------*
| <<< タブレットのイベント >>>
|	入力	hWnd		   = ウィンドウハンドル
|			uiMsg		   = メッセージ
|			wParam, lParam = パラメーター
*------------------------------------------------------------------------------*/
void	TabletProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{

	switch(uiMsg)
	{
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
		// 最小化時は処理しない
		if(IsIconic(hWnd))
		{
			break;
		}
		else
		{
			// タブレットの情報を得る(ちゃんと iMax 回処理しよう)
			int	iMax = WTPacketsGet(hctxTablet, NPACKETQSIZE, &pacektBuf);
			for(int i = 0; i < iMax; i++)
			{
				// パケットの個数分処理をしなければならないのだか、簡略化のため、最後のパケットのみにする。
				//i--;

				// 前回のデータ保持
				ptOld  = ptNew;

				// 新しいパケットの保持
				ptNew.x = (UINT)pacektBuf[i].pkX;
				ptNew.y = (UINT)pacektBuf[i].pkY;

				//--- マルチモニター用にディスプレイ位置を毎度計算する --------
				POINT			ptWindow ={      0,       0};
				POINT			ptIn    = {ptNew.x, ptNew.y};
				GetCursorPos(&ptIn);
				HMONITOR		hMonitor = MonitorFromPoint(ptIn, MONITOR_DEFAULTTONEAREST);
				MONITORINFOEX	stMonInfoEx;
				stMonInfoEx.cbSize = sizeof(stMonInfoEx);
				GetMonitorInfo(hMonitor, &stMonInfoEx);
				ClientToScreen(hWnd, &ptWindow);
				//-------------------------------------------------------------

				// クライアント座標への変換
				tb->iX = ptIn.x - ptWindow.x;
				tb->iY = ptIn.y - ptWindow.y;

				// クリック情報
				//tb->bClick = tb->bPush ^ tb->bTabletBak;
				//tb->bTabletBak   = tb->bPush;

				// 押下情報
				if(     HIWORD(pacektBuf[i].pkButtons) == TBN_DOWN){ tb->bPush = true; }
				else if(HIWORD(pacektBuf[i].pkButtons) == TBN_UP  ){ tb->bPush = false;}

				// 離した情報
				//tb->bRelease = tb->bPush ^ tb->bTabletBak;

				// タブレット座標、筆圧、移動量
				//tb->iTabletXBak     = tb->iX;						// 座標
				//tb->iTabletYBak     = tb->iY;

				tb->iPressure = pacektBuf[i].pkNormalPressure;	// 筆圧

				//tb->iTabletMoveX    = tb->iTabletXBak - tb->iX;	// 移動量
				//tb->iTabletMoveY    = tb->iTabletYBak - tb->iY;
			}
		}
		break;
	}
}



#if 0

	// タブレットがウィンドウを乗っ取るタイプの処理。
	// マウスカーソルと連動しないので、ちょっと不便かも？
	// 分解能は上記のものより優れています。

	static	long	lcOutExtX, lcOutExtY;

	/*------------------------------------------------------------------------------*
	| <<< タブレット初期化 >>>
	*------------------------------------------------------------------------------*/
	HCTX	TabletInit2(void)
	{

		LOGCONTEXT	lcMine;
  
		// WTInfoを使用してデフォルト設定値を取得
		WTInfo(WTI_DEFCONTEXT, 0, &lcMine);
  
		// 必要な項目を設定
		wsprintf(lcMine.lcName, "WinTab test %x", GetCurrentProcessId());
		lcMine.lcOptions  |= CXO_MESSAGES;  // WTメッセージが渡されるようにする
		lcMine.lcPktData   = PACKETDATA;
		lcMine.lcPktMode   = PACKETMODE;
		lcMine.lcMoveMask  = PACKETDATA;
		lcMine.lcBtnUpMask = lcMine.lcBtnDnMask;

		// タブレットの解像度設定 
		// タブレットドライバのスケーリング設定に関係なくタブレットの分解能そのものを取り込む
		// デフォルト設定値はドライバで設定されているスケーリングになっている
		lcMine.lcOutOrgX = lcMine.lcOutOrgY = 0;
		lcMine.lcOutExtX = lcMine.lcInExtX;
		lcMine.lcOutExtY = lcMine.lcInExtY;
  
		// タブレットサイズを格納しておく
		lcOutExtX = lcMine.lcOutExtX;
		lcOutExtY = lcMine.lcOutExtY;
  
		// パケット受信開始
		return WTOpen(sys.hWnd, &lcMine, TRUE);
	}


	/*------------------------------------------------------------------------------*
	| <<< タブレットのデータ受信イベント >>>
	*------------------------------------------------------------------------------*/
	void	TabletReceive(WPARAM wParam, LPARAM lParam)
	{

		//--- WndProc に以下を入れてください ------------------------
		//	case WT_PACKET:{ TabletReceive(wParam, lParam); break;}

		PACKET	pkt;

		if(WTPacket((HCTX)lParam, wParam, &pkt))
		{
			// クリック情報
			tb->bClick = tb->bPush ^ tb->bTabletBak;
			tb->bTabletBak   = tb->bPush;

			// 押下情報
			if(     HIWORD(pkt.pkButtons) == TBN_DOWN){ tablet.bPush = true; }
			else if(HIWORD(pkt.pkButtons) == TBN_UP  ){ tablet.bPush = false;}
			
			// タブレット座標、筆圧、移動量
			tb->iTabletXBak     = tb->iX;
			tb->iTabletYBak     = tb->iY;

			tb->iX        =			    (int)(((float)pkt.pkX / (float)lcOutExtX) * (float)d3.sRenderW);
			tb->iY        = d3.sRenderH - (int)(((float)pkt.pkY / (float)lcOutExtY) * (float)d3.sRenderH);

			tb->iTabletPressure = pkt.pkNormalPressure;				// 筆圧

			tb->iTabletMoveX    = tb->iTabletXBak - tb->iX;	// 移動量
			tb->iTabletMoveY    = tb->iTabletYBak - tb->iY;
		}
	}
#endif
