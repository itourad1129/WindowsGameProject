//
//	���^�Z�R�f�[�^�ǂݍ���
//


//--- �萔 -----------------------------------------------------------------
#undef	SCALE
#define	SCALE		0.01f					// �f�t�H���g�̃X�P�[��

#define	INDEX_MAX	65535					// �ő咸�_��

#define	FACE_MAX	65535					// �\�ʐ�
#define	TEX_MAX		255						// �e�N�X�`���[�ő吔
#define	VERTEX_MAX	65535					// ���_��


//--- �\���� ----------------------------------------------------------------
struct	META_TEX
{
	float		afCol[4];					// �}�e���A����{�F
	float		fDif;						// �g�U��
	float		fAmb;						// ���͌�
	float		fEmi;						// ���ȏƖ�
	float		fSpc;						// ���ˌ�
	float		fPower;						// ���ˌ��̋���

	char		strTex[ MAX_PATH];			// �e�N�X�`���[��
	char		strAlp[ MAX_PATH];			// �����}�b�v��
	char		strBump[MAX_PATH];			// �o���v�}�b�v��
	MATERIAL	mat;						// �}�e���A��
};

struct	META_WORK
{
	int				iObjNum;					// ��͒��̃I�u�W�F�N�g�ԍ�
	int				iMatCount;					// �}�e���A����
	D3DXMATERIAL	amat[    TEX_MAX];
	VEC3			avecPos[ VERTEX_MAX];		// ���_
	int				iPosCount;					// ���_�̓o�^��
	int				iPosBase;					// ���_�̃x�[�X�J�E���g(�I�u�W�F�N�g���Ƀx�[�X���͕ω�����)

	//--- ���_�o�b�t�@�]���p ----------
	D3_VTX_DEFAULT	avtx[    VERTEX_MAX];		// ���_
	uint			uiVtxCount;					// ���_�̓o�^��
	uint			auiIndex[INDEX_MAX];		// �C���f�b�N�X�o�b�t�@
	uchar			aucMat[  INDEX_MAX / 3];	// �}�e���A���o�b�t�@
	uint			uiIndexCount;				// �C���f�b�N�X��
	//---------------------------------

	META_TEX		atex[    TEX_MAX];			// �e�N�X�`���[���
	META_TEX		*pTex;						// ���W���̃e�N�X�`���[���
	int				iMatID;						// ���W���̃}�e���A���ԍ�
	int				iPolyCount;					// �ʐ�
	LPD3DXMESH		lpMesh;						// ���b�V���̊i�[�ꏊ
	int				iMirrorType;				// ���ʃ^�C�v(0 > �I��)
	float			fSmooth;					// �X���[�W���O�l(���̒l�����Ē��_�@���̒l���~�b�N�X���邩���߂�)

	//--- meta_buf_load �p
	D3_VTX_DEFAULT	*pvtxBuf;					// ���g�p���� NULL �ł�

	//--- �f�o�b�O�p
	#ifdef	_DEBUG
		char		strName[256];				// ��͒��̃I�u�W�F�N�g
	#endif
};


//--- �ϐ� -----------------------------------------------------------------
static	char		*MetaText;					// �e�L�X�g�f�[�^�ǂݍ��݈ʒu
static	int			iMetaCount;					// �ǂݍ��ݒ��̕����ʒu
static	META_WORK	*meta;
static	BOOL		bVisibleSkip = true;	// ����Ԃ�L���ɂ���
		void		*meta_visible_func;		// ����ԏ��𓾂鎞�Ɏg�p
		void		*meta_obj_name_func;	// �I�u�W�F���𓾂鎞�Ɏg�p

//--- �R�}���h�e�[�u�� -----------------------------------------------------
static	char	MetaCommandTbl[][16] = 
{
	" ",					// 00 :	���p�X�y�[�X
	"\t",					// 01 :	�^�u
	"\r\n",					// 02 :	�{���̉��s�R�[�h
	"Scene {",				// 03 :	�V�[���i����͓ǂݔ�΂��j
	"vertex ",				// 04 :	Object�`�����N�A���_���ǂݍ���
	"face ",				// 05 :	face  �`�����N�ǂݍ���
	"tex(",					// 06 :	�e�N�X�`���[���o�^
	"bump(",				// 07 :	�o���v�}�b�v���o�^
	"Material",				// 08 :	�}�e���A�����o�^
	"col(",					// 09 :	�J���[�o�^
	"dif(",					// 10 :	�g�U���o�^
	"amb(",					// 11 :	���͌��o�^
	"emi(",					// 12 :	���ȏƖ��o�^
	"spc(",					// 13 :	���ˌ��o�^
	"power(",				// 14 :	���˂̋����o�^
	"Object ",				// 15 :	�I�u�W�F�N�g
	"mirror_axis",			// 16 :	�~���[�^�C�v
	"mirror ",				// 17 :	�~���[
	"facet ",				// 18 : �X���[�W���O
	"",						// �R�}���h�`�F�b�N�̏I�[�ɂȂ�
};


//--- �֐��鉺 ----------------------------------------------------
static	BOOL	CommandCheck(BOOL bUV);
static	void	CreateMeshFromMetaseq(void);



/*------------------------------------------------------------------------------*
| <<< ������̐擪�ɂ���󔒁A�^�u��i�߂� >>>
|	����	*pstrBuf = ������
|			piPos    = �Q�ƈʒu
|	�߂�l	�����������
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
| <<< strBuf ��蕶����𓾂� >>>
|	��"(����ٸ��ð���)�Ŏn�܂�A" �ŏI��
|	����	strBuf = �Ώە�����
|	�o��	strOut = ���o����������
|	�߂�l	���o����������
*------------------------------------------------------------------------------*/
static	int		GET_STRING(char	*strOut, char *strBuf)
{
	
	if(sys.bError){ return 0;}				// �G���[���o�Ă���ꍇ�͏������Ȃ�

	int	i, iLen	= 0;

	//--- �擪�� " ��T�� -----------------------------------------
	SPACE_DEL(strBuf, &iLen);
	if(strBuf[iLen]	== '"') 
	{
		iLen +=	1;
	}
	else
	{
		return 0;
	}

	//--- ���� " ��T�� -----------------------------------------
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
	
	//--- NULL ����	-----------------------------------------------
	if(iLen	> 0) strOut[i] = NULL; 
	return iLen;
}



