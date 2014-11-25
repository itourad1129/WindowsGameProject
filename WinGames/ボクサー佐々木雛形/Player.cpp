//
//	プレイヤー
//

#include	"main.h"

//--- 定数定義 ---------------------------------------------------------------------------------
#define PUNCH_TIME				180.f				// パンチの表示時間
#define PUNCH_SPEED				20.f				// パンチの速度
#define PUNCH_Z					40.f				// パンチのZ移植値
#define PUNCH_Y					12.f				// パンチのY座標
#define PUNCH_X					4.f					// パンチのX移植値
#define PUNCH_XOFS				-12.f				// パンチの角度
#define PUNCH_ANGLE				R(30)				// パンチの角度

#define COUNTER_START_TIME		5			// カウンターは enemy->m_iAnimCount が COUNTER_START_TIME
#define COUNTER_END_TIME		(enemy->m_fAttackTime - 5)
#define COUNTER_A_POWER			5					// カウンター時の攻撃力

//--- 変数 -------------------------------------------------------------------------------------
cPlayer *player;									// プレイヤーワーク

/*--------------------------------------------------------------------------------*
| <<< パンチ >>>
*---------------------------------------------------------------------------------*/
void	cPlayer::Punch(void)
{
	for(int i = 0; i < PLAYER_PUNCH_MAX; i++)
	{//一回目 左
												//二回目 右
		//--- パンチを出す処理 -----------------------------------------
		//
		int	aiKey[] = {KEY_LCTRL, KEY_RCTRL};

		//　↓キー入力があり、			↓パンチしてなく、			↓防御してない時にパンチが打てる
		if(pad.bKeyClick[aiKey[i]] && m_afPunchTime[i] == 0 && m_fGuardTime == 0)
		{
			m_afPunchTime[i] = PUNCH_TIME;		// パンチ時間に値を入れたら開始
			snd_play(SND_PUNCH);
		}

		//--- パンチを戻す処理 ------------------------------------------
		VALUE_TO_TARGET(m_afPunchTime[i], 0, PUNCH_SPEED);
		//--- ダメージ計算 ----------------------------------------------
		if(m_afPunchTime[i] > 0)				// パンチのアニメーションを進める。PUNCH_TIME が 0 になるまで。
		{										// 20は速度(大きくすると速くなる)
			CalcEnemyDamage(i);					// ここでダメージ計算
		}
	}
}

/*-----------------------------------------------------------------------------------------*
| <<< エネミーへ与えるダメージ値計算 >>>
|	入力	iID = 0 : 左パンチ
|				= 1 : 右パンチ
*-----------------------------------------------------------------------------------------*/
void	cPlayer::CalcEnemyDamage(int iID)
{
	//						↓ここには自分で調べたエネミーにパンチが当たる値をいれてください。
	if(m_afPunchTime[iID] != 120){ return;}		// パンチが 指定の値の時のみ判定

	bool	bCounter = false;					// カウンターフラグ

	switch(enemy->m_iMode)
	{
	//--- パンチ ----------------------------------------------------------------------
	case ENE_RIGHT_PUNCH:
		if(iID == 0){ bCounter = true;}			// エネミー右パンチにはプレイヤー左
		break;
	case ENE_LEFT_PUNCH:						// エネミー左パンチにはプレイヤー右
		if(iID == 1){ bCounter = true;}			// でカウンター発動
		break;
	}

	//--- カウンターの場合、時間を計測してカウンターが入るか調べる --
	if(bCounter && enemy->m_iAnimCount >= COUNTER_START_TIME  && enemy->m_iAnimCount <= COUNTER_END_TIME)		// 有効とする
	{
		enemy->DamageSet(COUNTER_A_POWER);			// エネミーにダメージを与える
	}
}








/*---------------------------------------------------------------------------------*
| <<< 描画 >>>
*----------------------------------------------------------------------------------*/
void	cPlayer::Draw(void)
{
	d3_stat_alpha_set(true);			// 半透明オン

	for(int i = 0; i < PLAYER_PUNCH_MAX; i++) // パンチはループを使う
	{
		//--- グローブ描画 ------------------------------------------
		float	z =				 sinf(R(m_afPunchTime[i])) * PUNCH_Z;
		float	x =	PUNCH_XOFS + sinf(R(m_fGuardTime	)) * PUNCH_X;
		float	r = PUNCH_ANGLE;
		if(i == 1){ x *= -1, r *= -1;}		// 右パンチは左右反転

		MTX		mtxWorld, mtxRot, mtxTrans;
		D3DXMatrixRotationY(	&mtxRot,	r);
		D3DXMatrixTranslation(&mtxTrans, &VEC3(x, PUNCH_Y, z));
		mtxWorld = mtxRot * mtxTrans;
		d3_3d_draw(X_LEFT + i, &mtxWorld, 0.8f);// グローブは半透明にする
	}
}
//--- 一括処理 -------------------------------------------------------------------------------------
/*--------------------------------------------------------------------------------*
| <<< プレイヤー初期化 >>>
*---------------------------------------------------------------------------------*/
void	PlayerInit(void)
{
	// メモリ所得
	if(player == NULL){ player = new cPlayer;}

	// 初期化
	for(int i = 0; i < PLAYER_PUNCH_MAX; i++)
	{
		player->m_afPunchTime[i] = 0;		// パンチのアニメーション時間クリア
	}
	player->m_fGuardTime = 0;				// ガード時間
	player->m_fHP		 = HP_MAX;			// 体力
}

/*-------------------------------------------------------------------------------*
| <<< プレイヤーメイン >>>
*--------------------------------------------------------------------------------*/
void	PlayerMain(void)
{
	player->Punch();			// パンチ
}

/*-------------------------------------------------------------------------------*
| <<< プレイヤー描画 >>>
*--------------------------------------------------------------------------------*/
void	PlayerDraw(void)
{
	player->Draw();
}

