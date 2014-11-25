//
//	スクリプト処理
//

//#include	"main.h"

//--- 定数 --------------------------------------------------------
#define	BUF_MAX		30000
#define	FONT		4						// フォント番号（2 ～ 7 の範囲で）
#define	FONT_W		20
#define	FONT_H		40

#define	TEXT_X0		32						// 文字の描画開始位置
#define	TEXT_Y0		320
#define	TEXT_X1		640 - 96				// 文字の描画終了位置
#define	TEXT_Y1		480 - 16

#define	FONT_NAME	"HG丸ｺﾞｼｯｸM-PRO"		// 無い場合は同種のフォントになる

#define	GOSUB_MAX	8						// @gosub の入れ子最大数
#define	VAR_MAX		64						// 変数の最大数
#define	BG_MAX		3						// 画像の枚数

enum
{
	TEX_SCRIPT       = 40,
	TEX_SCRIPT_BG0,
	TEX_SCRIPT_BG1,
	TEX_SCRIPT_BG2,
	TEX_MENU_FRAME0,								// ここから下はメニュー用
	TEX_MENU_FRAME1,
	TEX_MENU0,
	TEX_MENU1,
	TEX_MENU2,
};


//--- 変数関係 ----------------------------------------------------
struct	VAR_WORK						// 変数の構造体
{
	char	strName[64];					// 変数名半角で 64 文字まで。
	float	fDat;							// 値
};

struct	GOSUB_WORK						// @gosub の構造体
{
	char	strName[MAX_PATH + 1];			// フルパスでファイル名が入る。
	int		iCount;							// 戻るべき場所
};

struct	SCRIPT_WORK
{
	char	strName[MAX_PATH + 1];			// 現在実行中のスクリプト名

	bool	bKeyWait;						// キー待ち
	short	sDelay;

	//--- バッファ用 ------------------------------------------------
	char	strBuf[BUF_MAX];				// スクリプト読み込みバッファ
	int		iBufCount;						// スクリプトの再生位置

	//--- 文字表示用 ------------------------------------------------
	short	sTextX,    sTextY;				// 文字の表示位置
	short	sTextAddX, sTextAddY;			// 次の文字の位置
	char	strText[256];					// 挿入文字
	int		iTextCount;


	//--- サブルーチン用 --------------------------------------------
	bool		bEnd;						// 強制終了スイッチ(true なら直ちに処理を中止します)
	GOSUB_WORK	gosub[GOSUB_MAX];			// サブルーチンワーク
	short		sGosubCount;				// サブルーチン用の入れ子数

	//--- スクリプト用変数 ------------------------------------------
	VAR_WORK	var[VAR_MAX];				// スクリプト内の変数

	//--- メニュー --------------------------------------------------
	bool		bMenu;						// true : メニュー使用中

	//--- 画像 ------------------------------------------------------
	float		fBGAlp[     BG_MAX];		// アルファ値
	float		fBGAlpSpeed[BG_MAX];		// フェード時の速度

	//--- エラー ----------------------------------------------------
	char		strError[512];				// エラーのコマンド名格納用
};

SCRIPT_WORK	script, *s;


/*---- コマンドテーブル -----------------------------*/
static	char	*CommandTbl[] =
{
	" ",					// 00 : 半角スペース
	"\t",					// 01 : タブ
	"\r\n",					// 02 : 本来の改行コード
	"//",					// 03 : コメント
	"@n",					// 04 : 改行
	"@wait",				// 05 : ウエイト
	"@clear",				// 06 : 文字列クリアー
	"@key",					// 07 : キー待ち
	"#",					// 08 : ラベル
	"@goto",				// 09 : 無条件ジャンプ
	"@gosub",				// 10 : サブルーチンコール
	"@return",				// 11 : リターン
	"$",					// 12 : 変数
	"%",					// 13 : 変数の表示
	"@if",					// 14 : 条件分岐
	"@menu",				// 15 : メニュー
	"@tex_load",			// 16 : テクスチャーロード
	"@fade_in",				// 17 : レイヤーフェードイン
	"@fade_out",			// 18 : レイヤーフェードアウト
	""
};

static	void	ComGosub(void);
static	void	ComComment(void);
static	int		ComGoto(void);



/*------------------------------------------------------------------------------*
| <<< 文字列の先頭にある空白、タブを進める >>>
|	入力	*pstrBuf = 文字列
|			piPos    = 参照位置
|	戻り値	削った文字数
*------------------------------------------------------------------------------*/
static	int		SPACE_DEL(char *pstrBuf, int *piPos)
{
	int	iLen = 0;
	for(;;)
	{
		uchar	a =pstrBuf[*piPos];
		if(a == ' ' || a == '\t'){ *piPos += 1, iLen++;}
		else					 { break;}
	}
	return	iLen;
}



