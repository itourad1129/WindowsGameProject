
//
//タイトルの処理	※説明やゲームオーバーにも使えます。
//

#include	"main.h"


/*---------------------------------------------------------------------*
<<<タイトル>>>
	入力	iTex	= 表示するテクスチャー番号
			*pMode  = 値をセットしたい変数のポインタ
			iNext   = 左クリック時の次のモード		※pMode の次の値
			iSound  = 左クリック時の効果音
	戻り値	true	= モードが変わった瞬間		※キーが押された瞬間
*-----------------------------------------------------------------------*/

//--- 定数 ---------------------------------------------------------------
#define FADE_SPEED_		20.f			// フィード速度

//--- 変数 ---------------------------------------------------------------
static	float	fCount = 0.0f;			// タイトルフィード用
static	int		iFade	= -1;			// -1 : フィードイン
										// 0  : なにもしない
										// +1 : フィードアウト

bool	Title(int iTex, int *pMode, int iNext, int iSound)
{
	bool	bRet = false;

	//--- 描画 ----------------------------------------------------------
	d3_tex_set(iTex);
	d3_2d_box_fill_tex_draw(&VEC2(0,0), &VEC2(__SCR_W, __SCR_H), D3_COL((int)fCount, 255, 255, 255));
	
	//---キー入力チェック------------------------------------------------
	// スペースキーかマウス左がクリックでモード切り替え
	if((pad.bKeyClick[KEY_SPACE] || pad.bLeftClick) && iFade == 0)
	{
		iFade = 1;						//いきなりモードを変えずに、フィード開始
		snd_play(iSound);
		//bRet	= true;					//コメント
		//*pMode	= iNext;			//コメント
	}

	//--- フィード -----------------------------------
	switch(iFade)
	{
	case -1:							// イン
		fCount += FADE_SPEED_;
		if(fCount >= 255.0f){ fCount = 255.0f; iFade = 0;}
		break;
	case 1:
		fCount -= FADE_SPEED_;
		if(fCount <= 0.0f)
		{
			fCount = 0.0f;
			iFade  = -1;

			*pMode = iNext;				// フェードアウトしきった時に
			bRet	= true;				// 終了とする
		}
		break;
	}
	return bRet;
}

/*-----------------------------------------------------------------------------*
| <<< メニュー使用時のタイトル >>>
|	入力	iTex	= 表示するテクスチャー番号
|			iSound	= 左クリック時の効果音
|			bSw		= true : フェードアウト開始
|	戻り値	true	= モードが変わった
*-------------------------------------------------------------------------------*/
bool	TitleMenu(int	iTex,	int	iSound,	bool bSw)
{
	bool	bRet	= false;

	//--- 640×480 ドットの絵を表示 --------------------
	d3_tex_set(iTex);
	d3_2d_box_fill_tex_draw(&VEC2(0, 0), &VEC2(__SCR_W, __SCR_H), D3_COL((int)fCount, 255, 255, 255));

	//--- キー入力チェック --------------------------------------
	if(bSw)						// ここ、Title関数と違う
	{
		iFade = 1;
		snd_play(iSound);
	}
	//--- フィード ------------------------------------
	switch(iFade)
	{
	case -1:		//イン
		fCount += FADE_SPEED_;
		if(fCount >= 255.0f){ fCount = 255.0f; iFade = 0;}
		break;
	case 1:			//アウト
		fCount -= FADE_SPEED_;
		if(fCount <= 0.0f)
		{
			fCount = 0.0f;
			iFade  = -1;
			
				bRet	= true;
		}
		break;
	}
	return bRet;
}