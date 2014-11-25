//
//	�V�F�[�_�[����
//
//		���V�F�[�_�[�Ăяo���̂b���ꑤ



//--- �V�F�[�_�[�e�[�u�� --------------------------------------------------------------------------
void	*pShaderTbl[D3_SHADER_TYPE_MAX] =
{
	shader_normal,
};



/**************************************************************************************************
	[[[	���_�V�F�[�_�[�E�s�N�Z���V�F�[�_�[�֌W ]]]
**************************************************************************************************/

/*----------------------------------------------------------------------*
| <<< �o���v�`�F�b�N >>>
|	�߂�l	true : �o���v���T�|�[�g���Ă���
*----------------------------------------------------------------------*/
static	bool	IsSupportsBumpMapping(void)
{

	D3DCAPS9	d3dCaps;

	d3.dev->GetDeviceCaps(&d3dCaps);
											// ���̃f�o�C�X�� 2	�̃o���v �}�b�s���O �u�����h�������T�|�[�g���Ă��邩?
	if(0 ==	(d3dCaps.TextureOpCaps & (D3DTEXOPCAPS_BUMPENVMAP |	D3DTEXOPCAPS_BUMPENVMAPLUMINANCE)))	return false;
											// ���̃f�o�C�X�� 3	�܂ł̃u�����f�B���O �X�e�[�W���T�|�[�g���Ă��邩?
	if(d3dCaps.MaxTextureBlendStages < 3) return false;
	return true;
}



/*----------------------------------------------------------------------*
| <<< ���_�V�F�[�_�[�R���p�C�� >>>
|	����	uiNum = �V�F�[�_�[�ԍ�
*----------------------------------------------------------------------*/
void	shader_compile(uint uiNum)
{

	//--- �G���[�`�F�b�N --------------------------------------------
	if(d3.shaderInfo[uiNum].bError){ return;}	// ��x�ł��G���[���o�Ă���ꍇ�́A�ȍ~���������Ȃ�

	//--- �o���v�`�F�b�N --------------------------------------------
	//if(uiNum == SHADER_TYPE_BUMP && !IsSupportsBumpMapping()){ return;}

	//--- HLSL �t�H���_�[�̃R���p�C�� -------------------------------
	sys_folder_push();
	sys_folder_set(NULL);

	char	file[MAX_PATH +	1];
	sprintf(file, "hlsl\\%02d.fx", uiNum - D3_SHADER_TYPE_NORMAL);

	char	path[_MAX_PATH  + 1];
	char	name[_MAX_FNAME	+ 1];
	char	ext[ _MAX_EXT	+ 1];
	sys_div_path(path, name, ext, file);
											// path �Ƀt�@�C����������܂�
	sprintf(path, "%s%s%s", path, name, ext);
											// ��x�t�@�C�����J���āA���݂��m�F
	HANDLE	handle = CreateFile(path, GENERIC_READ,	0, NULL, OPEN_EXISTING,	FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS, NULL);
	bool	bLoad  = false;
	if(handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(handle);
		
		SAFE_RELEASE(d3.lpEffect[uiNum - D3_SHADER_TYPE_NORMAL]);
											// �����Ȃ�΁A�ǂݍ��܂Ȃ��ŏI��
		if(strcmp(d3.shaderInfo[uiNum].strFullPath, path) == 0) return;
											// ���ۂɓǂݍ���
		LPD3DXBUFFER	pBuf;
//@@
		//HRESULT	hr = D3DXCreateEffectFromFile(d3.dev, path,	NULL, NULL, D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION, NULL, &d3.lpEffect[uiNum - D3_SHADER_TYPE_NORMAL], &pBuf);
		HRESULT	hr = D3DXCreateEffectFromFile(d3.dev, path,	NULL, NULL, 0, NULL, &d3.lpEffect[uiNum - D3_SHADER_TYPE_NORMAL], &pBuf);
											// �t���p�X�o�^
		strcpy(d3.shaderInfo[uiNum].strFullPath, path);
											// �^�C���X�^���v�o�^
		sys_timestamp_get(&d3.shaderInfo[uiNum].ftLastWrite, d3.shaderInfo[uiNum].strFullPath);
											// ���\�[�X�X�C�b�`�I�t
		d3.shaderInfo[uiNum].bResource = false;
		if(FAILED(hr))
		{
			char	str[512];
			if(pBuf == NULL)
			{
				sprintf(str, "%s ������܂���B", path);
			}
			else
			{
				char	*a = (char*)pBuf->GetBufferPointer();
				sprintf(str, "%s", a);
			}
			sys_error(str);
			d3.shaderInfo[uiNum].bError = true;
			FOLDER_POP();
			return;
		}
		else
		{
			bLoad = true;					// ���[�h�����B�ȍ~�A���\�[�X�̏����͂��Ȃ�
		}
	}

	//--- ���\�[�X�̃V�F�[�_�[�R���p�C�� ----------------------------
#ifdef	HLSL00
	if(!bLoad)
	{
		d3.shaderInfo[uiNum].bResource = true;
		SAFE_RELEASE(d3.lpEffect[uiNum - D3_SHADER_TYPE_NORMAL]);
		LPD3DXBUFFER	pBuf;
		HRESULT			hr = D3DXCreateEffectFromResource(d3.dev, NULL, MAKEINTRESOURCE(HLSL00 + uiNum), NULL, NULL, 0, NULL, &d3.lpEffect[uiNum - D3_SHADER_TYPE_NORMAL], &pBuf);
		if(FAILED(hr))
		{
			SYS_END(  " %d �Ԃ̂e�w�t�@�C���쐬���s", uiNum - D3_SHADER_TYPE_NORMAL);
			sys_error(" %d �Ԃ̂e�w�t�@�C���쐬���s", uiNum - D3_SHADER_TYPE_NORMAL);
			d3.shaderInfo[uiNum].bError = true;
			if(pBuf != NULL)
			{
				char	str[512];
				char	*a = (char*)pBuf->GetBufferPointer();
				sprintf(str, "%s", a);
				sys_printf(str);
				sys_printf(str);
			}
		}
	}
#endif
	sys_folder_pop();
}



