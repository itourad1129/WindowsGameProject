/**************************************************************************************************
	[[[	メッシュ系 ]]]
**************************************************************************************************/


/*----------------------------------------------------------------------*
| <<< ポリゴン表示 >>>
|	入力	iNum	    = モデル番号
|			iShaderType = シェーディングタイプ
|			mtxWorld    = 配置座標
|			fAlp	    = アルファ値(0.f ～ 1.f)
*----------------------------------------------------------------------*/
void	d3_3d_draw(int iNum, int iShaderType, MTX *mtxWorld, float fAlp)
{
	d3_shader_set(iShaderType);
	d3_mesh_draw(iNum, iShaderType, mtxWorld, fAlp, 0, d3.meshInfo[iNum].ucSubset);
}



/*----------------------------------------------------------------------*
| <<< ポリゴン表示 >>>					※半透明はオフ
|	入力	iNum	    = モデル番号
|			iShaderType = シェーディングタイプ
|			mtxWorld    = 配置座標
*----------------------------------------------------------------------*/
void	d3_3d_draw(int iNum, int iShaderType, MTX *mtxWorld)
{
	d3.eff = d3.lpEffect[iShaderType];
	d3_mesh_draw(iNum, iShaderType, mtxWorld, 1, 0, d3.meshInfo[iNum].ucSubset);
}



/*----------------------------------------------------------------------*
| <<< ポリゴン表示 >>>					※デフォルトで描画
|	入力	iNum	 = モデル番号
|			mtxWorld = 配置座標
|			fAlp	 = アルファ値(0.f ～ 1.f)
*----------------------------------------------------------------------*/
void	d3_3d_draw(int iNum, MTX *mtxWorld, float fAlp)
{

	d3.eff = d3.lpEffect[D3_SHADER_TYPE_NORMAL];
	if(fAlp < 1.0f){ d3_stat_alpha_set(true);}
	d3_mesh_draw(iNum, D3_SHADER_TYPE_NORMAL, mtxWorld, fAlp, 0, d3.meshInfo[iNum].ucSubset);
}



/*----------------------------------------------------------------------*
| <<< ポリゴン表示 >>>					※デフォルト描画、半透明オフ
|	入力	iNum  =	モデル番号
|			world =	配置座標
*----------------------------------------------------------------------*/
void	d3_3d_draw(int iNum, MTX *mtxWorld)
{
	d3.eff = d3.lpEffect[D3_SHADER_TYPE_NORMAL];
	d3_mesh_draw(iNum, D3_SHADER_TYPE_NORMAL, mtxWorld, 1, 0, d3.meshInfo[iNum].ucSubset);
}



/*----------------------------------------------------------------------*
| <<< 指定番号のサブセットを描画 >>>
|	入力	iNum	    = モデル番号
|			iMatNum	    = パーツ番号
|			iShaderType = シェーダータイプ
|			mtxWorld    = 配置座標
|			fAlp	    = アルファ値(0.f ～ 1.f)
*----------------------------------------------------------------------*/
void	d3_3d_draw_parts(int iNum, int iMatNum, int iShaderType, MTX *mtxWorld, float fAlp)
{
	if((uint)iMatNum >=	d3.meshInfo[iNum].ucSubset){ return;}
	d3.eff = d3.lpEffect[iShaderType];
	d3_mesh_draw(iNum, iShaderType, mtxWorld, fAlp, iMatNum, iMatNum +	1);
}



/*----------------------------------------------------------------------*
| <<< 指定番号のサブセットを描画 >>>
|	入力	iNum	    = モデル番号
|			iMatNum	    = パーツ番号
|			mtxWorld    = 配置座標
|			fAlp	    = アルファ値(0.f ～ 1.f)
*----------------------------------------------------------------------*/
void	d3_3d_draw_parts(int iNum, int iMatNum, MTX *mtxWorld, float fAlp)
{

	if((uint)iMatNum >=	d3.meshInfo[iNum].ucSubset){ return;}

	int	iShaderType = D3_SHADER_TYPE_NORMAL;
	d3.eff = d3.lpEffect[iShaderType];
	d3_mesh_draw(iNum, iShaderType, mtxWorld, fAlp, iMatNum, iMatNum +	1);
}



/*---------------------------------------------------------------------*
| <<< 指定番号のサブセットを描画 >>>
|	入力	iNum	    = モデル番号
|			iMatNum	    = パーツ番号
|			mtxWorld    = 配置座標
*----------------------------------------------------------------------*/
void	d3_3d_draw_parts(int iNum, int iMatNum, MTX *mtxWorld)
{

	if((uint)iMatNum >=	d3.meshInfo[iNum].ucSubset){ return;}

	int	iShaderType = D3_SHADER_TYPE_NORMAL;
	d3.eff = d3.lpEffect[iShaderType];
	d3_mesh_draw(iNum, iShaderType, mtxWorld, 1, iMatNum, iMatNum +	1);
}



/*----------------------------------------------------------------------*
| <<< メッシュ描画 >>>
|	入力	iNum		= モデル番号
|			iShaderType = シェーディングタイプ
|			mtxWorld	= ワールド行列
|			fAlp		= 半透明値
|			iStart,iEnd	= 描画開始位置、終了位置
*----------------------------------------------------------------------*/
void	d3_mesh_draw(int iNum, int iShaderType, MTX *mtxWorld, float fAlp, int iStart, int iEnd)
{

	//--- 未読み込みの場合はロードしにいく --------------------------
	if(!d3.meshInfo[iNum].bLoad){ d3_xfile_load(iNum, NULL, true);}

	//--- メッシュ未読み込みチェック --------------------------------
	if(d3.lpMesh[iNum] == NULL){ return;}

	//--- タイプ別にシェーダー呼び出し ------------------------------
	d3.eff = d3.lpEffect[iShaderType];
	
	if(d3.eff == NULL){ return;}

	bool	(*a)(int iNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMC, LPD3DXBONECOMBINATION pBC, MTX *mtx, float fAlp);
	a = (bool(*)(int iNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMC, LPD3DXBONECOMBINATION pBC, MTX *mtx, float fAlp))pShaderTbl[iShaderType];
	if(a != NULL)
	{
		for(int	i =	iStart;	i <	iEnd; i++)
		{
			bool	bDraw = (*a)(iNum, i, NULL, NULL, mtxWorld, fAlp);
		}
		d3.iPolyCount += d3.lpMesh[iNum]->GetNumFaces();		// 頂点数計算
	}
}



