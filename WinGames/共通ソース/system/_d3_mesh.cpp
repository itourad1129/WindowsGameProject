/**************************************************************************************************
	[[[	���b�V���n ]]]
**************************************************************************************************/


/*----------------------------------------------------------------------*
| <<< �|���S���\�� >>>
|	����	iNum	    = ���f���ԍ�
|			iShaderType = �V�F�[�f�B���O�^�C�v
|			mtxWorld    = �z�u���W
|			fAlp	    = �A���t�@�l(0.f �` 1.f)
*----------------------------------------------------------------------*/
void	d3_3d_draw(int iNum, int iShaderType, MTX *mtxWorld, float fAlp)
{
	d3_shader_set(iShaderType);
	d3_mesh_draw(iNum, iShaderType, mtxWorld, fAlp, 0, d3.meshInfo[iNum].ucSubset);
}



/*----------------------------------------------------------------------*
| <<< �|���S���\�� >>>					���������̓I�t
|	����	iNum	    = ���f���ԍ�
|			iShaderType = �V�F�[�f�B���O�^�C�v
|			mtxWorld    = �z�u���W
*----------------------------------------------------------------------*/
void	d3_3d_draw(int iNum, int iShaderType, MTX *mtxWorld)
{
	d3.eff = d3.lpEffect[iShaderType];
	d3_mesh_draw(iNum, iShaderType, mtxWorld, 1, 0, d3.meshInfo[iNum].ucSubset);
}



/*----------------------------------------------------------------------*
| <<< �|���S���\�� >>>					���f�t�H���g�ŕ`��
|	����	iNum	 = ���f���ԍ�
|			mtxWorld = �z�u���W
|			fAlp	 = �A���t�@�l(0.f �` 1.f)
*----------------------------------------------------------------------*/
void	d3_3d_draw(int iNum, MTX *mtxWorld, float fAlp)
{

	d3.eff = d3.lpEffect[D3_SHADER_TYPE_NORMAL];
	if(fAlp < 1.0f){ d3_stat_alpha_set(true);}
	d3_mesh_draw(iNum, D3_SHADER_TYPE_NORMAL, mtxWorld, fAlp, 0, d3.meshInfo[iNum].ucSubset);
}



/*----------------------------------------------------------------------*
| <<< �|���S���\�� >>>					���f�t�H���g�`��A�������I�t
|	����	iNum  =	���f���ԍ�
|			world =	�z�u���W
*----------------------------------------------------------------------*/
void	d3_3d_draw(int iNum, MTX *mtxWorld)
{
	d3.eff = d3.lpEffect[D3_SHADER_TYPE_NORMAL];
	d3_mesh_draw(iNum, D3_SHADER_TYPE_NORMAL, mtxWorld, 1, 0, d3.meshInfo[iNum].ucSubset);
}



/*----------------------------------------------------------------------*
| <<< �w��ԍ��̃T�u�Z�b�g��`�� >>>
|	����	iNum	    = ���f���ԍ�
|			iMatNum	    = �p�[�c�ԍ�
|			iShaderType = �V�F�[�_�[�^�C�v
|			mtxWorld    = �z�u���W
|			fAlp	    = �A���t�@�l(0.f �` 1.f)
*----------------------------------------------------------------------*/
void	d3_3d_draw_parts(int iNum, int iMatNum, int iShaderType, MTX *mtxWorld, float fAlp)
{
	if((uint)iMatNum >=	d3.meshInfo[iNum].ucSubset){ return;}
	d3.eff = d3.lpEffect[iShaderType];
	d3_mesh_draw(iNum, iShaderType, mtxWorld, fAlp, iMatNum, iMatNum +	1);
}



/*----------------------------------------------------------------------*
| <<< �w��ԍ��̃T�u�Z�b�g��`�� >>>
|	����	iNum	    = ���f���ԍ�
|			iMatNum	    = �p�[�c�ԍ�
|			mtxWorld    = �z�u���W
|			fAlp	    = �A���t�@�l(0.f �` 1.f)
*----------------------------------------------------------------------*/
void	d3_3d_draw_parts(int iNum, int iMatNum, MTX *mtxWorld, float fAlp)
{

	if((uint)iMatNum >=	d3.meshInfo[iNum].ucSubset){ return;}

	int	iShaderType = D3_SHADER_TYPE_NORMAL;
	d3.eff = d3.lpEffect[iShaderType];
	d3_mesh_draw(iNum, iShaderType, mtxWorld, fAlp, iMatNum, iMatNum +	1);
}



