//
//	エネミー
//

#include	"main.h"

//--- 定数定義 --------------------------------------------------------------------------

#define MOTION_BLEND_TIME	5					// モーションの切り替え時間(5/60秒)

#define ENE_X				0					// エネミー X 座標
#define ENE_Y				-30					//			 Y 座標
#define ENE_Z				55					//			 Z 座標
#define ENE_SCLAE			30.0f				// エネミーの大きさ
#define WAIT_TIME			RAND(10, 70)		// 乱数で待機時間を設定する
#define ATACK_TIME			(120  /	enemy->m_fLevel)	// 攻撃時間
#define ATTACK_SPEED		(0.5f * enemy->m_fLevel)	// 攻撃速度

#define DAMAGE_TIME			30						// ダメージ時間
#define KNOCKOUT_TIME		120						// ノックアウト時間(３秒)
#define KNOCKOUT_PLAY_SPEED 4						// ノックアウト時の再生速度

//--- 変数を定義 ---------------------------------------------------------------------------
cEnemy	*enemy;

/*---------------------------------------------------------------------------*
| <<< 動作リセット >>>		※エネミーのモード切り替えの際には必ず呼び出すこと
|	入力	bBlend = TRUE : ブレンドする
*--------------------------------------------------------------------------*/
void	cEnemy::Reset(BOOL bBlend)
{
	m_iMode		= ENE_WAIT;			// 最初は待ちから始める
	m_iRenda	= 0;				// エネミー、パンチの連打数をリセット

	float	fBlendTime;				// モーションブレンド時間計算
	if(bBlend){ fBlendTime = MOTION_BLEND_TIME;}
	else      { fBlendTime = 0;}
									// モーションブレンド。fBlendTime はモーション切り替えの時間
	skin_anim_set(X_ENEMY, m_iMode, fBlendTime);
	m_iAnimCount = WAIT_TIME;		// アニメーションタイム設定
}
/*-----------------------------------------------------------------*
| <<< 待機 >>>
*------------------------------------------------------------------*/
void	cEnemy::Wait(void)
{
	// アニメーションを進める
	//skin_anim_set(X_ENEMY, 5, 30);
	skin_frame_set(X_ENEMY, 1);

	// アニメーションが0ならモードを変える
	if(m_iAnimCount <= 0)
	{
		// 1/3 の確立で、左パンチ、右パンチ、待ちのどれかにする
		switch(RAND(3))
		{
		//--- 左パンチ ----------------------------------------------------
		case 0:
			m_iMode		= ENE_LEFT_PUNCH;
			m_iRenda	= RAND2(1, 5);		// パンチの連打数
			m_iAnimCount = (int)ATACK_TIME; // アニメーションタイム
			break;
		//--- 待機 -------------------------------------------------------
		case 2:
			m_iMode		= ENE_WAIT;
			m_iAnimCount = WAIT_TIME;		// アニメーションタイム乱数で決める
			break;
		}									// アニメーション切り替え。
		skin_anim_set(X_ENEMY, m_iMode, MOTION_BLEND_TIME);
	}

}
/*----------------------------------------------------------------------------------------------*
| <<< エネミー攻撃 >>>
*------------------------------------------------------------------------------------------------*/
void	cEnemy::Attack(void)
{
	// アニメーションを進める
	skin_frame_set(X_ENEMY, ATTACK_SPEED);

	// アニメーションが0ならモードを変える
	if(m_iAnimCount <= 0)
	{
		m_iRenda--;
		if(m_iRenda > 0)			// 連打数をチェックする
		{
			if(RAND(2) == 0){ m_iMode = ENE_LEFT_PUNCH ;}
			else			{ m_iMode = ENE_RIGHT_PUNCH;}
			m_iAnimCount = (int)ATACK_TIME; // アニメーション数
			skin_anim_set(X_ENEMY, m_iMode, MOTION_BLEND_TIME);
			snd_play(SND_PUNCH);
		}
		else						// 連打数が 0 なら MODE_WAIT へ
		{
			Reset(TRUE);			// 初期化→ MODE_WAIT になります。
		}
	}
}
/*--------------------------------------------------------------------*
| <<< ダメージを与える >>>
| 入力		fDamage = ダメージ値
*----------------------------------------------------------------------*/
void	cEnemy::DamageSet(float fDamage)
{
	//--- ダメージ計算 -----------------------------------------------
	m_fHP		-= fDamage;
	m_iMode		 = ENE_DAMAGE;
	m_iAnimCount = DAMAGE_TIME;

	// モーションをダメージに切り替え
	skin_anim_set(	X_ENEMY, m_iMode, MOTION_BLEND_TIME);
	snd_play(		SND_ENE_DAMAGE);

	//--- ノックアウトチェック -----------------------------------
	if(m_fHP <= 0)
	{
		m_iAnimCount = KNOCKOUT_TIME * KNOCKOUT_PLAY_SPEED;
		m_iMode		 = ENE_KNOCKOUT;

		// モーションをノックアウトに切り替え				↓ここの掛け算なくてもよいです
		skin_anim_set(	X_ENEMY, m_iMode, MOTION_BLEND_TIME * (float)KNOCKOUT_PLAY_SPEED);
		snd_play(SND_ENE_DAMAGE);
	}
}

