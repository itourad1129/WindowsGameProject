//
// アプリケーションの諸設定
//


#define	__SEPARATOR			1		// ファイルメニューのセパレーター位置
#define	__NEAR			    0.01f	// 手前のクリッピング領域
#define	__FAR				1000.f	// 奥のクリッピング領域
									// 頂点シェーダーのバージョン
#define	__VS_VERSION		D3DVS_VERSION(2, 0)
									// ピクセルシェーダーのバージョン
#define	__PS_VERSION		D3DPS_VERSION(2, 0)


#define	__SCR_W				640		// ウィンドー初期サイズ
#define	__SCR_H				480
#define	__RENDER_X0			  0		// レンダリング範囲
#define	__RENDER_Y0			  0
#define	__RENDER_X1			640
#define	__RENDER_Y1			480

#define	__SAVE_WINDOW				// ｳｨﾝﾄﾞｰ位置保存
#define	__WINDOW					// ウィンドーモードで起動
#define	__WINCURSOR					// ウィンドー内マウス表示
#define	__ESC_TO_EXIT				// ESC でアプリ終了
#define	__2D_RESIZE					// __SCR_W,__SCR_H でリサイズする

//--- ライブラリィ --------------------------------------------------------------------------------
#define	__D3						// Direct3D 使用(必ず有効にしてください)
#define	__PAD						// キーボード、ジョイパット使用(マウスは別)
#define	__SND						// サウンド使用
#define	__MQO						// メタセコイヤ用ライブラリィ
#define	__CALC						// 算術ライブラリィ
#define	__COLLISION					// 衝突判定ライブラリィ
#define	__SKIN						// スキンメッシュライブラリィ

//--- デバッグモード時のスイッチ ------------------------------------------------------------------
#ifdef	_DEBUG
	#define	__ERROR_MES_OUTPUT		// エラーメッセージを出力へ表示
	#define	__SYSTEM_DEBUG			// システムデバッグモード(TABで早送り、1 でストップ、2 で一コマ進む)
	#define	__SHOW_FPS				// FPS 表示
	#define	__HIT_FRAME				// ヒット枠描画

//--- リリースモード時のスイッチ ------------------------------------------------------------------
#else
	#define	__EXIT_MENU				// 終了確認表示
#endif


//--- インクルード --------------------------------------------------------------------------------
#include	"resource.h"
#include	"_sys.h"
#include	"includes.h"
