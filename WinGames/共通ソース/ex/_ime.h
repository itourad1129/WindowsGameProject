

extern	int		ime_proc(HWND hWnd,	UINT message, WPARAM wParam, LPARAM	lParam);
extern	void	ime_cancel(char	*str, int str_len);
extern	bool	ime_input(VEC2 *pos, char *str, int len);
extern	void	ime_clear(char *str);
