
/**************************************************************************************************
	[[[	HLSL マクロ	]]]
**************************************************************************************************/

inline	bool	SHADER_TECH_SET(LPD3DXEFFECT lpEff, char *strTechWord)
{
	if(lpEff == NULL){ return false;}

	D3DXHANDLE	h = lpEff->GetTechniqueByName(strTechWord);
	if(h == NULL){ return false;}
	lpEff->SetTechnique(h);

	d3.eff = lpEff;
	return true;
}

inline	void	SHADER_INT_SET(char *str, int iDat)
{
	#ifdef	_DEBUG
		HRESULT	hr = d3.eff->SetInt(str, iDat);
		if(hr != D3D_OK){ sys_error("SHADER ERROR!:SHADER_INT_SET %s", str);}
	#else
		d3.eff->SetInt(str, iDat);
	#endif
}

inline	void	SHADER_FLOAT_SET(char *str, float fDat)
{
	#ifdef	_DEBUG
		HRESULT	hr = d3.eff->SetFloat(str, fDat);
		if(hr != D3D_OK){ sys_error("SHADER ERROR!:SHADER_FLOAT_SET %s", str);}
	#else
		d3.eff->SetFloat(str, fDat);
	#endif
}

inline	void	SHADER_MATRIX_SET(char *str, MTX *mtx)
{
	#ifdef	_DEBUG
		HRESULT	hr = d3.eff->SetMatrix(str, mtx);
		if(hr != D3D_OK){ sys_error("SHADER ERROR!:SHADER_MATRIX_SET %s", str);}
	#else
		d3.eff->SetMatrix(str, mtx);
	#endif
}

inline	void	SHADER_VEC3_SET(char *str, VEC3 *vec)
{
	#ifdef	_DEBUG
		HRESULT	hr = d3.eff->SetVector(str, (VEC4*)vec);
		if(hr != D3D_OK){ sys_error("SHADER ERROR!:SHADER_VEC3_SET %s", str);}
	#else
		d3.eff->SetVector(str, (VEC4*)vec);
	#endif
}
inline	void	SHADER_VEC4_SET(char *str, VEC4 *vec)
{
	#ifdef	_DEBUG
		HRESULT	hr = d3.eff->SetVector(str, vec);
		if(hr != D3D_OK){ sys_error("SHADER ERROR!:SHADER_VEC4_SET %s", str);}
	#else
		d3.eff->SetVector(str, vec);
	#endif
}

inline	void	SHADER_LIGHT_INVERSE_SET(char *str, MTX *mtxWorld, D3DXBONECOMBINATION *pBC)
{
	#ifndef	__OBBTREE
		if(pBC == NULL)	// ボーン無しの場合
		{
	#endif
			VEC4	l =	VEC4(d3.light.Direction.x, d3.light.Direction.y, d3.light.Direction.z, 0);
			MTX		a;
			D3DXMatrixInverse(&a, NULL,	mtxWorld);
			D3DXVec4Transform(&l, &l,	&a);
			D3DXVec4Normalize(&l, &l);
			d3.eff->SetFloatArray(str, (float*)&l, sizeof(VEC3));
	#ifndef	__OBBTREE
		}
		else			// ボーン有りの場合はライトは回転させない
		{
			VEC3	l(d3.light.Direction.x, d3.light.Direction.y, d3.light.Direction.z);
			D3DXVec3Normalize(&l, &l);
			SHADER_VEC3_SET(str, &l);
		}
	#endif
}


inline	void	SHADER_LIGHT_SET(char *str, MTX *mtxWorld)
{
	VEC4	l =	VEC4(d3.light.Direction.x, d3.light.Direction.y, d3.light.Direction.z, 0);
	MTX		a;
	D3DXVec4Normalize(&l, &l);
	d3.eff->SetFloatArray(str, (float*)&l, sizeof(VEC3));
}

inline	void	SHADER_EYE_INVERSE_SET(char *str, MTX *mtxWorld)
{
	MTX		m =	*mtxWorld * d3.mtxView;
	D3DXMatrixInverse(&m, NULL,	&m);
	VEC4	v(0, 0,	0, 1);
	D3DXVec4Transform(&v, &v, &m);
	d3.eff->SetVector(str, &v);
}

// カメラ座標をセット
inline	void	SHADER_CAMERA_SET(char *str, VEC3 *v)
{
	VEC4	a(v->x, v->y, v->z, 0);
	D3DXVec4Normalize(&a, &a);
	d3.eff->SetVector(str, (VEC4*)&a);
}

// ワールド座標の転置行列をセット
inline	void	SHADER_WORLD_TRANSPOSE_SET(char *str, MTX *mtxWorld)
{
	MTX	m;
	D3DXMatrixTranspose(&m,	mtxWorld);
	d3.eff->SetMatrix(str, &m);
}

// テクスチャーセット
inline	void	SHADER_TEXTURE_SET(char *str, LPDIRECT3DTEXTURE9 lpTex)
{
	d3.eff->SetTexture(str, lpTex);
}

