
#define	AFTERIMAGE_MAX		200				// 残像登録数

// 残像クラス
class	cAfterimage
{
public:
	int		m_iMeshNum;		// ３Dモデル番号
	int		m_iTime;		// 表示時間
	MTX		m_mtxWorld;		// ワールド行列
	MTX		m_mtxUpDate;	// 更新行列

	void	Init(void);		// 初期化
	void	Main(void);		// メイン
	void	Draw(void);		// 描画
};

extern void		AfterimageInit(void);
extern void		AfterimageMain(void);
extern void		AfterimageDraw(void);
extern void		AfterimageSet(int	iMeshNum,	MTX *world,	MTX *update);