/*------------------------------------------------------------------------------*
| <<< strBuf より文字列を得る >>>
|	※"(ﾀﾞﾌﾞﾙｸｫｰﾃｰｼｮﾝ)で始まり、" で終了
|	入力	strBuf = 対象文字列
|	出力	strOut = 取り出した文字列
|	戻り値	取り出した文字数
*------------------------------------------------------------------------------*/
static	int		GET_STRING(char	*strOut, char *strBuf)
{
	if(sys.bError) return 0;				// エラーが出ている場合は処理しない

	int	i, iLen	= 0;

	//--- 先頭の " を探す -----------------------------------------
	SPACE_DEL(strBuf, &iLen);
	if(strBuf[iLen]	== '"') 
	{
		iLen +=	1;
	}
	else
	{
		return 0;
	}

	//--- 後ろの " を探す -----------------------------------------
	for(i =	0;;i++,	iLen +=	1)
	{
		if(strBuf[iLen]	== NULL)
		{
			return 0;
		}
		else if(strBuf[iLen] ==	'"')
		{
			iLen +=	1;
			break;
		}
		if(ISKANJI(strBuf[iLen]))
		{ 	strOut[i + 0] =	strBuf[iLen	+ 0];
			strOut[i + 1] =	strBuf[iLen	+ 1];
			i++, iLen += 1;
		}
		else
		{
			strOut[i + 0] =	strBuf[iLen];
		}
	}
	
	//--- NULL 処理	-----------------------------------------------
	if(iLen	> 0) strOut[i] = NULL; 
	return iLen;
}



/*------------------------------------------------------------------------------*
| <<< strBuf よりラベルを取得する >>>
|	※アンダーバー、半角英数、全角文字のみを取得する。それ以外ならば終了
|	入力	strBuf = 対象文字列
|	出力	strOut = 取り出した文字列
|	戻り値	取り出した文字数
*------------------------------------------------------------------------------*/
#define	GET_LABEL	LABEL_GET
static	int		LABEL_GET(char *strOut,	char *strBuf)
{

	if(sys.bError) return 0;				// エラーが出ている場合は処理しない

	int	i, iLen	= 0;

	SPACE_DEL(strBuf, &iLen);				// 先頭の空白を削る
	for(i =	0;;	iLen++)						// strOut にラベルを取り出す
	{
		uchar	a =	strBuf[iLen];
		if((uchar)(a - '0')	<=	9){	strOut[i] =	a; i++;	continue;}	// 数字か？
		if((uchar)(a - 'a')	<  26){	strOut[i] =	a; i++;	continue;}	// 小文字か？
		if((uchar)(a - 'A')	<  26){	strOut[i] =	a; i++;	continue;}	// 大文字か？
		if((uchar)a		   == '_'){	strOut[i] =	a; i++;	continue;}	// アンダーバーか？
		if(ISKANJI(a))												// 全角？
		{
			strOut[i + 0] =	strBuf[iLen	+ 0];
			strOut[i + 1] =	strBuf[iLen	+ 1];
			i += 2,	iLen++;
			continue;
		}
		break;
	}
	strOut[i] =	NULL;
	return iLen;
}



/*------------------------------------------------------------------------------*
| <<< strBuf より数値を得る	>>>
|	入力	*Buf = 文字列の位置
|	出力	fOut = 文字列より得た数値
|	戻り値	取り出した文字数
*------------------------------------------------------------------------------*/
static	int		PARAM_GET(float	*fOut, char	*strBuf)
{

	if(sys.bError){ return 0;}				// エラーが出ている場合は処理しない

	float	fAns      =	0.f;
	float	fFlag     =	1.f;
	int		iLen      =	0;					// 文字数
	int		iSpaceLen = 0;

	iSpaceLen += SPACE_DEL(strBuf, &iLen);	// 先頭の空白を削る

	if(strBuf[iLen]	== '-')					// 一文字目はマイナスか？
	{
		fFlag  = -1;
		iLen++;
	}
	int	i;
	for(i =	0;;	i++)						// 整数部分を取り出す。
	{
		uchar	a =	strBuf[iLen] - '0';		// 一文字ゲットして文字の０を引いてみる
		if(a < 10)							// 数値の１０以下ならばそれは文字になる。
		{
			fAns  *= 10;					// 前の値を１０倍して
			fAns  += a;						// 得た値を足す。
			iLen++;							// 次の文字へ進める。
		}
		else
		{
			break;							// 数字じゃないからここでfor文抜ける
		}
	}
	if(strBuf[iLen]	!= '.')
	{
		iSpaceLen += SPACE_DEL(strBuf, &iLen);		// 後ろの空白を削る
		*fOut =	fAns * fFlag;
		if(iLen	== 0 || iSpaceLen == iLen){ sys.bError = true;}
		return iLen;								// 小数点でないなら、数値を返しておしまい。
	}
	float	fMulti = 0.1f;
	iLen++;
	for(;;i++)								// 少数部分を取り出す。
	{										// 一文字ゲットして文字の０を引いてみる
		uchar	a =	(uchar)(strBuf[iLen] - '0');
		if(a < 10)							// 数値の１０以下ならばそれは文字になる。
		{
			fAns   += (float)a * fMulti;	// 得た値をmulti倍して足す。
			fMulti *= 0.1f;					// multi部分を1/10する（小数部分を進める）。
			iLen++;							// 次の文字へ進める。
		}
		else
		{
			break;							// 数字じゃないからここでfor文抜ける
		}
	}

	if(iLen	== 0){ sys.bError = true;}

	SPACE_DEL(strBuf, &iLen);				// 後ろの空白を削る
	*fOut =	fAns * fFlag;
	return iLen;
}



