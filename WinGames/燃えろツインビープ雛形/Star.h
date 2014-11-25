
#define STAR_MAX		100			// 星の数

// 星クラス
class	cStar
{
public:
	VEC3	m_vecPos;				// 位置
	VEC3	m_vecSpeed;				// 移動速度
	int		m_iDelay;				// 遅延時間
	MTX		m_mtxWorld;				// ワールド
	float	m_fAlp;					// 半透明レート

	void	Init(void);				// 初期化
	void	Main(void);				// メイン
	void	Draw(void);				// 描画
};

extern	cStar	*star;

extern	void	StarInit(void);
extern	void	StarMain(void);
extern	void	StarDraw(void);