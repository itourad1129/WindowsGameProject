
#ifndef	__SKIN_
#define	__SKIN_


#ifdef	__OBBTREE
	#include	"ex/_obbtree.h"
#endif


#pragma	warning(disable:4786)
#include	<vector>
using	namespace	std;					// ���̃X�R�[�v���ł�std::���ȗ��ł���


/*----------------------------------------------------------------------*
| <<< �t���[���Ƀ��[���h�s���ǉ� >>>
*----------------------------------------------------------------------*/
struct	D3DXFRAME_DERIVED:			public	D3DXFRAME
{
	MTX		CombinedTransformationMatrix;		// ���[���h�s��
};

//--- �N���X�A�\���̂̒�` ------------------------------------------------------------------------
// OBBTree �p
struct	SKINNING_WORK						// �X�L���f�[�^(OBBTree�Ŏg�p)
{
	int		m_iBoneID;
	float	m_fWeight;
};

// �A�j���[�V�����p
struct	SKIN_ANIM_WORK
{
	LPD3DXANIMATIONCONTROLLER	m_pController;	// �A�j���[�V�����R���g���[���[�̃R�s�[
	double						m_dTime;		// �A�j���[�V��������
	int							m_iCurrent;		// �J�����g�g���b�N�ԍ�
	int							m_iTrack;		// �Đ��A�j���[�V�����g���b�N
};


/*----------------------------------------------------------------------*
| <<< ���b�V���ʂ̃N���X >>>
*----------------------------------------------------------------------*/
struct	D3DXMESHCONTAINER_DERIVED:	public	D3DXMESHCONTAINER
{
#ifdef	__OBBTREE
	COBBTree				*pOBBTree;			// OBBTree �p
#endif
	short					*sTex;				// �ʏ�g�p����@�e�N�X�`���[�̓ǂݍ��ݔԍ�
	short					*sTexCopy;			// �ʏ�g�p����@�e�N�X�`���[�̓ǂݍ��ݔԍ�
	LPD3DXMESH				pMeshCopy;			// �R�s�[�p(�o���v�Ȃǂ̃t�H�[�}�b�g���Ⴄ���̂Ɏg�p����)
	short					sMeshCopyType;		// �R�s�[�̃^�C�v(�o���v�Ȃ�� D3_SHADER_TYPE_BUMP ������)
	D3_HIT					hitWork;			// �q�b�g���[�N
	short					m_sShaderType;		// �\�����钸�_�V�F�[�_�[�̌^


	D3DXMESHCONTAINER_DERIVED::D3DXMESHCONTAINER_DERIVED()
	{
		ZeroMemory(&MeshData, sizeof(D3DXMESHDATA));
		Name				= NULL;
		pMaterials			= NULL;
		pEffects			= NULL;
		NumMaterials		= 0;
		pAdjacency			= NULL;
		pSkinInfo			= NULL;

		m_pmtxBoneOffset	= NULL;
		ppBoneMatrixPtrs    = NULL;
		pBoneCombinationBuf = NULL;
		pOrigMesh           = NULL;
		pMeshCopy           = NULL;

#ifdef	__OBBTREE
		pOBBTree			= NULL;
#endif
		sTex				= NULL;
	}

	//--- �ȉ��A�X�L�����b�V���p ----------------------------------
	LPD3DXMESH				pOrigMesh;			// �I���W�i���̃��b�V��(OBBTREE ���ɕK�v�ɂȂ�)
	LPD3DXATTRIBUTERANGE	pAttributeTable;
	DWORD					NumAttributeGroups;	
	DWORD					NumInfl;			// �u�����h��
	LPD3DXBUFFER			pBoneCombinationBuf;
	MTX**					ppBoneMatrixPtrs;
	MTX*					m_pmtxBoneOffset;
	DWORD					m_dwPaletteEntries;	// ���_�p���b�g��
};

