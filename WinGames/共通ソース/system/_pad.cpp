//
//	DirectInput	関係のプログラム
//

#ifdef	__PAD

//--- 定数 ----------------------------------------------------------------------------------------
#define	KEY_REP_FIRST_TIME		20			// キーのリピート間隔	※最初の一回目
#define	KEY_REP_TIME			5			// キーのリピート間隔	※二回目以降
#define	DIDEVICE_BUFFERSIZE		20			// デバイスに設定するバッファ・サイズ

#define	ANALOG_STICK_THRESHOLD	500			// アナログキーの閾値


//--- ライブラリィ --------------------------------------------------------------------------------
#pragma comment(lib, "dinput8.lib")


//--- 変数 ----------------------------------------------------------------------------------------
PAD_WORK	pad;							// パッド処理ワーク


/*---- DirectInput 用 -------------------------------*/
static	uchar	ucKeyTbl[] =
{
	DIK_A,				// DirectInput のキー配列は
	DIK_B,				// DIK_A →	0x1E
	DIK_C,				// DIK_S →	0x1F などなっていて
	DIK_D,				// ABC 順には並んでいないので注意
	DIK_E,
	DIK_F,
	DIK_G,
	DIK_H,
	DIK_I,
	DIK_J,
	DIK_K,
	DIK_L,
	DIK_M,
	DIK_N,
	DIK_O,
	DIK_P,
	DIK_Q,
	DIK_R,
	DIK_S,
	DIK_T,
	DIK_U,
	DIK_V,
	DIK_W,
	DIK_X,
	DIK_Y,
	DIK_Z,
	DIK_0,
	DIK_1,
	DIK_2,
	DIK_3,
	DIK_4,
	DIK_5,
	DIK_6,
	DIK_7,
	DIK_8,
	DIK_9,
	DIK_NUMPAD0,			// テンキー
	DIK_NUMPAD1,
	DIK_NUMPAD2,
	DIK_NUMPAD3,
	DIK_NUMPAD4,
	DIK_NUMPAD5,
	DIK_NUMPAD6,
	DIK_NUMPAD7,
	DIK_NUMPAD8,
	DIK_NUMPAD9,
	DIK_DECIMAL,			// テンキーの.
	DIK_ADD,				// テンキーのプラス
	DIK_SUBTRACT,			// テンキーのマイナス
	DIK_DIVIDE,				// テンキーの割る
	DIK_MULTIPLY,			// テンキーの掛ける
	DIK_MINUS,				// フルキーのマイナス
	DIK_PREVTRACK,			// ^
	DIK_YEN,				// ￥マーク
	DIK_AT,					// @
	DIK_LBRACKET,			// [
	DIK_SEMICOLON,			// ;
	DIK_COLON,				// :
	DIK_RBRACKET,			// ]
	DIK_COMMA,				// ,
	DIK_PERIOD,				// .
	DIK_SLASH,				// /
	DIK_BACKSLASH,			// ￥キーボード下の
	DIK_SPACE,
	DIK_NUMPADENTER,		// テンキーのエンター
	DIK_RETURN,
	DIK_BACKSLASH,			// アンダーバー

	DIK_UP,
	DIK_DOWN,
	DIK_LEFT,
	DIK_RIGHT,
	DIK_LSHIFT,
	DIK_RSHIFT,
	DIK_INSERT,
	DIK_DELETE,
	DIK_PRIOR,
	DIK_NEXT,
	DIK_HOME,
	DIK_END,
	DIK_TAB,
	DIK_F1,					// ファンクションキー
	DIK_F2,
	DIK_F3,
	DIK_F4,
	DIK_F5,
	DIK_F6,
	DIK_F7,
	DIK_F8,
	DIK_F9,
	DIK_F10,
	DIK_F11,
	DIK_F12,
	DIK_ESCAPE,				// エスケープ
	DIK_LCONTROL,			// 左コントロール
	DIK_RCONTROL,			// 右コントロール
	DIK_LMENU,				// 左 Alt
	DIK_RMENU,				// 右 Alt
	DIK_BACK,				// バックスペース
};



/*------------------------------------------------------------------------------*
| <<< キーの情報クリアー >>>
*------------------------------------------------------------------------------*/
void	pad_clear(void)
{
	pad.bLeftClick   = false;
	pad.bRightClick  = false;
	pad.bCenterClick = false;
}



