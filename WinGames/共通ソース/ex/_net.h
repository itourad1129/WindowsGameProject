#ifndef	__NET_H
#define	__NET_H


//--- 定数定義 ------------------------------------------------------------------------------------
#define WM_WINSOCKMESSAGE (WM_USER + 300)	// winsock のイベント

//--- 構造体 --------------------------------------------------------------------------------------
typedef struct tugIPDATA					// WinSockets使用時のアドレス指定用構造体
{
	uchar	d1, d2, d3, d4;						// アドレス値
}IPDATA, *LPIPDATA;



extern	BOOL	net_init(void);
extern	BOOL	net_end(void);
extern	BOOL	net_proc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
extern	void	net_connect_start(IPDATA *ipDat);
extern	int		net_connect_wait(int *iNum);
extern	BOOL	net_connect_close(int iNum);

extern	BOOL	net_listen_preparation(void);
extern	BOOL	net_listen_stop(void);
extern	int		net_new_accept_get(void);
extern	BOOL	net_ip_get(IPDATA *ipOut, int iNum);
extern	int		net_lost_get(void);

extern	BOOL	net_main(void);


extern	BOOL	net_send(void *pBuf, uint uiSize);
extern	BOOL	net_recv(void *pBuf, uint uiSize);

extern	BOOL	net_accept_check(void);
extern	BOOL	net_connect_check(void);

extern	void	net_server_set(BOOL bServer);	// サーバーとクライアントの切り替えスイッチ
extern	BOOL	net_server_check(void);			// サーバーかクライアントかを調べる
extern	void	net_server_connect_preparation(void);

extern	IPDATA	*net_ip(void);

#endif