//
//	通信関係
//


//--- 定数定義 ------------------------------------------------------------------------------------
#define	WINSOCK_MAX	4						// ソケット最大数

#define	BUF_SIZE	256						// 送受信のバッファサイズ
#define	TIME_OUT	(60 * 5)				// タイムアウトまでの時間
//#define	TIME_OUT	0						// タイムアウトまでの時間(0で無効、この場合はクライアントのリトライは無し)


#define	SOCK_VER	MAKEWORD(2, 0)			// WinSock のバージョン

enum
{
	SOCK_UNINIT       = -4,					// 未初期化
	SOCK_ERROR		  = -3,					// エラー状態
	SOCK_CONNECT      = -2,
	SOCK_CONNECT_WAIT = -1,					// 接続待ち
	SOCK_OK			  =  0,
	SOCK_WAIT		  =  0,					// 接続待ち状態
	SOCK_MAIN		  =  1,					// ネット開通状態
};

//--- 構造体定義 ----------------------------------------------------------------------------------
typedef struct tugSOCKETDATA				// ソケット単体データ構造体
{
	bool	bUse;								// 使用中か、フラグ
	bool	bAccept;							// アクセプトフラグ
	bool	bLost;								// コネクトロストフラグ(切断後も確認するまで有効)
	bool	bAccepteConfir;						// アクセプトを確認したかのフラグ
	SOCKET	socket;								// ソケット識別番号
	IPDATA	ipAccept;							// 接続先のＩＰ(切断後も有効)
}SOCKETDATA, *LPSOCKETDATA;

typedef struct tugWINSOCKDATA				// Winsockets関係構造体
{
	SOCKETDATA	cSocket[WINSOCK_MAX + 1];		// コネクト用ソケットデータ
	bool		bInit;							// 初期化フラグ
	uchar		ucMax;							// 最大接続数
}WINSOCKDATA, *LPWINSOCKDATA;


//--- 変数定義 ------------------------------------------------------------------------------------
static	WINSOCKDATA	sockData;				// データ

static	bool	bRecvDataError;				// 受信データに誤りがある
static	bool	bReceive;
static	int		iRecvErrorCount;			// クライアントがサーバーのデータを受信できない場合のカウンター
static	int		iConectTime;				// クライアントがサーバーに接続するまでの時間(再接続に使用)
static	bool	bTimeOut;					// true : タイムアウト(クライアントが使用)
static	bool	bSend;						// true : データ送信要求

static	int		iSockMode = SOCK_UNINIT;	// ネットハンドル※最初は未初期状態から始まる。
static	uchar	*ucSendBuf;					// 送信バッファ
static	uchar	*ucRecvBuf;					// 受信バッファ

static	IPDATA	ipServer;					// サーバーのIP(クライアントが使用)

static	LPSOCKETDATA	pSock;
static	SOCKADDR_IN		dest;
static	int				iSockNum;
static	IPDATA			ipMyPc;				// 自分の IP アドレス

static	bool			bServer;			// true : サーバー

//--- 関数宣言 ------------------------------------------------------------------------------------
static	bool	CloseSocketToIn(int iNum, bool bLost);
static	bool	AcceptNetWork(void);
static	bool	SockSend(int iNum);
extern	bool	SockRecv(int iNum);



