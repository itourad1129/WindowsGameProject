// エネミークラス
class	cEnemy
{
public:
	float	m_fAttackTime;				// 攻撃時間
	int		m_iAnimCount;				// アニメーションカウンター
	int		m_iRenda;					// パンチの連打数
	int		m_iMode;					// モード
	float	m_fHP;						// 体力
	float	m_fLevel;					// 難易度

	void	Reset(BOOL bMotBlend);		// リセット
	void	Wait(void);					// 待ち
	void	Attack(void);				// 攻撃
	void	Damage(void);				// ダメージ
	void	KnockOut(void);				// ノックアウト
	void	Groggy(void);				// グロッキー
	void	Main(void);					// メイン
	void	Draw(void);					// 描画
	void	DamageSet(float fDamage);	// ダメージを与える
	void	GroggyStart(void);			// グロッキー開始
};

enum	ENE_MODE				// エネミーの攻撃モード エルフレイナのアニメーション順がおかしいため、入れ替えます。
{
	ENE_DAMAGE,					// ダメージ
	ENE_KNOCKOUT,				// ノックアウト
	ENE_GROGGY,					// グロッキー
	ENE_LEFT_PUNCH,				// 左パンチ
	ENE_RIGHT_PUNCH,			// 右パンチ
	ENE_WAIT,					// 待ち
};

extern cEnemy	*enemy;			// エネミーワーク
extern	void	EnemyInit(void);
extern	void	EnemyMain(void);
extern	void	EnemyDraw(void);