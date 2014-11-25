/************************************************************************
|	[[[	タスク処理プログラム ]]]
************************************************************************/
//
//	tsk_check は game_main より呼び出すこと
//


#ifdef	__TSK

//#include	"..\\main.h"
#include	<stdarg.h>



/************************************************************************
* <<< 定数 >>>
************************************************************************/
#define	TSK_DEBUG		OFF					// タスク用デバッグスイッチ

#define	TSK_BUF_SIZE	((sizeof(TSK_WORK) * (TSK_MAX +	TSK_PRI_MAX	* 2)) +	(sizeof(int) * TSK_PRI_MAX * 3))


/************************************************************************
* <<< 変数 >>>
************************************************************************/

		TSK_WORK	TskDummy[1];			// エラー用ダミーバッファ
static	TSK_WORK	*TskList;				// タスクリストバッファ
static	TSK_WORK	*TskBeginList;			// そのプライオリティの処理前に呼ばれるタスク
static	TSK_WORK	*TskEndList;			// そのプライオリティの処理後に呼ばれるタスク
static	int			TskCountAll;			// 現在登録されているタスクの数
											// 現在登録されているタスクの数
static	int			*TskCount;				// プライオリティ別
static	int			*Tsk1st;				// 各タスクの初回実行番号が入っている。※－１なら登録なし
static	int			*TskNext;				// 各タスクの最終実行番号が入っている。※－１なら登録なし
		uint		TskExePriolity;			// 実行中のプライオリティ
		int			TskStartNum	= -1;		// 直前に登録したタスク番号



/************************************************************************
* <<< プログラム >>>
************************************************************************/


/*----------------------------------------------------------------------*
| <<< タスク処理初期化 >>>
|	戻り値	FALSE = 初期化失敗
*----------------------------------------------------------------------*/
BOOL	tsk_init(void)
{
	
	if(TskList != NULL){ return FALSE;}

	if(TSK_ALL >= 32){ return (BOOL)sys_error("TSK:タスクの数が多すぎます。TSK_ALL : %d", TSK_ALL);}

	ushort		i;
	TSK_WORK	*t;
										// タスクワーク確保
	if((TskList	= (TSK_WORK	*)malloc(TSK_BUF_SIZE))	==	NULL){ return (BOOL)sys_error("TSK:メモリーが不足しています。");}

	TskBeginList = (TSK_WORK *)&TskList[	 TSK_MAX];
	TskEndList	 = (TSK_WORK *)&TskBeginList[TSK_PRI_MAX];
	TskCount	 = (int		 *)&TskEndList[	 TSK_PRI_MAX];
	Tsk1st		 = (int		 *)&TskCount[	 TSK_PRI_MAX];
	TskNext		 = (int		 *)&Tsk1st[		 TSK_PRI_MAX];

	TskCountAll	= 0;					// 登録数クリア

	for(i =	0; i < TSK_PRI_MAX;	++i)	// プライオリティ毎の情報をクリア
	{
		TskCount[i]	= 0;
		Tsk1st[  i] =	
		TskNext[ i] = -1;
	}

	for(i =	0; i < TSK_PRI_MAX;	++i)	// プライオリティ毎の情報をクリア
	{
		TskBeginList[i].ucStat = TSK_OFF;
		TskEndList[	 i].ucStat = TSK_OFF;
		TskBeginList[i].uiAdrs = (uint)NULL;
		TskEndList[	 i].uiAdrs = (uint)NULL;
	}

	t =	TskList;						// リスト先頭
	for(i =	0; i < TSK_MAX;	++i, ++t)	// タスクリストを初期化
	{
		t->ucStat =	TSK_OFF;			// 動作ステータス
		t->uiAdrs =	(uint)NULL;			// 登録ルーチンの実行アドレス
	}
	t =	TskList;						// リスト先頭

	return TRUE;
}



/*----------------------------------------------------------------------*
| <<< タスク終了処理 >>>
*----------------------------------------------------------------------*/
void	tsk_free(void)
{
	if(TskList != NULL){ free(TskList);}
}



