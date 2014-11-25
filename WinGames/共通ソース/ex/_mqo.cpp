//
//	メタセコデータ読み込み
//


//--- 定数 -----------------------------------------------------------------
#undef	SCALE
#define	SCALE		0.01f					// デフォルトのスケール

#define	INDEX_MAX	65535					// 最大頂点数

#define	FACE_MAX	65535					// 表面数
#define	TEX_MAX		255						// テクスチャー最大数
#define	VERTEX_MAX	65535					// 頂点数


//--- 構造体 ----------------------------------------------------------------
struct	META_TEX
{
	float		afCol[4];					// マテリアル基本色
	float		fDif;						// 拡散光
	float		fAmb;						// 周囲光
	float		fEmi;						// 自己照明
	float		fSpc;						// 反射光
	float		fPower;						// 反射光の強さ

	char		strTex[ MAX_PATH];			// テクスチャー名
	char		strAlp[ MAX_PATH];			// 透明マップ名
	char		strBump[MAX_PATH];			// バンプマップ名
	MATERIAL	mat;						// マテリアル
};

struct	META_WORK
{
	int				iObjNum;					// 解析中のオブジェクト番号
	int				iMatCount;					// マテリアル数
	D3DXMATERIAL	amat[    TEX_MAX];
	VEC3			avecPos[ VERTEX_MAX];		// 頂点
	int				iPosCount;					// 頂点の登録数
	int				iPosBase;					// 頂点のベースカウント(オブジェクト毎にベース数は変化する)

	//--- 頂点バッファ転送用 ----------
	D3_VTX_DEFAULT	avtx[    VERTEX_MAX];		// 頂点
	uint			uiVtxCount;					// 頂点の登録数
	uint			auiIndex[INDEX_MAX];		// インデックスバッファ
	uchar			aucMat[  INDEX_MAX / 3];	// マテリアルバッファ
	uint			uiIndexCount;				// インデックス数
	//---------------------------------

	META_TEX		atex[    TEX_MAX];			// テクスチャー情報
	META_TEX		*pTex;						// 収集中のテクスチャー情報
	int				iMatID;						// 収集中のマテリアル番号
	int				iPolyCount;					// 面数
	LPD3DXMESH		lpMesh;						// メッシュの格納場所
	int				iMirrorType;				// 鏡面タイプ(0 > オン)
	float			fSmooth;					// スムージング値(この値を見て頂点法線の値をミックスするか決める)

	//--- meta_buf_load 用
	D3_VTX_DEFAULT	*pvtxBuf;					// 未使用時は NULL です

	//--- デバッグ用
	#ifdef	_DEBUG
		char		strName[256];				// 解析中のオブジェクト
	#endif
};


//--- 変数 -----------------------------------------------------------------
static	char		*MetaText;					// テキストデータ読み込み位置
static	int			iMetaCount;					// 読み込み中の文字位置
static	META_WORK	*meta;
static	BOOL		bVisibleSkip = true;	// 可視状態を有効にする
		void		*meta_visible_func;		// 可視状態情報を得る時に使用
		void		*meta_obj_name_func;	// オブジェ名を得る時に使用

//--- コマンドテーブル -----------------------------------------------------
static	char	MetaCommandTbl[][16] = 
{
	" ",					// 00 :	半角スペース
	"\t",					// 01 :	タブ
	"\r\n",					// 02 :	本来の改行コード
	"Scene {",				// 03 :	シーン（これは読み飛ばす）
	"vertex ",				// 04 :	Objectチャンク、頂点情報読み込み
	"face ",				// 05 :	face  チャンク読み込み
	"tex(",					// 06 :	テクスチャー名登録
	"bump(",				// 07 :	バンプマップ名登録
	"Material",				// 08 :	マテリアル数登録
	"col(",					// 09 :	カラー登録
	"dif(",					// 10 :	拡散光登録
	"amb(",					// 11 :	周囲光登録
	"emi(",					// 12 :	自己照明登録
	"spc(",					// 13 :	反射光登録
	"power(",				// 14 :	反射の強さ登録
	"Object ",				// 15 :	オブジェクト
	"mirror_axis",			// 16 :	ミラータイプ
	"mirror ",				// 17 :	ミラー
	"facet ",				// 18 : スムージング
	"",						// コマンドチェックの終端になる
};


//--- 関数宣下 ----------------------------------------------------
static	BOOL	CommandCheck(BOOL bUV);
static	void	CreateMeshFromMetaseq(void);



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
	
	if(sys.bError){ return 0;}				// エラーが出ている場合は処理しない

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



/*-------------------------------------------------------------------------------*
| <<< テキスト読み込み >>>
|	入力	*strFile = 読み込みファイル名
*-------------------------------------------------------------------------------*/
static	BOOL	TextLoad_(char *strFile)
{

	HANDLE	Handle;						// 読み込みハンドル
	DWORD	rs;							// ダミー
	int		iSize;						// 読み込みサイズ

	//--- メモリー開放 ----------------------------------------------
	if(MetaText	!= NULL){ free(MetaText); MetaText = NULL;}

	//--- ファイルを開く --------------------------------------------
	Handle = CreateFile(strFile, GENERIC_READ, 0, NULL,	OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL |	FILE_FLAG_RANDOM_ACCESS, NULL);
	if(Handle == INVALID_HANDLE_VALUE)
	{
		return sys_error("%s が開けません。", strFile);
	}

	//--- サイズを取得する ------------------------------------------
	iSize =	GetFileSize(Handle,	NULL);
	if(iSize ==	0){	CloseHandle(Handle); return	sys_error("%s サイズが０バイトです。", strFile);}

	//--- メモリーを確保する ----------------------------------------
	iSize   += 1;									// +1 して NULL	を追加
	MetaText = (char *)malloc(iSize);
	if(MetaText	== NULL){ CloseHandle(Handle); return sys_error("%s を読み込み中のメモリーの確保に失敗しました。", strFile);}
	memset(MetaText, 0,	iSize);						// ゼロクリアー

	//--- 読み込む --------------------------------------------------
	ReadFile(Handle, MetaText, iSize, &rs, NULL);

	//--- ファイルを閉じる ------------------------------------------
	CloseHandle(Handle);

	//--- ワーク初期化 ----------------------------------------------
	meta->uiVtxCount = 0;
	return true;
}



