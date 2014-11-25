//
//	移動タスク
//


//#include	"main.h"


//--- 構造体定義 ----------------------------------------------------------------------------------
struct	MOVE_WORK
{
	int			iMode;
	TSK_WORK	*ptskMother;
	float		*pfStart, fStartBak;
	float		fStart, fEnd, fSpeed;
	float		fLength;
	float		fCount;
	int			iTime, iTimeMax;
};


//--- 関数宣言 ------------------------------------------------------------------------------------
static	void	Move(TSK_WORK *t);



/*------------------------------------------------------------------------------*
| <<< pfStart の中を fEnd にするタスク >>>
*------------------------------------------------------------------------------*/
static	void	Move(TSK_WORK *t)
{

	MOVE_WORK	*w = (MOVE_WORK	*)&t->uiWork[0];

	//--- 最初の一回のみスピードの計算をする ------------------------
	switch(t->ucMode)
	{
	case TSK_INIT:
		w->fSpeed = (w->fEnd - *w->pfStart)	/ (float)w->iTime;
		t->ucMode = TSK_MAIN;
		break;

	case TSK_END:
		tsk_end(t);
		break;
	}
	//--- mother が	-1 なら母体をチェックしない	---------------------
	if(w->ptskMother !=	TSK_NO_MOTHER)
	{
		//--- 母体のタスクが死んでいたらここで終了 ------------------
		if(w->ptskMother->uiAdrs ==	NULL)
		{
			tsk_end(t);
			return;
		}
	}
	//--- 以下時間を計測し移動---------------------------------------
	w->iTime--;
	*w->pfStart += w->fSpeed;
	if(w->iTime <= 0)
	{
		*w->pfStart = w->fEnd;
		tsk_end(t);
	}
}



/*------------------------------------------------------------------------------*
| <<< pfStar を fEnd に iTime 周期を掛けて変化させる >>>
|	入力	ptskMother = マザーとなるタスク	※TSK_NO_MOTHER として指定ならば無視
|			*pfStart   = 開始位置				※値は保持されないので注意
|			fEnd       = 終了位置
|			iTime	   = 移動時間
|			iDelay	   = 遅延時間
*------------------------------------------------------------------------------*/
TSK_WORK	*tsk_move_start(TSK_WORK *ptskMother, float *pfStart, float fEnd, int iTime, int iDelay)
{

	//--- ptskMother が	-1 なら母体をチェックしない	-----------------
	if(ptskMother != TSK_NO_MOTHER)
	{
		//--- 母体のタスクが死んでいたらここで終了 ------------------
		if(ptskMother->uiAdrs == NULL){ return NULL;}
	}
	//--- 時間が -1	なら即時実行 ------------------------------------
	if(iTime == -1)
	{
		*pfStart = fEnd;
		return NULL;
	}
	//--- すでに終了位置にいたら ------------------------------------
	if(*pfStart == fEnd)
	{
		return NULL;
	}
	//--- 以下、移動タスクを起動して数値を設定 ----------------------
	TSK_WORK	*t = tsk_start(TSK_MOVE, Move,	0, 0);
	MOVE_WORK	*w = (MOVE_WORK	*)&t->uiWork[0];

	w->ptskMother = ptskMother;
	w->pfStart	  = pfStart;
	w->fEnd		  = fEnd;
	w->iTimeMax	  =
	w->iTime	  = iTime;
	w->fStartBak  = *pfStart;

	if(iDelay == -1){ tsk_quick_call();}
	else			{ t->sDelay = iDelay;}

	return t;
}



/*------------------------------------------------------------------------------*
| <<< pfStar を fEnd に iTime 周期を掛けて変化させる >>>
|	入力	*pfStart   = 開始位置				※値は保持されないので注意
|			fEnd       = 終了位置
|			iTime	   = 移動時間
|			iDelay	   = 遅延時間
*------------------------------------------------------------------------------*/
TSK_WORK	*tsk_move_start(float *pfStart, float fEnd, int iTime, int iDelay)
{
	return	tsk_move_start(TSK_NO_MOTHER, pfStart, fEnd, iTime, iDelay);
}



/*------------------------------------------------------------------------------*
| <<< pfStar を fEnd に iTime 周期を掛けて変化させる >>>
|	入力	*pfStart   = 開始位置				※値は保持されないので注意
|			fEnd       = 終了位置
|			iTime	   = 移動時間
*------------------------------------------------------------------------------*/
TSK_WORK	*tsk_move_start(float *pfStart, float fEnd, int iTime)
{
	return	tsk_move_start(TSK_NO_MOTHER, pfStart, fEnd, iTime, -1);
}



