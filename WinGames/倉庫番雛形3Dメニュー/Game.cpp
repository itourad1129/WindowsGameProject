//
//	草庫番雛形
//

#include	"main.h"


//--- 定数を定義 ----------------------------------------------------------------------------------

//--- ゲーム進行用変数 ----------------------------------------------------------------------------
int	iMode;			// ゲームモード
int iRound;			//　面
float	fTimeDat;	// 時間計測用です
//--- 変数を定義 ----------------------------------------------------------------------------------
#define	 TIME_MAX	(60 * 60 * 1)		// 60で１秒、その１秒60倍して１分
//--- 関数宣言 ------------------------------------------------------------------------------------
static void		GameStart(void);
static void		Game(void);
static void		Draw(void);
static void		Over(void);
static void		ClearCheck(void);
static void		MItemSet(int	iNum);


/*------------------------------------------------------------------------------*
| <<< ゲーム初期化 >>>
*------------------------------------------------------------------------------*/
void	game_init(void)
{
	MenuInit();
	//MenuAdd("ゲームを始める");
	//MenuAdd("説明を見る");

	MItemSet(0);				// タイトルのメニューを呼び出す
	
	FOLDER_SET("画像");
	d3_tex_load(TEX_MOJI,		"文字.png");
	//d3_tex_load(TEX_MAP,		"マップ.bmp");
	//d3_tex_load(TEX_PLAYER,		"キャラ.bmp");
	d3_tex_load(TEX_TITLE,		"タイトル.png");
	d3_tex_load(TEX_CLEAR,		"ラウンドクリアー.png");
	d3_tex_load(TEX_ALLCLEAR,	"オールクリアー.png");
	d3_tex_load(TEX_OVER,		"ゲームオーバー.png");
	d3_tex_load(TEX_INFO,		"説明.png");

	// 3D モデル読み込み
	FOLDER_SET("3Dモデル");
	d3_xfile_load(X_WALL,		 "iceblock.mqo");
	d3_xfile_load(X_MOKUTEKICHI, "ring.mqo");
	d3_xfile_load(X_NIMOTU,		 "ball.mqo");
	d3_xfile_load(X_PLAYER,		 "phoca.mqo");

	FOLDER_SET("サウンド");
	snd_load(SND_WALK,			"歩行.wav",				false);
	snd_load(SND_OUCH,			"壁にぶつかる.wav",		false);
	snd_load(SND_START,			"スタート.wav",			false);
	snd_load(SND_ATTAINMENT,	"目的地到達.wav",		false);
	snd_load(SND_CLEAR,			"ただのクリアー.wav",	false);
	snd_load(SND_UNDO,			"アンドゥ.wav",			false);
	snd_load(SND_TIMEUP,		"タイムアップ.wav",		false);
	snd_load(SND_GIVEUP,		"ギブアップ.wav",		false);
	snd_load(SND_ALLCLEAR,		"オールクリアー.wav",	false);

	// カメラ座標
	d3.vecCamera = VEC3(0, 10, -0.01f);
	// 注意座標
	d3.vecEye	 = VEC3(0,	0,		0);
	
	
	player = new cPlayer;
	
}



/*------------------------------------------------------------------------------*
| <<< ゲームメイン >>>
*------------------------------------------------------------------------------*/
void	game_main(void)
{
	
	d3_render_begin();
	PrintfInit();

	Camera();

	switch(iMode)
	{
	//--- タイトル --------------------------------------------------------------
	case MODE_TITLE:
		//		テクスチャー		次のモード　サウンド
		//Title(TEX_TITLE, &iMode, MODE_INFO, SND_START);
		if(TitleMenu(TEX_TITLE, -1, bMenuOk))
		{
			//--- メニューの項目別にジャンプ
			switch(iMenuSel)
			{
			case 0:				// ゲーム
				MapLoad(iRound);
				GameStart();
				iMode	 = MODE_GAME;
				fTimeDat = TIME_MAX;
				Undo(0);
				snd_play(SND_START);
				break;
			case 1:							//説明
				iMode = MODE_INFO;
				MItemSet(1);
				snd_play(SND_OUCH);
				break;
			case 2:							//終了
				SYS_EXIT();
				break;
			}
		}
			
		break;
	//--- 説明 -----------------------------------------------------------------
	case MODE_INFO:
		//Title(TEX_TITLE, &iMode, MODE_INFO, SND_START);
		if(TitleMenu(TEX_TITLE, -1, bMenuOk))
		{
			//--- メニューの項目別にジャンプ
			switch(iMenuSel)
			{
			case 0:				// ゲーム
				MapLoad(iRound);
				GameStart();
				iMode	 = MODE_GAME;
				fTimeDat = TIME_MAX;
				Undo(0);
				snd_play(SND_START);
				break;
			case 1:							//説明
				iMode = MODE_TITLE;
				MItemSet(-1);
				snd_play(SND_OUCH);
				break;
			}
		}
		break;
	//--- ゲーム中 --------------------------------------------------------------
	case MODE_GAME:
		Game();
		break;
	//--- クリアー -------------------------------------------------------------
	case MODE_CLEAR:
		if(Title(TEX_CLEAR, &iMode, MODE_GAME, SND_NONE))
		{
			iRound++;				// 次のラウンドへ進む
			GameStart();
			

		}
		break;
	
	//--- オールクリアー ------------------------------------------------------
	case MODE_ALLCLEAR:
		if(Title(TEX_ALLCLEAR, &iMode, MODE_GAME, SND_ALLCLEAR))
		{
			GameStart();
			iRound = 0;
			

		}
		break;
	//--- オーバー--------------------------------------------------------------
	case MODE_OVER:
		Title(TEX_TITLE, &iMode, MODE_TITLE, SND_START);
		break;
	}

	MenuMain(&VEC2(64, 300), 56);
	PrintfFlush();
	d3_render_end();
	
}