class	CD3DApplication{};
class	CMyD3DApplication:	public	CD3DApplication
{
protected:
	void		DrawMeshContainer(   LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase);
	void		OBBDrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase);
	void		OBBDrawFrame(LPD3DXFRAME pFrame);
	HRESULT		SetupBoneMatrixPointersOnMesh(LPD3DXMESHCONTAINER pMeshContainer);
	HRESULT		SetupBoneMatrixPointers(      LPD3DXFRAME		  pFrame);
	void		DrawFrame(            LPD3DXFRAME pFrame);
public:
	void		DrawFrame(            LPD3DXFRAME pFrame, int iDrawParts, int iParts);
	void		UpdateFrameMatrices(  LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix);
	void		SubsetNameGet(        LPD3DXFRAME pFrame, char *strOutBuf, int iStrSize, int iCount);
	HRESULT		AddMeshtoOneSkinModel(D3DXFRAME *pD3DXFrame);
#ifdef	__OBBTREE
	void		GetSkinMesh(LPD3DXFRAME pD3DXFrame, LPD3DXSKININFO *pSkinInfo, LPD3DXMESHCONTAINER *pD3DXMeshContainer);
	void		GetFrame(D3DXFRAME *pD3DXFrame, std::vector< D3DXFRAME * > &FrameVector );
	void		OBBTreeClear(LPD3DXFRAME pD3DXFrame);
#endif
	HRESULT		SkinLoad(char *file);
	HRESULT		DeleteDeviceObjects(void);
	HRESULT		FrameMove(float	fDat);
	//void		FrameMatricesCountGet(LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix);
	void		HitWorkGet(int iXNum, LPD3DXFRAME pFrame);
	HRESULT		Render(	  MTX *world);
	HRESULT		OBBRender(MTX *world);
	HRESULT		GenerateSkinnedMesh(D3DXMESHCONTAINER_DERIVED *pMeshContainer);
	void		MaterialCountGet(LPD3DXFRAME lpFrame, uchar *ucCount);

	MTX							*m_pmtxBone;			// �{�[���̍s��i�[�p
	UINT						m_uiBoneMatricesMax;	// �{�[���̍s��̐�
	LPD3DXFRAME					m_pFrameRoot;			// ���[�g�t���[��
	VEC3						m_vecRoot;				// ���_�̍��W(�q�b�g���ȂǂŎg�p)
	MTX							m_mtxRoot;				// ���_�̍s��
#ifdef	__OBBTREE
	bool						m_bOBBBox;				// true : OBBTree �̃{�b�N�X��`��
	int							m_iOBBBoxNum;			// �擾������ OBBTree �̃{�b�N�X�ԍ�(-1 �őS��)
	int							m_iOBBBoxMax;			// OBBTree �̃{�b�N�X��(OBBTree)
#endif
	int							m_iXNum;				// �w�t�@�C���Ǘ��ԍ�
	int							m_iShaderType;			// �\�����钸�_�V�F�[�_�[�̌^
	float						m_fAlp;					// �A���t�@�l
	int							m_iID;					// ���݁A�������� anim �ԍ�(_anim �Ǘ��ԍ�)
	vector< SKIN_ANIM_WORK *>	m_anim;					// �A�j���[�V�����p
	uchar						m_ucSubset;				// ���T�u�Z�b�g��
	uchar						m_ucHitCount;			// �q�b�g���[�N�p�̃J�E���^�[
};


class	CAllocateHierarchy:	public ID3DXAllocateHierarchy
{
public:
	
	STDMETHOD(CreateFrame)(THIS_ LPCSTR	Name, LPD3DXFRAME *ppNewFrame);
	STDMETHOD(CreateMeshContainer)(THIS_ 
		LPCSTR Name, 
		CONST D3DXMESHDATA *pMeshData,
		CONST D3DXMATERIAL *pMaterials,	
		CONST D3DXEFFECTINSTANCE *pEffectInstances,	
		DWORD NumMaterials,	
		CONST DWORD	*pAdjacency, 
		LPD3DXSKININFO pSkinInfo, 
		LPD3DXMESHCONTAINER	*ppNewMeshContainer);
	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);
	CAllocateHierarchy(CMyD3DApplication *pApp)	:m_pApp(pApp) {}
