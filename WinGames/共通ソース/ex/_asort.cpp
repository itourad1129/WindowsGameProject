//
//	アルファソート処理
//
//		※ ポリゴンをソートし描画する。アルファ付きのポリゴンで使用すると良い。処理速度は遅いので注意！
//		※ 処理高速化のためテクスチャー統合している。そのため、端から端へ続いている UV やマイナスの UV は使えないので注意！



//--- 定数定義 ------------------------------------------------------------------------------------

#define	ASORT_W			iRenderW				// アルファソートテクスチャーソート
#define	ASORT_H			iRenderH
#define	ASORT_TEX_W		(ASORT_W / ASORT_DIV_)	// メッシュひとつあたりのテクスチャーサイズ
#define	ASORT_TEX_H		(ASORT_H / ASORT_DIV_)


//--- 構造体 --------------------------------------------------------------------------------------
struct	ASORT_WORK						// ポリが使う構造体
{
	D3_VTX_LX	avtx[3];					// 頂点情報
	float		fZ;							// ワールドトランスフォーム＋射影変換後のＺ座標
};


//--- 変数を定義 ----------------------------------------------------------------------------------
											// オフスクリーンサイズ
static	int			iRenderW  = ASORT_DEFAULT_SIZE;
static	int			iRenderH  = ASORT_DEFAULT_SIZE;
static	int			iAsortTex = -1;			// asort ライブラリィが使用するテクスチャー
static	uint		uiPoly;					// 登録ポリ数
static	ASORT_WORK	awork[ASORT_POLY_MAX];

// ソート用
static	int			iOrder[ASORT_POLY_MAX];	// こちらに管理番号
static	float		fOrder[ASORT_POLY_MAX];	// こちらに Z 座標が入る

// バッファ
static	D3_VTX_LX	avtxBuf[ASORT_POLY_MAX * 3];	


//--- 関数宣言 ------------------------------------------------------------------------------------
static	void	asort_tex_set(int iNum, int iTex);
static	void	QSort(float afDat[], int aiDat[ ], int iLeft, int iRight);



/*------------------------------------------------------------------------------*
| <<< アルファソート用オフスクリーンの横幅を得る >>>
|	戻り値	横幅
*------------------------------------------------------------------------------*/
int		asort_render_width_get(void)
{
	return ASORT_W;
}



/*------------------------------------------------------------------------------*
| <<< アルファソート用オフスクリーンの縦幅を得る >>>
|	戻り値	縦幅
*------------------------------------------------------------------------------*/
int		asort_render_height_get(void)
{
	return ASORT_W;
}



/*------------------------------------------------------------------------------*
| <<< アルファソート初期化 >>>
|	入力	iTex   = アルファソートで使用するテクスチャー番号
|			iW, iH = オフスクリーンサイズ
*------------------------------------------------------------------------------*/
void	asort_init(int iTex, int iW, int iH)
{
	//
	// オフスクリーンを作成しクリアーする
	//
	asort_free();

	iAsortTex = iTex;

	iRenderW  = iW;
	iRenderH  = iH;

	if(iRenderW > (int)d3.caps.MaxTextureWidth ){ iRenderW = d3.caps.MaxTextureWidth;}
	if(iRenderH > (int)d3.caps.MaxTextureHeight){ iRenderH = d3.caps.MaxTextureHeight;}

	d3_offscreen_create(iAsortTex, iRenderW, iRenderH);
	d3.texInfo[			iAsortTex].bRepair = true;
	d3_offscreen_begin( iAsortTex);
	d3_clear(D3_COL(0, 0, 0, 0));
	d3_offscreen_end(	iAsortTex);
}



/*------------------------------------------------------------------------------*
| <<< アルファソートテクスチャーセット >>>
|	入力	iNum = 管理番号
|			iTex = アルファソートで使用するテクスチャー番号
*------------------------------------------------------------------------------*/
static	void	asort_tex_set(int iNum, int iTex)
{

	d3_offscreen_begin(iAsortTex);

	float	fX = (iNum % ASORT_DIV_) * (float)ASORT_TEX_W;
	float	fY = (iNum / ASORT_DIV_) * (float)ASORT_TEX_H;
	d3_tex_set(				iTex);
	d3_2d_box_fill_tex_draw(&VEC2(fX, fY), &VEC2(fX + (float)ASORT_TEX_W, fY + (float)ASORT_TEX_H));
	d3_offscreen_end(		iAsortTex);
}



