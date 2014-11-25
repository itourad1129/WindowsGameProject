//
//	�ʐM�֌W
//


//--- �萔��` ------------------------------------------------------------------------------------
#define	WINSOCK_MAX	4						// �\�P�b�g�ő吔

#define	BUF_SIZE	256						// ����M�̃o�b�t�@�T�C�Y
#define	TIME_OUT	(60 * 5)				// �^�C���A�E�g�܂ł̎���
//#define	TIME_OUT	0						// �^�C���A�E�g�܂ł̎���(0�Ŗ����A���̏ꍇ�̓N���C�A���g�̃��g���C�͖���)


#define	SOCK_VER	MAKEWORD(2, 0)			// WinSock �̃o�[�W����

enum
{
	SOCK_UNINIT       = -4,					// ��������
	SOCK_ERROR		  = -3,					// �G���[���
	SOCK_CONNECT      = -2,
	SOCK_CONNECT_WAIT = -1,					// �ڑ��҂�
	SOCK_OK			  =  0,
	SOCK_WAIT		  =  0,					// �ڑ��҂����
	SOCK_MAIN		  =  1,					// �l�b�g�J�ʏ��
};

//--- �\���̒�` ----------------------------------------------------------------------------------
typedef struct tugSOCKETDATA				// �\�P�b�g�P�̃f�[�^�\����
{
	bool	bUse;								// �g�p�����A�t���O
	bool	bAccept;							// �A�N�Z�v�g�t���O
	bool	bLost;								// �R�l�N�g���X�g�t���O(�ؒf����m�F����܂ŗL��)
	bool	bAccepteConfir;						// �A�N�Z�v�g���m�F�������̃t���O
	SOCKET	socket;								// �\�P�b�g���ʔԍ�
	IPDATA	ipAccept;							// �ڑ���̂h�o(�ؒf����L��)
}SOCKETDATA, *LPSOCKETDATA;

typedef struct tugWINSOCKDATA				// Winsockets�֌W�\����
{
	SOCKETDATA	cSocket[WINSOCK_MAX + 1];		// �R�l�N�g�p�\�P�b�g�f�[�^
	bool		bInit;							// �������t���O
	uchar		ucMax;							// �ő�ڑ���
}WINSOCKDATA, *LPWINSOCKDATA;


//--- �ϐ���` ------------------------------------------------------------------------------------
static	WINSOCKDATA	sockData;				// �f�[�^

static	bool	bRecvDataError;				// ��M�f�[�^�Ɍ�肪����
static	bool	bReceive;
static	int		iRecvErrorCount;			// �N���C�A���g���T�[�o�[�̃f�[�^����M�ł��Ȃ��ꍇ�̃J�E���^�[
static	int		iConectTime;				// �N���C�A���g���T�[�o�[�ɐڑ�����܂ł̎���(�Đڑ��Ɏg�p)
static	bool	bTimeOut;					// true : �^�C���A�E�g(�N���C�A���g���g�p)
static	bool	bSend;						// true : �f�[�^���M�v��

static	int		iSockMode = SOCK_UNINIT;	// �l�b�g�n���h�����ŏ��͖�������Ԃ���n�܂�B
static	uchar	*ucSendBuf;					// ���M�o�b�t�@
static	uchar	*ucRecvBuf;					// ��M�o�b�t�@

static	IPDATA	ipServer;					// �T�[�o�[��IP(�N���C�A���g���g�p)

static	LPSOCKETDATA	pSock;
static	SOCKADDR_IN		dest;
static	int				iSockNum;
static	IPDATA			ipMyPc;				// ������ IP �A�h���X

static	bool			bServer;			// true : �T�[�o�[

//--- �֐��錾 ------------------------------------------------------------------------------------
static	bool	CloseSocketToIn(int iNum, bool bLost);
static	bool	AcceptNetWork(void);
static	bool	SockSend(int iNum);
extern	bool	SockRecv(int iNum);