/*-----------------------------------------------------------------------------------------*
| <<< コマンド : Scene { >>>				※ シーンは全て読み飛ばされる
*-----------------------------------------------------------------------------------------*/
static	void	ComScene(void)
{
	while(MetaText[iMetaCount] != '}')
	{
		if(ISKANJI(MetaText[iMetaCount])){ iMetaCount++;}
		iMetaCount++;
	}
	iMetaCount++;
}



/*-----------------------------------------------------------------------------------------*
| <<< コマンド : vertex	>>>
*-----------------------------------------------------------------------------------------*/
static	void	ComVertex(void)
{
	float	fAns;							// 頂点数
	iMetaCount += PARAM_GET(&fAns, &MetaText[iMetaCount]);
	iMetaCount += 3;						// "{" と改行分を移動させる

	// ベース設定
	meta->iPosBase = meta->iPosCount;

	//--- 頂点データを保存 ------------------------------------------
	VEC3	*p = &meta->avecPos[meta->iPosCount];
	for(int	i =	0; i < (int)fAns; i++, p++)
	{
		iMetaCount += PARAM_GET(&p->x, &MetaText[iMetaCount]);
		iMetaCount += PARAM_GET(&p->y, &MetaText[iMetaCount]);
		iMetaCount += PARAM_GET(&p->z, &MetaText[iMetaCount]);
		p->z	   *= -1;
		*p		   *= SCALE;				// スケールを合わせる
		iMetaCount += 2;					// 改行を進める

		meta->iPosCount++;					// 登録数増やす
	}
}



/*-----------------------------------------------------------------------------------------*
| <<< 頂点登録 >>>
|	入力	pvecPos    = 頂点
|			pvecNormal = 頂点法線
|			pvecUV     = UV
|	戻り値	登録バッファ番号
*-----------------------------------------------------------------------------------------*/
static	int		EntryVertexBuf(VEC3 *pvecPos, VEC3 *pvecNormal, VEC2 *pvecUV)
{

	// メタセコイヤの頂点データは UV が含まれていないが、DirectX のメッシュデータは UV を必要とするので、
	// 頂点 + UV として頂点データを登録する。この時、同一情報がないかチェックする

	//--- 法線処理 --------------------------------------------------
	// 同一頂点を探して、法線を加算
	VEC3	nor = *pvecNormal;
	uint	i;
	for(i =	0; i < meta->uiVtxCount; i++)
	{
		if(meta->avtx[i].p == *pvecPos)
		{
			// UV がある時
			if(pvecUV != NULL)
			{
				// UV と 法線が同一のものがある
				if(meta->avtx[i].n == nor && meta->avtx[i].uv == *pvecUV){ break;}
			}
			else
			{
				// 法線が同一のものがある
				if(meta->avtx[i].n == nor){ break;}
			}
			// ミックスする法線同士の角とスムージング値を比較
			if(meta->fSmooth > calc_vec3_vec_to_vec_angle(&meta->avtx[i].n, &nor))
			{
				// 法線同士の角がスムージング値より小さい場合は法線をミックスする
				meta->avtx[i].n += nor;
				calc_vec3_normalize(&meta->avtx[i].n, &meta->avtx[i].n);
				nor				 = meta->avtx[i].n;
			}
		}
	}

	//--- バッファに同一情報がないか調べる --------------------------
	for(i =	0; i < meta->uiVtxCount; i++)
	{
		// UV がある時
		if(pvecUV != NULL)
		{
			if(meta->avtx[i].p == *pvecPos && meta->avtx[i].uv == *pvecUV)
			{
				meta->uiVtxCount++;
				return i;
			}
		}
		// UV が無い時
		else
		{
			if(meta->avtx[i].p == *pvecPos)
			{
				meta->uiVtxCount++;
				return i;
			}
		}
	}

	//--- 新規登録 --------------------------------------------------
	if(i ==	meta->uiVtxCount && i < VERTEX_MAX)
	{
		meta->avtx[i].p  = *pvecPos;		// 頂点
											// UV
		if(pvecUV != NULL){	meta->avtx[i].uv = *pvecUV;}
		meta->avtx[i].n  = nor;				// 法線(新規登録、二回目以降は加算)
		
		calc_vec3_normalize(&meta->avtx[i].n, &meta->avtx[i].n);

		meta->uiVtxCount++;
		return i;
	}



#if 0

	// メタセコイヤの頂点データは UV が含まれていないが、DirectX のメッシュデータは UV を必要とするので、
	// 頂点 + UV として頂点データを登録する。この時、同一情報がないかチェックする

	//--- 法線処理 --------------------------------------------------
	// 同一頂点を探して、法線を加算
	VEC3	nor    = *pvecNormal;
	uint	i;

	for(i =	0; i < meta->uiVtxCount; i++)
	{
		if(meta->avtx[i].p == *pvecPos)
		{
			// UV がある時
			if(pvecUV != NULL)
			{
				// UV と 法線が同一のものがある
				if(meta->avtx[i].n == nor && meta->avtx[i].uv == *pvecUV)
				{
					return i;
				}
			}
			else
			{
				// 法線が同一のものがある
				if(meta->avtx[i].n == nor)
				{
					return i;
				}
			}
		}
	}
	
	//--- バッファに同一情報がないか調べる --------------------------
	/*for(i =	0; i < meta->uiVtxCount; i++)
	{
		// UV がある時
		if(pvecUV != NULL)
		{
			if(meta->avtx[i].p == *pvecPos && meta->avtx[i].uv == *pvecUV)
			{
				return i;
			}
		}
		// UV が無い時
		else
		{
			if(meta->avtx[i].p == *pvecPos)
			{
				return i;
			}
		}
	}*/

	//--- 新規登録 --------------------------------------------------
	if(i ==	meta->uiVtxCount && i < VERTEX_MAX)
	{
		meta->avtx[i].p  = *pvecPos;		// 頂点
											// UV
		if(pvecUV != NULL){	meta->avtx[i].uv = *pvecUV;}
		meta->avtx[i].n  = nor;				// 法線(新規登録、二回目以降は加算)
		
		calc_vec3_normalize(&meta->avtx[i].n, &meta->avtx[i].n);
		meta->uiVtxCount++;
		return i;
	}
#endif


	//--- 登録不可 --------------------------------------------------
	return -1;
}



