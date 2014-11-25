//
//	スクリプト処理
//

//#include	"main.h"
//#include	"_text.h"
//#include	"_menu.h"


//--- マクロ ------------------------------------------------------
#define	TEXT_FONT		4					// フォント番号（2 ～ 7 の範囲で）
#define	RENDER_SIZE		512					// 文字のレンダリングサイズ
#define	TEXT_X1			0					// 文字の描画開始位置
#define	TEXT_X2			(RENDER_SIZE - 64)	// 文字の描画終了位置
#define	FONT_NAME		"HG丸ｺﾞｼｯｸM-PRO"	// 無い場合は同種のフォントになる
#define	BUF_MAX			1					// 一度に描画する文字数

#define	VAR_MAX			256					// 変数の最大数
#define	GOSUB_MAX		8					// 変数の最大数
#define	LAYER_MAX		4					// レイヤーの総数（０が一番上）
#define	LAYER_TEXT_NUM	1					// LAYER_MAX 中のテキスト面の位置

//--- 構造体 ------------------------------------------------------
struct	VAR_WORK						// 変数の構造体
{
	char	strName[64];					// 変数名半角で 64 文字まで。
	float	fDat;							// 値
};

struct	GOSUB_WORK						// 変数の構造体
{
	char	strName[MAX_PATH + 1];			// フルパスでファイル名が入る。
	int		iCount;							// 値
};

struct	LAYER_WORK						// レイヤー構造体
{
	int		iType;							// レイヤータイプ	0:画像、1～いろいろ
	int		iTex;							// テクスチャー番号
	float	fAlp;							// アルファ値
	float	fU0, fV0, fU1, fV1;				// 画像切り出し時 UV 値
	VEC3	vecP0, vecP1;					// 画像の左上と右下
};

//--- 変数関係 ----------------------------------------------------
static	int			iX, iY;					// 描画Ｘ位置、Ｙ位置
static	int			iNextX;					// 次に文字を描画するときに iX に足される値
static	char		*Text;
static	int			iCount;					// 文字の位置
static	int			iDelay = 0;				// @wait 用
											// フォントのサイズ
static	int			iWidth = 16, iHeight = 40;
static	MENU_WORK	Menu;
static	bool		bMenu;
static	bool		bKeyWait;				// true : キーウエイト
static	bool		bKeyMark;				// true : キーウエイトのマーク表示
static	DWORD		dwColor = 0xffffffff;				// 文字の色
static	char		strPath[MAX_PATH + 1];	// ベースフォルダー
static	char		strErrorCommand[ 512];	// エラーのコマンド名格納用
static	char		strBuf[BUF_MAX  + 32];	// 描画する文字
static	int			iBufCount;				// 描画する文字数

static	int			iGosubCount;
static	GOSUB_WORK	GosubWork[GOSUB_MAX];
static	VAR_WORK	Var[        VAR_MAX];
static	LAYER_WORK	LayerWork[LAYER_MAX];


/*---- コマンドテーブル -----------------------------*/
static	char	CommandTbl[][16] =
{
	" ",					// 00 : 半角スペース
	"\t",					// 01 : タブ
	"\r\n",					// 02 : 本来の改行コード
	"@n",					// 03 : 新規の改行
	"@wait",				// 04 : ウエイト
	"#",					// 05 : ラベル
	"@goto",				// 06 : 無条件ジャンプ
	"$",					// 07 : 変数
	"@if",					// 08 : 条件分岐
	"@menu",				// 09 : メニュー
	"%",					// 10 : 変数の表示
	"@clear",				// 11 : 文字列クリアー
	"@key",					// 12 : キー待ち
	"@Key",					// 13 : キー待ち（マーク表示しない）
	"@gosub",				// 14 : サブルーチンコール
	"@return",				// 15 : リターン
	"//",					// 16 : コメント
	"@tex_load",			// 17 : テクスチャーロード
	"@tex_set",				// 18 : レイヤーにテクスチャー番号設定
	"@fade_in",				// 19 : レイヤーフェードイン
	"@fade_out",			// 20 : レイヤーフェードアウト
	"@move",				// 21 : レイヤー移動
	"@sound",				// 22 : 効果音
	"@bgm",					// 23 : ＢＧＭ
	"",						// コマンドチェックの終端になる
};							// ※終端は必ず入れること


static	void	script_error(void);
static	int		VarNumGet(int *iOut, char *strBuf);
static	void	ComGosub(void);
static	void	ComComment(void);



/*------------------------------------------------------------------------------*
| <<< テキスト解析のエラー処理 >>>
*------------------------------------------------------------------------------*/
static	void	script_error(void)
{
	int	iKai = 1, iPos = 0;
	while(iCount > iPos)
	{										// 文字を進める
		if(ISKANJI(Text[iPos])) iPos += 2;
		else					iPos += 1;
											// 改行回数を数える
		if(Text[iPos] == '\r' && Text[iPos + 1] == '\n') iKai++, iPos += 2;
		if(Text[iPos] == NULL) break;		// NULL でもループを止める。
	}

	sys_error("%d 行目 : %s", iKai, strErrorCommand);
}



/*------------------------------------------------------------------------------*
| <<< コマンドのパラメーターを得る >>>
|	入力	strBuf = 文字列の位置
|	出力	fOut   = 文字列から拾い出した数値
|	戻り値	文字列から拾い出した字数
*------------------------------------------------------------------------------*/
static	int		ParamGet(float *fOut, char *strBuf)
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
		fAns   = Var[iID].fDat;
	}

	SPACE_DEL(strBuf, &iLen);				// 後ろの空白を削る
	*fOut = fAns * fFlag;

	if(sys.bError){ script_error(); return iLen;}

	return iLen;
}



