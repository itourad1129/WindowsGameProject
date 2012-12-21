

struct	GAME_WORK						// ゲームの進行用ワーク
{
	int		iMode;						// ゲームモード
	int		iScore;						// スコア
	int		iHiScore;					// ハイスコア
};



//--- 以下、各自で定義する用 ------------------------------------------------
extern	void	game_init(void);
extern	void	game_main(void);
extern	void	game_end(void);
extern	void	game_render(void);

extern	GAME_WORK	game;