/**************************************************************************************************
	[[[	メッシュ読み込み、作成系 ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< ＸファイルのＸの最小値を得る >>>
|	入力	iXNum = Ｘファイルの読み込み番号
*------------------------------------------------------------------------------*/
float	gr_xfile_xmin_get(int iXNum)
{
	
	// メッシュ未読み込み
	if(d3.lpMesh[iXNum] == NULL){ return 0;}

	// 頂点バッファのサイズ
	DWORD	dwVtxSize = d3.lpMesh[iXNum]->GetNumBytesPerVertex();

	// 全頂点に対し、Ｙ座標の中で最少のものを探す
	LPDIRECT3DVERTEXBUFFER9	pVB;
	uchar					*pucBuf;

	d3.lpMesh[iXNum]->GetVertexBuffer(&pVB);
	pVB->Lock(0, 0,	(void**)&pucBuf, 0);
	float	fX = 1000;
	for(int	i =	0; i < (int)d3.meshInfo[iXNum].usVertex; i++)
	{
		D3_VTX_DEFAULT	*v = (D3_VTX_DEFAULT*)&pucBuf[dwVtxSize * i];
		if(fX > v->p.x){ fX = v->p.x;}
	}
	pVB->Unlock();
	pVB->Release();

	return	fX;
}


/*------------------------------------------------------------------------------*
| <<< ＸファイルのＸの最大値を得る >>>
|	入力	iXNum = Ｘファイルの読み込み番号
*------------------------------------------------------------------------------*/
float	gr_xfile_xmax_get(int iXNum)
{
	
	// メッシュ未読み込み
	if(d3.lpMesh[iXNum] == NULL){ return 0;}

	// 頂点バッファのサイズ
	DWORD	dwVtxSize = d3.lpMesh[iXNum]->GetNumBytesPerVertex();

	// 全頂点に対し、Ｙ座標の中で最少のものを探す
	LPDIRECT3DVERTEXBUFFER9	pVB;
	uchar					*pucBuf;

	d3.lpMesh[iXNum]->GetVertexBuffer(&pVB);
	pVB->Lock(0, 0,	(void**)&pucBuf, 0);
	float	fX = -1000;
	for(int	i =	0; i < (int)d3.meshInfo[iXNum].usVertex; i++)
	{
		D3_VTX_DEFAULT	*v = (D3_VTX_DEFAULT*)&pucBuf[dwVtxSize * i];
		if(fX < v->p.x){ fX = v->p.x;}
	}
	pVB->Unlock();
	pVB->Release();

	return	fX;
}



/*------------------------------------------------------------------------------*
| <<< ＸファイルのＹの最小値を得る >>>
|	入力	iXNum = Ｘファイルの読み込み番号
*------------------------------------------------------------------------------*/
float	gr_xfile_ymin_get(int iXNum)
{
	
	// メッシュ未読み込み
	if(d3.lpMesh[iXNum] == NULL){ return 0;}

	// 頂点バッファのサイズ
	DWORD	dwVtxSize = d3.lpMesh[iXNum]->GetNumBytesPerVertex();

	// 全頂点に対し、Ｙ座標の中で最少のものを探す
	LPDIRECT3DVERTEXBUFFER9	pVB;
	uchar					*pucBuf;

	d3.lpMesh[iXNum]->GetVertexBuffer(&pVB);
	pVB->Lock(0, 0,	(void**)&pucBuf, 0);
	float	fY = 1000;
	for(int	i =	0; i < (int)d3.meshInfo[iXNum].usVertex; i++)
	{
		D3_VTX_DEFAULT	*v = (D3_VTX_DEFAULT*)&pucBuf[dwVtxSize * i];
		if(fY > v->p.y){ fY = v->p.y;}
	}
	pVB->Unlock();
	pVB->Release();

	return	fY;
}


/*------------------------------------------------------------------------------*
| <<< ＸファイルのＹの最大値を得る >>>
|	入力	iXNum = Ｘファイルの読み込み番号
*------------------------------------------------------------------------------*/
float	gr_xfile_ymax_get(int iXNum)
{
	
	// メッシュ未読み込み
	if(d3.lpMesh[iXNum] == NULL){ return 0;}

	// 頂点バッファのサイズ
	DWORD	dwVtxSize = d3.lpMesh[iXNum]->GetNumBytesPerVertex();

	// 全頂点に対し、Ｙ座標の中で最少のものを探す
	LPDIRECT3DVERTEXBUFFER9	pVB;
	uchar					*pucBuf;

	d3.lpMesh[iXNum]->GetVertexBuffer(&pVB);
	pVB->Lock(0, 0,	(void**)&pucBuf, 0);
	float	fY = -1000;
	for(int	i =	0; i < (int)d3.meshInfo[iXNum].usVertex; i++)
	{
		D3_VTX_DEFAULT	*v = (D3_VTX_DEFAULT*)&pucBuf[dwVtxSize * i];
		if(fY < v->p.y){ fY = v->p.y;}
	}
	pVB->Unlock();
	pVB->Release();

	return	fY;
}