/*------------------------------------------------------------------------------*
| <<< スクリプト初期化 >>>
|	入力	iTex = 使用するテクスチャー番号
*------------------------------------------------------------------------------*/
void	script_init(int iTex)
{
											// フォントの作成
	d3_font_create(TEXT_FONT, 16, 40, "HG丸ｺﾞｼｯｸM-PRO", false);

	d3_offscreen_create(iTex, RENDER_SIZE, RENDER_SIZE);
	d3.texInfo[iTex].bRepair = true;		// このスイッチを true にするとフルスクリーン切り替え時に画像が保持される
	iX     = iNextX = iY = iCount = 0;
	iDelay = 0;
	sprintf(Var[0].strName, "メニューの答え");
	sprintf(Var[1].strName, "マウスのＸ座標");
	sprintf(Var[2].strName, "マウスのＹ座標");

	//--- 文字の色 ------------------------------------------------
	dwColor	= D3_COL(255, 255, 255, 255);

	//--- 画像ワーク初期化 ----------------------------------------
	int	i;
	for(i = 0; i < LAYER_MAX; i++)
	{
		LayerWork[i].fAlp  = 0;
		LayerWork[i].fU0   = LayerWork[i].fV0 = 0;
		LayerWork[i].fU1   = LayerWork[i].fV1 = 1;
		LayerWork[i].vecP0 = VEC3(  0,   0, 0);
		LayerWork[i].vecP1 = VEC3(640, 480, 0);
	}
	//--- テキスト面初期化 ----------------------------------------
	i				  = LAYER_TEXT_NUM;
	LayerWork[i].fAlp = 255;
	LayerWork[i].iTex = iTex;

	//--- フォルダーセット ----------------------------------------
	sprintf(strPath, "%sスクリプト\\", sys.strExeFolder);
}



/*------------------------------------------------------------------------------*
| <<< スクリプト開放 >>>
*------------------------------------------------------------------------------*/
void	script_free(void)
{
	if(Text != NULL)						// メモリーを開放
	{
		free(Text);
		Text = NULL;
	}
}



/*------------------------------------------------------------------------------*
| <<< スクリプト終了 >>>
|	入力	iTex = 使用していたテクスチャー番号
*------------------------------------------------------------------------------*/
void	script_end(int iTex)
{
	d3_offscreen_free(iTex);				// レンダリング面を開放
}



