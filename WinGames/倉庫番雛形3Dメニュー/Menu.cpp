//
//	メニュー処理
//

#include	"main.h"

//--- 定数 ------------------------------------------------------------------------------------------------

#define	CENTER_		1			//0 : 文字左よせ、1 : 文字中央

#define FONT_ID_	3			// フォント番号(2 ~ 7 の範囲で)
#define TEX_W_		512			// テクスチャーの横サイズ(2,4,8,16,32,64,128,256,512,1024の中で)
#define TEX_H_		32			// テクスチャーの縦サイズ(2,4,8,16,32,64,128,256,512,1024の中で)
#define FONT_SIZE_	42			// 文字のサイズ
#define	INTERVAL_TIME_	30	

//--- 変数 ------------------------------------------------------------------------------------------------

int		iMenuMax;				//メニューの項目数

int		iMenuSel;				// メニューがいまどこを指しているか
bool	bMenuCancel;	// メニューのボタン情報
bool	bMenuOk;
bool	bMenuCancelEnable;		// true : キャンセル許可

static	int	iIntervalTime  = 0;		//点滅時間(0 以外の時)


/*---------------------------------------------------------------------------------*
| <<< マウスの範囲チェック >>>				※メニューの項目チェックに使う
|	入力	p0, p1 = 範囲の左上と右下
|	戻り値	true : 範囲内
*----------------------------------------------------------------------------------*/
static	BOOL	CheckBox(VEC2 *p0, VEC2 *p1)
{
	if((int)p0->x <= pad.sX && p1->x >pad.sX
	&& (int)p0->y <= pad.sY && p1->y >pad.sY)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*--------------------------------------------------------------------------*
| <<< メニュー初期化 >>>
*--------------------------------------------------------------------------*/
void	MenuInit(void)
{
	FOLDER_SET("画像");
	d3_tex_load(TEX_MENU_FRAME0, "menubar.png", true);		// 非選択時
	d3_tex_load(TEX_MENU_FRAME1, "menubar2.png", true);		// 選択時

	//--- 文字フォントを作成する ------------------------------------
	//	"HG丸ゴシックM-PRO" はフォトショップをインストールすることで使用できるフォントです。
	//	未インスト時には「MSゴシック」になります。
	d3_font_create(FONT_ID_, FONT_SIZE_ / 2, FONT_SIZE_, "HG丸ｺﾞｼｯｸM-PRO", false, 300);
}

/*-----------------------------------------------------------------------*
| <<< メニュー描画＆判定 >>>
|	入力	p		  = 描画座標
|			HInterval = 縦の配置
*------------------------------------------------------------------------*/
void	MenuMain(VEC2 *p, float fHInterval)
{

	if(iIntervalTime == 0 && bMenuOk){ return;}

	//--- メニューの項目を描画 ----------------------------------
	d3_stat_zread_set( false);					// 重なりを無視する
	d3_stat_zwrite_set(false);
	d3_stat_alpha_set( true);					// 透明オン
	d3_stat_filter_set(D3_FILTER_LINEAR);		// ぼかし処理

	//--- カーソル処理 ------------------------------------------
	if(pad.bKeyClick[KEY_UP])
	{
		iMenuSel--;
		if(iMenuSel < 0){ iMenuSel = iMenuMax -1;}
	}
	if(pad.bKeyClick[KEY_DOWN])
	{
		iMenuSel++;
		if(iMenuSel >= iMenuMax){ iMenuSel = 0;}
	}

	//--- マウスを動かしたらどこを指しているかの変数をリセッする --
	if((pad.sMoveX || pad.sMoveY) && iIntervalTime == 0)
	{
		iMenuSel = -1;
	}

	// p0 を左上座標とする
	VEC2	p0 = *p;

	for(int i = 0; i < iMenuMax; i++)
	{
		//--- 描画スイッチ ------------------------------------------
		bool	bDraw = false;
		if(iIntervalTime == 0				// iIntervaltime が 0の時はメニュー未選択の時
		||(iIntervalTime % 20) > 5 && iMenuSel == i)
		{
			bDraw = true;
		}

		//--- 枠 -----------------------------------------------------
		int		iTex  = TEX_MENU_FRAME0;				// 枠のテクスチャー番号
		D3_INFO *info = &d3.texInfo[iTex];
		// p0 は右下座標		↓オリジナル画像のサイズを得ている
		VEC2	p1 = p0 + VEC2(info->fOriWidth, info->fOriHeight);

		// マウスとの当たり判定
		//if(CheckBox(&p0, &p1))
		if((pad.sMoveX || pad.sMoveY) && CheckBox(&p0, &p1) || iMenuSel == i)
		{
			d3_tex_set(iTex + 1);
			if(iIntervalTime == 0){ iMenuSel = i;}
		}
		else
		{
			d3_tex_set(iTex);
		}
		// 描画
		if(bDraw){ d3_2d_box_fill_tex_draw(&p0, &p1, 0, 0, info->fU, info->fV, D3_COL(255, 255, 255, 255));}

		//--- メニューの文字を描画 ----------------------------------------------
		info = &d3.texInfo[TEX_MENU0];

		#if CENTER_ == 1
			float	fX = (__SCR_W - info->fWidth / 4) / 2;
		#else
			float	fX = p0.x;
		#endif

		d3_tex_set(				TEX_MENU0 + i);
		if(bDraw){ d3_2d_box_fill_tex_draw(&VEC2(fX, p0.y),&VEC2(fX + info->fWidth / 2, p0.y + info->fHeight / 2));}

		//--- 改行 --------------------------------------------------------------
		p0.y += fHInterval;
	}
	//--- キャンセル時には -2、それ以外は指定の値を返す ---------------------------
	if((pad.bRightClick || pad.bKeyClick[KEY_X]) && bMenuCancelEnable)
	{
		iMenuMax	=0;
		bMenuCancel = true;
		iMenuSel	= -2;
	}
	else if((pad.bLeftClick || pad.bKeyClick[KEY_Z]) && iMenuSel >= 0)
	{
		iIntervalTime = INTERVAL_TIME_;
	}
	//--- 点滅処理 ----------------------------------------------
	if(iIntervalTime > 0)
	{
		iIntervalTime--;
		if(iIntervalTime == 0)
		{
			iMenuMax = 0;
			bMenuOk  = true;
		}
	}

	//--- 終了処理 ---------------------------------------------------------------
	d3_stat_zread_set( true);				// 重なりを有効にする
	d3_stat_zwrite_set(true);
}

/*----------------------------------------------------------------------*
| <<< メニュー項目の追加 >>>		※レンダリング面に文字を作図している
|	入力	str	= 項目の文字列
*----------------------------------------------------------------------*/
void	MenuAdd(char *str)
{
	int		iTex = TEX_MENU0 + iMenuMax;	// iTex は作図するテクスチャー番号

	//--- レンダリング面作成 ----------------------------------
	d3_offscreen_free(		iTex);
	d3_offscreen_create(	iTex, TEX_W_ * 2, TEX_H_ * 2);
	d3.texInfo[iTex].bRepair = true;		// このスイッチをtrue にするとフルスクリーン切り替え時に画像が保存される
	
	//--- メニューの項目を描画 --------------------------------
	d3_offscreen_begin(iTex);
	d3_clear(D3_COL(0, 0, 0, 0));

	RECT	r = {32, 10, TEX_W_-32, TEX_H_};
	//影
	r.left -= 2;
	r.top  += 2;
	d3_font_draw(FONT_ID_, &r, CENTER_, D3_COL(255,	0,	0,	0), str);
	// 文字
	r.left -= 2;
	r.top  -= 2;
	d3_font_draw(FONT_ID_, &r, CENTER_, D3_COL(255,	255, 255, 255), str);

	d3_offscreen_mem_save(iTex);
	d3_offscreen_end(iTex);

	//--- メニューの選択項目を設定 ----------------------------------------
	iMenuMax++;

	iMenuSel		= -1;
	bMenuOk			= false;
	bMenuCancel		= false;
	bMenuCancelEnable = false;
}