/*------------------------------------------------------------------------------*
| <<< ジョイスティックデバイスを列挙 >>>	※コールバック関数
*------------------------------------------------------------------------------*/
int		CALLBACK	joy_callback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
{

	HRESULT		hr;
	JOY_WORK	*j = &pad.joy[pad.iJoyCount];

	hr = pad.dev->CreateDevice(pdidInstance->guidInstance, &j->dev,	NULL);
											// ジョイスティックの能力を調べる
	pad.caps.dwSize	= sizeof(DIDEVCAPS);
	hr = j->dev->GetCapabilities(&pad.caps);
	if(SUCCEEDED(hr))
	{
		pad.iJoyCount++;
		if(pad.iJoyCount > JOYPAD_MAX){ return DIENUM_STOP;}
		return DIENUM_CONTINUE;
	}
	return DIENUM_STOP;
}



/*------------------------------------------------------------------------------*
| <<< ジョイスティックの軸を列挙する関数 >>>	※コールバック関数
*------------------------------------------------------------------------------*/
static	int		CALLBACK	EnumAxesCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
	HRESULT					hr;
	DIPROPRANGE				diprg;
	LPDIRECTINPUTDEVICE8	j =	(LPDIRECTINPUTDEVICE8)pvRef;

											// 軸の値の範囲を設定（-1000～1000）
	ZeroMemory(&diprg, sizeof(diprg));
	diprg.diph.dwSize		= sizeof(diprg); 
	diprg.diph.dwHeaderSize	= sizeof(diprg.diph); 
	diprg.diph.dwObj		= lpddoi->dwType;
	diprg.diph.dwHow		= DIPH_BYID;
	diprg.lMin				= -1000;
	diprg.lMax				= +1000;
	hr = j->SetProperty(DIPROP_RANGE, &diprg.diph);
	
	if(FAILED(hr)){ return DIENUM_STOP;}
	return DIENUM_CONTINUE;
}