/*------------------------------------------------------------------------------*
| <<< 通信機能を初期化する >>>
|	戻り値	true  : 成功
|			false : 失敗
*------------------------------------------------------------------------------*/
bool	net_init(void)
{

	LPSOCKETDATA	sock = sockData.cSocket;
	WSADATA			wsaData;

	if(sockData.bInit) return false;

	sys_printf("net_init:WinSockを初期化します。");

	//--- WinSockets初期化 ------------------------------------------
	if(WSAStartup(SOCK_VER, &wsaData) != 0) return sys_error("net_init:WinSock.DLLがありません。");

	//--- 指定バージョンで初期化されなかった場合は終了 --------------
	if(wsaData.wVersion != SOCK_VER)
	{
		WSACleanup();
		return	sys_error("net_init:Winsock.DLLのバージョンが違います(要ver2.0以上)。");
	}

	//--- ＩＰアドレスを取得する -------------------------------------
	char	str[256];
	//memset(&sockData.myIP, 0, sizeof(sockData.myIP));
	if(gethostname(str, sizeof(str)) == 0)
	{
		HOSTENT	*host;
		if((host = gethostbyname(str)) != NULL)
		{									// データセット
			ipMyPc.d1 = host->h_addr_list[0][0];
			ipMyPc.d2 = host->h_addr_list[0][1];
			ipMyPc.d3 = host->h_addr_list[0][2];
			ipMyPc.d4 = host->h_addr_list[0][3];
			//sys_printf("このPCのIPは %d,%d,%d,%d です。", host->h_addr_list[0][0], host->h_addr_list[0][1], host->h_addr_list[0][2], host->h_addr_list[0][3]);
		}
	}

	//--- ソケットデータゼロ初期化 ----------------------------------
	ZeroMemory(sock, sizeof(SOCKETDATA) * WINSOCK_MAX);

	//--- 接続最大数をセットし初期化フラグを立てる ------------------
	sockData.ucMax = WINSOCK_MAX;
	sockData.bInit = true;

	//--- メモリ確保 ------------------------------------------------
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
	//--- 終了 ------------------------------------------------------
	sys_printf("net_init:初期化を完了しました。");

	return true;
}



/*------------------------------------------------------------------------------*
| <<< 通信機能を終了する >>>
|	戻り値	true  : 成功
|			false : 失敗
*------------------------------------------------------------------------------*/
bool	net_end(void)
{

	net_connect_close(iSockNum);

	if(!sockData.bInit) return false;

	sys_printf("net_end:ネットワーク関連の終了処理を行います。");

	//--- 終了処理 --------------------------------------------------
	for(int i = 0; i < WINSOCK_MAX + 1; i++) net_connect_close(i);

	//--- ゼロクリア ------------------------------------------------
	ZeroMemory(&sockData, sizeof(sockData));

	//--- WinSocketsの使用を終了する --------------------------------
	WSACleanup() ;

	//--- メモリ開放 ------------------------------------------------
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
	//--- 終了 ------------------------------------------------------
	sys_printf("net_end:ネットワーク関連の終了処理は完了しました。");
	
	return true;
}