/*------------------------------------------------------------------------------*
| <<< ＸファイルのＺの最小値を得る >>>
|	入力	iXNum = Ｘファイルの読み込み番号
*------------------------------------------------------------------------------*/
float	gr_xfile_zmin_get(int iXNum)
{
	
	// メッシュ未読み込み
	if(d3.lpMesh[iXNum] == NULL){ return 0;}

	// 頂点バッファのサイズ
	DWORD	dwVtxSize = d3.lpMesh[iXNum]->GetNumBytesPerVertex();

	// 全頂点に対し、Ｙ座標の中で最少のものを探す
	LPDIRECT3DVERTEXBUFFER9	pVB;
	uchar					*pucBuf;

	d3.lpMesh[iXNum]->GetVertexBuffer(&pVB);
	pVB->Lock(0, 0,	(void**)&pucBuf, 0);
	float	fZ = 1000;
	for(int	i =	0; i < (int)d3.meshInfo[iXNum].usVertex; i++)
	{
		D3_VTX_DEFAULT	*v = (D3_VTX_DEFAULT*)&pucBuf[dwVtxSize * i];
		if(fZ > v->p.z){ fZ = v->p.z;}
	}
	pVB->Unlock();
	pVB->Release();

	return	fZ;
}


/*------------------------------------------------------------------------------*
| <<< ＸファイルのＺの最大値を得る >>>
|	入力	iXNum = Ｘファイルの読み込み番号
*------------------------------------------------------------------------------*/
float	gr_xfile_zmax_get(int iXNum)
{
	
	// メッシュ未読み込み
	if(d3.lpMesh[iXNum] == NULL){ return 0;}

	// 頂点バッファのサイズ
	DWORD	dwVtxSize = d3.lpMesh[iXNum]->GetNumBytesPerVertex();

	// 全頂点に対し、Ｙ座標の中で最少のものを探す
	LPDIRECT3DVERTEXBUFFER9	pVB;
	uchar					*pucBuf;

	d3.lpMesh[iXNum]->GetVertexBuffer(&pVB);
	pVB->Lock(0, 0,	(void**)&pucBuf, 0);
	float	fZ = -1000;
	for(int	i =	0; i < (int)d3.meshInfo[iXNum].usVertex; i++)
	{
		D3_VTX_DEFAULT	*v = (D3_VTX_DEFAULT*)&pucBuf[dwVtxSize * i];
		if(fZ < v->p.z){ fZ = v->p.z;}
	}
	pVB->Unlock();
	pVB->Release();

	return	fZ;
}



/*----------------------------------------------------------------------*
| <<< 境界ボックス／境界球の算出 >>>
|	入力	no	   = Ｘファイル読み込み番号
|			p_mesh = メッシュのポインター
*----------------------------------------------------------------------*/
void	d3_hit_work_get(D3_HIT *pHit, LPD3DXMESH pMesh)
{

	LPDIRECT3DVERTEXBUFFER9	pVB;
	BYTE					*pByte = NULL;

	pMesh->GetVertexBuffer(&pVB);
	pVB->Lock(0, 0,	(VOID**)&pByte,	0);
	D3DXComputeBoundingSphere((VEC3*)pByte, pMesh->GetNumVertices(), D3DXGetFVFVertexSize(pMesh->GetFVF()), &pHit->vecCenter, &pHit->fRadius);
	D3DXComputeBoundingBox(	  (VEC3*)pByte, pMesh->GetNumVertices(), D3DXGetFVFVertexSize(pMesh->GetFVF()), &pHit->vecMin,	   &pHit->vecMax);
	pVB->Unlock();
	pVB->Release();

	VEC3	*min = &pHit->vecMin;
	VEC3	*max = &pHit->vecMax;

	// boxにより入れ替わっている番号があるのでチェックのこと
	pHit->vecLocal[0] =	VEC3(min->x, min->y, min->z);
	pHit->vecLocal[1] =	VEC3(max->x, min->y, min->z);
	pHit->vecLocal[3] =	VEC3(min->x, max->y, min->z);
	pHit->vecLocal[2] =	VEC3(max->x, max->y, min->z);
	pHit->vecLocal[4] =	VEC3(min->x, min->y, max->z);
	pHit->vecLocal[5] =	VEC3(max->x, min->y, max->z);
	pHit->vecLocal[7] =	VEC3(min->x, max->y, max->z);
	pHit->vecLocal[6] =	VEC3(max->x, max->y, max->z);
}



/*----------------------------------------------------------------------*
| <<< メッシュの最適化 >>>
|	入力	*out	   = 最適化後の出力位置
|			p_mesh	   = 最適前のメッシュポインター
|			p_mesh_adj = メッシュの位置
*----------------------------------------------------------------------*/
static	void	d3_mesh_optimized(LPD3DXMESH p_mesh, LPD3DXBUFFER p_mesh_adj)
{
	DWORD	*p = new DWORD[p_mesh->GetNumFaces() * 3];

	p_mesh->OptimizeInplace(d3.dwMeshOptimized,	(DWORD*)p_mesh_adj->GetBufferPointer(),	p, NULL, NULL);
	delete(p);
}



/*----------------------------------------------------------------------*
| <<< メッシュオブジェクト削除 >>>		※メッシュ情報を全て破棄
|	入力	iNum = メッシュの読み込み番号(-1:全開放)	
*----------------------------------------------------------------------*/
void	d3_xfile_free(int iNum)
{

	if(iNum == -1){ d3_xfile_free_all(); return;}

	if(d3.lpMesh[iNum] == NULL){	return;}

	// テクスチャーを開放
	for(int i =	0; i < (int)d3.meshInfo[iNum].ucSubset;	i++)
	{
		int	a = d3.sMeshTexNum[iNum][i];
		if(a < 0){ continue;}
		d3.texInfo[a].usUseCount--;
		if((short)d3.texInfo[a].usUseCount <= 0)
		{
			SAFE_RELEASE(d3.tex[a]);
			d3.texInfo[a].usUseCount     = 0;
			d3.texInfo[a].strFullPath[0] = NULL;
		}
	}

	SAFE_DELETE( d3.matMesh[iNum]);			// マテリアル
	SAFE_RELEASE(d3.lpMesh[ iNum]);			// メッシュ
	d3.meshInfo[iNum].strFullPath[0] = NULL;
	d3.meshInfo[iNum].bResource		 = false;
	d3.hitWork[ iNum].fRadius		 = 0;
}



