//
//	�A���t�@�\�[�g����
//
//		�� �|���S�����\�[�g���`�悷��B�A���t�@�t���̃|���S���Ŏg�p����Ɨǂ��B�������x�͒x���̂Œ��ӁI
//		�� �����������̂��߃e�N�X�`���[�������Ă���B���̂��߁A�[����[�֑����Ă��� UV ��}�C�i�X�� UV �͎g���Ȃ��̂Œ��ӁI



//--- �萔��` ------------------------------------------------------------------------------------

#define	ASORT_W			iRenderW				// �A���t�@�\�[�g�e�N�X�`���[�\�[�g
#define	ASORT_H			iRenderH
#define	ASORT_TEX_W		(ASORT_W / ASORT_DIV_)	// ���b�V���ЂƂ�����̃e�N�X�`���[�T�C�Y
#define	ASORT_TEX_H		(ASORT_H / ASORT_DIV_)


//--- �\���� --------------------------------------------------------------------------------------
struct	ASORT_WORK						// �|�����g���\����
{
	D3_VTX_LX	avtx[3];					// ���_���
	float		fZ;							// ���[���h�g�����X�t�H�[���{�ˉe�ϊ���̂y���W
};


//--- �ϐ����` ----------------------------------------------------------------------------------
											// �I�t�X�N���[���T�C�Y
static	int			iRenderW  = ASORT_DEFAULT_SIZE;
static	int			iRenderH  = ASORT_DEFAULT_SIZE;
static	int			iAsortTex = -1;			// asort ���C�u�����B���g�p����e�N�X�`���[
static	uint		uiPoly;					// �o�^�|����
static	ASORT_WORK	awork[ASORT_POLY_MAX];

// �\�[�g�p
static	int			iOrder[ASORT_POLY_MAX];	// ������ɊǗ��ԍ�
static	float		fOrder[ASORT_POLY_MAX];	// ������� Z ���W������

// �o�b�t�@
static	D3_VTX_LX	avtxBuf[ASORT_POLY_MAX * 3];	


//--- �֐��錾 ------------------------------------------------------------------------------------
static	void	asort_tex_set(int iNum, int iTex);
static	void	QSort(float afDat[], int aiDat[ ], int iLeft, int iRight);



/*------------------------------------------------------------------------------*
| <<< �A���t�@�\�[�g�p�I�t�X�N���[���̉����𓾂� >>>
|	�߂�l	����
*------------------------------------------------------------------------------*/
int		asort_render_width_get(void)
{
	return ASORT_W;
}



/*------------------------------------------------------------------------------*
| <<< �A���t�@�\�[�g�p�I�t�X�N���[���̏c���𓾂� >>>
|	�߂�l	�c��
*------------------------------------------------------------------------------*/
int		asort_render_height_get(void)
{
	return ASORT_W;
}



