


struct	MENU_WORK
{
	bool	bFirst;							// true : ����`�F�b�N�I��
	float	fX, fY;							// �\�����W
	int		iStrMax, iLine;					// �����̍ő吔�A���C���i���ځj��
	char	str[1024];						// ���j���[�̍���
	int		iAns;							// �I�����ڂ̓���
};

extern	void	MenuDraw(MENU_WORK *w);
extern	int		MenuMain(MENU_WORK *w);
extern	void	MenuInit(void);
extern	void	MenuFree(void);
extern	void	MenuFrameDraw(VEC3 *p0, VEC3 *p1);
extern	void	MenuInit(void);
extern	void	MenuMake(char *str);