/*----------------------------------------------------------------------*
| <<< 全メッシュオブジェクト削除 >>>
*----------------------------------------------------------------------*/
void	d3_xfile_free_all(void)
{
	for(int	i =	0; i < D3_MESH_MAX; ++i)
	{
		d3_xfile_free(i);
		skin_free(i);
	}
}



/*----------------------------------------------------------------------*
| <<< 最適化とパンプの作成 >>>
|	入力	iID		   = ID	番号
|			pMesh	   = メッシュの格納位置
*----------------------------------------------------------------------*/
void	d3_mesh_make(int iNum, LPD3DXMESH pMesh)
{

	//--- メッシュを形成する	-------------------------------------
	pMesh->CloneMeshFVF(pMesh->GetOptions(), /*pMesh->GetFVF() |*/D3_FVF_DEFAULT, d3.dev, &d3.lpMesh[iNum]);

	//--- 法線作成 --------------------------------------------------
	if(!(pMesh->GetFVF() & D3DFVF_NORMAL))		// 法線が無いときは作成のこと
	{
		D3DXComputeNormals(d3.lpMesh[iNum], NULL);
	}
	//--- メッシュの最適化 ------------------------------------------
	DWORD	*rgdwAdjacency = NULL;
	rgdwAdjacency =	new	DWORD[d3.lpMesh[iNum]->GetNumFaces() * 3];
	if(rgdwAdjacency !=	NULL)
	{
		pMesh->GenerateAdjacency(1e-6f,	rgdwAdjacency);
		pMesh->OptimizeInplace(d3.dwMeshOptimized, rgdwAdjacency, NULL,	NULL, NULL);
		delete []rgdwAdjacency;
	}
	//--- 最適化後の頂点数、ポリゴン数を得る ------------------------
	d3.meshInfo[iNum].usVertex = (ushort)d3.lpMesh[iNum]->GetNumVertices();
	d3.meshInfo[iNum].usFaces  = (ushort)d3.lpMesh[iNum]->GetNumFaces();
	pMesh->Release();

	pMesh =	d3.lpMesh[iNum];
}



/*------------------------------------------------------------------------------*
| <<< Ｘファイルの読み込みチェック >>>
|	入力	iNum	 = メッシュ管理番号
|	戻り値	true : 読み込み済み
*------------------------------------------------------------------------------*/
bool	d3_xfile_check(int iNum)
{

	return (d3.lpMesh[iNum] != NULL);
}



/*----------------------------------------------------------------------*
| <<< Ｘファイルの読み込み	>>>
|	入力	iNum	    = 読み込み番号
|			strFile		= 読み込みファイル名( NULL の場合は d3.texInfo より読み込み)
|			bLoadAtOnce = true : 即時読み込み
|			bSysMem     = true : システムメモリへ読み込み
|			bErrMessage = true : エラーメッセージを表示する
|	戻り値	true or	false
*----------------------------------------------------------------------*/
bool	d3_xfile_load(int iNum, char *pstrFile, bool bLoadAtOnce, bool bSysMem, bool bErrMessage)
{

	char	strPath[_MAX_PATH + 1];

	if(pstrFile != NULL)
	{
		//--- 拡張子がないものはリソースとみなす --------------------
		d3.meshInfo[iNum].bResource = true;
		int		i = 0;
		while(pstrFile[i] != NULL)
		{
			if(ISKANJI(pstrFile[i]   )){ i += 2;									}
			else if(pstrFile[i] == '.'){ d3.meshInfo[iNum].bResource = false; break;}
			else					   { i++;									    }
		}

		//--- ファイル名を作成 --------------------------------------
		//	　リソースの場合にはただコピー
		if(!d3.meshInfo[iNum].bResource)
		{
			char	strName[_MAX_FNAME + 1];
			char	strExt[ _MAX_EXT   + 1];
			sys_div_path(strPath, strName, strExt, pstrFile);
			sprintf(strPath, "%s%s%s", strPath, strName, strExt);
		}
		else{ strcpy(strPath, pstrFile);}

		//--- 同名ならば、読み込まないで終了 ------------------------
		if(d3.lpMesh[iNum] != NULL && d3.meshInfo[iNum].bLoad)
		{
			if(strcmp(d3.meshInfo[iNum].strFullPath, strPath) == 0){ return false;}
		}
	}
	//--- 後読みの場合 ----------------------------------------------
	else
	{
		strcpy(strPath, d3.meshInfo[iNum].strFullPath);	// フォルダー名をコピー
	}
	if(strlen(strPath) == 0){ return false;}

	//--- 即時、読み込みで無い場合 ----------------------------------
	if(!bLoadAtOnce)
	{
		strcpy(d3.meshInfo[iNum].strFullPath, strPath);	// ファイル名保存
		d3.meshInfo[iNum].bLoad = false;
		return true;
	}

	//--- 前回分を消去 ----------------------------------------------
	skin_free(	  iNum);
	d3_xfile_free(iNum);

	//--- メタセコファイルか調べる ----------------------------------
#ifdef __MQO
	if(strstr(strPath, ".mqo"))
	{
		BOOL	bRet = (BOOL)meta_load(iNum, strPath);
		strcpy(d3.meshInfo[iNum].strFullPath, strPath);	// フルパスをコピー
		d3.meshInfo[iNum].bSkin = false;
		d3.meshInfo[iNum].bLoad = true;					// 読み込みフラグオン
														// タイムスタンプ登録
		sys_timestamp_get(&d3.meshInfo[iNum].ftLastWrite, d3.meshInfo[iNum].strFullPath);
		return bRet ? true : false;
	}
#endif
	//--- メッシュ読み込み	-----------------------------------------
	LPD3DXMESH		pMesh;
	LPD3DXBUFFER	pMatBuf	= NULL;
	DWORD			n;									// マテリアル数
	HRESULT			h;

	DWORD			dwFlag;
	if(bSysMem){ dwFlag = D3DXMESH_SYSTEMMEM;}
	else	   { dwFlag = D3DXMESH_MANAGED ;}
	
	if(d3.meshInfo[iNum].bResource){ h = D3DXLoadMeshFromXResource(NULL, strPath, "MESH", dwFlag, d3.dev, NULL, &pMatBuf, NULL, &n, &pMesh);}
	else						   { h = D3DXLoadMeshFromX(		 	     strPath,		  dwFlag, d3.dev, NULL, &pMatBuf, NULL, &n, &pMesh);}

											// フォルダー位置を元の状態へ戻してリターン
	if(FAILED(h))
	{
		if(bErrMessage)
		{
			return sys_error("d3_xfile_load: %s が読み込めませんでした。", strPath);
		}
		else
		{
			return false;
		}
	}

	if(!d3.meshInfo[iNum].bResource){ sys_folder_push(), FOLDER_SET(strPath);}

	d3_mesh_make(iNum, pMesh);				// メッシュを作成
										
	// マテリアル確保
	SAFE_DELETE(d3.matMesh[iNum]);
	if(n > 0){ d3.matMesh[iNum] = new MATERIAL[n];}
	d3.meshInfo[iNum].ucSubset			 = (uchar)n;	// サブセット数
	d3.meshInfo[iNum].ucSkinAnimationSet = 0;			// アニメーショントラック数

	// テクスチャーロード
	sys_folder_push();
	D3DXMATERIAL	*m = (D3DXMATERIAL*)pMatBuf->GetBufferPointer();
	for(uint i = 0; i < n; i++)
	{
		d3.matMesh[        iNum][i] = m[i].MatD3D;
		d3.sMeshTexNum[    iNum][i] = -1;
		if(m->pTextureFilename)
		{
			short	sTex = d3_tex_load(-1, m[i].pTextureFilename, true);
			if(sTex != -1){ d3.sMeshTexNum[iNum][i] = sTex, d3.texInfo[sTex].usUseCount++;}
		}
	}
	sys_folder_pop();

	// Diffuse が 0 のものを探す
	for(uint i = 0; i < d3.meshInfo[iNum].ucSubset; i++)
	{
		if(d3.matMesh[iNum][i].Diffuse.a == 0){ d3.matMesh[iNum][i].Diffuse.a = 1.0f;}
	}

	// テクスチャー読み込み
	//d3_mesh_tex_load(iNum, (D3DXMATERIAL*)pMatBuf->GetBufferPointer(), n);

	d3_hit_work_get(&d3.hitWork[iNum], d3.lpMesh[iNum]);

	if(!d3.meshInfo[iNum].bResource){ sys_folder_pop();}	// フォルダー位置を元の状態へ戻す
	strcpy(d3.meshInfo[iNum].strFullPath, strPath);			// フルパスをコピー
	d3.meshInfo[iNum].bSkin = false;						// スキンメッシュオフ
	d3.meshInfo[iNum].bLoad = true;							// 読み込みフラグオン

	SAFE_RELEASE(pMatBuf);

	//--- タイムスタンプ登録 ----------------------------------------
	sys_timestamp_get(&d3.meshInfo[iNum].ftLastWrite, d3.meshInfo[iNum].strFullPath);

	return true;
}