bool	shader_compile(LPD3DXEFFECT *lpEff, char *strFile)
{

	//--- ��x�t�@�C�����J���āA���݂��m�F --------------------------
	HANDLE	handle = CreateFile(strFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS, NULL);
	if(handle == INVALID_HANDLE_VALUE){ return false;}
	CloseHandle(handle);

	//--- HLSL �t�H���_�[�̃R���p�C�� -------------------------------
	LPD3DXBUFFER	pBuf; 
	HRESULT	hr = D3DXCreateEffectFromFile(d3.dev, strFile, NULL, NULL, 0, NULL, lpEff, &pBuf);

	if(FAILED(hr))
	{
		if(pBuf == NULL){ return sys_error("%s �� HLSL �t�@�C���ł͂���܂���B", strFile);}
		return sys_error("%s", (char*)pBuf->GetBufferPointer());
	}
	return true;
}

bool	shader_legacy_compile(LPD3DXEFFECT *lpEff, char *strFile)
{

	//--- ��x�t�@�C�����J���āA���݂��m�F --------------------------
	HANDLE	handle = CreateFile(strFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS, NULL);
	if(handle == INVALID_HANDLE_VALUE) return false;
	CloseHandle(handle);

	//--- HLSL �t�H���_�[�̃R���p�C�� -------------------------------
	LPD3DXBUFFER	pBuf; 
#ifndef	__DX90B
	HRESULT	hr = D3DXCreateEffectFromFile(d3.dev, strFile, NULL, NULL, D3DXSHADER_USE_LEGACY_D3DX9_31_DLL, NULL, lpEff, &pBuf);
#else
	HRESULT	hr = D3DXCreateEffectFromFile(d3.dev, strFile, NULL, NULL,                                  0, NULL, lpEff, &pBuf);
#endif
	if(FAILED(hr))
	{
		if(pBuf == NULL) return sys_error("%s �� HLSL �t�@�C���ł͂���܂���B", strFile);
		return sys_error("%s", (char*)pBuf->GetBufferPointer());
	}
	return true;
}



/*----------------------------------------------------------------------*
| <<< �V�F�[�_�[��� >>>
|	����	uiType = �V�F�[�_�[�^�C�v
*----------------------------------------------------------------------*/
void	shader_free(uint uiType)
{
	SAFE_RELEASE(d3.lpEffect[uiType - D3_SHADER_TYPE_NORMAL]);
	memset(&d3.shaderInfo[uiType], 0, sizeof(D3_INFO));
	d3.lpEffect[uiType] = NULL;
}



/*----------------------------------------------------------------------*
| <<< �V�F�[�_�[��� >>>
*----------------------------------------------------------------------*/
void	shader_free(void)
{
	for(int	i =	D3_SHADER_TYPE_NORMAL; i < D3_SHADER_TYPE_MAX; i++){ shader_free(i);}
}



/*----------------------------------------------------------------------*
| <<< �V�F�[�_�[���Z�b�g >>>
*----------------------------------------------------------------------*/
void	shader_reset(void)
{
	for(int	i =	D3_SHADER_TYPE_NORMAL; i < D3_SHADER_TYPE_MAX; i++)
	{
		if(d3.lpEffect[i - D3_SHADER_TYPE_NORMAL] != NULL){ d3.lpEffect[i - D3_SHADER_TYPE_NORMAL]->OnResetDevice();}
	}
}



