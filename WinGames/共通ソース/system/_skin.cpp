//
//	�X�L�����b�V������
//
//	������� __OBBTREE ���g���ƁA�V�F�[�_�[���|����Ȃ��Ȃ�܂�


#ifdef	__SKIN

// �ȉ��� main.h �Œ�`�̂���
//#define	__OBBTREE					// OBBTree ���g�p����
//#define	__HIT_OBBTREE_BOX			// OBBTree �̃q�b�gBOX ��`�悷��


//--- �e��C���N���[�h ----------------------------------------------------------------------------
#ifdef	__OBBTREE
	#include	"ex/_obbtree.cpp"
	#pragma warning(disable:4244 4786 4819 4996)
	using	namespace	std;					// ���̃X�R�[�v���ł�std::���ȗ��ł���
	#include	<vector>
#endif

//--- �萔��` ------------------------------------------------------------------------------------
#define	__3D_DRAW_TEST		0				// 1 : d3_draw	�̃`�F�b�N�p
#define	SHADER_BONE_MAX		50				// ���_�V�F�[�_�[ 1.1 �܂ł� 26	�{


//--- �ϐ� ----------------------------------------------------------------------------------------
static	int			iUpdateCount;			// skinproc �ɓn����܂�
		int			iSkinFaceCount;			// ���ʐ�
		int			iSkinVertexCount;		// �����_��
CMyD3DApplication	*skin[D3_MESH_MAX];

static	bool		bBumpLoadSw;			// true	: �o���v�ǂݍ���
static	bool		bFurLoadSw;				// true	: �t�@�[�ǂݍ���
static	int			iContainerCount;		// ���b�V���̏����v���p
											// DrawMeshContainer ��ʂ��������v�����܂�(�`�悵�Ȃ����� +1 ����܂�����)
static	int			iDrawCount;				// ���b�V���̕`��v���p(�P�t���[�����ɃJ�E���g�A�b�v���Ă����܂�)
											// ���荞�ݎ��Ɏg�p���܂�
static	void		*pSkinProc;				// ��̉�]�ȂǂŎg���֐��|�C���^
static	void		*pFrameProc;			// �c���₠���蔻��Ŏg���֐��|�C���^
											// skin_proc_set �Őݒ�̂���
		void		*pShaderProc;			// �V�F�[�_�[���荞�ݗp
											// �s��p���b�g�p
		MTX			mtxSkinPalette[SHADER_BONE_MAX];

static	int			iDrawMeshCount;			// �`�悷�鐔
		int			iSkinBoneCount;			// �I���W�i���̃{�[���̐�(50�{�𒴂�����T�O�Ɋۂߍ��܂��)


//--- �֐��錾 ------------------------------------------------------------------------------------
static	HRESULT	AllocateName(LPCSTR Name, LPSTR *pNewName);



/*----------------------------------------------------------------------*
| <<< ���O�̃Z�b�g >>>
|	����	Name     = �V�������O
|			pNewName = ���O�̊i�[�ʒu
|	�߂�l	S_OK     = ����
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
| <<< �t���[���̍쐬 >>>	�����̊֐��͈����̕ύX�s��
*----------------------------------------------------------------------*/
HRESULT	CAllocateHierarchy::CreateFrame(LPCSTR Name, LPD3DXFRAME *ppNewFrame)
{

	HRESULT		hr = S_OK;

	*ppNewFrame					= NULL;
	D3DXFRAME_DERIVED	*pFrame = new D3DXFRAME_DERIVED;
	if(pFrame == NULL){	hr = E_OUTOFMEMORY;	goto e_Exit;}

	AllocateName(Name, &pFrame->Name);

	// �����o�[���N���A�[
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
| <<< ���b�V���̐��` >>>
|	�����̊֐��� D3DXLoadMeshHierarchyFromX	�R�[�����ɌĂяo�����֐��ł��B
|	  ���e�̓��b�V���A�e�N�X�`���[�A�}�e���A���̓ǂݍ��݂� FVF �̕ύX�ł��B
|	  �Ȃ��A�����̕ύX�͂ł��Ȃ��̂Œ��ӁB
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

	//--- �s���ȃ��b�V���̏ꍇ --------------------------------------
	if(pMeshData->Type != D3DXMESHTYPE_MESH){ hr = E_FAIL; goto	e_Exit;}

	//--- ���b�V���f�[�^�\������R�s�[ ------------------------------
	pMesh =	pMeshData->pMesh;

	//--- �������� FVF �̏ꍇ ---------------------------------------
	if(pMesh->GetFVF()      == 0){ hr = E_FAIL; goto e_Exit;}

	//--- �ʂ��O�̏ꍇ ----------------------------------------------
	if(pMesh->GetNumFaces() == 0){ hr = E_FAIL; goto e_Exit;}

	//--- �������m�� ------------------------------------------------
	pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	if(pMeshContainer == NULL){	hr = E_OUTOFMEMORY;	goto e_Exit;}
	memset(pMeshContainer, 0, sizeof(D3DXMESHCONTAINER_DERIVED));

	//--- ���O�̃Z�b�g ----------------------------------------------
	if(FAILED(hr = AllocateName(Name, &pMeshContainer->Name))){ goto e_Exit;}

	//--- ���̃t���[���̃|���S�����𓾂� ----------------------------
	NumFaces = pMesh->GetNumFaces();

	//--- ������ ----------------------------------------------------
	// �t�H�[�}�b�g�𐮌`����
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
	iSkinVertexCount			 +=	pMesh->GetNumVertices();	// ���_���𓾂�
	iSkinFaceCount				 +=	pMesh->GetNumFaces();		// �ʐ��𓾂�

	//--- �q�b�g���[�N�𓾂� ----------------------------------------
	d3_hit_work_get(&pMeshContainer->hitWork, pMesh);

	//--- �}�e���A���ƃe�N�X�`���[�̓ǂݍ��� ------------------------
	pMeshContainer->NumMaterials = max(1, dwMaterialMax);
	pMeshContainer->pMaterials	 = new D3DXMATERIAL[pMeshContainer->NumMaterials];
	pMeshContainer->pAdjacency	 = new DWORD[       NumFaces * 3];

	if((pMeshContainer->pAdjacency == NULL)	|| (pMeshContainer->pMaterials == NULL)){ hr = E_OUTOFMEMORY; goto e_Exit;}

	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * NumFaces	* 3);

	if(dwMaterialMax > 0)					// �e�N�X�`���[��ǂݍ��݂ɂ���
	{
		memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL)	* dwMaterialMax);
											// �e�N�X�`���[�̈���m��		
		pMeshContainer->sTex = new short[dwMaterialMax];

		for(uint uiMaterial = 0; uiMaterial < dwMaterialMax; uiMaterial++)
		{
			pMeshContainer->sTex[uiMaterial] = -1;

			if(pMeshContainer->pMaterials[uiMaterial].pTextureFilename != NULL)
			{								// �ʏ�e�N�X�`���[
				short	sTex = d3_tex_load(-1, pMeshContainer->pMaterials[uiMaterial].pTextureFilename, true);
				if(sTex != -1){ pMeshContainer->sTex[uiMaterial] = sTex; d3.texInfo[sTex].usUseCount++;}
				pMeshContainer->pMaterials[uiMaterial].pTextureFilename = NULL;
			}
		}
	}
	else									// �}�e���A�����Ȃ��ꍇ�̓_�~�[������Ă�����
	{
		pMeshContainer->sTex[      0]				   = -1;
		pMeshContainer->pMaterials[0].pTextureFilename = NULL;
		memset(&pMeshContainer->pMaterials[0].MatD3D, 0, sizeof(D3DMATERIAL9));
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Specular  = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
	}

	//--- �{�[���Ȃ� ----------------------------------------------
	if(pSkinInfo !=	NULL)
	{
		// pSkinInfo �� pMesh ���Z�b�g
		pMeshContainer->pSkinInfo =	pSkinInfo;
		pSkinInfo->AddRef();

		pMeshContainer->pOrigMesh =	pMesh;
		pMesh->AddRef();

		// Bone Offset Matrices �ۑ��p�̃��������m��
		uint	uiBoneMax = pSkinInfo->GetNumBones();
		iSkinBoneCount   += uiBoneMax;		// �O���ŕ\���ł���悤�Ƀ{�[�������o��

		pMeshContainer->m_pmtxBoneOffset = new MTX[uiBoneMax];
		if(pMeshContainer->m_pmtxBoneOffset == NULL){ hr = E_OUTOFMEMORY; goto e_Exit;}

		// Bone Offset Matrices �ǂݍ���
		for(uint uiBone = 0; uiBone < uiBoneMax; uiBone++)
		{
			pMeshContainer->m_pmtxBoneOffset[uiBone] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(uiBone));
		}
		// �X�L�����b�V���쐬
		if(FAILED(hr = m_pApp->GenerateSkinnedMesh(pMeshContainer))) goto e_Exit;
	}
