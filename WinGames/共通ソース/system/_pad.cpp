//
//	DirectInput	�֌W�̃v���O����
//

#ifdef	__PAD

//--- �萔 ----------------------------------------------------------------------------------------
#define	KEY_REP_FIRST_TIME		20			// �L�[�̃��s�[�g�Ԋu	���ŏ��̈���
#define	KEY_REP_TIME			5			// �L�[�̃��s�[�g�Ԋu	�����ڈȍ~
#define	DIDEVICE_BUFFERSIZE		20			// �f�o�C�X�ɐݒ肷��o�b�t�@�E�T�C�Y

#define	ANALOG_STICK_THRESHOLD	500			// �A�i���O�L�[��臒l


//--- ���C�u�����B --------------------------------------------------------------------------------
#pragma comment(lib, "dinput8.lib")


//--- �ϐ� ----------------------------------------------------------------------------------------
PAD_WORK	pad;							// �p�b�h�������[�N


/*---- DirectInput �p -------------------------------*/
static	uchar	ucKeyTbl[] =
{
	DIK_A,				// DirectInput �̃L�[�z���
	DIK_B,				// DIK_A ��	0x1E
	DIK_C,				// DIK_S ��	0x1F �ȂǂȂ��Ă���
	DIK_D,				// ABC ���ɂ͕���ł��Ȃ��̂Œ���
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
	DIK_NUMPAD0,			// �e���L�[
	DIK_NUMPAD1,
	DIK_NUMPAD2,
	DIK_NUMPAD3,
	DIK_NUMPAD4,
	DIK_NUMPAD5,
	DIK_NUMPAD6,
	DIK_NUMPAD7,
	DIK_NUMPAD8,
	DIK_NUMPAD9,
	DIK_DECIMAL,			// �e���L�[��.
	DIK_ADD,				// �e���L�[�̃v���X
	DIK_SUBTRACT,			// �e���L�[�̃}�C�i�X
	DIK_DIVIDE,				// �e���L�[�̊���
	DIK_MULTIPLY,			// �e���L�[�̊|����
	DIK_MINUS,				// �t���L�[�̃}�C�i�X
	DIK_PREVTRACK,			// ^
	DIK_YEN,				// ���}�[�N
	DIK_AT,					// @
	DIK_LBRACKET,			// [
	DIK_SEMICOLON,			// ;
	DIK_COLON,				// :
	DIK_RBRACKET,			// ]
	DIK_COMMA,				// ,
	DIK_PERIOD,				// .
	DIK_SLASH,				// /
	DIK_BACKSLASH,			// ���L�[�{�[�h����
	DIK_SPACE,
	DIK_NUMPADENTER,		// �e���L�[�̃G���^�[
	DIK_RETURN,
	DIK_BACKSLASH,			// �A���_�[�o�[

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
	DIK_F1,					// �t�@���N�V�����L�[
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
	DIK_ESCAPE,				// �G�X�P�[�v
	DIK_LCONTROL,			// ���R���g���[��
	DIK_RCONTROL,			// �E�R���g���[��
	DIK_LMENU,				// �� Alt
	DIK_RMENU,				// �E Alt
	DIK_BACK,				// �o�b�N�X�y�[�X
};



/*------------------------------------------------------------------------------*
| <<< �L�[�̏��N���A�[ >>>
*------------------------------------------------------------------------------*/
void	pad_clear(void)
{
	pad.bLeftClick   = false;
	pad.bRightClick  = false;
	pad.bCenterClick = false;
}



