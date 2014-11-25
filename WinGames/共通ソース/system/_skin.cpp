//
//	スキンメッシュ処理
//
//	※現状で __OBBTREE を使うと、シェーダーが掛からなくなります


#ifdef	__SKIN

// 以下を main.h で定義のこと
//#define	__OBBTREE					// OBBTree を使用する
//#define	__HIT_OBBTREE_BOX			// OBBTree のヒットBOX を描画する


//--- 各種インクルード ----------------------------------------------------------------------------
#ifdef	__OBBTREE
	#include	"ex/_obbtree.cpp"
	#pragma warning(disable:4244 4786 4819 4996)
	using	namespace	std;					// このスコープ内ではstd::が省略できる
	#include	<vector>
#endif

//--- 定数定義 ------------------------------------------------------------------------------------
#define	__3D_DRAW_TEST		0				// 1 : d3_draw	のチェック用
#define	SHADER_BONE_MAX		50				// 頂点シェーダー 1.1 までは 26	本


//--- 変数 ----------------------------------------------------------------------------------------
static	int			iUpdateCount;			// skinproc に渡されます
		int			iSkinFaceCount;			// 総面数
		int			iSkinVertexCount;		// 総頂点数
CMyD3DApplication	*skin[D3_MESH_MAX];

static	bool		bBumpLoadSw;			// true	: バンプ読み込み
static	bool		bFurLoadSw;				// true	: ファー読み込み
static	int			iContainerCount;		// メッシュの処理計測用
											// DrawMeshContainer を通った数を計測します(描画しない時も +1 されます注意)
static	int			iDrawCount;				// メッシュの描画計測用(１フレーム毎にカウントアップしていきます)
											// 割り込み時に使用します
static	void		*pSkinProc;				// 首の回転などで使う関数ポインタ
static	void		*pFrameProc;			// 残像やあたり判定で使う関数ポインタ
											// skin_proc_set で設定のこと
		void		*pShaderProc;			// シェーダー割り込み用
											// 行列パレット用
		MTX			mtxSkinPalette[SHADER_BONE_MAX];

static	int			iDrawMeshCount;			// 描画する数
		int			iSkinBoneCount;			// オリジナルのボーンの数(50本を超えたら５０に丸め込まれる)


//--- 関数宣言 ------------------------------------------------------------------------------------
static	HRESULT	AllocateName(LPCSTR Name, LPSTR *pNewName);



/*----------------------------------------------------------------------*
| <<< 名前のセット >>>
|	入力	Name     = 新しい名前
|			pNewName = 名前の格納位置
|	戻り値	S_OK     = 成功
*----------------------------------------------------------------------*/
static	HRESULT	AllocateName(LPCSTR Name, LPSTR *pNewName)
{
	if(Name	!= NULL)
	{
		uint	uiLength = (uint)strlen(Name) + 1;
		*pNewName		 = new CHAR[uiLength];
		if(*pNewName ==	NULL) return E_OUTOFMEMORY;
		memcpy(*pNewName, Name,	uiLength * sizeof(CHAR));
	}
	else
	{
		*pNewName =	NULL;
	}
	return S_OK;
}


/*----------------------------------------------------------------------*
| <<< フレームの作成 >>>	※この関数は引数の変更不可
*----------------------------------------------------------------------*/
HRESULT	CAllocateHierarchy::CreateFrame(LPCSTR Name, LPD3DXFRAME *ppNewFrame)
{

	HRESULT		hr = S_OK;

	*ppNewFrame					= NULL;
	D3DXFRAME_DERIVED	*pFrame = new D3DXFRAME_DERIVED;
	if(pFrame == NULL){	hr = E_OUTOFMEMORY;	goto e_Exit;}

	AllocateName(Name, &pFrame->Name);

	// メンバーをクリアー
	pFrame->TransformationMatrix         = d3.mtxIdentity;
	pFrame->CombinedTransformationMatrix = d3.mtxIdentity;
	pFrame->pMeshContainer				 = NULL;
	pFrame->pFrameSibling				 = NULL;
	pFrame->pFrameFirstChild			 = NULL;
	*ppNewFrame							 = pFrame;
	pFrame								 = NULL;

e_Exit:
	delete pFrame;
	return hr;
}


/*----------------------------------------------------------------------*
| <<< メッシュの整形 >>>
|	※この関数は D3DXLoadMeshHierarchyFromX	コール時に呼び出される関数です。
|	  内容はメッシュ、テクスチャー、マテリアルの読み込みと FVF の変更です。
|	  なお、引数の変更はできないので注意。
*----------------------------------------------------------------------*/
HRESULT	CAllocateHierarchy::CreateMeshContainer(
	LPCSTR	Name, 
	CONST	D3DXMESHDATA		*pMeshData,
	CONST	D3DXMATERIAL		*pMaterials,	
	CONST	D3DXEFFECTINSTANCE	*pEffectInstances,	
			DWORD				dwMaterialMax,
	CONST	DWORD				*pAdjacency, 
	LPD3DXSKININFO				pSkinInfo, 
	LPD3DXMESHCONTAINER			*ppNewMeshContainer) 
{
	HRESULT						hr;
	UINT						NumFaces;
	D3DXMESHCONTAINER_DERIVED	*pMeshContainer	 = NULL;
	LPD3DXMESH					pMesh			 = NULL;

	*ppNewMeshContainer	= NULL;

	//--- 不正なメッシュの場合 --------------------------------------
	if(pMeshData->Type != D3DXMESHTYPE_MESH){ hr = E_FAIL; goto	e_Exit;}

	//--- メッシュデータ構造からコピー ------------------------------
	pMesh =	pMeshData->pMesh;

	//--- おかしな FVF の場合 ---------------------------------------
	if(pMesh->GetFVF()      == 0){ hr = E_FAIL; goto e_Exit;}

	//--- 面が０の場合 ----------------------------------------------
	if(pMesh->GetNumFaces() == 0){ hr = E_FAIL; goto e_Exit;}

	//--- メモリ確保 ------------------------------------------------
	pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	if(pMeshContainer == NULL){	hr = E_OUTOFMEMORY;	goto e_Exit;}
	memset(pMeshContainer, 0, sizeof(D3DXMESHCONTAINER_DERIVED));

	//--- 名前のセット ----------------------------------------------
	if(FAILED(hr = AllocateName(Name, &pMeshContainer->Name))){ goto e_Exit;}

	//--- このフレームのポリゴン数を得る ----------------------------
	NumFaces = pMesh->GetNumFaces();

	//--- 骨あり ----------------------------------------------------
	// フォーマットを整形する
	if(!(pMesh->GetFVF() & D3DFVF_NORMAL))
	{
		hr = pMesh->CloneMeshFVF(pMesh->GetOptions(), pMesh->GetFVF() |	D3DFVF_NORMAL |	D3DFVF_TEX1, d3.dev, &pMeshContainer->MeshData.pMesh);
		if(FAILED(hr)){ goto e_Exit;}
		pMesh =	pMeshContainer->MeshData.pMesh;
	}
	else
	{
		pMeshContainer->MeshData.pMesh = pMesh;
		pMesh->AddRef();
	}
	pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
	iSkinVertexCount			 +=	pMesh->GetNumVertices();	// 頂点数を得る
	iSkinFaceCount				 +=	pMesh->GetNumFaces();		// 面数を得る

	//--- ヒットワークを得る ----------------------------------------
	d3_hit_work_get(&pMeshContainer->hitWork, pMesh);

	//--- マテリアルとテクスチャーの読み込み ------------------------
	pMeshContainer->NumMaterials = max(1, dwMaterialMax);
	pMeshContainer->pMaterials	 = new D3DXMATERIAL[pMeshContainer->NumMaterials];
	pMeshContainer->pAdjacency	 = new DWORD[       NumFaces * 3];

	if((pMeshContainer->pAdjacency == NULL)	|| (pMeshContainer->pMaterials == NULL)){ hr = E_OUTOFMEMORY; goto e_Exit;}

	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * NumFaces	* 3);

	if(dwMaterialMax > 0)					// テクスチャーを読み込みにいく
	{
		memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL)	* dwMaterialMax);
											// テクスチャー領域を確保		
		pMeshContainer->sTex = new short[dwMaterialMax];

		for(uint uiMaterial = 0; uiMaterial < dwMaterialMax; uiMaterial++)
		{
			pMeshContainer->sTex[uiMaterial] = -1;

			if(pMeshContainer->pMaterials[uiMaterial].pTextureFilename != NULL)
			{								// 通常テクスチャー
				short	sTex = d3_tex_load(-1, pMeshContainer->pMaterials[uiMaterial].pTextureFilename, true);
				if(sTex != -1){ pMeshContainer->sTex[uiMaterial] = sTex; d3.texInfo[sTex].usUseCount++;}
				pMeshContainer->pMaterials[uiMaterial].pTextureFilename = NULL;
			}
		}
	}
	else									// マテリアルがない場合はダミーを作ってあげる
	{
		pMeshContainer->sTex[      0]				   = -1;
		pMeshContainer->pMaterials[0].pTextureFilename = NULL;
		memset(&pMeshContainer->pMaterials[0].MatD3D, 0, sizeof(D3DMATERIAL9));
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Specular  = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
	}

	//--- ボーンなど ----------------------------------------------
	if(pSkinInfo !=	NULL)
	{
		// pSkinInfo と pMesh をセット
		pMeshContainer->pSkinInfo =	pSkinInfo;
		pSkinInfo->AddRef();

		pMeshContainer->pOrigMesh =	pMesh;
		pMesh->AddRef();

		// Bone Offset Matrices 保存用のメモリを確保
		uint	uiBoneMax = pSkinInfo->GetNumBones();
		iSkinBoneCount   += uiBoneMax;		// 外部で表示できるようにボーン数を出す

		pMeshContainer->m_pmtxBoneOffset = new MTX[uiBoneMax];
		if(pMeshContainer->m_pmtxBoneOffset == NULL){ hr = E_OUTOFMEMORY; goto e_Exit;}

		// Bone Offset Matrices 読み込み
		for(uint uiBone = 0; uiBone < uiBoneMax; uiBone++)
		{
			pMeshContainer->m_pmtxBoneOffset[uiBone] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(uiBone));
		}
		// スキンメッシュ作成
		if(FAILED(hr = m_pApp->GenerateSkinnedMesh(pMeshContainer))) goto e_Exit;
	}
#ifdef	__OBBTREE
	//--- ボーンが無い場合 ------------------------------------------
	else
	{
		pMeshContainer->pOBBTree = new COBBTree();
		pMeshContainer->pOBBTree->Create(pMesh, iOBBTreeLevel);
	}
#endif
	*ppNewMeshContainer	= pMeshContainer;
	pMeshContainer		= NULL;

e_Exit:
	if(pMeshContainer != NULL){ DestroyMeshContainer(pMeshContainer);}

	return hr;
}