/*----------------------------------------------------------------------*
| <<< プライオリティ毎のタスク初期化処理登録 >>>
|	入力	uiPri  = 登録プライオリティ
|			*pAdr  = 実行アドレス			TSK_PRI0 ～	TSK_PRI7
|			uiID   = 値受け渡しに使用		※ユーザーが自由に使ってよい
|			iDelay = 実行までの遅延タイム	-1 で即実行
*----------------------------------------------------------------------*/
TSK_WORK	*tsk_begin_start(uint uiPri, void *pAdr, uint uiID, int iDelay)
{

	TSK_WORK	*t = &TskBeginList[uiPri];

	t->ucStat =	TSK_USE	+ uiPri;		// 動作開始設定
	t->ucMode =	TSK_INIT;				// 内部情報
	t->usID	  =	uiID;
	t->uiAdrs =	(uint)pAdr;				// コールアドレス登録
	t->sDelay =	iDelay;

	return t;
}



/*----------------------------------------------------------------------*
| <<< プライオリティ毎のタスク初期化処理開放 >>>
|	入力	uiPri  = 登録プライオリティ
*----------------------------------------------------------------------*/
void	tsk_begin_free(uint uiPri)
{

	TskBeginList[uiPri].ucStat = TSK_OFF;
	TskBeginList[uiPri].uiAdrs = NULL;
}



/*----------------------------------------------------------------------*
| <<< プライオリティ毎のタスク終了処理登録 >>>
|	入力	uiPri  = 登録プライオリティ
|			*pAdr  = 実行アドレス			TSK_PRI0 ～	TSK_PRI7
|			uiID   = 値受け渡しに使用		※ユーザーが自由に使ってよい
|			iDelay = 実行までの遅延タイム	-1 で即実行
*----------------------------------------------------------------------*/
TSK_WORK	*tsk_end_start(uint uiPri, void *pAdr, uint uiID, int iDelay)
{

	TSK_WORK	*t = &TskEndList[uiPri];

	t->ucStat =	TSK_USE	+ uiPri;			// 動作開始設定
	t->ucMode =	TSK_INIT;					// 内部情報
	t->usID	  =	uiID;
	t->uiAdrs =	(uint)pAdr;					// コールアドレス登録
	t->sDelay =	iDelay;
	return t;
}



/*----------------------------------------------------------------------*
| <<< プライオリティ毎のタスク終了処理開放 >>>
|	入力	uiPri  = 登録プライオリティ
*----------------------------------------------------------------------*/
void	tsk_end_free(uint uiPri)
{

	TskEndList[uiPri].ucStat = TSK_OFF;
	TskEndList[uiPri].uiAdrs = NULL;
}



/*----------------------------------------------------------------------*
| <<< タスク登録 >>>
|	入力	uiPri  = 登録プライオリティ
|			*pAdr  = 実行アドレス			tsk.h に記述
|			uiID   = 値受け渡しに使用		※ユーザーが自由に使ってよい
|			iDelay = 実行までの遅延タイム	-1 で即実行
*----------------------------------------------------------------------*/
TSK_WORK	*tsk_start(uint uiPri, void *pAdr, uint uiID, int iDelay)
{

	ushort		i, j;
	TSK_WORK	*t;

	TskStartNum	= -1;						// 登録番号初期化
	
	if(TskCountAll > TSK_MAX)				// 登録限界チェック
	{
		TskDummy[0].ucMode = TSK_INIT;
		TskDummy[0].uiAdrs = (uint)pAdr;
		return &TskDummy[0];
	}

	for(i =	0; i < TSK_MAX;	++i)
	{
		j =	i;
		t =	&TskList[j];

		if(t->ucStat & TSK_USE){ continue;}	// 空きを探す
		if(t->sDelay > 0      ){ continue;}	// 終了処理中？

		TskStartNum	= j;					// 一応、登録番号はとっておく
		t->ucStat   = TSK_USE + uiPri;		// 動作開始設定
		t->ucMode	= TSK_INIT;				// 内部情報
		t->usID		= uiID;
		t->uiAdrs	= (uint)pAdr;			// コールアドレス登録
		if(Tsk1st[uiPri] == -1)				// 登録プライオリティが何も
		{
			t->sBefore	   = -1;			// セットされてないときの処理
			t->sAfter	   = -1;
			Tsk1st[ uiPri] = j;
			TskNext[uiPri] = j;
		}
		else								// 前回のワークにこのワークの
		{									// 番号をセット
			TskList[TskNext[uiPri]].sAfter = j;
			t->sBefore					   = TskNext[uiPri];
			t->sAfter					   = -1;
			TskNext[uiPri]				   = j;
		}
											// タスクのワークをここでクリアー
		memset(t->uiWork, 0, TSK_WSIZE * sizeof(uint));
											// してあげる
											// 現実行中のタスクプライオリティ
											// を参照して必ず次の周期に実行す
											// るようにディレイ値を設定
		if(iDelay == -1)					// 即実行チェック
		{
			iDelay = 0;
			void(*func)(TSK_WORK*);
			func  =	(void(*)(TSK_WORK*))t->uiAdrs;
			(*func)(t);
		}
		if(uiPri < TskExePriolity){ t->sDelay = iDelay;}
		else				      { t->sDelay = iDelay + 1;}

		++TskCount[uiPri];					// 登録数＋１
		++TskCountAll;
		return t;							// ワークアドレスを返す
	}


	TskDummy[0].ucMode = TSK_INIT;
	TskDummy[0].uiAdrs = (uint)pAdr;

#if	TSK_DEBUG
	sys_error("タスクオーバーです。");
#endif

	return &TskDummy[0];					// 登録失敗
}



