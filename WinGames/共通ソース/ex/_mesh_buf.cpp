//
//	���b�V���o�^�֌W
//

#define	MESH_PRI_MAX		1			// �o�^���b�V���̎�ސ�
#define	MESH_BUF_MAX		1000		// �o�b�t�@�̓o�^��


//--- �N���X --------------------------------------------------------------------------------------
class	cMesh							// ���b�V���ʂ̃o�b�t�@
{
public:
	int		m_iXNum;						// �w�t�@�C���ԍ�
	MTX		m_mtxWorld;						// ���[���h�s��
	float	m_fAlp;							// ���������[�g
	//float	m_fRadius;						// SPHERE �̔��a
};

struct	cMeshBuf						// �}�U�[�o�b�t�@
{
public:
	uchar		m_ucType;					// 0 : �ʏ탁�b�V��
											// 1 : SPHERE
	uchar		m_ucShaderType;				// �V�F�[�_�[�^�C�v
	short		m_sCount;					// �o�^��
	cMesh		m_pWork[MESH_BUF_MAX];		// �o�^�o�b�t�@

	// �R���X�g���N�^
	cMeshBuf()
	{
		Reset();
		memset(m_pWork, 0, sizeof(cMesh) * MESH_BUF_MAX);
	}

	// �f�X�g���N�^
	~cMeshBuf()
	{
	}

	// ���Z�b�g
	void	Reset(void)
	{
		m_ucType	   = 0;
		m_ucShaderType = D3_SHADER_TYPE_NORMAL;
		m_sCount	   = 0;
	}
};


//--- �ϐ���` ------------------------------------------------------------------------------------
static	cMeshBuf	*meshBuf;				// ���b�V���̓o�^���[�N
static	int			iPriMax;



/*----------------------------------------------------------------------*
| <<< ���b�V���o�b�t�@������������ >>>
|	����	iMax = �v���C�I���e�B��(�V�F�[�_�[�𕡐���g���Ƃ��ȂǂɎg�p)
*----------------------------------------------------------------------*/
void	mesh_buf_init(int iMax)
{

	// �������m��
	if(meshBuf == NULL)
	{
		meshBuf = new cMeshBuf[iMax];
		iPriMax = iMax;
	}

	// ���Z�b�g
	for(int i = 0; i < iPriMax; i++)
	{
		meshBuf[i].Reset();
	}
}

void	mesh_buf_init(void)
{
	mesh_buf_init(MESH_PRI_MAX);
}



/*----------------------------------------------------------------------*
| <<< ���b�V���o�b�t�@���N���A�[���� >>>
*----------------------------------------------------------------------*/
void	mesh_buf_clear(void)
{

	for(int i = 0; i < iPriMax; i++){ meshBuf[i].m_sCount = 0;}
}



/*----------------------------------------------------------------------*
| <<< ���b�V����o�^���� >>>
|	����	iPri     = �v���C�I���e�B�ԍ�
|			iXNum	 = ���f���ԍ�
|			mtxWorld = �z�u���W
|			fAlp	 = �A���t�@�l(0.0f �` 1.0f)
*----------------------------------------------------------------------*/
void	mesh_buf_set(int iPri, int iXNum, MTX *mtxWorld, float fAlp)
{

	cMeshBuf	*m = &meshBuf[   iPri];
	cMesh		*w = &m->m_pWork[m->m_sCount];

	w->m_iXNum	   = iXNum;					// �e����o�^
	w->m_mtxWorld  = *mtxWorld;
	w->m_fAlp	   = fAlp;

	m->m_sCount++;							// �����œo�^���̃J�E���g�A�b�v
}

void	mesh_buf_set(int iXNum, MTX *mtxWorld, float fAlp)
{
	mesh_buf_set(0, iXNum, mtxWorld, fAlp);
}



/*----------------------------------------------------------------------*
| <<< �N�C�b�N�\�[�g���s�� >>>
*----------------------------------------------------------------------*/
static	int		iOrder[MESH_BUF_MAX];		// �\�[�g�p
static	float	fOrder[MESH_BUF_MAX];

static	void	QSort(int *piTbl, float *pfTbl, int iLeft, int iRight)
{

	int		i	  = iLeft;						// �\�[�g����z��̈�ԏ������v�f�̓Y��
	int		j	  = iRight;						// �\�[�g����z��̈�ԑ傫���v�f�̓Y��
	float	pivot = pfTbl[(iLeft + iRight) / 2];// ��l��z��̒����t�߂ɂƂ�
	while(1)
	{
		while(pfTbl[i] < pivot   ){ i++;}		// pivot ���傫���l���o��܂� i �𑝉�������
		while(pivot    < pfTbl[j]){ j--;}       // pivot ��菬�����l���o��܂� j ������������
		if(i >= j){ break;}						// i >= j �Ȃ疳�����[�v���甲����
		SWAP( piTbl[i], piTbl[j]);
		SWAPF(pfTbl[i], pfTbl[j]);				// pfTbl[i] �� pfTbl[j]������
		j--;									// ���̃f�[�^
	}
												// ��l�̍��� 2 �ȏ�v�f������΁A���̔z��� Q �\�[�g����
	if(iLeft < i - 1 ){ QSort(piTbl, pfTbl, iLeft, i - 1 );}
												// ��l�̉E�� 2 �ȏ�v�f������΁A�E�̔z��� Q �\�[�g����
	if(j + 1 < iRight){ QSort(piTbl, pfTbl, j + 1, iRight);}
}



/*------------------------------------------------------------------------------*
| <<< �\�[�g(���ёւ�) >>>
|	�߂�l		�����̐�
*------------------------------------------------------------------------------*/
static	int		Sort(void)
{

	//--- ���[�N���l�߂Ȃ��琔�𐔂��� ------------------------------
	cMeshBuf	*m = &meshBuf[   0];
	cMesh		*w = &m->m_pWork[0];
	for(int i = 0; i < m->m_sCount; i++, w++)
	{
		VEC3	v(w->m_mtxWorld._41, w->m_mtxWorld._42, w->m_mtxWorld._43);
		D3DXVec3TransformCoord(&v, &v, &d3.mtxView);
		iOrder[i] = i;
		fOrder[i] = v.z;
	}

	//--- ���ёւ��Ă݂� --------------------------------------------
	if(m->m_sCount >= 2)
	{
		QSort(iOrder, fOrder, 0, m->m_sCount - 1);
	}

	return m->m_sCount;
}



/*----------------------------------------------------------------------*
| <<< �o�b�t�@�̃��b�V�����\�[�g���� >>>
*----------------------------------------------------------------------*/
void	mesh_sort(void)
{

	Sort();
}



/*----------------------------------------------------------------------*
| <<< �o�b�t�@�̃��b�V����`�悷�� >>>
|	����	iStart, iEnd = �N���A�[����o�b�t�@�ԍ�
*----------------------------------------------------------------------*/
void	mesh_buf_flush(void)
{

	for(int	i = 0; i < iPriMax; i++)
	{
		cMeshBuf	*m = &meshBuf[   i];

		// �V�F�[�_�[�Z�b�g
		d3_shader_set(m->m_ucShaderType);
		for(int j = m->m_sCount - 1; j != -1; j--)
		{
			cMesh	*w = &m->m_pWork[iOrder[j]];
			d3_3d_draw(w->m_iXNum, m->m_ucShaderType, &w->m_mtxWorld, w->m_fAlp);
		}
	}
}