/*----------------------------------------------------------------------*
| <<< フレームの削除 >>>
|	※この関数は引数の変更不可
*----------------------------------------------------------------------*/
HRESULT	CAllocateHierarchy::DestroyFrame(LPD3DXFRAME pFrame) 
{
	SAFE_DELETE_ARRAY(pFrame->Name);
	SAFE_DELETE(	  pFrame);
	return S_OK; 
}


/*----------------------------------------------------------------------*
| <<< メッシュコンテナーの削除 >>>
|	※確保したメモリはここで開放しましょう。
*----------------------------------------------------------------------*/
HRESULT	CAllocateHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{

	D3DXMESHCONTAINER_DERIVED	*pMeshContainer	= (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

	SAFE_DELETE_ARRAY(pMeshContainer->Name);
	SAFE_DELETE_ARRAY(pMeshContainer->pAdjacency);
	SAFE_DELETE_ARRAY(pMeshContainer->pMaterials);
	SAFE_DELETE_ARRAY(pMeshContainer->m_pmtxBoneOffset);
	SAFE_DELETE_ARRAY(pMeshContainer->ppBoneMatrixPtrs);

	// テスクチャー開放
	for(uint i = 0; i < pMeshContainer->NumMaterials; i++)
	{
		//d3_mesh_tex_free(pMeshContainer->sTex[i]);
		int	a = pMeshContainer->sTex[i];
		d3.texInfo[a].usUseCount--;
		if((short)d3.texInfo[a].usUseCount <= 0)
		{
			SAFE_RELEASE(d3.tex[a]);
			d3.texInfo[a].usUseCount     = 0;
			d3.texInfo[a].strFullPath[0] = NULL;
		}
	}

	SAFE_DELETE_ARRAY(pMeshContainer->sTex    );

	SAFE_RELEASE(	  pMeshContainer->pBoneCombinationBuf);
	SAFE_RELEASE(	  pMeshContainer->MeshData.pMesh);
	SAFE_RELEASE(	  pMeshContainer->pSkinInfo);
	SAFE_RELEASE(	  pMeshContainer->pOrigMesh);
#ifdef	__OBBTREE
	SAFE_DELETE(      pMeshContainer->pOBBTree);
#endif
	SAFE_DELETE(	  pMeshContainer);

	return S_OK;
}



/*----------------------------------------------------------------------*
| <<< ヒットボックスを得る >>>
|	入力	iXNum  = X File 番号
|			pFrame = ルートフレーム
|	※元のメッシュより大きさを得るので正確な大きさは得られない。
|	  例えば、人間のメッシュの場合は手を広げた状態の大きさが入る
|	  また、複数のフレームが有る場合は半径の大きなものを優先する
*----------------------------------------------------------------------*/
void	CMyD3DApplication::HitWorkGet(int iXNum, LPD3DXFRAME pFrame)
{

	if(pFrame == NULL){ return;}

	LPD3DXMESHCONTAINER		pMeshContainer = pFrame->pMeshContainer;

	while(pMeshContainer !=	NULL)
	{
		//--- ヒットワークを得る ------------------------------------
		if(pMeshContainer->MeshData.pMesh != NULL)
		{
			// あらかじめ得ているヒットボックスより、ヒットＢＯＸ、ヒット球を得る
			D3DXMESHCONTAINER_DERIVED	*p = (D3DXMESHCONTAINER_DERIVED*)pMeshContainer;
			D3_HIT						*h = &d3.hitWork[iXNum];
			//--- BOX -----------------------------------------------
			if(m_ucHitCount == 0)			// ヒットボックスは初回のみ無条件に値を設定する
			{
				h->vecMin = p->hitWork.vecMin;
				h->vecMax = p->hitWork.vecMax;
			}
			else							// 二度目よりは比較する
			{
				if(h->vecMin.x > p->hitWork.vecMin.x){ h->vecMin.x = p->hitWork.vecMin.x;}
				if(h->vecMin.y > p->hitWork.vecMin.y){ h->vecMin.y = p->hitWork.vecMin.y;}
				if(h->vecMin.z > p->hitWork.vecMin.z){ h->vecMin.z = p->hitWork.vecMin.z;}
				if(h->vecMax.x < p->hitWork.vecMax.x){ h->vecMax.x = p->hitWork.vecMax.x;}
				if(h->vecMax.y < p->hitWork.vecMax.y){ h->vecMax.y = p->hitWork.vecMax.y;}
				if(h->vecMax.z < p->hitWork.vecMax.z){ h->vecMax.z = p->hitWork.vecMax.z;}
			}
			//--- Sphere --------------------------------------------
			// 半径の大きなものを採用する
			if(h->fRadius < p->hitWork.fRadius){ h->fRadius = p->hitWork.fRadius;}

			//--- ヒット球の中心座標を求める ------------------------
			h->vecCenter += p->hitWork.vecCenter;
			m_ucHitCount++;
		}
		pMeshContainer = pMeshContainer->pNextMeshContainer;
	}
	if(pFrame->pFrameSibling	!= NULL){ HitWorkGet(iXNum, pFrame->pFrameSibling   );}
	if(pFrame->pFrameFirstChild	!= NULL){ HitWorkGet(iXNum, pFrame->pFrameFirstChild);}
}



/*----------------------------------------------------------------------*
| <<< メッシュの FVF 変更時によびだす >>>
*----------------------------------------------------------------------*/
HRESULT	CMyD3DApplication::GenerateSkinnedMesh(D3DXMESHCONTAINER_DERIVED *pMeshContainer)
{

	HRESULT	hr = S_OK;

	if(pMeshContainer->pSkinInfo ==	NULL){ return hr;}

	SAFE_RELEASE(pMeshContainer->MeshData.pMesh);
	SAFE_RELEASE(pMeshContainer->pMeshCopy);
	SAFE_RELEASE(pMeshContainer->pBoneCombinationBuf);
#if 0
	hr = pMeshContainer->pOrigMesh->CloneMeshFVF(D3_FVF_DEFAULT, pMeshContainer->pOrigMesh->GetFVF(), d3.dev, &pMeshContainer->MeshData.pMesh);
											// メッシュの属性テーブル、またはメッシュの属性テーブルに格納されているエントリの数を取得
	hr = pMeshContainer->MeshData.pMesh->GetAttributeTable(NULL, &pMeshContainer->NumAttributeGroups);
	if(FAILED(hr)){ goto e_Exit;}

	SAFE_DELETE(pMeshContainer->pAttributeTable);
	pMeshContainer->pAttributeTable = new D3DXATTRIBUTERANGE[pMeshContainer->NumAttributeGroups];
	if(!pMeshContainer->pAttributeTable){ hr = E_OUTOFMEMORY; goto e_Exit;}

	hr = pMeshContainer->MeshData.pMesh->GetAttributeTable(pMeshContainer->pAttributeTable, NULL);
	if(FAILED(hr)){ goto e_Exit;}
#endif

#ifndef	__OBBTREE
	//--- インデックスパレットのサイズを得る ----------------------
	{
		UINT	MaxMatrices	= SHADER_BONE_MAX;	// SHADER_BONE_MAX を超えないように注意すること。
		pMeshContainer->m_dwPaletteEntries = min(MaxMatrices, pMeshContainer->pSkinInfo->GetNumBones());

		DWORD	Flags =	D3DXMESHOPT_VERTEXCACHE;
		if(d3.caps.VertexShaderVersion >= D3DVS_VERSION(1, 1)){	Flags |= D3DXMESH_MANAGED;}
		else												  {	Flags |= D3DXMESH_SYSTEMMEM;}

		//--- 重み係数をミックスした新しいメッシュの作成 --------------
		hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh(pMeshContainer->pOrigMesh,
																	Flags,
																	pMeshContainer->m_dwPaletteEntries,
																	pMeshContainer->pAdjacency,
																	NULL, NULL,	NULL,	  
																	&pMeshContainer->NumInfl,
																	&pMeshContainer->NumAttributeGroups,
																	&pMeshContainer->pBoneCombinationBuf,
																	&pMeshContainer->MeshData.pMesh);
		if(FAILED(hr)){ goto e_Exit;}

		//--- SHADER1 用に FVF 変更	-----------------------------------
		DWORD	NewFVF = (pMeshContainer->MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;

		LPD3DXMESH	pMesh;
		pMeshContainer->MeshData.pMesh->CloneMeshFVF(pMeshContainer->MeshData.pMesh->GetOptions(), NewFVF, d3.dev, &pMesh);
		pMeshContainer->MeshData.pMesh->Release();
		pMeshContainer->MeshData.pMesh = pMesh;

		//--- フォーマットの整形 --------------------------------------
		D3DVERTEXELEMENT9	pDecl[MAX_FVF_DECL_SIZE];
		LPD3DVERTEXELEMENT9	pDeclCur;
		if(FAILED(hr = pMeshContainer->MeshData.pMesh->GetDeclaration(pDecl))){ goto e_Exit;}
		pDeclCur = pDecl;
		while(pDeclCur->Stream != 0xff)
		{
			if((pDeclCur->Usage	== D3DDECLUSAGE_BLENDINDICES) && (pDeclCur->UsageIndex == 0)){ pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;}
			pDeclCur++;
		}
		if(FAILED(hr = pMeshContainer->MeshData.pMesh->UpdateSemantics(pDecl))){ goto e_Exit;}
	}
#endif
	//--- ボーンのバッファ確保 ------------------------------------
	if(m_uiBoneMatricesMax < pMeshContainer->pSkinInfo->GetNumBones())
	{
		m_uiBoneMatricesMax = pMeshContainer->pSkinInfo->GetNumBones();
		SAFE_DELETE(m_pmtxBone);
		m_pmtxBone = new MTX[m_uiBoneMatricesMax];
		if(m_pmtxBone == NULL){ hr = E_OUTOFMEMORY; goto e_Exit;}
	}

e_Exit:
	return hr;
}



/*----------------------------------------------------------------------*
| <<< アニメーションタイムを設定 >>>
|	入力	fDat = コマ数
*----------------------------------------------------------------------*/
HRESULT	CMyD3DApplication::FrameMove(float fDat)
{

	if(m_anim.size() <= (uint)m_iID){ return S_FALSE;}

	SKIN_ANIM_WORK	*a = m_anim[m_iID];
	if(a->m_pController != NULL)
	{
		a->m_dTime += (double)fDat / 60.0;
		a->m_pController->AdvanceTime((double)fDat / 60.0, NULL);
	}
	return S_OK;
}



/*----------------------------------------------------------------------*
| <<< メッシュの描画 >>>
*----------------------------------------------------------------------*/
#ifdef	__OBBTREE
void	CMyD3DApplication::DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainer, LPD3DXFRAME pD3DXFrame)
{

	bool						bDraw       = false;
	D3DXMESHCONTAINER_DERIVED	*pCD3DXMesh = (D3DXMESHCONTAINER_DERIVED*)pMeshContainer;

	//--- スキンメッシュの場合 ------------------------------------------------
	if(pCD3DXMesh->pSkinInfo)
	{
		MTX		pBoneMatrices[SHADER_BONE_MAX];
		DWORD	NumBones = pCD3DXMesh->pSkinInfo->GetNumBones();
		if(NumBones > SHADER_BONE_MAX){ NumBones = SHADER_BONE_MAX;}
		DWORD	i;

		//--- 行列パレットの作成 ------------------------------------
		// この時、ワールド行列だけ削除すること
		MTX		*m = &((D3DXFRAME_DERIVED *)pD3DXFrame)->CombinedTransformationMatrix;
		MTX		mtxInv;
		D3DXMatrixInverse(&mtxInv, NULL, m);
		for(i = 0; i < NumBones; i++)
		{
			MTX		*m0 = &pCD3DXMesh->m_pmtxBoneOffset[i];
			MTX		*m1 = pCD3DXMesh->ppBoneMatrixPtrs[ i];
			pBoneMatrices[i] = *m0 * *m1 * mtxInv;
		}

		//--- 頂点バッファをロックして頂点をトランスフォーム --------
		// この場合、フォーマットを変更したバンプなどには対応不可になります。

		PBYTE		pVerticesSrc;
		PBYTE		pVerticesDest;
		pCD3DXMesh->pOrigMesh->LockVertexBuffer(     D3DLOCK_READONLY, (LPVOID*)&pVerticesSrc );
		pCD3DXMesh->MeshData.pMesh->LockVertexBuffer(0,				   (LPVOID*)&pVerticesDest);

		// 頂点移動(計算は CPU なので遅い)
		pCD3DXMesh->pSkinInfo->UpdateSkinnedMesh(pBoneMatrices, NULL, pVerticesSrc, pVerticesDest);

		// 頂点バッファのロックを解除
		pCD3DXMesh->pOrigMesh->UnlockVertexBuffer(     );
		pCD3DXMesh->MeshData.pMesh->UnlockVertexBuffer();

		//--- スキンメッシュ描画 ----------------------------------------------
		d3.eff = d3.lpEffect[m_iShaderType];

		for(i = 0; i < pCD3DXMesh->NumAttributeGroups; i++)
		{
			// シェーダー別に描画
			bool	(*a)(int iXNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMeshContainer, LPD3DXBONECOMBINATION pBoneComb, MTX *mtx, float fAlp);
			a = (bool(*)(int iXNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMeshContainer, LPD3DXBONECOMBINATION pBoneComb, MTX *mtx, float fAlp))pShaderProc;
			if(a != NULL) (*a)(m_iXNum, i, pCD3DXMesh, NULL, m, m_fAlp);
		}

		// ルートフレームの行列を保存
		if(iDrawMeshCount == 0) m_vecRoot = VEC3(pBoneMatrices[0]._41, pBoneMatrices[0]._42, pBoneMatrices[0]._43);
	}
	//--- 通常のメッシュの場合 ------------------------------------------------
	else
	{
		MTX	*m = &((D3DXFRAME_DERIVED *)pD3DXFrame)->CombinedTransformationMatrix;
		//d3.dev->SetTransform(D3DTS_WORLD, m);

		//--- メッシュ描画 ------------------------------------------
		d3.eff = d3.lpEffect[m_iShaderType];
		for(DWORD i = 0; i < pCD3DXMesh->NumMaterials; i++)
		{
			// シェーダー別に描画
			bool	(*a)(int iXNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMeshContainer, LPD3DXBONECOMBINATION pBoneComb, MTX *mtx);
			a = (bool(*)(int iXNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMeshContainer, LPD3DXBONECOMBINATION pBoneComb, MTX *mtx))pShaderProc;
			if(a != NULL) (*a)(m_iXNum, i, pCD3DXMesh, NULL, m);
		}
		// ルートフレームの行列を保存
		if(iDrawMeshCount == 0)
		{
			m_vecRoot = pCD3DXMesh->hitWork.vecCenter + VEC3(m->_41, m->_42, m->_43);
			m_mtxRoot = *m;
		}
	}

	//--- 描画時にカウントアップ ------------------------------------
	if(bDraw){ iDrawCount++;}

	//--- DrawMeshContainer を処理した数 ----------------------------
	iContainerCount++;
}



/*----------------------------------------------------------------------*
| <<< OBBTree の描画 >>>
*----------------------------------------------------------------------*/
void	CMyD3DApplication::OBBDrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainer, LPD3DXFRAME pD3DXFrame)
{

	D3DXMESHCONTAINER_DERIVED	*pCD3DXMesh = (D3DXMESHCONTAINER_DERIVED*)pMeshContainer;

	//--- スキンメッシュの場合 ------------------------------------------------
	if(pCD3DXMesh->pSkinInfo)
	{
	}
	//--- 通常のメッシュの場合 --------------------------------------
	else
	{
		MTX	*m = &((D3DXFRAME_DERIVED *)pD3DXFrame)->CombinedTransformationMatrix;
		d3.dev->SetTransform(D3DTS_WORLD, m);
	}

	//--- added for OBBTree -----------------------------------------
	if(pCD3DXMesh->pOBBTree)
	{
		pCD3DXMesh->pOBBTree->vecHit.clear();
	#ifdef	__HIT_OBBTREE_BOX
		//if(m_bOBBBox) pCD3DXMesh->pOBBTree->Draw(iOBBTreeLevel);
	#endif
		pCD3DXMesh->pOBBTree->HitBoxGet(iOBBTreeLevel, m_iOBBBoxNum, m_bOBBBox, &m_iOBBBoxMax);
	}
}



