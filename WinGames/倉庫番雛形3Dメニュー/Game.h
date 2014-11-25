

struct GAME_WORK							// ゲームの進行用ワーク
{

};


//--- 以下、各自で定義する用 ------------------------------------------------
extern	void	game_init(void);
extern	void	game_main(void);
extern	void	game_end(void);
extern	void	game_render(void);
extern	void	MenuMain(VEC2 *p, float fHInterval);
extern	void	MenuInit(void);
extern	void	MenuAdd(char *str);
extern	void	MItemSet(int  iNum);
extern	bool	TitleMenu(int iTex, int iSound, bool bSw);

extern	GAME_WORK	game;

extern	int		iRound;
extern	int		iMenuSel;
extern	bool	bMenuOk;
extern	bool	bMenuCancel;
extern	bool	bMenuCancelEnable;