/*------------------------------------------------------------------------------*
| <<< テキスト解析のエラー処理 >>>
*------------------------------------------------------------------------------*/
void	ScriptError(char *Format, ...)
{
	int	iKai = 1, iPos = 0;

	while(s->iBufCount > iPos)
	{										// 文字を進める
		if(ISKANJI(s->strBuf[iPos])) iPos += 2;
											// 改行回数を数える
		else if(strncmp(&s->strBuf[iPos], "\r\n", 2) == 0) iKai++, iPos += 2;
		else if(s->strBuf[iPos] == NULL) break;			// NULL でもループを止める。
		else iPos++;
	}

	char	str[512];
	vsprintf(str, Format, (char	*)(&Format + 1));
	sys_error("%s: %d 行目 : %s", s->strName, iKai, str);
}


/*------------------------------------------------------------------------------*
| <<< 変数名から変数番号をえる >>>			※新規に出てきたものは登録する。
|	入力	strBuf = 文字列の位置
|	出力	fOut   = 文字列から拾い出した数値
|	戻り値	変数名の登録番号
*------------------------------------------------------------------------------*/
int		VarNumGet(int *iOut, char *strBuf)
{
	int		iLen;
	char	strName[256];						// 変数名格納用

	iLen		 = GET_LABEL(strName, strBuf);
	int	iNameLen = strlen(   strName);
	int	i;
	for(i = 0;;i++)								// 変数名(name)が登録されているか調べる
	{											// 登録されていなければ登録する。
		if(strlen(s->var[i].strName) == 0)		// 文字列が登録されていなければ
		{										// 登録してチェック終了
			strcpy(s->var[i].strName, strName);	// 変数名を登録して
			break;								// ループ抜ける
		}
		if(strncmp(s->var[i].strName, strName, iNameLen) == 0)
		{
			break;								// 同じならばここでループ抜ける
		}
	}
	*iOut = i;									// 登録番号セット

	SPACE_DEL(strBuf, &iLen);					// 後ろの空白を削る
	return iLen;								// 最後に文字数を返す
}


/*------------------------------------------------------------------------------*
| <<< コマンドのパラメーターを得る >>>
|	入力	strBuf = 文字列の位置
|	出力	fOut   = 文字列から拾い出した数値
|	戻り値	文字列から拾い出した字数
*------------------------------------------------------------------------------*/
int		ParamGet(float *fOut, char *strBuf)
{
	if(sys.bError) return 0;				// エラーが発生しているときは処理しない

	float	fAns  = 0.f;
	int		iLen  = 0;
	float	fFlag = 1.f;

	SPACE_DEL(strBuf, &iLen);				// 先頭の空白を削る
	if(strBuf[iLen] == '-')					// 一文字目はマイナスか？
	{
		fFlag = -1;
		iLen++;
	}
	SPACE_DEL(strBuf, &iLen);				// 先頭の空白を削る
	if(strBuf[iLen] != '$')					// 一文字目は $ か？ $ なら else 以降の変数処理へ
	{
		iLen += PARAM_GET(&fAns, &strBuf[iLen]);
	}
	else
	{
		iLen++;								// $ を進める
		int	iID;
		iLen += VarNumGet(&iID, &strBuf[iLen]);
		fAns  = s->var[iID].fDat;
	}

	SPACE_DEL(strBuf, &iLen);				// 後ろの空白を削る
	*fOut = fAns * fFlag;

	if(sys.bError){ ScriptError("変数、数値に誤りがあります。"); return iLen;}
	return iLen;
}



/*------------------------------------------------------------------------------*
| <<< 禁則文字チェック >>>
|	戻り値	true : 禁則文字
*------------------------------------------------------------------------------*/
static	bool	IsKinsokuCheck(uchar *str)
{

	if( ! ISKANJI(str[0])) return false;		// 半角？
	
	switch((str[0] << 8) | str[1])
	{
	case '、':
	case '。':
	case '」':
	case '》':
	case '』':
	case '？':
	case '）':
	case '！':
	case '“':
	case '”':
		return true;
	}
	return false;
}


/*------------------------------------------------------------------------------*
| <<< コマンド : フェードアウト >>>			@fade_out レイヤー番号, 時間
*------------------------------------------------------------------------------*/
static	void	ComFadeOut(void)
{
	//--- レイヤー番号 --------------------------------------------
	float	fID;
	s->iBufCount += ParamGet(&fID, &s->strBuf[s->iBufCount]);
	if(fID >= 3){ sys.bError = true; return;}
	int		iID = (int)fID;

	//--- 時間 ----------------------------------------------------
	float	fTime;
	s->iBufCount++;								// カンマを読み飛ばす
	s->iBufCount += ParamGet(&fTime, &s->strBuf[s->iBufCount]);

	//--- フェードイン --------------------------------------------
	if(s->fBGAlp[iID] > 0)
	{
		s->fBGAlpSpeed[iID] = -(s->fBGAlp[iID] / fTime);
	}
}



/*------------------------------------------------------------------------------*
| <<< コマンド : フェードイン >>>			@fade_in レイヤー番号, 時間
*------------------------------------------------------------------------------*/
static	void	ComFadeIn(void)
{
	//--- レイヤー番号 --------------------------------------------
	float	fID;
	s->iBufCount += ParamGet(&fID, &s->strBuf[s->iBufCount]);
	if(fID >= 3){ sys.bError = true; return;}
	int		iID = (int)fID;

	//--- 時間 ----------------------------------------------------
	float	fTime;
	s->iBufCount++;								// カンマを読み飛ばす
	s->iBufCount += ParamGet(&fTime, &s->strBuf[s->iBufCount]);

	//--- フェードイン --------------------------------------------
	if(s->fBGAlp[iID] < 255)
	{
		s->fBGAlpSpeed[iID] = (255 - s->fBGAlp[iID]) / fTime;
	}
}


