
extern	void	script_init(int iTex);				// スクリプト初期化
extern	void	script_free(void);					// スクリプト開放
extern	bool	script_main(void);					// スクリプトメイン処理
extern	void	script_draw(int iTex);				// スクリプト描画
extern	bool	script_load(char *cFile);			// スクリプト読み込み

extern	void	script_printf(char *Format,...);	// スクリプトで文字表示
extern	void	script_end(int iTex);				// スクリプト終了


/*
extern	int		TextVarNumGet(char *strName);
extern	int		TextVarNameSet(char *strName);
extern	bool	TextVarGet(int iID, float *fOut);
extern	bool	TextVarSet(int iID, float fIn);
extern	void	TextCountClear(void);
*/