/*----------------------------------------------------------------------*
| <<< ダメージ >>>						※エネミーダメージは×２倍で再生される
*-----------------------------------------------------------------------*/
void	cEnemy::Damage(void)
{
	skin_frame_set(X_ENEMY, 2);		// アニメーションを進める
	if(m_iAnimCount <= 0)			// アニメーションが０ならモードを変える
	{
		Reset(TRUE);				// 初期化→MODE_WAITになります。
	}
}

/*---------------------------------------------------------------------*
| <<< エネミー >>>
*---------------------------------------------------------------------*/
void	cEnemy::Main(void)
{
	//--- アニメーション --------------------------------------------
	m_iAnimCount--;
	
	//--- モード別に処理を分岐 ---------------------------------------
	switch(m_iMode)
	{
	case ENE_LEFT_PUNCH:		// 左パンチ
	case ENE_RIGHT_PUNCH:		// 右パンチ
		Attack();
		break;
	case ENE_WAIT:				// 待ち
		Wait();
		break;
	case ENE_DAMAGE:			// ダメージ
		Damage();
		break;
	}
}

/*--------------------------------------------------------------------*
| <<< 描画 >>>
*-------------------------------------------------------------------*/
void	cEnemy::Draw(void)
{
	MTX		mtxWorld, mtxScale, mtxRot, mtxTrans;

	D3DXMatrixScaling(		&mtxScale, ENE_SCLAE);		// 拡大率は各自で設定のこと
														// 座標も各自で設定のこと
	D3DXMatrixTranslation(&mtxTrans, &VEC3(ENE_X, ENE_Y, ENE_Z));
	mtxWorld = mtxScale * mtxTrans;
	skin_draw(X_ENEMY, &mtxWorld);
}

//--- 一括処理 ----------------------------------------------------------------------------------

/*--------------------------------------------------------------------------*
| <<< エネミー動作リセット >>> ※エネミーのモード切替の際には必ず呼び出すこと
*---------------------------------------------------------------------------*/
void	EnemyReset(void)
{
	enemy->Reset(FALSE);
	enemy->m_fHP = HP_MAX;
}
/*-------------------------------------------------------------------------------------------
| <<< 初期化 >>>
*------------------------------------------------------------------------------------------*/
void	EnemyInit(void)
{
	// メモリ取得
	if(enemy == NULL){ enemy = new cEnemy;}

	enemy->Reset(FALSE);
	enemy->m_fHP		= HP_MAX;				// エネミーの体力
	enemy->m_fLevel		= 1;					// エネミーの AI レベル設定(1, 2, 4, 8(難))
	enemy->m_fAttackTime = ATACK_TIME;
}

/*------------------------------------------------------------------------------------------*
| <<< エネミーメイン >>>
*------------------------------------------------------------------------------------------*/
void	EnemyMain(void)
{
	enemy->Main();
}

/*----------------------------------------------------------------------------------------------*
| <<< エネミー描画 >>>
*------------------------------------------------------------------------------------------------*/
void	EnemyDraw(void)
{
	enemy->Draw();
}