/*------------------------------------------------------------------------------*
| <<< 登録ポリゴンをクリア >>>
*------------------------------------------------------------------------------*/
void	asort_clear(void)
{

	uiPoly = 0;
}



/*------------------------------------------------------------------------------*
| <<< ポリゴン登録 >>>
|	入力	pavtx = 三角形の頂点
*------------------------------------------------------------------------------*/
void	asort_set(D3_VTX_LX *pavtx)
{

	if(uiPoly >= ASORT_POLY_MAX){ return;}

	ASORT_WORK	*w = &awork[uiPoly];

	memcpy(w->avtx, pavtx, sizeof(D3_VTX_LX) * 3);

	// 重心を求める
	VEC3	v = (pavtx[0].p + pavtx[1].p + pavtx[2].p) / 3.0f;

	// 重心をトランスフォーム後登録
	calc_vec3_transform_coord(&v, &v, &d3.mtxView);
	w->fZ			= v.z;
	iOrder[uiPoly] = uiPoly;
	fOrder[uiPoly] = w->fZ;

	uiPoly++;
}



/*------------------------------------------------------------------------------*
| <<< クイックソート >>>
*------------------------------------------------------------------------------*/
static	void	QSort(float afDat[], int aiDat[ ], int iLeft, int iRight)
{

	int		i, j;
	float	fPivot;

	i      = iLeft;							// ソートする配列の一番小さい要素の添字
	j      = iRight;						// ソートする配列の一番大きい要素の添字
	fPivot = afDat[(iLeft + iRight) / 2];	// 基準値を配列の中央付近にとる
	while(1)
	{
		while(afDat[i] < fPivot  ){ i++;}	// fPivot より大きい値を探す
		while(fPivot   < afDat[j]){ j--;}	// fPivot より小さい値を探す
		if(i >= j){ break;} 				// i >= j なら無限ループから抜ける
		SWAPF(afDat[i], afDat[j]);			// 交換
		SWAP( aiDat[i], aiDat[j]);
		i++;								// 次のデータ
		j--;
	}
	if(iLeft < i - 1)						// 基準値の左に 2 以上要素があれば、左の配列を Q ソートする
	{
		QSort(afDat, aiDat, iLeft, i - 1);
	}
	if(j + 1 < iRight)						// 基準値の右に 2 以上要素があれば、右の配列を Q ソートする
	{
		QSort(afDat, aiDat, j + 1, iRight);
	}
}



/*------------------------------------------------------------------------------*
| <<< 登録ポリゴンをソート >>>
*------------------------------------------------------------------------------*/
void	asort_sort(void)
{
	if(uiPoly > 0)
	{
		QSort(fOrder, iOrder, 0, uiPoly - 1);
	}
}



/*------------------------------------------------------------------------------*
| <<< 登録ポリゴンを描画 >>>
*------------------------------------------------------------------------------*/
void	asort_draw(void)
{

	if(uiPoly == 0){ return;}

	d3_stat_fvf_set(	  D3_FVF_LX);
	d3_stat_transform_set(&d3.mtxIdentity);
	d3_tex_set(			  iAsortTex);
	d3.dev->SetRenderState(D3DRS_AMBIENT, D3_COL((int)(d3.vecAmbient.w * 255), (int)(d3.vecAmbient.x * 255), (int)(d3.vecAmbient.y * 255), (int)(d3.vecAmbient.z * 255)));

	// 登録ポリゴンを一枚ずつ描画(奥から手前に)
	for(uint i = uiPoly - 1; i != -1; i--)
	{
		ASORT_WORK	*w = &awork[iOrder[i]];
		d3.dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST,	1, w->avtx, sizeof(D3_VTX_LX));
		d3.iPolyCount++;
	}
}