#ifdef	__OBBTREE
	//--- �{�[���������ꍇ ------------------------------------------
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
| <<< �t���[���̍폜 >>>
|	�����̊֐��͈����̕ύX�s��
*----------------------------------------------------------------------*/
HRESULT	CAllocateHierarchy::DestroyFrame(LPD3DXFRAME pFrame) 
{
	SAFE_DELETE_ARRAY(pFrame->Name);
	SAFE_DELETE(	  pFrame);
	return S_OK; 
}


/*----------------------------------------------------------------------*
| <<< ���b�V���R���e�i�[�̍폜 >>>
|	���m�ۂ����������͂����ŊJ�����܂��傤�B
*----------------------------------------------------------------------*/
HRESULT	CAllocateHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{

	D3DXMESHCONTAINER_DERIVED	*pMeshContainer	= (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

	SAFE_DELETE_ARRAY(pMeshContainer->Name);
	SAFE_DELETE_ARRAY(pMeshContainer->pAdjacency);
	SAFE_DELETE_ARRAY(pMeshContainer->pMaterials);
	SAFE_DELETE_ARRAY(pMeshContainer->m_pmtxBoneOffset);
	SAFE_DELETE_ARRAY(pMeshContainer->ppBoneMatrixPtrs);

	// �e�X�N�`���[�J��
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
| <<< �q�b�g�{�b�N�X�𓾂� >>>
|	����	iXNum  = X File �ԍ�
|			pFrame = ���[�g�t���[��
|	�����̃��b�V�����傫���𓾂�̂Ő��m�ȑ傫���͓����Ȃ��B
|	  �Ⴆ�΁A�l�Ԃ̃��b�V���̏ꍇ�͎���L������Ԃ̑傫��������
|	  �܂��A�����̃t���[�����L��ꍇ�͔��a�̑傫�Ȃ��̂�D�悷��
*----------------------------------------------------------------------*/
void	CMyD3DApplication::HitWorkGet(int iXNum, LPD3DXFRAME pFrame)
{

	if(pFrame == NULL){ return;}

	LPD3DXMESHCONTAINER		pMeshContainer = pFrame->pMeshContainer;

	while(pMeshContainer !=	NULL)
	{
		//--- �q�b�g���[�N�𓾂� ------------------------------------
		if(pMeshContainer->MeshData.pMesh != NULL)
		{
			// ���炩���ߓ��Ă���q�b�g�{�b�N�X���A�q�b�g�a�n�w�A�q�b�g���𓾂�
			D3DXMESHCONTAINER_DERIVED	*p = (D3DXMESHCONTAINER_DERIVED*)pMeshContainer;
			D3_HIT						*h = &d3.hitWork[iXNum];
			//--- BOX -----------------------------------------------
			if(m_ucHitCount == 0)			// �q�b�g�{�b�N�X�͏���̂ݖ������ɒl��ݒ肷��
			{
				h->vecMin = p->hitWork.vecMin;
				h->vecMax = p->hitWork.vecMax;
			}
			else							// ��x�ڂ��͔�r����
			{
				if(h->vecMin.x > p->hitWork.vecMin.x){ h->vecMin.x = p->hitWork.vecMin.x;}
				if(h->vecMin.y > p->hitWork.vecMin.y){ h->vecMin.y = p->hitWork.vecMin.y;}
				if(h->vecMin.z > p->hitWork.vecMin.z){ h->vecMin.z = p->hitWork.vecMin.z;}
				if(h->vecMax.x < p->hitWork.vecMax.x){ h->vecMax.x = p->hitWork.vecMax.x;}
				if(h->vecMax.y < p->hitWork.vecMax.y){ h->vecMax.y = p->hitWork.vecMax.y;}
				if(h->vecMax.z < p->hitWork.vecMax.z){ h->vecMax.z = p->hitWork.vecMax.z;}
			}
			//--- Sphere --------------------------------------------
			// ���a�̑傫�Ȃ��̂��̗p����
			if(h->fRadius < p->hitWork.fRadius){ h->fRadius = p->hitWork.fRadius;}

			//--- �q�b�g���̒��S���W�����߂� ------------------------
			h->vecCenter += p->hitWork.vecCenter;
			m_ucHitCount++;
		}
		pMeshContainer = pMeshContainer->pNextMeshContainer;
	}
	if(pFrame->pFrameSibling	!= NULL){ HitWorkGet(iXNum, pFrame->pFrameSibling   );}
	if(pFrame->pFrameFirstChild	!= NULL){ HitWorkGet(iXNum, pFrame->pFrameFirstChild);}
}



/*----------------------------------------------------------------------*
| <<< ���b�V���� FVF �ύX���ɂ�т��� >>>
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
											// ���b�V���̑����e�[�u���A�܂��̓��b�V���̑����e�[�u���Ɋi�[����Ă���G���g���̐����擾
	hr = pMeshContainer->MeshData.pMesh->GetAttributeTable(NULL, &pMeshContainer->NumAttributeGroups);
	if(FAILED(hr)){ goto e_Exit;}

	SAFE_DELETE(pMeshContainer->pAttributeTable);
	pMeshContainer->pAttributeTable = new D3DXATTRIBUTERANGE[pMeshContainer->NumAttributeGroups];
	if(!pMeshContainer->pAttributeTable){ hr = E_OUTOFMEMORY; goto e_Exit;}

	hr = pMeshContainer->MeshData.pMesh->GetAttributeTable(pMeshContainer->pAttributeTable, NULL);
	if(FAILED(hr)){ goto e_Exit;}
#endif

#ifndef	__OBBTREE
	//--- �C���f�b�N�X�p���b�g�̃T�C�Y�𓾂� ----------------------
	{
		UINT	MaxMatrices	= SHADER_BONE_MAX;	// SHADER_BONE_MAX �𒴂��Ȃ��悤�ɒ��ӂ��邱�ƁB
		pMeshContainer->m_dwPaletteEntries = min(MaxMatrices, pMeshContainer->pSkinInfo->GetNumBones());

		DWORD	Flags =	D3DXMESHOPT_VERTEXCACHE;
		if(d3.caps.VertexShaderVersion >= D3DVS_VERSION(1, 1)){	Flags |= D3DXMESH_MANAGED;}
		else												  {	Flags |= D3DXMESH_SYSTEMMEM;}

		//--- �d�݌W�����~�b�N�X�����V�������b�V���̍쐬 --------------
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

		//--- SHADER1 �p�� FVF �ύX	-----------------------------------
		DWORD	NewFVF = (pMeshContainer->MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;

		LPD3DXMESH	pMesh;
		pMeshContainer->MeshData.pMesh->CloneMeshFVF(pMeshContainer->MeshData.pMesh->GetOptions(), NewFVF, d3.dev, &pMesh);
		pMeshContainer->MeshData.pMesh->Release();
		pMeshContainer->MeshData.pMesh = pMesh;

		//--- �t�H�[�}�b�g�̐��` --------------------------------------
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
	//--- �{�[���̃o�b�t�@�m�� ------------------------------------
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
| <<< �A�j���[�V�����^�C����ݒ� >>>
|	����	fDat = �R�}��
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
| <<< ���b�V���̕`�� >>>
*----------------------------------------------------------------------*/
#ifdef	__OBBTREE
void	CMyD3DApplication::DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainer, LPD3DXFRAME pD3DXFrame)
{

	bool						bDraw       = false;
	D3DXMESHCONTAINER_DERIVED	*pCD3DXMesh = (D3DXMESHCONTAINER_DERIVED*)pMeshContainer;

	//--- �X�L�����b�V���̏ꍇ ------------------------------------------------
	if(pCD3DXMesh->pSkinInfo)
	{
		MTX		pBoneMatrices[SHADER_BONE_MAX];
		DWORD	NumBones = pCD3DXMesh->pSkinInfo->GetNumBones();
		if(NumBones > SHADER_BONE_MAX){ NumBones = SHADER_BONE_MAX;}
		DWORD	i;

		//--- �s��p���b�g�̍쐬 ------------------------------------
		// ���̎��A���[���h�s�񂾂��폜���邱��
		MTX		*m = &((D3DXFRAME_DERIVED *)pD3DXFrame)->CombinedTransformationMatrix;
		MTX		mtxInv;
		D3DXMatrixInverse(&mtxInv, NULL, m);
		for(i = 0; i < NumBones; i++)
		{
			MTX		*m0 = &pCD3DXMesh->m_pmtxBoneOffset[i];
			MTX		*m1 = pCD3DXMesh->ppBoneMatrixPtrs[ i];
			pBoneMatrices[i] = *m0 * *m1 * mtxInv;
		}

		//--- ���_�o�b�t�@�����b�N���Ē��_���g�����X�t�H�[�� --------
		// ���̏ꍇ�A�t�H�[�}�b�g��ύX�����o���v�Ȃǂɂ͑Ή��s�ɂȂ�܂��B

		PBYTE		pVerticesSrc;
		PBYTE		pVerticesDest;
		pCD3DXMesh->pOrigMesh->LockVertexBuffer(     D3DLOCK_READONLY, (LPVOID*)&pVerticesSrc );
		pCD3DXMesh->MeshData.pMesh->LockVertexBuffer(0,				   (LPVOID*)&pVerticesDest);

		// ���_�ړ�(�v�Z�� CPU �Ȃ̂Œx��)
		pCD3DXMesh->pSkinInfo->UpdateSkinnedMesh(pBoneMatrices, NULL, pVerticesSrc, pVerticesDest);

		// ���_�o�b�t�@�̃��b�N������
		pCD3DXMesh->pOrigMesh->UnlockVertexBuffer(     );
		pCD3DXMesh->MeshData.pMesh->UnlockVertexBuffer();

		//--- �X�L�����b�V���`�� ----------------------------------------------
		d3.eff = d3.lpEffect[m_iShaderType];

		for(i = 0; i < pCD3DXMesh->NumAttributeGroups; i++)
		{
			// �V�F�[�_�[�ʂɕ`��
			bool	(*a)(int iXNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMeshContainer, LPD3DXBONECOMBINATION pBoneComb, MTX *mtx, float fAlp);
			a = (bool(*)(int iXNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMeshContainer, LPD3DXBONECOMBINATION pBoneComb, MTX *mtx, float fAlp))pShaderProc;
			if(a != NULL) (*a)(m_iXNum, i, pCD3DXMesh, NULL, m, m_fAlp);
		}

		// ���[�g�t���[���̍s���ۑ�
		if(iDrawMeshCount == 0) m_vecRoot = VEC3(pBoneMatrices[0]._41, pBoneMatrices[0]._42, pBoneMatrices[0]._43);
	}
	//--- �ʏ�̃��b�V���̏ꍇ ------------------------------------------------
	else
	{
		MTX	*m = &((D3DXFRAME_DERIVED *)pD3DXFrame)->CombinedTransformationMatrix;
		//d3.dev->SetTransform(D3DTS_WORLD, m);

		//--- ���b�V���`�� ------------------------------------------
		d3.eff = d3.lpEffect[m_iShaderType];
		for(DWORD i = 0; i < pCD3DXMesh->NumMaterials; i++)
		{
			// �V�F�[�_�[�ʂɕ`��
			bool	(*a)(int iXNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMeshContainer, LPD3DXBONECOMBINATION pBoneComb, MTX *mtx);
			a = (bool(*)(int iXNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMeshContainer, LPD3DXBONECOMBINATION pBoneComb, MTX *mtx))pShaderProc;
			if(a != NULL) (*a)(m_iXNum, i, pCD3DXMesh, NULL, m);
		}
		// ���[�g�t���[���̍s���ۑ�
		if(iDrawMeshCount == 0)
		{
			m_vecRoot = pCD3DXMesh->hitWork.vecCenter + VEC3(m->_41, m->_42, m->_43);
			m_mtxRoot = *m;
		}
	}

	//--- �`�掞�ɃJ�E���g�A�b�v ------------------------------------
	if(bDraw){ iDrawCount++;}

	//--- DrawMeshContainer ������������ ----------------------------
	iContainerCount++;
}



/*----------------------------------------------------------------------*
| <<< OBBTree �̕`�� >>>
*----------------------------------------------------------------------*/
void	CMyD3DApplication::OBBDrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainer, LPD3DXFRAME pD3DXFrame)
{

	D3DXMESHCONTAINER_DERIVED	*pCD3DXMesh = (D3DXMESHCONTAINER_DERIVED*)pMeshContainer;

	//--- �X�L�����b�V���̏ꍇ ------------------------------------------------
	if(pCD3DXMesh->pSkinInfo)
	{
	}
	//--- �ʏ�̃��b�V���̏ꍇ --------------------------------------
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

	//--- �{�[������ ------------------------------------------------------------------------------
	bool	bDraw = false;					// ��x�ł��`�悵���� 1 ������
	if(pMeshContainer->pSkinInfo !=	NULL)
	{
		if(pMeshContainer->pBoneCombinationBuf == NULL){ return;}
		pBoneComb =	reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());

		for(iAttrib	= 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
		{
			//--- �s��p���b�g�̌v�Z ----------------------------
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
			//--- �����ŕ`�� ------------------------------------
			bool	(*a)(int iXNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMeshContainer, LPD3DXBONECOMBINATION pBoneComb, MTX *mtx, float fAlp);
			a = (bool(*)(int iXNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMeshContainer, LPD3DXBONECOMBINATION pBoneComb, MTX *mtx, float fAlp))pShaderProc;

			if(a != NULL){ bDraw  = (*a)(m_iXNum, iAttrib, pMeshContainer, pBoneComb, &pFrame->CombinedTransformationMatrix, m_fAlp);}
		}
		//--- ���[�g�t���[���̍s���ۑ� ----------------------------
		if(iDrawCount == 0)
		{
			MTX	*m = &pFrame->CombinedTransformationMatrix;
			m_vecRoot = VEC3(m->_41, m->_42, m->_43);
			m_mtxRoot = *m;
		}
	}
	//--- �{�[���Ȃ� ------------------------------------------------------------------------------
	else
	{
		for(iAttrib = 0; iAttrib < pMeshContainer->NumMaterials; iAttrib++)
		{
			//--- �����ŕ`�� ----------------------------------------
			bool	(*a)(int iXNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMeshContainer, LPD3DXBONECOMBINATION pBoneComb, MTX *mtx, float fAlp);
			a = (bool(*)(int iXNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMeshContainer, LPD3DXBONECOMBINATION pBoneComb, MTX *mtx, float fAlp))pShaderProc;
			if(a != NULL) bDraw = (*a)(m_iXNum, iAttrib, pMeshContainer, NULL, &pFrame->CombinedTransformationMatrix, m_fAlp);
		}
		//--- ���[�g�t���[���̍s���ۑ� ----------------------------
		MTX	*m = &pFrame->CombinedTransformationMatrix;
		if(iDrawCount == 0)
		{
			m_vecRoot = pMeshContainer->hitWork.vecCenter + VEC3(m->_41, m->_42, m->_43);
			m_mtxRoot = *m;
		}
	}

	d3.dev->SetVertexShader(NULL);
	d3.dev->SetPixelShader(	NULL);

	//--- ���荞�� --------------------------------------------------
	// �`���Ɋ��荞�܂��̂ŁA�c���Ȃǂ̏����Ɏg�p���Ă��������B
	if(bDraw && pFrameProc != NULL)
	{
		void(*func    )(int iMeshNum, int iNum, D3DXMESHCONTAINER_DERIVED *pMeshContainer, LPD3DXFRAME pFrameBase);
		func = (void(*)(int iMeshNum, int iNum, D3DXMESHCONTAINER_DERIVED *pMeshContainer, LPD3DXFRAME pFrameBase))pFrameProc;
		(*func)(m_iXNum, iDrawCount, pMeshContainer, pFrameBase);
	}
	//--- �`�掞�ɃJ�E���g�A�b�v ------------------------------------
	if(bDraw){ iDrawCount++;}

	//--- DrawMeshContainer ������������ ----------------------------
	iContainerCount++;
}
#endif



/*----------------------------------------------------------------------*
| <<< �K�w�\���ɂȂ��Ă��郁�b�V����`�� >>>
|	����	pFrame = �t���[��
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
| <<< �K�w�\���ɂȂ��Ă��� OBB ��`�� >>>
|	����	pFrame = �t���[��
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
| <<< �K�w�\���ɂȂ��Ă��郁�b�V���R���e�i�[�̖��O�𓾂� >>>
|	����	pFrame    = �t���[��(��ԍŏ��̓��[�g�t���[��)
|			iStrSize  = ���O�̍ő啶����
|			iCount    = �K�w�̃J�E���g
|	�o��	strOutBuf = ���O�� iStrSize ���Ɋi�[
|	���o�b�t�@�� iStrSize * 256 ���炢�͗p�ӂ��Ă�������
*----------------------------------------------------------------------*/
void	CMyD3DApplication::SubsetNameGet(LPD3DXFRAME pFrame, char *strOutBuf, int iStrSize, int iCount)
{

	LPD3DXMESHCONTAINER	pMeshContainer = pFrame->pMeshContainer;

	while(pMeshContainer !=	NULL)
	{										// �o�b�t�@�ɕ������R�s�[����
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
| <<< �{�[���s����Z�b�g >>>
|	����	pMeshContainerBase = ���b�V���̃R���e�i�[
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
| <<< �{�[���̃Z�b�g�A�b�v >>>
|	����	pFrame = �t���[��
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
| <<< �t���[���̍X�V >>>
|	������q�ɂȂ��Ă���ꍇ�͍ċN�Ăяo�������̂Œ���
*----------------------------------------------------------------------*/
void	CMyD3DApplication::UpdateFrameMatrices(LPD3DXFRAME pFrameBase, LPD3DXMATRIX	pParentMatrix)
{
	// �{�[���͈ȉ��̏����Ŏ擾�\�ł��B
	//  
	// �{�[���̎n�_(p0)�ƏI�_(p1)�𓾂�
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

		//--- ���荞�� ------------------------------------------
		// �����͎�̉�]�ȂǂɎg�p���܂��傤
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

	// �Z��̏ꍇ
	if(pFrame->pFrameSibling != NULL)
	{
		UpdateFrameMatrices(pFrame->pFrameSibling, pParentMatrix);
	}
	// �q���̏ꍇ
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
	//pCD3DXMesh->SetName(strBuf);				/* �K�v�͂Ȃ����A�����g���邩��*/
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
|	��OBBTree �ɕK�v�Ȋ֐�
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
| <<< OBBTree �Ƀt���[����ǉ����� >>>
|	����		pFrame      = �t���[��
|	�o��		FrameVector = �����Ƀt���[����񂪒ǉ�����܂�
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
| <<< �����œǂݍ��� >>>
|	����	strFile =  �ǂݍ��݂w�t�@�C����
|	�߂�l	S_OK    == �ǂݍ��ݐ���
|			S_OK    != �ǂݍ��ݎ��s
*----------------------------------------------------------------------*/
HRESULT		CMyD3DApplication::SkinLoad(char *strFile)
{
	
	HRESULT				hr;
	CAllocateHierarchy	Alloc(this);

	//--- ���b�V���̓ǂݍ��� ----------------------------------------
	LPD3DXANIMATIONCONTROLLER	lpAnim;
	hr = D3DXLoadMeshHierarchyFromX(strFile, D3DXMESH_MANAGED, d3.dev, &Alloc, NULL, &m_pFrameRoot, &lpAnim);
	if(FAILED(hr)) return hr;

	//--- �A�j���[�V�����Z�b�g������ꍇ�͓o�^ ----------------------
	if(lpAnim != NULL)
	{
		SKIN_ANIM_WORK	*anim = new SKIN_ANIM_WORK;
		memset(anim, 0, sizeof(SKIN_ANIM_WORK));
		anim->m_pController   = lpAnim;
		m_anim.push_back(anim);
		//iSkinAnimationSet = lpAnim->GetMaxNumAnimationSets();
	}

	//--- OBBTree �̍쐬 --------------------------------------------
#ifdef	__OBBTREE
	if(m_pmtxBone)
	{
	//	D3DXFRAME_DERIVED	*a = (D3DXFRAME_DERIVED *)m_pFrameRoot;

		// �����X�L����OBBTree�ۑ��p�Ɋe�{�[���Ƀ��b�V����t��
		AddMeshtoOneSkinModel(m_pFrameRoot);

		// �e���_���ǂ̃{�[������e�����󂯂Ă��邩���ׂ�
		// ��!!! �{�[�����������b�V������ł��邱�Ƃ�O��ɂ��Ă���
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

		// ���_�擾
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

			// ���_�t�H�[�}�b�g��float ��DWORD�̃f�[�^�����Ȃ��̂�4�o�C�g�P�ʂ�
			// �A�N�Z�X�\����( pf += ( FVFSize - 4 * 3 ) / 4 )
			// �O�̂��߃o�C�g�P�ʂŃA�N�Z�X����
			pf = (float *)((char *)pf + (iVtxSize - 4 * 3));
		}
		pVB->Unlock();

		// 3�p�|���S���擾
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
			return S_FALSE ;				// �C���f�b�N�X���s��
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

			// 3�̒��_���e�����󂯂Ă���{�[���̃E�F�C�g�̑��a�����
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

			// ��ԉe�����󂯂Ă���{�[�����T�[�`
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

			// ��ԉe�����󂯂Ă���{�[����3�p�|���S����U�蕪����
			FACE	face;
			for(i = 0; i < 3; i++)
			{
				// �{�[�����[�J����Ԃ�3�p�|���S����ϊ�
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

	//--- �{�[���̃Z�b�g�A�b�v�@-------------------------------------
	hr = SetupBoneMatrixPointers(m_pFrameRoot);
	if(FAILED(hr)){ return hr;}

	//--- �����܂� --------------------------------------------------
	return S_OK;
}



/*----------------------------------------------------------------------*
| <<< �����ŊJ�� >>>
*----------------------------------------------------------------------*/
HRESULT	CMyD3DApplication::DeleteDeviceObjects(void)
{
	CAllocateHierarchy	Alloc(this);

	D3DXFrameDestroy(m_pFrameRoot, &Alloc);
	m_pFrameRoot = NULL;


	SAFE_DELETE_ARRAY(m_pmtxBone);

	// �A�j���[�V�����R���g���[���[�̊J��
	uint	a = m_anim.size();
	for(uint i = 0; i < a; i++)
	{
		SAFE_RELEASE(m_anim[i]->m_pController);
	}
	m_anim.clear();

	return S_OK;
}



/*----------------------------------------------------------------------*
| <<< �X�L�����b�V���ǂݍ��� >>>
|	����	iXNum	    = X File �ԍ�
|			strfile	    = �ǂݍ��݂w�t�@�C����
|			bLoadAtOnce = true : �����ǂݍ���
|			bBumpLoad   = true : �@���}�b�v�������ɓǂݍ���
|			bFurLoad    = true : �t�@�[�}�b�v�������ɓǂݍ���
|	�߂�l	true or	false
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
		//--- �g���q���Ȃ����̂̓��\�[�X�Ƃ݂Ȃ� --------------------
		d3.meshInfo[iXNum].bResource = true;
		int		i		 = 0;
		while(strFile[i] != NULL)
		{
			if(ISKANJI(strFile[i]))   { i += 2;}
			else if(strFile[i] == '.'){ d3.meshInfo[iXNum].bResource = false; break;}
			else					  { i++;}
		}

		//--- �t�@�C�������쐬 --------------------------------------
		//	�@���\�[�X�̏ꍇ�ɂ͂����R�s�[
		if(!d3.meshInfo[iXNum].bResource)
		{
			char	name[_MAX_FNAME	+ 1];
			char	ext[ _MAX_EXT	+ 1];
			sys_div_path(path, name, ext, strFile);
			sprintf(path, "%s%s%s",	path, name,	ext);
		}
		else{ strcpy(path, strFile);}
	}
	//--- ��ǂ݂̏ꍇ ----------------------------------------------
	else
	{
		strcpy(path, d3.meshInfo[iXNum].strFullPath);	// �t�H���_�[�����R�s�[
	}

	//--- �����A�ǂݍ��݂Ŗ����ꍇ ----------------------------------
	if(!bLoadAtOnce)
	{
		strcpy(d3.meshInfo[iXNum].strFullPath, path);	// �t�@�C�����ۑ�
		d3.meshInfo[iXNum].bLoad = false;
		//d3.meshInfo[iXNum].bBump = bBumpLoad;
		return true;
	}

	//--- �O�񕪂����� ----------------------------------------------
	skin_free(	  iXNum);					// �O��f�[�^������
	d3_xfile_free(iXNum);					// �O��f�[�^������
	
	//--- ���[�N������ ----------------------------------------------
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
	iSkinVertexCount				      = 0;		// ���_�����N���A�[
	iSkinFaceCount					      = 0;		// �ʐ����N���A�[
	skin[iXNum]->m_iID					  = 0;		// �A�j���[�V�����R���g���[���[������
	strcpy(d3.meshInfo[iXNum].strFullPath, path);	// �t�H���_�[�����R�s�[
	d3.meshInfo[iXNum].bSkin			  = true;
	d3.meshInfo[iXNum].bLoad			  = true;
	d3.meshInfo[iXNum].ucSubset			  = 0;
	
	//--- �ǂݍ���	-------------------------------------------------
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
			//--- �q�b�g���[�N�𓾂� ----------------------------------------
			memset(&d3.hitWork[iXNum], 0, sizeof(D3_HIT));
			d3.hitWork[iXNum].vecCenter     = VEC3(0, 0, 0);
			skin[      iXNum]->m_ucHitCount = 0;
			skin[      iXNum]->HitWorkGet(iXNum, skin[iXNum]->m_pFrameRoot);
			d3.hitWork[iXNum].vecCenter    /= (float)skin[iXNum]->m_ucHitCount;

			// HitWorkGet �ł� min �� max �̒l�������Ă��Ȃ��̂ŁA������ BOX ���쐬
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
			d3.meshInfo[iXNum].ucSubset	= skin_subset_get(iXNum);		// �T�u�Z�b�g��(�w�t�@�C���m�F�c�[���ȂǂŎg�p)
			d3.meshInfo[iXNum].usVertex	= iSkinVertexCount;				// ���_���𓾂�
			d3.meshInfo[iXNum].usFaces	= iSkinFaceCount;				// �ʐ��𓾂�
																		// �A�j���[�V�����g���b�N��
			if(skin[iXNum]->m_anim.size() > 0){ d3.meshInfo[iXNum].ucSkinAnimationSet = skin[iXNum]->m_anim[0]->m_pController->GetMaxNumAnimationSets();}
			else							  { d3.meshInfo[iXNum].ucSkinAnimationSet = 0;}
																		// �^�C���X�^���v�o�^
			sys_timestamp_get(&d3.meshInfo[iXNum].ftLastWrite, d3.meshInfo[iXNum].strFullPath);
			return true;
		}
		else
		{
			sys_error("skin_load: %s ���ǂݍ��߂܂���ł����B", strFile);
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
| <<< �X�L�����b�V���ǂݍ��� >>>
|	����	iXNum	    = X File �ԍ�
|			strFile	 = �w�t�@�C����
|	�߂�l	true or	false
*----------------------------------------------------------------------*/
bool	skin_load(int iXNum, char *strFile)
{
	return	skin_load(iXNum, strFile, !sys.bAfterLoad, false, false);
}



/*----------------------------------------------------------------------*
| <<< �X�L�����b�V���J�� >>>
|	����	iXNum = X File �ԍ�
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

		// �q�b�g���[�N�N���A�[
		memset(&d3.hitWork[iXNum], 0, sizeof(D3_HIT));
	}
}



/*----------------------------------------------------------------------*
| <<< �X�L�����b�V���S�J�� >>>
*----------------------------------------------------------------------*/
void	skin_end(void)
{
	for(int	i =	0; i < D3_MESH_MAX; i++) skin_free(i);
}



/*----------------------------------------------------------------------*
| <<< �A�j���[�V�����^�C����i�߂� >>>
|	����	iXNum =	XFILE �ԍ�
|			iANum = �A�j���[�V�����R���g���[���[�ԍ� 
|			fTime =	�i�߂鎞��
*----------------------------------------------------------------------*/
/*void	skin_frame_set(int iXNum, int iANum, float fTime)
{
	if(skin[iXNum]				  == NULL       ){ return;}	// �w�t�@�C�����ǂݍ���
	if(skin[iXNum]->m_anim.size() <= (uint)iANum){ return;}	// �s���ȃA�j���[�V�����ԍ��̏ꍇ
	
	skin[iXNum]->m_iAnim = iANum;
	skin[iXNum]->FrameMove(fTime);
}*/



/*----------------------------------------------------------------------*
| <<< �A�j���[�V�����^�C����i�߂� >>>
|	����	iXNum =	XFILE �ԍ�
|			fTime =	�i�߂鎞��
*----------------------------------------------------------------------*/
void	skin_anim_move(int iXNum, float fTime)
{
	//if(skin[iXNum] == NULL){ return;}		// ���ǂݍ���
	//skin_frame_set(iXNum, skin[iXNum]->m_iAnim, fTime);

	if(skin[iXNum] == NULL){ return;}		// �w�t�@�C�����ǂݍ���
	skin[iXNum]->FrameMove(fTime);
}

void	skin_frame_set(int iXNum, float fTime)
{
	skin_anim_move(iXNum, fTime);
}



/*----------------------------------------------------------------------*
| <<< �A�j���[�V�����^�C�������Z�b�g>>>
|	����	iXNum =	XFILE �ԍ�
*----------------------------------------------------------------------*/
void	skin_anim_reset(int iXNum)
{

	if(skin[iXNum] == NULL){ return;}		// ���ǂݍ���

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
| <<< �A�j���[�V���� ID �ύX >>>			���ЂƂ̃��f���𕡐��\������Ƃ��Ɏg�p�̂���
|	����	iXNum =	XFILE �ԍ�
|			iID   = �h�c�ԍ�
*----------------------------------------------------------------------*/
void	skin_id_change(int iXNum, int iID)
{

	if(skin[iXNum] == NULL){ return;}
	skin[iXNum]->m_iID = iID;

	// �A�j���[�V�����g���b�N���𒲂ׂ�
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
				anim->m_pController->SetTrackPosition(0, 0);	// �����ƃg���b�N�ƃ^�C�������Z�b�g���܂��傤�B
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
| <<< �A�j���[�V�����g���b�N�ύX >>>
|	����	iXNum     =	XFILE �ԍ�
|			iTrackNum =	�g���b�N�ԍ�
|			fFadeTime =	�t�F�[�h����
|			bReset	  =	�؂�ւ���A�j���[�V��������R�}�ڂ���Đ����邩
|	�߂�l	true : �g���b�N�ύX����
*----------------------------------------------------------------------*/
bool	skin_anim_set(int iXNum, int iTrackNum, float fFadeTime, bool bReset)
{

	CMyD3DApplication	*s = skin[iXNum];


	if(s ==	NULL){ return false;}			// ���ǂݍ���
											// �{�[���Ȃ�
	if(s->m_pmtxBone == NULL){ return false;}

	SKIN_ANIM_WORK	*a = s->m_anim[s->m_iID];
	if(s->m_anim.size() < (uint)s->m_iID){ return false;}

	if(a->m_iTrack							 == iTrackNum) return false;
	if(d3.meshInfo[iXNum].ucSkinAnimationSet <= iTrackNum) return false;

	int		iCurrect = a->m_iCurrent++;
	int		iNext	 = a->m_iCurrent & 1;

	if(fFadeTime == 0){ fFadeTime = 0.01f;}				// .net2008 �ł́A���̍s�������ƈȉ��Ŗ������[�v�ɓ����Ă��܂��܂�

	double	dFade	 = (double)fFadeTime / 30.0;		// �A�j���[�V�����~�L�V���O����

	LPD3DXANIMATIONSET			as;
	LPD3DXANIMATIONCONTROLLER	c = a->m_pController;

	c->GetAnimationSet(	    a->m_iTrack, &as);
	c->SetTrackAnimationSet(iCurrect,     as);
	as->Release();
	c->GetAnimationSet(	    iTrackNum,	 &as);
	c->SetTrackAnimationSet(iNext,		  as);
	as->Release();

	// dFade �� 0 �̏ꍇ�A�������[�v�ɓ���܂�

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
| <<< �A�j���[�V�����g���b�N�ύX >>>
|	����	iXNum     =	XFILE �ԍ�
|			iTrackNum =	�g���b�N�ԍ�
|			fFadeTime =	�t�F�[�h����
|	�߂�l	true : �g���b�N�ύX����
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
| <<< �����_�����O >>>
|	����	*mtxWorld = ���[���h�s��
*----------------------------------------------------------------------*/
HRESULT	CMyD3DApplication::Render(MTX *mtxWorld)
{

	iUpdateCount = 0;						// �A�b�v�f�[�g���N���A�[
	UpdateFrameMatrices(m_pFrameRoot, mtxWorld);
	DrawFrame(m_pFrameRoot);
	return true;
}


#ifdef	__OBBTREE
/*----------------------------------------------------------------------*
| <<< OBBBox �`�� >>>
|	����	*mtxWorld = ���[���h�s��
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
| <<< ���[�g�̖��O�𓾂� >>>
|	����	iXNum    = XFILE �ԍ�
|			iStrSize = ���O�̍ő啶����
|	�o��	strOutBuf = ���O�� iStrSize ���Ɋi�[
|	���o�b�t�@�� iStrSize * 256 ���炢�͗p�ӂ��Ă�������
*----------------------------------------------------------------------*/
void	skin_root_name_get(int iXNum, char *strOut, int iStrSize)
{
	if(skin[iXNum] == NULL){ return;}
	skin[iXNum]->SubsetNameGet(skin[iXNum]->m_pFrameRoot, strOut, iStrSize, 0);
}




/*----------------------------------------------------------------------*
| <<< �T�u�Z�b�g���𐔂��� >>>
|	������q�ɂȂ��Ă���ꍇ�͍ċN�Ăяo�������̂Œ���
*----------------------------------------------------------------------*/
void	CMyD3DApplication::MaterialCountGet(LPD3DXFRAME lpFrame, uchar *ucCount)
{
	LPD3DXMESHCONTAINER			lpMeshContainer        = lpFrame->pMeshContainer;
	//D3DXMESHCONTAINER_DERIVED	*pMeshContainerDerived = (D3DXMESHCONTAINER_DERIVED*)lpMeshContainer;

	while(lpMeshContainer != NULL)
	{
		if(lpMeshContainer->MeshData.Type == D3DXMESHTYPE_MESH)
		{									// �����ŃT�u�Z�b�g���𑝂₵�Ă��܂�
			//*ucCount = *ucCount + (char)pMeshContainerDerived->NumMaterials;
			*ucCount = *ucCount + (char)lpMeshContainer->NumMaterials;
		}
		lpMeshContainer = lpMeshContainer->pNextMeshContainer;
	}

	if(lpFrame->pFrameSibling    != NULL){ MaterialCountGet(lpFrame->pFrameSibling,	ucCount);}
	if(lpFrame->pFrameFirstChild != NULL){ MaterialCountGet(lpFrame->pFrameFirstChild, ucCount);}
}



/*----------------------------------------------------------------------*
| <<< �T�u�Z�b�g���𓾂� >>>
|	����	iXNum    = XFILE �ԍ�
|			iType	 = ���_�V�F�[�_�[�̌^
|			mtxWorld = ���[���h���W
|			fAlp	 = ���������[�g
|	�߂�l	true  :	�`�悵���i�t���[���X�L�b�v���܂ށj
|			false :	�`�悵�Ȃ�����
*----------------------------------------------------------------------*/
int		skin_subset_get(int iXNum)
{
	skin[iXNum]->m_ucSubset = 0;
	skin[iXNum]->MaterialCountGet(skin[iXNum]->m_pFrameRoot, &skin[iXNum]->m_ucSubset);
	return (int)skin[iXNum]->m_ucSubset;
}




/*----------------------------------------------------------------------*
| <<< �`�� >>>
|	����	iXNum    = XFILE �ԍ�
|			iType	 = ���_�V�F�[�_�[�̌^
|			mtxWorld = ���[���h���W
|			fAlp	 = ���������[�g
|	�߂�l	true  :	�`�悵���i�t���[���X�L�b�v���܂ށj
|			false :	�`�悵�Ȃ�����
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

	//--- �ǂݍ���ł��Ȃ����̂͂����őΉ����� ----------------------
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
		return false;						// ���ǂݍ���
	}

	iDrawCount				   = 0;			// ���b�V���̕`��v���p(�P�t���[�����ɃJ�E���g�A�b�v���Ă����܂�)
	skin[iXNum]->m_iShaderType = iType;
	skin[iXNum]->m_fAlp        = fAlp;
	skin[iXNum]->Render(mtxWorld);

#ifdef	__OBBTREE
	iDrawMeshCount             = 0;			// ���b�V���̕`��v���p(�P�t���[�����ɃJ�E���g�A�b�v���Ă����܂�)
	skin[iXNum]->OBBRender(mtxWorld);
#endif
											// ���_���v�Z
	d3.iPolyCount += d3.meshInfo[iXNum].usFaces;

	d3_shader_set(D3_SHADER_TYPE_OFF);
	return true;
}


/*----------------------------------------------------------------------*
| <<< �`�� >>>
|	����	iXNum       = XFILE �ԍ�
|			iShaderType = ���_�V�F�[�_�[�̌^
|			mtxWorld    = ���[���h���W
|			fAlp	    = ���������[�g
|	�߂�l	true  :	�`�悵���i�t���[���X�L�b�v���܂ށj
|			false :	�`�悵�Ȃ�����
*----------------------------------------------------------------------*/
bool	skin_draw(int iXNum, int iShaderType, MTX *mtxWorld, float fAlp)
{
	d3_shader_set(iShaderType);
	return	skin_draw2(iXNum, iShaderType, mtxWorld, fAlp);
}


/*----------------------------------------------------------------------*
| <<< �`�� >>>
|	����	iXNum       = XFILE �ԍ�
|			iShaderType = ���_�V�F�[�_�[�̌^
|			mtxWorld    = ���[���h���W
|			fAlp	    = ���������[�g
|	�߂�l	true  :	�`�悵���i�t���[���X�L�b�v���܂ށj
|			false :	�`�悵�Ȃ�����
*----------------------------------------------------------------------*/
bool	skin_draw(int iXNum, int iShaderType, MTX *mtxWorld)
{
	d3_shader_set(iShaderType);
	return	skin_draw2(iXNum, iShaderType, mtxWorld, 1);
}


/*----------------------------------------------------------------------*
| <<< �`�� >>>
|	����	iXNum     =	XFILE �ԍ�
|			iShaderType = ���_�V�F�[�_�[�̌^
|			mtxWorld    = ���[���h���W
|			fAlp	    = ���������[�g
|	�߂�l	true  :	�`�悵���i�t���[���X�L�b�v���܂ށj
|			false :	�`�悵�Ȃ�����
*----------------------------------------------------------------------*/
bool	skin_draw(int iXNum, MTX *mtxWorld)
{
	d3_shader_set(D3_SHADER_TYPE_NORMAL);
	return skin_draw2(iXNum, D3_SHADER_TYPE_NORMAL, mtxWorld, 1);
}


/*----------------------------------------------------------------------*
| <<< �`�� >>>
|	����	iXNum    = XFILE �ԍ�
|			mtxWorld = ���[���h���W
|			fAlp	 = �������l
|	�߂�l	true  :	�`�悵���i�t���[���X�L�b�v���܂ށj
|			false :	�`�悵�Ȃ�����
*----------------------------------------------------------------------*/
bool	skin_draw(int iXNum, MTX *mtxWorld, float fAlp)
{
	d3_shader_set(D3_SHADER_TYPE_NORMAL);
	return	skin_draw2(iXNum, D3_SHADER_TYPE_NORMAL, mtxWorld, fAlp);
}


/*----------------------------------------------------------------------*
| <<< �P�p�[�c�̂ݕ`�� >>>
|	����	iXNum       = XFILE �ԍ�
|			iParts      = �p�[�c�ԍ�
|			iShaderType = �V�F�[�_�[�^�C�v
|			mtxWorld    = �z�u���W
|			fAlp	    = �A���t�@�l(0.f �` 1.f)
|	�߂�l	true  :	�`�悵���i�t���[���X�L�b�v���܂ށj
|			false :	�`�悵�Ȃ�����
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
	//--- �ǂݍ���ł��Ȃ����̂͂����őΉ����� ----------------------
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
		return false;						// ���ǂݍ���
	}

	iDrawCount				   = 0;			// ���b�V���̕`��v���p(�P�t���[�����ɃJ�E���g�A�b�v���Ă����܂�)
	skin[iXNum]->m_iShaderType = iShaderType;
	skin[iXNum]->m_fAlp        = fAlp;

	//skin[iXNum]->Render(mtxWorld);

	iUpdateCount = 0;						// �A�b�v�f�[�g���N���A�[
	skin[iXNum]->UpdateFrameMatrices(skin[iXNum]->m_pFrameRoot, mtxWorld);
	skin[iXNum]->DrawFrame(skin[iXNum]->m_pFrameRoot, 0, iParts);

#ifdef	__OBBTREE
	iDrawMeshCount             = 0;			// ���b�V���̕`��v���p(�P�t���[�����ɃJ�E���g�A�b�v���Ă����܂�)
	skin[iXNum]->OBBRender(mtxWorld);
#endif
											// ���_���v�Z
	d3.iPolyCount += d3.meshInfo[iXNum].usFaces;

	d3_shader_set(D3_SHADER_TYPE_OFF);
	return true;

}


/*----------------------------------------------------------------------*
| <<< ���[�g�̍s���Ԃ� >>>
|	����	iXNum   =	XFILE �ԍ�
|	�߂�l	���[�g�̍s��
*----------------------------------------------------------------------*/
MTX		*skin_root_matrix_get(int iXNum)
{
	if(skin[iXNum] == NULL){ return &d3.mtxIdentity;}
	return	&skin[iXNum]->m_mtxRoot;
}


/*----------------------------------------------------------------------*
| <<< �A�j���[�V��������Ԃ� >>>
|	����	iXNum     =	XFILE �ԍ�
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
| <<< �X�L�����b�V����ǂݍ���ł��邩�`�F�b�N���� >>>
|	����	iXNum     =	XFILE �ԍ�
|	�߂�l	true  :	�ǂݍ���ł���
|			false :	�ǂݍ���ł��Ȃ�
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
| <<< �X�L�����b�V���g�p���̊��荞�ݏ����ݒ� >>>
|	����	pProc = ���荞�݊֐�
*----------------------------------------------------------------------*/
void	skin_skin_proc_set(void *pProc)
{
	pSkinProc = pProc;
}


/*----------------------------------------------------------------------*
| <<< �X�L�����b�V���`�掞�̎c�������Ȃǂ�o�^���� >>>	���{�[�����g�p��
|	����	pProc = ���荞�݊֐�
*----------------------------------------------------------------------*/
void	skin_frame_proc_set(void *pProc)
{
	pFrameProc = pProc;
}


/*----------------------------------------------------------------------*
| <<< �X�L�����b�V�����m�A����̃{�b�N�X���m�̓����蔻�� >>>
|	����	a, b = �X�L�����b�V���ǂݍ��ݔԍ�
|			iANum, iBNum = �{�b�N�X�ԍ�( -1 : �S�Ẵ{�b�N�X���Ώ�)
*----------------------------------------------------------------------*/
bool	skin_hit_check(int *pAOut, int *pBOut, int a, int iANum, int b, int iBNum)
{

	//--- ���b�V�������݂��邩�`�F�b�N ------------------------------
	if(skin[a] == NULL || skin[b] == NULL) return false;

#ifdef	__OBBTREE

	//--- �`�F�b�N�J�n ----------------------------------------------
	vector< D3DXFRAME *>FrameAVector, FrameBVector;

	skin[a]->GetFrame(skin[a]->m_pFrameRoot, FrameAVector);
	skin[b]->GetFrame(skin[b]->m_pFrameRoot, FrameBVector);

	int		iACount;						// �q�b�g BOX �̌v���p
	int		iBCount = -1;					// ���[�v�� i, j �̓t���[�����ł���A�t���[������ OBBTree ���܂܂�Ȃ����̂�����̂Œ���

	for(uint j = 0; j < FrameBVector.size(); j++)
	{
											// ���b�V���� OBBTree �����邩���ׂ�
		D3DXMESHCONTAINER_DERIVED	*pCD3DXMeshB = (D3DXMESHCONTAINER_DERIVED *)FrameBVector[j]->pMeshContainer;
		if(pCD3DXMeshB           == NULL) continue;
		if(pCD3DXMeshB->pOBBTree == NULL) continue;
		iBCount++;
		if(iBNum != -1 && iBNum != iBCount) continue;

		iACount = -1;
		for(uint i = 0; i < FrameAVector.size(); i++)
		{
											// ���b�V���� OBBTree �����邩���ׂ�
			D3DXMESHCONTAINER_DERIVED	*pCD3DXMeshA = (D3DXMESHCONTAINER_DERIVED *)FrameAVector[i]->pMeshContainer;
			if(pCD3DXMeshA           == NULL) continue;
			if(pCD3DXMeshA->pOBBTree == NULL) continue;
			iACount++;
			if(iANum != -1 && iANum != iACount) continue;

			//---- �R���W���� ---------------------------------------
			D3DXFRAME_DERIVED *a = (D3DXFRAME_DERIVED *)FrameAVector[i];
			D3DXFRAME_DERIVED *b = (D3DXFRAME_DERIVED *)FrameBVector[j];

			if(COBBTree().CheckCollision(*pCD3DXMeshA->pOBBTree, a->CombinedTransformationMatrix, 
										 *pCD3DXMeshB->pOBBTree, b->CombinedTransformationMatrix))
			{
				*pAOut = iACount;
				*pBOut = iBCount;
				return true;
			}
			// �t�����b�e�~��(�R���W�������s���m�Ȃ��߈ꎞ�I�ȏ����ł��B�����I�ɂȂ��Ȃ�܂�)
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
| <<< �X�L�����b�V�����m�̓����蔻�� >>>
|	����	a, b = �X�L�����b�V���ǂݍ��ݔԍ�
*----------------------------------------------------------------------*/
bool	skin_hit_check(int a, int b)
{
	int	dumA, dumB;
	return	skin_hit_check(&dumA, &dumB, a, -1, b, -1);
}


/*----------------------------------------------------------------------*
| <<< OBBTree �̎w��� BOX �̒��S�𓾂� >>>
|	����	iXNum   = XFILE
|			iBoxNum = �{�b�N�X�ԍ�( -1 �͕s��)
*----------------------------------------------------------------------*/
VEC3	*skin_hit_pos_get(int iXNum, int iBoxNum)
{

	static	VEC3	vecRet;

	vecRet = VEC3(0, 0, 0);

	//--- ���b�V�������݂��邩�`�F�b�N ------------------------------
	if(skin[iXNum] == NULL) return &vecRet;

#ifdef	__OBBTREE

	//--- �`�F�b�N�J�n ----------------------------------------------
	vector< D3DXFRAME *>FrameAVector;

	skin[iXNum]->GetFrame(skin[iXNum]->m_pFrameRoot, FrameAVector);

	int		iACount = -1;					// �q�b�g BOX �̌v���p

	iACount = -1;
	//uint	aaa =FrameAVector.size();
	for(uint i = 0; i < FrameAVector.size(); i++)
	{
										// ���b�V���� OBBTree �����邩���ׂ�
		D3DXMESHCONTAINER_DERIVED	*pCD3DXMeshA = (D3DXMESHCONTAINER_DERIVED *)FrameAVector[i]->pMeshContainer;
		if(pCD3DXMeshA           == NULL) continue;
		if(pCD3DXMeshA->pOBBTree == NULL) continue;
		iACount++;
		if(iBoxNum!= iACount) continue;

		//--- BOX �̒��S���W�����܂�
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
| <<< OBBTree �̃{�b�N�X�\���ݒ� >>>
|	����	iXNum =	XFILE �ԍ�
|			bSw   = true : OBBTree �̃{�b�N�X��`��
*----------------------------------------------------------------------*/
void	skin_obbtree_box_set(int iXNum, bool bSw)
{
#ifdef	__OBBTREE
	if(skin[iXNum] == NULL) return;
	skin[iXNum]->m_bOBBBox = bSw;
#endif
}


/*----------------------------------------------------------------------*
| <<< OBBTree �̃{�b�N�X�`��ԍ��ݒ� >>>
|	����	iXNum   = XFILE �ԍ�
|			iOBBNum = OBB �ԍ� (-1 �őS�`��)
*----------------------------------------------------------------------*/
void	skin_obbtree_box_draw_num_set(int iXNum, int iOBBNum)
{
#ifdef	__OBBTREE
	if(skin[iXNum] == NULL) return;
	skin[iXNum]->m_iOBBBoxNum = iOBBNum;
#endif
}


/*----------------------------------------------------------------------*
| <<< OBBTree �̃{�b�N�X�����𓾂� >>>	���`��łȂ��ƒl���Ԃ���܂���
|	����	iXNum   = XFILE �ԍ�
|	�߂�l	OBBTree �� BOX ����
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
| <<< �X�L�����b�V���̃t���[���A�b�v�f�[�g���𓾂� >>>
|	����	iNum	 = �\���ԍ�
|	�߂�l	�t���[����
*----------------------------------------------------------------------*/
int		skin_frame_update_count_get(int iNum)
{

#if	__3D_DRAW_TEST
	return 0;
#endif

	if(skin[iNum] == NULL) return 0;

	iUpdateCount = 0;						// �A�b�v�f�[�g���N���A�[
//	skin[iNum]->FrameMatricesCountGet(skin[iNum]->m_pFrameRoot, &d3.mtxIdentity);
	return iUpdateCount;
}




#endif			// __SKIN
