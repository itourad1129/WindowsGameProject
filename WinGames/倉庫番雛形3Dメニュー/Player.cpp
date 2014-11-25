//
//	プレイヤー
//

#include	"main.h"

//--- 定数定義 ---------------------------------------------------------------------------------------
#define		MOVE_SPEED	4							// 移動速度
#define		MOVE_TIME	(BLK_SIZE / MOVE_SPEED)		// 移動時間（速度が 4 にならば、32 / 4 = 8 となる）

//--- 変数定義 ---------------------------------------------------------------------------------------
cPlayer *player;									// ポインタなので new でメモリを確保のこと


/*----------------------------------------------------------------------------------------------------
| <<< プレイヤーリセット処理 >>>
*----------------------------------------------------------------------------------------------------*/
void		cPlayer::Reset(void)
{
	m_iDir		= DIR_DOWN;				// 下向きにする

	m_vecPos	= VEC2(0, 0);			// 位置
	m_iMoveTime	= 0;					// 移動時間
	m_iNimotu = 0;						// 荷物オフ

	m_fAnim = 0;
	m_fRot  = 0;
}

/*----------------------------------------------------------------------------------------------------*
| <<< プレイヤー処理 >>>
*-----------------------------------------------------------------------------------------------------*/
void	cPlayer::Main(void)
{
	//--- キャラ回転中 --------------------------------------------------------------------
	//
	float	fRotTbl[4] = {90, 270, 0, 180};//90  270  0  180
	//m_fRot = fRotTbl[m_iDir];

	if(m_fRot != fRotTbl[m_iDir])
	{
		float	a = fRotTbl[m_iDir];
		float	b = (float)m_fRot - a;
		if(b < -180){ a -= 360;}
		if(b >  180){ a += 360;}
		VALUE_TO_TARGET(m_fRot, a, 20.f);
		if(m_fRot == a){ m_fRot = fRotTbl[m_iDir];}
		return;
	}

	int iNext = ((int)m_vecSpeed.y / MOVE_SPEED) * MAP_W + (int)m_vecSpeed.x / MOVE_SPEED;
	
	// アニメーション
	m_fAnim += 0.1f;
	//--- キャラ移動チェック -----------------------------------
	if(m_iMoveTime == 0)
	{
		// キーが押されたら４の方向へ移動													↓移動時間			移動速度
		if(pad.bKeyClick[KEY_UP		]){m_vecSpeed = VEC2(0, -MOVE_SPEED),m_iDir = DIR_UP,m_iMoveTime = MOVE_TIME;}
		if(pad.bKeyClick[KEY_LEFT	]){m_vecSpeed = VEC2(-MOVE_SPEED, 0),m_iDir = DIR_LEFT,m_iMoveTime = MOVE_TIME;}
		if(pad.bKeyClick[KEY_RIGHT	]){m_vecSpeed = VEC2( MOVE_SPEED, 0),m_iDir = DIR_RIGHT,m_iMoveTime = MOVE_TIME;}
		if(pad.bKeyClick[KEY_DOWN	]){m_vecSpeed = VEC2(0,  MOVE_SPEED),m_iDir = DIR_DOWN,m_iMoveTime = MOVE_TIME;}

		// KEY_UP を参考に残り３方向を作成のこと
	

	// 移動する時のみ壁判定
	if(m_iMoveTime > 0)
		{
		snd_play(SND_WALK);			//歩行音

		// 移動するマスへの加算値(この値にm_iAirを足したものが次のアドレス)
		int iNext = ((int)m_vecSpeed.y / MOVE_SPEED) * MAP_W + ((int)m_vecSpeed.x / MOVE_SPEED);
										// xy プレイヤーの足元
		m_iAdr = POS_ADR(m_vecPos.x, m_vecPos.y);
		m_iAdr += iNext;				// xy プレイヤーの移動先を足す
		
			if(chMap[m_iAdr] == '@')		// 障害物に当たったら※一文字なのでシングルクォーテーション
			{
				m_iMoveTime = 0;			// 移動時間を０にする
				snd_play(SND_OUCH);			// 壁衝突音※ENUMで定義したものを使いましょう。
			}

			int	iXY2 = m_iAdr + iNext;		// ２歩先を調べる
											// 荷物の先に障害物あるか？
			if(chMap[m_iAdr] == 'o' && (chMap[iXY2] == '@' || chMap[iXY2] == 'o'))
			{
				m_iMoveTime = 0;		// 移動時間を０にする
				snd_play(SND_OUCH);		// 壁衝突音
			}
				
			else if(chMap[m_iAdr ] == 'o')	// 荷物に当たったら
			{
				Undo(UNDO_SET);
				//chMap[m_iAdr] = ' ';		// コメントにする
				//		↓のように書き換える
											// 荷物は消すけど、目的地は残す
				if(chMapBak[m_iAdr] == 'x'){ chMap[m_iAdr] = 'x';}
				else					   { chMap[m_iAdr] = ' ';}

				m_iNimotu		= m_iDir + 1;
			}
			else
			{
				Undo(UNDO_SET);		// 一マス歩くたびにアンドゥセット
			}

		}
	

	}	

	

	//--- キャラ移動中 ----------------------------
	if(m_iMoveTime > 0)
	{
		m_vecPos += m_vecSpeed;		// ここで移動
		m_iMoveTime--;				// 移動時間計算
	
		// 移動時間が０になったら荷物を下ろす
		if(m_iMoveTime == 0 && m_iNimotu > 0)
		{
			int iNext = ((int)m_vecSpeed.y / MOVE_SPEED) * MAP_W + (int)m_vecSpeed.x / MOVE_SPEED;
			int a	  = m_iAdr + iNext;		// a は荷物のマップアドレス
			chMap[a]  = 'o';				// 荷物を置く
			m_iNimotu = 0;
		}
	}

	m_fAnim += 0.025f; 
	m_iAnimTime_a++;
	if(m_iAnimTime_a >= 30)
	{
		m_iAnimTime_b++;
		m_iAnimTime_a = 0;

		if(m_iAnimTime_b >= 4)
		{
			m_iAnimTime_b = 0;
		}
	}
	
}
/*----------------------------------------------------------------------------------------------------*
| <<< プレイヤーと荷物描画 >>>
*-----------------------------------------------------------------------------------------------------*/
void		cPlayer::Draw(void)
{
	

	MTX		mtxWorld, mtxTrans, mtxRot;

	mtxWorld = mtxRot  * mtxTrans;

	VEC3	v;
	
	v.x =  (m_vecPos.x - __SCR_W / 2 + BLK_SIZE / 2.f) / 56.0f;
	v.z = -(m_vecPos.y - __SCR_H / 2 + BLK_SIZE / 2.f) / 56.0f;
	v.y = 0;
	D3DXMatrixTranslation(&mtxTrans, &v);			// 移動行列作成
	D3DXMatrixRotationY(	&mtxRot,	R(m_fRot));	// 回転用列作成
	mtxWorld = mtxRot * mtxTrans;					// 行列の合成(回転、移動の順)
	
	d3_3d_draw(X_PLAYER, &mtxWorld);

	
	//--- マップ座標、マップアドレス表示 -------------------------------------------------
	int x = (int)m_vecPos.x / BLK_SIZE;
	int y = (int)m_vecPos.y / BLK_SIZE;
	Printf(0, 16, 16, "PLAYER:%d,%d", x, y);
	Printf(0, 32, 16, "ADR	  :%d",		ADR(x, y));

	//--- 足元になにがあるか表示 --------------------------------------------------------
	char	b = chMap[ADR(x,y)];
	if(b == '@'){ d3_printf("壁"		);}		// Printf 関数は全角文字が使えないので、d3_printf を使ってます
	if(b == ' '){ d3_printf("床"		);}
	if(b == 'x'){ d3_printf("目的地"	);}
	if(b == 'o'){ d3_printf("荷物"	);}
	
	
	
	RECT	r;

	// プレイヤー描画
	/*if(m_iDir == DIR_UP){SetRect(&r, 0,  0,  BLK_SIZE, BLK_SIZE);	// 画像の切り出し上
	}
	if(m_iDir == DIR_DOWN){SetRect(&r, 0, BLK_SIZE, BLK_SIZE, BLK_SIZE * 2); //した
	}
	if(m_iDir == DIR_LEFT){SetRect(&r, 0, BLK_SIZE * 2, BLK_SIZE, BLK_SIZE * 3);// 左
	}
	if(m_iDir == DIR_RIGHT){SetRect(&r, 0, BLK_SIZE * 3, BLK_SIZE, BLK_SIZE * 4);//右
	}*/
	//SetRect(&r, 0 + (int)m_fAnim % 3 * BLK_SIZE, m_iDir * BLK_SIZE, BLK_SIZE + (int)m_fAnim % 3 * BLK_SIZE, BLK_SIZE + m_iDir * BLK_SIZE);
	// 1上   2下   3左    4右 
	//v.x = m_vecPos.x + BLK_SIZE / 2.f;		// 表示座標(BLK_SIZE / 2 で半ブロックずらしている）
	//v.y = m_vecPos.y + BLK_SIZE / 2.f;
	//v.z = 0;
	//d3_spr_draw(TEX_PLAYER, &r, &v);		// 描画
	


	// 以下、荷物を運んでいるときは荷物を描画
	if(m_iNimotu == 0){return;}				// 荷物を運んでいないときはここで終了

	SetRect(&r, 128, 0, 160, 32);
	v.x += m_vecSpeed.x * MOVE_TIME;		// += です。注意
	v.y += m_vecSpeed.y * MOVE_TIME;
	d3_spr_draw(TEX_MAP, &r, &v);

	
}