/*------------------------------------------------------------------------------*
| <<< �A���t�@�\�[�g������ >>>
|	����	iTex   = �A���t�@�\�[�g�Ŏg�p����e�N�X�`���[�ԍ�
|			iW, iH = �I�t�X�N���[���T�C�Y
*------------------------------------------------------------------------------*/
void	asort_init(int iTex, int iW, int iH)
{
	//
	// �I�t�X�N���[�����쐬���N���A�[����
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
| <<< �A���t�@�\�[�g�e�N�X�`���[�Z�b�g >>>
|	����	iNum = �Ǘ��ԍ�
|			iTex = �A���t�@�\�[�g�Ŏg�p����e�N�X�`���[�ԍ�
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
| <<< �o�^�|���S�����N���A >>>
*------------------------------------------------------------------------------*/
void	asort_clear(void)
{

	uiPoly = 0;
}



/*------------------------------------------------------------------------------*
| <<< �|���S���o�^ >>>
|	����	pavtx = �O�p�`�̒��_
*------------------------------------------------------------------------------*/
void	asort_set(D3_VTX_LX *pavtx)
{

	if(uiPoly >= ASORT_POLY_MAX){ return;}

	ASORT_WORK	*w = &awork[uiPoly];

	memcpy(w->avtx, pavtx, sizeof(D3_VTX_LX) * 3);

	// �d�S�����߂�
	VEC3	v = (pavtx[0].p + pavtx[1].p + pavtx[2].p) / 3.0f;

	// �d�S���g�����X�t�H�[����o�^
	calc_vec3_transform_coord(&v, &v, &d3.mtxView);
	w->fZ			= v.z;
	iOrder[uiPoly] = uiPoly;
	fOrder[uiPoly] = w->fZ;

	uiPoly++;
}



/*------------------------------------------------------------------------------*
| <<< �N�C�b�N�\�[�g >>>
*------------------------------------------------------------------------------*/
static	void	QSort(float afDat[], int aiDat[ ], int iLeft, int iRight)
{

	int		i, j;
	float	fPivot;

	i      = iLeft;							// �\�[�g����z��̈�ԏ������v�f�̓Y��
	j      = iRight;						// �\�[�g����z��̈�ԑ傫���v�f�̓Y��
	fPivot = afDat[(iLeft + iRight) / 2];	// ��l��z��̒����t�߂ɂƂ�
	while(1)
	{
		while(afDat[i] < fPivot  ){ i++;}	// fPivot ���傫���l��T��
		while(fPivot   < afDat[j]){ j--;}	// fPivot ��菬�����l��T��
		if(i >= j){ break;} 				// i >= j �Ȃ疳�����[�v���甲����
		SWAPF(afDat[i], afDat[j]);			// ����
		SWAP( aiDat[i], aiDat[j]);
		i++;								// ���̃f�[�^
		j--;
	}
	if(iLeft < i - 1)						// ��l�̍��� 2 �ȏ�v�f������΁A���̔z��� Q �\�[�g����
	{
		QSort(afDat, aiDat, iLeft, i - 1);
	}
	if(j + 1 < iRight)						// ��l�̉E�� 2 �ȏ�v�f������΁A�E�̔z��� Q �\�[�g����
	{
		QSort(afDat, aiDat, j + 1, iRight);
	}
}



/*------------------------------------------------------------------------------*
| <<< �o�^�|���S�����\�[�g >>>
*------------------------------------------------------------------------------*/
void	asort_sort(void)
{
	if(uiPoly > 0)
	{
		QSort(fOrder, iOrder, 0, uiPoly - 1);
	}
}



/*------------------------------------------------------------------------------*
| <<< �o�^�|���S����`�� >>>
*------------------------------------------------------------------------------*/
void	asort_draw(void)
{

	if(uiPoly == 0){ return;}

	d3_stat_fvf_set(	  D3_FVF_LX);
	d3_stat_transform_set(&d3.mtxIdentity);
	d3_tex_set(			  iAsortTex);
	d3.dev->SetRenderState(D3DRS_AMBIENT, D3_COL((int)(d3.vecAmbient.w * 255), (int)(d3.vecAmbient.x * 255), (int)(d3.vecAmbient.y * 255), (int)(d3.vecAmbient.z * 255)));

	// �o�^�|���S�����ꖇ���`��(�������O��)
	for(uint i = uiPoly - 1; i != -1; i--)
	{
		ASORT_WORK	*w = &awork[iOrder[i]];
		d3.dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST,	1, w->avtx, sizeof(D3_VTX_LX));
		d3.iPolyCount++;
	}
}



/*------------------------------------------------------------------------------*
| <<< �o�^�|���S�����o�b�t�@�ɒ��߂� >>>
*------------------------------------------------------------------------------*/
static	D3_VTX_LX	avtxAsort[ASORT_BUF_MAX_ * 3];
static	int			iAsortCount;
void	asort_buf_set(void)
{

	if(uiPoly == 0){ return;}

	// �o�^�|���S�����ꖇ���`��(�������O��)
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
| <<< asort ���C�u�����B���J�� >>>
*------------------------------------------------------------------------------*/
void	asort_free(void)
{

	if(iAsortTex != -1){ d3_tex_free(iAsortTex);}
	iAsortTex  = -1;
	uiPoly     = 0;
}



/*---------------------------------------------------------------------------------------------------------------------------------------------------
	<<< ���b�V���p >>>
*--------------------------------------------------------------------------------------------------------------------------------------------------*/

#define	MESH_MAX		D3_MESH_MAX			// asort ���b�V���o�^��
#define	MATERIAL_MAX	1					// �}�e���A����(mqo �̍ގ��̐�)


//--- �\���� --------------------------------------------------------------------------------------
struct	FACE_WORK
{
	VEC3	avecPos[3];
	VEC2	avecUV[ 3];
};


//--- �ϐ����` ----------------------------------------------------------------------------------
static	FACE_WORK	*pfaceWork[MESH_MAX];
static	uint		auiFaceMax[MESH_MAX];
static	int			aiTex[	   MESH_MAX];
static	int			iAsortMeshCount;		// �ǂݍ��݃��b�V����
static	int			iAsortTexCount;			// �ǂݍ��݃e�N�X�`���[��

//static	int			iAsortMeshCount;		// �o�^��
//static	int			iControlID[MESH_MAX];	// �Ǘ��ԍ�


//--- �֐��錾-------------------------------------------------------------------------------------


/*------------------------------------------------------------------------------*
| <<< asort ���b�V���N���A >>>
*------------------------------------------------------------------------------*/
void	asort_mesh_init(void)
{

	asort_mesh_free();						// �ǂݍ��݃��b�V�����

	iAsortMeshCount = 0;					// �ǂݍ��݃��b�V����
	iAsortTexCount  = 0;					// �݌v�e�N�X�`���[��
	//iAsortMeshCount = 0;					// �o�^���N���A
}



/*------------------------------------------------------------------------------*
| <<< asort �p�Ƀ��b�V���ǂݍ��� >>>
|	����	iMesh = �o�^���郁�b�V���ԍ�(d3_xfile_load �̓ǂݍ��ݔԍ�)
|	�߂�l	FALSE : �ǂݍ��ݎ��s
*------------------------------------------------------------------------------*/
BOOL	asort_mesh_load(int iMesh)
{
	// ���������
	asort_mesh_free(iMesh);

	// ���肷��ʂ̐�
	auiFaceMax[iMesh] = d3.meshInfo[iMesh].usFaces;
	if(auiFaceMax[iMesh] == 0){ return FALSE;}

	// �������m��(�|���S�����A���[�N���m�ۂ���)
	pfaceWork[iMesh] = (FACE_WORK *)malloc(sizeof(FACE_WORK) * auiFaceMax[iMesh]);
	memset(pfaceWork[iMesh], 0, sizeof(FACE_WORK) * auiFaceMax[iMesh]);

	//--- ���_������������ ----------------------------------------
	D3_VTX_DEFAULT	*pvtxBuf;				// ���_�o�b�t�@
	WORD			*pwIndex;				// �C���f�b�N�X�o�b�t�@
	DWORD			*pdwMaterial;			// �}�e���A���o�b�t�@
	d3.lpMesh[iMesh]->LockVertexBuffer(   D3DLOCK_DISCARD, (void **)&pvtxBuf);
	d3.lpMesh[iMesh]->LockIndexBuffer(    D3DLOCK_DISCARD, (void **)&pwIndex);
	d3.lpMesh[iMesh]->LockAttributeBuffer(D3DLOCK_DISCARD, &pdwMaterial);

	// �C���f�b�N�X�o�b�t�@�]��
	FACE_WORK	*w = pfaceWork[iMesh];
	for(uint i = 0; i < auiFaceMax[iMesh]; i++, w++)
	{
		w->avecPos[0] =  pvtxBuf[pwIndex[i * 3 + 0]].p;
		w->avecPos[1] =  pvtxBuf[pwIndex[i * 3 + 1]].p;
		w->avecPos[2] =  pvtxBuf[pwIndex[i * 3 + 2]].p;
											// �}�e���A���ԍ� + �݌v�e�N�X�`���[�ԍ�
		DWORD	dwMat = pdwMaterial[i] + iAsortTexCount;
											// UV �ʒu
		VEC2	vecOfs((float)((dwMat % ASORT_DIV_) * ASORT_TEX_W) / (float)ASORT_W, (float)((dwMat / ASORT_DIV_) * ASORT_TEX_H) / (float)ASORT_H);
		w->avecUV[ 0] = (pvtxBuf[pwIndex[i * 3 + 0]].uv / ASORT_DIV_) + vecOfs;
		w->avecUV[ 1] = (pvtxBuf[pwIndex[i * 3 + 1]].uv / ASORT_DIV_) + vecOfs;
		w->avecUV[ 2] = (pvtxBuf[pwIndex[i * 3 + 2]].uv / ASORT_DIV_) + vecOfs;
	}
	// ���_�o�b�t�@����
	d3.lpMesh[iMesh]->UnlockAttributeBuffer();
	d3.lpMesh[iMesh]->UnlockIndexBuffer();
	d3.lpMesh[iMesh]->UnlockVertexBuffer();

	//--- �e�N�X�`���[���� ------------------------------------------
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
| <<< asort �p�Ƀ��b�V���ǂݍ��� >>>	�� ���^�Z�R�C���̃f�[�^�����ƂɑS�|���S���̒��_��ǂݍ���(�q�b�g�}�b�v���Ŏg�p�̂���)
|	����	iMesh   = ���b�V���ԍ�
|			strFile = �t�@�C����
|	�߂�l	FALSE : �ǂݍ��ݎ��s
*------------------------------------------------------------------------------*/
BOOL	asort_mesh_load(int iMesh, char *strFile)
{

	if(!meta_buf_load(strFile)){ return FALSE;}

	// ���������
	asort_mesh_free(iMesh);

	// ���肷��ʂ̐�
	auiFaceMax[iMesh] = meta_buf_poly_count_get();
	if(auiFaceMax[iMesh] == 0){ return FALSE;}

	// �������m��(�|���S�����A���[�N���m�ۂ���)
	pfaceWork[iMesh] = (FACE_WORK *)malloc(sizeof(FACE_WORK) * auiFaceMax[iMesh]);
	memset(pfaceWork[iMesh], 0, sizeof(FACE_WORK) * auiFaceMax[iMesh]);

	// UV �ʒu
	VEC2	vecOfs((float)((iMesh % ASORT_DIV_) * ASORT_TEX_W) / (float)ASORT_W, (float)((iMesh / ASORT_DIV_) * ASORT_TEX_H) / (float)ASORT_H);

	//--- ���_������������ ----------------------------------------
	// �C���f�b�N�X�o�b�t�@�]��
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
	
	//--- �o�b�t�@��� ----------------------------------------------
	meta_buf_free();

	return TRUE;
}



/*------------------------------------------------------------------------------*
| <<< asort �œǂݍ��񂾃��b�V����� >>>
|	����	iMesh = ���b�V���ԍ�
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
| <<< asort �œǂݍ��񂾃��b�V����� >>>
*------------------------------------------------------------------------------*/
void	asort_mesh_free(void)
{

	for(int i = 0; i < iAsortMeshCount; i++)
	{
		asort_mesh_free(i);
	}
}



/*------------------------------------------------------------------------------*
| <<< asort �p�ɓǂݍ��񂾃��b�V���`�� >>>	���\�[�g���܂���
|	����	iMesh      = ���b�V���ԍ�
|			*pmtxWorld = ���[���h�s��
*------------------------------------------------------------------------------*/
void	asort_mesh_draw(int iMesh, MTX *pmtxWorld)
{

	d3_stat_fvf_set(	  D3_FVF_LX);
	d3_stat_transform_set(pmtxWorld);
	d3_tex_set(			  iAsortTex);

	// �|���S���ꖇ��������`��
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
| <<< asort �p�Ƀ��b�V����`��o�^ >>>
|	����	iMesh      = ���b�V���ԍ�
|			*pmtxWorld = ���[���h�s��
*------------------------------------------------------------------------------*/
void	asort_mesh_set(int iMesh, MTX *pmtxWorld)
{

	D3_VTX_LX	avtx[3];

	avtx[0].c = D3_COL_NORMAL;
	avtx[1].c = D3_COL_NORMAL;
	avtx[2].c = D3_COL_NORMAL;

	// �|���S�����ꖇ�����[���h�g�����X�t�H�[�����ˉe�ϊ�
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