/*------------------------------------------------------------------------------*
| <<< 登録ポリゴンをバッファに貯める >>>
*------------------------------------------------------------------------------*/
static	D3_VTX_LX	avtxAsort[ASORT_BUF_MAX_ * 3];
static	int			iAsortCount;
void	asort_buf_set(void)
{

	if(uiPoly == 0){ return;}

	// 登録ポリゴンを一枚ずつ描画(奥から手前に)
	int	iCount = 0;
	for(uint i = uiPoly - 1; i != -1; i--)
	{
		ASORT_WORK	*w = &awork[iOrder[i]];
		
		memcpy(&avtxAsort[iCount * 3], w->avtx, sizeof(D3_VTX_LX) * 3);
		iCount++;

		if(iCount > ASORT_BUF_MAX_){ break;}
	}
	iAsortCount = iCount;
}
void	asort_buf_draw(void)
{

	if(iAsortCount > 0)
	{
		d3_stat_fvf_set(	  D3_FVF_LX);
		d3_stat_transform_set(&d3.mtxIdentity);
		d3_tex_set(			  iAsortTex);
		//d3.dev->SetRenderState(D3DRS_AMBIENT, D3_COL((int)(d3.vecAmbient.w * 255), (int)(d3.vecAmbient.x * 255), (int)(d3.vecAmbient.y * 255), (int)(d3.vecAmbient.z * 255)));
		d3.dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST,	iAsortCount, avtxAsort, sizeof(D3_VTX_LX));
	}
}


/*------------------------------------------------------------------------------*
| <<< asort ライブラリィを開放 >>>
*------------------------------------------------------------------------------*/
void	asort_free(void)
{

	if(iAsortTex != -1){ d3_tex_free(iAsortTex);}
	iAsortTex  = -1;
	uiPoly     = 0;
}



/*---------------------------------------------------------------------------------------------------------------------------------------------------
	<<< メッシュ用 >>>
*--------------------------------------------------------------------------------------------------------------------------------------------------*/

#define	MESH_MAX		D3_MESH_MAX			// asort メッシュ登録数
#define	MATERIAL_MAX	1					// マテリアル数(mqo の材質の数)


//--- 構造体 --------------------------------------------------------------------------------------
struct	FACE_WORK
{
	VEC3	avecPos[3];
	VEC2	avecUV[ 3];
};


//--- 変数を定義 ----------------------------------------------------------------------------------
static	FACE_WORK	*pfaceWork[MESH_MAX];
static	uint		auiFaceMax[MESH_MAX];
static	int			aiTex[	   MESH_MAX];
static	int			iAsortMeshCount;		// 読み込みメッシュ数
static	int			iAsortTexCount;			// 読み込みテクスチャー数

//static	int			iAsortMeshCount;		// 登録数
//static	int			iControlID[MESH_MAX];	// 管理番号


//--- 関数宣言-------------------------------------------------------------------------------------


/*------------------------------------------------------------------------------*
| <<< asort メッシュクリア >>>
*------------------------------------------------------------------------------*/
void	asort_mesh_init(void)
{

	asort_mesh_free();						// 読み込みメッシュ解放

	iAsortMeshCount = 0;					// 読み込みメッシュ数
	iAsortTexCount  = 0;					// 累計テクスチャー数
	//iAsortMeshCount = 0;					// 登録数クリア
}



