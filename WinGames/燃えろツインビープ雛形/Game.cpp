//
//	燃えろツインビープ雛型
//

#include	"main.h"


//--- 変数定義 ------------------------------------------------------------------------------------
GAME_WORK	game;


/*------------------------------------------------------------------------------*
| <<< ゲーム初期化 >>>
*------------------------------------------------------------------------------*/
void	game_init(void)
{
	//--- 画像読み込み ---------------------------------------------
	FOLDER_SET("画像");
	d3_tex_load(TEX_MOJI,		"文字.png");
	d3_tex_load(TEX_TITLE,		"タイトル.png");
	d3_tex_load(TEX_INFO,		"説明.png");
	d3_tex_load(TEX_OVER,		"ゲームオーバー.png");
	d3_tex_load(TEX_PARTICLE,	"パーティクル.png");

	//--- ３Dモデル読み込み ----------------------------------------
	FOLDER_SET("3Dモデル");
	d3_xfile_load(X_PLAYER,	"プレイヤー.mqo");
	d3_xfile_load(X_METEO,	"隕石.mqo");
	d3_xfile_load(X_BELL,	"ボーナス.mqo");
	d3_xfile_load(X_STAR,	"星.mqo");

	//--- 音楽 ＆ 効果音 -------------------------------------------
	FOLDER_SET("サウンド");
	snd_load(SND_DAMAGE, "ダメージ.wav", false);	// 死亡
	snd_load(SND_START,	 "スタート.wav", false);	// 開始
	snd_load(SND_BGM,	 "音楽.wav",	 true);		// BGM(ループ再生するので true)
	snd_load(SND_BELL,	 "ベル.wav",	 false);	// ベルゲット
	snd_load(SND_MOVE,	 "移動.wav",	 false);	// プレイヤー移動

	//--- カメラ設定 -----------------------------------------------
	//d3.vecCamera		= VEC3(0, 0.0f, -10.0f);
	//↓
	d3.vecCamera	= VEC3(0,-10.0f, -5.0f);
	//--- 以下、パーティクルの時に必要になります。--------------------
	float	tmp;
	D3DXMatrixInverse(&d3.mtxViewInverse, &tmp, &d3.mtxView);
	d3.mtxBillboard = d3.mtxViewInverse;

	//--- いろいろ初期化 -------------------------------------------
	ParticleInit();									// パーティクル初期化
	StarInit();
	MeteoInit();
	BellInit();
	ParticleInit();
	AfterimageInit();
}


/*------------------------------------------------------------------------------*
| <<< ゲームメイン >>>
*------------------------------------------------------------------------------*/
void	game_main(void)
{
	d3_render_begin();
	PrintfInit();			// Printf クリアー
	Camera();				// カメラ

	switch(game.iMode)
	{
	//--- タイトル -------------------------------------------------------------
	case MODE_TITLE:
		Title(TEX_TITLE, &game.iMode, MODE_INFO, SND_START);
		break;

	//--- 説明 ---------------------------------------------------------------
	case MODE_INFO:
		if(Title(TEX_INFO, &game.iMode, MODE_GAME, SND_BGM))
		{
			PlayerInit();			// プレイヤー初期化
			
		}
		break;

	//--- ゲーム -------------------------------------------------------------
	case MODE_GAME:
		PlayerMain();				// プレイヤーメイン
		PlayerDraw();				// プレイヤー描画
		StarMain();
		StarDraw();
		MeteoMain();
		MeteoDraw();
		BellMain();
		BellDraw();
		ParticleMain();
		AfterimageMain();
		AfterimageDraw();
		CameraMoveInit();
		CameraMoveMain();
		
		if(player->m_iHp <= 0){ game.iMode = MODE_OVER; snd_stop(SND_BGM);}
		break;

	//--- ゲームオーバー -----------------------------------------------------
	case MODE_OVER:
		Title(TEX_OVER, &game.iMode, MODE_TITLE, SND_NONE_);
		break;
	}
	PrintfFlush();							// Printf 描画
	d3_render_end();
}



/*------------------------------------------------------------------------------*
| <<< ゲーム終了 >>>
*------------------------------------------------------------------------------*/
void	game_end(void)
{
}