/*------------------------------------------------------------------------------*
| <<< strBuf ��胉�x�����擾���� >>>
|	���A���_�[�o�[�A���p�p���A�S�p�����݂̂��擾����B����ȊO�Ȃ�ΏI��
|	����	strBuf = �Ώە�����
|	�o��	strOut = ���o����������
|	�߂�l	���o����������
*------------------------------------------------------------------------------*/
#define	GET_LABEL	LABEL_GET
static	int		LABEL_GET(char *strOut,	char *strBuf)
{

	if(sys.bError) return 0;				// �G���[���o�Ă���ꍇ�͏������Ȃ�

	int	i, iLen	= 0;

	SPACE_DEL(strBuf, &iLen);				// �擪�̋󔒂����
	for(i =	0;;	iLen++)						// strOut �Ƀ��x�������o��
	{
		uchar	a =	strBuf[iLen];
		if((uchar)(a - '0')	<=	9){	strOut[i] =	a; i++;	continue;}	// �������H
		if((uchar)(a - 'a')	<  26){	strOut[i] =	a; i++;	continue;}	// ���������H
		if((uchar)(a - 'A')	<  26){	strOut[i] =	a; i++;	continue;}	// �啶�����H
		if((uchar)a		   == '_'){	strOut[i] =	a; i++;	continue;}	// �A���_�[�o�[���H
		if(ISKANJI(a))												// �S�p�H
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
| <<< strBuf ��萔�l�𓾂�	>>>
|	����	*Buf = ������̈ʒu
|	�o��	fOut = �������蓾�����l
|	�߂�l	���o����������
*------------------------------------------------------------------------------*/
static	int		PARAM_GET(float	*fOut, char	*strBuf)
{

	if(sys.bError){ return 0;}				// �G���[���o�Ă���ꍇ�͏������Ȃ�

	float	fAns      =	0.f;
	float	fFlag     =	1.f;
	int		iLen      =	0;					// ������
	int		iSpaceLen = 0;

	iSpaceLen += SPACE_DEL(strBuf, &iLen);	// �擪�̋󔒂����

	if(strBuf[iLen]	== '-')					// �ꕶ���ڂ̓}�C�i�X���H
	{
		fFlag  = -1;
		iLen++;
	}
	int	i;
	for(i =	0;;	i++)						// �������������o���B
	{
		uchar	a =	strBuf[iLen] - '0';		// �ꕶ���Q�b�g���ĕ����̂O�������Ă݂�
		if(a < 10)							// ���l�̂P�O�ȉ��Ȃ�΂���͕����ɂȂ�B
		{
			fAns  *= 10;					// �O�̒l���P�O�{����
			fAns  += a;						// �����l�𑫂��B
			iLen++;							// ���̕����֐i�߂�B
		}
		else
		{
			break;							// ��������Ȃ����炱����for��������
		}
	}
	if(strBuf[iLen]	!= '.')
	{
		iSpaceLen += SPACE_DEL(strBuf, &iLen);		// ���̋󔒂����
		*fOut =	fAns * fFlag;
		if(iLen	== 0 || iSpaceLen == iLen){ sys.bError = true;}
		return iLen;								// �����_�łȂ��Ȃ�A���l��Ԃ��Ă����܂��B
	}
	float	fMulti = 0.1f;
	iLen++;
	for(;;i++)								// �������������o���B
	{										// �ꕶ���Q�b�g���ĕ����̂O�������Ă݂�
		uchar	a =	(uchar)(strBuf[iLen] - '0');
		if(a < 10)							// ���l�̂P�O�ȉ��Ȃ�΂���͕����ɂȂ�B
		{
			fAns   += (float)a * fMulti;	// �����l��multi�{���đ����B
			fMulti *= 0.1f;					// multi������1/10����i����������i�߂�j�B
			iLen++;							// ���̕����֐i�߂�B
		}
		else
		{
			break;							// ��������Ȃ����炱����for��������
		}
	}

	if(iLen	== 0){ sys.bError = true;}

	SPACE_DEL(strBuf, &iLen);				// ���̋󔒂����
	*fOut =	fAns * fFlag;
	return iLen;
}



/*-------------------------------------------------------------------------------*
| <<< �e�L�X�g�ǂݍ��� >>>
|	����	*strFile = �ǂݍ��݃t�@�C����
*-------------------------------------------------------------------------------*/
static	BOOL	TextLoad_(char *strFile)
{

	HANDLE	Handle;						// �ǂݍ��݃n���h��
	DWORD	rs;							// �_�~�[
	int		iSize;						// �ǂݍ��݃T�C�Y

	//--- �������[�J�� ----------------------------------------------
	if(MetaText	!= NULL){ free(MetaText); MetaText = NULL;}

	//--- �t�@�C�����J�� --------------------------------------------
	Handle = CreateFile(strFile, GENERIC_READ, 0, NULL,	OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL |	FILE_FLAG_RANDOM_ACCESS, NULL);
	if(Handle == INVALID_HANDLE_VALUE)
	{
		return sys_error("%s ���J���܂���B", strFile);
	}

	//--- �T�C�Y���擾���� ------------------------------------------
	iSize =	GetFileSize(Handle,	NULL);
	if(iSize ==	0){	CloseHandle(Handle); return	sys_error("%s �T�C�Y���O�o�C�g�ł��B", strFile);}

	//--- �������[���m�ۂ��� ----------------------------------------
	iSize   += 1;									// +1 ���� NULL	��ǉ�
	MetaText = (char *)malloc(iSize);
	if(MetaText	== NULL){ CloseHandle(Handle); return sys_error("%s ��ǂݍ��ݒ��̃������[�̊m�ۂɎ��s���܂����B", strFile);}
	memset(MetaText, 0,	iSize);						// �[���N���A�[

	//--- �ǂݍ��� --------------------------------------------------
	ReadFile(Handle, MetaText, iSize, &rs, NULL);

	//--- �t�@�C������� ------------------------------------------
	CloseHandle(Handle);

	//--- ���[�N������ ----------------------------------------------
	meta->uiVtxCount = 0;
	return true;
}



/*-----------------------------------------------------------------------------------------*
| <<< �R�}���h : Scene { >>>				�� �V�[���͑S�ēǂݔ�΂����
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
| <<< �R�}���h : vertex	>>>
*-----------------------------------------------------------------------------------------*/
static	void	ComVertex(void)
{
	float	fAns;							// ���_��
	iMetaCount += PARAM_GET(&fAns, &MetaText[iMetaCount]);
	iMetaCount += 3;						// "{" �Ɖ��s�����ړ�������

	// �x�[�X�ݒ�
	meta->iPosBase = meta->iPosCount;

	//--- ���_�f�[�^��ۑ� ------------------------------------------
	VEC3	*p = &meta->avecPos[meta->iPosCount];
	for(int	i =	0; i < (int)fAns; i++, p++)
	{
		iMetaCount += PARAM_GET(&p->x, &MetaText[iMetaCount]);
		iMetaCount += PARAM_GET(&p->y, &MetaText[iMetaCount]);
		iMetaCount += PARAM_GET(&p->z, &MetaText[iMetaCount]);
		p->z	   *= -1;
		*p		   *= SCALE;				// �X�P�[�������킹��
		iMetaCount += 2;					// ���s��i�߂�

		meta->iPosCount++;					// �o�^�����₷
	}
}



/*-----------------------------------------------------------------------------------------*
| <<< ���_�o�^ >>>
|	����	pvecPos    = ���_
|			pvecNormal = ���_�@��
|			pvecUV     = UV
|	�߂�l	�o�^�o�b�t�@�ԍ�
*-----------------------------------------------------------------------------------------*/
static	int		EntryVertexBuf(VEC3 *pvecPos, VEC3 *pvecNormal, VEC2 *pvecUV)
{

	// ���^�Z�R�C���̒��_�f�[�^�� UV ���܂܂�Ă��Ȃ����ADirectX �̃��b�V���f�[�^�� UV ��K�v�Ƃ���̂ŁA
	// ���_ + UV �Ƃ��Ē��_�f�[�^��o�^����B���̎��A�����񂪂Ȃ����`�F�b�N����

	//--- �@������ --------------------------------------------------
	// ���꒸�_��T���āA�@�������Z
	VEC3	nor = *pvecNormal;
	uint	i;
	for(i =	0; i < meta->uiVtxCount; i++)
	{
		if(meta->avtx[i].p == *pvecPos)
		{
			// UV �����鎞
			if(pvecUV != NULL)
			{
				// UV �� �@��������̂��̂�����
				if(meta->avtx[i].n == nor && meta->avtx[i].uv == *pvecUV){ break;}
			}
			else
			{
				// �@��������̂��̂�����
				if(meta->avtx[i].n == nor){ break;}
			}
			// �~�b�N�X����@�����m�̊p�ƃX���[�W���O�l���r
			if(meta->fSmooth > calc_vec3_vec_to_vec_angle(&meta->avtx[i].n, &nor))
			{
				// �@�����m�̊p���X���[�W���O�l��菬�����ꍇ�͖@�����~�b�N�X����
				meta->avtx[i].n += nor;
				calc_vec3_normalize(&meta->avtx[i].n, &meta->avtx[i].n);
				nor				 = meta->avtx[i].n;
			}
		}
	}

	//--- �o�b�t�@�ɓ����񂪂Ȃ������ׂ� --------------------------
	for(i =	0; i < meta->uiVtxCount; i++)
	{
		// UV �����鎞
		if(pvecUV != NULL)
		{
			if(meta->avtx[i].p == *pvecPos && meta->avtx[i].uv == *pvecUV)
			{
				meta->uiVtxCount++;
				return i;
			}
		}
		// UV ��������
		else
		{
			if(meta->avtx[i].p == *pvecPos)
			{
				meta->uiVtxCount++;
				return i;
			}
		}
	}

	//--- �V�K�o�^ --------------------------------------------------
	if(i ==	meta->uiVtxCount && i < VERTEX_MAX)
	{
		meta->avtx[i].p  = *pvecPos;		// ���_
											// UV
		if(pvecUV != NULL){	meta->avtx[i].uv = *pvecUV;}
		meta->avtx[i].n  = nor;				// �@��(�V�K�o�^�A���ڈȍ~�͉��Z)
		
		calc_vec3_normalize(&meta->avtx[i].n, &meta->avtx[i].n);

		meta->uiVtxCount++;
		return i;
	}



#if 0

	// ���^�Z�R�C���̒��_�f�[�^�� UV ���܂܂�Ă��Ȃ����ADirectX �̃��b�V���f�[�^�� UV ��K�v�Ƃ���̂ŁA
	// ���_ + UV �Ƃ��Ē��_�f�[�^��o�^����B���̎��A�����񂪂Ȃ����`�F�b�N����

	//--- �@������ --------------------------------------------------
	// ���꒸�_��T���āA�@�������Z
	VEC3	nor    = *pvecNormal;
	uint	i;

	for(i =	0; i < meta->uiVtxCount; i++)
	{
		if(meta->avtx[i].p == *pvecPos)
		{
			// UV �����鎞
			if(pvecUV != NULL)
			{
				// UV �� �@��������̂��̂�����
				if(meta->avtx[i].n == nor && meta->avtx[i].uv == *pvecUV)
				{
					return i;
				}
			}
			else
			{
				// �@��������̂��̂�����
				if(meta->avtx[i].n == nor)
				{
					return i;
				}
			}
		}
	}
	
	//--- �o�b�t�@�ɓ����񂪂Ȃ������ׂ� --------------------------
	/*for(i =	0; i < meta->uiVtxCount; i++)
	{
		// UV �����鎞
		if(pvecUV != NULL)
		{
			if(meta->avtx[i].p == *pvecPos && meta->avtx[i].uv == *pvecUV)
			{
				return i;
			}
		}
		// UV ��������
		else
		{
			if(meta->avtx[i].p == *pvecPos)
			{
				return i;
			}
		}
	}*/

	//--- �V�K�o�^ --------------------------------------------------
	if(i ==	meta->uiVtxCount && i < VERTEX_MAX)
	{
		meta->avtx[i].p  = *pvecPos;		// ���_
											// UV
		if(pvecUV != NULL){	meta->avtx[i].uv = *pvecUV;}
		meta->avtx[i].n  = nor;				// �@��(�V�K�o�^�A���ڈȍ~�͉��Z)
		
		calc_vec3_normalize(&meta->avtx[i].n, &meta->avtx[i].n);
		meta->uiVtxCount++;
		return i;
	}
#endif


	//--- �o�^�s�� --------------------------------------------------
	return -1;
}



/*-----------------------------------------------------------------------------------------*
| <<< �ʓo�^�T�u >>>
|	����	pfIndex  = �C���f�b�N�X�o�b�t�@
|			pfUV	 = UV �o�b�t�@
|			uiMat    = �}�e���A���F
|			bUV	�@	 = TRUE : UV �g�p
|			bSquare	 = �S���_�|���S��
|			uiMirror = 0 : �ʏ�o�^
|					   1 : X �~���[( X ���W�𔽓]���ēo�^)
|					   2 : Y �~���[( Y ���W�𔽓]���ēo�^)
|					   3 : Z �~���[( Z ���W�𔽓]���ēo�^)
*-----------------------------------------------------------------------------------------*/
static	void	FaceSub(float *pfIndex, float *pfUV, uint uiMat, BOOL bUV, BOOL bSquare, uint uiMirror)
{

	//--- ���_�𓾂� ------------------------------------------------
	int		j  = meta->uiIndexCount;
	VEC3	v0 = meta->avecPos[(int)pfIndex[0]];
	VEC3	v1 = meta->avecPos[(int)pfIndex[1]];
	VEC3	v2 = meta->avecPos[(int)pfIndex[2]];
	VEC3	v3 = meta->avecPos[(int)pfIndex[3]];

	//--- �~���[ ----------------------------------------------------
	BOOL	bMirror = false;
	switch(uiMirror)
	{
	// X �~���[
	case 1:
		v0.x   *= -1;
		v1.x   *= -1;
		v2.x   *= -1;
		v3.x   *= -1;
		bMirror = TRUE;
		break; 
	// Y �~���[
	case 2:
		v0.y   *= -1;
		v1.y   *= -1;
		v2.y   *= -1;
		v3.y   *= -1;
		bMirror = TRUE;
		break; 
	// Z �~���[
	case 3:
		v0.z   *= -1;
		v1.z   *= -1;
		v2.z   *= -1;
		v3.z   *= -1;
		bMirror = TRUE;
		break; 
	}

	//--- ���_�@���v�Z ----------------------------------------------
	VEC3	vecNormal;
	if(bMirror){ calc_vec3_cross(&vecNormal, &(v2 - v0), &(v1 - v2));}
	else	   { calc_vec3_cross(&vecNormal, &(v1 - v0), &(v2 - v1));}
	calc_vec3_normalize(&vecNormal, &vecNormal);

	//--- ���_�o�^ --------------------------------------------------
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

	//--- �l�p�`�̏ꍇ ----------------------------------------------
	if(bSquare)
	{
		// ���_�@���v�Z
		if(bMirror){ calc_vec3_cross(&vecNormal, &(v3 - v0), &(v1 - v3));}
		else	   { calc_vec3_cross(&vecNormal, &(v1 - v0), &(v3 - v1));}

		// ���_�o�^
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
| <<< �R�}���h : face >>>
|	����	bUV = false : uv �̃`�F�b�N���s��Ȃ�(�@���}�b�v���쐬����ۂɎg�p)
*-----------------------------------------------------------------------------------------*/
static	void	ComFace(BOOL bUV)
{
// face�`�����N����\�ʐ���ǂݎ��A���̕������ʂ��쐬����
// V(0 1 3 2) �� MvtSub[0],	MvtSub[1], MvtSub[3], MvtSub[2]	�̒��_��񂩂�Ȃ�ʂł���
// meta[���^�Z�R�t�@�C���ԍ�].face[�o�^�\�ʐ�]

	float		fFaceCount;					// �ʐ�
	iMetaCount += PARAM_GET(&fFaceCount, &MetaText[iMetaCount]);

	iMetaCount += 3;						// "{" �Ɖ��s���ړ�������

	float	a, afIndex[4], afUV[8];

	afIndex[3] = 0;
	//--- �\�ʃf�[�^��ۑ� ------------------------------------------
	for(int	i =	0; i < (int)fFaceCount; i++)
	{
		//--- ���_�� ------------------------------------------------
		float	fVertexCount;
		iMetaCount += PARAM_GET(&fVertexCount, &MetaText[iMetaCount]);

		//--- V() ---------------------------------------------------
		iMetaCount +=2;						// V( ��i�߂�
											// ���ʐ����v�Z�A�l�p�`�͂Q�ʂƂȂ�
		int	iPoly;
		if(fVertexCount	== 4){ iPoly = 2;}
		else				 { iPoly = 1;}
		
		// ���_�ԍ��𓾂�
		for(int j =	0; j < (int)fVertexCount; j++)
		{
			iMetaCount += PARAM_GET(&afIndex[j], &MetaText[iMetaCount]);
			afIndex[j] += meta->iPosBase;
		}
		iMetaCount++;						// �J�b�R����

		//--- M() ---------------------------------------------------
		SPACE_DEL(MetaText,	&iMetaCount);
		uint	uiMat = 0;
		if(MetaText[iMetaCount]	== 'M')
		{
			iMetaCount += 2;				// �ގ����
			iMetaCount += PARAM_GET(&a, &MetaText[iMetaCount]);
			uiMat	    = (int)a;
			iMetaCount++;					// �J�b�R����
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
				//while(afUV[j] > 1){ afUV[j] -= 1;}	// UV �� 0 �` 1 �Ɋۂ߂�
			}
			iMetaCount += 3;
			bUV         = TRUE;
		}
		else
		{
			iMetaCount += 2;
		}

		//--- �o�b�t�@�o�^ ------------------------------------------
		FaceSub(afIndex, afUV, uiMat, bUV, iPoly == 2, 0);
	
		if(meta->iMirrorType)
		{
			FaceSub(afIndex, afUV, uiMat, bUV, iPoly == 2, meta->iMirrorType);
		}

		//--- �|���������Z ------------------------------------------
		if(meta->iMirrorType){ meta->iPolyCount += iPoly * 2;}
		else			     { meta->iPolyCount += iPoly;}
	}
	iMetaCount		 +=	3;
	meta->iMirrorType =	0;
}



/*-----------------------------------------------------------------------------------------*
| <<< �R�}���h : tex�Abump >>>
|	����	iType =	0 :	�e�N�X��[�A1 :	�p���v
*-----------------------------------------------------------------------------------------*/
static	void	ComTexBump(int iType)
{
	//--- Material { �����`�F�b�N ---------------------------------
	if(meta->pTex == NULL)
	{
		sys.bError = true;
		sys_error("meta.pTex �� NULL �ł��Bcol �̐ݒ肪 Material { �̊O�ōs���Ă��܂��B"); return;
	}
	//--- �t�@�C�����𓾂� ----------------------------------------
	char	strFile[MAX_PATH + 1];
	memset(strFile,	0, sizeof(strFile));	// ���݂�����Ȃ��悤�ɃN���A�[
	iMetaCount += GET_STRING(strFile, &MetaText[iMetaCount]);

	//--- �t�@�C������o�^ ----------------------------------------------------
	switch(iType)
	{
	case 0:	strcpy(meta->atex[meta->iMatID].strTex,	 strFile); break;
	case 1:	strcpy(meta->atex[meta->iMatID].strBump, strFile); break;
	}
	iMetaCount++;
}



/*-----------------------------------------------------------------------------------------*
| <<< �R�}���h : Material >>>
|	����	bUV = false : uv �̃`�F�b�N���s��Ȃ�(�@���}�b�v���쐬����ۂɎg�p)
*-----------------------------------------------------------------------------------------*/
static	void	ComMaterial(BOOL bUV)
{
	float	a;

	iMetaCount	   += PARAM_GET(&a,	&MetaText[iMetaCount]);
	meta->iMatCount	= (int)a;				// �}�e���A������
	iMetaCount	   += 1;					// "{" ���ړ�
											// �e�N�X�`���[�f�[�^�̏������݈ʒu��ݒ�
	meta->pTex		= &meta->atex[meta->iMatID];
	//--- �e�p�����[�^���o ------------------------------------------
	while(MetaText[iMetaCount] != '}')
	{										// ���s����������A���ւ�����
		char	str[512];

		if(MetaText[iMetaCount]	== '\r'	&& MetaText[iMetaCount + 1]	== '\n')
		{
			meta->iMatID++;					// �}�e���A���ǂݍ��ݏI���Ȃ̂ŁA���֐i�߂�
											// �e�N�X�`���[�f�[�^�̏������݈ʒu��ݒ�
			meta->pTex  = &meta->atex[meta->iMatID];

			iMetaCount += 2;
			//--- �}�e���A�����𓾂� --------------------------------
			int		i  = GET_STRING(str, &MetaText[iMetaCount]);

			if(meta_obj_name_func != NULL)	// �I�u�W�F�����O���֓n��
			{
				void(*func)(int iNum, char *str);
				func = (void(*)(int iNum, char *str))meta_obj_name_func;
				(*func)(meta->iMatID, str);
			}
			iMetaCount += i;
			//-------------------------------------------------------
		}

		//--- �R�}���h�`�F�b�N --------------------------------------
		BOOL	bRet = CommandCheck(bUV);

		//--- ���p�E�S�p�`�F�b�N ------------------------------------
		if(!bRet)
		{
			//char	str[512];				// ����ٸ��ð��ݒ��� �R�}���h�������Ă���̂�h�����߁A��������B
			int		i   = GET_STRING(str, &MetaText[iMetaCount]);
			iMetaCount += i;
			if(i ==	0)
			{
				if(ISKANJI(MetaText[iMetaCount])){ iMetaCount += 2;}	// �S�p
				else							 { iMetaCount += 1;}	// ���p
			}
		}
	}
	iMetaCount++;
	meta->pTex = NULL;						// �e�N�X�`���[�f�[�^�̏������݈ʒu���N���A�[
}



/*-----------------------------------------------------------------------------------------*
| <<< �R�}���h : col >>>
*-----------------------------------------------------------------------------------------*/
static	void	ComCol(void)
{
	if(meta->pTex == NULL)
	{
		sys.bError = true;
		sys_error("meta.pTex �� NULL �ł��Bcol �̐ݒ肪 Material { �̊O�ōs���Ă��܂��B"); return;
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
| <<< �R�}���h : dif, amb, emi,	spc, power >>>
|	����	iType =	0 :	dif�A1 : amb�A2	: emi�A3 : spc�A4 :	power
*-----------------------------------------------------------------------------------------*/
static	void	ComDifAmbEmiSpcPower(int iType)
{
	if(meta->pTex == NULL)
	{
		sys.bError = true;
		sys_error("meta.pTex �� NULL �ł��B�F�̐ݒ肪 Material { �̊O�ōs���Ă��܂��B");
		return;
	}
	else
	{										// �|�C���^���g���������l����
		float	*f  = &meta->pTex->fDif;
		iMetaCount += PARAM_GET(&f[iType], &MetaText[iMetaCount]);
		iMetaCount++;						// } ��i�߂�
	}
}



/*-----------------------------------------------------------------------------------------*
| <<< �R�}���h : Object	 >>>				�����A�s�����`�F�b�N���A�s���Ȃ�ǂݔ�΂�
*-----------------------------------------------------------------------------------------*/
static	void	ComObject(void)
{

	char	str[512];							// ����ٸ��ð��ݒ��� �R�}���h�������Ă���̂�h�����߁A��������B
	//--- �ŏ��̃J�b�R�܂ŕ�����i�߂� ------------------------------
	//Text[iCount] = Text[iCount];
	while(MetaText[iMetaCount] != '{')
	{
		int		i   = GET_STRING(str, &MetaText[iMetaCount]);
		iMetaCount += i;
		if(i ==	0)
		{
			if(ISKANJI(MetaText[iMetaCount])){ iMetaCount += 2;}	// �S�p
			else					         { iMetaCount += 1;}	// ���p
		}
		/*
		if(meta_obj_name_func != NULL)			// �I�u�W�F�����O���֓n��
		{
			void(*func)(int iNum, char *str);
			func = (void(*)(int iNum, char *str))meta_obj_name_func;
			(*func)(meta->iObjNum, str);
		}*/
	}
	iMetaCount++;

	//--- �Ō�̃J�b�R�܂ŕ�����i�߂� ------------------------------
	#ifdef	_DEBUG
		strcpy(meta->strName, str);
	#endif

	char	*a = strstr(&MetaText[iMetaCount], "visible");	// visible �܂ŕ�����i�߂�
	iMetaCount += (a - &MetaText[iMetaCount]) + 8;
	//iCount += 11;

	if(meta_visible_func != NULL)							// �������O���֓n��(���݁A������)
	{
		void(*func)(int iNum, BOOL b);
		func = (void(*)(int iNum, BOOL b))meta_visible_func;
		//(*func)(meta->iObjNum, Text[iCount] != '0');
		(*func)(meta->iObjNum, true);						// ���݂͕K�����Ƃ��ēn��
	}

	if(!bVisibleSkip || MetaText[iMetaCount] != '0'){ return;}// visible	���`�F�b�N����

	iMetaCount++;
	for(;;)
	{
		char	str[512];							// ����ٸ��ð��ݒ��� �R�}���h�������Ă���̂�h�����߁A��������B
		int		i   = GET_STRING(str, &MetaText[iMetaCount]);
		iMetaCount += i;
		if(i ==	0)
		{											// �S�p
			if(ISKANJI(MetaText[iMetaCount]))
			{
				iMetaCount += 2;
			}
			else
			{
				if(MetaText[iMetaCount]	== '\r'	&& MetaText[iMetaCount + 1]	== '\n'	&& MetaText[iMetaCount + 2]	== '}'){ break;}
				iMetaCount += 1;					// ���p
			}
		}
	}
	iMetaCount += 3;
}



/*-----------------------------------------------------------------------------------------*
| <<< �R�}���h : mirror_axis >>>
*-----------------------------------------------------------------------------------------*/
static	void	ComMirrorAxis(void)
{
	float	fAns;
	iMetaCount		 +=	PARAM_GET(&fAns, &MetaText[iMetaCount]);
	meta->iMirrorType =	(int)fAns;
}



/*-----------------------------------------------------------------------------------------*
| <<< �R�}���h : mirror	>>>
*-----------------------------------------------------------------------------------------*/
static	void	ComMirror(void)
{
	float	fAns;
	iMetaCount		 += PARAM_GET(&fAns, &MetaText[iMetaCount]);
	meta->iMirrorType = (int)fAns;
}



/*-----------------------------------------------------------------------------------------*
| <<< �R�}���h : facet	>>>					�� �X���[�W���O�̒l
*-----------------------------------------------------------------------------------------*/
static	void	ComFacet(void)
{
	iMetaCount   += PARAM_GET(&meta->fSmooth, &MetaText[iMetaCount]);
	meta->fSmooth = R(meta->fSmooth);
}



/*-----------------------------------------------------------------------------------------*
| <<< �R�}���h�`�F�b�N >>>
|	����	bUV = false : uv �̃`�F�b�N���s��Ȃ�(�@���}�b�v���쐬����ۂɎg�p)
|	�߂�l	true : ��x�ł��R�}���h�����s����
*-----------------------------------------------------------------------------------------*/
static	BOOL	CommandCheck(BOOL bUV)
{
	BOOL	bRet = false;

	for(int	i =	0;;i++)
	{
		if(sys.bError) return bRet;
		int	iLen = strlen(MetaCommandTbl[i]);	// ��strlen	= ������̒����𒲂ׂ�
		if(iLen	== 0) break;					// ������̒������O�Ȃ�I�[�Ƃ݂Ȃ�
		if(strncmp(&MetaText[iMetaCount], MetaCommandTbl[i], iLen) == 0)
		{
			iMetaCount += iLen;					// �R�}���h�̕�������i�߂�
			bRet	    = true;
			//sprintf(strErrorCommand, "%s �ɃG���[������܂��B", CommandTbl[i]);

			switch(i)
			{
			case 0:				// ���p�X�y�[�X
			case 1:				// �^�u
			case 2:				// �{���̉��s�R�[�h
				break;
			case 3:				// �V�[���i�ǂݔ�΂����Ɓj
				ComScene();					  break;
			case 4:				// Object�`�����N�A���_���ǂݍ���
				ComVertex();				  break;
			case 5:				// face�`�����N�ǂݍ���
				ComFace(bUV);				  break;
			case 6:				// �o���v�}�b�v�ǂݍ���
			case 7:				// �e�N�X�`���[�ǂݍ���
				ComTexBump(i - 6);			  break;
			case 8:				// �}�e���A���o�^
				ComMaterial(bUV);			  break;
			case 9:				// �J���[�o�^
				ComCol();					  break;
			case 10:			// �g�U���o�^
			case 11:			// ���͌��o�^
			case 12:			// ���ȏƖ��o�^
			case 13:			// ���ˌ��o�^
			case 14:			// ���˂̋����o�^
				ComDifAmbEmiSpcPower(i - 10); break;
			case 15:			// Object�`�����N
				meta->iObjNum++;
				ComObject();				  break;
			case 16:			// ���ʃ^�C�v
				ComMirrorAxis();			  break;
			case 17:			// ���ʃX�C�b�`
				ComMirror();				  break;
			case 18:			// �X���[�W���O
				ComFacet();					  break;
			}
			//i	= -1;
			return true;
		}
	}
	return bRet;
}



/*-----------------------------------------------------------------------------------------*
| <<< �e�L�X�g���C�� >>>
|	����	bUV = false : uv �̃`�F�b�N���s��Ȃ�(�@���}�b�v���쐬����ۂɎg�p)
*-----------------------------------------------------------------------------------------*/
static	void	TextMain_(BOOL bUV)
{

	while(MetaText[iMetaCount] != NULL)					// ������̏I�[�Ȃ�Ώ������Ȃ�
	{
		//--- �R�}���h�`�F�b�N --------------------------------------
		BOOL	bRet = CommandCheck(bUV);

		//--- ���p�E�S�p�`�F�b�N ------------------------------------
		if(!bRet)
		{
			char	str[4096];							// ����ٸ��ð��ݒ��� �R�}���h�������Ă���̂�h�����߁A��������B
			int		i   = GET_STRING(str, &MetaText[iMetaCount]);
			iMetaCount += i;
			if(i ==	0)
			{
				if(ISKANJI(MetaText[iMetaCount])){ iMetaCount += 2;}	// �S�p
				else							 { iMetaCount += 1;}	// ���p
			}
		}
	}
}



/*-----------------------------------------------------------------------------------------*
| <<< ���b�V���쐬 >>>
*-----------------------------------------------------------------------------------------*/
static	void	CreateMeshFromMetaseq(void)
{

	if(meta->iPolyCount	== 0){ return;}

	// �w�t�@�C������ǂݍ��ޏꍇ��
	// �����̂̏ꍇ�@�C���f�b�N�X��	40 ��
	//				 ���_���@�@�@	24 ��	�ƂȂ��Ă����B
	D3DXCreateMeshFVF(meta->iPolyCount,		// �\�ʐ�	���͓K��
					  meta->uiVtxCount,		// ���_��	���͓K��
					  D3DXMESH_MANAGED,		// ���b�V���̃I�v�V����
					  D3_FVF_DEFAULT,		// ���_�t�H�[�}�b�g
					  d3.dev,				// �f�o�C�X
					  &meta->lpMesh);		// �쐬���ꂽ���b�V��

	//--- ���_������������ ----------------------------------------
	D3_VTX_DEFAULT	*pvtxBuf;				// ���_�o�b�t�@
	WORD			*pwIndex;				// �C���f�b�N�X�o�b�t�@
	DWORD			*pdwMaterial;			// �}�e���A���o�b�t�@
	meta->lpMesh->LockVertexBuffer(   D3DLOCK_DISCARD, (void **)&pvtxBuf);
	meta->lpMesh->LockIndexBuffer(    D3DLOCK_DISCARD, (void **)&pwIndex);
	meta->lpMesh->LockAttributeBuffer(D3DLOCK_DISCARD, &pdwMaterial);

	// ���_�o�b�t�@�]��
	for(uint i = 0; i < meta->uiVtxCount; i++)
	{
		pvtxBuf[i].p  = meta->avtx[i].p;
		pvtxBuf[i].n  = meta->avtx[i].n;
		pvtxBuf[i].uv = meta->avtx[i].uv;
	}

	// �C���f�b�N�X�o�b�t�@�]��
	for(int i = 0; i < meta->iPolyCount * 3; i++)
	{
		pwIndex[i] = meta->auiIndex[i];
	}

	// �}�e���A���o�b�t�@�]��
	for(int i = 0; i < meta->iPolyCount; i++)
	{
		pdwMaterial[i] = meta->aucMat[i];
	}

	//--- ���_�o�b�t�@���� ----------------------------------------
	meta->lpMesh->UnlockAttributeBuffer();
	meta->lpMesh->UnlockIndexBuffer();
	meta->lpMesh->UnlockVertexBuffer();
}



/*-----------------------------------------------------------------------------------------*
| <<< �e�N�X�`���[�쐬 >>>
|	����	iNum	= �쐬���郁�b�V���ԍ�
|			bBump	= true : �o���v���쐬����
*-----------------------------------------------------------------------------------------*/
static	void	MetaTexCreate(int iNum, BOOL bBump)
{

	if(meta->iPolyCount	== 0){ return;}

	//--- ���^�Z�R�C���̓}�e���A�������̏ꍇ������̂ŁA���̑΍� ----
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
			//--- �}�e���A���f�[�^���ڂ� ----------------------------
			if(strlen(meta->atex[i].strTex) > 0){ meta->amat[i].pTextureFilename = meta->atex[i].strTex;}
												// �f�B�t�B�[�Y
			VEC4	*dif = (VEC4*)&meta->amat[i].MatD3D.Diffuse;
			meta->amat[i].MatD3D.Diffuse.a = meta->atex[i].afCol[0];
			meta->amat[i].MatD3D.Diffuse.r = meta->atex[i].afCol[1];
			meta->amat[i].MatD3D.Diffuse.g = meta->atex[i].afCol[2];
			meta->amat[i].MatD3D.Diffuse.b = meta->atex[i].afCol[3];

			VEC4	*a;							// �A���r�G���g
			a  = (VEC4*)&meta->amat[i].MatD3D.Ambient;
			*a = *dif *	meta->atex[i].fAmb;
												// �G�~�b�V�u
			a  = (VEC4*)&meta->amat[i].MatD3D.Emissive;
			*a = *dif *	meta->atex[i].fEmi;
												// �X�y�L�����[
			a  = (VEC4*)&meta->amat[i].MatD3D.Specular;
			*a = *dif *	meta->atex[i].fSpc;
												// �p���[
			meta->amat[i].MatD3D.Power = meta->atex[i].fPower;
		}
	}
}