/*------------------------------------------------------------------------------*
| <<< winsock コールバック関数 >>>
*------------------------------------------------------------------------------*/
bool	net_proc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{

	int		iNum, iSock;

	//--- 同じエラーが頻発しないようにウエイトをかけて抑制する ------
	//static	int		iWait = 0;
	//static	DWORD	dwErr = 0;
	//bool	bPrint = true;
//	if(iWait > 0)							// ウエイトが掛かっている間
	//{										// 同じエラーが来たらメッセージ表示しない
//		iWait--;
//		if(iWait == 0) dwErr = 0;
	//	if((sys.iTimeCount - iWait) < 60 && dwErr == WSAGETSELECTERROR(lParam))
	//		bPrint = false;
	//	iWait = sys.iTimeCount;
	//}

	//--- エラーの場合 ----------------------------------------------
	if(WSAGETSELECTERROR(lParam) != 0)
	{
//		iWait = 5;							// ウエイト
		//dwErr = WSAGETSELECTERROR(lParam);

		DWORD	dwErr = WSAGETSELECTERROR(lParam);
		//if(!bPrint) return false;
											// TIME OUT の場合はメッセージ無し
		if(dwErr == WSAETIMEDOUT) return false;
											// 接続先が見つからない場合
		if(dwErr == WSAENOTSOCK) return sys_error("接続先が見つかりません。");

		sys_error("ソケットイベントの通知でエラーが発生しました。%d", dwErr);
		return false;
	}

	//--- どのハンドルへのメッセージなのかを取得 --------------------
	iSock = (SOCKET)wParam;
	for(iNum = 0; iNum < sockData.ucMax && sockData.cSocket[iNum].socket != (uint)iSock; iNum++) ;

	//--- メッセージの取得 ------------------------------------------
	uint	uiEvent = WSAGETSELECTEVENT((LPARAM)lParam);
	//uint	uiError = WSAGETSELECTERROR((LPARAM)lParam);

	switch(uiEvent)
	{
	//--- コネクトメッセージ ----------------------------------------
	case FD_CONNECT:
		if(iNum >= sockData.ucMax) return false;
		sockData.cSocket[iNum].bAccept = true;
		break;
	//--- アクセプトメッセージ --------------------------------------
	case FD_ACCEPT:
		return (bool)AcceptNetWork();		// アクセプト処理

	//--- データ受信 ------------------------------------------------
	case FD_READ:
		if(iNum >= sockData.ucMax) return false;
		SockRecv(iNum);						// 内部データ受信処理を行う
		break;
	//--- データ送信準備完了 ----------------------------------------
	case FD_WRITE:
		if(iNum >= sockData.ucMax) return false;
		SockSend(iNum);						// 内部送信処理関数を実行する
		break;
	//--- クローズメッセージ ----------------------------------------
	case FD_CLOSE:
		if(iNum >= sockData.ucMax) return false;
		CloseSocketToIn(iNum, true);		// 接続を終了する
		break;
	}
	//--- 終了 ------------------------------------------------------
	return true;
}




