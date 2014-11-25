
#ifndef	__SKIN_
#define	__SKIN_


#ifdef	__OBBTREE
	#include	"ex/_obbtree.h"
#endif


#pragma	warning(disable:4786)
#include	<vector>
using	namespace	std;					// このスコープ内ではstd::が省略できる


/*----------------------------------------------------------------------*
| <<< フレームにワールド行列を追加 >>>
*----------------------------------------------------------------------*/
struct	D3DXFRAME_DERIVED:			public	D3DXFRAME
{
	MTX		CombinedTransformationMatrix;		// ワールド行列
};

//--- クラス、構造体の定義 ------------------------------------------------------------------------
// OBBTree 用
struct	SKINNING_WORK						// スキンデータ(OBBTreeで使用)
{
	int		m_iBoneID;
	float	m_fWeight;
};

// アニメーション用
struct	SKIN_ANIM_WORK
{
	LPD3DXANIMATIONCONTROLLER	m_pController;	// アニメーションコントローラーのコピー
	double						m_dTime;		// アニメーション時間
	int							m_iCurrent;		// カレントトラック番号
	int							m_iTrack;		// 再生アニメーショントラック
};


/*----------------------------------------------------------------------*
| <<< メッシュ個別のクラス >>>
*----------------------------------------------------------------------*/
struct	D3DXMESHCONTAINER_DERIVED:	public	D3DXMESHCONTAINER
{
#ifdef	__OBBTREE
	COBBTree				*pOBBTree;			// OBBTree 用
#endif
	short					*sTex;				// 通常使用する　テクスチャーの読み込み番号
	short					*sTexCopy;			// 通常使用する　テクスチャーの読み込み番号
	LPD3DXMESH				pMeshCopy;			// コピー用(バンプなどのフォーマットが違うものに使用する)
	short					sMeshCopyType;		// コピーのタイプ(バンプならば D3_SHADER_TYPE_BUMP が入る)
	D3_HIT					hitWork;			// ヒットワーク
	short					m_sShaderType;		// 表示する頂点シェーダーの型


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

	//--- 以下、スキンメッシュ用 ----------------------------------
	LPD3DXMESH				pOrigMesh;			// オリジナルのメッシュ(OBBTREE 時に必要になる)
	LPD3DXATTRIBUTERANGE	pAttributeTable;
	DWORD					NumAttributeGroups;	
	DWORD					NumInfl;			// ブレンド数
	LPD3DXBUFFER			pBoneCombinationBuf;
	MTX**					ppBoneMatrixPtrs;
	MTX*					m_pmtxBoneOffset;
	DWORD					m_dwPaletteEntries;	// 頂点パレット数
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

	MTX							*m_pmtxBone;			// ボーンの行列格納用
	UINT						m_uiBoneMatricesMax;	// ボーンの行列の数
	LPD3DXFRAME					m_pFrameRoot;			// ルートフレーム
	VEC3						m_vecRoot;				// 原点の座標(ヒット球などで使用)
	MTX							m_mtxRoot;				// 原点の行列
#ifdef	__OBBTREE
	bool						m_bOBBBox;				// true : OBBTree のボックスを描画
	int							m_iOBBBoxNum;			// 取得したい OBBTree のボックス番号(-1 で全て)
	int							m_iOBBBoxMax;			// OBBTree のボックス数(OBBTree)
#endif
	int							m_iXNum;				// Ｘファイル管理番号
	int							m_iShaderType;			// 表示する頂点シェーダーの型
	float						m_fAlp;					// アルファ値
	int							m_iID;					// 現在、処理中の anim 番号(_anim 管理番号)
	vector< SKIN_ANIM_WORK *>	m_anim;					// アニメーション用
	uchar						m_ucSubset;				// 総サブセット数
	uchar						m_ucHitCount;			// ヒットワーク用のカウンター
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




extern	CMyD3DApplication	*skin[];				// スキンメッシュのワーク
extern	MTX					mtxSkinPalette[];		// 行列パレット用
extern	int					iSkinAnimationSet;		// 総アニメーショントラック数
extern	int					iSkinFaceCount;			// 総面数
extern	int					iSkinVertexCount;		// 総頂点数
extern	int					iSkinBoneCount;			// オリジナルのボーンの数(50本を超えたら５０に丸め込まれる)

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
extern	void	skin_skin_proc_set( void *pProc);		// 割り込みセット
extern	void	skin_frame_proc_set(void *pProc);		// 割り込みセット

// ＩＤ切り替え
extern	void	skin_anime_controller_change(int iXNum, int iID);
extern	void	skin_id_change(int iXNum, int iID);

// モーション切り替え
extern	bool	skin_anim_change(int iXNum, int iTrackNum, float fFadeTime);
extern	bool	skin_anim_change(int iXNum, int iTrackNum);

// アニメーション関係
extern	void	skin_frame_set(	 int iNum, float dTime);
extern	void	skin_anim_move(int iNum, float dTime);
extern	void	skin_anim_reset(int iXNum);
extern	void	skin_frame_reset(int iNum, int iTrack);
extern	void	skin_frame_reset(int iNum);


extern	MTX		*skin_root_matrix_get(int iXNum);							// ルートの行列を得る
extern	int		skin_subset_get(int iXNum);									// サブセット数を得る
extern	void	skin_root_name_get(int iXNum, char *strOut, int iStrSize);	// ルートの名前を得る

																			// スキンメッシュ同士のヒットチェック
extern	bool	skin_hit_check(int iXANum, int iXBNum);
extern	bool	skin_hit_check(int *pAOut, int *pBOut, int a, int iANum, int b, int iBNum);
extern	VEC3	*skin_hit_pos_get(int iXNum, int iBoxNum);


extern	void	skin_obbtree_box_set(    int iXNum, bool bSw);	
extern	void	skin_obbtree_box_draw_num_set(int iXNum, int iBoxNum);	
extern	int		skin_obbtree_box_count_get(int iXNum);			// OBB のボックスの総数を得る



#endif		// __SKIN_

