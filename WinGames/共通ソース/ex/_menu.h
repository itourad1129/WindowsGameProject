


struct	MENU_WORK
{
	bool	bFirst;							// true : 初回チェック終了
	float	fX, fY;							// 表示座標
	int		iStrMax, iLine;					// 文字の最大数、ライン（項目）数
	char	str[1024];						// メニューの項目
	int		iAns;							// 選択項目の答え
};

extern	void	MenuDraw(MENU_WORK *w);
extern	int		MenuMain(MENU_WORK *w);
extern	void	MenuInit(void);
extern	void	MenuFree(void);
extern	void	MenuFrameDraw(VEC3 *p0, VEC3 *p1);
extern	void	MenuInit(void);
extern	void	MenuMake(char *str);

