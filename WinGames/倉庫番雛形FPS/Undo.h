enum
{
	UNDO_RESET,						// アンドゥ初期化(リセット)
	UNDO_SET,						// アンドゥセット
	UNDO_GET,						// アンドゥのデータを取り出す(リドゥ)
};

extern	void	Undo(int	iSw);