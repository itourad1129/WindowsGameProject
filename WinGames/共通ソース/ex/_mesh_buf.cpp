//
//	メッシュ登録関係
//

#define	MESH_PRI_MAX		1			// 登録メッシュの種類数
#define	MESH_BUF_MAX		1000		// バッファの登録数


//--- クラス --------------------------------------------------------------------------------------
class	cMesh							// メッシュ個別のバッファ
{
public:
	int		m_iXNum;						// Ｘファイル番号
	MTX		m_mtxWorld;						// ワールド行列
	float	m_fAlp;							// 半透明レート
	//float	m_fRadius;						// SPHERE の半径
};

struct	cMeshBuf						// マザーバッファ
{
public:
	uchar		m_ucType;					// 0 : 通常メッシュ
											// 1 : SPHERE
	uchar		m_ucShaderType;				// シェーダータイプ
	short		m_sCount;					// 登録数
	cMesh		m_pWork[MESH_BUF_MAX];		// 登録バッファ

	// コンストラクタ
	cMeshBuf()
	{
		Reset();
		memset(m_pWork, 0, sizeof(cMesh) * MESH_BUF_MAX);
	}

	// デストラクタ
	~cMeshBuf()
	{
	}

	// リセット
	void	Reset(void)
	{
		m_ucType	   = 0;
		m_ucShaderType = D3_SHADER_TYPE_NORMAL;
		m_sCount	   = 0;
	}
};


//--- 変数定義 ------------------------------------------------------------------------------------
static	cMeshBuf	*meshBuf;				// メッシュの登録ワーク
static	int			iPriMax;



/*----------------------------------------------------------------------*
| <<< メッシュバッファを初期化する >>>
|	入力	iMax = プライオリティ数(シェーダーを複数種使うときなどに使用)
*----------------------------------------------------------------------*/
void	mesh_buf_init(int iMax)
{

	// メモリ確保
	if(meshBuf == NULL)
	{
		meshBuf = new cMeshBuf[iMax];
		iPriMax = iMax;
	}

	// リセット
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
| <<< メッシュバッファをクリアーする >>>
*----------------------------------------------------------------------*/
void	mesh_buf_clear(void)
{

	for(int i = 0; i < iPriMax; i++){ meshBuf[i].m_sCount = 0;}
}



/*----------------------------------------------------------------------*
| <<< メッシュを登録する >>>
|	入力	iPri     = プライオリティ番号
|			iXNum	 = モデル番号
|			mtxWorld = 配置座標
|			fAlp	 = アルファ値(0.0f ～ 1.0f)
*----------------------------------------------------------------------*/
void	mesh_buf_set(int iPri, int iXNum, MTX *mtxWorld, float fAlp)
{

	cMeshBuf	*m = &meshBuf[   iPri];
	cMesh		*w = &m->m_pWork[m->m_sCount];

	w->m_iXNum	   = iXNum;					// 各情報を登録
	w->m_mtxWorld  = *mtxWorld;
	w->m_fAlp	   = fAlp;

	m->m_sCount++;							// ここで登録情報のカウントアップ
}

void	mesh_buf_set(int iXNum, MTX *mtxWorld, float fAlp)
{
	mesh_buf_set(0, iXNum, mtxWorld, fAlp);
}



/*----------------------------------------------------------------------*
| <<< クイックソートを行う >>>
*----------------------------------------------------------------------*/
static	int		iOrder[MESH_BUF_MAX];		// ソート用
static	float	fOrder[MESH_BUF_MAX];

static	void	QSort(int *piTbl, float *pfTbl, int iLeft, int iRight)
{

	int		i	  = iLeft;						// ソートする配列の一番小さい要素の添字
	int		j	  = iRight;						// ソートする配列の一番大きい要素の添字
	float	pivot = pfTbl[(iLeft + iRight) / 2];// 基準値を配列の中央付近にとる
	while(1)
	{
		while(pfTbl[i] < pivot   ){ i++;}		// pivot より大きい値が出るまで i を増加させる
		while(pivot    < pfTbl[j]){ j--;}       // pivot より小さい値が出るまで j を減少させる
		if(i >= j){ break;}						// i >= j なら無限ループから抜ける
		SWAP( piTbl[i], piTbl[j]);
		SWAPF(pfTbl[i], pfTbl[j]);				// pfTbl[i] と pfTbl[j]を交換
		j--;									// 次のデータ
	}
												// 基準値の左に 2 以上要素があれば、左の配列を Q ソートする
	if(iLeft < i - 1 ){ QSort(piTbl, pfTbl, iLeft, i - 1 );}
												// 基準値の右に 2 以上要素があれば、右の配列を Q ソートする
	if(j + 1 < iRight){ QSort(piTbl, pfTbl, j + 1, iRight);}
}



/*------------------------------------------------------------------------------*
| <<< ソート(並び替え) >>>
|	戻り値		爆発の数
*------------------------------------------------------------------------------*/
static	int		Sort(void)
{

	//--- ワークを詰めながら数を数える ------------------------------
	cMeshBuf	*m = &meshBuf[   0];
	cMesh		*w = &m->m_pWork[0];
	for(int i = 0; i < m->m_sCount; i++, w++)
	{
		VEC3	v(w->m_mtxWorld._41, w->m_mtxWorld._42, w->m_mtxWorld._43);
		D3DXVec3TransformCoord(&v, &v, &d3.mtxView);
		iOrder[i] = i;
		fOrder[i] = v.z;
	}

	//--- 並び替えてみる --------------------------------------------
	if(m->m_sCount >= 2)
	{
		QSort(iOrder, fOrder, 0, m->m_sCount - 1);
	}

	return m->m_sCount;
}



/*----------------------------------------------------------------------*
| <<< バッファのメッシュをソートする >>>
*----------------------------------------------------------------------*/
void	mesh_sort(void)
{

	Sort();
}



/*----------------------------------------------------------------------*
| <<< バッファのメッシュを描画する >>>
|	入力	iStart, iEnd = クリアーするバッファ番号
*----------------------------------------------------------------------*/
void	mesh_buf_flush(void)
{

	for(int	i = 0; i < iPriMax; i++)
	{
		cMeshBuf	*m = &meshBuf[   i];

		// シェーダーセット
		d3_shader_set(m->m_ucShaderType);
		for(int j = m->m_sCount - 1; j != -1; j--)
		{
			cMesh	*w = &m->m_pWork[iOrder[j]];
			d3_3d_draw(w->m_iXNum, m->m_ucShaderType, &w->m_mtxWorld, w->m_fAlp);
		}
	}
}

