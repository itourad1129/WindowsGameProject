//--- プレイヤークラス -----------------------------------------------------------------
class	cPlayer
{
private:							// pivate（プライベートはこのクラス外から参照できない）
	VEC2	m_vecPos;				// 表示座標
	VEC2	m_vecSpeed;				// 移動速度
	int		m_iMoveTime;			// 移動回数、32ドットを 8 回に分けて進む
	int		m_iDir;					// 向き
	int		m_iAdr;					// 足元のマップアドレス
	int		m_iNimotu;				// 0 != 荷物を運んでいる
	float	m_fAnim;
	int		m_iAnimTime_a;
	int		m_iAnimTime_b;
	float	m_fRot;					// 回転値

public:							// public(パブリックはクラス外から参照可)
	void	SetDir(int iDir)			// 向きを設定
	{
		m_iDir = iDir;
	}

	int		GetDir(void)				// 向きを得る
	{
		return m_iDir;
	}
	
	void	SetPos(float fX, float fY)	// 座標を設定
	{
		m_vecPos = VEC2(fX, fY);		// このように処理することで、
	}									// クラスから private の変数に書き込める
	
	void	Reset(void);			// リセット
	void	Main(void);				// メイン
	void	Draw(void);				// 描画

	float	GetXPos(void)			// プレイヤーの X 座標を得る
	{
		return m_vecPos.x;
	}

	float	GetYPos(void)			// プレイヤーの Y 座標を得る
	{
		return m_vecPos.y;
	}

	BOOL	CheckStop(void)			// 停止しているときに true を返す
	{
		return (m_iMoveTime == 0);
	}

	
};

extern	cPlayer *player;