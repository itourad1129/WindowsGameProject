#define SHOT_MAX		20			//弾の最大数

// 弾クラス
class
{
public:
	BOOL	m_bUse;					// TRUE : 使用中
	VEC3	m_vecPos;				// 座標
	float	m_fSpeed;				// 移動速度
	float	m_fDist;				// 移動した距離
	float	m_fDir;					// 向き

	void	Main(void);				// メイン
	void	Draw(void);				// 描画
};

// 変数
extern cShot	*shot[];			// 弾ワーク

// 以下、一括処理用の関数
extern	void	ShotInit(void);
extern	void	ShotMain(void);
extern	void	ShotDraw(void);
extern	void	ShotStart(VEC3 *pvecPos, float fDir);