#else


void	CMyD3DApplication::DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase)
{

	D3DXMESHCONTAINER_DERIVED	*pMeshContainer	= (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
	D3DXFRAME_DERIVED			*pFrame			= (D3DXFRAME_DERIVED		*)pFrameBase;
	UINT						iAttrib;
	LPD3DXBONECOMBINATION		pBoneComb;

	d3.eff = d3.lpEffect[m_iShaderType];

	//--- ボーンあり ------------------------------------------------------------------------------
	bool	bDraw = false;					// 一度でも描画したら 1 が入る
	if(pMeshContainer->pSkinInfo !=	NULL)
	{
		if(pMeshContainer->pBoneCombinationBuf == NULL){ return;}
		pBoneComb =	reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());

		for(iAttrib	= 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
		{
			//--- 行列パレットの計算 ----------------------------
			for(UINT iPaletteEntry = 0;	iPaletteEntry <	pMeshContainer->m_dwPaletteEntries; iPaletteEntry++)
			{
				UINT	uiMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
				if(uiMatrixIndex != UINT_MAX)
				{
					MTX		m3 =  pMeshContainer->m_pmtxBoneOffset[uiMatrixIndex];
					MTX		m4 = *pMeshContainer->ppBoneMatrixPtrs[uiMatrixIndex];
					mtxSkinPalette[iPaletteEntry] = m3 * m4;//* d3.mtxView;
				}
			}
			//--- ここで描画 ------------------------------------
			bool	(*a)(int iXNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMeshContainer, LPD3DXBONECOMBINATION pBoneComb, MTX *mtx, float fAlp);
			a = (bool(*)(int iXNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMeshContainer, LPD3DXBONECOMBINATION pBoneComb, MTX *mtx, float fAlp))pShaderProc;

			if(a != NULL){ bDraw  = (*a)(m_iXNum, iAttrib, pMeshContainer, pBoneComb, &pFrame->CombinedTransformationMatrix, m_fAlp);}
		}
		//--- ルートフレームの行列を保存 ----------------------------
		if(iDrawCount == 0)
		{
			MTX	*m = &pFrame->CombinedTransformationMatrix;
			m_vecRoot = VEC3(m->_41, m->_42, m->_43);
			m_mtxRoot = *m;
		}
	}
	//--- ボーンなし ------------------------------------------------------------------------------
	else
	{
		for(iAttrib = 0; iAttrib < pMeshContainer->NumMaterials; iAttrib++)
		{
			//--- ここで描画 ----------------------------------------
			bool	(*a)(int iXNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMeshContainer, LPD3DXBONECOMBINATION pBoneComb, MTX *mtx, float fAlp);
			a = (bool(*)(int iXNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMeshContainer, LPD3DXBONECOMBINATION pBoneComb, MTX *mtx, float fAlp))pShaderProc;
			if(a != NULL) bDraw = (*a)(m_iXNum, iAttrib, pMeshContainer, NULL, &pFrame->CombinedTransformationMatrix, m_fAlp);
		}
		//--- ルートフレームの行列を保存 ----------------------------
		MTX	*m = &pFrame->CombinedTransformationMatrix;
		if(iDrawCount == 0)
		{
			m_vecRoot = pMeshContainer->hitWork.vecCenter + VEC3(m->_41, m->_42, m->_43);
			m_mtxRoot = *m;
		}
	}

	d3.dev->SetVertexShader(NULL);
	d3.dev->SetPixelShader(	NULL);

	//--- 割り込み --------------------------------------------------
	// 描画後に割り込まれるので、残像などの処理に使用してください。
	if(bDraw && pFrameProc != NULL)
	{
		void(*func    )(int iMeshNum, int iNum, D3DXMESHCONTAINER_DERIVED *pMeshContainer, LPD3DXFRAME pFrameBase);
		func = (void(*)(int iMeshNum, int iNum, D3DXMESHCONTAINER_DERIVED *pMeshContainer, LPD3DXFRAME pFrameBase))pFrameProc;
		(*func)(m_iXNum, iDrawCount, pMeshContainer, pFrameBase);
	}
	//--- 描画時にカウントアップ ------------------------------------
	if(bDraw){ iDrawCount++;}

	//--- DrawMeshContainer を処理した数 ----------------------------
	iContainerCount++;
}
#endif



/*----------------------------------------------------------------------*
| <<< 階層構造になっているメッシュを描画 >>>
|	入力	pFrame = フレーム
*----------------------------------------------------------------------*/
void	CMyD3DApplication::DrawFrame(LPD3DXFRAME pFrame)
{

	LPD3DXMESHCONTAINER	pMeshContainer = pFrame->pMeshContainer;

	while(pMeshContainer !=	NULL)
	{
		DrawMeshContainer(pMeshContainer, pFrame);
		pMeshContainer = pMeshContainer->pNextMeshContainer;
	}
	if(pFrame->pFrameSibling	!= NULL){ DrawFrame(pFrame->pFrameSibling);}
	if(pFrame->pFrameFirstChild	!= NULL){ DrawFrame(pFrame->pFrameFirstChild);}
}
void	CMyD3DApplication::DrawFrame(LPD3DXFRAME pFrame, int iDrawParts, int iParts)
{

	LPD3DXMESHCONTAINER	pMeshContainer = pFrame->pMeshContainer;
	while(pMeshContainer !=	NULL)
	{
		if(iDrawParts == iParts)
		{
			DrawMeshContainer(pMeshContainer, pFrame);
		}
		pMeshContainer = pMeshContainer->pNextMeshContainer;
		iDrawParts++;
		iParts++;
	}
	if(pFrame->pFrameSibling	!= NULL){ DrawFrame(pFrame->pFrameSibling   , iDrawParts, iParts);}
	if(pFrame->pFrameFirstChild	!= NULL){ DrawFrame(pFrame->pFrameFirstChild, iDrawParts, iParts);}
}



#ifdef	__OBBTREE
/*----------------------------------------------------------------------*
| <<< 階層構造になっている OBB を描画 >>>
|	入力	pFrame = フレーム
*----------------------------------------------------------------------*/
void	CMyD3DApplication::OBBDrawFrame(LPD3DXFRAME pFrame)
{

	LPD3DXMESHCONTAINER	pMeshContainer = pFrame->pMeshContainer;

	while(pMeshContainer !=	NULL)
	{
		OBBDrawMeshContainer(pMeshContainer, pFrame);
		pMeshContainer = pMeshContainer->pNextMeshContainer;
	}
	if(pFrame->pFrameSibling	!= NULL) OBBDrawFrame(pFrame->pFrameSibling);
	if(pFrame->pFrameFirstChild	!= NULL) OBBDrawFrame(pFrame->pFrameFirstChild);
}
#endif



/*----------------------------------------------------------------------*
| <<< 階層構造になっているメッシュコンテナーの名前を得る >>>
|	入力	pFrame    = フレーム(一番最初はルートフレーム)
|			iStrSize  = 名前の最大文字数
|			iCount    = 階層のカウント
|	出力	strOutBuf = 名前が iStrSize 毎に格納
|	※バッファは iStrSize * 256 ぐらいは用意しておくこと
*----------------------------------------------------------------------*/
void	CMyD3DApplication::SubsetNameGet(LPD3DXFRAME pFrame, char *strOutBuf, int iStrSize, int iCount)
{

	LPD3DXMESHCONTAINER	pMeshContainer = pFrame->pMeshContainer;

	while(pMeshContainer !=	NULL)
	{										// バッファに文字をコピーする
		if(pMeshContainer->Name != NULL && pMeshContainer->MeshData.Type == D3DXMESHTYPE_MESH)
		{
			char	*str = pMeshContainer->Name;
			int		b  = strlen(str);
			if(b > iStrSize){ b = iStrSize;}
			strncpy(&strOutBuf[iStrSize * iCount], str, iStrSize);
			iCount++;
		}
		pMeshContainer = pMeshContainer->pNextMeshContainer;
	}
	if(pFrame->pFrameSibling	!= NULL){ SubsetNameGet(pFrame->pFrameSibling,    strOutBuf, iStrSize, iCount);}
	if(pFrame->pFrameFirstChild	!= NULL){ SubsetNameGet(pFrame->pFrameFirstChild, strOutBuf, iStrSize, iCount);}
}



/*----------------------------------------------------------------------*
| <<< ボーン行列をセット >>>
|	入力	pMeshContainerBase = メッシュのコンテナー
*----------------------------------------------------------------------*/
HRESULT		CMyD3DApplication::SetupBoneMatrixPointersOnMesh(LPD3DXMESHCONTAINER pMeshContainerBase)
{

	D3DXFRAME_DERIVED			*pFrame;
	D3DXMESHCONTAINER_DERIVED	*pMeshContainer	= (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

	if(pMeshContainer->pSkinInfo !=	NULL)
	{
		uint	uiBoneMax				 = pMeshContainer->pSkinInfo->GetNumBones();
		pMeshContainer->ppBoneMatrixPtrs = new MTX*[uiBoneMax];
		if(pMeshContainer->ppBoneMatrixPtrs	== NULL) return	E_OUTOFMEMORY;

		for(uint uiBone = 0; uiBone < uiBoneMax; uiBone++)
		{
			pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pFrameRoot, pMeshContainer->pSkinInfo->GetBoneName(uiBone));
			
			if(pFrame == NULL) return E_FAIL;
			pMeshContainer->ppBoneMatrixPtrs[uiBone] = &pFrame->CombinedTransformationMatrix;
		}
	}
	return S_OK;
}



/*----------------------------------------------------------------------*
| <<< ボーンのセットアップ >>>
|	入力	pFrame = フレーム
*----------------------------------------------------------------------*/
HRESULT		CMyD3DApplication::SetupBoneMatrixPointers(LPD3DXFRAME pFrame)
{

	HRESULT		hr;

	if(pFrame->pMeshContainer != NULL)
	{
		if(FAILED(hr = SetupBoneMatrixPointersOnMesh(pFrame->pMeshContainer))) return hr;
	}

	if(pFrame->pFrameSibling !=	NULL)
	{
		if(FAILED(hr = SetupBoneMatrixPointers(pFrame->pFrameSibling)))	return hr;
	}

	if(pFrame->pFrameFirstChild	!= NULL)
	{
		if(FAILED(hr = SetupBoneMatrixPointers(pFrame->pFrameFirstChild))) return hr;
	}
	return S_OK;
}



/*----------------------------------------------------------------------*
| <<< フレームの更新 >>>
|	※入れ子になっている場合は再起呼び出しされるので注意
*----------------------------------------------------------------------*/
void	CMyD3DApplication::UpdateFrameMatrices(LPD3DXFRAME pFrameBase, LPD3DXMATRIX	pParentMatrix)
{
	// ボーンは以下の処理で取得可能です。
	//  
	// ボーンの始点(p0)と終点(p1)を得る
	//	MTX	mtxStart, mtxEnd;
	//	if(pFrame->pFrameFirstChild != NULL){ mtxEnd = pFrame->pFrameFirstChild->TransformationMatrix * pFrame->CombinedTransformationMatrix;}
	//	else								{ mtxEnd = pFrame->TransformationMatrix				      * pFrame->CombinedTransformationMatrix;}
	//	mtxStart = pFrame->CombinedTransformationMatrix;
	//	VEC3	*p0 = (VEC3*)&mtxStart._41;
	//	VEC3	*p1 = (VEC3*)&mtxEnd._41;


	D3DXFRAME_DERIVED	*pFrame	= (D3DXFRAME_DERIVED*)pFrameBase;

	if(pParentMatrix !=	NULL)
	{
		D3DXMatrixMultiply(&pFrame->CombinedTransformationMatrix, &pFrame->TransformationMatrix, pParentMatrix);

		//--- 割り込み ------------------------------------------
		// ここは首の回転などに使用しましょう
		if(pSkinProc != NULL)
		{
			void(*func    )(int iMeshNum, int iNum, LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix);
			func = (void(*)(int iMeshNum, int iNum, LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix))pSkinProc;
			(*func)(m_iXNum, iUpdateCount, pFrameBase, pParentMatrix);
			iUpdateCount++;
		}
	}
	else
	{
		pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;
	}

	// 兄弟の場合
	if(pFrame->pFrameSibling != NULL)
	{
		UpdateFrameMatrices(pFrame->pFrameSibling, pParentMatrix);
	}
	// 子供の場合
	if(pFrame->pFrameFirstChild	!= NULL)
	{
		UpdateFrameMatrices(pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix);
	}
}



#ifdef	__OBBTREE
/*----------------------------------------------------------------------*
| <<< ?? >>>
*----------------------------------------------------------------------*/
HRESULT		CMyD3DApplication::AddMeshtoOneSkinModel(D3DXFRAME *pD3DXFrame)
{
	
	D3DXMESHCONTAINER	*pD3DXMeshContainer;
	D3DXMESHCONTAINER	*pD3DXMeshContainerPrev = NULL;

	pD3DXMeshContainer = pD3DXFrame->pMeshContainer;

	while(pD3DXMeshContainer)
	{
		pD3DXMeshContainerPrev = pD3DXMeshContainer;
		pD3DXMeshContainer     = pD3DXMeshContainer->pNextMeshContainer;
	}

	D3DXMESHCONTAINER_DERIVED	*pCD3DXMesh = new D3DXMESHCONTAINER_DERIVED;
	if(!pCD3DXMesh) return E_OUTOFMEMORY;

	char	strBuf[512];
	sprintf(strBuf, "BoneOBB_%s", pD3DXFrame->Name);
	//pCD3DXMesh->SetName(strBuf);				/* 必要はないが、何か使えるかも*/
	AllocateName(strBuf, &pCD3DXMesh->Name);


	pCD3DXMesh->pNextMeshContainer = NULL;
	if(pD3DXMeshContainerPrev) pD3DXMeshContainerPrev->pNextMeshContainer = pCD3DXMesh;
	else					   pD3DXFrame->pMeshContainer				  = pCD3DXMesh;

	if(pD3DXFrame->pFrameSibling   ) AddMeshtoOneSkinModel(pD3DXFrame->pFrameSibling   );
	if(pD3DXFrame->pFrameFirstChild) AddMeshtoOneSkinModel(pD3DXFrame->pFrameFirstChild);

	return S_OK;
}



/*----------------------------------------------------------------------*
| <<< ?? >>>
|	※OBBTree に必要な関数
*----------------------------------------------------------------------*/
void	CMyD3DApplication::GetSkinMesh(LPD3DXFRAME pD3DXFrame, LPD3DXSKININFO *pSkinInfo, LPD3DXMESHCONTAINER *pD3DXMeshContainer)
{
	
	LPD3DXMESHCONTAINER	pD3DXMeshContainerTmp = pD3DXFrame->pMeshContainer;

	while(pD3DXMeshContainerTmp)
	{
		if(pD3DXMeshContainerTmp->pSkinInfo)
		{
			*pSkinInfo			= pD3DXMeshContainerTmp->pSkinInfo;
			*pD3DXMeshContainer = pD3DXMeshContainerTmp;
		}
		pD3DXMeshContainerTmp = pD3DXMeshContainerTmp->pNextMeshContainer;
	}

	if(pD3DXFrame->pFrameSibling					) GetSkinMesh(pD3DXFrame->pFrameSibling,    pSkinInfo, pD3DXMeshContainer);
	if(pD3DXFrame->pFrameFirstChild && !(*pSkinInfo)) GetSkinMesh(pD3DXFrame->pFrameFirstChild, pSkinInfo, pD3DXMeshContainer);
}



/*----------------------------------------------------------------------*
| <<< OBBTree にフレームを追加する >>>
|	入力		pFrame      = フレーム
|	出力		FrameVector = ここにフレーム情報が追加されます
*----------------------------------------------------------------------*/
void	CMyD3DApplication::GetFrame(D3DXFRAME *pFrame, std::vector< D3DXFRAME *> &FrameVector)
{

	LPD3DXMESHCONTAINER	pMeshContainer = pFrame->pMeshContainer;

	while(pMeshContainer !=	NULL)
	{
		FrameVector.push_back(pFrame);
		pMeshContainer = pMeshContainer->pNextMeshContainer;
	}
	if(pFrame->pFrameSibling)    GetFrame(pFrame->pFrameSibling,    FrameVector);
	if(pFrame->pFrameFirstChild) GetFrame(pFrame->pFrameFirstChild, FrameVector);
}
#endif



/*----------------------------------------------------------------------*
| <<< ここで読み込む >>>
|	入力	strFile =  読み込みＸファイル名
|	戻り値	S_OK    == 読み込み成功
|			S_OK    != 読み込み失敗
*----------------------------------------------------------------------*/
HRESULT		CMyD3DApplication::SkinLoad(char *strFile)
{
	
	HRESULT				hr;
	CAllocateHierarchy	Alloc(this);

	//--- メッシュの読み込み ----------------------------------------
	LPD3DXANIMATIONCONTROLLER	lpAnim;
	hr = D3DXLoadMeshHierarchyFromX(strFile, D3DXMESH_MANAGED, d3.dev, &Alloc, NULL, &m_pFrameRoot, &lpAnim);
	if(FAILED(hr)) return hr;

	//--- アニメーションセットがある場合は登録 ----------------------
	if(lpAnim != NULL)
	{
		SKIN_ANIM_WORK	*anim = new SKIN_ANIM_WORK;
		memset(anim, 0, sizeof(SKIN_ANIM_WORK));
		anim->m_pController   = lpAnim;
		m_anim.push_back(anim);
		//iSkinAnimationSet = lpAnim->GetMaxNumAnimationSets();
	}

	//--- OBBTree の作成 --------------------------------------------
#ifdef	__OBBTREE
	if(m_pmtxBone)
	{
	//	D3DXFRAME_DERIVED	*a = (D3DXFRAME_DERIVED *)m_pFrameRoot;

		// ワンスキンのOBBTree保存用に各ボーンにメッシュを付加
		AddMeshtoOneSkinModel(m_pFrameRoot);

		// 各頂点がどのボーンから影響を受けているか調べる
		// 注!!! ボーン情報を持つメッシュが一つであることを前提にしている
		LPD3DXSKININFO		pSkinInfo		   = NULL;
		LPD3DXMESHCONTAINER	pD3DXMeshContainer = NULL;
		
		GetSkinMesh(m_pFrameRoot, &pSkinInfo, &pD3DXMeshContainer);
		DWORD		dwBonesNum = pSkinInfo->GetNumBones();
		LPD3DXMESH	pMesh      = pD3DXMeshContainer->MeshData.pMesh;
		int			iVertexNum = pMesh->GetNumVertices();

		std::vector < std::vector < SKINNING_WORK > > SkinningDataVector(iVertexNum);
		
		uint	i, j;
		for(j = 0; j < dwBonesNum; j++)
		{
			DWORD	BoneInfluencesNum = pSkinInfo->GetNumBoneInfluences(j);
			DWORD	*pVertices		  = new DWORD[BoneInfluencesNum];
			FLOAT	*pWeights		  = new FLOAT[BoneInfluencesNum];

			pSkinInfo->GetBoneInfluence(j, pVertices, pWeights);
			for(i = 0; i < BoneInfluencesNum; i++)
			{
				SKINNING_WORK SkinningData = {j, pWeights[i]};
				SkinningDataVector[pVertices[i]].push_back(SkinningData);
			}
			delete [] pWeights;
			delete [] pVertices;
		}

		// 頂点取得
		std::vector < VEC3 >vertex;
		LPDIRECT3DVERTEXBUFFER9	pVB;
		D3DVERTEXBUFFER_DESC	VDesc;

		pMesh->GetVertexBuffer(&pVB);
		pVB->GetDesc(          &VDesc);
		int		iVtxSize = VDesc.Size / iVertexNum;
		float	*pf;
		pVB->Lock(0, 0, (void **)&pf, D3DLOCK_DISCARD);
		
		for(i = 0; i < (uint)iVertexNum; i++)
		{
			VEC3	v;
			v.x = *(pf++);
			v.y = *(pf++);
			v.z = *(pf++);
			vertex.push_back(v);

			// 頂点フォーマットはfloat かDWORDのデータしかないので4バイト単位で
			// アクセス可能だが( pf += ( FVFSize - 4 * 3 ) / 4 )
			// 念のためバイト単位でアクセスする
			pf = (float *)((char *)pf + (iVtxSize - 4 * 3));
		}
		pVB->Unlock();

		// 3角ポリゴン取得
		int		iFaceNum = pMesh->GetNumFaces();
		std::vector < FACES > Faces(dwBonesNum);

		LPDIRECT3DINDEXBUFFER9	pIB;
		D3DINDEXBUFFER_DESC		IDesc;
		pMesh->GetIndexBuffer(&pIB);
		pIB->GetDesc(		  &IDesc);

		WORD	*pw = NULL;
		DWORD	*pd = NULL;
		if(IDesc.Format == D3DFMT_INDEX16)
		{
			pIB->Lock(0, 0, (void**)&pw, D3DLOCK_DISCARD);
		}
		else if(IDesc.Format == D3DFMT_INDEX32)
		{
			pIB->Lock(0, 0, (void**)&pd, D3DLOCK_DISCARD);
		}
		else
		{
			return S_FALSE ;				// インデックスが不正
		}
		// 
		for(int k = 0; k < iFaceNum; k++)
		{
			int		idx[3];
			if(IDesc.Format == D3DFMT_INDEX16)
			{
				idx[0] = *(pw++);
				idx[1] = *(pw++);
				idx[2] = *(pw++);
			}
			else 
			{
				idx[0] = *(pd++);
				idx[1] = *(pd++);
				idx[2] = *(pd++);
			}

			// 3つの頂点が影響を受けているボーンのウェイトの総和を取る
			std::vector < double > Weight(dwBonesNum);
			for(i = 0; i < dwBonesNum; i++)
			{
				Weight[i] = 0;
			}
			for(j = 0; j < 3; j++)
			{
				for(i = 0; i < (uint)SkinningDataVector[idx[j]].size(); i++)
				{
					Weight[SkinningDataVector[idx[j]][i].m_iBoneID] += SkinningDataVector[idx[j]][i].m_fWeight;
				}
			}

			// 一番影響を受けているボーンをサーチ
			double	MaxWeight = -1;
			int		BoneID    = 0;
			for(i = 0; i < dwBonesNum; i++)
			{
				if(MaxWeight< Weight[i])
				{
					MaxWeight = Weight[i];
					BoneID    = i;
				}
			}

			// 一番影響を受けているボーンに3角ポリゴンを振り分ける
			FACE	face;
			for(i = 0; i < 3; i++)
			{
				// ボーンローカル空間に3角ポリゴンを変換
				VEC4	v;
				D3DXVec3Transform(&v, &vertex[idx[i]], pSkinInfo->GetBoneOffsetMatrix(BoneID));
				face.Vertex[i].x = v.x;
				face.Vertex[i].y = v.y;
				face.Vertex[i].z = v.z;
			}
			face.Center = (face.Vertex[0] + face.Vertex[1] + face.Vertex[2]) / 3;
			Faces[BoneID].push_back(face);
		}
		pIB->Unlock();

		std::vector< D3DXFRAME *> FrameVector;
		GetFrame(m_pFrameRoot, FrameVector);
		for(j = 0; j < dwBonesNum; j++)
		{
			LPCSTR	pName = pSkinInfo->GetBoneName(j);
			for(i = 0; i < FrameVector.size(); i++)
			{
				if(FrameVector[i]->Name)
				{
					if(strcmp(pName, FrameVector[i]->Name) == 0) break;
				}
			}
			if(FrameVector.size() != i && Faces[j].size() != 0)
			{
				D3DXMESHCONTAINER_DERIVED	*pCD3DXMesh = (D3DXMESHCONTAINER_DERIVED*)FrameVector[i]->pMeshContainer;
				if(pCD3DXMesh != NULL)
				{
					pCD3DXMesh->pOBBTree = new COBBTree();
					pCD3DXMesh->pOBBTree->Create(Faces[j], iOBBTreeLevel, pCD3DXMesh->pOBBTree->pOBBTopNode);
				}
			}
		}
	}
#endif

	//--- ボーンのセットアップ　-------------------------------------
	hr = SetupBoneMatrixPointers(m_pFrameRoot);
	if(FAILED(hr)){ return hr;}

	//--- おしまい --------------------------------------------------
	return S_OK;
}



/*----------------------------------------------------------------------*
| <<< ここで開放 >>>
*----------------------------------------------------------------------*/
HRESULT	CMyD3DApplication::DeleteDeviceObjects(void)
{
	CAllocateHierarchy	Alloc(this);

	D3DXFrameDestroy(m_pFrameRoot, &Alloc);
	m_pFrameRoot = NULL;


	SAFE_DELETE_ARRAY(m_pmtxBone);

	// アニメーションコントローラーの開放
	uint	a = m_anim.size();
	for(uint i = 0; i < a; i++)
	{
		SAFE_RELEASE(m_anim[i]->m_pController);
	}
	m_anim.clear();

	return S_OK;
}



/*----------------------------------------------------------------------*
| <<< スキンメッシュ読み込み >>>
|	入力	iXNum	    = X File 番号
|			strfile	    = 読み込みＸファイル名
|			bLoadAtOnce = true : 即時読み込み
|			bBumpLoad   = true : 法線マップも同時に読み込む
|			bFurLoad    = true : ファーマップも同時に読み込む
|	戻り値	true or	false
*----------------------------------------------------------------------*/
bool	skin_load(int iXNum, char *strFile, bool bLoadAtOnce, bool bBumpLoad, bool bFurLoad)
{
	
	if(strFile == NULL){ return false;}

#if	__3D_DRAW_TEST
	bool	bRet = d3_xfile_load(iXNum, strFile, bLoadAtOnce);
	d3.meshInfo[iXNum].bSkin = true;
	return bRet;
#endif

	char	path[_MAX_PATH + 1];

	if(strFile != NULL)
	{
		//--- 拡張子がないものはリソースとみなす --------------------
		d3.meshInfo[iXNum].bResource = true;
		int		i		 = 0;
		while(strFile[i] != NULL)
		{
			if(ISKANJI(strFile[i]))   { i += 2;}
			else if(strFile[i] == '.'){ d3.meshInfo[iXNum].bResource = false; break;}
			else					  { i++;}
		}

		//--- ファイル名を作成 --------------------------------------
		//	　リソースの場合にはただコピー
		if(!d3.meshInfo[iXNum].bResource)
		{
			char	name[_MAX_FNAME	+ 1];
			char	ext[ _MAX_EXT	+ 1];
			sys_div_path(path, name, ext, strFile);
			sprintf(path, "%s%s%s",	path, name,	ext);
		}
		else{ strcpy(path, strFile);}
	}
	//--- 後読みの場合 ----------------------------------------------
	else
	{
		strcpy(path, d3.meshInfo[iXNum].strFullPath);	// フォルダー名をコピー
	}

	//--- 即時、読み込みで無い場合 ----------------------------------
	if(!bLoadAtOnce)
	{
		strcpy(d3.meshInfo[iXNum].strFullPath, path);	// ファイル名保存
		d3.meshInfo[iXNum].bLoad = false;
		//d3.meshInfo[iXNum].bBump = bBumpLoad;
		return true;
	}

	//--- 前回分を消去 ----------------------------------------------
	skin_free(	  iXNum);					// 前回データを消去
	d3_xfile_free(iXNum);					// 前回データを消去
	
	//--- ワーク初期化 ----------------------------------------------
 	skin[iXNum]						      = new CMyD3DApplication();
	skin[iXNum]->m_pFrameRoot			  = NULL;
	skin[iXNum]->m_pmtxBone			      = NULL;
	skin[iXNum]->m_uiBoneMatricesMax      = 0;
	skin[iXNum]->m_iXNum				  = iXNum;

	#ifdef	__OBBTREE
		skin[iXNum]->m_bOBBBox			  = false;
		skin[iXNum]->m_iOBBBoxNum		  = -1;
	#endif
	d3.meshInfo[iXNum].ucSkinAnimationSet = 0;
	iSkinVertexCount				      = 0;		// 頂点数をクリアー
	iSkinFaceCount					      = 0;		// 面数をクリアー
	skin[iXNum]->m_iID					  = 0;		// アニメーションコントローラー初期化
	strcpy(d3.meshInfo[iXNum].strFullPath, path);	// フォルダー名をコピー
	d3.meshInfo[iXNum].bSkin			  = true;
	d3.meshInfo[iXNum].bLoad			  = true;
	d3.meshInfo[iXNum].ucSubset			  = 0;
	
	//--- 読み込み	-------------------------------------------------
	BOOL	bFolderChange = FALSE;
	if(!d3.meshInfo[iXNum].bResource){ sys_folder_push(), bFolderChange = FOLDER_SET(path);}
	iSkinBoneCount = 0;

	if(bFolderChange)
	{
		HRESULT	h = skin[iXNum]->SkinLoad(path);

		//---------------------------------------------------------------
		if(!d3.meshInfo[iXNum].bResource){ sys_folder_pop();}
		if(h ==	S_OK)
		{
			//--- ヒットワークを得る ----------------------------------------
			memset(&d3.hitWork[iXNum], 0, sizeof(D3_HIT));
			d3.hitWork[iXNum].vecCenter     = VEC3(0, 0, 0);
			skin[      iXNum]->m_ucHitCount = 0;
			skin[      iXNum]->HitWorkGet(iXNum, skin[iXNum]->m_pFrameRoot);
			d3.hitWork[iXNum].vecCenter    /= (float)skin[iXNum]->m_ucHitCount;

			// HitWorkGet では min と max の値しか得ていないので、ここで BOX を作成
			D3_HIT	*pHit = &d3.hitWork[iXNum];
			VEC3	*min  = &pHit->vecMin;
			VEC3	*max  = &pHit->vecMax;
			pHit->vecLocal[0] =	VEC3(min->x, min->y, min->z);
			pHit->vecLocal[1] =	VEC3(max->x, min->y, min->z);
			pHit->vecLocal[3] =	VEC3(min->x, max->y, min->z);
			pHit->vecLocal[2] =	VEC3(max->x, max->y, min->z);
			pHit->vecLocal[4] =	VEC3(min->x, min->y, max->z);
			pHit->vecLocal[5] =	VEC3(max->x, min->y, max->z);
			pHit->vecLocal[7] =	VEC3(min->x, max->y, max->z);
			pHit->vecLocal[6] =	VEC3(max->x, max->y, max->z);
			d3.meshInfo[iXNum].ucSubset	= skin_subset_get(iXNum);		// サブセット数(Ｘファイル確認ツールなどで使用)
			d3.meshInfo[iXNum].usVertex	= iSkinVertexCount;				// 頂点数を得る
			d3.meshInfo[iXNum].usFaces	= iSkinFaceCount;				// 面数を得る
																		// アニメーショントラック数
			if(skin[iXNum]->m_anim.size() > 0){ d3.meshInfo[iXNum].ucSkinAnimationSet = skin[iXNum]->m_anim[0]->m_pController->GetMaxNumAnimationSets();}
			else							  { d3.meshInfo[iXNum].ucSkinAnimationSet = 0;}
																		// タイムスタンプ登録
			sys_timestamp_get(&d3.meshInfo[iXNum].ftLastWrite, d3.meshInfo[iXNum].strFullPath);
			return true;
		}
		else
		{
			sys_error("skin_load: %s が読み込めませんでした。", strFile);
			if(skin[iXNum] != NULL)
			{
				delete skin[iXNum];
				skin[iXNum] = NULL;
			}
		}
	}

	return false;
}



/*----------------------------------------------------------------------*
| <<< スキンメッシュ読み込み >>>
|	入力	iXNum	    = X File 番号
|			strFile	 = Ｘファイル名
|	戻り値	true or	false
*----------------------------------------------------------------------*/
bool	skin_load(int iXNum, char *strFile)
{
	return	skin_load(iXNum, strFile, !sys.bAfterLoad, false, false);
}



/*----------------------------------------------------------------------*
| <<< スキンメッシュ開放 >>>
|	入力	iXNum = X File 番号
*----------------------------------------------------------------------*/
void	skin_free(int iXNum)
{
	if(skin[iXNum] != NULL)
	{
		skin[iXNum]->DeleteDeviceObjects();
		delete skin[iXNum];
		skin[		iXNum]					  = NULL;
		d3.meshInfo[iXNum].ucSkinAnimationSet = 0;
		d3.meshInfo[iXNum].bResource		  = false;
		d3.meshInfo[iXNum].bLoad			  = false;

		// ヒットワーククリアー
		memset(&d3.hitWork[iXNum], 0, sizeof(D3_HIT));
	}
}



/*----------------------------------------------------------------------*
| <<< スキンメッシュ全開放 >>>
*----------------------------------------------------------------------*/
void	skin_end(void)
{
	for(int	i =	0; i < D3_MESH_MAX; i++) skin_free(i);
}



/*----------------------------------------------------------------------*
| <<< アニメーションタイムを進める >>>
|	入力	iXNum =	XFILE 番号
|			iANum = アニメーションコントローラー番号 
|			fTime =	進める時間
*----------------------------------------------------------------------*/
/*void	skin_frame_set(int iXNum, int iANum, float fTime)
{
	if(skin[iXNum]				  == NULL       ){ return;}	// Ｘファイル未読み込み
	if(skin[iXNum]->m_anim.size() <= (uint)iANum){ return;}	// 不正なアニメーション番号の場合
	
	skin[iXNum]->m_iAnim = iANum;
	skin[iXNum]->FrameMove(fTime);
}*/



/*----------------------------------------------------------------------*
| <<< アニメーションタイムを進める >>>
|	入力	iXNum =	XFILE 番号
|			fTime =	進める時間
*----------------------------------------------------------------------*/
void	skin_anim_move(int iXNum, float fTime)
{
	//if(skin[iXNum] == NULL){ return;}		// 未読み込み
	//skin_frame_set(iXNum, skin[iXNum]->m_iAnim, fTime);

	if(skin[iXNum] == NULL){ return;}		// Ｘファイル未読み込み
	skin[iXNum]->FrameMove(fTime);
}

void	skin_frame_set(int iXNum, float fTime)
{
	skin_anim_move(iXNum, fTime);
}



/*----------------------------------------------------------------------*
| <<< アニメーションタイムをリセット>>>
|	入力	iXNum =	XFILE 番号
*----------------------------------------------------------------------*/
void	skin_anim_reset(int iXNum)
{

	if(skin[iXNum] == NULL){ return;}		// 未読み込み

	int	a = skin[iXNum]->m_iID;
	skin[iXNum]->m_anim[a]->m_pController->SetTrackPosition(skin[iXNum]->m_anim[a]->m_iCurrent, 0);
	skin[iXNum]->m_anim[a]->m_dTime = 0;
	skin[iXNum]->m_anim[a]->m_pController->AdvanceTime(0, NULL);
	skin[iXNum]->m_anim[a]->m_pController->ResetTime();
}

void	skin_frame_reset(int iXNum)
{
	skin_anim_reset(iXNum);
}



/*----------------------------------------------------------------------*
| <<< アニメーション ID 変更 >>>			※ひとつのモデルを複数表示するときに使用のこと
|	入力	iXNum =	XFILE 番号
|			iID   = ＩＤ番号
*----------------------------------------------------------------------*/
void	skin_id_change(int iXNum, int iID)
{

	if(skin[iXNum] == NULL){ return;}
	skin[iXNum]->m_iID = iID;

	// アニメーショントラック数を調べる
	if(skin[iXNum]->m_anim.size() < (uint)iID + 1)
	{
		if(skin[iXNum]->m_anim.size() > 0)
		{
			SKIN_ANIM_WORK	*ori  = skin[iXNum]->m_anim[0];
			SKIN_ANIM_WORK	*anim = new SKIN_ANIM_WORK;
			//memset(anim, 0, sizeof(SKIN_ANIM_WORK));
			
			memcpy(anim, ori, sizeof(SKIN_ANIM_WORK));

			LPD3DXANIMATIONCONTROLLER	a = ori->m_pController;
			HRESULT	hr = a->CloneAnimationController(a->GetMaxNumAnimationOutputs(),
													 a->GetMaxNumAnimationSets(),
													 a->GetMaxNumTracks(),
													 a->GetMaxNumEvents(),
													 &anim->m_pController);
			if(hr == S_OK)
			{
				anim->m_pController->SetTrackPosition(0, 0);	// ちゃんとトラックとタイムもリセットしましょう。
				anim->m_pController->AdvanceTime(0, NULL);
				skin[iXNum]->m_anim.push_back(anim);
			}
		}
	}
}

void	skin_anime_controller_change(int iXNum, int iID)
{
	skin_id_change(iXNum, iID);
}



/*----------------------------------------------------------------------*
| <<< アニメーショントラック変更 >>>
|	入力	iXNum     =	XFILE 番号
|			iTrackNum =	トラック番号
|			fFadeTime =	フェード時間
|			bReset	  =	切り替えるアニメーションを一コマ目から再生するか
|	戻り値	true : トラック変更成功
*----------------------------------------------------------------------*/
bool	skin_anim_set(int iXNum, int iTrackNum, float fFadeTime, bool bReset)
{

	CMyD3DApplication	*s = skin[iXNum];


	if(s ==	NULL){ return false;}			// 未読み込み
											// ボーンなし
	if(s->m_pmtxBone == NULL){ return false;}

	SKIN_ANIM_WORK	*a = s->m_anim[s->m_iID];
	if(s->m_anim.size() < (uint)s->m_iID){ return false;}

	if(a->m_iTrack							 == iTrackNum) return false;
	if(d3.meshInfo[iXNum].ucSkinAnimationSet <= iTrackNum) return false;

	int		iCurrect = a->m_iCurrent++;
	int		iNext	 = a->m_iCurrent & 1;

	if(fFadeTime == 0){ fFadeTime = 0.01f;}				// .net2008 では、この行が無いと以下で無限ループに入ってしまいます

	double	dFade	 = (double)fFadeTime / 30.0;		// アニメーションミキシング時間

	LPD3DXANIMATIONSET			as;
	LPD3DXANIMATIONCONTROLLER	c = a->m_pController;

	c->GetAnimationSet(	    a->m_iTrack, &as);
	c->SetTrackAnimationSet(iCurrect,     as);
	as->Release();
	c->GetAnimationSet(	    iTrackNum,	 &as);
	c->SetTrackAnimationSet(iNext,		  as);
	as->Release();

	// dFade が 0 の場合、無限ループに入ります

	c->UnkeyAllTrackEvents(iCurrect);
	c->UnkeyAllTrackEvents(iNext);

	c->KeyTrackEnable(iCurrect, FALSE, a->m_dTime + dFade);
	c->KeyTrackSpeed( iCurrect, 0.0f,  a->m_dTime,	dFade, D3DXTRANSITION_LINEAR);
	c->KeyTrackWeight(iCurrect, 0.0f,  a->m_dTime,	dFade, D3DXTRANSITION_LINEAR);

	c->SetTrackEnable(iNext,	TRUE);
	c->KeyTrackSpeed( iNext,	1.0f,  a->m_dTime,  dFade, D3DXTRANSITION_LINEAR);
	c->KeyTrackWeight(iNext,	1.0f,  a->m_dTime,  dFade, D3DXTRANSITION_LINEAR);

	//if(bReset){ c->SetTrackPosition(iNext, 0);}

	c->SetTrackPosition(iNext, 0);

	a->m_iTrack   = iTrackNum;
	a->m_iCurrent = iNext;
	return true;
}



/*----------------------------------------------------------------------*
| <<< アニメーショントラック変更 >>>
|	入力	iXNum     =	XFILE 番号
|			iTrackNum =	トラック番号
|			fFadeTime =	フェード時間
|	戻り値	true : トラック変更成功
*----------------------------------------------------------------------*/
bool	skin_anim_set(int iXNum, int iTrackNum, float fFadeTime)
{
	return skin_anim_set(iXNum, iTrackNum, fFadeTime, true);
}

bool	skin_anim_change(int iXNum, int iTrackNum, float fFadeTime)
{
	return skin_anim_set(iXNum, iTrackNum, fFadeTime, true);
}
bool	skin_anim_change(int iXNum, int iTrackNum)
{
	return skin_anim_set(iXNum, iTrackNum, 0.1f, true);
}


/*----------------------------------------------------------------------*
| <<< レンダリング >>>
|	入力	*mtxWorld = ワールド行列
*----------------------------------------------------------------------*/
HRESULT	CMyD3DApplication::Render(MTX *mtxWorld)
{

	iUpdateCount = 0;						// アップデート数クリアー
	UpdateFrameMatrices(m_pFrameRoot, mtxWorld);
	DrawFrame(m_pFrameRoot);
	return true;
}


#ifdef	__OBBTREE
/*----------------------------------------------------------------------*
| <<< OBBBox 描画 >>>
|	入力	*mtxWorld = ワールド行列
*----------------------------------------------------------------------*/
HRESULT	CMyD3DApplication::OBBRender(MTX *mtxWorld)
{

	m_iOBBBoxMax = 0;
	UpdateFrameMatrices(m_pFrameRoot, mtxWorld);
	OBBDrawFrame(m_pFrameRoot);
	return true;
}
#endif


/*----------------------------------------------------------------------*
| <<< ルートの名前を得る >>>
|	入力	iXNum    = XFILE 番号
|			iStrSize = 名前の最大文字数
|	出力	strOutBuf = 名前が iStrSize 毎に格納
|	※バッファは iStrSize * 256 ぐらいは用意しておくこと
*----------------------------------------------------------------------*/
void	skin_root_name_get(int iXNum, char *strOut, int iStrSize)
{
	if(skin[iXNum] == NULL){ return;}
	skin[iXNum]->SubsetNameGet(skin[iXNum]->m_pFrameRoot, strOut, iStrSize, 0);
}




/*----------------------------------------------------------------------*
| <<< サブセット数を数える >>>
|	※入れ子になっている場合は再起呼び出しされるので注意
*----------------------------------------------------------------------*/
void	CMyD3DApplication::MaterialCountGet(LPD3DXFRAME lpFrame, uchar *ucCount)
{
	LPD3DXMESHCONTAINER			lpMeshContainer        = lpFrame->pMeshContainer;
	//D3DXMESHCONTAINER_DERIVED	*pMeshContainerDerived = (D3DXMESHCONTAINER_DERIVED*)lpMeshContainer;

	while(lpMeshContainer != NULL)
	{
		if(lpMeshContainer->MeshData.Type == D3DXMESHTYPE_MESH)
		{									// ここでサブセット数を増やしています
			//*ucCount = *ucCount + (char)pMeshContainerDerived->NumMaterials;
			*ucCount = *ucCount + (char)lpMeshContainer->NumMaterials;
		}
		lpMeshContainer = lpMeshContainer->pNextMeshContainer;
	}

	if(lpFrame->pFrameSibling    != NULL){ MaterialCountGet(lpFrame->pFrameSibling,	ucCount);}
	if(lpFrame->pFrameFirstChild != NULL){ MaterialCountGet(lpFrame->pFrameFirstChild, ucCount);}
}



/*----------------------------------------------------------------------*
| <<< サブセット数を得る >>>
|	入力	iXNum    = XFILE 番号
|			iType	 = 頂点シェーダーの型
|			mtxWorld = ワールド座標
|			fAlp	 = 半透明レート
|	戻り値	true  :	描画した（フレームスキップを含む）
|			false :	描画しなかった
*----------------------------------------------------------------------*/
int		skin_subset_get(int iXNum)
{
	skin[iXNum]->m_ucSubset = 0;
	skin[iXNum]->MaterialCountGet(skin[iXNum]->m_pFrameRoot, &skin[iXNum]->m_ucSubset);
	return (int)skin[iXNum]->m_ucSubset;
}




/*----------------------------------------------------------------------*
| <<< 描画 >>>
|	入力	iXNum    = XFILE 番号
|			iType	 = 頂点シェーダーの型
|			mtxWorld = ワールド座標
|			fAlp	 = 半透明レート
|	戻り値	true  :	描画した（フレームスキップを含む）
|			false :	描画しなかった
*----------------------------------------------------------------------*/
static	bool	skin_draw2(int iXNum, int iType, MTX *mtxWorld, float fAlp)
{

	if(iType > shader_max_get()){ return false;}
	pShaderProc = pShaderTbl[iType];

#if	__3D_DRAW_TEST
	d3_3d_draw(iXNum, iType, mtxWorld, fAlp);
	return true;
#endif

	if(iType == D3_SHADER_TYPE_OFF){ return false;}

	//--- 読み込んでいないものはここで対応する ----------------------
	if(skin[iXNum] == NULL)
	{
		if(d3.meshInfo[iXNum].strFullPath[0] != NULL)
		{
			if(skin_load(iXNum, &d3.meshInfo[iXNum].strFullPath[0], true, false, false))//d3.meshInfo[iXNum].bBump, d3.meshInfo[iXNum].bFur))
			{
				d3.meshInfo[iXNum].strFullPath[0] = NULL;
				return false;
			}
		}
		return false;						// 未読み込み
	}

	iDrawCount				   = 0;			// メッシュの描画計測用(１フレーム毎にカウントアップしていきます)
	skin[iXNum]->m_iShaderType = iType;
	skin[iXNum]->m_fAlp        = fAlp;
	skin[iXNum]->Render(mtxWorld);

#ifdef	__OBBTREE
	iDrawMeshCount             = 0;			// メッシュの描画計測用(１フレーム毎にカウントアップしていきます)
	skin[iXNum]->OBBRender(mtxWorld);
#endif
											// 頂点数計算
	d3.iPolyCount += d3.meshInfo[iXNum].usFaces;

	d3_shader_set(D3_SHADER_TYPE_OFF);
	return true;
}


/*----------------------------------------------------------------------*
| <<< 描画 >>>
|	入力	iXNum       = XFILE 番号
|			iShaderType = 頂点シェーダーの型
|			mtxWorld    = ワールド座標
|			fAlp	    = 半透明レート
|	戻り値	true  :	描画した（フレームスキップを含む）
|			false :	描画しなかった
*----------------------------------------------------------------------*/
bool	skin_draw(int iXNum, int iShaderType, MTX *mtxWorld, float fAlp)
{
	d3_shader_set(iShaderType);
	return	skin_draw2(iXNum, iShaderType, mtxWorld, fAlp);
}


/*----------------------------------------------------------------------*
| <<< 描画 >>>
|	入力	iXNum       = XFILE 番号
|			iShaderType = 頂点シェーダーの型
|			mtxWorld    = ワールド座標
|			fAlp	    = 半透明レート
|	戻り値	true  :	描画した（フレームスキップを含む）
|			false :	描画しなかった
*----------------------------------------------------------------------*/
bool	skin_draw(int iXNum, int iShaderType, MTX *mtxWorld)
{
	d3_shader_set(iShaderType);
	return	skin_draw2(iXNum, iShaderType, mtxWorld, 1);
}


/*----------------------------------------------------------------------*
| <<< 描画 >>>
|	入力	iXNum     =	XFILE 番号
|			iShaderType = 頂点シェーダーの型
|			mtxWorld    = ワールド座標
|			fAlp	    = 半透明レート
|	戻り値	true  :	描画した（フレームスキップを含む）
|			false :	描画しなかった
*----------------------------------------------------------------------*/
bool	skin_draw(int iXNum, MTX *mtxWorld)
{
	d3_shader_set(D3_SHADER_TYPE_NORMAL);
	return skin_draw2(iXNum, D3_SHADER_TYPE_NORMAL, mtxWorld, 1);
}


/*----------------------------------------------------------------------*
| <<< 描画 >>>
|	入力	iXNum    = XFILE 番号
|			mtxWorld = ワールド座標
|			fAlp	 = 半透明値
|	戻り値	true  :	描画した（フレームスキップを含む）
|			false :	描画しなかった
*----------------------------------------------------------------------*/
bool	skin_draw(int iXNum, MTX *mtxWorld, float fAlp)
{
	d3_shader_set(D3_SHADER_TYPE_NORMAL);
	return	skin_draw2(iXNum, D3_SHADER_TYPE_NORMAL, mtxWorld, fAlp);
}


/*----------------------------------------------------------------------*
| <<< １パーツのみ描画 >>>
|	入力	iXNum       = XFILE 番号
|			iParts      = パーツ番号
|			iShaderType = シェーダータイプ
|			mtxWorld    = 配置座標
|			fAlp	    = アルファ値(0.f ～ 1.f)
|	戻り値	true  :	描画した（フレームスキップを含む）
|			false :	描画しなかった
*----------------------------------------------------------------------*/
bool	skin_draw_parts(int iXNum, int iParts, int iShaderType, MTX *mtxWorld, float fAlp)
{

	//d3_shader_set(iShaderType);
	//return	skin_draw2(iNum, iShaderType, mtxWorld, fAlp);
	if(iShaderType > shader_max_get()){ return false;}
	pShaderProc = pShaderTbl[iShaderType];

#if	__3D_DRAW_TEST
	//d3_3d_draw(iXNum, iShaderType, mtxWorld, fAlp);
	d3_3d_draw_parts(iXNum, iParts, iShaderType, mtxWorld, fAlp);
	return true;
#endif

	if(iShaderType == D3_SHADER_TYPE_OFF){ return false;}

	d3.eff = d3.lpEffect[iShaderType];
	//--- 読み込んでいないものはここで対応する ----------------------
	if(skin[iXNum] == NULL)
	{
		if(d3.meshInfo[iXNum].strFullPath[0] != NULL)
		{
			if(skin_load(iXNum, &d3.meshInfo[iXNum].strFullPath[0], true, false, false))//d3.meshInfo[iXNum].bBump, d3.meshInfo[iXNum].bFur))
			{
				d3.meshInfo[iXNum].strFullPath[0] = NULL;
				return false;
			}
		}
		return false;						// 未読み込み
	}

	iDrawCount				   = 0;			// メッシュの描画計測用(１フレーム毎にカウントアップしていきます)
	skin[iXNum]->m_iShaderType = iShaderType;
	skin[iXNum]->m_fAlp        = fAlp;

	//skin[iXNum]->Render(mtxWorld);

	iUpdateCount = 0;						// アップデート数クリアー
	skin[iXNum]->UpdateFrameMatrices(skin[iXNum]->m_pFrameRoot, mtxWorld);
	skin[iXNum]->DrawFrame(skin[iXNum]->m_pFrameRoot, 0, iParts);

#ifdef	__OBBTREE
	iDrawMeshCount             = 0;			// メッシュの描画計測用(１フレーム毎にカウントアップしていきます)
	skin[iXNum]->OBBRender(mtxWorld);
#endif
											// 頂点数計算
	d3.iPolyCount += d3.meshInfo[iXNum].usFaces;

	d3_shader_set(D3_SHADER_TYPE_OFF);
	return true;

}


/*----------------------------------------------------------------------*
| <<< ルートの行列を返す >>>
|	入力	iXNum   =	XFILE 番号
|	戻り値	ルートの行列
*----------------------------------------------------------------------*/
MTX		*skin_root_matrix_get(int iXNum)
{
	if(skin[iXNum] == NULL){ return &d3.mtxIdentity;}
	return	&skin[iXNum]->m_mtxRoot;
}


/*----------------------------------------------------------------------*
| <<< アニメーション数を返す >>>
|	入力	iXNum     =	XFILE 番号
*----------------------------------------------------------------------*/
int		skin_anim_count_get(int iXNum)
{
	return	d3.meshInfo[iXNum].ucSkinAnimationSet;
}
int		skin_animation_count_get(int iXNum)
{
	return	d3.meshInfo[iXNum].ucSkinAnimationSet;
}


/*----------------------------------------------------------------------*
| <<< スキンメッシュを読み込んでいるかチェックする >>>
|	入力	iXNum     =	XFILE 番号
|	戻り値	true  :	読み込んでいる
|			false :	読み込んでいない
*----------------------------------------------------------------------*/
bool	skin_load_check(int iXNum)
{

	if(!d3.meshInfo[iXNum].bLoad)
	{
		if(skin_load(iXNum, NULL, true, false, false))//d3.meshInfo[iXNum].bBump, d3.meshInfo[iXNum].bFur))
		{
			d3.meshInfo[iXNum].bLoad = true;
			return false;
		}
	}
	return skin[iXNum] != NULL;
}


/*----------------------------------------------------------------------*
| <<< スキンメッシュ使用時の割り込み処理設定 >>>
|	入力	pProc = 割り込み関数
*----------------------------------------------------------------------*/
void	skin_skin_proc_set(void *pProc)
{
	pSkinProc = pProc;
}


/*----------------------------------------------------------------------*
| <<< スキンメッシュ描画時の残像処理などを登録する >>>	※ボーン未使用時
|	入力	pProc = 割り込み関数
*----------------------------------------------------------------------*/
void	skin_frame_proc_set(void *pProc)
{
	pFrameProc = pProc;
}


/*----------------------------------------------------------------------*
| <<< スキンメッシュ同士、特定のボックス同士の当たり判定 >>>
|	入力	a, b = スキンメッシュ読み込み番号
|			iANum, iBNum = ボックス番号( -1 : 全てのボックスが対象)
*----------------------------------------------------------------------*/
bool	skin_hit_check(int *pAOut, int *pBOut, int a, int iANum, int b, int iBNum)
{

	//--- メッシュが存在するかチェック ------------------------------
	if(skin[a] == NULL || skin[b] == NULL) return false;

#ifdef	__OBBTREE

	//--- チェック開始 ----------------------------------------------
	vector< D3DXFRAME *>FrameAVector, FrameBVector;

	skin[a]->GetFrame(skin[a]->m_pFrameRoot, FrameAVector);
	skin[b]->GetFrame(skin[b]->m_pFrameRoot, FrameBVector);

	int		iACount;						// ヒット BOX の計測用
	int		iBCount = -1;					// ループの i, j はフレーム数であり、フレーム中に OBBTree が含まれないものもあるので注意

	for(uint j = 0; j < FrameBVector.size(); j++)
	{
											// メッシュと OBBTree があるか調べる
		D3DXMESHCONTAINER_DERIVED	*pCD3DXMeshB = (D3DXMESHCONTAINER_DERIVED *)FrameBVector[j]->pMeshContainer;
		if(pCD3DXMeshB           == NULL) continue;
		if(pCD3DXMeshB->pOBBTree == NULL) continue;
		iBCount++;
		if(iBNum != -1 && iBNum != iBCount) continue;

		iACount = -1;
		for(uint i = 0; i < FrameAVector.size(); i++)
		{
											// メッシュと OBBTree があるか調べる
			D3DXMESHCONTAINER_DERIVED	*pCD3DXMeshA = (D3DXMESHCONTAINER_DERIVED *)FrameAVector[i]->pMeshContainer;
			if(pCD3DXMeshA           == NULL) continue;
			if(pCD3DXMeshA->pOBBTree == NULL) continue;
			iACount++;
			if(iANum != -1 && iANum != iACount) continue;

			//---- コリジョン ---------------------------------------
			D3DXFRAME_DERIVED *a = (D3DXFRAME_DERIVED *)FrameAVector[i];
			D3DXFRAME_DERIVED *b = (D3DXFRAME_DERIVED *)FrameBVector[j];

			if(COBBTree().CheckCollision(*pCD3DXMeshA->pOBBTree, a->CombinedTransformationMatrix, 
										 *pCD3DXMeshB->pOBBTree, b->CombinedTransformationMatrix))
			{
				*pAOut = iACount;
				*pBOut = iBCount;
				return true;
			}
			// 逆もヤッテミル(コリジョンが不正確なため一時的な処理です。将来的になくなります)
			if(COBBTree().CheckCollision(*pCD3DXMeshB->pOBBTree, b->CombinedTransformationMatrix, 
										 *pCD3DXMeshA->pOBBTree, a->CombinedTransformationMatrix))
			{
				*pAOut = iACount;
				*pBOut = iBCount;
				return true;
			}
		}
	}
#endif
	return false;
}


/*----------------------------------------------------------------------*
| <<< スキンメッシュ同士の当たり判定 >>>
|	入力	a, b = スキンメッシュ読み込み番号
*----------------------------------------------------------------------*/
bool	skin_hit_check(int a, int b)
{
	int	dumA, dumB;
	return	skin_hit_check(&dumA, &dumB, a, -1, b, -1);
}


/*----------------------------------------------------------------------*
| <<< OBBTree の指定の BOX の中心を得る >>>
|	入力	iXNum   = XFILE
|			iBoxNum = ボックス番号( -1 は不可)
*----------------------------------------------------------------------*/
VEC3	*skin_hit_pos_get(int iXNum, int iBoxNum)
{

	static	VEC3	vecRet;

	vecRet = VEC3(0, 0, 0);

	//--- メッシュが存在するかチェック ------------------------------
	if(skin[iXNum] == NULL) return &vecRet;

#ifdef	__OBBTREE

	//--- チェック開始 ----------------------------------------------
	vector< D3DXFRAME *>FrameAVector;

	skin[iXNum]->GetFrame(skin[iXNum]->m_pFrameRoot, FrameAVector);

	int		iACount = -1;					// ヒット BOX の計測用

	iACount = -1;
	//uint	aaa =FrameAVector.size();
	for(uint i = 0; i < FrameAVector.size(); i++)
	{
										// メッシュと OBBTree があるか調べる
		D3DXMESHCONTAINER_DERIVED	*pCD3DXMeshA = (D3DXMESHCONTAINER_DERIVED *)FrameAVector[i]->pMeshContainer;
		if(pCD3DXMeshA           == NULL) continue;
		if(pCD3DXMeshA->pOBBTree == NULL) continue;
		iACount++;
		if(iBoxNum!= iACount) continue;

		//--- BOX の中心座標を取ります
		uint	uiMax = pCD3DXMeshA->pOBBTree->vecHit.size();
		for(uint j = 0; j < uiMax; j++)
		{
			vecRet += pCD3DXMeshA->pOBBTree->vecHit[j];
		}
		vecRet /= (float)uiMax;

		return &vecRet;
	}
#endif
	return &vecRet;
}


/*----------------------------------------------------------------------*
| <<< OBBTree のボックス表示設定 >>>
|	入力	iXNum =	XFILE 番号
|			bSw   = true : OBBTree のボックスを描画
*----------------------------------------------------------------------*/
void	skin_obbtree_box_set(int iXNum, bool bSw)
{
#ifdef	__OBBTREE
	if(skin[iXNum] == NULL) return;
	skin[iXNum]->m_bOBBBox = bSw;
#endif
}


/*----------------------------------------------------------------------*
| <<< OBBTree のボックス描画番号設定 >>>
|	入力	iXNum   = XFILE 番号
|			iOBBNum = OBB 番号 (-1 で全描画)
*----------------------------------------------------------------------*/
void	skin_obbtree_box_draw_num_set(int iXNum, int iOBBNum)
{
#ifdef	__OBBTREE
	if(skin[iXNum] == NULL) return;
	skin[iXNum]->m_iOBBBoxNum = iOBBNum;
#endif
}


/*----------------------------------------------------------------------*
| <<< OBBTree のボックス総数を得る >>>	※描画でないと値が返されません
|	入力	iXNum   = XFILE 番号
|	戻り値	OBBTree の BOX 総数
*----------------------------------------------------------------------*/
int		skin_obbtree_box_count_get(int iXNum)
{
#ifdef	__OBBTREE
	if(skin[iXNum] == NULL) return 0;
	return skin[iXNum]->m_iOBBBoxMax - 1;
#else
	return 0;
#endif
}



/*----------------------------------------------------------------------*
| <<< スキンメッシュのフレームアップデート数を得る >>>
|	入力	iNum	 = 表示番号
|	戻り値	フレーム数
*----------------------------------------------------------------------*/
int		skin_frame_update_count_get(int iNum)
{

#if	__3D_DRAW_TEST
	return 0;
#endif

	if(skin[iNum] == NULL) return 0;

	iUpdateCount = 0;						// アップデート数クリアー
//	skin[iNum]->FrameMatricesCountGet(skin[iNum]->m_pFrameRoot, &d3.mtxIdentity);
	return iUpdateCount;
}




#endif			// __SKIN
