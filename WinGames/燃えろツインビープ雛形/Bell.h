#define	BELL_MAX			30					// 隕石の数（数が多いほど難易度があがる）

// 隕石クラス
class	cBell
{
public:
	VEC3	m_vecPos;			// 位置
	VEC3	m_vecSpeed;			// 移動速度
	int		m_iDelay;			// 遅延時間
	MTX		m_mtxWorld;			// ワールド
	float	m_fRot;				// 回転値
	float	m_fAlp;				// 描画

	void	Init(int	iNum);	// 初期化
	void	Main(void);			// メイン
	void	Draw(void);			// 描画
};

extern	cBell	*bell;

extern	void	BellInit(void);
extern	void	BellMain(void);
extern	void	BellDraw(void);