/*------------------------------------------------------------------------------*
| <<< asort 用にメッシュ読み込み >>>
|	入力	iMesh = 登録するメッシュ番号(d3_xfile_load の読み込み番号)
|	戻り値	FALSE : 読み込み失敗
*------------------------------------------------------------------------------*/
BOOL	asort_mesh_load(int iMesh)
{
	// メモリ解放
	asort_mesh_free(iMesh);

	// 判定する面の数
	auiFaceMax[iMesh] = d3.meshInfo[iMesh].usFaces;
	if(auiFaceMax[iMesh] == 0){ return FALSE;}

	// メモリ確保(ポリゴン数、ワークを確保する)
	pfaceWork[iMesh] = (FACE_WORK *)malloc(sizeof(FACE_WORK) * auiFaceMax[iMesh]);
	memset(pfaceWork[iMesh], 0, sizeof(FACE_WORK) * auiFaceMax[iMesh]);

	//--- 頂点情報を書き込む ----------------------------------------
	D3_VTX_DEFAULT	*pvtxBuf;				// 頂点バッファ
	WORD			*pwIndex;				// インデックスバッファ
	DWORD			*pdwMaterial;			// マテリアルバッファ
	d3.lpMesh[iMesh]->LockVertexBuffer(   D3DLOCK_DISCARD, (void **)&pvtxBuf);
	d3.lpMesh[iMesh]->LockIndexBuffer(    D3DLOCK_DISCARD, (void **)&pwIndex);
	d3.lpMesh[iMesh]->LockAttributeBuffer(D3DLOCK_DISCARD, &pdwMaterial);

	// インデックスバッファ転送
	FACE_WORK	*w = pfaceWork[iMesh];
	for(uint i = 0; i < auiFaceMax[iMesh]; i++, w++)
	{
		w->avecPos[0] =  pvtxBuf[pwIndex[i * 3 + 0]].p;
		w->avecPos[1] =  pvtxBuf[pwIndex[i * 3 + 1]].p;
		w->avecPos[2] =  pvtxBuf[pwIndex[i * 3 + 2]].p;
											// マテリアル番号 + 累計テクスチャー番号
		DWORD	dwMat = pdwMaterial[i] + iAsortTexCount;
											// UV 位置
		VEC2	vecOfs((float)((dwMat % ASORT_DIV_) * ASORT_TEX_W) / (float)ASORT_W, (float)((dwMat / ASORT_DIV_) * ASORT_TEX_H) / (float)ASORT_H);
		w->avecUV[ 0] = (pvtxBuf[pwIndex[i * 3 + 0]].uv / ASORT_DIV_) + vecOfs;
		w->avecUV[ 1] = (pvtxBuf[pwIndex[i * 3 + 1]].uv / ASORT_DIV_) + vecOfs;
		w->avecUV[ 2] = (pvtxBuf[pwIndex[i * 3 + 2]].uv / ASORT_DIV_) + vecOfs;
	}
	// 頂点バッファ閉じる
	d3.lpMesh[iMesh]->UnlockAttributeBuffer();
	d3.lpMesh[iMesh]->UnlockIndexBuffer();
	d3.lpMesh[iMesh]->UnlockVertexBuffer();

	//--- テクスチャー合体 ------------------------------------------
	for(uint i = 0; i < d3.meshInfo[iMesh].ucSubset; i++)
	{
		if(d3.sMeshTexNum[iMesh][i] > 0)
		{
 			asort_tex_set(i + iAsortTexCount, d3.sMeshTexNum[iMesh][i]);
		}
	}
	iAsortTexCount += d3.meshInfo[iMesh].ucSubset;
	iAsortMeshCount++;

	return TRUE;
}



/*------------------------------------------------------------------------------*
| <<< asort 用にメッシュ読み込み >>>	※ メタセコイヤのデータをもとに全ポリゴンの頂点を読み込む(ヒットマップ等で使用のこと)
|	入力	iMesh   = メッシュ番号
|			strFile = ファイル名
|	戻り値	FALSE : 読み込み失敗
*------------------------------------------------------------------------------*/
BOOL	asort_mesh_load(int iMesh, char *strFile)
{

	if(!meta_buf_load(strFile)){ return FALSE;}

	// メモリ解放
	asort_mesh_free(iMesh);

	// 判定する面の数
	auiFaceMax[iMesh] = meta_buf_poly_count_get();
	if(auiFaceMax[iMesh] == 0){ return FALSE;}

	// メモリ確保(ポリゴン数、ワークを確保する)
	pfaceWork[iMesh] = (FACE_WORK *)malloc(sizeof(FACE_WORK) * auiFaceMax[iMesh]);
	memset(pfaceWork[iMesh], 0, sizeof(FACE_WORK) * auiFaceMax[iMesh]);

	// UV 位置
	VEC2	vecOfs((float)((iMesh % ASORT_DIV_) * ASORT_TEX_W) / (float)ASORT_W, (float)((iMesh / ASORT_DIV_) * ASORT_TEX_H) / (float)ASORT_H);

	//--- 頂点情報を書き込む ----------------------------------------
	// インデックスバッファ転送
	FACE_WORK	*w = pfaceWork[iMesh];
	for(uint i = 0; i < auiFaceMax[iMesh]; i++, w++)
	{
		w->avecPos[0] =  meta_buf_vertex_get(i * 3 + 0)->p;
		w->avecPos[1] =  meta_buf_vertex_get(i * 3 + 1)->p;
		w->avecPos[2] =  meta_buf_vertex_get(i * 3 + 2)->p;
		w->avecUV[ 0] = (meta_buf_vertex_get(i * 3 + 0)->uv / ASORT_DIV_) + vecOfs;
		w->avecUV[ 1] = (meta_buf_vertex_get(i * 3 + 1)->uv / ASORT_DIV_) + vecOfs;
		w->avecUV[ 2] = (meta_buf_vertex_get(i * 3 + 2)->uv / ASORT_DIV_) + vecOfs;
	}
	
	//--- バッファ解放 ----------------------------------------------
	meta_buf_free();

	return TRUE;
}



