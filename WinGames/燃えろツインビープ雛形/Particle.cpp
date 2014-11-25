//
//	パーティクル
//

#include	"main.h"

//--- 定数定義 -----------------------------------------------------------------------------
#undef	MAX_						// #undef は一度 #define で定義したものを無効にするものです。
#undef	TEX_						// ヘッダーファイル等で使用されている可能性が高いものを予め無効にしておく
#undef	SIZE_
#undef	TIME_-1
#undef	U0
#undef	V0	
#undef	U1
#undef	V1

#define	MAX_			500			// 表示最大数
#define	TEX_			TEX_PARTICLE// テクスチャー
#define	SIZE_			0.5f		// 表示サイズ
#define TIME_			100			// UV値

#define U0				2.f  / d3.texInfo[TEX_].fWidth
#define V0				2.f  / d3.texInfo[TEX_].fHeight
#define U1				29.f / d3.texInfo[TEX_].fWidth
#define V1				29.f / d3.texInfo[TEX_].fHeight

//--- 変数定義 -----------------------------------------------------------------------
static	D3_LPVTX_BUF	vb[		TIME_];			// ここに頂点の情報が入る
static	VEC3			vecPos[	 MAX_ ];		// 表示座標
static	VEC3			vecSpeed[MAX_ ];		// 拡散速度
static	int				iTime[	 MAX_ ];		// 表示時間
static	VEC3			vecAccel[MAX_ ];		// 加速度

/*--------------------------------------------------------------*
| <<< パーティクルメイン >>>
*--------------------------------------------------------------*/
void	ParticleMain(void)
{
	// 使用中のパーティクルの時間を計算する
	for(int i = 0; i < MAX_; i++)
	{
		if(iTime[i] > 0){ iTime[i]--;}
		vecSpeed[i] += vecAccel[i];
		vecPos[	 i] += vecSpeed[i];
	}
}
/*-----------------------------------------------------------------------*
| <<< パーティクル初期化 >>>
*-----------------------------------------------------------------------*/
void	ParticleInit(void)
{
	int	i;

	for(i = 0; i < MAX_; i++)			// パーティクルの時間を０として
	{									// 初期化する
		iTime[i] = 0;
	}

	if(vb[0] != NULL){ return;}		// すでにポリゴンを登録していたら帰る

	// 自前でポリゴン作成
	for(i = 0; i < TIME_; i++)
	{
		float	fR	 = (SIZE_	/ (float)TIME_) * (float)(TIME_ -i) + SIZE_;
		float	fAlp = (float)i	/ (float)TIME_;
		DWORD	c	 = D3_COL((225.f * fAlp), 255, 255, 255);

		// 頂点情報
		D3_VTX_LX_	vt[] =				// パーティクルの頂点データ
		{								// ここでやらないと UV 値が設定されない
			-fR, -fR, 0, c, U0, V1,
			-fR,  fR, 0, c, U0, V0,
			 fR,  fR, 0, c, U1, V0,
			 fR, -fR, 0, c, U1, V1,
		};
		D3_VERTEX_CREATE(vt, vb[i], LX, 4);	// このマクロで頂点情報を登録
	}
}

/*------------------------------------------------------------------------*
| <<< パーティクル描画 >>>
*-------------------------------------------------------------------------*/
void	ParticleDraw(void)
{
	// 設定変更(処理終了後に元に戻す必要あり)
	d3_light_set(			false);					// ライトオフ
	d3_stat_zread_set(		false);					// 深度バッファへの書き込みを無効にする
	d3_stat_culling_set(	false);					// 裏面の表示をする
	d3_stat_alpha_greate_set(10);					// アルファテスト ON(RGBの合計が10未満ならば色を透過)
	d3_stat_blend_set(		D3_BLEND_ADD);			// 加算合成

	// パーティクル描画
	for(int  i = 0; i < MAX_; i++)					// MAX 個ループ
	{
		if(iTime[i] == 0){continue;}				// 時間が0以上のものを表示
													// 時間よりパターンを求める
		d3_stat_stream_source_set(vb[iTime[i]], sizeof(D3_VTX_LX));

		d3_billboard_set(&d3.mtxBillboard, &vecPos[i]);
													// 描画
		d3_primitive_draw(D3DPT_TRIANGLEFAN, 0, 2);
	}

	// 終了処理（変更された設定を元に戻す)
	d3_stat_blend_set(			D3_BLEND_NORMAL);	// 通常の合成に戻す
	d3_stat_culling_set(		true);				// 裏面の表示をしない
	d3_stat_alpha_greate_set(0);					// アルファテスト OFF
	d3_light_set(			true);					// ライディングする
	d3_stat_zread_set(		true);					// 深度バッファを有効にする
}

/*------------------------------------------------------------------------*
| <<< パーティクルスタート >>>
|	入力		pvecPos = 登録座標
|				fPower	= 加速値
*-------------------------------------------------------------------------*/
void	ParticleStart(VEC3	*pvecPos, float fPower)
{
	for(int i = 0; i < MAX_; i++)
	{
		if(iTime[i] == 0)						// 時間が 0 のものを探して登録する
		{
			vecPos[  i] = *pvecPos;			// 位置の設定
			iTime[	 i] = TIME_;					// 表示時間の設定
			vecSpeed[i] = VEC3(0, 0, 0);	// 拡散速度はランダムで決める

			// 加速度計算
			vecSpeed[i].x = FRAND2(-fPower, fPower);
			vecSpeed[i].y = FRAND2(-fPower, fPower);
			vecSpeed[i].z = FRAND2(-fPower, fPower);
			break;
		}
	}
}