/*------------------------------------------------------------------------------*
| <<< ジョイスティックチェック >>>	※途中差しが利くように一定周期で呼び出す
*------------------------------------------------------------------------------*/
void	pad_check(void)
{

	if(pad.dev == NULL){ return;}

	HRESULT		hr;
	DIPROPDWORD	diprg;
	int			i;

	//--- ここからジョイスティック --------------------------------
	pad.iJoyCount =	0;						// ジョイパットの数
											// ジョイスティックを探す（本数を数える）
	hr = pad.dev->EnumDevices(DI8DEVCLASS_GAMECTRL,	joy_callback, NULL, DIEDFL_ATTACHEDONLY);

	if(FAILED(hr))							// ジョイスティックが見付からない
	{
		for(i =	0; i < JOYPAD_MAX; i++){ SAFE_RELEASE(pad.joy[i].dev);}
		return;
		//return false;
	}										// データフォーマットを設定
	for(i =	0; i < pad.iJoyCount; i++)
	{
		JOY_WORK	*j = &pad.joy[i];
											// ジョイスティック用のデータ・フォーマットを設定
		hr = j->dev->SetDataFormat(&c_dfDIJoystick2);
		if(FAILED(hr)) return;				// データフォーマットの設定に失敗
											// モードを設定（バックグラウンド＆非排他モード）
		hr = j->dev->SetCooperativeLevel(sys.hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

		if(FAILED(hr)) return;				// モードの設定に失敗

		DIDEVCAPS				dcaps	= {sizeof(DIDEVCAPS)};
		DIDEVICEINSTANCE		ddi		= {sizeof(DIDEVICEINSTANCE)};
		DIDEVICEOBJECTINSTANCE	ddioi	= {sizeof(DIDEVICEOBJECTINSTANCE)};
		j->dev->GetCapabilities(&dcaps);
		j->dev->GetDeviceInfo(&ddi);
		j->dev->GetObjectInfo(&ddioi, 0, 0);
		j->bForceFeedback |= ((dcaps.dwFlags & DIDC_FORCEFEEDBACK	)  != 0);
		j->bPolling		  |= ((dcaps.dwFlags & DIDC_POLLEDDATAFORMAT)  != 0);
		j->bPolling		  |= ((dcaps.dwFlags & DIDC_POLLEDDEVICE	)  != 0);
		j->strName		   = (char *)malloc(strlen(ddi.tszInstanceName)	+ 1);
		j->strProduct	   = (char *)malloc(strlen(ddi.tszProductName )	+ 1);
		strcpy(j->strName,	  ddi.tszInstanceName);
		strcpy(j->strProduct, ddi.tszProductName);
												// コールバック関数を使って各軸のモードを設定
		hr = j->dev->EnumObjects(EnumAxesCallback, j->dev, DIDFT_AXIS);
		if(FAILED(hr)) return;
												// 軸の値の範囲を設定
		ZeroMemory(&diprg, sizeof(diprg));
		diprg.diph.dwSize		= sizeof(diprg); 
		diprg.diph.dwHeaderSize	= sizeof(diprg.diph); 
		diprg.diph.dwObj		= 0; 
		diprg.diph.dwHow		= DIPH_DEVICE; 
		diprg.dwData			= DIPROPAXISMODE_ABS;
		j->dev->SetProperty(DIPROP_AXISMODE,   &diprg.diph);
#if 0
		diprg.dwData			= DIDEVICE_BUFFERSIZE;
		j->dev->SetProperty(DIPROP_BUFFERSIZE, &diprg.diph);
#endif
		//if(sys.bActive) j->dev->Acquire();	
		// 入力制御開始
		j->dev->Acquire();						// 入力制御開始
	}
	//return true;
	return;
}

	
	
/*------------------------------------------------------------------------------*
| <<< パット処理初期化 >>>
*------------------------------------------------------------------------------*/
bool	pad_init(void)
{

	HRESULT		hr;
	DIPROPDWORD	diprg;

	hr = DirectInput8Create(sys.hInst, DIRECTINPUT_VERSION,	IID_IDirectInput8, (void**)&pad.dev, NULL);	
											// DirectInputの作成に失敗
	if(FAILED(hr)){ return sys_error("DirectInput 初期化失敗");}

	//--- ここからマウス ------------------------------------------
	hr = pad.dev->CreateDevice(GUID_SysMouse, &pad.lpMouse,	NULL);
											// デバイスの作成に失敗
	if(FAILED(hr)){ return sys_error("マウスデバイス取得失敗");}
											// データフォーマットを設定
	hr = pad.lpMouse->SetDataFormat(&c_dfDIMouse2);
	if(FAILED(hr)){ return sys_error("マウスデバイス形式の設定に失敗");}
											// モードを設定（フォアグラウンド＆非排他モード）
	//hr = pad.lpMouse->SetCooperativeLevel(sys.hWnd,	DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	hr = pad.lpMouse->SetCooperativeLevel(sys.hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
	
	if(FAILED(hr)) return sys_error("マウスデバイス、フォアグラウンド＆非排他モードの設定に失敗");
											// 軸モードを設定（相対値モードに設定）
	diprg.diph.dwSize		= sizeof(diprg); 
	diprg.diph.dwHeaderSize	= sizeof(diprg.diph); 
	diprg.diph.dwObj		= 0;
	diprg.diph.dwHow		= DIPH_DEVICE;
	diprg.dwData			= DIPROPAXISMODE_ABS;//DIPROPAXISMODE_REL;
	hr = pad.lpMouse->SetProperty(DIPROP_AXISMODE, &diprg.diph);
	if(FAILED(hr)) return sys_error("マウスデバイス、軸モードの設定に失敗");
#if 0
											// バッファリング・データを取得するため、バッファ・サイズを設定
	diprg.dwData = DIDEVICE_BUFFERSIZE;
	hr = pad.lpMouse->SetProperty(DIPROP_BUFFERSIZE, &diprg.diph);
	if(FAILED(hr)) return sys_error("マウスデバイス、バッファ・サイズの設定に失敗");
#endif
	pad.lpMouse->Acquire();					// 入力制御開始	

	//--- ここからキーボード --------------------------------------
											// キーボード用にデバイスオブジェクトを作成
	hr = pad.dev->CreateDevice(GUID_SysKeyboard, &pad.lpKey, NULL);
											// デバイスの作成に失敗
	if(FAILED(hr)){ return sys_error("キーボードデバイス取得失敗");}
											// データフォーマットを設定
											// キーボード用のデータ・フォーマットを設定
	hr = pad.lpKey->SetDataFormat(&c_dfDIKeyboard);
											// データフォーマットの設定に失敗
	if(FAILED(hr)){ return sys_error("キーボードデータフォーマット設定失敗");}
											// モードを設定（フォアグラウンド＆非排他モード）
	//hr = pad.lpKey->SetCooperativeLevel(sys.hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	hr = pad.lpKey->SetCooperativeLevel(sys.hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
											// モードの設定に失敗
	if(FAILED(hr)){ return sys_error("キーボードモード設定失敗");}
	pad.lpKey->Acquire();					// 入力制御開始

	//--- ここからジョイスティック --------------------------------
	pad_check();

	return true;
}



/*------------------------------------------------------------------------------*
| <<< DirectInput の解放処理 >>>
*------------------------------------------------------------------------------*/
bool	pad_end(void)
{
	if(pad.lpMouse){ pad.lpMouse->Unacquire();}
	if(pad.lpKey)  { pad.lpKey->Unacquire();}
	for(int	i =	0; i < pad.iJoyCount; i++)
	{
		if(pad.joy[i].dev)
		{
			pad.joy[i].dev->Unacquire();
			SAFE_RELEASE(pad.joy[i].dev);
			if(pad.joy[i].strName	 !=	NULL){ free(pad.joy[i].strName);}
			if(pad.joy[i].strProduct !=	NULL){ free(pad.joy[i].strProduct);}
			pad.joy[i].strName	  =	NULL;
			pad.joy[i].strProduct =	NULL;
		}
	}
	SAFE_RELEASE(pad.lpMouse);				// マウスを解放
	SAFE_RELEASE(pad.lpKey);				// キーボードを解放
	SAFE_RELEASE(pad.dev);
	return true;
}



/*------------------------------------------------------------------------------*
| <<< ダブルクリックマクロ >>>
*------------------------------------------------------------------------------*/
inline	void	DoubleClickCheck(float *f, bool	*b,	uint cl)
{
	*b = false;
	if(*f >	0)
	{
		*f -= 1.f;
		if(cl){ *b = true, *f = 0;}
	}
	else
	{
		if(cl){ *f = 30;}
	}
}



/*------------------------------------------------------------------------------*
| <<< パット処理メイン >>>
*------------------------------------------------------------------------------*/
void	pad_main(void)
{

	uchar	k[256];
	HRESULT	hr;
											// マウスの移動量を出す
	pad.sMoveX = pad.sX	- pad.sXBak;
	pad.sMoveY = pad.sY	- pad.sYBak;
	pad.sMoveZ = pad.sZ	- pad.sZBak;
	pad.sXBak  = pad.sX;
	pad.sYBak  = pad.sY;
	pad.sZBak  = pad.sZ;

	if(!pad.dev) return;

	//--- マウス処理 ----------------------------------------------
	if(pad.lpMouse)
	{
		DIMOUSESTATE2	dims;				// デバイスの直接データを取得する
		hr = pad.lpMouse->GetDeviceState(sizeof(DIMOUSESTATE2),	&dims);
		if(SUCCEEDED(hr))
		{
			static	LONG	lZBak;				// ホイールば差分を取り移動量を足していく
			if(pad_mosue_render_range_check())	// レンダリング範囲無いのみ有効とする
			{
				pad.sZ += (short)((lZBak - dims.lZ) / 120);
			}
			lZBak = dims.lZ;
											// リピート情報(いろいろ苦心したもの)
			char	*cRepTime = &pad.cLeftRepTime;
			bool	*bRep     = &pad.bLeftRep;
			bool	*bPush	  = &pad.bLeftPush;
			bool	*bClick   = &pad.bLeftClick;
			bool	*bPush2   = &pad.bRightPush;
			int		iSize	  = &pad.bRightPush - &pad.bLeftPush;
			for(int i = 0; i < iSize * 3; i += iSize)
			{
				if(bPush[i])
				{
					cRepTime[i]++;
					if(cRepTime[i] == 0){ bRep[i] = true, cRepTime[i] = -KEY_REP_TIME;}
					else				{ bRep[i] = false;}
					if(bClick[i])
					{
						bRep[    i] = true;
						cRepTime[i] = -KEY_REP_FIRST_TIME;
					}
				}
				else{ cRepTime[i] = 0, bRep[i] = false;}
			}
		}
	}

	pad.bLeftClick      = false;
	pad.bRightClick     = false;
	pad.bCenterClick    = false;
	pad.bLeftRelease    = false;
	pad.bRightRelease   = false;
	pad.bCenterRelease  = false;
	pad.bLeftDblClick	= false;
	pad.bRightDblClick	= false;
	pad.bCenterDblClick	= false;

	//--- キーボード処理 ------------------------------------------
	if(pad.lpKey)
	{
		hr = pad.lpKey->GetDeviceState(256,	k);
		if(SUCCEEDED(hr))
		{
			for(int	i =	0; i < KEY_MAX; i++)
			{
				pad.bKeyBak[i] = pad.bKeyPush[i];

				if(k[ucKeyTbl[i]] & 0x80){ pad.bKeyPush[i] = true;}
				else					 { pad.bKeyPush[i] = false;}
											// 押した瞬間
				pad.bKeyClick[i] = (pad.bKeyBak[i] ^ pad.bKeyPush[i]) * pad.bKeyPush[i];
											// 放された瞬間
				uchar	*p = &pad.bKeyRelease[i];
				*p = (pad.bKeyPush[i] ^ pad.bKeyBak[i]) * pad.bKeyBak[ i];

				if(pad.bKeyPush[i])			// リピート情報(いろいろ苦心したもの)
				{
					pad.cKeyRepTime[i]++;
					if(pad.cKeyRepTime[i] == 0){ pad.bKeyRep[i] = true, pad.cKeyRepTime[i] = -KEY_REP_TIME;}
					else					   { pad.bKeyRep[i] = false;}
					
					if(pad.bKeyClick[i])
					{
						pad.bKeyRep[    i] = true;
						pad.cKeyRepTime[i] = -KEY_REP_FIRST_TIME;
					}
				}
				else pad.cKeyRepTime[i] = 0, pad.bKeyRep[i] = false;
			}
		}		
	}
	//--- ジョイパット処理 ----------------------------------------
	for(int	i =	0; i < pad.iJoyCount; i++)
	{
		JOY_WORK	*j = &pad.joy[i];
		if(j->dev != NULL)
		{
			if(j->bPolling){ j->dev->Poll();}
			hr = j->dev->GetDeviceState(sizeof(DIJOYSTATE2), &j->di);
			if(SUCCEEDED(hr))
			{
				j->sLx = (short)j->di.lX;	// アナログ情報
				j->sLy = (short)j->di.lY;
				j->sLz = (short)j->di.lZ;
				j->sRx = (short)j->di.lRx;
				j->sRy = (short)j->di.lRy;
				j->sRz = (short)j->di.lRz;


				int	k;
				for(k =	JOY_UP;	k <	JOY_MAX; k++){ j->bKeyBak[k] = j->bKeyPush[k];}
											// ここでアナログの情報を上下左右のボタンに反映している
				int	r = j->di.rgdwPOV[0];										// POV データをカーソルに割り当て
				j->bKeyPush[JOY_UP	 ] = (j->di.lY <= -ANALOG_STICK_THRESHOLD) | (BOOL)((r >=     0 && r <=  4500) || r >= 31500);
				j->bKeyPush[JOY_RIGHT] = (j->di.lX >=  ANALOG_STICK_THRESHOLD) | (BOOL)( r >=  4500 && r <= 13500);
				j->bKeyPush[JOY_DOWN ] = (j->di.lY >=  ANALOG_STICK_THRESHOLD) | (BOOL)( r >= 13500 && r <= 22500);
				j->bKeyPush[JOY_LEFT ] = (j->di.lX <= -ANALOG_STICK_THRESHOLD) | (BOOL)( r >= 22500 && r <= 31500);

											// 押した情報
				for(k =	JOY_01;	k <	JOY_16;	 k++){ j->bKeyPush[   k] = j->di.rgbButtons[k - JOY_01] != 0;}
											// 押した瞬間情報
				for(k =	JOY_UP;	k <	JOY_MAX; k++){ j->bKeyClick[  k] = (j->bKeyBak[ k] ^ (uchar)j->bKeyPush[k]) * j->bKeyPush[k];}
											// 離された瞬間
				for(k =	JOY_UP;	k <	JOY_MAX; k++){ j->bKeyRelease[k] = (j->bKeyPush[k] ^ (uchar)j->bKeyBak[ k]) * j->bKeyBak[ k];}
											// キーリピート
				for(k =	JOY_UP;	k <	JOY_MAX; k++)
				{
					if(j->bKeyPush[k])
					{
						j->cKeyRepTime[k]++;
						if(j->cKeyRepTime[k] == 0){ j->bKeyRep[k] = true, j->cKeyRepTime[k] = -KEY_REP_TIME;}
						else					  { j->bKeyRep[k] = false;}
						if(j->bKeyClick[k])
						{
							j->bKeyRep[    k] = true;
							j->cKeyRepTime[k] = -KEY_REP_FIRST_TIME;
						}
					}
					else{ j->cKeyRepTime[k] = 0, j->bKeyRep[k] = false;}
				}
			}
		}
	}
}



/*------------------------------------------------------------------------------*
| <<< 何らかのキーが押されているか調べる >>>
*------------------------------------------------------------------------------*/
bool	pad_any_key_check(void)
{

	for(int	i =	0; i < KEY_MAX;	i++)
	{
		#ifdef	_DEBUG
			if(pad.bKeyClick[KEY_TAB]
			|| pad.bKeyClick[KEY_1]
			|| pad.bKeyClick[KEY_2]){ continue;}
		#endif

		if(pad.bKeyClick[KEY_LALT]
		|| pad.bKeyClick[KEY_LALT]){ continue;}

		if(pad.bKeyClick[i]){ return true;}
	}

	if(pad.bLeftClick || pad.bCenterClick || pad.bRightClick){ return true;}

	return false;
}



/*--------------------------------------------------------*
| <<< マウスの先から光線を出してその３Ｄ座標を返す >>>
|	入力	mtxWorld = ワールド座標（回転、移動がかかっているやつはレイ自体を移動させる）
|	出力	*vecPos	 = レイの位置（mtxWorld	の xyz 座標）
|			*vecDir	 = レイの方向（マウスの方向）
*--------------------------------------------------------*/
void	pad_mouse_ray_get(VEC3 *vecPos, VEC3 *vecDir, MTX *mtxWorld)
{
	MTX		m =	*mtxWorld *	d3.mtxView;

	D3DXMatrixInverse(&m, NULL,	&m);

	//--- ２Ｄ位置から３Ｄ位置と方向を求める ---------------
	float	fWidth  = __SCR_W * d3.fZoom;
	float	fHeight = __SCR_H * d3.fZoom;

	#ifdef	__2D_RESIZE
		vecPos->x =	 (((2.0f * (pad.sXOri - d3.vecOfs.x)) / (float)fWidth ) - 1) / d3.mtxProjection._11;
		vecPos->y =	-(((2.0f * (pad.sYOri - d3.vecOfs.y)) / (float)fHeight) - 1) / d3.mtxProjection._22;
		vecPos->z =	 1.0f;
	#else
		vecPos->x =	 (((2.0f * pad.sX) / (float)d3.sRenderW) - 1) / d3.mtxProjection._11;
		vecPos->y =	-(((2.0f * pad.sY) / (float)d3.sRenderH) - 1) / d3.mtxProjection._22;
		vecPos->z =	 1.0f;
	#endif

	vecDir->x =	vecPos->x *	m._11 +	vecPos->y *	m._21 +	vecPos->z *	m._31;
	vecDir->y =	vecPos->x *	m._12 +	vecPos->y *	m._22 +	vecPos->z *	m._32;
	vecDir->z =	vecPos->x *	m._13 +	vecPos->y *	m._23 +	vecPos->z *	m._33;
	vecPos->x =	m._41;
	vecPos->y =	m._42;
	vecPos->z =	m._43;
}



/*--------------------------------------------------------*
| <<< マウスの３Ｄ座標を返す >>>
|	入力	mtxWorld = ワールド座標（回転、移動がかかっているやつはレイ自体を移動させる）
|	出力	*vecPos	 = レイの位置（マウスの３Ｄ位置）
|			*vecDir	 = レイの方向（マウスの方向）
*--------------------------------------------------------*/
void	pad_mouse_pos_get(VEC3 *vecPos, VEC3 *vecDir, MTX *mtxWorld)
{
	MTX		m =	*mtxWorld *	d3.mtxView;

	D3DXMatrixInverse(&m, NULL,	&m);

	//--- ２Ｄ位置から３Ｄ位置と方向を求める ---------------
	vecPos->x =	 (((2.0f * pad.sX) / (float)d3.sRenderW) - 1) /	d3.mtxProjection._11;
	vecPos->y =	-(((2.0f * pad.sY) / (float)d3.sRenderH) - 1) /	d3.mtxProjection._22;
	vecPos->z =	 1.0f;

	vecDir->x =	vecPos->x *	m._11 +	vecPos->y *	m._21 +	vecPos->z *	m._31;
	vecDir->y =	vecPos->x *	m._12 +	vecPos->y *	m._22 +	vecPos->z *	m._32;
	vecDir->z =	vecPos->x *	m._13 +	vecPos->y *	m._23 +	vecPos->z *	m._33;
}



/*------------------------------------------------------------------------------*
| <<< 球とマウスの交差判定 >>>
|	入力	fRadius	= 球の半径
|			mtx		= ワールド行列
|	戻り値	true  :	交差している
|			false :	交差していない
*------------------------------------------------------------------------------*/
bool	pad_mouse_sphere_intersection_check(float fRadius, MTX *mtx)
{
	VEC3	vecPos,	vecDir;

	pad_mouse_ray_get(&vecPos, &vecDir,	mtx);
	if(D3DXSphereBoundProbe(&VEC3(0, 0, 0), fRadius, &vecPos, &vecDir)){ return true;}
	return false;
}



/*------------------------------------------------------------------------------*
| <<< メッシュとマウスの交差判定をして、交点を返す >>>
|	入力	iNum = メッシュの読み込み番号
|			iMat = 調べるマテリアル番号(-1なら全てを一度に調べる)
|			mtx	 = ワールド行列
|	出力	pOut = 交差している座標
|	戻り値	true  :	交差している
|			false :	交差していない
*------------------------------------------------------------------------------*/
//static	D3_VTX_DEFAULT	vtxTri[3];			// 三角形の座標
bool	pad_mouse_mesh_intersection_check(VEC3 *pOut, int iNum, int iMat, MTX *mtx)
{

	if(d3.lpMesh[iNum] == NULL){ return false;}

	//--- マウスの光線とマップの交差判定 ----------------------------
	BOOL				bHit;
	LPD3DXBUFFER		pBuffer	= NULL;
	DWORD				dwMax;
	VEC3				vecPos,	vecDir;

	pad_mouse_ray_get(&vecPos, &vecDir,	mtx);
	if(iMat	!= -1){ D3DXIntersectSubset(d3.lpMesh[iNum], iMat, &vecPos, &vecDir, &bHit, NULL, NULL, NULL, NULL, &pBuffer, &dwMax);}
	else		  { D3DXIntersect(	   d3.lpMesh[iNum],		   &vecPos, &vecDir, &bHit, NULL, NULL, NULL, NULL, &pBuffer, &dwMax);}
	if(!bHit){ return false;}

	//--- 交差していればどの面が交差しているか調べる ----------------
	D3DXINTERSECTINFO	*p = (D3DXINTERSECTINFO	*)pBuffer->GetBufferPointer();

	//---- 距離の近い物からソートする -------------------------------
	float	fDistMax = 10000;
	for(int	i =	0; i < (int)dwMax; i++)
	{
		DWORD	dwFace = p[i].FaceIndex;
		float	fBary1 = p[i].U;
		float	fBary2 = p[i].V;
		float	fDist  = p[i].Dist;

		if(fDistMax	< fDist) continue;
		//--- ヒットしている三角形を得る ----------------------------
		LPDIRECT3DVERTEXBUFFER9	pVB;
		LPDIRECT3DINDEXBUFFER9	pIB;
		WORD					*pIndices;
		D3_VTX_DEFAULT			*pVertices;

		d3.lpMesh[iNum]->GetVertexBuffer(&pVB);
		d3.lpMesh[iNum]->GetIndexBuffer( &pIB);
		pIB->Lock(0, 0,	(void**)&pIndices,	0);
		pVB->Lock(0, 0,	(void**)&pVertices,	0);

		WORD	*iThisTri;
		iThisTri = &pIndices[dwFace	* 3];

		vtxTri[0] =	pVertices[iThisTri[0]];			// ここが三角形の面情報となる
		vtxTri[1] =	pVertices[iThisTri[1]];			// 後に三角形を描画するので座標を保存
		vtxTri[2] =	pVertices[iThisTri[2]];

		pVB->Unlock();
		pIB->Unlock();
		pVB->Release();
		pIB->Release();

		//--- 面の表裏判定(裏面も判定する場合は以下をコメントのこと)
		D3DXPLANE	plane;
		D3DXPlaneFromPoints(&plane,	&vtxTri[0].p, &vtxTri[1].p,	&vtxTri[2].p);
		float	a =	D3DXPlaneDotNormal(&plane, &vecDir);
		if(a >=	0){ continue;}			// プラスの場合は裏面

		//--- 面のレイの交点を得る ----------------------------------
										// pOut に交点の座標
		D3DXVec3BaryCentric(pOut, &vtxTri[0].p,	&vtxTri[1].p, &vtxTri[2].p,	fBary1,	fBary2);
		fDistMax = fDist;
	}
	SAFE_RELEASE(pBuffer);

	return true;
}



/*------------------------------------------------------------------------------*
| <<< メッシュとマウスの交差判定をした後、交差している三角形の面情報を返す >>>
|	戻り値	三角形の面情報
*------------------------------------------------------------------------------*/
D3_VTX_DEFAULT	*pad_mouse_mesh_intersection_triangle(void)
{
	return d3_mesh_intersection_face();
	//return vtxTri;
}



/*------------------------------------------------------------------------------*
| <<< マウスの絶対値よりマウス位置がレンダリング範囲内か調べる >>>
|	戻り値	true : レンダリング範囲内にマウスがある
*------------------------------------------------------------------------------*/
bool	pad_mosue_render_range_check(void)
{
	
	POINT	p;
	RECT	r;
	int		x0, y0, x1, y1;

	GetCursorPos(&p);
	GetWindowRect(sys.hWnd, &r);

	if(d3.rectRender == NULL || !d3.bWindow)
	{
		x0 = r.left;
		y0 = r.top;
		x1 = r.right;
		y1 = r.bottom;
	}
	else
	{
		int	x  = r.left + GetSystemMetrics(SM_CXEDGE);
		int	y  = r.top  + GetSystemMetrics(SM_CYEDGE) + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYMENU);
		x0 = x + d3.rectRender->left;
		y0 = y + d3.rectRender->top;
		x1 = x + d3.rectRender->right;
		y1 = y + d3.rectRender->bottom;
	}

	if(p.x >= x0 && p.x <= x1 && p.y >= y0 && p.y <= y1){ return true;}
	else												{ return false;}
}