/*------------------------------------------------------------------------------*
| <<< asort で読み込んだメッシュ解放 >>>
|	入力	iMesh = メッシュ番号
*------------------------------------------------------------------------------*/
void	asort_mesh_free(int iNum)
{
	auiFaceMax[iNum] = 0;
	if(pfaceWork[iNum] != NULL)
	{
		free(pfaceWork[iNum]);
		pfaceWork[iNum] = NULL;
	}
}



/*------------------------------------------------------------------------------*
| <<< asort で読み込んだメッシュ解放 >>>
*------------------------------------------------------------------------------*/
void	asort_mesh_free(void)
{

	for(int i = 0; i < iAsortMeshCount; i++)
	{
		asort_mesh_free(i);
	}
}



/*------------------------------------------------------------------------------*
| <<< asort 用に読み込んだメッシュ描画 >>>	※ソートしません
|	入力	iMesh      = メッシュ番号
|			*pmtxWorld = ワールド行列
*------------------------------------------------------------------------------*/
void	asort_mesh_draw(int iMesh, MTX *pmtxWorld)
{

	d3_stat_fvf_set(	  D3_FVF_LX);
	d3_stat_transform_set(pmtxWorld);
	d3_tex_set(			  iAsortTex);

	// ポリゴン一枚ずつ奥から描画
	FACE_WORK	*w = pfaceWork[iMesh];
	for(uint i = 0; i < auiFaceMax[iMesh]; i++, w++)
	{
		//memcpy(w->avecPos, meta_face_get(0, i)->avecPos, sizeof(VEC3) * 3);
		D3_VTX_LX	avtx[3];
		avtx[0].p  = w->avecPos[0];
		avtx[0].c  = D3_COL_NORMAL;
		avtx[0].uv = w->avecUV[ 0];
		avtx[1].p  = w->avecPos[1];
		avtx[1].c  = D3_COL_NORMAL;
		avtx[1].uv = w->avecUV[ 1];
		avtx[2].p  = w->avecPos[2];
		avtx[2].c  = D3_COL_NORMAL;
		avtx[2].uv = w->avecUV[ 2];
		d3.dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST,	1, avtx, sizeof(D3_VTX_LX));
	}
}



/*------------------------------------------------------------------------------*
| <<< asort 用にメッシュを描画登録 >>>
|	入力	iMesh      = メッシュ番号
|			*pmtxWorld = ワールド行列
*------------------------------------------------------------------------------*/
void	asort_mesh_set(int iMesh, MTX *pmtxWorld)
{

	D3_VTX_LX	avtx[3];

	avtx[0].c = D3_COL_NORMAL;
	avtx[1].c = D3_COL_NORMAL;
	avtx[2].c = D3_COL_NORMAL;

	// ポリゴンを一枚ずつワールドトランスフォーム→射影変換
	FACE_WORK	*w = pfaceWork[iMesh];
	for(uint i = 0; i < auiFaceMax[iMesh]; i++, w++)
	{
		calc_vec3_transform_coord(&avtx[0].p, &w->avecPos[0], pmtxWorld);
		calc_vec3_transform_coord(&avtx[1].p, &w->avecPos[1], pmtxWorld);
		calc_vec3_transform_coord(&avtx[2].p, &w->avecPos[2], pmtxWorld);
		avtx[0].uv = w->avecUV[ 0];
		avtx[1].uv = w->avecUV[ 1];
		avtx[2].uv = w->avecUV[ 2];
		asort_set(avtx);
	}	
}