/*----------------------------------------------------------------------*
| <<< タスク削除 >>>
|	入力	t =	タスクのワークアドレス
*----------------------------------------------------------------------*/
void	tsk_end(TSK_WORK *t)
{

	if(t ==	NULL){ return;}

	uint	uiPri = TSK_PRI_GET(t);

											// tsk_end が複数回掛かるときの処理
	if(t->ucStat ==	TSK_OFF	|| t->uiAdrs ==	(uint)NULL){ return;}
	
	--TskCount[uiPri];						// 登録数 -	1
	--TskCountAll;							// 総登録数	- 1

	t->ucStat =	TSK_OFF;					// 動作ステータス
	t->uiAdrs =	(uint)NULL;					// 登録ルーチンの実行アドレス
	t->sDelay =	4;							// 親子関係維持のため４周期はこのタスクエリアの使用禁止(4は適当)

	if(t->sAfter == -1)						// 最終チェック
	{
		TskNext[uiPri] = t->sBefore;
	}
	else
	{
		TskList[t->sAfter].sBefore = t->sBefore;
		if(t->sBefore == -1){ Tsk1st[uiPri] = t->sAfter;}
	}

	if(t->sBefore == -1)					// 先頭チェック
	{
		Tsk1st[uiPri] = t->sAfter;
	}
	else
	{
		TskList[t->sBefore].sAfter = t->sAfter;
		if(t->sAfter ==	-1){ TskNext[uiPri] = t->sBefore;}
	}
}



/*----------------------------------------------------------------------*
| <<< 指定プライオリティのタスクモードを全て変更する >>>
|	入力	uiPri  = 登録プライオリティ
|			uiMode = 変更モード
*----------------------------------------------------------------------*/
void	tsk_mode_change_priority(uint uiPri, uint uiMode)
{

	int			iNum, iStart, iEnd;
	TSK_WORK	*t;


	if(uiPri == TSK_ALL)					// 全タスク対象
	{
		iStart = TSK_07;
		iEnd   = TSK_00;
	}
	else
	{
		iStart = iEnd =	uiPri;
	}

	for(int i =	iStart; i <= iEnd; i++)
	{
		if(TskCount[i] == 0){ continue;}	// プライオリティ登録数チェック
		iNum = Tsk1st[i];
		if(iNum == -1      ){ continue;}
		for(;;)
		{
			t =	&TskList[iNum];				// プライオリティワークセット
			if(!(t->ucStat & TSK_SAVE))		// SAVE	タスクは対象外となる
			{
				t->sDelay =	0;
				t->ucMode =	uiMode;
			}
											// 登録タスクの最後チェック
			if(t->sAfter ==	-1){ break;}
			iNum = t->sAfter;
		}
	}
}