/*------------------------------------------------------------------------------*
| <<< コマンド : テクスチャー読み込み >>>	@tex_load テクスチャー番号,ファイル名
*------------------------------------------------------------------------------*/
static	void	ComTexLoad(void)
{
	//--- 読み込み番号 --------------------------------------------
	float	fID;
	s->iBufCount += ParamGet(&fID, &s->strBuf[s->iBufCount]);
	if(fID >= 3){ sys.bError = true; return;}	

	//--- ファイル名を得る ----------------------------------------
	s->iBufCount += 1;							// カンマを読み飛ばす
	char	strFile[256];
	s->iBufCount += GET_STRING(strFile, &s->strBuf[s->iBufCount]);

	//--- ここで読み込む ------------------------------------------
	FOLDER_PUSH();
	FOLDER_SET("スクリプト");
	int		b = d3_tex_load(TEX_SCRIPT_BG0 + (int)fID, strFile);
	FOLDER_POP();
	if(b == -1) ScriptError("テクスチャーが読み込めません");
}


/*------------------------------------------------------------------------------*
| <<< コマンド : メニュー >>>				@menu x, y, 項目①:項目②;
*------------------------------------------------------------------------------*/
static	void	ComMenu(void)
{
#if 0
	//--- XY 座標を取り出す -----------------------------------------
	s->iBufCount += ParamGet(&vecMenu.x, &s->strBuf[s->iBufCount]); s->iBufCount++;	// Ｘ座標
	s->iBufCount += ParamGet(&vecMenu.y, &s->strBuf[s->iBufCount]); s->iBufCount++;	// Ｙ座標

	SPACE_DEL(s->strBuf, &s->iBufCount);

	s->bMenu = true;							// このフラグが true ならメニューモード
	//--- メニューの項目を取り出す --------------------------------
	char	strMenu[128];
	int		iCount = 0;
	for(int i = 0;;)								// セミコロンを改行に変える
	{
		if(s->strBuf[s->iBufCount] == NULL) break;	// NULL でならメニュー終わり
		if(ISKANJI(s->strBuf[s->iBufCount]))		// 全角ならば文字登録
		{
			strMenu[i    ] = s->strBuf[s->iBufCount + 0];
			strMenu[i + 1] = s->strBuf[s->iBufCount + 1];
			i              += 2;
			s->iBufCount   += 2;
		}
		else if(s->strBuf[s->iBufCount] <= ' ')		// スペース以下ならば削除
		{
			s->iBufCount++;
		}
		else if(s->strBuf[s->iBufCount] == ':')		// コロンで次の項目へ
		{
			strMenu[i] = NULL;
			s->iBufCount++;
			MenuMake(iCount, strMenu);
			iCount++;
			i = 0;
		}
		else if(s->strBuf[s->iBufCount] == ';')		// セミコロンでメニュー終了
		{
			strMenu[i] = NULL;
			s->iBufCount++;
			MenuMake(iCount, strMenu);
			iCount++;
			break;
		}
		else										// 半角は登録
		{
			strMenu[i] = s->strBuf[s->iBufCount];
			i++;
			s->iBufCount++;
		}
	}
#endif
}


/*------------------------------------------------------------------------------*
| <<< コマンド : if 文 >>>					@if 条件ジャンプ
*------------------------------------------------------------------------------*/
static	void	Comif(void)
{
	bool	bJump = false;
	float	fDat, fDat2;
												// 左辺の値を得る
	s->iBufCount += ParamGet(&fDat, &s->strBuf[s->iBufCount]);
	char	*a = &s->strBuf[s->iBufCount];
	s->iBufCount += 2;							// 右辺の値を得る
	s->iBufCount += ParamGet(&fDat2, &s->strBuf[s->iBufCount]);

	//--- 条件チェック --------------------------------------------
	if(     strncmp(a, "==", 2) == 0)			// == チェック
	{
		if(fDat == fDat2) bJump = true;
	}
	else if(strncmp(a, "!=", 2) == 0)			// != チェック
	{
		if(fDat != fDat2) bJump = true;
	}
	else if(strncmp(a, "<=", 2) == 0)			// <= チェック
	{
		if(fDat <= fDat2) bJump = true;
	}
	else if(strncmp(a, ">=", 2) == 0)			// >= チェック
	{
		if(fDat >= fDat2) bJump = true;
	}
	else if(strncmp(a, "<", 1) == 0)			// < チェック
	{
		if(fDat < fDat2) bJump = true;
	}
	else if(strncmp(a, ">", 1) == 0)			// > チェック
	{
		if(fDat > fDat2) bJump = true;
	}
	//--- 条件を満たしていればジャンプ -------------------
	if(bJump)								// 中は @goto と同等です
	{
		SPACE_DEL(&s->strBuf[s->iBufCount], &s->iBufCount);
		if(strncmp(&s->strBuf[s->iBufCount], "@gosub", 6) == 0)
		{
			s->iBufCount += 6;
			ComGosub();						// 後ほどコメントをはずします。
		}
		else if(strncmp(&s->strBuf[s->iBufCount], "@goto", 5) == 0)
		{
			s->iBufCount += 5;
			ComGoto();
		}
		else
		{
			ComGoto();
		}
	}
	//--- 条件を満たしていないときの処理 -------------------
	else									// 改行コードまで進める
	{
		ComComment();						// 後ほどコメントをはずしま
	}
}