/*-----------------------------------------------------------------------------------------*
| <<< 面登録サブ >>>
|	入力	pfIndex  = インデックスバッファ
|			pfUV	 = UV バッファ
|			uiMat    = マテリアル色
|			bUV	　	 = TRUE : UV 使用
|			bSquare	 = ４頂点ポリゴン
|			uiMirror = 0 : 通常登録
|					   1 : X ミラー( X 座標を反転して登録)
|					   2 : Y ミラー( Y 座標を反転して登録)
|					   3 : Z ミラー( Z 座標を反転して登録)
*-----------------------------------------------------------------------------------------*/
static	void	FaceSub(float *pfIndex, float *pfUV, uint uiMat, BOOL bUV, BOOL bSquare, uint uiMirror)
{

	//--- 頂点を得る ------------------------------------------------
	int		j  = meta->uiIndexCount;
	VEC3	v0 = meta->avecPos[(int)pfIndex[0]];
	VEC3	v1 = meta->avecPos[(int)pfIndex[1]];
	VEC3	v2 = meta->avecPos[(int)pfIndex[2]];
	VEC3	v3 = meta->avecPos[(int)pfIndex[3]];

	//--- ミラー ----------------------------------------------------
	BOOL	bMirror = false;
	switch(uiMirror)
	{
	// X ミラー
	case 1:
		v0.x   *= -1;
		v1.x   *= -1;
		v2.x   *= -1;
		v3.x   *= -1;
		bMirror = TRUE;
		break; 
	// Y ミラー
	case 2:
		v0.y   *= -1;
		v1.y   *= -1;
		v2.y   *= -1;
		v3.y   *= -1;
		bMirror = TRUE;
		break; 
	// Z ミラー
	case 3:
		v0.z   *= -1;
		v1.z   *= -1;
		v2.z   *= -1;
		v3.z   *= -1;
		bMirror = TRUE;
		break; 
	}

	//--- 頂点法線計算 ----------------------------------------------
	VEC3	vecNormal;
	if(bMirror){ calc_vec3_cross(&vecNormal, &(v2 - v0), &(v1 - v2));}
	else	   { calc_vec3_cross(&vecNormal, &(v1 - v0), &(v2 - v1));}
	calc_vec3_normalize(&vecNormal, &vecNormal);

	//--- 頂点登録 --------------------------------------------------
	if(bUV)
	{
		meta->auiIndex[j + 0		  ] = EntryVertexBuf(&v0, &vecNormal, &VEC2(pfUV[0], pfUV[1]));
		meta->auiIndex[j + 1 + bMirror] = EntryVertexBuf(&v1, &vecNormal, &VEC2(pfUV[2], pfUV[3]));
		meta->auiIndex[j + 2 - bMirror] = EntryVertexBuf(&v2, &vecNormal, &VEC2(pfUV[4], pfUV[5]));
	}
	else
	{
		meta->auiIndex[j + 0		  ] = EntryVertexBuf(&v0, &vecNormal, NULL);
		meta->auiIndex[j + 1 + bMirror] = EntryVertexBuf(&v1, &vecNormal, NULL);
		meta->auiIndex[j + 2 - bMirror] = EntryVertexBuf(&v2, &vecNormal, NULL);
	}
	meta->aucMat[j / 3] = uiMat;
	meta->uiIndexCount += 3;

	//--- 四角形の場合 ----------------------------------------------
	if(bSquare)
	{
		// 頂点法線計算
		if(bMirror){ calc_vec3_cross(&vecNormal, &(v3 - v0), &(v1 - v3));}
		else	   { calc_vec3_cross(&vecNormal, &(v1 - v0), &(v3 - v1));}

		// 頂点登録
		if(bUV)
		{
			meta->auiIndex[j + 3		  ] = EntryVertexBuf(&v0, &vecNormal, &VEC2(pfUV[0], pfUV[1]));
			meta->auiIndex[j + 4 + bMirror] = EntryVertexBuf(&v2, &vecNormal, &VEC2(pfUV[4], pfUV[5]));
			meta->auiIndex[j + 5 - bMirror] = EntryVertexBuf(&v3, &vecNormal, &VEC2(pfUV[6], pfUV[7]));
		}
		else
		{
			meta->auiIndex[j + 3		  ] = EntryVertexBuf(&v0, &vecNormal, NULL);
			meta->auiIndex[j + 4 + bMirror] = EntryVertexBuf(&v2, &vecNormal, NULL);
			meta->auiIndex[j + 5 - bMirror] = EntryVertexBuf(&v3, &vecNormal, NULL);
		}
		meta->aucMat[j / 3 + 1] = uiMat;
		meta->uiIndexCount	   += 3;
	}
}



