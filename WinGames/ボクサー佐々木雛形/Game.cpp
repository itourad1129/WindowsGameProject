//
//	ボクサー佐々木雛型
//

#include	"main.h"


//--- 変数を定義 ----------------------------------------------------------------------------------

GAME_WORK	game;

/*------------------------------------------------------------------------------*
| <<< ゲーム初期化 >>>
*------------------------------------------------------------------------------*/
void	game_init(void)
{
	//--- テクスチャー読み込み ----------------------------------------
	FOLDER_SET("画像");
	d3_tex_load(TEX_MOJI, "文字.png");

	//--- X ファイル読み込み ----------------------------------------------
	FOLDER_SET("3Dモデル");
	skin_load(	X_ENEMY, "enemy.x");
	d3_xfile_load(X_LEFT,	"punch.mqo");
	d3_xfile_load(X_RIGHT,	"punch.mqo");

	//--- テクスチャー読み込み ---------------------------------------------
	FOLDER_SET("画像");
	d3_tex_load(TEX_BACK,	"リング.jpg");
	d3_tex_load(TEX_KO_BAR,	"ko.png");
	d3_tex_load(TEX_TITLE,	"タイトル.jpg");
	d3_tex_load(TEX_INFO,	"説明.jpg");
	d3_tex_load(TEX_WIN,	"勝利.jpg");
	d3_tex_load(TEX_LOSE,	"敗北.jpg");

	//--- 効果音読み込み ---------------------------------------------------
	FOLDER_SET("サウンド");
	snd_load(SND_PUNCH,		"パンチ.wav",				false);
	snd_load(SND_GUARD,		"ガード.wav",				false);
	snd_load(SND_PLY_DAMAGE,"プレイヤーダメージ.wav",	false);
	snd_load(SND_ENE_DAMAGE,"エネミーダメージ.wav",		false);
	snd_load(SND_WIN_LOSE,	"試合終了.wav",				false);
	snd_load(SND_OK,		"ゴング.wav",				false);

	//--- カメラ位置 -------------------------------------------------------
	d3.vecCamera = VEC3(0, 15, -10);
	d3.vecEye	 = VEC3(0, 15,   0);

	d3.light.Direction.x = 0;
	d3.light.Direction.y = 0;
	d3.light.Direction.z = -d3.vecCamera.z;
}

/*-----------------------------------------------------------------*
| <<< テスト >>>
*-----------------------------------------------------------------*/
void	Test(void)
{
	//--- アニメーション ------------------------------------------
	skin_frame_set(X_ENEMY, 1);			//1 は再生速度

	//--- アニメーション切り替え ----------------------------------
	if(pad.bKeyClick[KEY_NUM0]){ skin_anim_set(X_ENEMY, 0, 30);}
	if(pad.bKeyClick[KEY_NUM1]){ skin_anim_set(X_ENEMY, 1, 30);}
	if(pad.bKeyClick[KEY_NUM2]){ skin_anim_set(X_ENEMY, 2, 30);}
	if(pad.bKeyClick[KEY_NUM3]){ skin_anim_set(X_ENEMY, 3, 30);}
	if(pad.bKeyClick[KEY_NUM4]){ skin_anim_set(X_ENEMY, 4, 30);}
	if(pad.bKeyClick[KEY_NUM5]){ skin_anim_set(X_ENEMY, 5, 30);}

	//--- 描画 ----------------------------------------------------
	MTX		mtxWorld, mtxScale, mtxRot, mtxTrans;

	D3DXMatrixScaling(		&mtxScale, 25);		// 拡大率は各自で設定のこと
												// 必要なでれば、座標も設定のこと
	D3DXMatrixTranslation(&mtxTrans, &VEC3(0, 0, 55));
	mtxWorld = mtxScale * mtxTrans;
	skin_draw(X_ENEMY, &mtxWorld);
}

/*-----------------------------------------------------------------*
| <<< ヒットポイント表示 >>>
|	入力	iNum = 0 : プレイヤー、 1 : エネミー
|			fLen = バーの長さ(0.0f ～ 1.0f )
*-------------------------------------------------------------------*/
void	HitPointDraw(int	iNum, float fLen)
{
	d3_stat_zwrite_set(false);
	d3_stat_zread_set( false);

	//--- HP描画 ----------------------------------------------------
	VEC2	p0, p1;
	float	a = 264.f;
	if(iNum == 0){ p0 = VEC2(  8, 8); p1 = VEC2(  8 + a, 40);}
	else		 { p0 = VEC2(368, 8); p1 = VEC2(368 + a, 40);}
	d3_2d_box_fill_draw(&p0, &p1, D3_COL(255, 255, 255, 0));// 下地
	p0.x += 1, p1.x += -1;									// 一回り小さくする
	p0.y += 1, p1.y += -1;
	if(iNum == 0){ p0.x += (a - a * fLen);}					// プレイヤー
	else		 { p1.x -= (a - a * fLen);}					// エネミー
	d3_2d_box_fill_draw(&p0, &p1, D3_COL(255, 255, 255, 0)); // ライフはボックスで

	//--- KO描画 -----------------------------------------------------
	if(iNum == 1)						// エネミーの時のみKO描画
	{
		d3_spr_draw(TEX_KO_BAR, NULL, &VEC3(320, 24, 0));
	}
	d3_stat_zread_set( true);
	d3_stat_zwrite_set(true);
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
		Title(TEX_TITLE, &game.iMode, MODE_INFO, SND_OK);
		break;

	//--- 説明 ---------------------------------------------------------------
	case MODE_INFO:
		if(Title(TEX_INFO, &game.iMode, MODE_GAME, SND_OK))
		{
			PlayerInit();			// プレイヤー初期化
			EnemyInit();
			
		}
		break;

	//--- ゲーム -------------------------------------------------------------
	case MODE_GAME:
		PlayerMain();				// プレイヤーメイン
		PlayerDraw();				// プレイヤー描画
		EnemyMain();
		EnemyDraw();
		HitPointDraw(0, player->m_fHP / HP_MAX);
		HitPointDraw(1, enemy->m_fHP / HP_MAX);
		break;

	//--- ゲームオーバー -----------------------------------------------------
	case MODE_WIN:
		Title(TEX_WIN, &game.iMode, MODE_TITLE, SND_WIN_LOSE);
		break;

	//case MODE_LOSE:
		//Title(TEX_LOSE, &game.iMode, MODE_TITLE, SND_WIN_LOSE);
		//break;
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