/*------------------------------------------------------------------------------*
| <<< メニューの項目設定 >>>
|	入力	iNum = 0 : タイトル、1 : メニュー
*-------------------------------------------------------------------------------*/
static	void	MItemSet(int iNum)
{
	switch(iNum)
	{
	//--- タイトル ------------------------------------------------------
	case 0:
		MenuAdd("ゲームを始める");
		MenuAdd("説明を見る");
		MenuAdd("終了する");
		break;
	//--- 説明 ----------------------------------------------------------
	case 1:
		MenuAdd("ゲームを始める");
		MenuAdd("タイトルへ戻る");
		bMenuCancelEnable = true;			// 右クリックでキャンセル可
		break;
	}
}

/*------------------------------------------------------------------------------*
| <<< ゲーム開始処理 >>>
*-------------------------------------------------------------------------------*/
static void GameStart(void)
{
	fTimeDat = TIME_MAX;		// 時間初期化
	
	player->Reset();
	
	MapLoad(iRound);

	Undo(UNDO_RESET);			// アンドゥリセット

}

/*------------------------------------------------------------------------------*
| <<< ゲーム処理 >>>
*------------------------------------------------------------------------------*/
static void Game(void)
{
	// ギブアップ
	if(pad.bKeyClick[KEY_ESC])
	{										// キーが押されると
		iMode = MODE_TITLE;					// ゲームモードを変更するので、タイトルへ戻ることができる
		snd_play(SND_GIVEUP);
		pad.bKeyClick[KEY_ESC] = false;		// この一行がないとゲームが終了してしまいます
		return;
	}

	fTimeDat -= 1.f;
	if(fTimeDat <= 0.f){ iMode = MODE_OVER; snd_play(SND_TIMEUP);}
	
	player->Main();		// プレイヤーメイン
	
	MapDraw();			// マップ描画

	player->Draw();		// プレイヤー描画

	ClearCheck();		// 面クリアー

	TimerDraw(fTimeDat, TIME_MAX);		// タイマー処理
										// ギブアップの表示
	Printf(		8, 460, 16, "ESC	:GIVE UP!");
	Printf(460, 460, 16, "ROUND%0d", iRound);
										// アンドゥ案内
	Printf(	8, 440, 16, "CTRL+Z:UNDO!");

	//アンドゥ
	if((pad.bKeyPush[KEY_LCTRL] || pad.bKeyPush[KEY_RCTRL]) && pad.bKeyClick[KEY_Z] && player->CheckStop())
	{
		Undo(UNDO_GET);
		snd_play(SND_UNDO);
	}

}

/*------------------------------------------------------------------------------*
| <<< 面クリアチェック >>>
*------------------------------------------------------------------------------*/
static		void	ClearCheck(void)
{
	for(int y = 0; y < MAP_H; y++)			// 縦にループ
	{
		for(int x = 0; x < MAP_W; x++)		// 横にループ
		{
			// x を見つけたら、ここで帰る
			if(chMap[ADR(x,y)] == 'x'){ return;}
		}
	}
	// x は荷物を運ぶ場所
	// 荷物の運ぶ場所に荷物があれば　x　は見つからない
	// 最後まで x が見つからなかったゆえにクリアー
	if(iRound != ROUND_MAX - 1){ iMode = MODE_CLEAR,    snd_play(SND_CLEAR 	 );}
	else					   { iMode = MODE_ALLCLEAR, snd_play(SND_ALLCLEAR);}
}
/*------------------------------------------------------------------------------*
| <<< ゲームオーバー >>>
*------------------------------------------------------------------------------*/
void	Over(void)
{
}
/*------------------------------------------------------------------------------*
| <<< ゲーム描画 >>>
*------------------------------------------------------------------------------*/
void	Draw(void)
{
}
/*------------------------------------------------------------------------------*
| <<< ゲーム終了 >>>
*------------------------------------------------------------------------------*/
void	game_end(void)
{
}