/*------------------------------------------------------------------------------*
| <<< 禁則文字チェック >>>
|	戻り値	true : 禁則文字
*------------------------------------------------------------------------------*/
static	bool	IsKinsokuCheck(uchar *str)
{

	if(ISKANJI(str)) return false;		// 半角？
	
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
| <<< 変数名から変数番号をえる >>>			※新規に出てきたものは登録する。
|	入力	strBuf = 文字列の位置
|	出力	fOut   = 文字列から拾い出した数値
|	戻り値	変数名の登録番号
*------------------------------------------------------------------------------*/
static	int		VarNumGet(int *iOut, char *strBuf)
{
	int		iLen;
	char	strName[256];					// 変数名格納用

	iLen		 = GET_LABEL(strName, strBuf);
	int	iNameLen = strlen(   strName);
	int	i;
	for(i = 0;;i++)							// 変数名(name)が登録されているか調べる
	{										// 登録されていなければ登録する。
		if(strlen(Var[i].strName) == 0)		// 文字列が登録されていなければ
		{									// 登録してチェック終了
			strcpy(Var[i].strName, strName);// 変数名を登録して
			break;							// ループ抜ける
		}
		if(strncmp(Var[i].strName, strName, iNameLen) == 0)
		{
			break;							// 同じならばここでループ抜ける
		}
	}
	*iOut = i;								// 登録番号セット

	SPACE_DEL(strBuf, &iLen);				// 後ろの空白を削る
	return iLen;							// 最後に文字数を返す
}



/*------------------------------------------------------------------------------*
| <<< コマンド : 改行 >>>					@n で改行です。
*------------------------------------------------------------------------------*/
static	void	ComKaigyo(void)
{
	strBuf[iBufCount] = '\n';				// バッファに改行マークをいれる。
	iBufCount++;
}



/*------------------------------------------------------------------------------*
| <<< コマンド : ウエイト >>>				@wait で指定秒待つ。60で一秒
*------------------------------------------------------------------------------*/
static	void	ComWait(void)
{
	//--- 時間 ----------------------------------------------------
	float	a;
	iCount += ParamGet(&a, &Text[iCount]);
	iDelay  = (int)a;
}



/*------------------------------------------------------------------------------*
| <<< コマンド : ラベル >>>					# から改行までがラベルになる。ラベルは何もしない
*------------------------------------------------------------------------------*/
static	void	ComLabel(void)
{
	char	strName[512];
											// ラベルを得る
	iCount += GET_LABEL(strName, &Text[iCount]);

	//--- ラベルにセミコロンが付いていないときの処理 --------------
	if(Text[iCount] != ':')
	{
		sys.bError = true;
		sys_error("ラベル『%s』にセミコロンがありません。または不正な文字が使われています。", strName);
	}
	iCount++;
}



/*------------------------------------------------------------------------------*
| <<< コマンド : ジャンプ >>>				@goto 無条件ジャンプ
|	戻り値	行末までの文字数
*------------------------------------------------------------------------------*/
static	int		ComGoto(void)
{
	int	iLen = 0;

	SPACE_DEL(Text, &iCount);				// 先頭の空白を削る

	//--- ファイル名を得る ----------------------------------------
	bool	bFile = false;
	char	strFile[256];
	if(Text[iCount] == '"')
	{
		bFile   = true;
		iCount += GET_STRING(strFile, &Text[iCount]);
		SPACE_DEL(Text, &iCount);
		if(Text[iCount] == ',') iCount++;
		SPACE_DEL(Text, &iCount);
	}
	//--- ラベル --------------------------------------------------
	bool	bLabel = false;
	char	strLabel[512];					// ここにラベルが格納される（半角512文字）
	char	*p;								// @goto の後に続くラベルの位置
	SPACE_DEL(Text, &iCount);
	if(Text[iCount] == '#')
	{
		bLabel      = true;
		strLabel[0] = '#';					// ラベルの先頭に # をセット
		p		    = &Text[iCount];		// p に@gotoに続くラベルの場所を格納
		iCount++;
											// ラベルを得る
		iCount += GET_LABEL(&strLabel[1], &Text[iCount]);
		strcat(strLabel, ":");
	}
	iLen = iCount;
	//--- ファイルを移動 ------------------------------------------
	if(bFile)
	{
		script_load(strFile);
		iCount	  = 0;
	}
	//--- ラベルを探す --------------------------------------------
	if(!sys.bError && bLabel)
	{
		int iLabelLen = strlen(strLabel);
		for(int i = 0;;i++)					// 文章の先頭よりラベル名を探す
		{									// @goto に続くラベルはここでスキップ
			if(p == &Text[i]) continue;
			if(strncmp(&Text[i], strLabel, iLabelLen) == 0)
			{
				iCount = i + iLabelLen; break;
			}
			if(Text[i] == NULL)				// エラー処理
			{
				sys.bError = true;
				sys_error("@goto でジャンプ先のラベル『%s』が見つかりません",  strLabel); break;
			}
		}
	}
	return iLen;
}



/*------------------------------------------------------------------------------*
| <<< コマンド : 変数 >>>					$ 変数
*------------------------------------------------------------------------------*/
static	void	ComVar(void)
{
	int		iNum;							// 左辺、変数の登録番号を得る
	iCount += VarNumGet(&iNum, &Text[iCount]);

	int		iType;							// 演算タイプを得る
	char	a = Text[iCount + 0];
	char	b = Text[iCount + 1];

	if(		a == '+' && b == '=') iCount += 2, iType = 0;
	else if(a == '-' && b == '=') iCount += 2, iType = 1;
	else if(a == '*' && b == '=') iCount += 2, iType = 2;
	else if(a == '/' && b == '=') iCount += 2, iType = 3;
	else if(a == '='            ) iCount++,    iType = 4;

	float	fDat;							// 右辺を得る
	iCount += ParamGet(&fDat, &Text[iCount]);

	switch(iType)							// 実際に演算
	{
	case 0: Var[iNum].fDat += fDat; break;	// +=
	case 1: Var[iNum].fDat -= fDat; break;	// -=
	case 2: Var[iNum].fDat *= fDat; break;	// *=
	case 3: Var[iNum].fDat /= fDat; break;	// /=
	case 4: Var[iNum].fDat  = fDat; break;	// =
	}
}



/*------------------------------------------------------------------------------*
| <<< コマンド : if 文 >>>					@if 条件ジャンプ
*------------------------------------------------------------------------------*/
static	void	Comif(void)
{
	bool	bJump = false;
	float	fDat, fDat2;
											// 左辺の値を得る
	iCount += ParamGet(&fDat, &Text[iCount]);
	char	a = Text[iCount + 0];
	char	b = Text[iCount + 1];

	iCount += 2;							// 右辺の値を得る
	iCount += ParamGet(&fDat2, &Text[iCount]);
	//--- 条件チェック --------------------------------------------
	if(a == '=' && b == '=')				// == チェック
	{
		if(fDat == fDat2) bJump = true;
	}
	else if(a == '!' && b == '=')			// != チェック
	{
		if(fDat != fDat2) bJump = true;
	}
	else if(a == '<' && b == '=')			// <= チェック
	{
		if(fDat <= fDat2) bJump = true;
	}
	else if(a == '>' && b == '=')			// >= チェック
	{
		if(fDat >= fDat2) bJump = true;
	}
	else if(a == '<')						// < チェック
	{
		if(fDat < fDat2) bJump = true;
	}
	else if(a == '>')						// > チェック
	{
		if(fDat > fDat2) bJump = true;
	}
	//--- 条件を満たしていればジャンプ -------------------
	if(bJump)								// 中は @goto と同等です
	{
		SPACE_DEL(Text, &iCount);
		if(strncmp(&Text[iCount], "@gosub", 6) == 0)
		{
			iCount += 6;
			ComGosub();
		}
		else if(strncmp(&Text[iCount], "@goto", 5) == 0)
		{
			iCount += 5;
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
		ComComment();
	}
}



/*------------------------------------------------------------------------------*
| <<< コマンド : メニュー >>>				@menu 条件ジャンプ
*------------------------------------------------------------------------------*/
static	void	ComMenu(void)
{
	iCount += ParamGet(&Menu.fX, &Text[iCount]); iCount++;	// Ｘ座標
	iCount += ParamGet(&Menu.fY, &Text[iCount]); iCount++;	// Ｙ座標

	SPACE_DEL(Text, &iCount);

	bMenu = true;							// このフラグが true ならメニューモード
	//--- メニューの項目を取り出す --------------------------------
	int	iStrMax  = 0;
	Menu.iStrMax = 0;
	Menu.iLine   = 0;
	for(int i = 0;;)						// セミコロンを改行に変える
	{
		if(Text[iCount] == NULL) break;		// NULL でならメニュー終わり
		if(ISKANJI(Text[iCount]))			// 全角ならば文字登録
		{
			Menu.str[i    ] = Text[iCount    ];
			Menu.str[i + 1] = Text[iCount + 1];
			i              += 2;
			iCount         += 2;
			iStrMax		   += 2;
		}
		else if(Text[iCount] <= ' ')		// スペース以下ならば削除
		{
			iCount++;
		}
		else if(Text[iCount] == ':')		// コロンで次の項目へ
		{
			Menu.str[i] = '\n';
			i++;
			iCount++;
			Menu.iLine++;
			if(Menu.iStrMax < iStrMax) Menu.iStrMax = iStrMax;
			iStrMax = 0;
		}
		else if(Text[iCount] == ';')		// セミコロンでメニュー終了
		{
			iCount++;
			Menu.iLine++;
			if(Menu.iStrMax < iStrMax) Menu.iStrMax = iStrMax;
			break;
		}
		else								// 半角は登録
		{
			Menu.str[i] = Text[iCount];
			i++;
			iCount++;
			iStrMax++;
		}
	}
	MenuMake(Menu.str);					// メニュー作成
	Menu.bFirst = false;
}



/*------------------------------------------------------------------------------*
| <<< コマンド : 変数を文字化 >>>			%d:変数名, %f:変数名
*------------------------------------------------------------------------------*/
static	void	ComVarToStr(void)
{
	char	a = Text[iCount];

	iCount++;								// d を進める
	SPACE_DEL(Text, &iCount);
	iCount++;								// : を進める
	iCount++;								// $ を進める
	int	iID;
											// 文字列より変数のIDを得る
	iCount += VarNumGet(&iID, &Text[iCount]);

	char	str[256];
	if(a == 'd')							// %d ？
	{
		sprintf(str, "%d", (int)Var[iID].fDat);
	}
	else if(a == 'f')						// %f ？
	{
		sprintf(str, "%f", Var[iID].fDat);
	}

	strcpy(&strBuf[iBufCount], str);		// 文字を登録する
	iBufCount += strlen(str);
}



/*------------------------------------------------------------------------------*
| <<< コマンド : クリアー >>>				@c
*------------------------------------------------------------------------------*/
static	void	ComClear(void)
{
	strBuf[iBufCount] = '\b';				// バッファに BS マークをいれる。
	iBufCount++;
}



/*------------------------------------------------------------------------------*
| <<< コマンド : キーウエイト >>>			@key
*------------------------------------------------------------------------------*/
static	void	ComKeyWait(void)
{
	bKeyWait = true;						// このフラグをオンにすると TextMain 中でキー待ちになる
	bKeyMark = true;
}



/*------------------------------------------------------------------------------*
| <<< コマンド : キーウエイト >>>			@Key
*------------------------------------------------------------------------------*/
static	void	ComKeyWaitNoMark(void)
{
	bKeyWait = true;						// このフラグをオンにすると TextMain 中でキー待ちになる
	bKeyMark = false;
}



/*------------------------------------------------------------------------------*
| <<< コマンド : サブルーチンコール >>>		@gosub
*------------------------------------------------------------------------------*/
static	void	ComGosub(void)
{
	for(int i = GOSUB_MAX - 1; i != 0; i--) memcpy(&GosubWork[i], &GosubWork[i - 1], sizeof(GOSUB_WORK));
	int	a = ComGoto();
	GosubWork[1].iCount = a;
	iGosubCount++;
	Var[0].fDat = -100;						// メニュー選択後のジャンプ先で支障が内容に適当な値をセット
}



/*------------------------------------------------------------------------------*
| <<< コマンド : リターン >>>				@return
*------------------------------------------------------------------------------*/
static	void	ComReturn(void)
{
	if(iGosubCount == 0)
	{
		free(Text);	Text = NULL; return;
	}
	for(int i = 0; i < GOSUB_MAX - 1; i++) memcpy(&GosubWork[i], &GosubWork[i + 1], sizeof(GOSUB_WORK));
	script_load(GosubWork[0].strName);
	iGosubCount--;
	iCount		= GosubWork[0].iCount;
	Var[0].fDat = -100;						// メニュー選択後のジャンプ先で支障が内容に適当な値をセット
}



/*------------------------------------------------------------------------------*
| <<< コマンド : コメント >>>				//		※ 改行までコメント
*------------------------------------------------------------------------------*/
static	void	ComComment(void)
{
	for(;;)
	{										// 文字を進める
		if(ISKANJI(Text[iCount])) iCount += 2;
		else					  iCount += 1;
											// 改行をチェック、改行ならばループ止める
		if(Text[iCount] == '\r' && Text[iCount + 1] == '\n'){ iCount += 2; break;}
											// NULL でもループを止める。
		if(Text[iCount] == NULL) break;
	}
}



/*------------------------------------------------------------------------------*
| <<< コマンド : テクスチャー読み込み >>>	@tex_load テクスチャー番号,ファイル名
*------------------------------------------------------------------------------*/
static	void	ComTexLoad(void)
{
	//--- 読み込み番号 --------------------------------------------
	float	fID;
	iCount += ParamGet(&fID, &Text[iCount]);

	//--- ファイル名を得る ----------------------------------------
	iCount += 1;							// カンマを読み飛ばす
	char	strFile[256];
	iCount += GET_STRING(strFile, &Text[iCount]);

	//--- ここで読み込む ------------------------------------------
	FOLDER_SET("スクリプト\\画像");
	bool	b = (d3_tex_load((int)fID, strFile) != -1);
	if(!b){ script_error(); return;}
}



/*------------------------------------------------------------------------------*
| <<< コマンド : テクスチャー番号設定 >>>	@tex_set レイヤー番号,テクスチャー番号
*------------------------------------------------------------------------------*/
static	void	ComTexSet(void)
{
	//--- レイヤー番号 --------------------------------------------
	float	fLay;
	iCount += ParamGet(&fLay, &Text[iCount]);

	//--- テクスチャー番号 ----------------------------------------
	iCount += 1;							// カンマを読み飛ばす
	float	fTex;
	iCount += ParamGet(&fTex, &Text[iCount]);
	
	int	iLay = (int)fLay;
	int	iTex = (int)fTex;
	LayerWork[iLay].iTex = iTex;			// 番号設定

	//--- ＵＶ設定 ------------------------------------------------
	LayerWork[iLay].fU1 = d3.texInfo[iTex].fU;
	LayerWork[iLay].fV1 = d3.texInfo[iTex].fV;

	//--- テクスチャーサイズより画像サイズを計算する --------------
	LayerWork[iLay].vecP0   = VEC3(0, 0, 0);
	LayerWork[iLay].vecP1.x = d3.texInfo[iTex].fOriWidth;
	LayerWork[iLay].vecP1.y = d3.texInfo[iTex].fOriHeight;
}



/*------------------------------------------------------------------------------*
| <<< コマンド : フェードイン >>>			@fade_in レイヤー番号, 時間
*------------------------------------------------------------------------------*/
static	void	ComFadeIn(void)
{
	//--- レイヤー番号 --------------------------------------------
	float	fLay;
	iCount += ParamGet(&fLay,  &Text[iCount]);	// レイヤー番号

	//--- 時間 ----------------------------------------------------
	float	fTime;
	iCount += 1;								// カンマを読み飛ばす
	iCount += ParamGet(&fTime, &Text[iCount]);	// テクスチャー番号。この画像が表示される

	//--- フェードイン --------------------------------------------
	int	iID             = (int)fLay;
	int	iTime           = (int)fTime;
	LayerWork[iID].fAlp = 0;
	//			対象変数		  終了値　時間  遅延時間
	MoveStart(&LayerWork[iID].fAlp, 255,  iTime, 0);
}



/*------------------------------------------------------------------------------*
| <<< コマンド : フェードアウト >>>			@fade_out レイヤー番号, 時間
*------------------------------------------------------------------------------*/
static	void	ComFadeOut(void)
{
	//--- レイヤー番号 --------------------------------------------
	float	fLay;
	iCount += ParamGet(&fLay,  &Text[iCount]);	// レイヤー番号

	//--- 時間 ----------------------------------------------------
	float	fTime;
	iCount += 1;								// カンマを読み飛ばす
	iCount += ParamGet(&fTime, &Text[iCount]);	// テクスチャー番号。この画像が表示される

	//--- フェードアウト ------------------------------------------
	int	iID             = (int)fLay;
	int	iTime           = (int)fTime;
	LayerWork[iID].fAlp = 255;
	//			対象変数		  終了値 時間  遅延時間
	MoveStart(&LayerWork[iID].fAlp, 0,  iTime, 0);
}



/*------------------------------------------------------------------------------*
| <<< コマンド : レイヤー移動 >>>			@move レイヤー番号, Ｘ座標,Ｙ座標,時間
*------------------------------------------------------------------------------*/
static	void	ComMove(void)
{
	//--- レイヤー番号 --------------------------------------------
	float	fLay;
	iCount += ParamGet(&fLay,  &Text[iCount]);	// レイヤー番号

	//--- Ｘ座標 --------------------------------------------------
	float	fX;
	iCount += 1;								// カンマを読み飛ばす
	iCount += ParamGet(&fX, &Text[iCount]);	// テクスチャー番号。この画像が表示される

	//--- Ｙ座標 --------------------------------------------------
	float	fY;
	iCount += 1;								// カンマを読み飛ばす
	iCount += ParamGet(&fY, &Text[iCount]);	// テクスチャー番号。この画像が表示される

	//--- 時間 ----------------------------------------------------
	float	fTime;
	iCount += 1;								// カンマを読み飛ばす
	iCount += ParamGet(&fTime, &Text[iCount]);	// テクスチャー番号。この画像が表示される

	//--- レイヤー移動 --------------------------------------------
	int		iID   = (int)fLay;
	int		iTime = (int)fTime;
	float	fW    = LayerWork[iID].vecP1.x - LayerWork[iID].vecP0.x;
	float	fH    = LayerWork[iID].vecP1.y - LayerWork[iID].vecP0.y;
	//			対象変数               終了値   時間  遅延時間
	MoveStart(&LayerWork[iID].vecP0.x, fX,      iTime, 0);
	MoveStart(&LayerWork[iID].vecP0.y, fY,      iTime, 0);
	MoveStart(&LayerWork[iID].vecP1.x, fX + fW, iTime, 0);
	MoveStart(&LayerWork[iID].vecP1.y, fY + fH, iTime, 0);
}



/*------------------------------------------------------------------------------*
| <<< コマンドチェック >>>
|	戻り値	true : 一度でもコマンドを実行した
*------------------------------------------------------------------------------*/
static	bool	CommandCheck(void)
{
	bool	bRet = false;

	for(int i = 0;;i++)
	{
		if(sys.bError) return bRet;
		int	iLen = strlen(CommandTbl[i]);	// ※strlen = 文字列の長さを調べる
		if(iLen == 0) break;				// 文字列の長さが０なら終端とみなす
		if(strncmp(&Text[iCount], CommandTbl[i], iLen) == 0)
		{
			iCount += iLen;					// コマンドの文字数を進める
			bRet    = true;
			sprintf(strErrorCommand, "%s にエラーがあります。", CommandTbl[i]);
			switch(i)
			{
			case 0:			// 00 : 半角スペース
			case 1:			// 01 : タブ
			case 2:			// 02 : 本来の改行コード
				break;
			case 3:			// 03 : 新規の改行			@n
				ComKaigyo(); break;
			case 4:			// 04 : ウエイト			@wait
				ComWait(); return true;
			case 5:			// 05 : ラベル				#		※なにも処理しないでスキップする。
				ComLabel(); break;
			case 6:			// 06 : 無条件ジャンプ		@goto	※指定ラベルまでジャンプ
				ComGoto(); break;
			case 7:			// 07 : 変数				$
				ComVar();  break;
			case 8:			// 08 : 条件分岐			@if
				Comif(); break;
			case 9:			// 09 : メニュー			@menu
				ComMenu(); return true;
			case 10:		// 10 : 変数を文字へ		%d, %f
				ComVarToStr(); break;
			case 11:		// 11 : クリアー			@clear
				ComClear(); break;
			case 12:		// 12 : キー待ち			@key
				ComKeyWait();        return true;
			case 13:		// 13 : キー待ち			@Key
				ComKeyWaitNoMark();  return true;
			case 14:		// 14 : サブルーチンコール	@gosub	※指定ラベルを呼び出す
				ComGosub(); return true;
			case 15:		// 15 : リターン			@return	※@gosubで呼び出されたのを戻る
				ComReturn(); return true;
			case 16:		// 16 : コメント			//
				ComComment();   break;
			case 17:		// 17 : テクスチャーロード	@tex_load
				ComTexLoad();  break;
			case 18:		// 18 : テクスチャー番号設定@tex_set
				ComTexSet();   break;
			case 19:		// 19 : フェードイン		@fade_in
				ComFadeIn();   break;
			case 20:		// 20 : フェードアウト		@fade_out
				ComFadeOut(); break;
			case 21:		// 21 : レイヤー移動		@move
				ComMove();	  break;
			}
			i = -1;							// もう一度、CommandTblの最初から調べる
		}
	}
	//--- 不正なコマンド処理 --------------------------------------
	if(Text[iCount] == '@')
	{
		sprintf(strErrorCommand, "不明なコマンドがあります。");
		script_error();
	}
	return bRet;
}



/*----------------------------------------------------------------------*
| <<< スクリプトメイン処理 >>>			※この関数内で描画処理は行いません
|	戻り値	true : スクリプト処理は終了した
*----------------------------------------------------------------------*/
bool	script_main(void)
{
	if(Text == NULL)  return false;			// スクリプト未ロード時には以下を処理しない。
	if(sys.bError)    return false;
	if(iBufCount > 0) return false;

	//--- メニュー ------------------------------------------------
	if(bMenu)
	{
		int a = MenuMain(&Menu);			// メニュー選択時、メニュー表示オフ、iAns に一時答えを退避
		if(a >= 0){	Var[0].fDat = (float)a; bMenu = false;}
		return false;
	}
	//--- ウエイトチェック ------------------------------------
	if(iDelay > 0){ iDelay--; return false;}	// @wait 時の処理。iDelay が０になるまで以下を処理しない
	
	//--- キーウエイト --------------------------------------------
	if(iDelay == 0 && bKeyWait)
	{
		if(bKeyMark)
		{
			char	a[] = {(char)0xf0, NULL};
			if((sys.iTimeCount % 60) > 30) a[0]++;
			float	x = (iX / 512.f) * 640.f;
			float	y = (iY / 512.f) * 480.f;
			Printf(iNextX + x, y, (float)d3.fontInfo[TEXT_FONT].Height * 0.8f, a);
		}
		if(pad.cl || pad.key[KEY_Z]) bKeyWait = false;
		else						 return false;
	}
	//--- テクスチャーに描画開始 ----------------------------------
	int		iLen;
											// 文字列の終端ならば処理しない
	while(Text[iCount] != NULL)
	{
		if(sys.bError) return false;
		//--- コマンドチェック ------------------------------------
		bool	bRet = CommandCheck();
		if(bRet) break;						// コマンドを実行していたら文字は描画しない

		//--- 半角・全角文字描画セット ----------------------------
		if(ISKANJI(Text[iCount])) iLen = 2;	// 全角
		else					  iLen = 1;	// 半角

		for(int i = 0; i < iLen; i++)
		{
			strBuf[iBufCount] = Text[iCount];
			iBufCount++;
			iCount++;
		}
		if(iBufCount + 2 > BUF_MAX) break;
	}

	//--- スクリプト終了チェック ----------------------------------
	if(Text == NULL) return true;			// スクリプトは終了した
	else			 return false;
}



/*----------------------------------------------------------------------*
| <<< レイヤーを描画 >>>
|	入力	iID = レイヤー番号
*----------------------------------------------------------------------*/
static	void	BgDraw(int iID)
{
	LAYER_WORK	*w = &LayerWork[iID];

	if(w->fAlp <= 0) return;
/*
	D3_ZREAD( false);
	D3_ZWRITE(false);
	D3_TEXTURE(0, w->iTex);
	D3_2DDRAW(&w->vecP0,					// 左上の座標
			  &w->vecP1,					// 右下の座標
			  w->fU0, w->fV0,
			  w->fU1, w->fV1,
			  D3_COL((int)w->fAlp, 255, 255, 255));
	D3_ZREAD( true);
	D3_ZWRITE(true);

//
return;//
*/
	
	int	type = D3_SHADER_TYPE_CHARAOUTLINE;

	if(d3_shader_set(type))
	{
		d3_mesh_draw_1st(type);
		uint	uiPasses;
		d3.eff->Begin(&uiPasses, D3DXFX_DONOTSAVESTATE);

		d3.eff->SetTexture("texDecale", d3.tex[w->iTex]);
		d3.eff->Pass(0);

		d3.eff->SetFloat("g_fWidth",  d3.texInfo[w->iTex].fWidth);
		d3.eff->SetFloat("g_fHeight", d3.texInfo[w->iTex].fHeight);

		D3_VTX_LX_	vt[] = {-1, -1, 0, D3_COL_NORMAL,  0, 1,
							-1,  1, 0, D3_COL_NORMAL,  0, 0,
							 1,  1, 0, D3_COL_NORMAL,  1, 0,
							 1, -1, 0, D3_COL_NORMAL,  1, 1};

		d3.dev->SetFVF(D3_FVF_LX);
		d3.dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vt, sizeof(D3_VTX_LX));

		d3.eff->End();
		d3.iPolyCount += 2;

		d3.eff->SetTexture("texDecale", NULL);
		d3_mesh_draw_3rd();
	}
}



/*----------------------------------------------------------------------*
| <<< スクリプト描画 >>>
|	入力	iTex = 使用するテクスチャー番号
*----------------------------------------------------------------------*/
void	script_draw(int iTex)
{
//	if(sys.bFrameSkip) return;

	//--- メニュー ------------------------------------------------
	if(bMenu) MenuDraw(&Menu);

	//--- 文字描画 ------------------------------------------------
	if(iBufCount > 0)
	{
		d3_offscreen_begin(iTex);
		d3.dev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3_COL(0, 0, 0, 0), 1.0f, 0);
		int	i;
		for(i = 0; i < BUF_MAX;)
		{
			int	iLen;
			if(ISKANJI(strBuf[i])) iLen = 2;	// 全角
			else				   iLen = 1;	// 半角

			if(strBuf[i] == '\n')				// 改行
			{
				iX     = TEXT_X1;
				iNextX = 0;
				iY    += d3.fontInfo[TEXT_FONT].Height;
				i++;
				continue;
			}
			else if(strBuf[i] == '\b')			// BS（画面クリアー）
			{
				iX     = TEXT_X1;
				iNextX = 0;
				iY     = 0;
//				d3.dev->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOL(255, 128, 0, 0), 1.0f, 0);
				d3.dev->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, d3.dwClearCol, 1.0f, 0);
				i++;
				continue;
			}

			iX += iNextX;
			if(iX >= TEXT_X2)					// 行末まできたか？
			{									// 以下の if 文で禁則処理
				if(!IsKinsokuCheck((uchar *)&strBuf[i]))
				{
					iX  = TEXT_X1;				// 改行
					iY += d3.fontInfo[TEXT_FONT].Height;
				}
			}
			RECT	r = {iX, iY, 0, 0};
			d3.font[TEXT_FONT]->DrawText(NULL, &strBuf[i], iLen, &r, DT_NOCLIP, dwColor);
			if(iLen == 1) iNextX = (int)((float)d3.fontInfo[TEXT_FONT].Width * 1.7f);
			else		  iNextX = (int)((float)d3.fontInfo[TEXT_FONT].Width * (float)iLen);

			i += iLen;
		}
		d3_offscreen_end(iTex);

		iBufCount -= i;
		memcpy(strBuf, &strBuf[i], sizeof(strBuf) - i);
	}
	//--- レイヤー ------------------------------------------------
	D3_MIN_MAG_FILTER(0, D3DTEXF_LINEAR);	// この設定で、文字がボケます
	D3_BLEND_NORMAL();						// 半透明設定
	d3.dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	for(int i = LAYER_MAX - 1; i != -1; i--)
	{										// テキスト面のみこの設定
		if(LayerWork[i].iTex == iTex) d3.dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		else						  d3.dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		switch(LayerWork[i].iType)
		{
		case 0:
			BgDraw(i);
			break;
		}
	}
	d3.dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
}