/*----------------------------------------------------------------------*
| <<< �V�F�[�_�[�ݒ� >>>
|	����	iType =	���_�V�F�[�_�[�^�C�v
|	�߂�l	false : �V�F�[�_�[�͖����ł���
*----------------------------------------------------------------------*/
bool	shader_set(int iType)
{

	//--- �I�t ------------------------------------------------------
	if(iType == D3_SHADER_TYPE_OFF)
	{
		d3_stat_zread_set(	true);
		d3_stat_zwrite_set(	true);
		d3_stat_culling_set(true);
		d3_stat_blend_set(     D3_BLEND_NORMAL);
		d3.dev->SetRenderState(D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA);
		d3.dev->SetRenderState(D3DRS_DESTBLEND,	D3DBLEND_INVSRCALPHA);
		d3.dev->SetRenderState(D3DRS_WRAP0,		0);
		d3.dev->SetRenderState(D3DRS_WRAP1,		0);
		d3.dev->SetTextureStageState(0,	D3DTSS_COLOROP,	  D3DTOP_MODULATE);
		d3.dev->SetTextureStageState(0,	D3DTSS_COLORARG1, D3DTA_TEXTURE);
		d3.dev->SetTextureStageState(0,	D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		d3.dev->SetTextureStageState(1,	D3DTSS_COLOROP,	  D3DTOP_DISABLE);
		d3.dev->SetTextureStageState(0,	D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		d3.dev->SetVertexShader(NULL);
		d3.dev->SetPixelShader(	NULL);
		return true;
	}

	//--- �V�F�[�_�[���R���p�C������Ă��Ȃ��ꍇ�̓R���p�C�� -------
	if(d3.lpEffect[iType] == NULL)
	{
		shader_compile(iType);
		if(d3.lpEffect[iType] == NULL){ return false;}
		d3.bReRender = true;
	}

	//--- �V�F�[�_�[���Z�b�g ----------------------------------------
	d3.eff = d3.lpEffect[iType];

	return true;
}



/*----------------------------------------------------------------------*
| <<< �V�F�[�_�[�̓o�^�ő吔�𓾂� >>>
|	�߂�l	�V�F�[�_�[�̓o�^��
*----------------------------------------------------------------------*/
int		shader_max_get(void)
{
	return D3_SHADER_TYPE_MAX;
}


/*----------------------------------------------------------------------*
| <<< ���O�V�F�[�_�[�̓o�^ >>>
|	����	iNum = �o�^�ԍ�
|			func = �o�^�֐�
*----------------------------------------------------------------------*/
void	shader_func_set(int iNum, void *func)
{
	if(shader_max_get() > iNum)
	{
		pShaderTbl[iNum] = func;
	}
}



//
//	�V�F�[�_�[�̃v���O�����̈����A�߂�l�͑S�ē����ɂȂ��Ă��܂��B
//	����	iXNum		   = �w�t�@�C���ԍ�
//			iAttrib		   = �T�u�Z�b�g�ԍ�
//			pMeshContainer = ���b�V���R���e�i�[
//			pBoneComb	   = �{�[�����
//			mtx			   = ���[���h�s��
//	�߂�l	true	: �`�掞
//			false   : ��`�掞

#undef	SKIN_INDEX_MAX
#define	SKIN_INDEX_MAX	4

class	vertex
{
public:
	VEC4	m_vecPos;						// ���_�ʒu
	float	m_aiBoneWeight[SKIN_INDEX_MAX];	// �{�[�� �d��
	float	m_aiBoneIndex[ SKIN_INDEX_MAX];	// �{�[�� �ԍ�
	VEC3	m_vecNorm;						// ���_�@��
	VEC2	m_vecTex;						// UV���W
};



/*----------------------------------------------------------------------*
| <<< 00 D3_SHADER_TYPE_NORMAL : �m�[�}�� >>>
*----------------------------------------------------------------------*/
bool	shader_normal(int iXNum, int iAttrib, D3DXMESHCONTAINER_DERIVED *pMC, D3DXBONECOMBINATION *pBC, MTX *mtx, float fAlp)
{

	//--- �e�N�j�b�N ------------------------------------------------
	//if(!SHADER_TECH_SET(d3.eff, "Normal")){ return false;}
	d3.eff = d3.lpEffect[D3_SHADER_TYPE_NORMAL];

	//--- ���C�g ----------------------------------------------------
	SHADER_LIGHT_INVERSE_SET("g_vecLightDirection", mtx, pBC);
	SHADER_INT_SET(			 "g_iLightSw",		    d3.bLight);

	//--- �v���W�F�N�V���������[���h�s�� ----------------------------
	SHADER_MATRIX_SET("g_mtxViewProjection", &d3.mtxViewProjection);
	SHADER_MATRIX_SET("g_mtxWorld",		     mtx);

	//--- �{�[�� ----------------------------------------------------
	SHADER_BONE_SET("g_mtxWorldMatrixArray", pMC, pBC);

	//--- �e�N�X�`���[���}�e���A�� ----------------------------------
	d3_stat_alpha_direct_set(d3.bAlphaSw);
	SHADER_FLOAT_SET(  "g_fAlp",       fAlp);
	SHADER_TEXTURE_SET("texDecale",    d3.tex[SHADER_TEXTURE_GET(iXNum, iAttrib, pMC, pBC)]);
	SHADER_VEC4_SET(   "g_vecDiffuse",        SHADER_DIFFUSE_GET(iXNum, iAttrib, pMC, pBC) );
	SHADER_VEC4_SET(   "g_vecAmbient", &d3.vecAmbient);

	//--- �`�� ------------------------------------------------------
	SHADER_BEGIN();
	SHADER_PASS_BEGIN(0);
	SHADER_DRAW(iXNum, iAttrib, pMC, pBC);
	SHADER_PASS_END();
	SHADER_END();

#if 0
	cVertex3	*pVtx;
	int	iSize = sizeof(cVertex3);
	WORD	*pIndices;
	pMC->MeshData.pMesh->LockVertexBuffer(D3DLOCK_DISCARD, (void **)&pVtx);
	pMC->MeshData.pMesh->LockIndexBuffer( D3DLOCK_DISCARD, (void **)&pIndices);
	
	D3DVERTEXELEMENT9	pDecl[MAX_FVF_DECL_SIZE];
	pMC->MeshData.pMesh->GetDeclaration(pDecl);

	pVtx[1].m_vecPos.x = pVtx[1].m_vecPos.x;

/*
	for(uint j = 0; j < pMC->MeshData.pMesh->GetNumFaces(); j++)
	{
		VEC3	*pv[3], *pn[3], vecOut[3];
		WORD	*pTri = &pIndices[j * 3];
		pv[0] = (VEC3*)&pVtx[pTri[0]].m_vecPos;		// ���_
		pv[1] = (VEC3*)&pVtx[pTri[1]].m_vecPos;
		pv[2] = (VEC3*)&pVtx[pTri[2]].m_vecPos;
		pn[0] = (VEC3*)&pVtx[pTri[0]].m_vecNorm;	// �@��
		pn[1] = (VEC3*)&pVtx[pTri[1]].m_vecNorm;
		pn[2] = (VEC3*)&pVtx[pTri[2]].m_vecNorm;

		// �|���S���̂R���_�v�Z
		VEC3	v[3];
		VEC3	n[3];
		for(int k = 0; k < 3; k++)
		{
			v[k] = VEC3(0, 0, 0);
			n[k] = VEC3(0, 0, 0);
			for(int l = 0; l < 4; l++)
			{
				// �{�[���v�Z
				VEC3	tmp0, tmp1;
				D3DXVec3TransformCoord(&tmp0, pv[k], &mtxSkinPalette[(int)pVtx[pTri[k]].m_aiBoneIndex[l]]);
				D3DXVec3TransformCoord(&tmp1, pn[k], &mtxSkinPalette[(int)pVtx[pTri[k]].m_aiBoneIndex[l]]);

				// �E�G�C�g�v�Z
				v[k] += tmp0 * pVtx[pTri[k]].m_aiBoneWeight[l];
				n[k] += tmp1 * pVtx[pTri[k]].m_aiBoneWeight[l];
			}
		}
		// �|���S���`��
		//d3_3d_triangle_fill_draw(&vPoly[0], &vPoly[1], &vPoly[2], D3_COL_RED(255));

		DWORD	dwCol = D3_COL(255, 255, 255, 255);
		D3_VTX_DEFAULT_	vt[] =
		{
			v[0].x, v[0].y, v[0].z, n[0].x, n[0].y, n[0].z, pVtx[pTri[0]].m_vecTex.x, pVtx[pTri[0]].m_vecTex.y,
			v[1].x, v[1].y, v[1].z, n[1].x, n[1].y, n[1].z, pVtx[pTri[1]].m_vecTex.x, pVtx[pTri[1]].m_vecTex.y,
			v[2].x, v[2].y, v[2].z, n[2].x, n[2].y, n[2].z, pVtx[pTri[2]].m_vecTex.x, pVtx[pTri[2]].m_vecTex.y,
		};

		MATERIAL	m = d3.matNormal;
		d3_material_set(&m);
		d3_tex_set(-1);
		d3.dev->SetFVF(D3_FVF_DEFAULT);
		d3.dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 1, vt, sizeof(D3_VTX_DEFAULT));
		d3.iPolyCount += 1;
	}
*/

	pMC->MeshData.pMesh->UnlockIndexBuffer();
	pMC->MeshData.pMesh->UnlockVertexBuffer();

#endif

	return true;
}


