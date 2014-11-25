// プレイヤークラス
class	cPlayer
{
public:
	VEC3	m_vecPos;					// 位置
	VEC3	m_vecSpeed;					// 移動速度
	MTX		m_mtxWorld;					// ワールド行列
	VEC3	m_vecBak;					// 前回座標
	int		m_iDamageTime;				// ダメージを受けている時間
	void	Init(void);					// 初期化
	void	Main(void);					// メイン
	void	Draw(void);					// 描画
	void	Damage(void);				// ダメージ時、カメラグラグラ
	int		m_iHp;						//プレイヤーHP
	int		m_iScore;					//スコア
};

extern	cPlayer	*player;

extern	void	PlayerInit(void);
extern	void	PlayerMain(void);
extern	void	PlayerDraw(void);