/*-----------------------------------------------------------------------------------------*
| <<< コマンド : face >>>
|	入力	bUV = false : uv のチェックを行わない(法線マップを作成する際に使用)
*-----------------------------------------------------------------------------------------*/
static	void	ComFace(BOOL bUV)
{
// faceチャンクから表面数を読み取り、その分だけ面を作成する
// V(0 1 3 2) ← MvtSub[0],	MvtSub[1], MvtSub[3], MvtSub[2]	の頂点情報からなる面である
// meta[メタセコファイル番号].face[登録表面数]

	float		fFaceCount;					// 面数
	iMetaCount += PARAM_GET(&fFaceCount, &MetaText[iMetaCount]);

	iMetaCount += 3;						// "{" と改行を移動させる

	float	a, afIndex[4], afUV[8];

	afIndex[3] = 0;
	//--- 表面データを保存 ------------------------------------------
	for(int	i =	0; i < (int)fFaceCount; i++)
	{
		//--- 頂点数 ------------------------------------------------
		float	fVertexCount;
		iMetaCount += PARAM_GET(&fVertexCount, &MetaText[iMetaCount]);

		//--- V() ---------------------------------------------------
		iMetaCount +=2;						// V( を進める
											// 総面数を計算、四角形は２面となる
		int	iPoly;
		if(fVertexCount	== 4){ iPoly = 2;}
		else				 { iPoly = 1;}
		
		// 頂点番号を得る
		for(int j =	0; j < (int)fVertexCount; j++)
		{
			iMetaCount += PARAM_GET(&afIndex[j], &MetaText[iMetaCount]);
			afIndex[j] += meta->iPosBase;
		}
		iMetaCount++;						// カッコ閉じる

		//--- M() ---------------------------------------------------
		SPACE_DEL(MetaText,	&iMetaCount);
		uint	uiMat = 0;
		if(MetaText[iMetaCount]	== 'M')
		{
			iMetaCount += 2;				// 材質情報
			iMetaCount += PARAM_GET(&a, &MetaText[iMetaCount]);
			uiMat	    = (int)a;
			iMetaCount++;					// カッコ閉じる
		}

		//--- UV() --------------------------------------------------
		BOOL	bUV = FALSE;
		SPACE_DEL(MetaText,	&iMetaCount);
		if(MetaText[iMetaCount]	== 'U' && MetaText[iMetaCount +	1] == 'V')
		{
			iMetaCount += 3;
			for(int j =	0; j < (int)fVertexCount * 2; j++)
			{
				iMetaCount += PARAM_GET(&afUV[j], &MetaText[iMetaCount]);
				//while(afUV[j] > 1){ afUV[j] -= 1;}	// UV は 0 ～ 1 に丸める
			}
			iMetaCount += 3;
			bUV         = TRUE;
		}
		else
		{
			iMetaCount += 2;
		}

		//--- バッファ登録 ------------------------------------------
		FaceSub(afIndex, afUV, uiMat, bUV, iPoly == 2, 0);
	
		if(meta->iMirrorType)
		{
			FaceSub(afIndex, afUV, uiMat, bUV, iPoly == 2, meta->iMirrorType);
		}

		//--- ポリ数を加算 ------------------------------------------
		if(meta->iMirrorType){ meta->iPolyCount += iPoly * 2;}
		else			     { meta->iPolyCount += iPoly;}
	}
	iMetaCount		 +=	3;
	meta->iMirrorType =	0;
}



/*-----------------------------------------------------------------------------------------*
| <<< コマンド : tex、bump >>>
|	入力	iType =	0 :	テクスゃー、1 :	パンプ
*-----------------------------------------------------------------------------------------*/
static	void	ComTexBump(int iType)
{
	//--- Material { 内かチェック ---------------------------------
	if(meta->pTex == NULL)
	{
		sys.bError = true;
		sys_error("meta.pTex が NULL です。col の設定が Material { の外で行われています。"); return;
	}
	//--- ファイル名を得る ----------------------------------------
	char	strFile[MAX_PATH + 1];
	memset(strFile,	0, sizeof(strFile));	// ごみが入らないようにクリアー
	iMetaCount += GET_STRING(strFile, &MetaText[iMetaCount]);

	//--- ファイル名を登録 ----------------------------------------------------
	switch(iType)
	{
	case 0:	strcpy(meta->atex[meta->iMatID].strTex,	 strFile); break;
	case 1:	strcpy(meta->atex[meta->iMatID].strBump, strFile); break;
	}
	iMetaCount++;
}



