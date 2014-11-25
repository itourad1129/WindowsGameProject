//
//	カメラ
//

#include	"main.h"

//--- 構造体定義 ----------------------------------------------
struct	CAMERA_MOVE				// カメラの構造体
{
	int		fTime;				// カメラの移動時間
	float	fX, fY, fZ;			// カメラの移動位置
};

//--- カメラ移動用変数 ---------------------------------------------------
static	int		iTime;			// カメラの移動時間計測用
static	int		iCount;			// ↓のカメラテーブルのカウント用
static	VEC3	vecSpeed;		// カメラの移動速度

static	CAMERA_MOVE acamera[] =		// カメラの移動用テーブル
{//TIME		X		Y		Z
	{ 500,	0,		-8.5f,	-3.0f},	// TIMEは60で1秒となります。500なら8秒ぐらい
	{ 300,	0,		-20.0f,	-22.0f},	//
	{-1},
};

/*-------------------------------------------------------------------*
| <<< カメラ移動初期化 >>>
*-------------------------------------------------------------------*/
void	CameraMoveInit(void)
{
	iCount		= 0;
	iTime		= acamera[iCount].fTime; // カメラの変化時間
	vecSpeed	= VEC3(0, 0, 0);		// カメラの変化速度
	d3.vecCamera.x = acamera[iCount].fX;
	d3.vecCamera.y = acamera[iCount].fY;
	d3.vecCamera.z = acamera[iCount].fZ;
	d3.light.Direction = d3.vecCamera * -1;
}

/*------------------------------------------------------*
| <<< カメラ移動メイン >>>
*-------------------------------------------------------*/
void	CameraMoveMain(void)
{
	//---- 時間が0になるまで移動し続ける ------------------------
	if(iTime > 0)
	{
		iTime--;
		d3.vecCamera	+= vecSpeed;		// カメラ座標を苦心

		d3.light.Direction = d3.vecCamera * -1;	// 暗くならないようにライトも設定
	}
	//--- 時間が0の時は次の座標までの速度を計算 ---------------------------------
	else
	{
		CAMERA_MOVE *w = &acamera[iCount];
		iTime			= w->fTime;

		vecSpeed		= (VEC3(w->fX, w->fY, w->fZ) - d3.vecCamera) / (float)iTime;
		iCount++;

		if(acamera[iCount].fTime == -1){iCount = 0;}
	}
	// デバック用にカメラ座標表示
	Printf(0, 468, 12, "CAMERA %.2f, %.2f, %.2f", d3.vecCamera.x, d3.vecCamera.y, d3.vecCamera.z);
}