/*---------------------------------------------------------------------*
| <<< �w��ԍ��̃T�u�Z�b�g��`�� >>>
|	����	iNum	    = ���f���ԍ�
|			iMatNum	    = �p�[�c�ԍ�
|			mtxWorld    = �z�u���W
*----------------------------------------------------------------------*/
void	d3_3d_draw_parts(int iNum, int iMatNum, MTX *mtxWorld)
{

	if((uint)iMatNum >=	d3.meshInfo[iNum].ucSubset){ return;}

	int	iShaderType = D3_SHADER_TYPE_NORMAL;
	d3.eff = d3.lpEffect[iShaderType];
	d3_mesh_draw(iNum, iShaderType, mtxWorld, 1, iMatNum, iMatNum +	1);
}



/*----------------------------------------------------------------------*
| <<< ���b�V���`�� >>>
|	����	iNum		= ���f���ԍ�
|			iShaderType = �V�F�[�f�B���O�^�C�v
|			mtxWorld	= ���[���h�s��
|			fAlp		= �������l
|			iStart,iEnd	= �`��J�n�ʒu�A�I���ʒu
*----------------------------------------------------------------------*/
void	d3_mesh_draw(int iNum, int iShaderType, MTX *mtxWorld, float fAlp, int iStart, int iEnd)
{

	//--- ���ǂݍ��݂̏ꍇ�̓��[�h���ɂ��� --------------------------
	if(!d3.meshInfo[iNum].bLoad){ d3_xfile_load(iNum, NULL, true);}

	//--- ���b�V�����ǂݍ��݃`�F�b�N --------------------------------
	if(d3.lpMesh[iNum] == NULL){ return;}

	//--- �^�C�v�ʂɃV�F�[�_�[�Ăяo�� ------------------------------
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
		d3.iPolyCount += d3.lpMesh[iNum]->GetNumFaces();		// ���_���v�Z
	}
}