/*------------------------------------------------------------------------------*
| <<< コマンド : 変数を文字化 >>>			%d:変数名, %f:変数名
*------------------------------------------------------------------------------*/
static	void	ComVarToStr(void)
{
	int		a = s->iBufCount - 1;
	char	b = s->strBuf[s->iBufCount];		// a には文字の'd'または'f'が入る

	s->iBufCount++;								// d を進める
	SPACE_DEL(&s->strBuf[s->iBufCount], &s->iBufCount);
	s->iBufCount++;								// : を進める
	s->iBufCount++;								// $ を進める
	int		iID;								// 文字列より変数のIDを得る
	s->iBufCount += VarNumGet(&iID, &s->strBuf[s->iBufCount]);

	s->iTextCount = 0;
	if(b == 'd')								// %d ？
	{
		sprintf(s->strText, "%d", (int)s->var[iID].fDat);
	}
	else if(b == 'f')							// %f ？
	{
		sprintf(s->strText, "%.4f", s->var[iID].fDat);
	}
}


/*------------------------------------------------------------------------------*
| <<< コマンド : 変数 >>>					$ 変数
*------------------------------------------------------------------------------*/
static	void	ComVar(void)
{
	int		iNum;								// 左辺、変数の登録番号を得る
	s->iBufCount += VarNumGet(&iNum, &s->strBuf[s->iBufCount]);

	int		iType;								// 演算タイプを得る
	char	*a = &s->strBuf[s->iBufCount + 0];

	if(		strncmp(a, "+=", 2) == 0) s->iBufCount += 2, iType = 0;
	else if(strncmp(a, "-=", 2) == 0) s->iBufCount += 2, iType = 1;
	else if(strncmp(a, "*=", 2) == 0) s->iBufCount += 2, iType = 2;
	else if(strncmp(a, "/=", 2) == 0) s->iBufCount += 2, iType = 3;
	else if(strncmp(a, "=",  1) == 0) s->iBufCount++,    iType = 4;

	float	fDat;								// 右辺を得る
	s->iBufCount += ParamGet(&fDat, &s->strBuf[s->iBufCount]);

	switch(iType)								// 実際に演算
	{
	case 0: s->var[iNum].fDat += fDat; break;	// +=
	case 1: s->var[iNum].fDat -= fDat; break;	// -=
	case 2: s->var[iNum].fDat *= fDat; break;	// *=
	case 3: s->var[iNum].fDat /= fDat; break;	// /=
	case 4: s->var[iNum].fDat  = fDat; break;	// =
	}
}


/*------------------------------------------------------------------------------*
| <<< コマンド : サブルーチンコール >>>		@gosub
*------------------------------------------------------------------------------*/
static	void	ComGosub(void)
{
	for(int i = GOSUB_MAX - 1; i != 0; i--) memcpy(&s->gosub[i], &s->gosub[i - 1], sizeof(GOSUB_WORK));
	int	a = ComGoto();
	s->gosub[1].iCount = a;
	s->sGosubCount++;
	s->var[0].fDat = -100;						// メニュー選択後のジャンプ先で支障が内容に適当な値をセット
}


/*------------------------------------------------------------------------------*
| <<< コマンド : リターン >>>				@return
*------------------------------------------------------------------------------*/
static	void	ComReturn(void)
{
	if(s->sGosubCount == 0)					// @gosub で呼び出してもいないのに @return が出てきた場合
	{
		s->bEnd = true;
		return;
	}
	for(int i = 0; i < GOSUB_MAX - 1; i++) memcpy(&s->gosub[i], &s->gosub[i + 1], sizeof(GOSUB_WORK));
	ScriptLoad(s->gosub[0].strName, false);
	s->sGosubCount--;
	s->iBufCount = s->gosub[0].iCount;
	s->var[0].fDat = -100;						// メニュー選択後のジャンプ先で支障が内容に適当な値をセット
}


