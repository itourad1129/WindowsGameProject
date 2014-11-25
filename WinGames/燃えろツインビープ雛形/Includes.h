
//--- アプリ名 ------------------------------------------------------------------------------------
#define	APP_NAME	"燃えろツインビープ"			// アプリ名
#define	VERSION		"ver1.00"						// バージョン情報

//--- ゲームモード --------------------------------------------------------------------------------
enum	MODE_ID
{
	MODE_TITLE,										// タイトル
	MODE_INFO,										// 説明
	MODE_GAME,										// ゲーム中
	MODE_OVER,										// ゲームオーバー
};
//--- Xファイル ------------------------------------------------------------------------------------
enum	MESH_ID
{
	X_PLAYER,										// プレイヤー
	X_METEO,										// 隕石
	X_BELL,											// ベル
	X_STAR,											// 星
};
//--- サウンド番号 --------------------------------------------------------------------------------
enum	SND_ID
{
	SND_START,										// スタート音
	SND_DAMAGE,										// ダメージ音
	SND_BGM,										// BGM
	SND_MOVE,										// 移動
	SND_BELL,										// ベル
};
//--- テクスチャー --------------------------------------------------------------------------------
enum	TEX_ID
{
	TEX_MOJI,										// 文字
	TEX_TITLE,										// タイトル
	TEX_INFO,										// 説明
	TEX_OVER,										// ゲームオーバー
	TEX_PARTICLE,									// パーティクル
};

//--- インクルードファイル ------------------------------------------------------------------------
#include	"Game.h"
#include	"Printf.h"
#include	"Camera.h"
#include	"Title.h"
#include	"Particle.h"
#include	"Player.h"
#include	"Star.h"
#include	"Meteo.h"
#include	"Afterimage.h"
#include	"Bell.h"
#include	"CameraMove.h"
