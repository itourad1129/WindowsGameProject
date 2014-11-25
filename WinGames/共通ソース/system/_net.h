#ifndef	__NET_H
#define	__NET_H


//--- 定数定義 ------------------------------------------------------------------------------------
#define WM_WINSOCKMESSAGE (WM_USER + 300)	// winsock のイベント

//--- 構造体 --------------------------------------------------------------------------------------
typedef struct tugIPDATA					// WinSockets使用時のアドレス指定用構造体
{
	uchar	d1, d2, d3, d4;						// アドレス値
}IPDATA, *LPIPDATA;



extern	bool	net_init(void);
extern	bool	net_end(void);
extern	bool	net_proc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
extern	void	net_connect_start(IPDATA *ipDat);
extern	int		net_connect_wait(int *iNum);
extern	bool	net_connect_close(int iNum);

extern	bool	net_listen_preparation(void);
extern	bool	net_listen_stop(void);
extern	int		net_new_accept_get(void);
extern	bool	net_ip_get(IPDATA *ipOut, int iNum);
extern	int		net_lost_get(void);

extern	bool	net_main(void);


extern	bool	net_send(void *pBuf, uint uiSize);
extern	bool	net_recv(void *pBuf, uint uiSize);

extern	bool	net_accept_check(void);
extern	bool	net_connect_check(void);

extern	void	net_set(bool bServer);		// サーバーとクライアントの切り替えスイッチ
extern	bool	net_get(void);				// サーバーかクライアントかを調べる

extern	IPDATA	*net_myip_get(void);

#endif