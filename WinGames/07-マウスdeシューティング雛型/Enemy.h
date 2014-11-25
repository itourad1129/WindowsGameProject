//エネミークラス
class	cEnemy
{
public:
	BOOL	m_bUse;					// TRUE : 使用中
	BOOL	m_bDead;				// TRUE : 死亡
	int		m_iType;				// エネミーの種類
	VEC3	m_vecPos;				// エネミーの位置
	float	m_fSpeed;				// 速度
	float	m_fDist;				// 目的地までの距離
	float	m_fRot;					// 回転値
	float	m_fAlp;					// アルファ値

	void	Main(void);				// メイン
	void	Draw(void);				// 描画
};

//	変数
extern	cEnemy	*enemy[];

// 以下、一括処理の関数
extern	void	EnemyInit(void);
extern	void	EnemyMain(void);
extern	void	EnemyDraw(void);