/*-----------------------------------------------------------------------------------------*
| <<< コマンド : Material >>>
|	入力	bUV = false : uv のチェックを行わない(法線マップを作成する際に使用)
*-----------------------------------------------------------------------------------------*/
static	void	ComMaterial(BOOL bUV)
{
	float	a;

	iMetaCount	   += PARAM_GET(&a,	&MetaText[iMetaCount]);
	meta->iMatCount	= (int)a;				// マテリアル総数
	iMetaCount	   += 1;					// "{" 分移動
											// テクスチャーデータの書き込み位置を設定
	meta->pTex		= &meta->atex[meta->iMatID];
	//--- 各パラメータ抽出 ------------------------------------------
	while(MetaText[iMetaCount] != '}')
	{										// 改行を見つけたら、次へすすむ
		char	str[512];

		if(MetaText[iMetaCount]	== '\r'	&& MetaText[iMetaCount + 1]	== '\n')
		{
			meta->iMatID++;					// マテリアル読み込み終了なので、次へ進める
											// テクスチャーデータの書き込み位置を設定
			meta->pTex  = &meta->atex[meta->iMatID];

			iMetaCount += 2;
			//--- マテリアル名を得る --------------------------------
			int		i  = GET_STRING(str, &MetaText[iMetaCount]);

			if(meta_obj_name_func != NULL)	// オブジェ名を外部へ渡す
			{
				void(*func)(int iNum, char *str);
				func = (void(*)(int iNum, char *str))meta_obj_name_func;
				(*func)(meta->iMatID, str);
			}
			iMetaCount += i;
			//-------------------------------------------------------
		}

		//--- コマンドチェック --------------------------------------
		BOOL	bRet = CommandCheck(bUV);

		//--- 半角・全角チェック ------------------------------------
		if(!bRet)
		{
			//char	str[512];				// ﾀﾞﾌﾞﾙｸｫｰﾃｰｼｮﾝ中に コマンドが入っているのを防ぐため、こうする。
			int		i   = GET_STRING(str, &MetaText[iMetaCount]);
			iMetaCount += i;
			if(i ==	0)
			{
				if(ISKANJI(MetaText[iMetaCount])){ iMetaCount += 2;}	// 全角
				else							 { iMetaCount += 1;}	// 半角
			}
		}
	}
	iMetaCount++;
	meta->pTex = NULL;						// テクスチャーデータの書き込み位置をクリアー
}



/*-----------------------------------------------------------------------------------------*
| <<< コマンド : col >>>
*-----------------------------------------------------------------------------------------*/
static	void	ComCol(void)
{
	if(meta->pTex == NULL)
	{
		sys.bError = true;
		sys_error("meta.pTex が NULL です。col の設定が Material { の外で行われています。"); return;
	}
	else
	{
		iMetaCount += PARAM_GET(&meta->pTex->afCol[1], &MetaText[iMetaCount]);	// r
		iMetaCount += PARAM_GET(&meta->pTex->afCol[2], &MetaText[iMetaCount]);	// g
		iMetaCount += PARAM_GET(&meta->pTex->afCol[3], &MetaText[iMetaCount]);	// b
		iMetaCount += PARAM_GET(&meta->pTex->afCol[0], &MetaText[iMetaCount]);	// a
	}
	iMetaCount++;
}



/*-----------------------------------------------------------------------------------------*
| <<< コマンド : dif, amb, emi,	spc, power >>>
|	入力	iType =	0 :	dif、1 : amb、2	: emi、3 : spc、4 :	power
*-----------------------------------------------------------------------------------------*/
static	void	ComDifAmbEmiSpcPower(int iType)
{
	if(meta->pTex == NULL)
	{
		sys.bError = true;
		sys_error("meta.pTex が NULL です。色の設定が Material { の外で行われています。");
		return;
	}
	else
	{										// ポインタを使い無理やり値を代入
		float	*f  = &meta->pTex->fDif;
		iMetaCount += PARAM_GET(&f[iType], &MetaText[iMetaCount]);
		iMetaCount++;						// } を進める
	}
}



/*-----------------------------------------------------------------------------------------*
| <<< コマンド : Object	 >>>				※可視、不可視をチェックし、不可視なら読み飛ばす
*-----------------------------------------------------------------------------------------*/
static	void	ComObject(void)
{

	char	str[512];							// ﾀﾞﾌﾞﾙｸｫｰﾃｰｼｮﾝ中に コマンドが入っているのを防ぐため、こうする。
	//--- 最初のカッコまで文字を進める ------------------------------
	//Text[iCount] = Text[iCount];
	while(MetaText[iMetaCount] != '{')
	{
		int		i   = GET_STRING(str, &MetaText[iMetaCount]);
		iMetaCount += i;
		if(i ==	0)
		{
			if(ISKANJI(MetaText[iMetaCount])){ iMetaCount += 2;}	// 全角
			else					         { iMetaCount += 1;}	// 半角
		}
		/*
		if(meta_obj_name_func != NULL)			// オブジェ名を外部へ渡す
		{
			void(*func)(int iNum, char *str);
			func = (void(*)(int iNum, char *str))meta_obj_name_func;
			(*func)(meta->iObjNum, str);
		}*/
	}
	iMetaCount++;

	//--- 最後のカッコまで文字を進める ------------------------------
	#ifdef	_DEBUG
		strcpy(meta->strName, str);
	#endif

	char	*a = strstr(&MetaText[iMetaCount], "visible");	// visible まで文字を進める
	iMetaCount += (a - &MetaText[iMetaCount]) + 8;
	//iCount += 11;

	if(meta_visible_func != NULL)							// 可視情報を外部へ渡す(現在、未動作)
	{
		void(*func)(int iNum, BOOL b);
		func = (void(*)(int iNum, BOOL b))meta_visible_func;
		//(*func)(meta->iObjNum, Text[iCount] != '0');
		(*func)(meta->iObjNum, true);						// 現在は必ず可視として渡す
	}

	if(!bVisibleSkip || MetaText[iMetaCount] != '0'){ return;}// visible	をチェックする

	iMetaCount++;
	for(;;)
	{
		char	str[512];							// ﾀﾞﾌﾞﾙｸｫｰﾃｰｼｮﾝ中に コマンドが入っているのを防ぐため、こうする。
		int		i   = GET_STRING(str, &MetaText[iMetaCount]);
		iMetaCount += i;
		if(i ==	0)
		{											// 全角
			if(ISKANJI(MetaText[iMetaCount]))
			{
				iMetaCount += 2;
			}
			else
			{
				if(MetaText[iMetaCount]	== '\r'	&& MetaText[iMetaCount + 1]	== '\n'	&& MetaText[iMetaCount + 2]	== '}'){ break;}
				iMetaCount += 1;					// 半角
			}
		}
	}
	iMetaCount += 3;
}