/*------------------------------------------------------------------------------*
| <<< �ʐM�@�\������������ >>>
|	�߂�l	true  : ����
|			false : ���s
*------------------------------------------------------------------------------*/
bool	net_init(void)
{

	LPSOCKETDATA	sock = sockData.cSocket;
	WSADATA			wsaData;

	if(sockData.bInit) return false;

	sys_printf("net_init:WinSock�����������܂��B");

	//--- WinSockets������ ------------------------------------------
	if(WSAStartup(SOCK_VER, &wsaData) != 0) return sys_error("net_init:WinSock.DLL������܂���B");

	//--- �w��o�[�W�����ŏ���������Ȃ������ꍇ�͏I�� --------------
	if(wsaData.wVersion != SOCK_VER)
	{
		WSACleanup();
		return	sys_error("net_init:Winsock.DLL�̃o�[�W�������Ⴂ�܂�(�vver2.0�ȏ�)�B");
	}

	//--- �h�o�A�h���X���擾���� -------------------------------------
	char	str[256];
	//memset(&sockData.myIP, 0, sizeof(sockData.myIP));
	if(gethostname(str, sizeof(str)) == 0)
	{
		HOSTENT	*host;
		if((host = gethostbyname(str)) != NULL)
		{									// �f�[�^�Z�b�g
			ipMyPc.d1 = host->h_addr_list[0][0];
			ipMyPc.d2 = host->h_addr_list[0][1];
			ipMyPc.d3 = host->h_addr_list[0][2];
			ipMyPc.d4 = host->h_addr_list[0][3];
			//sys_printf("����PC��IP�� %d,%d,%d,%d �ł��B", host->h_addr_list[0][0], host->h_addr_list[0][1], host->h_addr_list[0][2], host->h_addr_list[0][3]);
		}
	}

	//--- �\�P�b�g�f�[�^�[�������� ----------------------------------
	ZeroMemory(sock, sizeof(SOCKETDATA) * WINSOCK_MAX);

	//--- �ڑ��ő吔���Z�b�g���������t���O�𗧂Ă� ------------------
	sockData.ucMax = WINSOCK_MAX;
	sockData.bInit = true;

	//--- �������m�� ------------------------------------------------
	if(ucSendBuf == NULL)
	{
		ucSendBuf = (uchar *)malloc(BUF_SIZE);
		memset(ucSendBuf, 0, BUF_SIZE);
	}
	if(ucRecvBuf == NULL)
	{
		ucRecvBuf = (uchar *)malloc(BUF_SIZE);
		memset(ucRecvBuf, 0, BUF_SIZE);
	}
	//--- �I�� ------------------------------------------------------
	sys_printf("net_init:���������������܂����B");

	return true;
}



/*------------------------------------------------------------------------------*
| <<< �ʐM�@�\���I������ >>>
|	�߂�l	true  : ����
|			false : ���s
*------------------------------------------------------------------------------*/
bool	net_end(void)
{

	net_connect_close(iSockNum);

	if(!sockData.bInit) return false;

	sys_printf("net_end:�l�b�g���[�N�֘A�̏I���������s���܂��B");

	//--- �I������ --------------------------------------------------
	for(int i = 0; i < WINSOCK_MAX + 1; i++) net_connect_close(i);

	//--- �[���N���A ------------------------------------------------
	ZeroMemory(&sockData, sizeof(sockData));

	//--- WinSockets�̎g�p���I������ --------------------------------
	WSACleanup() ;

	//--- �������J�� ------------------------------------------------
	if(ucSendBuf != NULL)
	{
		free(ucSendBuf);
		ucSendBuf = NULL;
	}
	if(ucRecvBuf != NULL)
	{
		free(ucRecvBuf);
		ucRecvBuf = NULL;
	}
	//--- �I�� ------------------------------------------------------
	sys_printf("net_end:�l�b�g���[�N�֘A�̏I�������͊������܂����B");
	
	return true;
}