bool	d3_xfile_load(int iNum,	char *strFile, bool bLoadAtOnce, bool bSysMem)
{
	return d3_xfile_load(iNum, strFile, bLoadAtOnce, bSysMem, true);
}

bool	d3_xfile_load(int iNum,	char *strFile, bool bLoadAtOnce)
{
	return d3_xfile_load(iNum, strFile, bLoadAtOnce, false, true);
}

bool	d3_xfile_load(int iNum,	char *strFile)
{
	return d3_xfile_load(iNum, strFile, !sys.bAfterLoad, false, true);
}



/*----------------------------------------------------------------------*
| <<< モーションとモーションの中間を取り、新規にメッシュを作成する >>>
|	入力	iIn0Num	= 読み込み番号
|			iIn1Num	= 読み込み番号
|			fS      = 加重係数
|	出力	iOutNum = 中間をとるメッシュ
|	戻り値	true or	false
*----------------------------------------------------------------------*/
bool	d3_mesh_tween(int iOutNum, int iIn0Num, int iIn1Num, float fS)
{
	//--- 未読み込みチェック ----------------------------------------
	if(d3.lpMesh[iIn0Num] == NULL || d3.lpMesh[iIn1Num] == NULL) return false;

	//--- メッシュ情報チェックー ------------------------------------
	if(d3.meshInfo[iIn0Num].ucSubset != d3.meshInfo[iIn1Num].ucSubset
	|| d3.meshInfo[iIn0Num].usFaces  != d3.meshInfo[iIn1Num].usFaces
	|| d3.meshInfo[iIn0Num].usVertex != d3.meshInfo[iIn1Num].usVertex) return false;

	//--- メッシュ情報コピー ----------------------------------------
	d3.meshInfo[iOutNum] = d3.meshInfo[iIn0Num];
	
	//--- マテリアルコピー ------------------------------------------
	d3.matMesh[ iOutNum] = new D3DMATERIAL9[d3.meshInfo[iOutNum].ucSubset];
	uint	i;
	for(i = 0; i < d3.meshInfo[iOutNum].ucSubset; i++)
	{
		d3.matMesh[iOutNum][i] = d3.matMesh[iIn0Num][i];
		if(d3.sMeshTexNum[iIn0Num][i] != -1)
		{
			d3.sMeshTexNum[iOutNum][i] = d3.sMeshTexNum[iIn0Num][i];
			d3.texInfo[d3.sMeshTexNum[iIn0Num][i]].usUseCount++;

			//d3.sMeshTexBumpNum[iOutNum][i] = d3.sMeshTexBumpNum[iIn0Num][i];
			//d3.texInfo[d3.sMeshTexBumpNum[iIn0Num][i]].usUseCount++;

			//d3.sMeshTexFurNum[iOutNum][i] = d3.sMeshTexFurNum[iIn0Num][i];
			//d3.texInfo[d3.sMeshTexFurNum[iIn0Num][i]].usUseCount++;
		}
	}

	//--- メッシュコピー --------------------------------------------
	LPD3DXMESH	p0 = d3.lpMesh[iIn0Num];
	LPD3DXMESH	p1 = d3.lpMesh[iIn1Num];
	p0->CloneMeshFVF(p0->GetOptions(), p0->GetFVF() | D3_FVF_DEFAULT, d3.dev, &d3.lpMesh[iOutNum]);
	LPD3DXMESH	p2 = d3.lpMesh[iOutNum];

	//--- 中間のモーションを作成 ------------------------------------
	D3_VTX_DEFAULT	*pS0, *pS1, *pDst;
		
	p0->LockVertexBuffer(D3DLOCK_READONLY, (void **)&pS0);
	p1->LockVertexBuffer(D3DLOCK_READONLY, (void **)&pS1);
	p2->LockVertexBuffer(D3DLOCK_DISCARD,  (void **)&pDst);

	for(i = 0; i < p0->GetNumVertices(); i++)
	{
		pDst[i].p = pS1[i].p * fS + pS0[i].p * (1 - fS);
	}

	
	p0->UnlockVertexBuffer();
	p1->UnlockVertexBuffer();
	p2->UnlockVertexBuffer();

	//--- バンプも作成 ----------------------------------------------
	/*if(d3.meshInfo[iIn0Num].bBump && d3.meshInfo[iIn1Num].bBump)
	{
		p0 = d3.lpMeshBump[iIn0Num];
		p1 = d3.lpMeshBump[iIn1Num];
		p0->CloneMesh(p0->GetOptions(), d3_decl_bump, d3.dev, &d3.lpMeshBump[iOutNum]);
		p2 = d3.lpMeshBump[iOutNum];

		p0->LockVertexBuffer(D3DLOCK_READONLY, (void **)&pS0);
		p1->LockVertexBuffer(D3DLOCK_READONLY, (void **)&pS1);
		p2->LockVertexBuffer(D3DLOCK_DISCARD,  (void **)&pDst);

		for(i = 0; i < p0->GetNumVertices(); i++) pDst[i].p = pS1[i].p * fS + pS0[i].p * (1 - fS);
	
		p0->UnlockVertexBuffer();
		p1->UnlockVertexBuffer();
		p2->UnlockVertexBuffer();
	}*/
	//--- 終了 ------------------------------------------------------
	return true;
}