/*------------------------------------------------------------------------------*
| <<< コマンド : ジャンプ >>>				@goto 無条件ジャンプ
|	戻り値	行末までの文字数
*------------------------------------------------------------------------------*/
static	int		ComGoto(void)
{
	int	iLen = 0;

	SPACE_DEL(s->strBuf, &s->iBufCount);		// 先頭の空白を削る

	//--- ファイル名を得る ----------------------------------------
	bool	bFile = false;
	char	strFile[256];
	if(s->strBuf[s->iBufCount] == '"')
	{
		bFile		  = true;
		s->iBufCount += GET_STRING(strFile, &s->strBuf[s->iBufCount]);
		SPACE_DEL(s->strBuf, &s->iBufCount);
		if(s->strBuf[s->iBufCount] == ',') s->iBufCount++;
		SPACE_DEL(s->strBuf, &s->iBufCount);
	}
	//--- ラベル --------------------------------------------------
	bool	bLabel = false;
	char	strLabel[512];						// ここにラベルが格納される（半角512文字）
	char	*p;									// @goto の後に続くラベルの位置
	SPACE_DEL(s->strBuf, &s->iBufCount);
	if(s->strBuf[s->iBufCount] == '#')
	{
		bLabel      = true;
		strLabel[0] = '#';						// ラベルの先頭に # をセット
		p	        = &s->strBuf[s->iBufCount];	// p に@gotoに続くラベルの場所を格納
		s->iBufCount++;
												// ラベルを得る
		s->iBufCount += GET_LABEL(&strLabel[1], &s->strBuf[s->iBufCount]);
		strcat(strLabel, ":");
	}
	iLen = s->iBufCount;

	//--- ファイルを移動 ------------------------------------------
	if(bFile)
	{
		if(!ScriptLoad(strFile, false))
		{
			sys.bError = true;
			sprintf(s->strError, "@goto でファイル『%s』が見つかりません",  strFile);
		}
	}
	//--- ラベルを探す --------------------------------------------
	if(!sys.bError && bLabel)
	{
		int iLabelLen = strlen(strLabel);
		for(int	i =	0;;)					// 文章の先頭よりラベル名を探す
		{										// コメントは行末までスキップ
			if(s->strBuf[i] == NULL)			// エラー処理
			{
				sprintf(s->strError, "%s : @goto で『%s』というラベルが見つかりません。\r\n", s->gosub[0].strName, strLabel); break;
			}
			else if(strncmp(&s->strBuf[i], "//", 2) == 0)
			{
				i += 2;
				for(;;)
				{								// 文字を進める
					if(ISKANJI(s->strBuf[i]))	i += 2; else i++;
												// 改行をチェック、改行ならばループ止める
					if(s->strBuf[i] == '\r' && s->strBuf[i + 1] == '\n'){ i += 2; break;}
												// NULL	でもループを止める。
					if(s->strBuf[i] == NULL) break;
				}
			}
			else if(strncmp(&s->strBuf[i], strLabel, iLabelLen) == 0)
			{
				s->iBufCount = i + iLabelLen; break;
			}
			else if(ISKANJI(s->strBuf[i])) i += 2; else i++;
		}
	}
	return iLen;
}


/*------------------------------------------------------------------------------*
| <<< コマンド : ラベル >>>					# から改行までがラベルになる。ラベルは何もしない
*------------------------------------------------------------------------------*/
static	void	ComLabel(void)
{
	char	strName[512];
											// ラベルを得る
	s->iBufCount += GET_LABEL(strName, &s->strBuf[s->iBufCount]);

	//--- ラベルにセミコロンが付いていないときの処理 --------------
	if(s->strBuf[s->iBufCount] != ':')
	{
		sys.bError = true;
		sprintf(s->strError, "ラベル『%s』の末尾にセミコロンがありません。または不正な文字が使われています。", strName);
	}
	s->iBufCount++;
}


/*------------------------------------------------------------------------------*
| <<< コマンド : キーウエイト >>>			@key
*------------------------------------------------------------------------------*/
static	void	ComKeyWait(void)
{
	if(pad.bLeftPush){ s->iBufCount -= 4;}	// 
	else			 { s->bKeyWait = true;}	// このフラグをオンにすると ScriptMain 中でキー待ちになる
}


/*------------------------------------------------------------------------------*
| <<< コマンド : コメント >>>				//		※ 改行までコメント
*------------------------------------------------------------------------------*/
static	void	ComComment(void)
{
	for(;;)
	{										// 文字を進める
		if(ISKANJI(s->strBuf[s->iBufCount])) s->iBufCount += 2;
		else								 s->iBufCount += 1;
											// 改行をチェック、改行ならばループ止める
		if(s->strBuf[s->iBufCount] == '\r' && s->strBuf[s->iBufCount + 1] == '\n'){ s->iBufCount += 2; break;}
											// NULL でもループを止める。
		if(s->strBuf[s->iBufCount] == NULL) break;
	}
}


/*------------------------------------------------------------------------------*
| <<< コマンド : 改行 >>>					@n で改行です。
*------------------------------------------------------------------------------*/
static	void	ComKaigyo(void)
{
	s->sTextX      = TEXT_X0;
	s->sTextAddX   = 0;
	s->sTextY     += FONT_H + 8;
}


/*------------------------------------------------------------------------------*
| <<< コマンド : クリアー >>>				@clear
*------------------------------------------------------------------------------*/
static	void	ComClear(void)
{
	d3_offscreen_begin(TEX_SCRIPT);
	d3_clear();
	d3_offscreen_end(TEX_SCRIPT);
	s->sTextX    = TEXT_X0;
	s->sTextY    = TEXT_Y0;
	s->sTextAddX = 0;
	s->sTextAddY = 0;
}


/*------------------------------------------------------------------------------*
| <<< コマンド : ウエイト >>>				@wait で指定秒待つ。60で一秒
*------------------------------------------------------------------------------*/
static	void	ComWait(void)
{
	float	a;								// 時間を得る
	s->iBufCount += ParamGet(&a, &s->strBuf[s->iBufCount]);
	s->sDelay	  = (int)a;
}


