

#ifndef	__CTRL_DEBUG_DIALOG
#define	__CTRL_DEBUG_DIALOG


extern	void	ctrl_debug_dialog_open(void);
extern	void	ctrl_debug_dialog_close(void);

extern	void	ctrl_debug_str_monitor(char *strVar, char *str);

extern	void	ctrl_debug_monitor(char *str, char   *d);
extern	void	ctrl_debug_monitor(char *str, uchar  *d);
extern	void	ctrl_debug_monitor(char *str, short  *d);
extern	void	ctrl_debug_monitor(char *str, ushort *d);
extern	void	ctrl_debug_monitor(char *str, int    *d);
extern	void	ctrl_debug_monitor(char *str, uint   *d);

extern	void	ctrl_debug_monitor(char *strVar, float  *fVar);
extern	void	ctrl_debug_monitor(char *strVar, double *dVar);


extern	void	ctrl_debug_monitor(char *str, char   *d, bool bHex);
extern	void	ctrl_debug_monitor(char *str, uchar  *d, bool bHex);
extern	void	ctrl_debug_monitor(char *str, short  *d, bool bHex);
extern	void	ctrl_debug_monitor(char *str, ushort *d, bool bHex);
extern	void	ctrl_debug_monitor(char *str, int    *d, bool bHex);
extern	void	ctrl_debug_monitor(char *str, uint   *d, bool bHex);

extern	void	ctrl_debug_monitor_printf(char* Format,...);



#endif