// テクスチャー番号を得る
inline	int		SHADER_TEXTURE_GET(int iXNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMC, D3DXBONECOMBINATION *pBC)
{
	// ボーンありの場合
	if(pMC != NULL)
	{
		#ifndef	__OBBTREE
			int	b = pMC->sTex[pBC == NULL ? iAttrib : pBC[iAttrib].AttribId];
		#else
			int	b;
			if(pMC->pAttributeTable == NULL){ b = pMC->sTex[iAttrib];}
			else							{ b = pMC->sTex[pMC->pAttributeTable[iAttrib].AttribId];}
		#endif

		if(b == -1){ return TEX_WHITE;}
		else	   { return b;}
	}
	// ボーン無しの場合
	else
	{
		int	b = d3.sMeshTexNum[iXNum][iAttrib];
		if(b == -1){ return TEX_WHITE;}
		else	   { return b;}
	}
}

// マテリアルを得る
inline	VEC4	*SHADER_DIFFUSE_GET(int iXNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMC, D3DXBONECOMBINATION *pBC)
{
	// ボーンありの場合
	if(pMC != NULL)
	{
		#ifndef	__OBBTREE
			return	(VEC4*)&pMC->pMaterials[pBC == NULL ? iAttrib : pBC[iAttrib].AttribId].MatD3D.Diffuse;
		#else
			if(pMC->pAttributeTable == NULL) return (VEC4*)&pMC->pMaterials[                     iAttrib          ].MatD3D.Diffuse; 
			else							 return (VEC4*)&pMC->pMaterials[pMC->pAttributeTable[iAttrib].AttribId].MatD3D.Diffuse;
		#endif
	}
	// ボーン無しの場合
	else
	{
		return (VEC4*)&d3.matMesh[iXNum][iAttrib].Diffuse;
	}
}

// 描画
inline	void	SHADER_DRAW(int iXNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMC, D3DXBONECOMBINATION *pBC)
{
	// ボーンありの場合
	if(pMC != NULL)
	{
		#ifndef	__OBBTREE
			pMC->MeshData.pMesh->DrawSubset(iAttrib);
		#else
			if(pMC->pAttributeTable == NULL){ pMC->MeshData.pMesh->DrawSubset(iAttrib);}
			else							{ pMC->MeshData.pMesh->DrawSubset(pMC->pAttributeTable[iAttrib].AttribId);}
		#endif
		//d3.iPolyCount += pMC->MeshData.pMesh->GetNumFaces();	// 頂点数計算
	}
	// ボーン無しの場合
	else
	{
		if(d3.lpMesh[iXNum] == NULL){ return;}
		d3.lpMesh[iXNum]->DrawSubset(iAttrib);
		//d3.iPolyCount += d3.lpMesh[iXNum]->GetNumFaces();		// 頂点数計算(ここで計算してしまうと、ポリ数がアトリビュート倍されてしまう)
	}
}

// コピーを描画
inline	void	SHADER_COPY_DRAW(int iXNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMC, D3DXBONECOMBINATION *pBC)
{
	// ボーンありの場合
	if(pMC != NULL)
	{
		#ifndef	__OBBTREE
			pMC->pMeshCopy->DrawSubset(iAttrib);
		#else
			//pMC->pMeshCopy->DrawSubset(pMC->pAttributeTable[iAttrib].AttribId);
			if(pMC->pAttributeTable == NULL){ pMC->pMeshCopy->DrawSubset(iAttrib);}
			else							{ pMC->pMeshCopy->DrawSubset(pMC->pAttributeTable[iAttrib].AttribId);}
		#endif
		//d3.iPolyCount += pMC->pMeshCopy->GetNumFaces();			// 頂点数計算
	}	
	// ボーン無しの場合
	else
	{
		if(d3.lpMeshCopy[iXNum] == NULL){ return;}
		d3.lpMeshCopy[iXNum]->DrawSubset(iAttrib);
		//d3.iPolyCount += d3.lpMeshCopy[iXNum]->GetNumFaces();	// 頂点数計算
	}
}

// ボーン
inline	void	SHADER_BONE_SET(char *str, D3DXMESHCONTAINER_DERIVED *pMC, D3DXBONECOMBINATION *pBC)
{
	if(pBC == NULL)
	{
		SHADER_INT_SET("g_iNumBones", 4);
	}
	else
	{
		SHADER_INT_SET("g_iNumBones", pMC->NumInfl - 1);
		d3.eff->SetMatrixArray(str, mtxSkinPalette, pMC->m_dwPaletteEntries);
	}
}

inline	VOID	SHADER_PASS_BEGIN(int iNum)
{
	#ifndef	__DX90B
		d3.eff->BeginPass(iNum);
	#else
		d3.eff->Pass(iNum);
	#endif
}

inline	VOID	SHADER_PASS_END(void)
{
	#ifndef	__DX90B
		d3.eff->EndPass();
	#endif
}