#if 0
	/*----------------------------------------------------------------------*
	| <<< 文字列をメッシュ化 >>>
	|	入力	iID		  =	メッシュ格納番号
	|			pstrFont  =	フォント名
	|			size	  =	サイズ
	|			extrusion =	厚み
	|			str		  =	作成する文字（文字列もＯＫ）
	|	戻り値	true or	false
	*----------------------------------------------------------------------*/
	bool	d3_text_create(int iID, TCHAR *pstrFont, int size, float extrusion, char *str)
	{

		HRESULT			hr;
		HFONT			hFont;
		HFONT			hFontOld;
		LOGFONT			lf;
		LPD3DXMESH		pMesh;
		HDC				hdc	= CreateCompatibleDC(NULL);

		d3_xfile_free(iID);					// 前回データを消去

		//--- 文字のメッシュ作成 --------------------------------------
		ZeroMemory(&lf,	sizeof(lf));
		lf.lfHeight			= size;
		lf.lfStrikeOut		= 1;
		lf.lfPitchAndFamily	= FIXED_PITCH;
		lf.lfItalic			= true;
		lf.lfWeight			= (false) *	FW_NORMAL +	(true) * FW_BOLD;
		lstrcpy(lf.lfFaceName, pstrFont);
		hFont				= CreateFontIndirect(&lf);
		hFontOld			= (HFONT)SelectObject(hdc, hFont); 
		hr					= D3DXCreateText(d3.dev, hdc, str, 0.001f, extrusion, &pMesh, NULL,	NULL);
		SelectObject(hdc, hFontOld);
		DeleteObject(hFont);
		DeleteDC(hdc);
		if(FAILED(hr)){ return false;}

		d3_mesh_make(iID, pMesh);				// メッシュ作成
												// テクスチャー読み込み
		//return d3_mesh_tex_load(iID, (D3DXMATERIAL*)NULL, 0);

		return true;
	}
#endif





/*------------------------------------------------------------------------------*
| <<< メッシュとレイの交差判定をして、交点を返す >>>
|	入力	iNum  = メッシュの読み込み番号
|			iMat  = 調べるマテリアル番号(-1なら全てを一度に調べる)
|			pPos  = 座標
|			pDir  = レイの向き
|			fSize = メッシュの大きさ
|			mtx	  = ワールド行列
|	出力	pOutCrossPos  = 交差している座標
|			pOutCrossDist = 交点までの距離
|	戻り値	true  :	交差している
|			false :	交差していない
|
|	※Ｘファイルのフォーマットは D3_VTX_DEFAULT としてください。
*------------------------------------------------------------------------------*/
static	D3_VTX_DEFAULT	vtxTri[3];			// 三角形の座標
static	int				iCrossFaceNum = -1;	// 交差した面番号
											// 交差したメッシュ番号