/*------------------------------------------------------------------------------*
| <<< �W���C�X�e�B�b�N�f�o�C�X��� >>>	���R�[���o�b�N�֐�
*------------------------------------------------------------------------------*/
int		CALLBACK	joy_callback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
{

	HRESULT		hr;
	JOY_WORK	*j = &pad.joy[pad.iJoyCount];

	hr = pad.dev->CreateDevice(pdidInstance->guidInstance, &j->dev,	NULL);
											// �W���C�X�e�B�b�N�̔\�͂𒲂ׂ�
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
| <<< �W���C�X�e�B�b�N�̎���񋓂���֐� >>>	���R�[���o�b�N�֐�
*------------------------------------------------------------------------------*/
static	int		CALLBACK	EnumAxesCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
	HRESULT					hr;
	DIPROPRANGE				diprg;
	LPDIRECTINPUTDEVICE8	j =	(LPDIRECTINPUTDEVICE8)pvRef;

											// ���̒l�͈̔͂�ݒ�i-1000�`1000�j
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
| <<< �W���C�X�e�B�b�N�`�F�b�N >>>	���r�������������悤�Ɉ������ŌĂяo��
*------------------------------------------------------------------------------*/
void	pad_check(void)
{

	if(pad.dev == NULL){ return;}

	HRESULT		hr;
	DIPROPDWORD	diprg;
	int			i;

	//--- ��������W���C�X�e�B�b�N --------------------------------
	pad.iJoyCount =	0;						// �W���C�p�b�g�̐�
											// �W���C�X�e�B�b�N��T���i�{���𐔂���j
	hr = pad.dev->EnumDevices(DI8DEVCLASS_GAMECTRL,	joy_callback, NULL, DIEDFL_ATTACHEDONLY);

	if(FAILED(hr))							// �W���C�X�e�B�b�N�����t����Ȃ�
	{
		for(i =	0; i < JOYPAD_MAX; i++){ SAFE_RELEASE(pad.joy[i].dev);}
		return;
		//return false;
	}										// �f�[�^�t�H�[�}�b�g��ݒ�
	for(i =	0; i < pad.iJoyCount; i++)
	{
		JOY_WORK	*j = &pad.joy[i];
											// �W���C�X�e�B�b�N�p�̃f�[�^�E�t�H�[�}�b�g��ݒ�
		hr = j->dev->SetDataFormat(&c_dfDIJoystick2);
		if(FAILED(hr)) return;				// �f�[�^�t�H�[�}�b�g�̐ݒ�Ɏ��s
											// ���[�h��ݒ�i�o�b�N�O���E���h����r�����[�h�j
		hr = j->dev->SetCooperativeLevel(sys.hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

		if(FAILED(hr)) return;				// ���[�h�̐ݒ�Ɏ��s

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
												// �R�[���o�b�N�֐����g���Ċe���̃��[�h��ݒ�
		hr = j->dev->EnumObjects(EnumAxesCallback, j->dev, DIDFT_AXIS);
		if(FAILED(hr)) return;
												// ���̒l�͈̔͂�ݒ�
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
		// ���͐���J�n
		j->dev->Acquire();						// ���͐���J�n
	}
	//return true;
	return;
}

	
	
/*------------------------------------------------------------------------------*
| <<< �p�b�g���������� >>>
*------------------------------------------------------------------------------*/
bool	pad_init(void)
{

	HRESULT		hr;
	DIPROPDWORD	diprg;

	hr = DirectInput8Create(sys.hInst, DIRECTINPUT_VERSION,	IID_IDirectInput8, (void**)&pad.dev, NULL);	
											// DirectInput�̍쐬�Ɏ��s
	if(FAILED(hr)){ return sys_error("DirectInput ���������s");}

	//--- ��������}�E�X ------------------------------------------
	hr = pad.dev->CreateDevice(GUID_SysMouse, &pad.lpMouse,	NULL);
											// �f�o�C�X�̍쐬�Ɏ��s
	if(FAILED(hr)){ return sys_error("�}�E�X�f�o�C�X�擾���s");}
											// �f�[�^�t�H�[�}�b�g��ݒ�
	hr = pad.lpMouse->SetDataFormat(&c_dfDIMouse2);
	if(FAILED(hr)){ return sys_error("�}�E�X�f�o�C�X�`���̐ݒ�Ɏ��s");}
											// ���[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
	//hr = pad.lpMouse->SetCooperativeLevel(sys.hWnd,	DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	hr = pad.lpMouse->SetCooperativeLevel(sys.hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
	
	if(FAILED(hr)) return sys_error("�}�E�X�f�o�C�X�A�t�H�A�O���E���h����r�����[�h�̐ݒ�Ɏ��s");
											// �����[�h��ݒ�i���Βl���[�h�ɐݒ�j
	diprg.diph.dwSize		= sizeof(diprg); 
	diprg.diph.dwHeaderSize	= sizeof(diprg.diph); 
	diprg.diph.dwObj		= 0;
	diprg.diph.dwHow		= DIPH_DEVICE;
	diprg.dwData			= DIPROPAXISMODE_ABS;//DIPROPAXISMODE_REL;
	hr = pad.lpMouse->SetProperty(DIPROP_AXISMODE, &diprg.diph);
	if(FAILED(hr)) return sys_error("�}�E�X�f�o�C�X�A�����[�h�̐ݒ�Ɏ��s");
#if 0
											// �o�b�t�@�����O�E�f�[�^���擾���邽�߁A�o�b�t�@�E�T�C�Y��ݒ�
	diprg.dwData = DIDEVICE_BUFFERSIZE;
	hr = pad.lpMouse->SetProperty(DIPROP_BUFFERSIZE, &diprg.diph);
	if(FAILED(hr)) return sys_error("�}�E�X�f�o�C�X�A�o�b�t�@�E�T�C�Y�̐ݒ�Ɏ��s");
#endif
	pad.lpMouse->Acquire();					// ���͐���J�n	

	//--- ��������L�[�{�[�h --------------------------------------
											// �L�[�{�[�h�p�Ƀf�o�C�X�I�u�W�F�N�g���쐬
	hr = pad.dev->CreateDevice(GUID_SysKeyboard, &pad.lpKey, NULL);
											// �f�o�C�X�̍쐬�Ɏ��s
	if(FAILED(hr)){ return sys_error("�L�[�{�[�h�f�o�C�X�擾���s");}
											// �f�[�^�t�H�[�}�b�g��ݒ�
											// �L�[�{�[�h�p�̃f�[�^�E�t�H�[�}�b�g��ݒ�
	hr = pad.lpKey->SetDataFormat(&c_dfDIKeyboard);
											// �f�[�^�t�H�[�}�b�g�̐ݒ�Ɏ��s
	if(FAILED(hr)){ return sys_error("�L�[�{�[�h�f�[�^�t�H�[�}�b�g�ݒ莸�s");}
											// ���[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
	//hr = pad.lpKey->SetCooperativeLevel(sys.hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	hr = pad.lpKey->SetCooperativeLevel(sys.hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
											// ���[�h�̐ݒ�Ɏ��s
	if(FAILED(hr)){ return sys_error("�L�[�{�[�h���[�h�ݒ莸�s");}
	pad.lpKey->Acquire();					// ���͐���J�n

	//--- ��������W���C�X�e�B�b�N --------------------------------
	pad_check();

	return true;
}



/*------------------------------------------------------------------------------*
| <<< DirectInput �̉������ >>>
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
	SAFE_RELEASE(pad.lpMouse);				// �}�E�X�����
	SAFE_RELEASE(pad.lpKey);				// �L�[�{�[�h�����
	SAFE_RELEASE(pad.dev);
	return true;
}



/*------------------------------------------------------------------------------*
| <<< �_�u���N���b�N�}�N�� >>>
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
| <<< �p�b�g�������C�� >>>
*------------------------------------------------------------------------------*/
void	pad_main(void)
{

	uchar	k[256];
	HRESULT	hr;
											// �}�E�X�̈ړ��ʂ��o��
	pad.sMoveX = pad.sX	- pad.sXBak;
	pad.sMoveY = pad.sY	- pad.sYBak;
	pad.sMoveZ = pad.sZ	- pad.sZBak;
	pad.sXBak  = pad.sX;
	pad.sYBak  = pad.sY;
	pad.sZBak  = pad.sZ;

	if(!pad.dev) return;

	//--- �}�E�X���� ----------------------------------------------
	if(pad.lpMouse)
	{
		DIMOUSESTATE2	dims;				// �f�o�C�X�̒��ڃf�[�^���擾����
		hr = pad.lpMouse->GetDeviceState(sizeof(DIMOUSESTATE2),	&dims);
		if(SUCCEEDED(hr))
		{
			static	LONG	lZBak;				// �z�C�[���΍��������ړ��ʂ𑫂��Ă���
			if(pad_mosue_render_range_check())	// �����_�����O�͈͖����̂ݗL���Ƃ���
			{
				pad.sZ += (short)((lZBak - dims.lZ) / 120);
			}
			lZBak = dims.lZ;
											// ���s�[�g���(���낢���S��������)
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

	//--- �L�[�{�[�h���� ------------------------------------------
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
											// �������u��
				pad.bKeyClick[i] = (pad.bKeyBak[i] ^ pad.bKeyPush[i]) * pad.bKeyPush[i];
											// �����ꂽ�u��
				uchar	*p = &pad.bKeyRelease[i];
				*p = (pad.bKeyPush[i] ^ pad.bKeyBak[i]) * pad.bKeyBak[ i];

				if(pad.bKeyPush[i])			// ���s�[�g���(���낢���S��������)
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
	//--- �W���C�p�b�g���� ----------------------------------------
	for(int	i =	0; i < pad.iJoyCount; i++)
	{
		JOY_WORK	*j = &pad.joy[i];
		if(j->dev != NULL)
		{
			if(j->bPolling){ j->dev->Poll();}
			hr = j->dev->GetDeviceState(sizeof(DIJOYSTATE2), &j->di);
			if(SUCCEEDED(hr))
			{
				j->sLx = (short)j->di.lX;	// �A�i���O���
				j->sLy = (short)j->di.lY;
				j->sLz = (short)j->di.lZ;
				j->sRx = (short)j->di.lRx;
				j->sRy = (short)j->di.lRy;
				j->sRz = (short)j->di.lRz;


				int	k;
				for(k =	JOY_UP;	k <	JOY_MAX; k++){ j->bKeyBak[k] = j->bKeyPush[k];}
											// �����ŃA�i���O�̏����㉺���E�̃{�^���ɔ��f���Ă���
				int	r = j->di.rgdwPOV[0];										// POV �f�[�^���J�[�\���Ɋ��蓖��
				j->bKeyPush[JOY_UP	 ] = (j->di.lY <= -ANALOG_STICK_THRESHOLD) | (BOOL)((r >=     0 && r <=  4500) || r >= 31500);
				j->bKeyPush[JOY_RIGHT] = (j->di.lX >=  ANALOG_STICK_THRESHOLD) | (BOOL)( r >=  4500 && r <= 13500);
				j->bKeyPush[JOY_DOWN ] = (j->di.lY >=  ANALOG_STICK_THRESHOLD) | (BOOL)( r >= 13500 && r <= 22500);
				j->bKeyPush[JOY_LEFT ] = (j->di.lX <= -ANALOG_STICK_THRESHOLD) | (BOOL)( r >= 22500 && r <= 31500);

											// ���������
				for(k =	JOY_01;	k <	JOY_16;	 k++){ j->bKeyPush[   k] = j->di.rgbButtons[k - JOY_01] != 0;}
											// �������u�ԏ��
				for(k =	JOY_UP;	k <	JOY_MAX; k++){ j->bKeyClick[  k] = (j->bKeyBak[ k] ^ (uchar)j->bKeyPush[k]) * j->bKeyPush[k];}
											// �����ꂽ�u��
				for(k =	JOY_UP;	k <	JOY_MAX; k++){ j->bKeyRelease[k] = (j->bKeyPush[k] ^ (uchar)j->bKeyBak[ k]) * j->bKeyBak[ k];}
											// �L�[���s�[�g
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
| <<< ���炩�̃L�[��������Ă��邩���ׂ� >>>
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
| <<< �}�E�X�̐悩��������o���Ă��̂R�c���W��Ԃ� >>>
|	����	mtxWorld = ���[���h���W�i��]�A�ړ����������Ă����̓��C���̂��ړ�������j
|	�o��	*vecPos	 = ���C�̈ʒu�imtxWorld	�� xyz ���W�j
|			*vecDir	 = ���C�̕����i�}�E�X�̕����j
*--------------------------------------------------------*/
void	pad_mouse_ray_get(VEC3 *vecPos, VEC3 *vecDir, MTX *mtxWorld)
{
	MTX		m =	*mtxWorld *	d3.mtxView;

	D3DXMatrixInverse(&m, NULL,	&m);

	//--- �Q�c�ʒu����R�c�ʒu�ƕ��������߂� ---------------
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
| <<< �}�E�X�̂R�c���W��Ԃ� >>>
|	����	mtxWorld = ���[���h���W�i��]�A�ړ����������Ă����̓��C���̂��ړ�������j
|	�o��	*vecPos	 = ���C�̈ʒu�i�}�E�X�̂R�c�ʒu�j
|			*vecDir	 = ���C�̕����i�}�E�X�̕����j
*--------------------------------------------------------*/
void	pad_mouse_pos_get(VEC3 *vecPos, VEC3 *vecDir, MTX *mtxWorld)
{
	MTX		m =	*mtxWorld *	d3.mtxView;

	D3DXMatrixInverse(&m, NULL,	&m);

	//--- �Q�c�ʒu����R�c�ʒu�ƕ��������߂� ---------------
	vecPos->x =	 (((2.0f * pad.sX) / (float)d3.sRenderW) - 1) /	d3.mtxProjection._11;
	vecPos->y =	-(((2.0f * pad.sY) / (float)d3.sRenderH) - 1) /	d3.mtxProjection._22;
	vecPos->z =	 1.0f;

	vecDir->x =	vecPos->x *	m._11 +	vecPos->y *	m._21 +	vecPos->z *	m._31;
	vecDir->y =	vecPos->x *	m._12 +	vecPos->y *	m._22 +	vecPos->z *	m._32;
	vecDir->z =	vecPos->x *	m._13 +	vecPos->y *	m._23 +	vecPos->z *	m._33;
}



/*------------------------------------------------------------------------------*
| <<< ���ƃ}�E�X�̌������� >>>
|	����	fRadius	= ���̔��a
|			mtx		= ���[���h�s��
|	�߂�l	true  :	�������Ă���
|			false :	�������Ă��Ȃ�
*------------------------------------------------------------------------------*/
bool	pad_mouse_sphere_intersection_check(float fRadius, MTX *mtx)
{
	VEC3	vecPos,	vecDir;

	pad_mouse_ray_get(&vecPos, &vecDir,	mtx);
	if(D3DXSphereBoundProbe(&VEC3(0, 0, 0), fRadius, &vecPos, &vecDir)){ return true;}
	return false;
}



/*------------------------------------------------------------------------------*
| <<< ���b�V���ƃ}�E�X�̌�����������āA��_��Ԃ� >>>
|	����	iNum = ���b�V���̓ǂݍ��ݔԍ�
|			iMat = ���ׂ�}�e���A���ԍ�(-1�Ȃ�S�Ă���x�ɒ��ׂ�)
|			mtx	 = ���[���h�s��
|	�o��	pOut = �������Ă�����W
|	�߂�l	true  :	�������Ă���
|			false :	�������Ă��Ȃ�
*------------------------------------------------------------------------------*/
//static	D3_VTX_DEFAULT	vtxTri[3];			// �O�p�`�̍��W
bool	pad_mouse_mesh_intersection_check(VEC3 *pOut, int iNum, int iMat, MTX *mtx)
{

	if(d3.lpMesh[iNum] == NULL){ return false;}

	//--- �}�E�X�̌����ƃ}�b�v�̌������� ----------------------------
	BOOL				bHit;
	LPD3DXBUFFER		pBuffer	= NULL;
	DWORD				dwMax;
	VEC3				vecPos,	vecDir;

	pad_mouse_ray_get(&vecPos, &vecDir,	mtx);
	if(iMat	!= -1){ D3DXIntersectSubset(d3.lpMesh[iNum], iMat, &vecPos, &vecDir, &bHit, NULL, NULL, NULL, NULL, &pBuffer, &dwMax);}
	else		  { D3DXIntersect(	   d3.lpMesh[iNum],		   &vecPos, &vecDir, &bHit, NULL, NULL, NULL, NULL, &pBuffer, &dwMax);}
	if(!bHit){ return false;}

	//--- �������Ă���΂ǂ̖ʂ��������Ă��邩���ׂ� ----------------
	D3DXINTERSECTINFO	*p = (D3DXINTERSECTINFO	*)pBuffer->GetBufferPointer();

	//---- �����̋߂�������\�[�g���� -------------------------------
	float	fDistMax = 10000;
	for(int	i =	0; i < (int)dwMax; i++)
	{
		DWORD	dwFace = p[i].FaceIndex;
		float	fBary1 = p[i].U;
		float	fBary2 = p[i].V;
		float	fDist  = p[i].Dist;

		if(fDistMax	< fDist) continue;
		//--- �q�b�g���Ă���O�p�`�𓾂� ----------------------------
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

		vtxTri[0] =	pVertices[iThisTri[0]];			// �������O�p�`�̖ʏ��ƂȂ�
		vtxTri[1] =	pVertices[iThisTri[1]];			// ��ɎO�p�`��`�悷��̂ō��W��ۑ�
		vtxTri[2] =	pVertices[iThisTri[2]];

		pVB->Unlock();
		pIB->Unlock();
		pVB->Release();
		pIB->Release();

		//--- �ʂ̕\������(���ʂ����肷��ꍇ�͈ȉ����R�����g�̂���)
		D3DXPLANE	plane;
		D3DXPlaneFromPoints(&plane,	&vtxTri[0].p, &vtxTri[1].p,	&vtxTri[2].p);
		float	a =	D3DXPlaneDotNormal(&plane, &vecDir);
		if(a >=	0){ continue;}			// �v���X�̏ꍇ�͗���

		//--- �ʂ̃��C�̌�_�𓾂� ----------------------------------
										// pOut �Ɍ�_�̍��W
		D3DXVec3BaryCentric(pOut, &vtxTri[0].p,	&vtxTri[1].p, &vtxTri[2].p,	fBary1,	fBary2);
		fDistMax = fDist;
	}
	SAFE_RELEASE(pBuffer);

	return true;
}



/*------------------------------------------------------------------------------*
| <<< ���b�V���ƃ}�E�X�̌��������������A�������Ă���O�p�`�̖ʏ���Ԃ� >>>
|	�߂�l	�O�p�`�̖ʏ��
*------------------------------------------------------------------------------*/
D3_VTX_DEFAULT	*pad_mouse_mesh_intersection_triangle(void)
{
	return d3_mesh_intersection_face();
	//return vtxTri;
}



/*------------------------------------------------------------------------------*
| <<< �}�E�X�̐�Βl���}�E�X�ʒu�������_�����O�͈͓������ׂ� >>>
|	�߂�l	true : �����_�����O�͈͓��Ƀ}�E�X������
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
| <<< �����_�����O�͈͈ȊO���܂߂��}�E�X�ʒu�𓾂� >>>
|	���ʏ�� pad.sX�Apad.sY �̓R���g���[����ł͓��삵�Ȃ��B
|	���R���g���[������t�@�C�����h���b�O���h���b�v����Ƃ��Ɏg�p����
|	�o��	pX, pY = �}�E�X�̍��W
*------------------------------------------------------------------------------*/
void	pad_mosue_pos_get(short *pX, short *pY)
{
	
	POINT	p;
	RECT	r;

	GetCursorPos(&p);
	GetWindowRect(sys.hWnd, &r);

	//--- �t���X�N���[���� ----------------------------------------------------
	if(d3.rectRender == NULL || !d3.bWindow)
	{
		*pX = pad.sX;
		*pY = pad.sY;
		return;
	}

	//--- �E�B���h�E�� --------------------------------------------------------
	int	x  = r.left + GetSystemMetrics(SM_CXEDGE) * 2;
	int	y  = r.top  + GetSystemMetrics(SM_CYEDGE) * 2 + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYMENU);
	*pX = (ushort)(p.x - x);
	*pY = (ushort)(p.y - y);
}


#else

	PAD_WORK	pad;
	bool	pad_init(void){return true;}
	void	pad_main(void)
	{										// �}�E�X�̈ړ��ʂ��o��
		pad.sMoveX = pad.sX	- pad.sXBak;
		pad.sMoveY = pad.sY	- pad.sYBak;
		pad.sMoveZ = pad.sZ	- pad.sZBak;
		pad.sXBak  = pad.sX;
		pad.sYBak  = pad.sY;
		pad.sZBak  = pad.sZ;
											// ������̓N���b�N����
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