/*------------------------------------------------------------------------------*
| <<< コマンドチェック >>>
|	戻り値	true : 一度でもコマンドを実行した
*------------------------------------------------------------------------------*/
static	bool	CommandCheck(void)
{
	
	bool	bRet = false;
	int		i;

	for(i = 0;;i++)
	{
		if(sys.bError) return bRet;				// エラーならば中止する

		int	iLen = strlen(CommandTbl[i]);		// ※strlen = 文字列の長さを調べる(コマンドの文字の長さ)
		if(iLen == 0) break;					// 文字列の長さが０なら終端とみなす(どのコマンドでもないので処理中止)
												// strncmp でスクリプトとコマンドが合致するか調べる
		if(strncmp(&s->strBuf[s->iBufCount], CommandTbl[i], iLen) == 0)
		{
			s->iBufCount += iLen;				// コマンドの文字数を進める
			int		a	  = s->iBufCount;
			bRet          = true;
												// エラーが出たときの場合に備えメッセージを作成(ここでエラーが出ているわけではない)
			sprintf(s->strError, "%s にエラーがあります。", CommandTbl[i]);
			bool	bContinue = true;
			switch(i)
			{
			case 0:			// 00 : 半角スペース
			case 1:			// 01 : タブ
			case 2:			// 02 : 本来の改行コード
				break;
			case 3:			// 03 : コメント			//
				ComComment();   break;
			case 4:			// 04 : 改行
				ComKaigyo();	break;
			case 5:			// 05 : ウエイト			@wait
				ComWait();		bContinue = false; break;
			case 6:			// 06 : クリアー			@clear
				ComClear();		break;
			case 7:			// 07 : キー待ち			@key
				ComKeyWait();	bContinue = false; break;
			case 8:			// 08 : ラベル				#		※なにも処理しないでスキップする。
				ComLabel();		break;
			case 9:			// 09 : 無条件ジャンプ		@goto	※指定ラベルまでジャンプ
				ComGoto();		break;
			case 10:		// 10 : サブルーチンコール	@gosub	※指定ラベルを呼び出す
				ComGosub();		break;
			case 11:		// 11 : リターン			@return	※@gosubで呼び出されたのを戻る
				ComReturn();	break;
			case 12:		// 12 : 変数				$
				ComVar();		break;
			case 13:		// 13 : 変数を文字へ		%d, %f
				ComVarToStr();	bContinue = false; break;
			case 14:		// 14 : 条件分岐			@if
				Comif();		break;
			case 15:		// 15 : メニュー			@menu
				ComMenu();		bContinue = false; break;
			case 16:		// 16 : テクスチャーロード	@tex_load
				ComTexLoad();	break;
			case 17:		// 17 : フェードイン		@fade_in
				ComFadeIn();	break;
			case 18:		// 18 : フェードアウト		@fade_out
				ComFadeOut();	break;
			}
			if(sys.bError)
			{
				s->iBufCount = a;
				ScriptError(s->strError);
				return true;
			}
			if(!bContinue) return true;
			i = -1;							// もう一度、CommandTblの最初から調べる
		}
	}

	if(s->strBuf[s->iBufCount] == '@' && i != 0)
	{
		sys.bError = true;
		ScriptError("不正なコマンドです。");
		return true;
	}
	return bRet;
}



/*----------------------------------------------------------------------*
| <<< スクリプトを解析 >>>
|	入力	iTex = 使用するテクスチャー番号
|	戻り値	true : スクリプトが終了した
*----------------------------------------------------------------------*/
bool	ScriptMain(void)
{
	//--- エラーが出ているならば以降、処理しない --------------------
	if(sys.bError){ return false;}

	//--- ウエイト処理 ----------------------------------------------
	if(s->sDelay > 0){ s->sDelay--; return false;}

	//--- キーウエイト処理 ------------------------------------------
	if(s->bKeyWait)									// @keyの処理
	{
		char	a[] = {(char)0xf0, NULL};
		if((sys.iTimeCount % 60) > 30) a[0]++;
		//Printf((float)s->sTextX + s->sTextAddX + 12, (float)s->sTextY + 16, 20, a);
		if(pad.bLeftRelease){ s->bKeyWait = false;}
		else				{ return false;}
	}
	//--- コマンドチェック ------------------------------------------
	while(s->strBuf[s->iBufCount] != NULL || s->strText[s->iTextCount] != NULL)
	{
		//--- コマンドチェック --------------------------------------
		bool	bRet = CommandCheck();
		if(bRet || sys.bError){ break;}							// コマンドを実行していたら文字は描画しない

		//--- 半角・全角文字描画セット ------------------------------
		int		iLen;
		char	str[3];
		if(s->strText[s->iTextCount] == NULL)
		{
			if(ISKANJI(s->strBuf[s->iBufCount])){ iLen = 2;}	// 全角
			else								{ iLen = 1;}	// 半角
			strncpy(str, &s->strBuf[s->iBufCount], iLen);
			s->iBufCount += iLen;
		}
		else
		{
			if(ISKANJI(s->strText[s->iTextCount])){ iLen = 2;}	// 全角
			else								  { iLen = 1;}	// 半角
			strncpy(str, &s->strText[s->iTextCount], iLen);
			s->iTextCount += iLen;
		}

		d3_offscreen_begin(TEX_SCRIPT);
		d3_clear_zbuffer();

		s->sTextX += s->sTextAddX;					// 文字を進める
		s->sTextY += s->sTextAddY;					// 文字を進める

		//--- 改行 --------------------------------------------------
		if(s->sTextX > TEXT_X1)						// 右端に差し掛かったら改行する
		{
			if(!IsKinsokuCheck((uchar*)str))
			{
				s->sTextX      = TEXT_X0;
				s->sTextAddX   = 0;
				s->sTextY     += FONT_H + 8;
			}
		}
		//--- 文字描画 ----------------------------------------------
		RECT	r = {s->sTextX, s->sTextY, 0, 0};
		d3.font[FONT]->DrawText(NULL, str, iLen, &r, DT_NOCLIP, D3_COL(255, 255, 255, 255));
		d3_offscreen_end(TEX_SCRIPT);

		//--- 文字を進める ------------------------------------------
		if(iLen == 1){ s->sTextAddX = (int)(FONT_W * 1.7f);}// 描画した文字の大きさをセット
		else		 { s->sTextAddX = FONT_W * 2;		   }
		if(!pad.bLeftPush){ break;}
	}
	//--- スクリプト終了チェック ------------------------------------
	if(s->strBuf[s->iBufCount] != NULL){ return true; }		// スクリプトは終了した
	else							   { return false;}
}