bool	d3_mesh_intersection_check(VEC3 *pOutCrossPos, float *pOutCrossDist, int iNum, int iMat, VEC3 *pPos, VEC3 *pDir, float fSize, MTX *mtx)
{
	
	if(d3.lpMesh[iNum] == NULL) return false;	// メッシュ未読み込み

	//--- 距離をクリアー --------------------------------------------
	if(pOutCrossDist != NULL)
	{
		*pOutCrossDist = 10000;
	}
	//--- マウスの光線とマップの交差判定 ----------------------------
	BOOL				bHit;
	LPD3DXBUFFER		pBuffer	= NULL;
	DWORD				dwMax;

	iCrossFaceNum				= -1;

	VEC3	vecPos;
	MTX		mtxInv;
	D3DXMatrixInverse(&mtxInv, NULL, mtx);
	D3DXVec3TransformCoord(&vecPos, pPos, &mtxInv);

	if(iMat	!= -1) D3DXIntersectSubset(d3.lpMesh[iNum], iMat, &vecPos, pDir, &bHit, NULL, NULL, NULL, pOutCrossDist, &pBuffer, &dwMax);
	else		   D3DXIntersect(	   d3.lpMesh[iNum],		  &vecPos, pDir, &bHit, NULL, NULL, NULL, pOutCrossDist, &pBuffer, &dwMax);
	if(!bHit) return false;					// レイが交差していなければ帰る
	if(pBuffer == NULL) return false;		// バッファが確保できなくても帰る

	//--- 交差していればどの面が交差しているか調べる ----------------
	D3DXINTERSECTINFO	*p = (D3DXINTERSECTINFO	*)pBuffer->GetBufferPointer();

	DWORD	dwMeshSize = d3.lpMesh[iNum]->GetNumBytesPerVertex();
	//uint	aa = sizeof(D3_VTX_DEFAULT);
	// dwMeshSize が sizeof(D3_VTX_DEFAULT) と同値でなくとも動作するように改良しています

	//---- 距離の近い物からソートする ------------------------------
	if(pOutCrossPos != NULL)
	{
		float	fDistMax = 10000;
		for(int	i =	0; i < (int)dwMax; i++)
		{
			DWORD	dwFace = p[i].FaceIndex;
			float	fBary1 = p[i].U;
			float	fBary2 = p[i].V;
			float	fDist  = p[i].Dist;

			if(fDistMax	< fDist) continue;

			//--- ヒットしている三角形を得る ------------------------
			LPDIRECT3DVERTEXBUFFER9	pVB;
			LPDIRECT3DINDEXBUFFER9	pIB;
			WORD					*pIndices;

			//D3_VTX_DEFAULT			*pVertices;
			uchar					*pVertices;

			d3.lpMesh[iNum]->GetVertexBuffer(&pVB);
			d3.lpMesh[iNum]->GetIndexBuffer( &pIB);
			pIB->Lock(0, 0,	(void**)&pIndices,	0);
			pVB->Lock(0, 0,	(void**)&pVertices,	0);

			WORD	*iThisTri;
			iThisTri = &pIndices[dwFace	* 3];

			vtxTri[0] =	*((D3_VTX_DEFAULT*)&pVertices[iThisTri[0] * dwMeshSize]);	// ここが三角形の面情報となる
			vtxTri[1] =	*((D3_VTX_DEFAULT*)&pVertices[iThisTri[1] * dwMeshSize]);	// 後に三角形を描画するので座標を保存
			vtxTri[2] =	*((D3_VTX_DEFAULT*)&pVertices[iThisTri[2] * dwMeshSize]);

			pVB->Unlock();
			pIB->Unlock();
			pVB->Release();
			pIB->Release();

			//--- 面の表裏判定(裏面も判定する場合は以下をコメントのこと)
			D3DXPLANE	plane;
			D3DXPlaneFromPoints(&plane,	&vtxTri[0].p, &vtxTri[1].p,	&vtxTri[2].p);
			float	a =	D3DXPlaneDotNormal(&plane, pDir);
			if(a >=	0){ continue;}			// プラスの場合は裏面

			//--- 面のレイの交点を得る ------------------------------
											// pOutCrossPos に交点の座標
			D3DXVec3BaryCentric(pOutCrossPos, &vtxTri[0].p,	&vtxTri[1].p, &vtxTri[2].p,	fBary1,	fBary2);
			fDistMax = fDist;

			//--- 面番号をセット ------------------------------------
			iCrossFaceNum = (int)dwFace;
		}
		SAFE_RELEASE(pBuffer);
	}
											// サイズを見て、あたっていなかったら帰る
											// それでも、交差している座標は計算する
	//if(pOutCrossPos  == NULL)				return false;
	if(pOutCrossDist == NULL){				 return true;}
	if(fSize > 0 && *pOutCrossDist > fSize){ return false;}
	return true;
}

bool	d3_mesh_intersection_check(VEC3 *pOutCrossPos, float *pOutCrossDist, int iNum, int iMat, VEC3 *pPos, VEC3 *pDir, float fSize)
{
	return	d3_mesh_intersection_check(pOutCrossPos, pOutCrossDist, iNum, iMat, pPos, pDir, fSize, &d3.mtxIdentity);
}

bool	d3_mesh_intersection_check(float *pOutCrossDist, int iNum, int iMat, VEC3 *pPos, VEC3 *pDir, float fSize)
{
	VEC3	vecOutCrossPos;
	return	d3_mesh_intersection_check(&vecOutCrossPos, pOutCrossDist, iNum, iMat, pPos, pDir, fSize, &d3.mtxIdentity);
}

bool	d3_mesh_intersection_check(int iNum, int iMat, VEC3 *pPos, VEC3 *pDir, float fSize, MTX *mtx)
{
	VEC3	vecOutCrossPos;
	float	fOutCrossDist;
	return d3_mesh_intersection_check(&vecOutCrossPos, &fOutCrossDist, iNum, iMat, pPos, pDir, fSize, mtx);
}

bool	d3_mesh_intersection_check(int iNum, int iMat, VEC3 *pPos, VEC3 *pDir, float fSize)
{
	VEC3	vecOutCrossPos;
	float	fOutCrossDist;
	return d3_mesh_intersection_check(&vecOutCrossPos, &fOutCrossDist, iNum, iMat, pPos, pDir, fSize, &d3.mtxIdentity);
}