/*------------------------------------------------------------------------------*
| <<< 他マシンに接続する >>>			※クライアント用
|	入力	ipDat = IPアドレス
|	戻り値	ネットワークの接続番号
|			 0 >= 接続中ならば、その番号
|			-1 == 接続待ち状態に入ります
|			-2 == 接続不可
*------------------------------------------------------------------------------*/
static	int		_net_connect_start(IPDATA *ipDat)
{

	//--- 初期化 ----------------------------------------------------
	if(sockData.bInit == FALSE) net_init();

	//--- 現在接続している中に指定のＩＰがあるか確認 ----------------
	pSock = sockData.cSocket;
	for(iSockNum = 0; iSockNum < WINSOCK_MAX; iSockNum++, pSock++)
	{
		if(pSock->bUse && *((int*)&pSock->ipAccept) == *((int*)ipDat) && pSock->bAccept)
		{
			return iSockNum;				// ネットワークハンドルを返す
		}
	}
	//--- 空きソケットを検索 ----------------------------------------
	for(iSockNum = 0; iSockNum != WINSOCK_MAX && sockData.cSocket[iSockNum].bUse; iSockNum++) ;
	if(iSockNum == WINSOCK_MAX)
	{
		sys_error("net_connect_start:ソケットに空きがありません。");
		return SOCK_ERROR;
	}
	pSock = &sockData.cSocket[iSockNum];

	//--- ＩＰ保存・変換 --------------------------------------------
	ZeroMemory(pSock, sizeof(*pSock));
	pSock->ipAccept = *ipDat;
	in_addr	IP		= *((in_addr *)ipDat);

	//--- ソケットの作成 --------------------------------------------
	pSock->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(pSock->socket == INVALID_SOCKET)
	{
		sys_error("net_connect_start:WINSOCK.DLL が動作していません、ソケットの作成に失敗しました。");
		return SOCK_ERROR;
	}
	//--- WinSock メッセージ受け取り設定 ----------------------------
	if(WSAAsyncSelect(pSock->socket, sys.hWnd, WM_WINSOCKMESSAGE, FD_CONNECT | FD_ACCEPT | FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
	{
		closesocket(pSock->socket);
		sys_error("net_connect_start:WSAAsyncSelect でエラーが発生しました。");
		return SOCK_ERROR;
	}
	//--- コネクトパラメータセット ----------------------------------
	dest.sin_family = AF_INET;
	dest.sin_addr   = IP;
	dest.sin_port   = htons(10001);

	//--- コネクト --------------------------------------------------
	if(connect(pSock->socket, (PSOCKADDR)&dest, sizeof(dest)) == SOCKET_ERROR)
	{
		if(WSAGetLastError() != WSAEWOULDBLOCK)
		{
			closesocket(pSock->socket);
			sys_error("net_connect_start:%d.%d.%d.%d への接続に失敗しました。", ipDat->d1, ipDat->d2, ipDat->d3, ipDat->d4);
			return SOCK_ERROR;
		}
	}
	//--- 一定時間接続を待つ ----------------------------------------
	iSockMode = SOCK_CONNECT_WAIT;
	return iSockNum;
}



/*------------------------------------------------------------------------------*
| <<< 他マシンに接続する >>>			※クライアント用
|	入力	ipDat = IPアドレス
*------------------------------------------------------------------------------*/
void	net_connect_start(IPDATA *ipDat)
{
	ipServer    = *ipDat;
	iSockMode   = SOCK_CONNECT;
	iConectTime = 0;
}



/*------------------------------------------------------------------------------*
| <<< 他マシンに接続する >>>
|	出力	iNum = コネクト番号
|	戻り値	SOCK_OK or SOCK_CONNECT_WAIT
*------------------------------------------------------------------------------*/
int		net_connect_wait(int *iNum)
{

	//--- 接続されるまで待つ ----------------------------------------
	if(!pSock->bAccept) return SOCK_CONNECT_WAIT;

	IPDATA	*ip = &pSock->ipAccept;

	//--- コネクト確認、コネクト中フラグを立てる --------------------
	pSock->bAccepteConfir = true;

	//--- コネクトロストフラグをオフ --------------------------------
	pSock->bLost = false;

	//--- 使用フラグのセット ----------------------------------------
	pSock->bUse = true;

	//--- ソケットハンドルを返す ------------------------------------
	sys_printf("net_connect_wait:%d,%d,%d,%d へ接続しました。", ip->d1, ip->d2, ip->d3, ip->d4);

	*iNum = iSockNum;
	return SOCK_OK;
}



/*------------------------------------------------------------------------------*
| <<< 接続を終了する >>>
|	出力	iNum = コネクト番号
|	戻り値	true or false
*------------------------------------------------------------------------------*/
bool	net_connect_close(int iNum)
{

	LPSOCKETDATA	sock = &sockData.cSocket[iNum];
	IPDATA			buf  = sock->ipAccept;

	if(!sockData.bInit || !sock->bUse) return false;

	//--- ソケットをクローズする ------------------------------------
	CloseSocketToIn(iNum, false);

	//--- ゼロ初期化 ------------------------------------------------
	ZeroMemory(sock, sizeof(*sock));

	//--- ＩＰだけは残す --------------------------------------------
	sock->ipAccept = buf;

	//--- 終了 ------------------------------------------------------
	sys_printf("net_connect_close:接続を終了します。");
	
	iSockMode = SOCK_UNINIT;
	return true;
}



/*------------------------------------------------------------------------------*
| <<< 接続を受けられる状態にする >>>		※サーバー側
|	戻り値	true or false
*------------------------------------------------------------------------------*/
//bool	PreparationListenNetWork(void)
bool	net_listen_preparation(void)
{

	SOCKADDR_IN		local;
	LPSOCKETDATA	sock = &sockData.cSocket[WINSOCK_MAX];

	//--- 初期化チェック --------------------------------------------
	if(sockData.bInit == FALSE) net_init();
	if(sock->bUse || sock->bAccept)
	{
		return sys_error("net_listen_preparation:初期化に失敗しました。");
	}

	//--- アクセプト用ソケットの作成 --------------------------------
	sock->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock->socket == INVALID_SOCKET) return sys_error("net_listen_preparation:ソケットの作成に失敗しました。WINSOCK.DLLが作動していません。");

	//--- WinSock メッセージ受け取り設定 ----------------------------
	if(WSAAsyncSelect(sock->socket, sys.hWnd, WM_WINSOCKMESSAGE, FD_CONNECT | FD_ACCEPT | FD_READ | FD_WRITE | FD_CLOSE))
	{
		closesocket(sock->socket);
		return sys_error("net_listen_preparation:WSAsyncSelectでエラーが発生しました。");
	}

	//--- 作成したソケットとポート番号の関連付け --------------------
	local.sin_family      = AF_INET;
	local.sin_port        = htons(10001);
	local.sin_addr.s_addr = INADDR_ANY;

	if(bind(sock->socket, (SOCKADDR *)&local, sizeof(local)) == SOCKET_ERROR)
	{
		//if(WSAGetLastError() != WSAEWOULDBLOCK)
		//{
			closesocket(sock->socket);
			return sys_error("net_listen_preparation:bindできません");
		//}
	}
	//--- アクセプト用ソケットを待機状態にする ----------------------
	if(listen(sock->socket, SOMAXCONN) == SOCKET_ERROR)
	{
		closesocket(sock->socket);
		return sys_error("net_listen_preparation:listenを失敗しました");
	}

	//--- Listenポートは常にアクセプト状態とする --------------------
	sock->bAccept = true;

	//--- 使用フラグをセット ---------------------------------------
	sock->bUse = true;

	//--- 終了 ------------------------------------------------------
	sys_printf("net_listen_preparation:接続を受けられる状態です。");

	return true;
}



/*------------------------------------------------------------------------------*
| <<< 接続を受けつけ状態の解除 >>>
|	戻り値	true or false
*------------------------------------------------------------------------------*/
bool	net_listen_stop(void)
{

	LPSOCKETDATA	sock = &sockData.cSocket[WINSOCK_MAX];


	//--- 初期化フラグを調べる --------------------------------------
	if(!sockData.bInit || !sock->bAccept) return false;

	//--- 非ブロッキング解除 ----------------------------------------
	WSAAsyncSelect(sock->socket, sys.hWnd, 0, 0);

	//--- データ送受信シャットダウン --------------------------------
	shutdown(sock->socket, 2);

	//--- ソケットを閉じる ------------------------------------------
	closesocket(sock->socket);
	sock->socket = NULL;

	//--- ゼロ初期化 ------------------------------------------------
	ZeroMemory(sock, sizeof(*sock));

	//--- 終了 ------------------------------------------------------
	sys_printf("net_listen_stop:正常終了");
	return true;
}



/*------------------------------------------------------------------------------*
| <<< アクセプトする >>>
|	戻り値	true or false
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

	//--- アクセプト ------------------------------------------------
	iLen = sizeof(SOCKADDR_IN);
	if((sockID = accept(listenSock->socket, &sockAdr, &iLen)) == INVALID_SOCKET)
	{
		return sys_error("AcceptNetWork:acceptに失敗しました");
	}
	//--- 空きソケットを検索 ----------------------------------------
	for(iSockNum_ = 0; iSockNum_ != WINSOCK_MAX && sockData.cSocket[iSockNum_].bUse; iSockNum_++) ;
	if(iSockNum_ == WINSOCK_MAX)
	{
		closesocket(iSockNum_);				// アクセプト拒否
		return sys_error("AcceptNetWork:acceptを拒否しました。");
	}
	sock = &sockData.cSocket[iSockNum_];

	//--- ゼロ初期化 ------------------------------------------------
	ZeroMemory(sock, sizeof(*sock));

	//--- ＩＰの保存 ------------------------------------------------
	sock->ipAccept = *((IPDATA *)&sockAdr.sa_family);

	//--- ソケットIDを保存 ------------------------------------------
	sock->socket = sockID;

	//--- WinSock メッセージ受け取り設定 ----------------------------
	WSAAsyncSelect(sock->socket, sys.hWnd, 0, 0);
	if(WSAAsyncSelect(sock->socket, sys.hWnd, WM_WINSOCKMESSAGE, FD_WRITE | FD_READ | FD_CLOSE) == SOCKET_ERROR)
	{
		closesocket(sock->socket);
		return sys_error("AcceptNetWork:WSAAsyncSelectのエラー");
	}
	//--- コネクト確認フラグを倒す ----------------------------------
	sock->bAccepteConfir = false;

	//--- コネクト中フラグをたてる ----------------------------------
	sock->bAccept = true;

	//--- 使用フラグのセット ----------------------------------------
	sock->bUse = true;

	//--- 終了 ------------------------------------------------------
	return true;
}



/*------------------------------------------------------------------------------*
| <<< 接続を終了する(内部) >>>
|	入力	iNum  = 接続番号
|			bLost = true : 最後のデータを送受信する(未実装)
|	戻り値	true or false
*------------------------------------------------------------------------------*/
static	bool	CloseSocketToIn(int iNum, bool bLost)
{

	LPSOCKETDATA	sock = &sockData.cSocket[iNum];


	//--- 使用フラグと初期化フラグをみる ----------------------------
	if(!sockData.bInit || !sock->bUse) return false; 

	//--- ロストフラグを立てる --------------------------------------
	sock->bLost = true;

	//--- 非ブロッキング解除 ----------------------------------------
	WSAAsyncSelect(sock->socket, sys.hWnd, 0, 0);

	// データ送受信シャットダウン
	//shutdown( Sock->socket , 2 ) ;

	//--- ソケットを閉じる ------------------------------------------
	closesocket(sock->socket);
	sock->socket = NULL;

	//--- 終了 ------------------------------------------------------
	return true;
}



/*------------------------------------------------------------------------------*
| <<< データの受信処理(内部) >>>
|	入力	iNum = 接続番号
|	戻り値	true or false
*------------------------------------------------------------------------------*/
bool	SockRecv(int iNum)
{

	LPSOCKETDATA	sock = &sockData.cSocket[iNum];


	if(!sockData.bInit) return false;

	bRecvDataError = false;					// 受信データフラグクリアー

#if 1
	//--- データ受信 ------------------------------------------------
	int	iRecvLen = recv(sock->socket, (char *)ucRecvBuf, BUF_SIZE, 0);
	if(iRecvLen != BUF_SIZE)
	{
		sys_error("SockRecv:送信失敗");
		bRecvDataError = true;
		return false;
	}
	//--- チェックサムを調べる --------------------------------------
	uint	uiAns = 0;
	uchar	*p    = &ucRecvBuf[4];
	for(int i = 4; i < BUF_SIZE; i++, p++) uiAns += *p;

	uint	*pSum = (uint*)ucRecvBuf;
	if(*pSum != uiAns)
	{
		sys_error("SockRecv:受信失敗。チェックサムが合いません。");
		bRecvDataError = true;
		return false;
	}
#else
	recv(sock->socket, (char *)ucRecvBuf, BUF_SIZE, 0);
#endif
	//--- 終了 ------------------------------------------------------
	bReceive = true;
	return true;
}



/*------------------------------------------------------------------------------*
| <<< データの送信処理 >>>
|	入力	iNum = 接続番号
|	戻り値	true or false
*------------------------------------------------------------------------------*/
static	int	iSendErrorCount;				// 送信の連続失敗回数
bool	SockSend(int iNum)
{

	if(!bSend) return false;				// net_send による要求があった場合のみ対応する

	bSend = false;

	LPSOCKETDATA	sock = &sockData.cSocket[iNum];

#if 1
	//--- チェックサムを取る ----------------------------------------
	uint	uiAns = 0;
	uchar	*p    = &ucSendBuf[4];
	for(int i = 4; i < BUF_SIZE; i++, p++) uiAns += *p;
	uint	*pSum = (uint*)ucSendBuf;
	*pSum = uiAns;

	//--- ここでデータ送信 ------------------------------------------
	int	iSendLen = send(sock->socket, (char *)ucSendBuf, BUF_SIZE, 0);
	if(iSendLen != BUF_SIZE)
	{
		iSendErrorCount++;
											// ここでは適当に 30 回連続で受信に失敗したらエラー
		if(iSendErrorCount > 30) net_connect_close(iNum);
		//sys_error("SockSend:送信失敗");
		return false;
	}
#else
	send(sock->socket, (char *)ucSendBuf, BUF_SIZE, 0);
#endif
	//--- 終了 ------------------------------------------------------
	iSendErrorCount = 0;
	return true;
}



/*------------------------------------------------------------------------------*
| <<< 新たに接続した通信回線を得る >>>
|	戻り値	-1  = 接続なし
|			-1 != 接続番号
*------------------------------------------------------------------------------*/
int	net_new_accept_get(void)
{
	int				i;
	LPSOCKETDATA	sock;

	//--- 初期化フラグを調べる --------------------------------------
	if(!sockData.bInit) return false;

	//--- 接続番号を探す --------------------------------------------
	sock = sockData.cSocket;
	for(i = 0; i < WINSOCK_MAX; i++, sock++)
	{
		if(sock->bUse && sock->bAccept && !sock->bAccepteConfir)
		{
			sock->bAccepteConfir = true;
			//sys_printf("net_new_accept_get:ネットワークID=%d が接続可能です。", i);
			return i;
		}
	}
	//--- 終了 ------------------------------------------------------
	return -1;
}



/*------------------------------------------------------------------------------*
| <<< 接続を切断された通信回線を得る >>>
|	戻り値	-1  = 切断なし
|			-1 != 通信番号
*------------------------------------------------------------------------------*/
int		net_lost_get(void)
{

	int				i;
	LPSOCKETDATA	sock;

	//--- 初期化状態を調べる ----------------------------------------
	if(!sockData.bInit) return -1;

	//--- 切断された番号を探す --------------------------------------
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
	//--- 終了 ------------------------------------------------------
	return -1;
}



/*------------------------------------------------------------------------------*
| <<< 接続先のＩＰを得る >>>
|	入力	iNum  = 接続番号
|	出力	ipOut = IPが出力される
|	戻り値	true or false
*------------------------------------------------------------------------------*/
bool	net_ip_get(IPDATA *ipOut, int iNum)
{
	
	LPSOCKETDATA	sock = &sockData.cSocket[iNum];

	if(!sock->bUse) return false;

	//--- ＩＰを代入する --------------------------------------------
	*ipOut = sock->ipAccept;

	//--- 終了 ------------------------------------------------------
	return true;
}



/*------------------------------------------------------------------------------*
| <<< winsock メイン >>>
|	戻り値	true : 受信データあり
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
	//--- クライアント --------------------------------------------------------
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
			#if	TIME_OUT != 0					// タイムアウトチェック
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
		case SOCK_WAIT:						// 一度でもデータの受信があると SOCK_MAIN へ
			iSockMode = SOCK_MAIN;
		case SOCK_MAIN:						// 接続している場合のみ処理をする
			if(bReceive)					// 受信がある場合の処理
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
				if(iRecvErrorCount > TIME_OUT)	// データの受信が１秒間出来なかった場合は切断となる
				{
					bTimeOut = true;
					net_connect_close(iSockNum);
				}
			}
			break;
		}
	}
//#else
	//--- サーバー ------------------------------------------------------------
	else
	{
		switch(iSockMode)
		{
		case SOCK_UNINIT:						// 接続してくるのを待つ状態にする
			if(net_listen_preparation()) iSockMode = SOCK_WAIT;
			break;
		case SOCK_WAIT:							// 新しい接続があったらそのネットワークハンドルを得る
			iSockNum = net_new_accept_get();
			if(iSockNum != -1)
			{
				IPDATA	ip;
				net_listen_stop();				// 接続の受付を終了する
				net_ip_get(&ip, iSockNum);		// 接続してきたマシンのＩＰアドレスを得る
				sys_printf("ID:%02d(%d,%d,%d,%d)が接続されました。", iSockNum, ip.d1, ip.d2, ip.d3, ip.d4);
				iSockMode = SOCK_MAIN;
			}
			break;
		case SOCK_MAIN:							// 接続中の処理
			if(bReceive)
			{									// データ受信
				bReceive = false;
				bRet     = true;
			}
			if(net_lost_get() == iSockNum)		// 切断された接続が今まで通信してた相手だった場合ループを抜ける
			{
				sys_printf("%d番が切断されました。", iSockNum);
				iSockMode = SOCK_UNINIT;
			}
			break;
		}
	}
//#endif
	return bRet;
}



/*------------------------------------------------------------------------------*
| <<< データ送信 >>>
|	入力	pBuf   = 転送データの格納アドレス
|			uiSize = 転送サイズ
|	戻り値	true or false
*------------------------------------------------------------------------------*/
bool	net_send(void *pBuf, uint uiSize)
{
	if(ucSendBuf == NULL) return false;

	if(uiSize >= BUF_SIZE - 4)
	{
		return sys_error("net_send:送信データが大きすぎます。%d/%d", uiSize, BUF_SIZE - 4);
	}
	memcpy(&ucSendBuf[4], pBuf, uiSize);
	bSend = true;
	return	SockSend(iSockNum);
}



/*------------------------------------------------------------------------------*
| <<< データ受信 >>>
|	入力	pBuf   = 受信データの格納アドレス
|			uiSize = 受信サイズ
|	戻り値	true or false
*------------------------------------------------------------------------------*/
bool	net_recv(void *pBuf, uint uiSize)
{
	if(ucRecvBuf == NULL) return false;
	if(bRecvDataError)    return false;
	if(!bReceive)		  return false;

	if(uiSize >= BUF_SIZE - 4)
	{
		return sys_error("net_send:受信データが大きすぎます。%d/%d", uiSize, BUF_SIZE - 4);
	}
											// エラーが起きていない時にコピーする
	memcpy(pBuf, &ucRecvBuf[4], uiSize);
	return true;
}



/*------------------------------------------------------------------------------*
| <<< 接続可能か調べる >>>
|	戻り値	true : 接続可能
*------------------------------------------------------------------------------*/
bool	net_accept_check(void)
{
	if(iSockMode == SOCK_UNINIT) return true;
	else						 return false;
}



/*------------------------------------------------------------------------------*
| <<< 通信可能か調べる >>>
|	戻り値	true : 接続可能
*------------------------------------------------------------------------------*/
bool	net_connect_check(void)
{
	if(iSockMode == SOCK_MAIN) return true;
	else					   return false;
}



/*------------------------------------------------------------------------------*
| <<< この PC の IP を得る >>>
|	戻り値	ip アドレス
*------------------------------------------------------------------------------*/
IPDATA	*net_myip_get(void)
{
	return &ipMyPc;
}



/*------------------------------------------------------------------------------*
| <<< サーバーとクライアントの切り替えスイッチ >>>
|	入力	bServer = true  : サーバーにする
|					  false : クライアントにする
*------------------------------------------------------------------------------*/
void	net_set(bool bSw)
{
	bServer = bSw;
}



/*------------------------------------------------------------------------------*
| <<< サーバーか調べる >>>
|	戻り値	true  : サーバー
|			false : クライアント
*------------------------------------------------------------------------------*/
bool	net_get(void)
{
	return bServer;
}