/*-----------------------------------------------------------------------------------------*
| <<< コマンド : mirror_axis >>>
*-----------------------------------------------------------------------------------------*/
static	void	ComMirrorAxis(void)
{
	float	fAns;
	iMetaCount		 +=	PARAM_GET(&fAns, &MetaText[iMetaCount]);
	meta->iMirrorType =	(int)fAns;
}



/*-----------------------------------------------------------------------------------------*
| <<< コマンド : mirror	>>>
*-----------------------------------------------------------------------------------------*/
static	void	ComMirror(void)
{
	float	fAns;
	iMetaCount		 += PARAM_GET(&fAns, &MetaText[iMetaCount]);
	meta->iMirrorType = (int)fAns;
}



/*-----------------------------------------------------------------------------------------*
| <<< コマンド : facet	>>>					※ スムージングの値
*-----------------------------------------------------------------------------------------*/
static	void	ComFacet(void)
{
	iMetaCount   += PARAM_GET(&meta->fSmooth, &MetaText[iMetaCount]);
	meta->fSmooth = R(meta->fSmooth);
}



/*-----------------------------------------------------------------------------------------*
| <<< コマンドチェック >>>
|	入力	bUV = false : uv のチェックを行わない(法線マップを作成する際に使用)
|	戻り値	true : 一度でもコマンドを実行した
*-----------------------------------------------------------------------------------------*/
static	BOOL	CommandCheck(BOOL bUV)
{
	BOOL	bRet = false;

	for(int	i =	0;;i++)
	{
		if(sys.bError) return bRet;
		int	iLen = strlen(MetaCommandTbl[i]);	// ※strlen	= 文字列の長さを調べる
		if(iLen	== 0) break;					// 文字列の長さが０なら終端とみなす
		if(strncmp(&MetaText[iMetaCount], MetaCommandTbl[i], iLen) == 0)
		{
			iMetaCount += iLen;					// コマンドの文字数を進める
			bRet	    = true;
			//sprintf(strErrorCommand, "%s にエラーがあります。", CommandTbl[i]);

			switch(i)
			{
			case 0:				// 半角スペース
			case 1:				// タブ
			case 2:				// 本来の改行コード
				break;
			case 3:				// シーン（読み飛ばすこと）
				ComScene();					  break;
			case 4:				// Objectチャンク、頂点情報読み込み
				ComVertex();				  break;
			case 5:				// faceチャンク読み込み
				ComFace(bUV);				  break;
			case 6:				// バンプマップ読み込み
			case 7:				// テクスチャー読み込み
				ComTexBump(i - 6);			  break;
			case 8:				// マテリアル登録
				ComMaterial(bUV);			  break;
			case 9:				// カラー登録
				ComCol();					  break;
			case 10:			// 拡散光登録
			case 11:			// 周囲光登録
			case 12:			// 自己照明登録
			case 13:			// 反射光登録
			case 14:			// 反射の強さ登録
				ComDifAmbEmiSpcPower(i - 10); break;
			case 15:			// Objectチャンク
				meta->iObjNum++;
				ComObject();				  break;
			case 16:			// 鏡面タイプ
				ComMirrorAxis();			  break;
			case 17:			// 鏡面スイッチ
				ComMirror();				  break;
			case 18:			// スムージング
				ComFacet();					  break;
			}
			//i	= -1;
			return true;
		}
	}
	return bRet;
}



/*-----------------------------------------------------------------------------------------*
| <<< テキストメイン >>>
|	入力	bUV = false : uv のチェックを行わない(法線マップを作成する際に使用)
*-----------------------------------------------------------------------------------------*/
static	void	TextMain_(BOOL bUV)
{

	while(MetaText[iMetaCount] != NULL)					// 文字列の終端ならば処理しない
	{
		//--- コマンドチェック --------------------------------------
		BOOL	bRet = CommandCheck(bUV);

		//--- 半角・全角チェック ------------------------------------
		if(!bRet)
		{
			char	str[4096];							// ﾀﾞﾌﾞﾙｸｫｰﾃｰｼｮﾝ中に コマンドが入っているのを防ぐため、こうする。
			int		i   = GET_STRING(str, &MetaText[iMetaCount]);
			iMetaCount += i;
			if(i ==	0)
			{
				if(ISKANJI(MetaText[iMetaCount])){ iMetaCount += 2;}	// 全角
				else							 { iMetaCount += 1;}	// 半角
			}
		}
	}
}