bool	d3_mesh_intersection_check(int iNum, int iMat, VEC3 *pPos, VEC3 *pDir)
{
	VEC3	vecOutCrossPos;
	float	fOutCrossDist;
	return	d3_mesh_intersection_check(&vecOutCrossPos, &fOutCrossDist, iNum, iMat, pPos, pDir, -1, &d3.mtxIdentity);
}



/*------------------------------------------------------------------------------*
| <<< d3_mesh_intersection_check で得られた面情報を得る >>>
|	戻り値	交差した面の情報テーブル
*------------------------------------------------------------------------------*/
D3_VTX_DEFAULT	*d3_mesh_intersection_face(void)
{
	return	vtxTri;
}



/*------------------------------------------------------------------------------*
| <<< d3_mesh_intersection_check で交差した面番号を得る >>>
|	戻り値	交差した面の情報テーブル
*------------------------------------------------------------------------------*/
int		d3_mesh_intersection_face_num_get(void)
{
	return	iCrossFaceNum;
}



/*----------------------------------------------------------------------*
| <<< ボックス同士のあたり判定 >>>		※ D3_HIT_VEC_SET で座標を計算しておくこと
|	※ *a を大きいものとするように
*----------------------------------------------------------------------*/
bool	d3_hit_box_check(VEC3 *a, VEC3 *b)
{

	D3DXPLANE	p[6];						// A の各面を作成

	D3DXPlaneFromPoints(&p[0], &a[0], &a[1], &a[3]);	// 上面
	D3DXPlaneFromPoints(&p[1], &a[3], &a[2], &a[6]);	// 奥面
	D3DXPlaneFromPoints(&p[2], &a[4], &a[7], &a[6]);	// 底面
	D3DXPlaneFromPoints(&p[3], &a[0], &a[4], &a[5]);	// 正面
	D3DXPlaneFromPoints(&p[4], &a[0], &a[3], &a[7]);	// 左面
	D3DXPlaneFromPoints(&p[5], &a[1], &a[5], &a[6]);	// 右面

	for(int	j =	0; j < 8; j++)				// B の頂点が１つでも
	{
		bool	f =	true;
		for(int	i =	0;i	< 6; i++)			// A の全ての面の内側にあるか？
		{									// 外側にあったら別の頂点を調べる
			if(D3DXPlaneDotCoord(&p[i],	&b[j]) < 0)
			{
				f =	false;
				break;
			}
		}
		if(f) return true;
	}
	return false;
}



/*------------------------------------------------------------------------------*
| <<< メッシュのあたり判定用ＢＯＸを表示 >>>	※ D3_3DLINE_FLUSH	を呼び出すこと
|	入力	pHit   = あたり判定用ワーク			※ D3_HIT_VEC_SET で座標を計算しておくこと
*------------------------------------------------------------------------------*/
void	d3_hit_box_draw(VEC3 *pp)
{
#ifdef	__HIT_AREA_DRAW
	//--- ボックス描画 --------------------------------------------
	DWORD	c =	D3DCOLOR_ARGB(255, 255,	0, 0);

	D3_3DLINE(&pp[0], &pp[1], c);
	D3_3DLINE(&pp[3], &pp[0], c);
	D3_3DLINE(&pp[3], &pp[2], c);
	D3_3DLINE(&pp[2], &pp[1], c);

	D3_3DLINE(&pp[4], &pp[5], c);
	D3_3DLINE(&pp[7], &pp[4], c);

	D3_3DLINE(&pp[7], &pp[6], c);
	D3_3DLINE(&pp[6], &pp[5], c);

	D3_3DLINE(&pp[4], &pp[0], c);
	D3_3DLINE(&pp[7], &pp[3], c);

	D3_3DLINE(&pp[5], &pp[1], c);
	D3_3DLINE(&pp[6], &pp[2], c);
#endif
/*
	d3_billboard_printf(&pp[0],	0.2f, "0");
	d3_billboard_printf(&pp[1],	0.2f, "1");
	d3_billboard_printf(&pp[2],	0.2f, "2");
	d3_billboard_printf(&pp[3],	0.2f, "3");

	d3_billboard_printf(&pp[4],	0.2f, "4");
	d3_billboard_printf(&pp[5],	0.2f, "5");
	d3_billboard_printf(&pp[6],	0.2f, "6");
	d3_billboard_printf(&pp[7],	0.2f, "7");
*/
}



/*------------------------------------------------------------------------------*
| <<< レイと三角形の交差判定 >>>
|	入力	pos			= 調べたい始点
|			dir			= 終点（レイの方向）
|			v0,	v1,	v2	= 三角形それぞれの頂点
|	出力	*out		= 交点出力用 (NULLなら出力しない)
|	戻り値	true	: 交差した
|			false	: 交差していない
*------------------------------------------------------------------------------*/
bool	d3_hit_intersect(VEC3 *out, VEC3 *pos, VEC3	*dir, VEC3 *v0, VEC3 *v1, VEC3 *v2)
{

	D3DXPLANE	plane;
	VEC3		v;

										// ３個のベクトルより三角形を作成
	D3DXPlaneFromPoints(&plane,	v0,	v1,	v2);
										// 平面と直線の交点を調べる
										// 直線は d3_view_from	と pos を結ぶ
	if(D3DXPlaneIntersectLine(&v, &plane, dir, pos))
	{									// 内外判定
		VEC3	n[3];
		VEC3	vv0, vv1, vv2;
		vv0	= *v0 -	v;
		vv1	= *v1 -	v;
		vv2	= *v2 -	v;
		D3DXVec3Cross(&n[0], &vv0, &vv2);
		D3DXVec3Cross(&n[1], &vv1, &vv0);
		D3DXVec3Cross(&n[2], &vv2, &vv1);
		if(D3DXVec3Dot(&n[0], &n[1]) < 0 ||	D3DXVec3Dot(&n[0], &n[2]) <	0)
		{								// 外側
			return false;
		}
		else
		{								// 内側
			if(*out) *out =	v;			// out が NULL 以外ならば交点を返す
			return true;
		}
	}
	else
	{									// 視線と面が平行
		return false;
	}
}
