#define PLAYER_PUNCH_MAX		2					// プレイヤーの拳の数
// プレイヤークラス
class		cPlayer
{
public:
	float	m_fHP;									// 体力

	float	m_afPunchTime[PLAYER_PUNCH_MAX];		// パンチのアニメーション用左右ある。
	float	m_fGuardTime;							// ガード時間

	void	CalcEnemyDamage(int iID);				// エネミーへ与えるダメージ計算
	void	Guard(void);							// ガード
	void	Punch(void);							// パンチ

	void	Draw(void);								// 描画
	void	DamageSet(float fDamage);				// プレイヤーへダメージを与える
};

extern	cPlayer *player;							// プレイヤーワーク
extern	void	PlayerInit(void);
extern	void	PlayerMain(void);
extern	void	PlayerDraw(void);