/*------------------------------------------------------------------------------*
| <<< sin を使った pfStart の中を fEnd にするタスク >>>	※初速早く、終速遅い
*------------------------------------------------------------------------------*/
static	void	MoveSin(TSK_WORK *t)
{

	MOVE_WORK	*w = (MOVE_WORK	*)&t->uiWork[0];

	//--- 最初の一回のみスピードの計算をする ----------------
	switch(t->ucMode)
	{
	case TSK_INIT:
		w->fSpeed  = R(90) / (float)w->iTime;
		w->fLength = w->fEnd - *w->pfStart;
		t->ucMode  = TSK_MAIN;
		break;

	case TSK_END:
		tsk_end(t);
		break;
	}
	//--- mother が	-1 なら母体をチェックしない	---------------------
	if(w->ptskMother !=	TSK_NO_MOTHER)
	{
		//--- 母体のタスクが死んでいたらここで終了 ------------------
		if(w->ptskMother->uiAdrs ==	NULL)
		{
			tsk_end(t);
			return;
		}
	}

	//--- 以下時間を計測し移動---------------------------------------
	w->iTime--;
	w->fCount  += w->fSpeed;
	*w->pfStart = w->fStart + w->fLength * sinf(w->fCount);
	//d3_printf("sin:%d,%.4f,%.4f,%.4f", w->iTime, ( sinf(w->fCount)), w->fLength, *w->pfStart);

	if(w->iTime <= 0)
	{
		*w->pfStart = w->fEnd;
		tsk_end(t);
	}
}



/*------------------------------------------------------------------------------*
| <<< pfStar を fEnd に iTime 周期を掛けて変化させる( 正弦の 0 ～90 度を利用 ) >>>
|	入力	ptskMother = マザーとなるタスク	※TSK_NO_MOTHER として指定ならば無視
|			*pfStart   = 開始位置				※値は保持されないので注意
|			fEnd       = 終了位置
|			iTime	   = 移動時間
|			iDelay	   = 遅延時間
*------------------------------------------------------------------------------*/
TSK_WORK	*tsk_move_sin_start(TSK_WORK *ptskMother, float *pfStart, float fEnd, int iTime, int iDelay)
{

	//--- ptskMother が	-1 なら母体をチェックしない	-----------------
	if(ptskMother != TSK_NO_MOTHER)
	{
		//--- 母体のタスクが死んでいたらここで終了 ------------------
		if(ptskMother->uiAdrs == NULL){ return NULL;}
	}
	//--- 時間が -1	なら即時実行 ------------------------------------
	if(iTime == -1)
	{
		*pfStart = fEnd;
		return NULL;
	}
	//--- すでに終了位置にいたら ------------------------------------
	if(*pfStart == fEnd)
	{
		return NULL;
	}
	//--- 以下、移動タスクを起動して数値を設定 ----------------------
	TSK_WORK	*t = tsk_start(TSK_MOVE, MoveSin, 0, 0);
	MOVE_WORK	*w = (MOVE_WORK	*)&t->uiWork[0];

	w->ptskMother = ptskMother;
	w->pfStart	  = pfStart;
	w->fStart	  = *pfStart;
	w->fEnd		  = fEnd;
	w->iTimeMax	  =
	w->iTime	  = iTime;
	w->fStartBak  = *pfStart;

	if(iDelay == -1){ tsk_quick_call();}
	else			{ t->sDelay = iDelay;}

	return t;
}



/*------------------------------------------------------------------------------*
| <<< pfStar を fEnd に iTime 周期を掛けて変化させる( 正弦の 0 ～90 度を利用 ) >>>
|	入力	*pfStart   = 開始位置				※値は保持されないので注意
|			fEnd       = 終了位置
|			iTime	   = 移動時間
|			iDelay	   = 遅延時間
*------------------------------------------------------------------------------*/
TSK_WORK	*tsk_move_sin_start(float *pfStart, float fEnd, int iTime, int iDelay)
{
	return	tsk_move_sin_start(TSK_NO_MOTHER, pfStart, fEnd, iTime, iDelay);
}



/*------------------------------------------------------------------------------*
| <<< pfStar を fEnd に iTime 周期を掛けて変化させる( 正弦の 0 ～90 度を利用 ) >>>
|	入力	*pfStart   = 開始位置				※値は保持されないので注意
|			fEnd       = 終了位置
|			iTime	   = 移動時間
*------------------------------------------------------------------------------*/
TSK_WORK	*tsk_move_sin_start(float *pfStart, float fEnd, int iTime)
{
	return	tsk_move_sin_start(TSK_NO_MOTHER, pfStart, fEnd, iTime, -1);
}



