
extern	void	script_init(int iTex);				// �X�N���v�g������
extern	void	script_free(void);					// �X�N���v�g�J��
extern	bool	script_main(void);					// �X�N���v�g���C������
extern	void	script_draw(int iTex);				// �X�N���v�g�`��
extern	bool	script_load(char *cFile);			// �X�N���v�g�ǂݍ���

extern	void	script_printf(char *Format,...);	// �X�N���v�g�ŕ����\��
extern	void	script_end(int iTex);				// �X�N���v�g�I��


/*
extern	int		TextVarNumGet(char *strName);
extern	int		TextVarNameSet(char *strName);
extern	bool	TextVarGet(int iID, float *fOut);
extern	bool	TextVarSet(int iID, float fIn);
extern	void	TextCountClear(void);
*/