/*-----------------------------------------------------------------------------------------*
| <<< メッシュ作成 >>>
*-----------------------------------------------------------------------------------------*/
static	void	CreateMeshFromMetaseq(void)
{

	if(meta->iPolyCount	== 0){ return;}

	// Ｘファイルから読み込む場合は
	// 立方体の場合　インデックスは	40 個
	//				 頂点情報　　　	24 個	となっていた。
	D3DXCreateMeshFVF(meta->iPolyCount,		// 表面数	数は適当
					  meta->uiVtxCount,		// 頂点数	数は適当
					  D3DXMESH_MANAGED,		// メッシュのオプション
					  D3_FVF_DEFAULT,		// 頂点フォーマット
					  d3.dev,				// デバイス
					  &meta->lpMesh);		// 作成されたメッシュ

	//--- 頂点情報を書き込む ----------------------------------------
	D3_VTX_DEFAULT	*pvtxBuf;				// 頂点バッファ
	WORD			*pwIndex;				// インデックスバッファ
	DWORD			*pdwMaterial;			// マテリアルバッファ
	meta->lpMesh->LockVertexBuffer(   D3DLOCK_DISCARD, (void **)&pvtxBuf);
	meta->lpMesh->LockIndexBuffer(    D3DLOCK_DISCARD, (void **)&pwIndex);
	meta->lpMesh->LockAttributeBuffer(D3DLOCK_DISCARD, &pdwMaterial);

	// 頂点バッファ転送
	for(uint i = 0; i < meta->uiVtxCount; i++)
	{
		pvtxBuf[i].p  = meta->avtx[i].p;
		pvtxBuf[i].n  = meta->avtx[i].n;
		pvtxBuf[i].uv = meta->avtx[i].uv;
	}

	// インデックスバッファ転送
	for(int i = 0; i < meta->iPolyCount * 3; i++)
	{
		pwIndex[i] = meta->auiIndex[i];
	}

	// マテリアルバッファ転送
	for(int i = 0; i < meta->iPolyCount; i++)
	{
		pdwMaterial[i] = meta->aucMat[i];
	}

	//--- 頂点バッファ閉じる ----------------------------------------
	meta->lpMesh->UnlockAttributeBuffer();
	meta->lpMesh->UnlockIndexBuffer();
	meta->lpMesh->UnlockVertexBuffer();
}



/*-----------------------------------------------------------------------------------------*
| <<< テクスチャー作成 >>>
|	入力	iNum	= 作成するメッシュ番号
|			bBump	= true : バンプを作成する
*-----------------------------------------------------------------------------------------*/
static	void	MetaTexCreate(int iNum, BOOL bBump)
{

	if(meta->iPolyCount	== 0){ return;}

	//--- メタセコイヤはマテリアル無しの場合があるので、その対策 ----
	if(meta->iMatCount == 0)
	{
		meta->iMatCount++;
		meta->amat[0].MatD3D.Diffuse.a  =
		meta->amat[0].MatD3D.Diffuse.r  =
		meta->amat[0].MatD3D.Diffuse.g  =
		meta->amat[0].MatD3D.Diffuse.b  = 1;
		meta->amat[0].MatD3D.Ambient.a  =
		meta->amat[0].MatD3D.Ambient.r  =
		meta->amat[0].MatD3D.Ambient.g  =
		meta->amat[0].MatD3D.Ambient.b  = 0;
		meta->amat[0].MatD3D.Emissive.a =
		meta->amat[0].MatD3D.Emissive.r =
		meta->amat[0].MatD3D.Emissive.g =
		meta->amat[0].MatD3D.Emissive.b = 0;
		meta->amat[0].MatD3D.Specular.a =
		meta->amat[0].MatD3D.Specular.r =
		meta->amat[0].MatD3D.Specular.g =
		meta->amat[0].MatD3D.Specular.b = 0;
		meta->amat[0].MatD3D.Power	    = 0;
		meta->amat[0].pTextureFilename  = NULL;
	}
	else
	{
		for(int	i =	0; i < meta->iMatCount;	i++)
		{
			//--- マテリアルデータを移す ----------------------------
			if(strlen(meta->atex[i].strTex) > 0){ meta->amat[i].pTextureFilename = meta->atex[i].strTex;}
												// ディフィーズ
			VEC4	*dif = (VEC4*)&meta->amat[i].MatD3D.Diffuse;
			meta->amat[i].MatD3D.Diffuse.a = meta->atex[i].afCol[0];
			meta->amat[i].MatD3D.Diffuse.r = meta->atex[i].afCol[1];
			meta->amat[i].MatD3D.Diffuse.g = meta->atex[i].afCol[2];
			meta->amat[i].MatD3D.Diffuse.b = meta->atex[i].afCol[3];

			VEC4	*a;							// アンビエント
			a  = (VEC4*)&meta->amat[i].MatD3D.Ambient;
			*a = *dif *	meta->atex[i].fAmb;
												// エミッシブ
			a  = (VEC4*)&meta->amat[i].MatD3D.Emissive;
			*a = *dif *	meta->atex[i].fEmi;
												// スペキュラー
			a  = (VEC4*)&meta->amat[i].MatD3D.Specular;
			*a = *dif *	meta->atex[i].fSpc;
												// パワー
			meta->amat[i].MatD3D.Power = meta->atex[i].fPower;
		}
	}
}