/*------------------------------------------------------------------------------*
| <<< cos を使った pfStart の中を fEnd にするタスク >>>	※初速遅い、終速早い
*------------------------------------------------------------------------------*/
static	void	MoveCos(TSK_WORK *t)
{

	MOVE_WORK	*w = (MOVE_WORK	*)&t->uiWork[0];

	//--- 最初の一回のみスピードの計算をする ----------------
	switch(t->ucMode)
	{
	case TSK_INIT:
		w->fSpeed  = R(90) / (float)w->iTime;
		w->fLength = w->fEnd - *w->pfStart;
		t->ucMode  = TSK_MAIN;
		break;

	case TSK_END:
		tsk_end(t);
		break;
	}
	//--- mother が	-1 なら母体をチェックしない	---------------------
	if(w->ptskMother !=	TSK_NO_MOTHER)
	{
		//--- 母体のタスクが死んでいたらここで終了 ------------------
		if(w->ptskMother->uiAdrs ==	NULL)
		{
			tsk_end(t);
			return;
		}
	}

	//--- 以下時間を計測し移動---------------------------------------
	w->iTime--;
	w->fCount  += w->fSpeed;
	*w->pfStart = w->fStart + w->fLength * (1.0f - cosf(w->fCount));
	//d3_printf("cos:%d,%.4f,%.4f,%.4f", w->iTime, ( cos(w->fCount)), w->fLength, *w->pfStart);

	if(w->iTime <= 0)
	{
		*w->pfStart = w->fEnd;
		tsk_end(t);
	}
}



/*------------------------------------------------------------------------------*
| <<< pfStar を fEnd に iTime 周期を掛けて変化させる( 余弦の 0 ～90 度を利用 ) >>>
|	入力	ptskMother = マザーとなるタスク	※TSK_NO_MOTHER として指定ならば無視
|			*pfStart   = 開始位置				※値は保持されないので注意
|			fEnd       = 終了位置
|			iTime	   = 移動時間
|			iDelay	   = 遅延時間
*------------------------------------------------------------------------------*/
TSK_WORK	*tsk_move_cos_start(TSK_WORK *ptskMother, float *pfStart, float fEnd, int iTime, int iDelay)
{

	//--- ptskMother が	-1 なら母体をチェックしない	-----------------
	if(ptskMother != TSK_NO_MOTHER)
	{
		//--- 母体のタスクが死んでいたらここで終了 ------------------
		if(ptskMother->uiAdrs == NULL){ return NULL;}
	}
	//--- 時間が -1	なら即時実行 ------------------------------------
	if(iTime == -1)
	{
		*pfStart = fEnd;
		return NULL;
	}
	//--- すでに終了位置にいたら ------------------------------------
	if(*pfStart == fEnd)
	{
		return NULL;
	}
	//--- 以下、移動タスクを起動して数値を設定 ----------------------
	TSK_WORK	*t = tsk_start(TSK_MOVE, MoveCos, 0, 0);
	MOVE_WORK	*w = (MOVE_WORK	*)&t->uiWork[0];

	w->ptskMother = ptskMother;
	w->pfStart	  = pfStart;
	w->fStart	  = *pfStart;
	w->fEnd		  = fEnd;
	w->iTimeMax	  =
	w->iTime	  = iTime;
	w->fStartBak  = *pfStart;

	if(iDelay == -1){ tsk_quick_call();}
	else			{ t->sDelay = iDelay;}

	return t;
}



/*------------------------------------------------------------------------------*
| <<< pfStar を fEnd に iTime 周期を掛けて変化させる( 余弦 0 ～90 度を利用 ) >>>
|	入力	*pfStart   = 開始位置				※値は保持されないので注意
|			fEnd       = 終了位置
|			iTime	   = 移動時間
|			iDelay	   = 遅延時間
*------------------------------------------------------------------------------*/
TSK_WORK	*tsk_move_cos_start(float *pfStart, float fEnd, int iTime, int iDelay)
{
	return	tsk_move_cos_start(TSK_NO_MOTHER, pfStart, fEnd, iTime, iDelay);
}



/*------------------------------------------------------------------------------*
| <<< pfStar を fEnd に iTime 周期を掛けて変化させる( 正弦の 0 ～90 度を利用 ) >>>
|	入力	*pfStart   = 開始位置				※値は保持されないので注意
|			fEnd       = 終了位置
|			iTime	   = 移動時間
*------------------------------------------------------------------------------*/
TSK_WORK	*tsk_move_cos_start(float *pfStart, float fEnd, int iTime)
{
	return	tsk_move_cos_start(TSK_NO_MOTHER, pfStart, fEnd, iTime, -1);
}