/*------------------------------------------------------------------------------*
| <<< winsock �R�[���o�b�N�֐� >>>
*------------------------------------------------------------------------------*/
bool	net_proc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{

	int		iNum, iSock;

	//--- �����G���[���p�����Ȃ��悤�ɃE�G�C�g�������ė}������ ------
	//static	int		iWait = 0;
	//static	DWORD	dwErr = 0;
	//bool	bPrint = true;
//	if(iWait > 0)							// �E�G�C�g���|�����Ă����
	//{										// �����G���[�������烁�b�Z�[�W�\�����Ȃ�
//		iWait--;
//		if(iWait == 0) dwErr = 0;
	//	if((sys.iTimeCount - iWait) < 60 && dwErr == WSAGETSELECTERROR(lParam))
	//		bPrint = false;
	//	iWait = sys.iTimeCount;
	//}

	//--- �G���[�̏ꍇ ----------------------------------------------
	if(WSAGETSELECTERROR(lParam) != 0)
	{
//		iWait = 5;							// �E�G�C�g
		//dwErr = WSAGETSELECTERROR(lParam);

		DWORD	dwErr = WSAGETSELECTERROR(lParam);
		//if(!bPrint) return false;
											// TIME OUT �̏ꍇ�̓��b�Z�[�W����
		if(dwErr == WSAETIMEDOUT) return false;
											// �ڑ��悪������Ȃ��ꍇ
		if(dwErr == WSAENOTSOCK) return sys_error("�ڑ��悪������܂���B");

		sys_error("�\�P�b�g�C�x���g�̒ʒm�ŃG���[���������܂����B%d", dwErr);
		return false;
	}

	//--- �ǂ̃n���h���ւ̃��b�Z�[�W�Ȃ̂����擾 --------------------
	iSock = (SOCKET)wParam;
	for(iNum = 0; iNum < sockData.ucMax && sockData.cSocket[iNum].socket != (uint)iSock; iNum++) ;

	//--- ���b�Z�[�W�̎擾 ------------------------------------------
	uint	uiEvent = WSAGETSELECTEVENT((LPARAM)lParam);
	//uint	uiError = WSAGETSELECTERROR((LPARAM)lParam);

	switch(uiEvent)
	{
	//--- �R�l�N�g���b�Z�[�W ----------------------------------------
	case FD_CONNECT:
		if(iNum >= sockData.ucMax) return false;
		sockData.cSocket[iNum].bAccept = true;
		break;
	//--- �A�N�Z�v�g���b�Z�[�W --------------------------------------
	case FD_ACCEPT:
		return (bool)AcceptNetWork();		// �A�N�Z�v�g����

	//--- �f�[�^��M ------------------------------------------------
	case FD_READ:
		if(iNum >= sockData.ucMax) return false;
		SockRecv(iNum);						// �����f�[�^��M�������s��
		break;
	//--- �f�[�^���M�������� ----------------------------------------
	case FD_WRITE:
		if(iNum >= sockData.ucMax) return false;
		SockSend(iNum);						// �������M�����֐������s����
		break;
	//--- �N���[�Y���b�Z�[�W ----------------------------------------
	case FD_CLOSE:
		if(iNum >= sockData.ucMax) return false;
		CloseSocketToIn(iNum, true);		// �ڑ����I������
		break;
	}
	//--- �I�� ------------------------------------------------------
	return true;
}




