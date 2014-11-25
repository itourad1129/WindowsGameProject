#define	TSK_NO_MOTHER	((TSK_WORK *)-1)

/*------------------------------------------------------------------------------*
| <<< pfStar を fEnd に iTime 周期を掛けて変化させる >>>
|	入力	ptskMother = マザーとなるタスク	※TSK_NO_MOTHER として指定ならば無視
|			*pfStart   = 開始位置				※値は保持されないので注意
|			fEnd       = 終了位置
|			iTime	   = 移動時間
|			iDelay	   = 遅延時間
*------------------------------------------------------------------------------*/
extern	TSK_WORK	*tsk_move_start(TSK_WORK *ptskMother, float *pfStart, float fEnd, int iTime, int iDelay);
extern	TSK_WORK	*tsk_move_start(float *pfStart, float fEnd, int iTime, int iDelay);
extern	TSK_WORK	*tsk_move_start(float *pfStart, float fEnd, int iTime);


/*------------------------------------------------------------------------------*
| <<< pfStar を fEnd に iTime 周期を掛けて変化させる( 正弦の 0 ～90 度を利用 ) >>>
|	入力	ptskMother = マザーとなるタスク	※TSK_NO_MOTHER として指定ならば無視
|			*pfStart   = 開始位置				※値は保持されないので注意
|			fEnd       = 終了位置
|			iTime	   = 移動時間
|			iDelay	   = 遅延時間
*------------------------------------------------------------------------------*/
extern	TSK_WORK	*tsk_move_sin_start(TSK_WORK *ptskMother, float *pfStart, float fEnd, int iTime, int iDelay);
extern	TSK_WORK	*tsk_move_sin_start(float *pfStart, float fEnd, int iTime, int iDelay);
extern	TSK_WORK	*tsk_move_sin_start(float *pfStart, float fEnd, int iTime);


/*------------------------------------------------------------------------------*
| <<< pfStar を fEnd に iTime 周期を掛けて変化させる( 余弦の 0 ～90 度を利用 ) >>>
|	入力	ptskMother = マザーとなるタスク	※TSK_NO_MOTHER として指定ならば無視
|			*pfStart   = 開始位置				※値は保持されないので注意
|			fEnd       = 終了位置
|			iTime	   = 移動時間
|			iDelay	   = 遅延時間
*------------------------------------------------------------------------------*/
extern	TSK_WORK	*tsk_move_cos_start(TSK_WORK *ptskMother, float *pfStart, float fEnd, int iTime, int iDelay);
extern	TSK_WORK	*tsk_move_cos_start(float *pfStart, float fEnd, int iTime, int iDelay);
extern	TSK_WORK	*tsk_move_cos_start(float *pfStart, float fEnd, int iTime);