/*------------------------------------------------------------------------------*
| <<< レンダリング範囲以外を含めたマウス位置を得る >>>
|	※通常の pad.sX、pad.sY はコントロール上では動作しない。
|	※コントロールからファイルをドラッグ＆ドロップするときに使用する
|	出力	pX, pY = マウスの座標
*------------------------------------------------------------------------------*/
void	pad_mosue_pos_get(short *pX, short *pY)
{
	
	POINT	p;
	RECT	r;

	GetCursorPos(&p);
	GetWindowRect(sys.hWnd, &r);

	//--- フルスクリーン時 ----------------------------------------------------
	if(d3.rectRender == NULL || !d3.bWindow)
	{
		*pX = pad.sX;
		*pY = pad.sY;
		return;
	}

	//--- ウィンドウ時 --------------------------------------------------------
	int	x  = r.left + GetSystemMetrics(SM_CXEDGE) * 2;
	int	y  = r.top  + GetSystemMetrics(SM_CYEDGE) * 2 + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYMENU);
	*pX = (ushort)(p.x - x);
	*pY = (ushort)(p.y - y);
}


#else

	PAD_WORK	pad;
	bool	pad_init(void){return true;}
	void	pad_main(void)
	{										// マウスの移動量を出す
		pad.sMoveX = pad.sX	- pad.sXBak;
		pad.sMoveY = pad.sY	- pad.sYBak;
		pad.sMoveZ = pad.sZ	- pad.sZBak;
		pad.sXBak  = pad.sX;
		pad.sYBak  = pad.sY;
		pad.sZBak  = pad.sZ;
											// こちらはクリック処理
		pad.cl = false;
		pad.cr = false;
		pad.cc = false;
		pad.rl = false;
		pad.rr = false;
		pad.rc = false;
		pad.bDblClickLeft	= false;
		pad.bDblClickRight	= false;
		pad.bDblClickCenter	= false;
	}
	bool	pad_end(void){return true;}
	void	pad_pause(bool bSw){}
	void	pad_check(void){}

#endif	// __PAD