/*-------------------------------------------------------------------------------*
| <<< ���^�Z�R�t�@�C���ǂݍ��� >>>
|	����	iNum	= �ǂݍ��ݔԍ�
|			strFile	= �ǂݍ���MQO�t�@�C����
|	�߂�l	true or	false
*-------------------------------------------------------------------------------*/
BOOL	meta_load(int iNum, char *strFile)
{

	iMetaCount = 0;							// �����ǂݍ��݈ʒu������

	if((meta = (META_WORK *)malloc(sizeof(META_WORK))) == NULL){ return sys_error("meta_load : �������[���s�����Ă��܂��B");}

	memset(meta, 0,	sizeof(META_WORK));		// ���[�N������
	meta->iObjNum = -1;
	sys_folder_push();
											// �e�L�X�g��ǂݍ���
	if(!TextLoad_(strFile)){ sys_folder_pop(); return false;}
											// ���s������I��
	// �e�L�X�g���
	meta->iMatID  = -1;
	meta->fSmooth = R(90);
	TextMain_(false);						// �e�L�X�g�������s�Ȃ�
	CreateMeshFromMetaseq();				// ���^�Z�R�C���̃f�[�^��胁�b�V���쐬
	FOLDER_SET(strFile);
	MetaTexCreate(iNum, false);				// �e�N�X�`���[�쐬

	// FVF �ϊ�
	if(meta->uiVtxCount > 0 && meta->iPolyCount > 0)
	{
		meta->lpMesh->CloneMeshFVF(meta->lpMesh->GetOptions(), meta->lpMesh->GetFVF(), d3.dev, &d3.lpMesh[iNum]);

		// �����o�^
		d3.meshInfo[iNum].ucSubset			 = meta->iMatCount;	// �T�u�Z�b�g��
		d3.meshInfo[iNum].ucSkinAnimationSet = 0;				// �A�j���[�V�����g���b�N��
		d3.meshInfo[iNum].usFaces			 = meta->iPolyCount;

		// �}�e���A���쐬
		if(meta->iMatCount > 0){ d3.matMesh[iNum] = new MATERIAL[meta->iMatCount];}

		// �e�N�X�`���[���[�h
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
		
		// �q�b�g�{�b�N�X�̍쐬
		d3_hit_work_get(&d3.hitWork[iNum], d3.lpMesh[iNum]);
	}

	// ���������
	if(MetaText	!= NULL)						// �������[�����
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
| <<< ���^�Z�R�t�@�C���ǂݍ��ݎ��ɉ������X�L�b�v���邩�̐ݒ� >>>
|	����	bSw = true  : �����L���ɂ���(�f�t�H���g)
|				  false : ����񖳌��ɂ���
*-------------------------------------------------------------------------------*/
void	meta_visible_skip_set(BOOL bSw)
{
	bVisibleSkip = bSw;
}



/*-------------------------------------------------------------------------------*
| <<< ���^�Z�R�t�@�C����ǂݍ��݂��o�b�t�@��� >>>
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
| <<< ���^�Z�R�t�@�C������͂��ăo�b�t�@�֊i�[���� >>>
|	����	strFile	= �ǂݍ���MQO�t�@�C����
|	�߂�l	true or	false
*-------------------------------------------------------------------------------*/
BOOL	meta_buf_load(char *strFile)
{

	iMetaCount = 0;							// �����ǂݍ��݈ʒu������

	if((meta = (META_WORK *)malloc(sizeof(META_WORK))) == NULL){ return sys_error("meta_load : �������[���s�����Ă��܂��B");}

	memset(meta, 0,	sizeof(META_WORK));		// ���[�N������
	meta->iObjNum = -1;
	sys_folder_push();
											// �e�L�X�g��ǂݍ���
	if(!TextLoad_(strFile)){ sys_folder_pop(); return false;}
											// ���s������I��
	// �e�L�X�g���
	meta->iMatID  = -1;
	meta->fSmooth = R(90);
	TextMain_(false);						// �e�L�X�g�������s�Ȃ�

	// ���_�o�b�t�@�ɂ��ׂẴ|���S���������o��
	if((meta->pvtxBuf = (D3_VTX_DEFAULT *)malloc(sizeof(D3_VTX_DEFAULT) * meta->iPolyCount)) == NULL)
	{
		return sys_error("meta_load2 : �������[���s�����Ă��܂��B");
	}
	for(int i = 0; i < meta->iPolyCount * 3; i++)
	{
		meta->pvtxBuf[i] = meta->avtx[meta->auiIndex[i]];
	}
	sys_folder_pop();

	// ���������
	if(MetaText	!= NULL)						// �������[�����
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
| <<< meta_buf_load �œǂݍ��񂾃|���S������Ԃ� >>>
|	�߂�l	���_��
*-------------------------------------------------------------------------------*/
int		meta_buf_poly_count_get(void)
{
	return meta->iPolyCount;
}



/*-------------------------------------------------------------------------------*
| <<< meta_buf_load �̃o�b�t�@���璸�_�E�@���EUV ����Ԃ� >>>
|	����	iCount = �o�b�t�@�ԍ�
|	�߂�l	���_�o�b�t�@�̃A�h���X
*-------------------------------------------------------------------------------*/
D3_VTX_DEFAULT	*meta_buf_vertex_get(int iCount)
{
	return &meta->pvtxBuf[iCount];
}

