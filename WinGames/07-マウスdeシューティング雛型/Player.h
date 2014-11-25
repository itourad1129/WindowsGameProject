
// プレイヤークラス
class	cPlayer
{
public:
	VEC3	m_vecPos;			// プレイヤー座標
	
	float	m_fDir;				// 向き

	void	Main(void);			// メイン
	
	void	Draw(void);			// 描画処理
};

// 変数
extern	cPlayer *player;		// プレイヤー

//---   外部からの呼び出し関数 -----------------------------------------------------
extern	void	PlayerInit(void);
extern	void	PlayerMain(void);
extern	void	PlayerDraw(void);