inline	uint	SHADER_BEGIN(void)
{
	uint	uiPasses;
	d3.eff->Begin(&uiPasses, D3DXFX_DONOTSAVESTATE);
	return uiPasses;
}

inline	void	SHADER_END(void)
{
	d3.eff->End();
	d3.eff->SetTexture("texDecale", NULL);		// これをやらないとウィンドウ⇔フルスクリーン時に止まります。
	d3.dev->SetVertexShader(NULL);
	d3.dev->SetPixelShader(	NULL);
}

// フォーマットを一部変更
inline	void	SHADER_FORMAT_CHANGE(D3DXMESHCONTAINER_DERIVED *pMC, DWORD dwType)
{
	if(pMC == NULL){ return;}

	D3DVERTEXELEMENT9	pDecl[MAX_FVF_DECL_SIZE];
	LPD3DVERTEXELEMENT9	pDeclCur;
	pMC->MeshData.pMesh->GetDeclaration(pDecl);
	pDeclCur = pDecl;
	while(pDeclCur->Stream != 0xff)
	{
		if((pDeclCur->Usage	== D3DDECLUSAGE_BLENDINDICES) && (pDeclCur->UsageIndex == 0))
		{
			//pDeclCur->Type = D3DDECLTYPE_UBYTE4;
			pDeclCur->Type = (BYTE)dwType;
		}
		pDeclCur++;
	}
	pMC->MeshData.pMesh->UpdateSemantics(pDecl);
}

// メッシュのコピーを作成
inline	void	SHADER_MESH_COPY(int iXNum, D3DXMESHCONTAINER_DERIVED *pMC, D3DVERTEXELEMENT9 *decl, D3DVERTEXELEMENT9 *declBone)
{
	// pMC->pMeshCopy にコピーを作成
	if(pMC == NULL)
	{
		SAFE_RELEASE(d3.lpMeshCopy[ iXNum]);
		d3.lpMesh[iXNum]->CloneMesh(d3.lpMesh[iXNum]->GetOptions(), decl, d3.dev, &d3.lpMeshCopy[ iXNum]);
	}
	else
	{
		SAFE_RELEASE(pMC->pMeshCopy);
		pMC->MeshData.pMesh->CloneMesh(pMC->MeshData.pMesh->GetOptions(), declBone, d3.dev, &pMC->pMeshCopy);
	}
}

// メッシュにタンジェント情報を作成
inline	void	SHADER_COMPUTE_TANGENT(int iXNum, D3DXMESHCONTAINER_DERIVED *pMC)
{
	if(pMC == NULL){ D3DXComputeTangent(d3.lpMeshCopy[iXNum], 0, 0, 0, TRUE, NULL);}
	else		   { D3DXComputeTangent(pMC->pMeshCopy,       0, 0, 0, TRUE, NULL);}
}

// シェーダーのコピーのフォーマットチェック
inline	bool	SHADER_COPY_TYPE_CHECK(int iXNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMC, DWORD dwType)
{
	if(pMC == NULL)
	{
		if(d3.sMeshCopyType[iXNum][iAttrib] == dwType){ return false;}
		else										  { return true;}
	}
	else
	{
		if(pMC->sMeshCopyType == dwType){ return false;}
		else							{ return true;}
	}
}


//--- extern --------------------------------------------------------------------------------------

extern	void	*pShaderTbl[];

// シェーダーの登録最大数を得る
extern	int		shader_max_get(void);

// シェーダー乗っ取り
extern	void	shader_func_set(     int iNum, void *func);

// 00:ノーマル
extern	bool	shader_normal(       int iXNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMC, D3DXBONECOMBINATION *pBC, MTX *mtx, float fAlp);


extern	void	shader_compile(uint uiNum);
extern	bool	shader_compile(LPD3DXEFFECT *lpEff, char *strFile);
extern	bool	shader_legacy_compile(LPD3DXEFFECT *lpEff, char *strFile);
extern	void	shader_init(void);
extern	bool	shader_set(int iType);
extern	void	shader_free(uint iNum);
extern	void	shader_free( void);
extern	void	shader_reset(void);


/**************************************************************************************************
	[[[	頂点シェーダー・ピクセルシェーダー関係 ]]]
**************************************************************************************************/
inline	void	d3_shader_compile(uint uiNum){										 shader_compile(uiNum);}
inline	bool	d3_shader_compile(LPD3DXEFFECT *lpEff, char *strFile){		  return shader_compile(lpEff, strFile);}
inline	bool	d3_shader_legacy_compile(LPD3DXEFFECT *lpEff, char *strFile){ return shader_legacy_compile(lpEff, strFile);}
inline	void	d3_shader_init(void){												 shader_init();}
inline	bool	d3_shader_set(int iType){									  return shader_set(iType);}
inline	void	d3_shader_free(uint iNum){											 shader_free(iNum);}
inline	void	d3_shader_free( void){												 shader_free();}
inline	void	d3_shader_reset(void){												 shader_reset();}