/*----------------------------------------------------------------------*
| <<< 直前に登録されたタスクを実行 >>>
*----------------------------------------------------------------------*/
void	tsk_quick_call(void)
{

	if(TskStartNum	== -1){ return;}

	TSK_WORK	*t = &TskList[TskStartNum];

	void(*func)(TSK_WORK*);
	func = (void(*)(TSK_WORK*))t->uiAdrs;
	(*func)(t);
}



/*----------------------------------------------------------------------*
| <<< プライオリティ変更 >>>
|	入力	t     = タスクのワークアドレス
|			uiPri = 登録プライオリティ			
*----------------------------------------------------------------------*/
void	tsk_change_priority(TSK_WORK *t, uint uiPri)
{

	if(TSK_PRI_GET(t) == uiPri){ return;}	// 同じプライオリティはリターン

	int	iDelay = 0;

	if((uint)TSK_PRI_GET(t) < uiPri){ iDelay++;}

	TSK_WORK	*tt	= tsk_start(uiPri, (void *)t->uiAdrs, t->usID, iDelay);
	tt->ucMode = t->ucMode;
	memcpy(tt->uiWork, t->uiWork, sizeof(t->uiWork));
	tsk_end(t);
}



/*----------------------------------------------------------------------*
| <<< タスクコールサブ >>>
|	入力	t     = タスクのワークアドレス
*----------------------------------------------------------------------*/
inline	void	tsk_call_sub(TSK_WORK *t)
{

	if(t->sDelay > 0)						// ディレイチェック
	{
		t->sDelay--;
	}
	else									// 登録されたプログラムを実行
	{
		void(*func)(TSK_WORK*);
		func = (void(*)(TSK_WORK*))t->uiAdrs;
		(*func)(t);
	}
}



/*----------------------------------------------------------------------*
| <<< タスクに登録されたプログラムの実行 >>>	※ iPri に登録されたタスクを全て実行する
|	入力	uiPri = 登録プライオリティ
*----------------------------------------------------------------------*/
void	tsk_call(uint uiPri)
{

	int			no;
	TSK_WORK	*t;
	BOOL		bLoop = TRUE;

	TskExePriolity = uiPri;					// 現行のタスク番号を保存
											// タスク最初の一回呼び出す
	if(TskBeginList[TskExePriolity].uiAdrs != NULL)
	{
		tsk_call_sub(&TskBeginList[TskExePriolity]);
	}
	if(TskCount[TskExePriolity]	== 0)		// プライオリティ登録数チェック
	{
		bLoop = FALSE;
	}
	else
	{
		no = Tsk1st[TskExePriolity];
		if(no == -1){ bLoop = false;}
	}
	while(bLoop)
	{
		t =	&TskList[no];			// プライオリティワークセット
		tsk_call_sub(t);
									// 登録タスクの最後チェック
		if(t->sAfter ==	-1){ break;}
		no = t->sAfter;
	}
									// タスク最後の一回呼び出す
	if(TskEndList[TskExePriolity].uiAdrs !=	NULL)
	{
		tsk_call_sub(&TskEndList[TskExePriolity]);
	}
}



/*----------------------------------------------------------------------*
| <<< タスクの状態をチェック >>>
*----------------------------------------------------------------------*/
void	tsk_check(void)
{

	//--- 全タスク中から終了していて delay > 0 のものを探して delay-- する
	TSK_WORK	*t;
	int			i;

	for(i =	0; i < TSK_MAX;	++i)
	{
		t =	&TskList[i];
		if(t->ucStat ==	TSK_OFF	&& t->uiAdrs ==	(uint)NULL && t->sDelay	> 0)
		{
			t->sDelay--;
		}
	}

#if	TSK_DEBUG
	int		cnt	= 0;
	for(i =	0; i < TSK_PRI_MAX;	i++) cnt +=	TskCount[i];
	d3d_printf(0, 0, SCR_H - 64, 0xff0000ff, "現在のタスク数 : %d",	cnt);
#endif
}


#else

	BOOL	tsk_init(void){return TRUE;}
	void	tsk_free(void){}

#endif
