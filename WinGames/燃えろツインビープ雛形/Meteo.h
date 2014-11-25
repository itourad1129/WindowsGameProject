#define	METEO_MAX			100					// 隕石の数（数が多いほど難易度があがる）

// 隕石クラス
class	cMeteo
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

extern	cMeteo	*meteo;

extern	void	MeteoInit(void);
extern	void	MeteoMain(void);
extern	void	MeteoDraw(void);

