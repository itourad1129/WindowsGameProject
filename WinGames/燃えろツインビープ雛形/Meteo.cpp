//
//	メテオ(隕石)
//

#include	"main.h"

//--- 定数を定義 ----------------------------------------------------
#define METEO_START		60.0f			// 隕石のスタート位置
#define	METEO_END		-13.0f			// 隕石の終了位置
#define SPEED_MIN		0.1f			// 隕石の最低速度
#define SPEED_MAX		0.5f			// 隕石の最高速度
#define FADE_IN_SPEED	0.01f			// 登録時のフェードイン速度
#define	DELAY_TIME		240				// 隕石登録時の遅延時間
#define DAMAGE_TIME		60				// プレイヤーがダメージ時にくるくる回る時間

//--- 変数を定義 -----------------------------------------------------------
static	int	iOperationMax;				// 隕石の処理数
cMeteo	*meteo;

/*--------------------------------------------------------------------*
|	<<< 初期化 >>>
|	入力	iNum = 管理番号
*--------------------------------------------------------------------*/
void	cMeteo::Init(int iNum)
{
	m_vecPos.x	= (iNum % 3) * 3.0f -3.0f; // 開始位置設定
	m_vecPos.y	= METEO_START;
	m_vecPos.z	= 0;
	m_fAlp		= 0;
	m_iDelay	= DELAY_TIME * (iNum + 1);	// 遅延時間設定
	m_iDelay	/= 4;						// 最初はすぐ出てくるように割ってみる
}

/*--------------------------------------------------------------------
| <<< メイン >>>
*------------------------------------------------------------------*/
void	cMeteo::Main(void)
{
	//--- 遅延時間計算 -----------------------------------------
	if(m_iDelay > 0)
	{
		m_iDelay--;
	}
	//--- 移動 -------------------------------------------------
	else
	{
		//--- 半透明レート計算 ---------------------------------
		// 移動の前に処理しないと、画面端で見きれて初期位置に戻った時に少しフェードインしてしまう
		VALUE_TO_TARGET(m_fAlp, 1.0f, FADE_IN_SPEED);

		//--- 移動処理 -----------------------------------------
		// スタート位置か？スタート位置ならば速度を計算
		if(m_vecPos.y == METEO_START){ m_vecSpeed.y = FRAND2(SPEED_MIN, SPEED_MAX);}

		// 移動
		//m_vecPos.y -= m_vecSpeed.y;
		m_vecPos.y -= (m_vecSpeed.y + (float)player->m_iScore / 10000.f);

		// 画面の下端で見切れたか？
		if(m_vecPos.y < METEO_END)
		{
			m_vecPos.y = METEO_START;		// 初期位置へ
			m_fAlp		= 0;				// 透明にする
		}
	
		//--- 当たり判定 -----------------------------------------------
		if(collision_sphere_sphere(
				&player->m_vecPos,	d3.hitWork[X_PLAYER].fRadius * 0.25f,
				&m_vecPos,					d3.hitWork[X_METEO ].fRadius * 0.5f))
		{
			// ここでパーティクルを呼び出してください(複数個)
			//ParticleStart(&VEC3(m_vecPos.x,m_vecPos.y,m_vecPos.z));
			m_vecPos.y			= METEO_START;		// 隕石を初期位置へ戻す
			m_iDelay			= RAND(DELAY_TIME);	// 遅延時間の設定
			snd_play(SND_DAMAGE);
			player->m_iDamageTime = DAMAGE_TIME;	// プレイヤーダメージ
			player->m_iHp = player->m_iHp - 20;
		}
	}
		//--- 行列計算 -------------------------------------------------
		MTX		mtxTrans,	mtxRot;
										// 平行移動
		D3DXMatrixTranslation(&mtxTrans, &m_vecPos);
											// X回転
		D3DXMatrixRotationX(	&mtxRot,	R(m_vecPos.y * 10));
		m_mtxWorld = mtxRot * mtxTrans;			// 行列の掛け合わせ
	
}

/*----------------------------------------------------*
| <<< 描画 >>>
*----------------------------------------------------*/
void	cMeteo::Draw(void)
{
	if(m_iDelay == 0)
	{
		d3_3d_draw(X_METEO, &m_mtxWorld, m_fAlp);
	}
}

//--- 以下、一括処理 ------------------------------------------------------------------------
/*----------------------------------------------------------*
| <<< 隕石初期化 >>>
*-----------------------------------------------------------*/
void	MeteoInit(void)
{
	// メモリ確保
	if(meteo == NULL)
	{
		meteo = new cMeteo[METEO_MAX];
	}

	// 初期化
	for(int i = 0; i < METEO_MAX; i++)
	{
		meteo[i].Init(i);
		
	}
}

/*--------------------------------------------------------------------*
| <<< 隕石メイン >>>
*-------------------------------------------------------------------*/
void	MeteoMain(void)
{
	// 隕石の処理数
	//iOperationMax = 3;
	//
	iOperationMax = 3 + (player->m_iScore / 1000);
	//


	// 隕石の数分、メイン処理
	for(int i = 0; i < iOperationMax; i++)
	{
		meteo[i].Main();
	}
}

/*--------------------------------------------------------------*
| <<< 隕石描画 >>>
*---------------------------------------------------------------*/
void	MeteoDraw(void)
{

	d3_stat_alpha_set(true);				// 半透明オン
	for(int	i = 0; i < iOperationMax; i++)
	{
		meteo[i].Draw();
	}
}