/*-------------------------------------------------------------------------------*
| <<< メタセコファイル読み込み >>>
|	入力	iNum	= 読み込み番号
|			strFile	= 読み込みMQOファイル名
|	戻り値	true or	false
*-------------------------------------------------------------------------------*/
BOOL	meta_load(int iNum, char *strFile)
{

	iMetaCount = 0;							// 文字読み込み位置初期化

	if((meta = (META_WORK *)malloc(sizeof(META_WORK))) == NULL){ return sys_error("meta_load : メモリーが不足しています。");}

	memset(meta, 0,	sizeof(META_WORK));		// ワーク初期化
	meta->iObjNum = -1;
	sys_folder_push();
											// テキストを読み込む
	if(!TextLoad_(strFile)){ sys_folder_pop(); return false;}
											// 失敗したら終了
	// テキスト解析
	meta->iMatID  = -1;
	meta->fSmooth = R(90);
	TextMain_(false);						// テキスト処理を行なう
	CreateMeshFromMetaseq();				// メタセコイヤのデータよりメッシュ作成
	FOLDER_SET(strFile);
	MetaTexCreate(iNum, false);				// テクスチャー作成

	// FVF 変換
	if(meta->uiVtxCount > 0 && meta->iPolyCount > 0)
	{
		meta->lpMesh->CloneMeshFVF(meta->lpMesh->GetOptions(), meta->lpMesh->GetFVF(), d3.dev, &d3.lpMesh[iNum]);

		// 諸情報登録
		d3.meshInfo[iNum].ucSubset			 = meta->iMatCount;	// サブセット数
		d3.meshInfo[iNum].ucSkinAnimationSet = 0;				// アニメーショントラック数
		d3.meshInfo[iNum].usFaces			 = meta->iPolyCount;

		// マテリアル作成
		if(meta->iMatCount > 0){ d3.matMesh[iNum] = new MATERIAL[meta->iMatCount];}

		// テクスチャーロード
		for(int i =	0; i < meta->iMatCount; i++)
		{
			d3.matMesh[    iNum][i] = meta->amat[i].MatD3D;
			d3.sMeshTexNum[iNum][i] = -1;
			if(meta->amat[i].pTextureFilename)
			{
				short	sTex = d3_tex_load(-1, meta->amat[i].pTextureFilename, true);
				if(sTex != -1){ d3.sMeshTexNum[iNum][i] = sTex, d3.texInfo[sTex].usUseCount++;}
			}
		}
		sys_folder_pop();
		
		// ヒットボックスの作成
		d3_hit_work_get(&d3.hitWork[iNum], d3.lpMesh[iNum]);
	}

	// メモリ解放
	if(MetaText	!= NULL)						// メモリーを解放
	{
		free(MetaText);
		MetaText = NULL;
	}
	if(meta	!= NULL)
	{
		free(meta);
		meta = NULL;
	}

	return true;
}



/*-------------------------------------------------------------------------------*
| <<< メタセコファイル読み込み時に可視情報をスキップするかの設定 >>>
|	入力	bSw = true  : 可視情報有効にする(デフォルト)
|				  false : 可視情報無効にする
*-------------------------------------------------------------------------------*/
void	meta_visible_skip_set(BOOL bSw)
{
	bVisibleSkip = bSw;
}



/*-------------------------------------------------------------------------------*
| <<< メタセコファイルを読み込みだバッファ解放 >>>
*-------------------------------------------------------------------------------*/
void	meta_buf_free(void)
{
	if(meta->pvtxBuf)
	{
		free(meta->pvtxBuf);
		meta->pvtxBuf = NULL;
	}
}



/*-------------------------------------------------------------------------------*
| <<< メタセコファイルを解析してバッファへ格納する >>>
|	入力	strFile	= 読み込みMQOファイル名
|	戻り値	true or	false
*-------------------------------------------------------------------------------*/
BOOL	meta_buf_load(char *strFile)
{

	iMetaCount = 0;							// 文字読み込み位置初期化

	if((meta = (META_WORK *)malloc(sizeof(META_WORK))) == NULL){ return sys_error("meta_load : メモリーが不足しています。");}

	memset(meta, 0,	sizeof(META_WORK));		// ワーク初期化
	meta->iObjNum = -1;
	sys_folder_push();
											// テキストを読み込む
	if(!TextLoad_(strFile)){ sys_folder_pop(); return false;}
											// 失敗したら終了
	// テキスト解析
	meta->iMatID  = -1;
	meta->fSmooth = R(90);
	TextMain_(false);						// テキスト処理を行なう

	// 頂点バッファにすべてのポリゴンを書き出す
	if((meta->pvtxBuf = (D3_VTX_DEFAULT *)malloc(sizeof(D3_VTX_DEFAULT) * meta->iPolyCount)) == NULL)
	{
		return sys_error("meta_load2 : メモリーが不足しています。");
	}
	for(int i = 0; i < meta->iPolyCount * 3; i++)
	{
		meta->pvtxBuf[i] = meta->avtx[meta->auiIndex[i]];
	}
	sys_folder_pop();

	// メモリ解放
	if(MetaText	!= NULL)						// メモリーを解放
	{
		free(MetaText);
		MetaText = NULL;
	}
	if(meta	!= NULL)
	{
		free(meta);
		meta = NULL;
	}

	return true;
}



/*-------------------------------------------------------------------------------*
| <<< meta_buf_load で読み込んだポリゴン数を返す >>>
|	戻り値	頂点数
*-------------------------------------------------------------------------------*/
int		meta_buf_poly_count_get(void)
{
	return meta->iPolyCount;
}



/*-------------------------------------------------------------------------------*
| <<< meta_buf_load のバッファから頂点・法線・UV 等を返す >>>
|	入力	iCount = バッファ番号
|	戻り値	頂点バッファのアドレス
*-------------------------------------------------------------------------------*/
D3_VTX_DEFAULT	*meta_buf_vertex_get(int iCount)
{
	return &meta->pvtxBuf[iCount];
}