/**************************************************************************************************
	[[[	���b�V���ǂݍ��݁A�쐬�n ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< �w�t�@�C���̂w�̍ŏ��l�𓾂� >>>
|	����	iXNum = �w�t�@�C���̓ǂݍ��ݔԍ�
*------------------------------------------------------------------------------*/
float	gr_xfile_xmin_get(int iXNum)
{
	
	// ���b�V�����ǂݍ���
	if(d3.lpMesh[iXNum] == NULL){ return 0;}

	// ���_�o�b�t�@�̃T�C�Y
	DWORD	dwVtxSize = d3.lpMesh[iXNum]->GetNumBytesPerVertex();

	// �S���_�ɑ΂��A�x���W�̒��ōŏ��̂��̂�T��
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
| <<< �w�t�@�C���̂w�̍ő�l�𓾂� >>>
|	����	iXNum = �w�t�@�C���̓ǂݍ��ݔԍ�
*------------------------------------------------------------------------------*/
float	gr_xfile_xmax_get(int iXNum)
{
	
	// ���b�V�����ǂݍ���
	if(d3.lpMesh[iXNum] == NULL){ return 0;}

	// ���_�o�b�t�@�̃T�C�Y
	DWORD	dwVtxSize = d3.lpMesh[iXNum]->GetNumBytesPerVertex();

	// �S���_�ɑ΂��A�x���W�̒��ōŏ��̂��̂�T��
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
| <<< �w�t�@�C���̂x�̍ŏ��l�𓾂� >>>
|	����	iXNum = �w�t�@�C���̓ǂݍ��ݔԍ�
*------------------------------------------------------------------------------*/
float	gr_xfile_ymin_get(int iXNum)
{
	
	// ���b�V�����ǂݍ���
	if(d3.lpMesh[iXNum] == NULL){ return 0;}

	// ���_�o�b�t�@�̃T�C�Y
	DWORD	dwVtxSize = d3.lpMesh[iXNum]->GetNumBytesPerVertex();

	// �S���_�ɑ΂��A�x���W�̒��ōŏ��̂��̂�T��
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
| <<< �w�t�@�C���̂x�̍ő�l�𓾂� >>>
|	����	iXNum = �w�t�@�C���̓ǂݍ��ݔԍ�
*------------------------------------------------------------------------------*/
float	gr_xfile_ymax_get(int iXNum)
{
	
	// ���b�V�����ǂݍ���
	if(d3.lpMesh[iXNum] == NULL){ return 0;}

	// ���_�o�b�t�@�̃T�C�Y
	DWORD	dwVtxSize = d3.lpMesh[iXNum]->GetNumBytesPerVertex();

	// �S���_�ɑ΂��A�x���W�̒��ōŏ��̂��̂�T��
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
| <<< �w�t�@�C���̂y�̍ŏ��l�𓾂� >>>
|	����	iXNum = �w�t�@�C���̓ǂݍ��ݔԍ�
*------------------------------------------------------------------------------*/
float	gr_xfile_zmin_get(int iXNum)
{
	
	// ���b�V�����ǂݍ���
	if(d3.lpMesh[iXNum] == NULL){ return 0;}

	// ���_�o�b�t�@�̃T�C�Y
	DWORD	dwVtxSize = d3.lpMesh[iXNum]->GetNumBytesPerVertex();

	// �S���_�ɑ΂��A�x���W�̒��ōŏ��̂��̂�T��
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
| <<< �w�t�@�C���̂y�̍ő�l�𓾂� >>>
|	����	iXNum = �w�t�@�C���̓ǂݍ��ݔԍ�
*------------------------------------------------------------------------------*/
float	gr_xfile_zmax_get(int iXNum)
{
	
	// ���b�V�����ǂݍ���
	if(d3.lpMesh[iXNum] == NULL){ return 0;}

	// ���_�o�b�t�@�̃T�C�Y
	DWORD	dwVtxSize = d3.lpMesh[iXNum]->GetNumBytesPerVertex();

	// �S���_�ɑ΂��A�x���W�̒��ōŏ��̂��̂�T��
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
| <<< ���E�{�b�N�X�^���E���̎Z�o >>>
|	����	no	   = �w�t�@�C���ǂݍ��ݔԍ�
|			p_mesh = ���b�V���̃|�C���^�[
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

	// box�ɂ�����ւ���Ă���ԍ�������̂Ń`�F�b�N�̂���
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
| <<< ���b�V���̍œK�� >>>
|	����	*out	   = �œK����̏o�͈ʒu
|			p_mesh	   = �œK�O�̃��b�V���|�C���^�[
|			p_mesh_adj = ���b�V���̈ʒu
*----------------------------------------------------------------------*/
static	void	d3_mesh_optimized(LPD3DXMESH p_mesh, LPD3DXBUFFER p_mesh_adj)
{
	DWORD	*p = new DWORD[p_mesh->GetNumFaces() * 3];

	p_mesh->OptimizeInplace(d3.dwMeshOptimized,	(DWORD*)p_mesh_adj->GetBufferPointer(),	p, NULL, NULL);
	delete(p);
}



/*----------------------------------------------------------------------*
| <<< ���b�V���I�u�W�F�N�g�폜 >>>		�����b�V������S�Ĕj��
|	����	iNum = ���b�V���̓ǂݍ��ݔԍ�(-1:�S�J��)	
*----------------------------------------------------------------------*/
void	d3_xfile_free(int iNum)
{

	if(iNum == -1){ d3_xfile_free_all(); return;}

	if(d3.lpMesh[iNum] == NULL){	return;}

	// �e�N�X�`���[���J��
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

	SAFE_DELETE( d3.matMesh[iNum]);			// �}�e���A��
	SAFE_RELEASE(d3.lpMesh[ iNum]);			// ���b�V��
	d3.meshInfo[iNum].strFullPath[0] = NULL;
	d3.meshInfo[iNum].bResource		 = false;
	d3.hitWork[ iNum].fRadius		 = 0;
}



/*----------------------------------------------------------------------*
| <<< �S���b�V���I�u�W�F�N�g�폜 >>>
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
| <<< �œK���ƃp���v�̍쐬 >>>
|	����	iID		   = ID	�ԍ�
|			pMesh	   = ���b�V���̊i�[�ʒu
*----------------------------------------------------------------------*/
void	d3_mesh_make(int iNum, LPD3DXMESH pMesh)
{

	//--- ���b�V�����`������	-------------------------------------
	pMesh->CloneMeshFVF(pMesh->GetOptions(), /*pMesh->GetFVF() |*/D3_FVF_DEFAULT, d3.dev, &d3.lpMesh[iNum]);

	//--- �@���쐬 --------------------------------------------------
	if(!(pMesh->GetFVF() & D3DFVF_NORMAL))		// �@���������Ƃ��͍쐬�̂���
	{
		D3DXComputeNormals(d3.lpMesh[iNum], NULL);
	}
	//--- ���b�V���̍œK�� ------------------------------------------
	DWORD	*rgdwAdjacency = NULL;
	rgdwAdjacency =	new	DWORD[d3.lpMesh[iNum]->GetNumFaces() * 3];
	if(rgdwAdjacency !=	NULL)
	{
		pMesh->GenerateAdjacency(1e-6f,	rgdwAdjacency);
		pMesh->OptimizeInplace(d3.dwMeshOptimized, rgdwAdjacency, NULL,	NULL, NULL);
		delete []rgdwAdjacency;
	}
	//--- �œK����̒��_���A�|���S�����𓾂� ------------------------
	d3.meshInfo[iNum].usVertex = (ushort)d3.lpMesh[iNum]->GetNumVertices();
	d3.meshInfo[iNum].usFaces  = (ushort)d3.lpMesh[iNum]->GetNumFaces();
	pMesh->Release();

	pMesh =	d3.lpMesh[iNum];
}



/*------------------------------------------------------------------------------*
| <<< �w�t�@�C���̓ǂݍ��݃`�F�b�N >>>
|	����	iNum	 = ���b�V���Ǘ��ԍ�
|	�߂�l	true : �ǂݍ��ݍς�
*------------------------------------------------------------------------------*/
bool	d3_xfile_check(int iNum)
{

	return (d3.lpMesh[iNum] != NULL);
}



/*----------------------------------------------------------------------*
| <<< �w�t�@�C���̓ǂݍ���	>>>
|	����	iNum	    = �ǂݍ��ݔԍ�
|			strFile		= �ǂݍ��݃t�@�C����( NULL �̏ꍇ�� d3.texInfo ���ǂݍ���)
|			bLoadAtOnce = true : �����ǂݍ���
|			bSysMem     = true : �V�X�e���������֓ǂݍ���
|			bErrMessage = true : �G���[���b�Z�[�W��\������
|	�߂�l	true or	false
*----------------------------------------------------------------------*/
bool	d3_xfile_load(int iNum, char *pstrFile, bool bLoadAtOnce, bool bSysMem, bool bErrMessage)
{

	char	strPath[_MAX_PATH + 1];

	if(pstrFile != NULL)
	{
		//--- �g���q���Ȃ����̂̓��\�[�X�Ƃ݂Ȃ� --------------------
		d3.meshInfo[iNum].bResource = true;
		int		i = 0;
		while(pstrFile[i] != NULL)
		{
			if(ISKANJI(pstrFile[i]   )){ i += 2;									}
			else if(pstrFile[i] == '.'){ d3.meshInfo[iNum].bResource = false; break;}
			else					   { i++;									    }
		}

		//--- �t�@�C�������쐬 --------------------------------------
		//	�@���\�[�X�̏ꍇ�ɂ͂����R�s�[
		if(!d3.meshInfo[iNum].bResource)
		{
			char	strName[_MAX_FNAME + 1];
			char	strExt[ _MAX_EXT   + 1];
			sys_div_path(strPath, strName, strExt, pstrFile);
			sprintf(strPath, "%s%s%s", strPath, strName, strExt);
		}
		else{ strcpy(strPath, pstrFile);}

		//--- �����Ȃ�΁A�ǂݍ��܂Ȃ��ŏI�� ------------------------
		if(d3.lpMesh[iNum] != NULL && d3.meshInfo[iNum].bLoad)
		{
			if(strcmp(d3.meshInfo[iNum].strFullPath, strPath) == 0){ return false;}
		}
	}
	//--- ��ǂ݂̏ꍇ ----------------------------------------------
	else
	{
		strcpy(strPath, d3.meshInfo[iNum].strFullPath);	// �t�H���_�[�����R�s�[
	}
	if(strlen(strPath) == 0){ return false;}

	//--- �����A�ǂݍ��݂Ŗ����ꍇ ----------------------------------
	if(!bLoadAtOnce)
	{
		strcpy(d3.meshInfo[iNum].strFullPath, strPath);	// �t�@�C�����ۑ�
		d3.meshInfo[iNum].bLoad = false;
		return true;
	}

	//--- �O�񕪂����� ----------------------------------------------
	skin_free(	  iNum);
	d3_xfile_free(iNum);

	//--- ���^�Z�R�t�@�C�������ׂ� ----------------------------------
#ifdef __MQO
	if(strstr(strPath, ".mqo"))
	{
		BOOL	bRet = (BOOL)meta_load(iNum, strPath);
		strcpy(d3.meshInfo[iNum].strFullPath, strPath);	// �t���p�X���R�s�[
		d3.meshInfo[iNum].bSkin = false;
		d3.meshInfo[iNum].bLoad = true;					// �ǂݍ��݃t���O�I��
														// �^�C���X�^���v�o�^
		sys_timestamp_get(&d3.meshInfo[iNum].ftLastWrite, d3.meshInfo[iNum].strFullPath);
		return bRet ? true : false;
	}
#endif
	//--- ���b�V���ǂݍ���	-----------------------------------------
	LPD3DXMESH		pMesh;
	LPD3DXBUFFER	pMatBuf	= NULL;
	DWORD			n;									// �}�e���A����
	HRESULT			h;

	DWORD			dwFlag;
	if(bSysMem){ dwFlag = D3DXMESH_SYSTEMMEM;}
	else	   { dwFlag = D3DXMESH_MANAGED ;}
	
	if(d3.meshInfo[iNum].bResource){ h = D3DXLoadMeshFromXResource(NULL, strPath, "MESH", dwFlag, d3.dev, NULL, &pMatBuf, NULL, &n, &pMesh);}
	else						   { h = D3DXLoadMeshFromX(		 	     strPath,		  dwFlag, d3.dev, NULL, &pMatBuf, NULL, &n, &pMesh);}

											// �t�H���_�[�ʒu�����̏�Ԃ֖߂��ă��^�[��
	if(FAILED(h))
	{
		if(bErrMessage)
		{
			return sys_error("d3_xfile_load: %s ���ǂݍ��߂܂���ł����B", strPath);
		}
		else
		{
			return false;
		}
	}

	if(!d3.meshInfo[iNum].bResource){ sys_folder_push(), FOLDER_SET(strPath);}

	d3_mesh_make(iNum, pMesh);				// ���b�V�����쐬
										
	// �}�e���A���m��
	SAFE_DELETE(d3.matMesh[iNum]);
	if(n > 0){ d3.matMesh[iNum] = new MATERIAL[n];}
	d3.meshInfo[iNum].ucSubset			 = (uchar)n;	// �T�u�Z�b�g��
	d3.meshInfo[iNum].ucSkinAnimationSet = 0;			// �A�j���[�V�����g���b�N��

	// �e�N�X�`���[���[�h
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

	// Diffuse �� 0 �̂��̂�T��
	for(uint i = 0; i < d3.meshInfo[iNum].ucSubset; i++)
	{
		if(d3.matMesh[iNum][i].Diffuse.a == 0){ d3.matMesh[iNum][i].Diffuse.a = 1.0f;}
	}

	// �e�N�X�`���[�ǂݍ���
	//d3_mesh_tex_load(iNum, (D3DXMATERIAL*)pMatBuf->GetBufferPointer(), n);

	d3_hit_work_get(&d3.hitWork[iNum], d3.lpMesh[iNum]);

	if(!d3.meshInfo[iNum].bResource){ sys_folder_pop();}	// �t�H���_�[�ʒu�����̏�Ԃ֖߂�
	strcpy(d3.meshInfo[iNum].strFullPath, strPath);			// �t���p�X���R�s�[
	d3.meshInfo[iNum].bSkin = false;						// �X�L�����b�V���I�t
	d3.meshInfo[iNum].bLoad = true;							// �ǂݍ��݃t���O�I��

	SAFE_RELEASE(pMatBuf);

	//--- �^�C���X�^���v�o�^ ----------------------------------------
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
| <<< ���[�V�����ƃ��[�V�����̒��Ԃ����A�V�K�Ƀ��b�V�����쐬���� >>>
|	����	iIn0Num	= �ǂݍ��ݔԍ�
|			iIn1Num	= �ǂݍ��ݔԍ�
|			fS      = ���d�W��
|	�o��	iOutNum = ���Ԃ��Ƃ郁�b�V��
|	�߂�l	true or	false
*----------------------------------------------------------------------*/
bool	d3_mesh_tween(int iOutNum, int iIn0Num, int iIn1Num, float fS)
{
	//--- ���ǂݍ��݃`�F�b�N ----------------------------------------
	if(d3.lpMesh[iIn0Num] == NULL || d3.lpMesh[iIn1Num] == NULL) return false;

	//--- ���b�V�����`�F�b�N�[ ------------------------------------
	if(d3.meshInfo[iIn0Num].ucSubset != d3.meshInfo[iIn1Num].ucSubset
	|| d3.meshInfo[iIn0Num].usFaces  != d3.meshInfo[iIn1Num].usFaces
	|| d3.meshInfo[iIn0Num].usVertex != d3.meshInfo[iIn1Num].usVertex) return false;

	//--- ���b�V�����R�s�[ ----------------------------------------
	d3.meshInfo[iOutNum] = d3.meshInfo[iIn0Num];
	
	//--- �}�e���A���R�s�[ ------------------------------------------
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

	//--- ���b�V���R�s�[ --------------------------------------------
	LPD3DXMESH	p0 = d3.lpMesh[iIn0Num];
	LPD3DXMESH	p1 = d3.lpMesh[iIn1Num];
	p0->CloneMeshFVF(p0->GetOptions(), p0->GetFVF() | D3_FVF_DEFAULT, d3.dev, &d3.lpMesh[iOutNum]);
	LPD3DXMESH	p2 = d3.lpMesh[iOutNum];

	//--- ���Ԃ̃��[�V�������쐬 ------------------------------------
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

	//--- �o���v���쐬 ----------------------------------------------
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
	//--- �I�� ------------------------------------------------------
	return true;
}



#if 0
	/*----------------------------------------------------------------------*
	| <<< ����������b�V���� >>>
	|	����	iID		  =	���b�V���i�[�ԍ�
	|			pstrFont  =	�t�H���g��
	|			size	  =	�T�C�Y
	|			extrusion =	����
	|			str		  =	�쐬���镶���i��������n�j�j
	|	�߂�l	true or	false
	*----------------------------------------------------------------------*/
	bool	d3_text_create(int iID, TCHAR *pstrFont, int size, float extrusion, char *str)
	{

		HRESULT			hr;
		HFONT			hFont;
		HFONT			hFontOld;
		LOGFONT			lf;
		LPD3DXMESH		pMesh;
		HDC				hdc	= CreateCompatibleDC(NULL);

		d3_xfile_free(iID);					// �O��f�[�^������

		//--- �����̃��b�V���쐬 --------------------------------------
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

		d3_mesh_make(iID, pMesh);				// ���b�V���쐬
												// �e�N�X�`���[�ǂݍ���
		//return d3_mesh_tex_load(iID, (D3DXMATERIAL*)NULL, 0);

		return true;
	}
#endif





/*------------------------------------------------------------------------------*
| <<< ���b�V���ƃ��C�̌�����������āA��_��Ԃ� >>>
|	����	iNum  = ���b�V���̓ǂݍ��ݔԍ�
|			iMat  = ���ׂ�}�e���A���ԍ�(-1�Ȃ�S�Ă���x�ɒ��ׂ�)
|			pPos  = ���W
|			pDir  = ���C�̌���
|			fSize = ���b�V���̑傫��
|			mtx	  = ���[���h�s��
|	�o��	pOutCrossPos  = �������Ă�����W
|			pOutCrossDist = ��_�܂ł̋���
|	�߂�l	true  :	�������Ă���
|			false :	�������Ă��Ȃ�
|
|	���w�t�@�C���̃t�H�[�}�b�g�� D3_VTX_DEFAULT �Ƃ��Ă��������B
*------------------------------------------------------------------------------*/
static	D3_VTX_DEFAULT	vtxTri[3];			// �O�p�`�̍��W
static	int				iCrossFaceNum = -1;	// ���������ʔԍ�
											// �����������b�V���ԍ�
bool	d3_mesh_intersection_check(VEC3 *pOutCrossPos, float *pOutCrossDist, int iNum, int iMat, VEC3 *pPos, VEC3 *pDir, float fSize, MTX *mtx)
{
	
	if(d3.lpMesh[iNum] == NULL) return false;	// ���b�V�����ǂݍ���

	//--- �������N���A�[ --------------------------------------------
	if(pOutCrossDist != NULL)
	{
		*pOutCrossDist = 10000;
	}
	//--- �}�E�X�̌����ƃ}�b�v�̌������� ----------------------------
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
	if(!bHit) return false;					// ���C���������Ă��Ȃ���΋A��
	if(pBuffer == NULL) return false;		// �o�b�t�@���m�ۂł��Ȃ��Ă��A��

	//--- �������Ă���΂ǂ̖ʂ��������Ă��邩���ׂ� ----------------
	D3DXINTERSECTINFO	*p = (D3DXINTERSECTINFO	*)pBuffer->GetBufferPointer();

	DWORD	dwMeshSize = d3.lpMesh[iNum]->GetNumBytesPerVertex();
	//uint	aa = sizeof(D3_VTX_DEFAULT);
	// dwMeshSize �� sizeof(D3_VTX_DEFAULT) �Ɠ��l�łȂ��Ƃ����삷��悤�ɉ��ǂ��Ă��܂�

	//---- �����̋߂�������\�[�g���� ------------------------------
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

			//--- �q�b�g���Ă���O�p�`�𓾂� ------------------------
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

			vtxTri[0] =	*((D3_VTX_DEFAULT*)&pVertices[iThisTri[0] * dwMeshSize]);	// �������O�p�`�̖ʏ��ƂȂ�
			vtxTri[1] =	*((D3_VTX_DEFAULT*)&pVertices[iThisTri[1] * dwMeshSize]);	// ��ɎO�p�`��`�悷��̂ō��W��ۑ�
			vtxTri[2] =	*((D3_VTX_DEFAULT*)&pVertices[iThisTri[2] * dwMeshSize]);

			pVB->Unlock();
			pIB->Unlock();
			pVB->Release();
			pIB->Release();

			//--- �ʂ̕\������(���ʂ����肷��ꍇ�͈ȉ����R�����g�̂���)
			D3DXPLANE	plane;
			D3DXPlaneFromPoints(&plane,	&vtxTri[0].p, &vtxTri[1].p,	&vtxTri[2].p);
			float	a =	D3DXPlaneDotNormal(&plane, pDir);
			if(a >=	0){ continue;}			// �v���X�̏ꍇ�͗���

			//--- �ʂ̃��C�̌�_�𓾂� ------------------------------
											// pOutCrossPos �Ɍ�_�̍��W
			D3DXVec3BaryCentric(pOutCrossPos, &vtxTri[0].p,	&vtxTri[1].p, &vtxTri[2].p,	fBary1,	fBary2);
			fDistMax = fDist;

			//--- �ʔԍ����Z�b�g ------------------------------------
			iCrossFaceNum = (int)dwFace;
		}
		SAFE_RELEASE(pBuffer);
	}
											// �T�C�Y�����āA�������Ă��Ȃ�������A��
											// ����ł��A�������Ă�����W�͌v�Z����
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
| <<< d3_mesh_intersection_check �œ���ꂽ�ʏ��𓾂� >>>
|	�߂�l	���������ʂ̏��e�[�u��
*------------------------------------------------------------------------------*/
D3_VTX_DEFAULT	*d3_mesh_intersection_face(void)
{
	return	vtxTri;
}



/*------------------------------------------------------------------------------*
| <<< d3_mesh_intersection_check �Ō��������ʔԍ��𓾂� >>>
|	�߂�l	���������ʂ̏��e�[�u��
*------------------------------------------------------------------------------*/
int		d3_mesh_intersection_face_num_get(void)
{
	return	iCrossFaceNum;
}



/*----------------------------------------------------------------------*
| <<< �{�b�N�X���m�̂����蔻�� >>>		�� D3_HIT_VEC_SET �ō��W���v�Z���Ă�������
|	�� *a ��傫�����̂Ƃ���悤��
*----------------------------------------------------------------------*/
bool	d3_hit_box_check(VEC3 *a, VEC3 *b)
{

	D3DXPLANE	p[6];						// A �̊e�ʂ��쐬

	D3DXPlaneFromPoints(&p[0], &a[0], &a[1], &a[3]);	// ���
	D3DXPlaneFromPoints(&p[1], &a[3], &a[2], &a[6]);	// ����
	D3DXPlaneFromPoints(&p[2], &a[4], &a[7], &a[6]);	// ���
	D3DXPlaneFromPoints(&p[3], &a[0], &a[4], &a[5]);	// ����
	D3DXPlaneFromPoints(&p[4], &a[0], &a[3], &a[7]);	// ����
	D3DXPlaneFromPoints(&p[5], &a[1], &a[5], &a[6]);	// �E��

	for(int	j =	0; j < 8; j++)				// B �̒��_���P�ł�
	{
		bool	f =	true;
		for(int	i =	0;i	< 6; i++)			// A �̑S�Ă̖ʂ̓����ɂ��邩�H
		{									// �O���ɂ�������ʂ̒��_�𒲂ׂ�
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
| <<< ���b�V���̂����蔻��p�a�n�w��\�� >>>	�� D3_3DLINE_FLUSH	���Ăяo������
|	����	pHit   = �����蔻��p���[�N			�� D3_HIT_VEC_SET �ō��W���v�Z���Ă�������
*------------------------------------------------------------------------------*/
void	d3_hit_box_draw(VEC3 *pp)
{
#ifdef	__HIT_AREA_DRAW
	//--- �{�b�N�X�`�� --------------------------------------------
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
| <<< ���C�ƎO�p�`�̌������� >>>
|	����	pos			= ���ׂ����n�_
|			dir			= �I�_�i���C�̕����j
|			v0,	v1,	v2	= �O�p�`���ꂼ��̒��_
|	�o��	*out		= ��_�o�͗p (NULL�Ȃ�o�͂��Ȃ�)
|	�߂�l	true	: ��������
|			false	: �������Ă��Ȃ�
*------------------------------------------------------------------------------*/
bool	d3_hit_intersect(VEC3 *out, VEC3 *pos, VEC3	*dir, VEC3 *v0, VEC3 *v1, VEC3 *v2)
{

	D3DXPLANE	plane;
	VEC3		v;

										// �R�̃x�N�g�����O�p�`���쐬
	D3DXPlaneFromPoints(&plane,	v0,	v1,	v2);
										// ���ʂƒ����̌�_�𒲂ׂ�
										// ������ d3_view_from	�� pos ������
	if(D3DXPlaneIntersectLine(&v, &plane, dir, pos))
	{									// ���O����
		VEC3	n[3];
		VEC3	vv0, vv1, vv2;
		vv0	= *v0 -	v;
		vv1	= *v1 -	v;
		vv2	= *v2 -	v;
		D3DXVec3Cross(&n[0], &vv0, &vv2);
		D3DXVec3Cross(&n[1], &vv1, &vv0);
		D3DXVec3Cross(&n[2], &vv2, &vv1);
		if(D3DXVec3Dot(&n[0], &n[1]) < 0 ||	D3DXVec3Dot(&n[0], &n[2]) <	0)
		{								// �O��
			return false;
		}
		else
		{								// ����
			if(*out) *out =	v;			// out �� NULL �ȊO�Ȃ�Ό�_��Ԃ�
			return true;
		}
	}
	else
	{									// �����Ɩʂ����s
		return false;
	}
}