/*------------------------------------------------------------------------------*
| <<< ���}�V���ɐڑ����� >>>			���N���C�A���g�p
|	����	ipDat = IP�A�h���X
|	�߂�l	�l�b�g���[�N�̐ڑ��ԍ�
|			 0 >= �ڑ����Ȃ�΁A���̔ԍ�
|			-1 == �ڑ��҂���Ԃɓ���܂�
|			-2 == �ڑ��s��
*------------------------------------------------------------------------------*/
static	int		_net_connect_start(IPDATA *ipDat)
{

	//--- ������ ----------------------------------------------------
	if(sockData.bInit == FALSE) net_init();

	//--- ���ݐڑ����Ă��钆�Ɏw��̂h�o�����邩�m�F ----------------
	pSock = sockData.cSocket;
	for(iSockNum = 0; iSockNum < WINSOCK_MAX; iSockNum++, pSock++)
	{
		if(pSock->bUse && *((int*)&pSock->ipAccept) == *((int*)ipDat) && pSock->bAccept)
		{
			return iSockNum;				// �l�b�g���[�N�n���h����Ԃ�
		}
	}
	//--- �󂫃\�P�b�g������ ----------------------------------------
	for(iSockNum = 0; iSockNum != WINSOCK_MAX && sockData.cSocket[iSockNum].bUse; iSockNum++) ;
	if(iSockNum == WINSOCK_MAX)
	{
		sys_error("net_connect_start:�\�P�b�g�ɋ󂫂�����܂���B");
		return SOCK_ERROR;
	}
	pSock = &sockData.cSocket[iSockNum];

	//--- �h�o�ۑ��E�ϊ� --------------------------------------------
	ZeroMemory(pSock, sizeof(*pSock));
	pSock->ipAccept = *ipDat;
	in_addr	IP		= *((in_addr *)ipDat);

	//--- �\�P�b�g�̍쐬 --------------------------------------------
	pSock->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(pSock->socket == INVALID_SOCKET)
	{
		sys_error("net_connect_start:WINSOCK.DLL �����삵�Ă��܂���A�\�P�b�g�̍쐬�Ɏ��s���܂����B");
		return SOCK_ERROR;
	}
	//--- WinSock ���b�Z�[�W�󂯎��ݒ� ----------------------------
	if(WSAAsyncSelect(pSock->socket, sys.hWnd, WM_WINSOCKMESSAGE, FD_CONNECT | FD_ACCEPT | FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
	{
		closesocket(pSock->socket);
		sys_error("net_connect_start:WSAAsyncSelect �ŃG���[���������܂����B");
		return SOCK_ERROR;
	}
	//--- �R�l�N�g�p�����[�^�Z�b�g ----------------------------------
	dest.sin_family = AF_INET;
	dest.sin_addr   = IP;
	dest.sin_port   = htons(10001);

	//--- �R�l�N�g --------------------------------------------------
	if(connect(pSock->socket, (PSOCKADDR)&dest, sizeof(dest)) == SOCKET_ERROR)
	{
		if(WSAGetLastError() != WSAEWOULDBLOCK)
		{
			closesocket(pSock->socket);
			sys_error("net_connect_start:%d.%d.%d.%d �ւ̐ڑ��Ɏ��s���܂����B", ipDat->d1, ipDat->d2, ipDat->d3, ipDat->d4);
			return SOCK_ERROR;
		}
	}
	//--- ��莞�Ԑڑ���҂� ----------------------------------------
	iSockMode = SOCK_CONNECT_WAIT;
	return iSockNum;
}



/*------------------------------------------------------------------------------*
| <<< ���}�V���ɐڑ����� >>>			���N���C�A���g�p
|	����	ipDat = IP�A�h���X
*------------------------------------------------------------------------------*/
void	net_connect_start(IPDATA *ipDat)
{
	ipServer    = *ipDat;
	iSockMode   = SOCK_CONNECT;
	iConectTime = 0;
}



/*------------------------------------------------------------------------------*
| <<< ���}�V���ɐڑ����� >>>
|	�o��	iNum = �R�l�N�g�ԍ�
|	�߂�l	SOCK_OK or SOCK_CONNECT_WAIT
*------------------------------------------------------------------------------*/
int		net_connect_wait(int *iNum)
{

	//--- �ڑ������܂ő҂� ----------------------------------------
	if(!pSock->bAccept) return SOCK_CONNECT_WAIT;

	IPDATA	*ip = &pSock->ipAccept;

	//--- �R�l�N�g�m�F�A�R�l�N�g���t���O�𗧂Ă� --------------------
	pSock->bAccepteConfir = true;

	//--- �R�l�N�g���X�g�t���O���I�t --------------------------------
	pSock->bLost = false;

	//--- �g�p�t���O�̃Z�b�g ----------------------------------------
	pSock->bUse = true;

	//--- �\�P�b�g�n���h����Ԃ� ------------------------------------
	sys_printf("net_connect_wait:%d,%d,%d,%d �֐ڑ����܂����B", ip->d1, ip->d2, ip->d3, ip->d4);

	*iNum = iSockNum;
	return SOCK_OK;
}



/*------------------------------------------------------------------------------*
| <<< �ڑ����I������ >>>
|	�o��	iNum = �R�l�N�g�ԍ�
|	�߂�l	true or false
*------------------------------------------------------------------------------*/
bool	net_connect_close(int iNum)
{

	LPSOCKETDATA	sock = &sockData.cSocket[iNum];
	IPDATA			buf  = sock->ipAccept;

	if(!sockData.bInit || !sock->bUse) return false;

	//--- �\�P�b�g���N���[�Y���� ------------------------------------
	CloseSocketToIn(iNum, false);

	//--- �[�������� ------------------------------------------------
	ZeroMemory(sock, sizeof(*sock));

	//--- �h�o�����͎c�� --------------------------------------------
	sock->ipAccept = buf;

	//--- �I�� ------------------------------------------------------
	sys_printf("net_connect_close:�ڑ����I�����܂��B");
	
	iSockMode = SOCK_UNINIT;
	return true;
}



/*------------------------------------------------------------------------------*
| <<< �ڑ����󂯂����Ԃɂ��� >>>		���T�[�o�[��
|	�߂�l	true or false
*------------------------------------------------------------------------------*/
//bool	PreparationListenNetWork(void)
bool	net_listen_preparation(void)
{

	SOCKADDR_IN		local;
	LPSOCKETDATA	sock = &sockData.cSocket[WINSOCK_MAX];

	//--- �������`�F�b�N --------------------------------------------
	if(sockData.bInit == FALSE) net_init();
	if(sock->bUse || sock->bAccept)
	{
		return sys_error("net_listen_preparation:�������Ɏ��s���܂����B");
	}

	//--- �A�N�Z�v�g�p�\�P�b�g�̍쐬 --------------------------------
	sock->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock->socket == INVALID_SOCKET) return sys_error("net_listen_preparation:�\�P�b�g�̍쐬�Ɏ��s���܂����BWINSOCK.DLL���쓮���Ă��܂���B");

	//--- WinSock ���b�Z�[�W�󂯎��ݒ� ----------------------------
	if(WSAAsyncSelect(sock->socket, sys.hWnd, WM_WINSOCKMESSAGE, FD_CONNECT | FD_ACCEPT | FD_READ | FD_WRITE | FD_CLOSE))
	{
		closesocket(sock->socket);
		return sys_error("net_listen_preparation:WSAsyncSelect�ŃG���[���������܂����B");
	}

	//--- �쐬�����\�P�b�g�ƃ|�[�g�ԍ��̊֘A�t�� --------------------
	local.sin_family      = AF_INET;
	local.sin_port        = htons(10001);
	local.sin_addr.s_addr = INADDR_ANY;

	if(bind(sock->socket, (SOCKADDR *)&local, sizeof(local)) == SOCKET_ERROR)
	{
		//if(WSAGetLastError() != WSAEWOULDBLOCK)
		//{
			closesocket(sock->socket);
			return sys_error("net_listen_preparation:bind�ł��܂���");
		//}
	}
	//--- �A�N�Z�v�g�p�\�P�b�g��ҋ@��Ԃɂ��� ----------------------
	if(listen(sock->socket, SOMAXCONN) == SOCKET_ERROR)
	{
		closesocket(sock->socket);
		return sys_error("net_listen_preparation:listen�����s���܂���");
	}

	//--- Listen�|�[�g�͏�ɃA�N�Z�v�g��ԂƂ��� --------------------
	sock->bAccept = true;

	//--- �g�p�t���O���Z�b�g ---------------------------------------
	sock->bUse = true;

	//--- �I�� ------------------------------------------------------
	sys_printf("net_listen_preparation:�ڑ����󂯂����Ԃł��B");

	return true;
}



/*------------------------------------------------------------------------------*
| <<< �ڑ����󂯂���Ԃ̉��� >>>
|	�߂�l	true or false
*------------------------------------------------------------------------------*/
bool	net_listen_stop(void)
{

	LPSOCKETDATA	sock = &sockData.cSocket[WINSOCK_MAX];


	//--- �������t���O�𒲂ׂ� --------------------------------------
	if(!sockData.bInit || !sock->bAccept) return false;

	//--- ��u���b�L���O���� ----------------------------------------
	WSAAsyncSelect(sock->socket, sys.hWnd, 0, 0);

	//--- �f�[�^����M�V���b�g�_�E�� --------------------------------
	shutdown(sock->socket, 2);

	//--- �\�P�b�g����� ------------------------------------------
	closesocket(sock->socket);
	sock->socket = NULL;

	//--- �[�������� ------------------------------------------------
	ZeroMemory(sock, sizeof(*sock));

	//--- �I�� ------------------------------------------------------
	sys_printf("net_listen_stop:����I��");
	return true;
}



/*------------------------------------------------------------------------------*
| <<< �A�N�Z�v�g���� >>>
|	�߂�l	true or false
*------------------------------------------------------------------------------*/
static	bool	AcceptNetWork(void)
{

	LPSOCKETDATA	listenSock = &sockData.cSocket[WINSOCK_MAX];
	LPSOCKETDATA	sock;
	int				iSockNum_;
	int				iLen;
	SOCKET			sockID;
	SOCKADDR		sockAdr;


	if(!sockData.bInit || !listenSock->bUse || !listenSock->bAccept) return false;

	//--- �A�N�Z�v�g ------------------------------------------------
	iLen = sizeof(SOCKADDR_IN);
	if((sockID = accept(listenSock->socket, &sockAdr, &iLen)) == INVALID_SOCKET)
	{
		return sys_error("AcceptNetWork:accept�Ɏ��s���܂���");
	}
	//--- �󂫃\�P�b�g������ ----------------------------------------
	for(iSockNum_ = 0; iSockNum_ != WINSOCK_MAX && sockData.cSocket[iSockNum_].bUse; iSockNum_++) ;
	if(iSockNum_ == WINSOCK_MAX)
	{
		closesocket(iSockNum_);				// �A�N�Z�v�g����
		return sys_error("AcceptNetWork:accept�����ۂ��܂����B");
	}
	sock = &sockData.cSocket[iSockNum_];

	//--- �[�������� ------------------------------------------------
	ZeroMemory(sock, sizeof(*sock));

	//--- �h�o�̕ۑ� ------------------------------------------------
	sock->ipAccept = *((IPDATA *)&sockAdr.sa_family);

	//--- �\�P�b�gID��ۑ� ------------------------------------------
	sock->socket = sockID;

	//--- WinSock ���b�Z�[�W�󂯎��ݒ� ----------------------------
	WSAAsyncSelect(sock->socket, sys.hWnd, 0, 0);
	if(WSAAsyncSelect(sock->socket, sys.hWnd, WM_WINSOCKMESSAGE, FD_WRITE | FD_READ | FD_CLOSE) == SOCKET_ERROR)
	{
		closesocket(sock->socket);
		return sys_error("AcceptNetWork:WSAAsyncSelect�̃G���[");
	}
	//--- �R�l�N�g�m�F�t���O��|�� ----------------------------------
	sock->bAccepteConfir = false;

	//--- �R�l�N�g���t���O�����Ă� ----------------------------------
	sock->bAccept = true;

	//--- �g�p�t���O�̃Z�b�g ----------------------------------------
	sock->bUse = true;

	//--- �I�� ------------------------------------------------------
	return true;
}



/*------------------------------------------------------------------------------*
| <<< �ڑ����I������(����) >>>
|	����	iNum  = �ڑ��ԍ�
|			bLost = true : �Ō�̃f�[�^�𑗎�M����(������)
|	�߂�l	true or false
*------------------------------------------------------------------------------*/
static	bool	CloseSocketToIn(int iNum, bool bLost)
{

	LPSOCKETDATA	sock = &sockData.cSocket[iNum];


	//--- �g�p�t���O�Ə������t���O���݂� ----------------------------
	if(!sockData.bInit || !sock->bUse) return false; 

	//--- ���X�g�t���O�𗧂Ă� --------------------------------------
	sock->bLost = true;

	//--- ��u���b�L���O���� ----------------------------------------
	WSAAsyncSelect(sock->socket, sys.hWnd, 0, 0);

	// �f�[�^����M�V���b�g�_�E��
	//shutdown( Sock->socket , 2 ) ;

	//--- �\�P�b�g����� ------------------------------------------
	closesocket(sock->socket);
	sock->socket = NULL;

	//--- �I�� ------------------------------------------------------
	return true;
}



/*------------------------------------------------------------------------------*
| <<< �f�[�^�̎�M����(����) >>>
|	����	iNum = �ڑ��ԍ�
|	�߂�l	true or false
*------------------------------------------------------------------------------*/
bool	SockRecv(int iNum)
{

	LPSOCKETDATA	sock = &sockData.cSocket[iNum];


	if(!sockData.bInit) return false;

	bRecvDataError = false;					// ��M�f�[�^�t���O�N���A�[

#if 1
	//--- �f�[�^��M ------------------------------------------------
	int	iRecvLen = recv(sock->socket, (char *)ucRecvBuf, BUF_SIZE, 0);
	if(iRecvLen != BUF_SIZE)
	{
		sys_error("SockRecv:���M���s");
		bRecvDataError = true;
		return false;
	}
	//--- �`�F�b�N�T���𒲂ׂ� --------------------------------------
	uint	uiAns = 0;
	uchar	*p    = &ucRecvBuf[4];
	for(int i = 4; i < BUF_SIZE; i++, p++) uiAns += *p;

	uint	*pSum = (uint*)ucRecvBuf;
	if(*pSum != uiAns)
	{
		sys_error("SockRecv:��M���s�B�`�F�b�N�T���������܂���B");
		bRecvDataError = true;
		return false;
	}
#else
	recv(sock->socket, (char *)ucRecvBuf, BUF_SIZE, 0);
#endif
	//--- �I�� ------------------------------------------------------
	bReceive = true;
	return true;
}



/*------------------------------------------------------------------------------*
| <<< �f�[�^�̑��M���� >>>
|	����	iNum = �ڑ��ԍ�
|	�߂�l	true or false
*------------------------------------------------------------------------------*/
static	int	iSendErrorCount;				// ���M�̘A�����s��
bool	SockSend(int iNum)
{

	if(!bSend) return false;				// net_send �ɂ��v�����������ꍇ�̂ݑΉ�����

	bSend = false;

	LPSOCKETDATA	sock = &sockData.cSocket[iNum];

#if 1
	//--- �`�F�b�N�T������� ----------------------------------------
	uint	uiAns = 0;
	uchar	*p    = &ucSendBuf[4];
	for(int i = 4; i < BUF_SIZE; i++, p++) uiAns += *p;
	uint	*pSum = (uint*)ucSendBuf;
	*pSum = uiAns;

	//--- �����Ńf�[�^���M ------------------------------------------
	int	iSendLen = send(sock->socket, (char *)ucSendBuf, BUF_SIZE, 0);
	if(iSendLen != BUF_SIZE)
	{
		iSendErrorCount++;
											// �����ł͓K���� 30 ��A���Ŏ�M�Ɏ��s������G���[
		if(iSendErrorCount > 30) net_connect_close(iNum);
		//sys_error("SockSend:���M���s");
		return false;
	}
#else
	send(sock->socket, (char *)ucSendBuf, BUF_SIZE, 0);
#endif
	//--- �I�� ------------------------------------------------------
	iSendErrorCount = 0;
	return true;
}



/*------------------------------------------------------------------------------*
| <<< �V���ɐڑ������ʐM����𓾂� >>>
|	�߂�l	-1  = �ڑ��Ȃ�
|			-1 != �ڑ��ԍ�
*------------------------------------------------------------------------------*/
int	net_new_accept_get(void)
{
	int				i;
	LPSOCKETDATA	sock;

	//--- �������t���O�𒲂ׂ� --------------------------------------
	if(!sockData.bInit) return false;

	//--- �ڑ��ԍ���T�� --------------------------------------------
	sock = sockData.cSocket;
	for(i = 0; i < WINSOCK_MAX; i++, sock++)
	{
		if(sock->bUse && sock->bAccept && !sock->bAccepteConfir)
		{
			sock->bAccepteConfir = true;
			//sys_printf("net_new_accept_get:�l�b�g���[�NID=%d ���ڑ��\�ł��B", i);
			return i;
		}
	}
	//--- �I�� ------------------------------------------------------
	return -1;
}



/*------------------------------------------------------------------------------*
| <<< �ڑ���ؒf���ꂽ�ʐM����𓾂� >>>
|	�߂�l	-1  = �ؒf�Ȃ�
|			-1 != �ʐM�ԍ�
*------------------------------------------------------------------------------*/
int		net_lost_get(void)
{

	int				i;
	LPSOCKETDATA	sock;

	//--- ��������Ԃ𒲂ׂ� ----------------------------------------
	if(!sockData.bInit) return -1;

	//--- �ؒf���ꂽ�ԍ���T�� --------------------------------------
	sock = sockData.cSocket;
	for(i = 0; i < WINSOCK_MAX + 1; i++, sock++)
	{
		if(sock->bUse && sock->bLost)
		{
			sock->bUse	= false;
			sock->bLost = true;
			return i;
		}
	}
	//--- �I�� ------------------------------------------------------
	return -1;
}



/*------------------------------------------------------------------------------*
| <<< �ڑ���̂h�o�𓾂� >>>
|	����	iNum  = �ڑ��ԍ�
|	�o��	ipOut = IP���o�͂����
|	�߂�l	true or false
*------------------------------------------------------------------------------*/
bool	net_ip_get(IPDATA *ipOut, int iNum)
{
	
	LPSOCKETDATA	sock = &sockData.cSocket[iNum];

	if(!sock->bUse) return false;

	//--- �h�o�������� --------------------------------------------
	*ipOut = sock->ipAccept;

	//--- �I�� ------------------------------------------------------
	return true;
}



/*------------------------------------------------------------------------------*
| <<< winsock ���C�� >>>
|	�߂�l	true : ��M�f�[�^����
*------------------------------------------------------------------------------*/
bool	net_main(void)
{

	if(!sockData.bInit)
	{
	//	net_init();
		return false;
	}

	bool	bRet = false;

//#ifndef SERVER
	//--- �N���C�A���g --------------------------------------------------------
	if(!bServer)
	{
		switch(iSockMode)
		{
		case SOCK_CONNECT:
			iConectTime = 0;
			bTimeOut    = false;
			_net_connect_start(&ipServer);
			break;

		case SOCK_CONNECT_WAIT:
			#if	TIME_OUT != 0					// �^�C���A�E�g�`�F�b�N
				iConectTime++;
			#endif
			if(iConectTime > TIME_OUT)
			{
				net_connect_close(iSockNum);
				iSockMode = SOCK_CONNECT;
				break;
			}
			if(net_connect_wait(&iSockNum) == SOCK_OK)
			{
				iSockMode       = SOCK_WAIT;
				iRecvErrorCount = 0;
			}
			break;
		case SOCK_WAIT:						// ��x�ł��f�[�^�̎�M������� SOCK_MAIN ��
			iSockMode = SOCK_MAIN;
		case SOCK_MAIN:						// �ڑ����Ă���ꍇ�̂ݏ���������
			if(bReceive)					// ��M������ꍇ�̏���
			{
				bReceive        = false;
				bRet            = true;
				iRecvErrorCount = 0;
			}
			else
			{
				#if	TIME_OUT != 0
					iRecvErrorCount++;
				#endif
				if(iRecvErrorCount > TIME_OUT)	// �f�[�^�̎�M���P�b�ԏo���Ȃ������ꍇ�͐ؒf�ƂȂ�
				{
					bTimeOut = true;
					net_connect_close(iSockNum);
				}
			}
			break;
		}
	}
//#else
	//--- �T�[�o�[ ------------------------------------------------------------
	else
	{
		switch(iSockMode)
		{
		case SOCK_UNINIT:						// �ڑ����Ă���̂�҂�Ԃɂ���
			if(net_listen_preparation()) iSockMode = SOCK_WAIT;
			break;
		case SOCK_WAIT:							// �V�����ڑ����������炻�̃l�b�g���[�N�n���h���𓾂�
			iSockNum = net_new_accept_get();
			if(iSockNum != -1)
			{
				IPDATA	ip;
				net_listen_stop();				// �ڑ��̎�t���I������
				net_ip_get(&ip, iSockNum);		// �ڑ����Ă����}�V���̂h�o�A�h���X�𓾂�
				sys_printf("ID:%02d(%d,%d,%d,%d)���ڑ�����܂����B", iSockNum, ip.d1, ip.d2, ip.d3, ip.d4);
				iSockMode = SOCK_MAIN;
			}
			break;
		case SOCK_MAIN:							// �ڑ����̏���
			if(bReceive)
			{									// �f�[�^��M
				bReceive = false;
				bRet     = true;
			}
			if(net_lost_get() == iSockNum)		// �ؒf���ꂽ�ڑ������܂ŒʐM���Ă����肾�����ꍇ���[�v�𔲂���
			{
				sys_printf("%d�Ԃ��ؒf����܂����B", iSockNum);
				iSockMode = SOCK_UNINIT;
			}
			break;
		}
	}
//#endif
	return bRet;
}



/*------------------------------------------------------------------------------*
| <<< �f�[�^���M >>>
|	����	pBuf   = �]���f�[�^�̊i�[�A�h���X
|			uiSize = �]���T�C�Y
|	�߂�l	true or false
*------------------------------------------------------------------------------*/
bool	net_send(void *pBuf, uint uiSize)
{
	if(ucSendBuf == NULL) return false;

	if(uiSize >= BUF_SIZE - 4)
	{
		return sys_error("net_send:���M�f�[�^���傫�����܂��B%d/%d", uiSize, BUF_SIZE - 4);
	}
	memcpy(&ucSendBuf[4], pBuf, uiSize);
	bSend = true;
	return	SockSend(iSockNum);
}



/*------------------------------------------------------------------------------*
| <<< �f�[�^��M >>>
|	����	pBuf   = ��M�f�[�^�̊i�[�A�h���X
|			uiSize = ��M�T�C�Y
|	�߂�l	true or false
*------------------------------------------------------------------------------*/
bool	net_recv(void *pBuf, uint uiSize)
{
	if(ucRecvBuf == NULL) return false;
	if(bRecvDataError)    return false;
	if(!bReceive)		  return false;

	if(uiSize >= BUF_SIZE - 4)
	{
		return sys_error("net_send:��M�f�[�^���傫�����܂��B%d/%d", uiSize, BUF_SIZE - 4);
	}
											// �G���[���N���Ă��Ȃ����ɃR�s�[����
	memcpy(pBuf, &ucRecvBuf[4], uiSize);
	return true;
}



/*------------------------------------------------------------------------------*
| <<< �ڑ��\�����ׂ� >>>
|	�߂�l	true : �ڑ��\
*------------------------------------------------------------------------------*/
bool	net_accept_check(void)
{
	if(iSockMode == SOCK_UNINIT) return true;
	else						 return false;
}



/*------------------------------------------------------------------------------*
| <<< �ʐM�\�����ׂ� >>>
|	�߂�l	true : �ڑ��\
*------------------------------------------------------------------------------*/
bool	net_connect_check(void)
{
	if(iSockMode == SOCK_MAIN) return true;
	else					   return false;
}



/*------------------------------------------------------------------------------*
| <<< ���� PC �� IP �𓾂� >>>
|	�߂�l	ip �A�h���X
*------------------------------------------------------------------------------*/
IPDATA	*net_myip_get(void)
{
	return &ipMyPc;
}



/*------------------------------------------------------------------------------*
| <<< �T�[�o�[�ƃN���C�A���g�̐؂�ւ��X�C�b�` >>>
|	����	bServer = true  : �T�[�o�[�ɂ���
|					  false : �N���C�A���g�ɂ���
*------------------------------------------------------------------------------*/
void	net_set(bool bSw)
{
	bServer = bSw;
}



/*------------------------------------------------------------------------------*
| <<< �T�[�o�[�����ׂ� >>>
|	�߂�l	true  : �T�[�o�[
|			false : �N���C�A���g
*------------------------------------------------------------------------------*/
bool	net_get(void)
{
	return bServer;
}

