#ifndef	__NET_H
#define	__NET_H


//--- �萔��` ------------------------------------------------------------------------------------
#define WM_WINSOCKMESSAGE (WM_USER + 300)	// winsock �̃C�x���g

//--- �\���� --------------------------------------------------------------------------------------
typedef struct tugIPDATA					// WinSockets�g�p���̃A�h���X�w��p�\����
{
	uchar	d1, d2, d3, d4;						// �A�h���X�l
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

extern	void	net_server_set(BOOL bServer);	// �T�[�o�[�ƃN���C�A���g�̐؂�ւ��X�C�b�`
extern	BOOL	net_server_check(void);			// �T�[�o�[���N���C�A���g���𒲂ׂ�
extern	void	net_server_connect_preparation(void);

extern	IPDATA	*net_ip(void);

#endif