public:
	CMyD3DApplication	*m_pApp;
};




extern	CMyD3DApplication	*skin[];				// �X�L�����b�V���̃��[�N
extern	MTX					mtxSkinPalette[];		// �s��p���b�g�p
extern	int					iSkinAnimationSet;		// ���A�j���[�V�����g���b�N��
extern	int					iSkinFaceCount;			// ���ʐ�
extern	int					iSkinVertexCount;		// �����_��
extern	int					iSkinBoneCount;			// �I���W�i���̃{�[���̐�(50�{�𒴂�����T�O�Ɋۂߍ��܂��)

extern	bool	skin_load(int iNum,	char *strFile);
extern	bool	skin_load(int iNum, char *strFile, bool bLoadAtOnce, bool bBumpLoad, bool bFurLoad);

extern	bool	skin_draw(int iNum,	int	iVsType, MTX *mtxWorld, float fAlp);
extern	bool	skin_draw(int iNum,	int	iVsType, MTX *mtxWorld);
extern	bool	skin_draw(int iNum,				 MTX *mtxWorld, float fAlp);
extern	bool	skin_draw(int iNum,				 MTX *mtxWorld);
extern	bool	skin_draw_parts(int iNum, int iMatNum, int iShaderType, MTX *mtxWorld, float fAlp);

extern	int		skin_anim_count_get(int iNum);			// 
extern	bool	skin_anim_set(int iNum,	int	iTrackNum, float fFadeTime);
extern	bool	skin_anim_set(int iNum,	int	iTrackNum, float fFadeTime,	bool bReset);
extern	void	skin_end(void);
extern	void	skin_free(int iNum);

extern	bool	skin_load_check(int iNum);
extern	void	skin_skin_proc_set( void *pProc);		// ���荞�݃Z�b�g
extern	void	skin_frame_proc_set(void *pProc);		// ���荞�݃Z�b�g

// �h�c�؂�ւ�
extern	void	skin_anime_controller_change(int iXNum, int iID);
extern	void	skin_id_change(int iXNum, int iID);

// ���[�V�����؂�ւ�
extern	bool	skin_anim_change(int iXNum, int iTrackNum, float fFadeTime);
extern	bool	skin_anim_change(int iXNum, int iTrackNum);

// �A�j���[�V�����֌W
extern	void	skin_frame_set(	 int iNum, float dTime);
extern	void	skin_anim_move(int iNum, float dTime);
extern	void	skin_anim_reset(int iXNum);
extern	void	skin_frame_reset(int iNum, int iTrack);
extern	void	skin_frame_reset(int iNum);


extern	MTX		*skin_root_matrix_get(int iXNum);							// ���[�g�̍s��𓾂�
extern	int		skin_subset_get(int iXNum);									// �T�u�Z�b�g���𓾂�
extern	void	skin_root_name_get(int iXNum, char *strOut, int iStrSize);	// ���[�g�̖��O�𓾂�

																			// �X�L�����b�V�����m�̃q�b�g�`�F�b�N
extern	bool	skin_hit_check(int iXANum, int iXBNum);
extern	bool	skin_hit_check(int *pAOut, int *pBOut, int a, int iANum, int b, int iBNum);
extern	VEC3	*skin_hit_pos_get(int iXNum, int iBoxNum);


extern	void	skin_obbtree_box_set(    int iXNum, bool bSw);	
extern	void	skin_obbtree_box_draw_num_set(int iXNum, int iBoxNum);	
extern	int		skin_obbtree_box_count_get(int iXNum);			// OBB �̃{�b�N�X�̑����𓾂�



#endif		// __SKIN_