/*----------------------------------------------------------------------*
| <<< スクリプトを描画 >>>
*----------------------------------------------------------------------*/
void	ScriptDraw(void)
{
	for(int i = 0; i < BG_MAX; i++)
	{
		s->fBGAlp[i] += s->fBGAlpSpeed[i];
		if(s->fBGAlpSpeed[i] < 0 && s->fBGAlp[i] <=   0) s->fBGAlp[i] =   0, s->fBGAlpSpeed[i] = 0;
		if(s->fBGAlpSpeed[i] > 0 && s->fBGAlp[i] >= 255) s->fBGAlp[i] = 255, s->fBGAlpSpeed[i] = 0;

		if(s->fBGAlp[i] == 0) continue;
		d3_tex_set(TEX_SCRIPT_BG0 + i);
		d3_2d_box_fill_tex_draw(
				&VEC2(0,										0										 ),
				&VEC2(d3.texInfo[TEX_SCRIPT_BG0 + i].fOriWidth, d3.texInfo[TEX_SCRIPT_BG0 + i].fOriHeight),
				0,
				0,
				d3.texInfo[TEX_SCRIPT_BG0 + i].fU,
				d3.texInfo[TEX_SCRIPT_BG0 + i].fV,
				D3_COL((int)s->fBGAlp[i], 255, 255, 255));
	}

	d3_stat_filter_set(D3DTEXF_LINEAR);			// この設定で、文字がボケます
	d3_tex_set(TEX_SCRIPT);
	d3_stat_blend_set(D3_BLEND_NORMAL);			// 半透明設定
	d3.dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	d3.dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	d3_2d_box_fill_tex_draw(
			  &VEC2(0,		 0		),
			  &VEC2(__SCR_W, __SCR_H),
			  0,
			  0,
			  __SCR_W / d3.texInfo[TEX_SCRIPT].fWidth,
			  __SCR_H / d3.texInfo[TEX_SCRIPT].fHeight,
			  D3_COL(255, 255, 255, 255));
}



/*------------------------------------------------------------------------------*
| <<< スクリプト読み込み >>>
|	入力	*strFile = 読み込みファイル
|			bReset   = true : 画面周りをリセットする
|	戻り値	false : 読み込み失敗
*------------------------------------------------------------------------------*/
bool	ScriptLoad(char *strFile, bool bReset)
{
	HANDLE	h;										// 読み込みハンドル
	DWORD	rs;										// ダミー

	s = &script;									// スクリプトのポインタを設定
													// 以降、script.～	→	s->	となる
	//--- ファイルを開く --------------------------------------------
	strcpy(s->strName, strFile);
	FOLDER_PUSH();
	FOLDER_SET("スクリプト");
	h = CreateFile(strFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	FOLDER_POP();
	if(h == INVALID_HANDLE_VALUE) return false;

	//--- 読み込む --------------------------------------------------
	int	iSize  = GetFileSize(h, NULL) + 1;			// ファイルのサイズを得る
	memset(s->strBuf, 0, sizeof(s->strBuf));
	ReadFile(h, s->strBuf, iSize, &rs, NULL);
	CloseHandle(h);									// ファイルを閉じる

	//--- @gosub 用にファイル名を絶対パスで保存 ---------------------
	sprintf(s->gosub[0].strName, "%s", strFile);

	if(bReset)
	{
		//--- フォントとレンダリング面の作成 ------------------------
		d3_font_create(FONT, FONT_W, FONT_H, FONT_NAME, false);
														// レンダリング面を作成
		d3_offscreen_create(TEX_SCRIPT, d3.sRenderW, d3.sRenderH);
		d3.texInfo[TEX_SCRIPT].bRepair = true;			// これで画像サイズが変わっても復帰する

		//--- 文字の表示位置 ----------------------------------------
		s->sTextX    = TEXT_X0;
		s->sTextY    = TEXT_Y0;
		s->sTextAddX = 0;

		//--- 変数処理 ----------------------------------------------
		strcpy(s->var[0].strName, "メニューの答え");
	}

	s->iBufCount = 0;
	return true;
}