/*------------------------------------------------------------------------------*
| <<< スクリプトで文字表示 >>>
|	入力	Format = printfと同じです
*------------------------------------------------------------------------------*/
void	script_printf(char *Format,...)
{

	script_free();

	//--- 文字列作成 ------------------------------------------------
    char	strBuf[1024];
	va_list	ap;
	va_start(ap,	 Format);
	vsprintf(strBuf, Format, ap);
	va_end(ap);

	//--- サイズを取得する ------------------------------------------
	int	iSize = strlen(strBuf);
		
	//--- メモリーを確保する ----------------------------------------
	iSize += 1;								// +1 をしてNULLを追加
	Text  = (char *)malloc(iSize);
	if(Text == NULL) sys_error("script_printf: %s の文章でメモリーの確保に失敗しました。", strBuf);
	memset(Text, 0, iSize);					// ゼロクリアー
}



/*------------------------------------------------------------------------------*
| <<< スクリプト読み込み >>>
|	入力	*strFile = 読み込みファイル
*------------------------------------------------------------------------------*/
bool	script_load(char *strFile)
{
	HANDLE	Handle;							// 読み込みハンドル
	DWORD	rs;								// ダミー
	int		iSize;							// 読み込みサイズ

	//--- メモリー開放 --------------------------------------------
	if(Text != NULL){ free(Text); Text = NULL;}

	//--- ファイルを開く --------------------------------------
	//char	str[MAX_PATH + 1];
	//sprintf(str, "%s%s", strPath, strFile);
	Handle = CreateFile(strFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	if(Handle == INVALID_HANDLE_VALUE){ return sys_error("%s が開けません。", strFile);}

	sprintf(sys.strName, "%s", strFile);

	//--- サイズを取得する ------------------------------------
	iSize = GetFileSize(Handle, NULL);
	if(iSize == 0){ CloseHandle(Handle); return sys_error("%s のサイズが０バイトです。", strFile);}
		
	//--- メモリーを確保する ----------------------------------
	iSize += 1;								// +1 をしてNULLを追加
	Text  = (char *)malloc(iSize);
	if(Text == NULL){ CloseHandle(Handle); return sys_error("%s を読み込み中メモリーの確保に失敗しました。", strFile);}
	memset(Text, 0, iSize);					// ゼロクリアー

	//--- 読み込む --------------------------------------------
	ReadFile(Handle, Text, iSize, &rs, NULL);

	//--- ファイルを閉じる ------------------------------------
	CloseHandle(Handle);

	//--- ファイル名を絶対パスで得る ------------------------------
	sprintf(GosubWork[0].strName, "%s", strFile);
	return true;
}



/*------------------------------------------------------------------------------*
| <<< 変数名より変数管理番号を得る >>>
|	入力	*strName = 変数名（先頭の $ は不要です）
|	戻り値			 =  -1 : その変数は登録されていない。
|				    !=  -1 : その変数の管理番号
*------------------------------------------------------------------------------*/
int		TextVarNumGet(char *strName)
{
	int	iNameLen = strlen(strName);
	int	i;
	for(i = 0;;i++)							// 変数名(name)が登録されているか調べる
	{										// 登録名が同じか？
		if(strncmp(Var[i].strName, strName, iNameLen) == 0) break;
											// 登録されていなければ -1 を返す
		if(strlen( Var[i].strName) == 0)					return -1;
	}
	return i;								// 戻り値は変数の管理番号
}



/*------------------------------------------------------------------------------*
| <<< 変数名を変数に割り当てる >>>
|	入力	*strName = 変数名（先頭の $ は不要です）
|	戻り値			 =  -1 : 変数は登録できなかった。または同名の変数が存在する
|				    !=  -1 : その変数の管理番号
*------------------------------------------------------------------------------*/
int		TextVarNameSet(char *strName)
{
	//--- 同名の変数があるか調べる --------------------------------
	int	iNameLen = strlen(strName);
	int	i;
	for(i = 0; i < VAR_MAX; i++)
	{
		if(strncmp(Var[i].strName, strName, iNameLen) == 0) break;
	}

	//--- 同名の変数があるか調べる --------------------------------
	for(i = 0; i < VAR_MAX; i++)
	{
		if(Var[i].strName[0] == NULL)
		{
			strcpy(Var[i].strName, strName);
			return i;
		}
	}
	//--- 変数は全て登録済み --------------------------------------
	return -1;
}



/*------------------------------------------------------------------------------*
| <<< 管理番号より変数の値を得る >>>
|	入力	iID   = 変数管理番号（TextVarNumGetで得た値）
|	出力	*fOut = 変数の値
|	戻り値		  = true  : 正常に値を返した。
|				  = false : その変数は未登録。
*------------------------------------------------------------------------------*/
bool	TextVarGet(int iID, float *fOut)
{
	if(iID == -1)					return false;	// -1 という変数IDはない
	if(Var[iID].strName[0] == NULL) return false;	// 変数名チェック
													// 一文字目がNULLなら未登録
	*fOut = Var[iID].fDat;							// fOut に値をセット
	return true;
}



/*------------------------------------------------------------------------------*
| <<< 管理番号より変数の値をセット >>>
|	入力	iID  = 変数管理番号（TextVarNumGetで得た値）
|	出力	fIn  = 変数の値
|	戻り値		 = true  : 正常に値をセットした。
|				 = false : その変数は未登録。
*------------------------------------------------------------------------------*/
bool	TextVarSet(int iID, float fIn)
{
	if(iID == -1)					return false;	// -1 という変数IDはない
	if(Var[iID].strName[0] == NULL) return false;	// 変数名チェック
													// 一文字目がNULLなら未登録
	Var[iID].fDat = fIn;							// fOut に値をセット
	return true;
}



/*------------------------------------------------------------------------------*
| <<< テキスト位置を開始位置に設定する >>>
*------------------------------------------------------------------------------*/
void	TextCountClear(void)
{
	iCount = 0;
}
