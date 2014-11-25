//
// DirectGraphics �֌W�̃v���O����
//

//--- �n�[�h�Ɋւ���ݒ�

#define	D3_SOFTWARE_ONLY			0		// 1 : �\�t�g�E�F�A�݂̂ŃW�I���g���v�Z
#define	D3_HZ						60		// �t���X�N���[�����̃f�t�H���g�f�B�X�v���C���g��
#define	D3_REZO_H					480		// �t���X�N���[�����̃f�t�H���g�̏c�̍Œ�𑜓x
#define	D3_FMT_DISPLAY_MAX			2		// fmtDisplay �̐�
#define	D3_FMT_STENCIL_MAX			6		// �X�e���V���o�b�t�@�̓o�^��
											// �o�b�N�o�b�t�@�̃f�t�H���g�t�H�[�}�b�g
#define	D3_FORMAT					D3DFMT_X8R8G8B8
											// ����
#define	D3_AMBIENT					D3_COL(255, 51, 51, 51)

//-- �e�N�X�`���[�֌W
#define	D3_TEX_MAX					512		// ���[�U�[���g�p�\�ȃe�N�X�`���[�̍ő吔
#define	D3_TEX_MAX2					1024	// ������̓��b�V�����܂߂��e�X�N�`���[��

//--- �e��G�t�F�N�g
#define	D3_FUR_COUNT				20		// �t�@�[�̕`�搔

//--- �t�H���g
#define	D3_FONT_MAX					8		// �����̎��
#define	D3_TEXT_MAX					16		// ������̓o�^��

//--- ���b�V��
#define	D3_MESH_MAX					512		// �w�t�@�C���̓ǂݍ��ݍő吔

//--- �I�t�X�N���[��
#define	D3_OFFSCREEN_BAK_MAX		4		// �I�t�X�N���[���̓���q��

//--- �r���[
#define	D3_VIEW_NEAR				__NEAR	// ��O�̃N���b�s���O�̈�
#define	D3_VIEW_FAR					__FAR	// ���̃N���b�s���O�̈�

//--- ���C��
#ifndef	D3_3DLINE_MAX
	#define	D3_3DLINE_MAX			4096	// �R�c���C���o�^�p
#endif

//--- �u������ ------------------------------------------------------------------------------------
#define	D3_LPVTX_BUF				LPDIRECT3DVERTEXBUFFER9
#define	D3_LPINDEX_BUF				LPDIRECT3DINDEXBUFFER9
#define	D3_LPTEX					LPDIRECT3DTEXTURE9

//--- �e�X�N�`���[�p ------------------------------------------------------------------------------
	enum
	{
		TEX_RESERVE    = D3_TEX_MAX - 40 - 1,	// �\�񂳂ꂽ�e�N�X�`���[

		#ifdef	__PRINTF
			TEX_MOJI_,							// �����p
		#endif
		#ifdef	__AFONT
			TEX_AFONT,							// �A���`�t�H���g
			TEX_AFONT_END,
		#endif

		TEX_WHITE,								// ��(�e�N�X�`���[���Ȃ��Ƃ��̎g�p����)
	};

//--- �V�F�[�_�[�^�C�v ----------------------------------------------------------------------------
	enum
	{										// ���̒l�ɂ��V�F�[�f�B���O�����܂�
		D3_SHADER_TYPE_OFF				= -1,	// �I�t
		D3_SHADER_TYPE_NORMAL,					// 00(�ʏ�)
		D3_SHADER_TYPE_MAX				= 50,
	};

//--- �t�B���^�[ ----------------------------------------------------------------------------------
	enum
	{
		D3_FILTER_LINEAR = D3DTEXF_LINEAR,
		D3_FILTER_POINT  = D3DTEXF_POINT,
		D3_LINEAR		 = D3DTEXF_LINEAR,
		D3_POINT		 = D3DTEXF_POINT,
	};

//--- �����\���p ----------------------------------------------------------------------------------
	struct	D3_TEXT
	{
		uchar		ucFont;					// �t�H���g�ԍ�
		short		sX, sY;
		D3DCOLOR	dwCol;
		bool		bShadow;				// true : �e������
		char		str[512];
	};

//--- ���p�@---------------------------------------------------------------------------------------
	struct	D3_SPHERE
	{
		MTX		mtxWorld;					// ���̍s��
		COL4	col;						// �F
	};

//--- �q�b�g�`�F�b�N�p ----------------------------------------------------------------------------
	struct	D3_HIT
	{
		VEC3		vecLocal[  9];			// �{�b�N�X�`�F�b�N�p�̃��[�J�����W
		VEC3		vecWorld[  9];			// �{�b�N�X�`�F�b�N�p�̃��[���h���W
		D3DXPLANE	planeWorld[6];			// �����蔻��p���ʎ�
		VEC3		vecMin,	vecMax;			// ���f���̑傫��
		float		fRadius;				// ���a
		VEC3		vecCenter;				// ���S���W
	};

//--- �e�N�X�`���[�����b�V���ǂݍ��ݏ��	-------------------------------------------------------
	struct	D3_INFO						// �e�N�X�`���[�̏����
	{
		//--- �e�N�X�`���[�p ------------------------------------------
		float		fU,		   fV;				// �I���W�i���̉摜���t���ɕ\������ۂ�	U1 V1 �̒l
		float		fOriWidth, fOriHeight;		// �I���W�i���̉摜�T�C�Y
		float		fWidth,	   fHeight;			// �摜�̃T�C�Y
		ushort		usFormat;					// �摜�̃t�H�[�}�b�g
		bool		bOffscreen;					// true	: �I�t�X�N���[���ʂł���
		//--- ���b�V���p ----------------------------------------------
		bool		bSkin;						// true	: �X�L�����b�V��
		uchar		ucSkinAnimationSet;			// �A�j���[�V�����g���b�N��
		uchar		ucSubset;					// �T�u�Z�b�g��
		ushort		usVertex;					// ���_���A	   �œK���O�A�œK����
		ushort		usFaces;					// �|���S�����A�œK���O�A�œK����

		//--- ���� ----------------------------------------------------
		bool		bResource;					// true : ���\�[�X
		bool		bLoad;						// true : �ǂݍ��ݍς݂Ȃ�
		bool		bError;						// true : �ǂݍ��݃G���[
		ushort		usUseCount;					// �����̒l���O�ȏ�Ȃ�΁A���̉ӏ�(���b�V���Ȃ�)�œǂݍ���ł���
		bool		bRepair;					// true	: ���X�g���ɕ��A������
		char		strFullPath[MAX_PATH + 1];	// �t���p�X
		FILETIME	ftLastWrite;				// �^�C�v�X�^���v�p(�^�C���X�^���v���ύX�ɂȂ�Ɠǂݍ��݂���悤)
	};

//--- UV ------------------------------------------------------------------------------------------
	struct	D3_UV
	{
		float	fU0, fV0, fU1, fV1;
	};

//--- �F ------------------------------------------------------------------------------------------
	#define	D3_COL(__a,	__r, __g, __b)	D3DCOLOR_ARGB((int)(__a), (int)(__r), (int)(__g), (int)(__b))
	#define	D3_COL_NORMAL				D3DCOLOR_ARGB(255, 255,	255, 255)
	#define	D3_COL_ALL(	   __a)			D3DCOLOR_ARGB(__a, __a,	__a, __a)
	#define	D3_COL_WHITE(  __a)			D3DCOLOR_ARGB(__a, 255,	255, 255)
	#define	D3_COL_BLACK(  __a)			D3DCOLOR_ARGB(__a,	 0,	  0,   0)
	#define	D3_COL_RED(	   __a)			D3DCOLOR_ARGB(__a, 255,	  0,   0)
	#define	D3_COL_GREEN(  __a)			D3DCOLOR_ARGB(255,	 0,	__a,   0)
	#define	D3_COL_BLUE(   __a)			D3DCOLOR_ARGB(__a,	 0,	  0, 255)
	#define	D3_COL_YELLOW( __a)			D3DCOLOR_ARGB(__a, 255,	255,   0)

//--- �Œ�p�C�v���C���ݒ� ------------------------------------------------------------------------
	//--- �����C�e�B���O�A�e�N�X�`���L��-----------------------------
										// ���_�V�F�[�_�[ FVF
	#define	D3_FVF_DEFAULT	(D3DFVF_XYZ	| D3DFVF_NORMAL	| D3DFVF_TEX1)
	struct	D3_VTX_DEFAULT
	{
		VEC3	p;							// ���W
		VEC3	n;							// �@��
		VEC2	uv;							// �e�N�X�`���[���W
	};
	struct	D3_VTX_DEFAULT_
	{
		float	x, y, z, nx, ny, nz;		// ���W�A�@��
		float	u, v;						// �e�N�X�`���[���W
	};

	//--- �Q�c�p ----------------------------------------------------
	#define	D3_FVF_TL		(D3DFVF_XYZRHW | D3DFVF_DIFFUSE)	// ���W�ϊ��ς݁A���C�e�B���O�ς�
	struct	D3_VTX_TL
	{
		VEC3	p;
		float	rhw;
		DWORD	c;
	};
	struct	D3_VTX_TL_
	{
		float	x, y, z;
		float	rhw;
		DWORD	c;
	};

	//--- ���C���A�|�C���g�p ----------------------------------------
	#define	D3_FVF_L		(D3DFVF_XYZ	| D3DFVF_DIFFUSE)
	struct	D3_VTX_L
	{
		VEC3	p;
		DWORD	c;
	};
	struct	D3_VTX_L_
	{
		float	x, y, z;
		DWORD	c;
	};

	//--- ���W�ϊ��ς݁A���C�e�B���O�ς݁A�e�N�X�`���L�� --------------
	#define	D3_FVF_TLX		(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
	struct	D3_VTX_TLX
	{
		VEC3	p;
 		float	rhw;
		DWORD	c;
		VEC2	uv;
	};
	struct	D3_VTX_TLX_
	{
		float	x, y, z;
 		float	rhw;
		DWORD	c;
		float	u, v;
	};

	//--- ���C�e�B���O�ς݁A�e�N�X�`���L��-----------------------------
	#define	D3_FVF_LX		(D3DFVF_XYZ	| D3DFVF_DIFFUSE | D3DFVF_TEX1)
	struct	D3_VTX_LX
	{
		VEC3	p;
		DWORD	c;
		VEC2	uv;
	};
	struct	D3_VTX_LX_
	{
		float	x, y, z;
		DWORD	c;
		float	u,v;
	};

//--- �a�l�o�p ------------------------------------------------------------------------------------
	struct	BMP_HEAD
	{										// 14 bytes
		ushort	usType;
		uint	uiSize;
		ushort	usReserved1;
		ushort	usReserved2;
		uint	uiOffBits;
	};

	struct	BMP_INFO
	{										// 40 bytes
		uint	uiSize;
		union
		{
			uint	uiXsize;
			uint	uiXSize;
		};
		union
		{
			uint	uiYsize;
			uint	uiYSize;
		};
		ushort	usPlanes;
		ushort	usBitCount;
		uint	uiComp;
		uint	uiSizeImage;
		uint	uiXPelsPerMeter;
		uint	uiYPelsPerMeter;
		uint	uiClrUsed;
		uint	uiClrImportant;
	};

	struct	BMP_COL
	{
		uchar	ucB;
		uchar	ucG;
		uchar	ucR;
		uchar	ucRev;
	};

	struct	BMP_WORK
	{
		char		strName[256];		// �t�@�C����
		BMP_INFO	info;				// �a�l�o���
		BMP_HEAD	head;				// �a�l�o�w�b�_
		BMP_COL		col[256];			// �a�l�o�J���[
		union
		{
			uchar	*ucBuf;				// �摜�f�[�^�i�[�ʒu
			uchar	*pucBuf;
		};
		ushort		*usBuf16;			// �摜�f�[�^�i�[�ʒu
		uchar		*ucBuf24;			// �摜�f�[�^�i�[�ʒu
		uchar		*ucBuf32;			// �摜�f�[�^�i�[�ʒu
		ushort		usXbak;				// ���̕␳�O�̒l
		uchar		ucDat;				// ���̗]��ɓ����l
	};									// �a�l�o�؂�o���p�̃��[�N

//--- DirectX �Ǘ��p ------------------------------------------------------------------------------
	struct	D3_WORK
	{
		LPDIRECT3D9				d;						// Direct3D	�̌�
		LPDIRECT3DDEVICE9		dev;					// �f�o�C�X

		D3DDISPLAYMODE			deskTopDisplayMode;		// ���݂̃f�B�X�v���C���[�h
		D3DDISPLAYMODE			displayMode;			// �f�B�X�v���C���[�h
		D3DPRESENT_PARAMETERS	para;					// �v���[���g�p�����[�^�[(Direct3D �̐ݒ�)
		D3DCAPS9				caps;					// �A�_�v�^�[�̏��
		//DWORD					dwSwapEffectMode;		// Present �̃��[�h�BD3DSWAPEFFECT �񋓌^������܂�
		//												// D3DSWAPEFFECT_FLIP	 �E�B���h�E���[�h��(��ʂ̃L���v�`���[���ł���)	��DEF
		//												// D3DSWAPEFFECT_COPY	 �t���X�N���[����
		//												// D3DSWAPEFFECT_DISCARD �c�[����(d3.rectRender ���g�p�\)

		bool					bWindow;				// �E�B���h�E���[�h�Ȃ�	true
		bool					bWindowBak;				// __EXIT_MENU �L�����Ɏg�p����܂�
		bool					bRender;				// �����_�����O�v���t���O
		bool					bMovieEncode; 			// ���[�r�[���P�t���[���G���R�[�h����
		bool					bPresent;				// �ʏ�� true �����A�Z�p���[�^�[�ړ����Ȃǂ� false �Ƃ���Ɖ�ʂ�������Ȃ�
		bool					bPaintPresent;			// WM_PAINT ���ɉ摜�̓]�����s��(�����ltrue);
		bool					bReRender;				// �ă����_�����O�v���t���O(�e�N�X�`���[�ǂݍ��ݎ��Ȃǂɔ���)
		bool					bDeviceChange;			// true : �f�o�C�X�ύX��
		uchar					ucPresentCount;			// ��ʂ̐؂�ւ��J�E���g
		uchar					ucAdapter;				// �g�p����A�_�v�^�[�ԍ�
		uchar					ucVertexProcessing;		// ���_�̌v�Z���@
		D3DDEVTYPE				deviceType;				// �f�o�C�X�^�C�v HAL or REF or	SW
		D3DFORMAT				fmtDisplay;				// �f�B�X�v���C�t�H�[�}�b�g
		D3DFORMAT				fmtBackBuffer;			// �o�b�N�o�b�t�@�t�H�[�}�b�g
		D3DFORMAT				fmtStencil;				// �X�e���V���t�H�[�}�b�g
		D3DFORMAT				fmtTexture;				// �ǂݍ��݃e�N�X�`���[�̃t�H�[�}�b�g
		RECT					*rectRender;			// �����_�����O���̓]����͈͂������ϐ��̃|�C���^(NULL�Ȃ�S���)
		short					sRenderW, sRenderH;		// �����_�����O�T�C�Y
		short					sFullScreenRenderW;		// ������̓t���X�N���[�����̃����_�����O�T�C�Y
		short					sFullScreenRenderH;

		float					fZoom;					// ��ʂ��Y�[���p�̒l(�c���䂪�����ɂȂ�悤�Ɋg�厞�ɂ͓��l���g��)
		VEC2					vecOfs;					// ��ʒ����ւ̃I�t�Z�b�g�l

		//--- �r���[�֌W ----------------------------------------------
		D3DVIEWPORT9			viewport;
		MTX						mtxViewProjectionTranspose;	// mtxViewProjection�̓]�u�s��
		MTX						mtxViewProjection;			// d3.mtxView * d3.mtxProjection �Ƃ�������
		MTX						mtxProjection;				// �ˉe�s��
		MTX						mtxProjectionInverse;		// �ˉe�t�s��
		MTX						mtxView, mtxViewBak;		// �r���[�s��
		MTX						mtxViewInverse;				// �r���[�t�s��(�r���{�[�h�Ŏg�p)
		MTX						mtxIdentity;				// �P�ʍs��
		MTX						mtxBillboard;				// �r���{�[�h�p(41,42,43 �̓��e��ۏႵ�Ȃ�)
		VEC3					vecEye;						// �������W
		VEC3					vecCamera;					// �J�������W
		VEC3					vecUp;						// �J�����̌���

		//--- �œK���t���O --------------------------------------------
		DWORD					dwMeshOptimized;

		//--- �t�H�O�֌W ----------------------------------------------
		DWORD					dwClearCol;					// �t�H�O�̐F

		//--- �}�e���A�� ------------------------------------------------
		D3DMATERIAL9			matNormal;					// (�V)

		//--- ���C�g ----------------------------------------------------
		D3DLIGHT9				light;
		MTX						mtxLightViewProjection;		// ���C�g�̍s��
		VEC4					vecAmbient;					// ����

		//--- ���b�V���֌W --------------------------------------------
		LPD3DXMESH				lpMeshSphere;						// �q�b�g�`�F�b�N�̋��̃��b�V��
		LPD3DXMESH				lpMesh[			D3_MESH_MAX];		// ���b�V��
		short					sMeshCopyType[  D3_MESH_MAX][128];	// ���b�V���̃R�s�[�̃^�C�v(D3_SHADER_TYPE �`������)
		LPD3DXMESH				lpMeshCopy[		D3_MESH_MAX];		// ���b�V���̃R�s�[�p(�����Ƀo���v�ȂǂŎg�p
		MATERIAL				*matMesh[		D3_MESH_MAX];		// ���b�V���̃}�e���A��
		short					sMeshTexNum[    D3_MESH_MAX][128];	// ���b�V���̎g�p����e�N�X�`���[�ԍ�(�ЂƂ̃��b�V�����g����̃e�N�X�`���[�͂P�Q�W��)
		short					sMeshTexCopyNum[D3_MESH_MAX][128];	// ���b�V���̎g�p����o���v�e�N�X�`���[�ԍ�
		int						iPolyCount;							// �|���S������
		int						iPolyCountMax;						// �|���S���ō���
		D3_INFO					meshInfo[		D3_MESH_MAX];		// ���b�V���̏��
		
		//--- �q�b�g�`�F�b�N ----------------------------------------
		D3_HIT					hitWork[D3_MESH_MAX];				// �q�b�g�`�F�b�N�p���[�N
		VEC3					vecFrustum[	 8];					// corners of the view frustum
		D3DXPLANE				planeFrustum[6];					// planes of the view frustum

		//--- �e�N�X�`���[�֌W --------------------------------------
		int							iTexCurrent;					// �J�����g�e�N�X�`���[�ԍ�
		D3_INFO						texInfo[D3_TEX_MAX2];			// �e�N�X�`���[�̏��
		LPDIRECT3DTEXTURE9			tex[	D3_TEX_MAX2];			// �e�N�X�`���[
		LPDIRECT3DVOLUMETEXTURE9	vol[	D3_TEX_MAX2];			// �{�����[���e�N�X�`���[

		//--- �t�H���g�[�֌W ----------------------------------------
		LPD3DXFONT				font[	 D3_FONT_MAX];				// �t�H���g
		D3DXFONT_DESC			fontInfo[D3_FONT_MAX];				// �t�H���g�쐬���̃f�[�^
		D3_TEXT					textWork[D3_TEXT_MAX];

		//--- ���C�� ------------------------------------------------
		int						iLineCount;							// �o�^���̂R�c���C����
		D3_VTX_L				vtxLine[D3_3DLINE_MAX];				// �o�^���̃��C���o�b�t�@

		//--- ���_�E�s�N�Z���V�F�[�_�[ ------------------------------
		D3_INFO					shaderInfo[D3_SHADER_TYPE_MAX];		// �V�F�[�_�[�̏��
		bool					bLegacy;							// true:���s���� VS11�APS11�̂ݓ���
		uchar					ucShaderType;						// �V�F�[�_�[�̃^�C�v
		LPD3DXEFFECT			lpEffect[D3_SHADER_TYPE_MAX];		// �G�t�F�N�g�iHLSL�j
		D3DXHANDLE				hEffect[ D3_SHADER_TYPE_MAX];		// �G�t�F�N�g�̃n���h��
		LPD3DXEFFECT			eff;								// ���ݎg�p���̃G�t�F�N�g
		bool					bAlphaSw;							// true : �A���t�@�L��
		bool					bAlphaTest;							// true : �A���t�@�e�X�g�L��
		bool					bLight;								// true : ���C�g�L��
		uchar					ucCulling;							// true : �J�����O�L��
		uchar					ucSprFilter;						// �X�v���C�g�̃t�B���^�[�^�C�v(D3DTEXF_POINT or D3DTEXF_LINEAR)
		float					fLightIntensity;					// �o���v�̎��Ɏg�����C�g(�l���傫���قǖ��邢)

		//--- �I�t�X�N���[�� ------------------------------------------
		int						iOffscreenBackUpCount;				// �I�t�X�N���[���̓���q�ɂȂ��Ă��鐔
		LPDIRECT3DSURFACE9		lpOffscreenZBuffer[	   D3_TEX_MAX];	// �y�o�b�t�@
		LPDIRECT3DSURFACE9		lpOffscreenOldSurface[ D3_OFFSCREEN_BAK_MAX];
		LPDIRECT3DSURFACE9		lpOffscreenOldZBuffer[ D3_OFFSCREEN_BAK_MAX];
		LPDIRECT3DSURFACE9		lpOffscreenNewTarget[  D3_OFFSCREEN_BAK_MAX];
		DWORD					*pdwOffscreenRepairBuf[D3_TEX_MAX];	// �T�[�t�F�C�X�ۑ��p
		bool					bOffScreenFlagBak[     D3_TEX_MAX];	// �X�N���[���؂�ւ����Ƀt���O��ۑ�����
	};

	extern	D3_WORK	d3;

//--- �}�N�� --------------------------------------------------------------------------------------
	#define	R(fDeg)					((fDeg) * 0.01745329252f)		// D3DXToRadian(r)
	#define	D(fRad)					((fRad) * 57.29577951f)			// D3DXToDegree(deg)

	inline	void	d3_stat_stream_source_set(IDirect3DVertexBuffer9 *pStreamData, UINT Stride)
	{
		d3.dev->SetStreamSource(0, pStreamData, 0, Stride);
	}

	inline	void	d3_primitive_draw(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
	{
		d3.dev->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
		d3.iPolyCount += PrimitiveCount;
	}


//--- DirectX �X�e�C�^�X�ݒ�n --------------------------------------------------------------------

	/*------------------------------------------------------------------------------*
	| <<< �����ݒ� >>>
	|	����	iDat  = 0 : ��������
	|			iDat != 0 : RGB 256 �i�K�Ƃ������̍����������ǂꂾ���������̒l
	*------------------------------------------------------------------------------*/
	inline	void	d3_stat_alpha_greate_set(int iDat)
	{
		if(iDat == 0)
		{
			d3.bAlphaTest = false;
			d3.dev->SetRenderState(D3DRS_ALPHAREF, iDat);
		}
		else
		{
			d3.bAlphaTest = true;
			d3.dev->SetRenderState(D3DRS_ALPHAFUNC,	D3DCMP_GREATEREQUAL);
			d3.dev->SetRenderState(D3DRS_ALPHAREF, iDat);
		}
	}

	/*------------------------------------------------------------------------------*
	| <<< �A���t�@�ݒ� >>>
	|	����	bSw = true : �F���������L��
	*------------------------------------------------------------------------------*/
	inline	void	d3_stat_alpha_set(bool bSw)
	{
		d3.bAlphaSw = bSw;
		d3.dev->SetRenderState(D3DRS_ALPHABLENDENABLE, (bSw));
	}

	/*------------------------------------------------------------------------------*
	| <<< �A���t�@�ݒ� >>>	d3.bAlphaSw �ɒl���������܂��ݒ�
	|	����	bSw = true : �F���������L��
	*------------------------------------------------------------------------------*/
	inline	void	d3_stat_alpha_direct_set(bool bSw)
	{
		d3.dev->SetRenderState(D3DRS_ALPHABLENDENABLE, (bSw));
	}

	/*------------------------------------------------------------------------------*
	| <<< FVF �ݒ� >>>
	|	����	dwFvf = FVF �l
	*------------------------------------------------------------------------------*/
	inline	void	d3_stat_fvf_set(DWORD dwFvf)
	{
   		d3.dev->SetFVF(dwFvf);
	}

	/*------------------------------------------------------------------------------*
	| <<< �t�B���^�[�ݒ� >>>
	|	����	bSw = true : �J�����O�L��
	*------------------------------------------------------------------------------*/
	inline	void	d3_stat_filter_set(int iType)
	{
		d3.dev->SetSamplerState(0, D3DSAMP_MINFILTER, iType);
		d3.dev->SetSamplerState(0, D3DSAMP_MAGFILTER, iType);
	}
	inline	void	d3_stat_filter_set(int iMinType, int iMagType)
	{
		d3.dev->SetSamplerState(0, D3DSAMP_MINFILTER, iMinType);
		d3.dev->SetSamplerState(0, D3DSAMP_MAGFILTER, iMagType);
	}

	/*------------------------------------------------------------------------------*
	| <<< �J�����O�ݒ� >>>
	|	����	bSw = true : �J�����O�L��
	*------------------------------------------------------------------------------*/
	inline	void	d3_stat_culling_set(bool bSw)
	{
		d3.ucCulling = (uchar)bSw * D3DCULL_CCW;
		d3.dev->SetRenderState(D3DRS_CULLMODE, bSw ? D3DCULL_CCW : D3DCULL_NONE);
	}

	/*------------------------------------------------------------------------------*
	| <<< �y�o�b�t�@�̓ǂݏ����ݒ� >>>
	|	����	bRead  = true : �y�ǂݍ��ݗL��
	|			bWrite = true : �y�������ݗL��
	*------------------------------------------------------------------------------*/
	inline	void	d3_stat_zread_write_set(bool bRead, bool bWrite)
	{
		d3.dev->SetRenderState(D3DRS_ZWRITEENABLE, bRead);
		d3.dev->SetRenderState(D3DRS_ZENABLE,      bWrite);
	}

	/*------------------------------------------------------------------------------*
	| <<< �y�o�b�t�@�̓ǂݐݒ� >>>
	|	����	bRead  = true : �y�ǂݍ��ݗL��
	*------------------------------------------------------------------------------*/
	inline	void	d3_stat_zread_set(bool bRead)
	{
		d3.dev->SetRenderState(D3DRS_ZWRITEENABLE, bRead);
	}

	/*------------------------------------------------------------------------------*
	| <<< �y�o�b�t�@�̏����ݒ� >>>
	|	����	bWrite = true : �y�������ݗL��
	*------------------------------------------------------------------------------*/
	inline	void	d3_stat_zwrite_set(bool bWrite)
	{
		d3.dev->SetRenderState(D3DRS_ZENABLE,      bWrite);
	}

	/*------------------------------------------------------------------------------*
	| <<< �Œ�p�C�v���C���Ƀ��[���h���W�𑗂� >>>
	|	����	dwType = D3DTS_VIEW or D3DTS_PROJECTION or D3DTS_WORLD
	|			pmtx   = �s��
	*------------------------------------------------------------------------------*/
	inline	void	d3_stat_transform_set(DWORD dwType, MTX *pmtx)
	{
		d3.dev->SetTransform((D3DTRANSFORMSTATETYPE)dwType, pmtx);
	}
	inline	void	d3_stat_transform_set(MTX *pmtx)
	{
		d3.dev->SetTransform(D3DTS_WORLD, pmtx);
	}

	/*------------------------------------------------------------------------------*
	| <<< �h��Ԃ����[�h�ݒ� >>>				* �� D3_FILLMODE
	|	����	bFill  = true : �h��Ԃ�
	*------------------------------------------------------------------------------*/
	inline	void	d3_stat_fill_set(BOOL bFill)
	{
		d3.dev->SetRenderState(D3DRS_FILLMODE, bFill ==	FALSE ?	D3DFILL_WIREFRAME :	D3DFILL_SOLID);
	}

	/*------------------------------------------------------------------------------*
	| <<< ���C�g�ݒ� >>>
	|	����	bSw = true : ���C�g�L��
	*------------------------------------------------------------------------------*/
	inline	void	d3_light_set(bool bSw)
	{
		d3.bLight = bSw;
		d3.dev->SetRenderState(D3DRS_LIGHTING, bSw);
	}

	/*------------------------------------------------------------------------------*
	| <<< �}�e���A���ݒ� >>>
	|	����	m = �}�e���A��
	*------------------------------------------------------------------------------*/
	inline	void	d3_material_set(MATERIAL *m)
	{
		d3.dev->SetMaterial(m);
	}
	
	// �e�N�X�`���[
	extern	bool	d3_tex_set(int iTexNum);

	// �e�N�X�`���[�̋󂫂�������
	extern	int		d3_tex_unused_get(void);


	inline	bool	D3_TEXTURE(int iID, int iTexNum)
	{
		return d3_tex_set(iTexNum);
	}
	inline	bool	D3_TEXTURE(int iTexNum)
	{
		return d3_tex_set(iTexNum);
	}
												// �e�N�X�`���[���ǂݍ��܂�Ă��邩���ׂ�
	inline	bool	d3_texture_check(int iTexNum)
	{
		return (d3.tex[iTexNum] != NULL);
	}

												// �f�B�U�����O
	//#define	D3_DITHER()			d3.dev->SetRenderState(D3DRS_DITHERENABLE, d3.bDither);

												// �f�B�U�����O
	//#define	D3_DITHER_SW(a)		d3.dev->SetRenderState(D3DRS_DITHERENABLE,	a);
	inline	void	d3_stat_dither_set(bool bSw)
	{
		//d3.bDither = bSw;
		d3.dev->SetRenderState(D3DRS_DITHERENABLE, bSw);
	}

	/*inline	void	D3_SHADER_GOURAUD(bool b)	// �V�F�[�f�B���O���[�h
	{
		d3.dev->SetRenderState(D3DRS_SHADEMODE, b == false ? D3DSHADE_FLAT : D3DSHADE_GOURAUD);
	}*/
											// �V�F�[�f�B���O���[�h
	inline	void	d3_stat_gouraud(bool bSw)
	{
		d3.dev->SetRenderState(D3DRS_SHADEMODE, bSw ? D3DSHADE_GOURAUD : D3DSHADE_FLAT);
	}

	enum								// �������[�h
	{
		D3_BLEND_NONE,						// �x�^
		D3_BLEND_NORMAL,					// �ʏ�(���߂���)
		D3_BLEND_ADD,						// ���Z
		D3_BLEND_DEC,						// ���Z
	};

	inline	void	d3_stat_blend_set(uint uiType)
	{
		switch(uiType)
		{
		// �x�^
		case D3_BLEND_NONE:
			d3.dev->SetRenderState(D3DRS_ALPHABLENDENABLE,  TRUE);
			d3.dev->SetRenderState(D3DRS_BLENDOP,		    D3DBLENDOP_ADD);
			d3.dev->SetRenderState(D3DRS_SRCBLEND,		    D3DBLEND_SRCALPHA);
			d3.dev->SetRenderState(D3DRS_DESTBLEND,		    D3DBLEND_INVSRCALPHA);
			d3.dev->SetTextureStageState(0,	D3DTSS_ALPHAOP,	D3DTOP_MODULATE);
			d3_stat_alpha_greate_set(0);
			break;
		// �ʏ�(���߂���)
		case D3_BLEND_NORMAL:
			d3.dev->SetRenderState(D3DRS_ALPHABLENDENABLE,  TRUE);
			d3.dev->SetRenderState(D3DRS_BLENDOP,		    D3DBLENDOP_ADD);
			d3.dev->SetRenderState(D3DRS_SRCBLEND,		    D3DBLEND_SRCALPHA);
			d3.dev->SetRenderState(D3DRS_DESTBLEND,		    D3DBLEND_INVSRCALPHA);
			d3.dev->SetTextureStageState(0,	D3DTSS_ALPHAOP,	D3DTOP_MODULATE);
			d3_stat_alpha_greate_set(16);
			break;
		// ���Z
		case D3_BLEND_ADD:
			d3.dev->SetRenderState(D3DRS_ALPHABLENDENABLE,  TRUE);
			d3.dev->SetRenderState(D3DRS_BLENDOP,		    D3DBLENDOP_ADD);
			d3.dev->SetRenderState(D3DRS_SRCBLEND,		    D3DBLEND_SRCALPHA);
			d3.dev->SetRenderState(D3DRS_DESTBLEND,		    D3DBLEND_ONE);
			d3.dev->SetTextureStageState(0,	D3DTSS_ALPHAOP,	D3DTOP_MODULATE);
			break;
		// ���Z
		case D3_BLEND_DEC:
			d3.dev->SetRenderState(D3DRS_ALPHABLENDENABLE,  TRUE);
			d3.dev->SetRenderState(D3DRS_BLENDOP,		    D3DBLENDOP_REVSUBTRACT);
			d3.dev->SetRenderState(D3DRS_SRCBLEND,		    D3DBLEND_ONE);
			d3.dev->SetRenderState(D3DRS_DESTBLEND,		    D3DBLEND_ONE);
			d3.dev->SetTextureStageState(0,	D3DTSS_ALPHAOP,	D3DTOP_MODULATE);
			break;
		}
	}

												// �r���{�[�h�ϊ�
	inline	void	D3_BILLBOARD(MTX *v, VEC3 *pos)
	{
		v->_41 = pos->x, v->_42	= pos->y, v->_43 = pos->z;
		d3.dev->SetTransform(D3DTS_WORLD, v);
	}

	inline	void	d3_billboard_set(MTX *v, VEC3 *pos)
	{
		v->_41 = pos->x, v->_42	= pos->y, v->_43 = pos->z;
		d3.dev->SetTransform(D3DTS_WORLD, v);
	}

	enum	D3_TEX_ADDRES
	{
		D3_TEX_ADDRESS_WRAP		  = D3DTADDRESS_WRAP,
		D3_TEX_ADDRESS_MIRROR	  = D3DTADDRESS_MIRROR,
		D3_TEX_ADDRESS_CLAMP	  = D3DTADDRESS_CLAMP,
		D3_TEX_ADDRESS_BORDER	  = D3DTADDRESS_BORDER,
		D3_TEX_ADDRESS_MIRRORONCE = D3DTADDRESS_MIRRORONCE,
	};

	inline	void	d3_stat_tex_address_set(D3_TEX_ADDRES texAddres)
	{
		d3.dev->SetSamplerState(0, D3DSAMP_ADDRESSU, texAddres);
		d3.dev->SetSamplerState(0, D3DSAMP_ADDRESSV, texAddres);
	}

	inline	void	d3_clear(void)
	{
		d3.dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3_COL(255, 0, 0, 0), 1,	0);
	}

	inline	void	d3_clear(DWORD col)
	{
		d3.dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, col,	1, 0);
	}

	inline	void	d3_clear_zbuffer(void)
	{
		d3.dev->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0,	1, 0);
	}

	inline	void	d3_present(void)
	{
		if(!d3.bPresent || sys.bFrameSkip){ return;}

		d3.dev->Present(NULL, d3.rectRender, NULL, NULL);
		d3.ucPresentCount++;
	}

	inline	void	d3_present(CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion)
	{
		d3.dev->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
		d3.ucPresentCount++;
	}
	
	//--- �����_�����O�J�n ------------------------------------------
	inline	void	d3_render_begin(void)
	{
		d3.dev->SetLight(	0, &d3.light);
		d3.dev->LightEnable(0, true);
		d3.dev->BeginScene();
		d3.dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, d3.dwClearCol, 1, 0);
	}

	inline	void	d3_render_begin(DWORD col)
	{
		d3.dev->SetLight(	0, &d3.light);
		d3.dev->LightEnable(0, true);
		d3.dev->BeginScene();
		d3.dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, col, 1, 0);
	}

	inline	void	d3_render_begin(COL *pcol)
	{
		d3.dev->SetLight(	0, &d3.light);
		d3.dev->LightEnable(0, true);
		d3.dev->BeginScene();
		DWORD	dwCol = D3_COL(pcol->a, pcol->r, pcol->g, pcol->b);
		d3.dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, dwCol, 1, 0);
	}

	inline	void	d3_render_begin_scene(void)
	{
		d3.dev->BeginScene();
	}

	inline	void	d3_render_end_scene(void)
	{
		d3.dev->EndScene();
	}

	extern	void	d3_render_end(BOOL bPrint = TRUE);


//--- �֐�	---------------------------------------------------------------------------------------
/**************************************************************************************************
	[[[	�����_�����O�n ]]]
**************************************************************************************************/
extern	void	d3_render_sub(void *sub);
extern	void	d3_render(void);


/**************************************************************************************************
	[[[	�����n ]]]
**************************************************************************************************/
																							// �t�H���g�̍쐬
extern	void	d3_font_create(int iNum, int iW, int iH, char *str, bool bItalic, float fWeightMult);
extern	void	d3_font_create(int iNum, int iW, int iH, char *str, bool bItalic);
extern	void	d3_font_create(int iNum, int iW, int iH, char *str);
extern	void	d3_font_create(          int iW, int iH, char *str);
extern	void	d3_font_free(void);															// �t�H���g�̊J��
																							// �����`��
extern	void	d3_font_draw(int iFontNum, float fX, float fY,			 DWORD dwCol, char* Format,...);
extern	void	d3_font_draw(int iFontNum, float fX, float fY, int iLen, DWORD dwCol, char* Format,...);
extern	void	d3_font_draw(int iFontNum, RECT *r, bool bCenter, DWORD dwCol, char* Format,...);
extern	void	d3_printf(int iNum, int iX, int iY, D3DCOLOR dwCol, char* Format,...);		// ������o�^
extern	void	d3_printf(int iNum, int iX, int iY,                 char* Format,...);
extern	void	d3_printf(          int iX, int iY,                 char* Format,...);
extern	void	d3_printf(                                          char* Format,...);
extern	void	d3_print_shadow_set(int iNum, bool bShadow);								// �f�o�b�N�p�����ɉe�ݒ�
extern	void	d3_print(void);																// d3_printf �œo�^�����������S�ĕ`�悷��(�����Ŏg�p)
extern	void	d3_print_off(int iNum);														// �����\���I�t
extern	void	d3_print_off_all(void);														// �S�����\���I�t

/**************************************************************************************************
	[[[	�`��n ]]]
**************************************************************************************************/
											// �~�`��
extern	void	d3_2d_circle_draw(VEC2 *pvecPos, float fR, uint uiDiv, DWORD dwCol);
extern	void	d3_2d_circle_draw(VEC2 *pvecPos, float fR,             DWORD dwCol);

											// �~��`���h��Ԃ�
extern	void	d3_2d_circle_fill_draw(VEC2 *pvecPos, float fR, uint uiDiv, DWORD dwCol);
extern	void	d3_2d_circle_fill_draw(VEC2 *pvecPos, float fR,             DWORD dwCol);

											// �Q�c�l�p�`�h��Ԃ��`��
extern	void	d3_2d_box_fill_draw(int iX0, int iY0, int iX1, int iY1, float fZ, DWORD dwCol);
											// �Q�c�O�p�`�`��
extern	void	d3_2d_triangle_fill_draw(VEC3 *pvecP0, VEC3 *pvecP1, VEC3 *pvecP2, COL *pCol);
											// �R�c�O�p�`�`��
extern	void	d3_3d_triangle_fill_draw(VEC3 *pvecP0, VEC3 *pvecP1, VEC3 *pvecP2, COL *pCol);
											// ���_�����A�Q�c�}�`��`��
extern	void	d3_2d_shape_draw(VEC3 *pvecPos, float fScale, float fAngle, float fXScale, float fYScale, COL *pcol, int iPolyCount, float *pfVertex, short *psIndex);
											// �Q�c�l�p�`�e�N�X�`���[�\��t���`��
extern	void	d3_2d_box_fill_tex_draw(VEC2 *pvecP, VEC2 *pvecUV, uint uiBlendType, uint uiAlp);
extern	void	d3_2d_box_fill_tex_draw(int iX0, int iY0, int iX1, int iY1, int iZ, RECT *prect, uint uiBlendType, uint uiAlp);
extern	void	d3_2d_box_fill_tex_draw(int iX0, int iY0, int iX1, int iY1, int iZ, BOOL bAlphaGreat, uint uiAlp);
extern	void	d3_2d_box_fill_tex_draw(int iX0, int iY0, int iX1, int iY1, int iX2, int iY2, int iX3, int iY3, int iZ, BOOL bAlphaGreat, uint uiAlp);

extern	void	d3_2d_box_fill_tex_draw(VEC2 *pvecP0, VEC2 *pvecP1, float fU0, float fV0, float fU1, float fV1, DWORD dwCol);
extern	void	d3_2d_box_fill_tex_draw(VEC2 *pvecP0, VEC2 *pvecP1, float fU0, float fV0, float fU1, float fV1);
extern	void	d3_2d_box_fill_tex_draw(VEC2 *pvecP0, VEC2 *pvecP1, DWORD dwCol);
extern	void	d3_2d_box_fill_tex_draw(VEC2 *pvecP0, VEC2 *pvecP1);

extern	void	d3_2d_box_fill_tex_draw(VEC2 *apvecPos, float *afUV, DWORD dwCol);
extern	void	d3_2d_box_fill_tex_draw(VEC2 *apvecPos, float *afUV);
extern	void	d3_2d_box_fill_tex_draw(VEC2 *apvecPos, DWORD dwCol);
extern	void	d3_2d_box_fill_tex_draw(VEC2 *apvecPos);

																							// �R�c���̕`��
extern	void	d3_3d_sphere_draw(VEC3 *pvecPos, float fRadius, MTX *pmtxWorld, COL4 *pCol);
extern	void	d3_3d_sphere_draw(VEC3 *pvecPos, float fRadius, COL4 *pCol);
																							// �O�p�`�̕`��
extern	void	d3_2d_triangle_fill_draw(VEC3 *pvecP0, VEC3 *pvecP1, VEC3 *pvecP2, DWORD dwCol);
extern	void	d3_3d_triangle_fill_draw(VEC3 *pvecP0, VEC3 *pvecP1, VEC3 *pvecP2, DWORD dwCol);

extern	void	d3_3d_line_set(VEC3 *p0, VEC3 *p1, DWORD c);								// ���C���o�^
extern	void	d3_3d_line_flush(MTX *pmtx, bool bSw);
extern	void	d3_3d_line_draw(VEC3 *p0, VEC3 *p1, DWORD c);
extern	void	d3_2d_line_draw(VEC2 *pvecP0, VEC2 *pvecP1, DWORD dwCol);


extern	void	d3_2d_box_fill_draw(VEC2 *pvecP0, VEC2 *pvecP1, DWORD dwCol);				// �e�N�X�`���[�����a�n�w�h��Ԃ��`��
extern	void	d3_2d_box_draw(VEC2 *pvecP0, VEC2 *pvecP1, DWORD dwCol);					// �e�N�X�`���[�����a�n�w�`��

/**************************************************************************************************
	[[[	�X�v���C�g�n ]]]
**************************************************************************************************/
extern	void	d3_spr_draw(int iTex, RECT *r, VEC3 *vecPos, MTX *mtxWorld, DWORD dwCol);
extern	void	d3_spr_draw(int iTex, RECT *r, VEC3 *vecPos, MTX *mtxWorld);
extern	void	d3_spr_draw(int iTex, RECT *r, VEC3 *vecPos, DWORD dwCol);
extern	void	d3_spr_draw(int iTex, RECT *r, VEC3 *vecPos);

/**************************************************************************************************
	[[[	���b�V���n ]]]
**************************************************************************************************/

extern	void	d3_3d_draw(int	iNum,			       MTX *mtxWorld, float fAlp);
extern	void	d3_3d_draw(int	iNum,			       MTX *mtxWorld);
extern	void	d3_3d_draw(int	iNum, int iShaderType, MTX *mtxWorld, float fAlp);
extern	void	d3_3d_draw(int	iNum, int iShaderType, MTX *mtxWorld);
																		// �w��ԍ��̃T�u�Z�b�g��`��
extern	void	d3_3d_draw_parts(int iNum, int iMatNum, int iShaderType,  MTX *mtxWorld, float fAlp);
extern	void	d3_3d_draw_parts(int iNum, int iMatNum,                   MTX *mtxWorld, float fAlp);
extern	void	d3_3d_draw_parts(int iNum, int iMatNum,                   MTX *mtxWorld);
extern	void	d3_mesh_draw(	 int iNum, int iVsType, MTX	*mtxWorld, float fAlp, int iStart, int iEnd);

/**************************************************************************************************
	[[[	�o�b�N�o�b�t�@���A�N�Z�X����n ]]]
**************************************************************************************************/
extern	D3DCOLOR	d3_pixel_get(int x, int y);
extern	void		d3_texture_to_buffer(int tex_no, uchar	*buf);
extern	void		d3_buffer_to_texture(int tex_no, uchar	*buf);

/************************************************************************
	[[[	�a�l�o�n ]]]
************************************************************************/
extern	D3DCOLOR	d3_change_xrgb(DWORD col, D3DFORMAT format);
extern	bool		d3_bmp_head_read(HANDLE	f0,	BMP_WORK *w, char *s);
extern	bool		d3_bmp_read(BMP_WORK *w, char *s);
extern	void		d3_bmp_change_32color(BMP_WORK	*w);
extern	void		d3_bmp_x_reverse(BMP_WORK *w);
extern	void		d3_bmp_y_reverse(BMP_WORK *w);
extern	void		d3_bmp_free(BMP_WORK *w);
extern	bool		d3_bmp_info_read(BMP_INFO *w, char	*s);

/**************************************************************************************************
	[[[	�e�N�X�`���[�n ]]]
**************************************************************************************************/
extern	int		d3_tex_load(int iNum, char *strFile, bool bLoadAtOnce, bool bErrMessage);
extern	int		d3_tex_load(int no, char *file, bool bLoadAtOnce);
extern	int		d3_tex_load(int no, char *file);

extern	int		d3_vol_load(int iNum, char *strFile);

extern	void	d3_tex_free_all(void);
extern	void	d3_tex_free(int no);
extern	bool	d3_tex_create(int no, int w, int h, D3DFORMAT fmt);
extern	void	d3_screen_capture(int tex_no);
extern	void	d3_reload_set(void);

/**************************************************************************************************
	[[[	�I�t�X�N���[���n ]]]
**************************************************************************************************/
extern	bool	d3_offscreen_create(int iID, int iW, int iH, D3DFORMAT fmt);
extern	bool	d3_offscreen_create(int no, int w,	int	h);
extern	void	d3_offscreen_free(int iNum);
extern	void	d3_offscreen_free(void);
extern	bool	d3_offscreen_begin(int	no);
extern	void	d3_offscreen_end(int no);
extern	void	d3_offscreen_mem_free(int iNum);
extern	void	d3_offscreen_mem_free(void);
extern	void	d3_offscreen_save(int iNum, char *strFile);
extern	void	d3_offscreen_load(int iNum, char *strFile);
extern	void	d3_offscreen_mem_save(int iNum);
extern	void	d3_offscreen_mem_save(void);
extern	void	d3_offscreen_repair(void);

extern	DWORD	*d3_offscreen_capture_begin(int iNum);
extern	void	d3_offscreen_capture_end(void);

/**************************************************************************************************
	[[[	�������A�I���n ]]]
**************************************************************************************************/
extern	void		d3_view_port_set(void);
extern	bool		d3_end(void);
extern	void		d3_caps_get(char *str);
extern	void		d3_window_style(bool window);
extern	bool		d3_init(bool bWindow);
extern	D3DFORMAT	d3_display_format_tbl[];
extern	D3DFORMAT	d3_stencil_tbl[];
extern	void		d3_device_repair(void);
extern	void		d3_screen_change(bool bStyle);


/**************************************************************************************************
	[[[	�֗����[�ȒZ�k�֐� ]]]
**************************************************************************************************/
	inline	D3DXMATRIX	*D3DXMatrixTranslation(MTX *pOut, VEC3 p){					return D3DXMatrixTranslation(pOut, p.x,	p.y, p.z);}

	inline	D3DXMATRIX	*D3DXMatrixTranslation(MTX *pOut, VEC3 *p){					return D3DXMatrixTranslation(pOut, p->x, p->y, p->z);}

	inline	D3DXMATRIX	*D3DXMatrixScaling(MTX *pOut, float fZoom){					return D3DXMatrixScaling(pOut, fZoom, fZoom, fZoom);}

	inline	D3DXMATRIX	*D3DXMatrixScaling(MTX *pOut, VEC3 p){						return D3DXMatrixScaling(pOut, p.x,	p.y, p.z);}

	inline	D3DXMATRIX	*D3DXMatrixScaling(MTX *pOut, VEC3 *p){						return D3DXMatrixScaling(pOut, p->x, p->y, p->z);}

	inline	D3DXMATRIX	*D3DXMatrixRotationYawPitchRoll(MTX *pOut, VEC3 p){			return D3DXMatrixRotationYawPitchRoll(pOut,	p.y, p.x, p.z);}

	inline	D3DXMATRIX	*D3DXMatrixRotationYawPitchRoll(MTX *pOut, VEC3 *p){		return D3DXMatrixRotationYawPitchRoll(pOut,	p->y, p->x,	p->z);}

	inline	D3DXQUATERNION	*D3DXQuaternionRotationYawPitchRoll(QTN *pOut, VEC3	p){	return D3DXQuaternionRotationYawPitchRoll(pOut,	p.y, p.x, p.z);}

	inline	D3DXQUATERNION	*D3DXQuaternionRotationYawPitchRoll(QTN *pOut, VEC3 *p){return D3DXQuaternionRotationYawPitchRoll(pOut,	p->y, p->x,	p->z);}


/**************************************************************************************************
	[[[	���W�v�Z�p�}�N�� ]]]
**************************************************************************************************/
	// ������ NDS ��A�C�t�H���A�v���ȂǃE�B���h�E�T�C�Y���s��`�Ȃ��̂��A
	// �t���X�N���[�����ɉ�ʒ����֊g��E�ړ������邽�߂̃}�N���ł��B
	
	// �ڂ����́w�T���v��_�s��`�E�B���h�E�x���Q�l�̂��ƁB

	#define	D3_CONV_X(xx)	((xx) * d3.fZoom + d3.vecOfs.x)
	#define	D3_CONV_Y(yy)	((yy) * d3.fZoom + d3.vecOfs.y)

	#define	D3_CONV_3D_SCALE_X()	((float)__SCR_W / (float)d3.sFullScreenRenderW * d3.fZoom)
	#define	D3_CONV_3D_SCALE_Y()	((float)__SCR_H / (float)d3.sFullScreenRenderH * d3.fZoom)
	#define	D3_CONV_3D_SCALE(mat)																	\
			if(!d3.bWindow) D3DXMatrixScaling(mat, D3_CONV_3D_SCALE_X(), D3_CONV_3D_SCALE_Y(), 1);	\
			else			memcpy(mat, &d3.mtxIdentity, sizeof(MTX));



/**************************************************************************************************
	[[[	�Z�p�}�N�� ]]]
**************************************************************************************************/

	//	sin	�� cos		���o�͂� float �^
	__inline	void	__cdecl	SINCOS(float x,	float *s, float	*c)
	{
		__asm
		{
			fld	x
			fsincos
			mov	eax, c
			mov	edx, s
			fstp	dword ptr [eax]
			fstp	dword ptr [edx]
		}
	}


/**************************************************************************************************
	[[[	�o�b�t�@����n ]]]
**************************************************************************************************/

	//--- �o�b�t�@�쐬 --------------------------------------------------------------------------------
	// ����
	//	__SRC	= ���_���`�����z��̃A�h���X
	//	__VB	= ���_�o�b�t�@�̃A�h���X
	//	__TYPE	= �`��^�C�v
	//	__COUNT	= ���_�̐�
	#define	D3_VERTEX_CREATE(__SRC, __VB, __TYPE, __COUNT)									\
		{																					\
			HRESULT	hr;																		\
			hr = d3.dev->CreateVertexBuffer((__COUNT) *	sizeof(D3_VTX_##__TYPE), 0,			\
										D3_FVF_##__TYPE, D3DPOOL_MANAGED, &__VB, NULL);		\
			if(!FAILED(hr))																	\
			{																				\
				LPVOID	pDst;																\
				hr = __VB->Lock(0, 0, (void	**)&pDst, 0);									\
				if(!FAILED(hr))																\
				{																			\
					memcpy(pDst, __SRC,	(__COUNT) *	sizeof(D3_VTX_##__TYPE));				\
					__VB->Unlock();															\
				}																			\
			}																				\
		}
	
	//--- �o�b�t�@�`�� ---------------------------------------------------
	// ����
	//	__DRAW	= �|���S���̕`��^�C�v	(D3DPT_TRIANGLEFAN or D3DPT_TRIANGLELIST)
	//	__VB	= ���_�o�b�t�@�̃A�h���X
	//	__TYPE	= �`��^�C�v
	//	__COUNT	= �|���S���̐�
	#define	D3_VERTEX_RENDER(__DRAW, __VB,	__TYPE,	__COUNT)									\
		{																						\
			if(__VB != NULL)																	\
			{																					\
				d3.dev->SetStreamSource(0,	__VB, sizeof(D3_VTX_##__TYPE));						\
				d3.dev->SetFVF(D3_FVF_##__TYPE);												\
				d3.dev->DrawPrimitive(__DRAW, 0, __COUNT);										\
				d3.iPolyCount += __COUNT;														\
			}																					\
		}
	#define	D3_RENDER(__DRAW, __VT, __TYPE, __COUNT)											\
		{																						\
			d3.dev->SetFVF(D3_FVF_##__TYPE);													\
			d3.dev->DrawPrimitiveUP(__DRAW,	__COUNT, __VT, sizeof(D3_VTX_##__TYPE));			\
			d3.iPolyCount += __COUNT;															\
		}
	#define	D3_POLY_RENDER(__DRAW, __VTX, __TYPE, __COUNT)										\
		{																						\
			d3.dev->SetFVF(D3_FVF_##__TYPE);													\
			d3.dev->DrawPrimitiveUP(D3DPT_##__DRAW,	__COUNT, __VTX,	sizeof(D3_VTX_##__TYPE));	\
			d3.iPolyCount += __COUNT;															\
		}

/**************************************************************************************************
	[[[	�֗��ȕ`��}�N�� ]]]
**************************************************************************************************/

	//--- 2D�œh��Ԃ��ăe�N�X�`���[�𒣂� -----------------------------
	// ��������̓t���X�N���[�����̕␳������܂���
	//	p0,	c0 = ����
	//	p1,	c1 = �E��

	inline	void	D3_2DDRAW_(VEC3 *p0, VEC3 *p1, float u0, float v0, float u1, float v1, DWORD c)
	{
		d3_light_set(false);
		D3_VTX_TLX_	vt[] =
		{
			p0->x, p1->y, 0, 1, c, u0, v1,
			p0->x, p0->y, 0, 1, c, u0, v0,
			p1->x, p0->y, 0, 1, c, u1, v0,
			p1->x, p1->y, 0, 1, c, u1, v1
		};
		d3_stat_fvf_set(D3_FVF_TLX);
		d3.dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,	2, vt, sizeof(D3_VTX_TLX));
		d3.iPolyCount += 2;
		d3_light_set(true);
	}
	inline	void	D3_2DDRAW_(VEC3 *p0, VEC3 *p1, float u0, float v0, float u1, float v1)
	{
		D3_2DDRAW_(p0, p1, u0, v0, u1, v1, D3_COL(255, 255, 255, 255));
	}
	inline	void	D3_2DDRAW_(VEC2 *p0, VEC2 *p1, float u0, float v0, float u1, float v1)
	{
		D3_2DDRAW_(&VEC3(p0->x, p0->y, 0), &VEC3(p1->x, p1->y, 0), u0, v0, u1, v1, D3_COL(255, 255, 255, 255));
	}
	inline	void	D3_2DDRAW_(void)
	{
		D3_2DDRAW_(&VEC3(0, 0, 0.9999f), &VEC3((float)d3.sRenderW, (float)d3.sRenderH, 0.9999f), 0, 0, 1, 1, D3_COL(255, 255, 255, 255));
	}
	inline	void	D3_2DDRAW_(VEC2 *p0, VEC2 *p1, float u0, float v0, float u1, float v1, DWORD c)
	{
		D3_2DDRAW_(&VEC3(p0->x, p0->y, 0), &VEC3(p1->x, p1->y, 0), u0, v0, u1, v1, c);
	}
	inline	void	D3_2DDRAW_(VEC3 *p0, VEC3 *p1, D3_UV *pUV, DWORD c)
	{
		D3_2DDRAW_(p0, p1, pUV->fU0, pUV->fV0, pUV->fU1, pUV->fV1, c);
	}
	inline	void	D3_2DDRAW_(VEC2 *p0, VEC2 *p1, D3_UV *pUV, DWORD c)
	{
		D3_2DDRAW_(&VEC3(p0->x, p0->y, 0), &VEC3(p1->x, p1->y, 0), pUV->fU0, pUV->fV0, pUV->fU1, pUV->fV1, c);
	}
	inline	void	D3_2DDRAW_(VEC3 *p0, VEC3 *p1)
	{
		D3_2DDRAW_(p0, p1, 0, 0, 1, 1, D3_COL_NORMAL);
	}
	inline	void	D3_2DDRAW_(VEC2 *p0, VEC2 *p1)
	{
		D3_2DDRAW_(&VEC3(p0->x, p0->y, 0), &VEC3(p1->x, p1->y, 0), 0, 0, 1, 1, D3_COL_NORMAL);
	}
	//--- 2D�œh��Ԃ��ăe�N�X�`���[�𒣂� -----------------------------
	//�@p0	  p1
	//	*-----*
	//	|     |
	//	*-----*
	//  p3    p2
	inline	void	D3_2DDRAW(VEC3 *p0, VEC3 *p1, VEC3 *p2, VEC3 *p3, 
		float u0, float v0, float u1, float v1,
		float u2, float v2, float u3, float v3,
		DWORD c)
	{
		d3_light_set(false);

		#ifdef	__2D_RESIZE
			float	x0 = D3_CONV_X(p0->x);
			float	x1 = D3_CONV_X(p1->x);
			float	x2 = D3_CONV_X(p2->x);
			float	x3 = D3_CONV_X(p3->x);
			float	y0 = D3_CONV_Y(p0->y);
			float	y1 = D3_CONV_Y(p1->y);
			float	y2 = D3_CONV_Y(p2->y);
			float	y3 = D3_CONV_Y(p3->y);
			D3_VTX_TLX_	vt[] = {
								x3, y3, 0, 1, c, u3, v3,
								x2, y2, 0, 1, c, u2, v2,
								x1, y1, 0, 1, c, u1, v1,
								x0, y0, 0, 1, c, u0, v0,
								};
		#else
			D3_VTX_TLX_	vt[] = {
								p3->x, p3->y, 0, 1, c, u3, v3,
								p2->x, p2->y, 0, 1, c, u2, v2,
								p1->x, p1->y, 0, 1, c, u1, v1,
								p0->x, p0->y, 0, 1, c, u0, v0,
								};
		#endif

		d3.dev->SetFVF(D3_FVF_TLX);
		d3.dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,	2, vt, sizeof(D3_VTX_TLX));
		d3.iPolyCount += 2;
		d3_light_set(true);
	}
	
	//	p0,	c0 = ����
	//	p1,	c1 = �E��
	inline	void	D3_2DDRAW(VEC3 *p0, VEC3 *p1, float u0, float v0, float u1, float v1, DWORD c)
	{
		d3_light_set(false);

		#ifdef	__2D_RESIZE
			float	x0 = D3_CONV_X(p0->x);
			float	x1 = D3_CONV_X(p1->x);
			float	y0 = D3_CONV_Y(p0->y);
			float	y1 = D3_CONV_Y(p1->y);
			D3_VTX_TLX_	vt[] = {x0, y1, 0, 1, c, u0, v1,
								x0, y0, 0, 1, c, u0, v0,
								x1, y0, 0, 1, c, u1, v0,
								x1, y1, 0, 1, c, u1, v1};
		#else
			D3_VTX_TLX_	vt[] = {p0->x, p1->y, 0, 1, c, u0, v1,
								p0->x, p0->y, 0, 1, c, u0, v0,
								p1->x, p0->y, 0, 1, c, u1, v0,
								p1->x, p1->y, 0, 1, c, u1, v1};
		#endif

		d3.dev->SetFVF(D3_FVF_TLX);
		d3.dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,	2, vt, sizeof(D3_VTX_TLX));
		d3.iPolyCount += 2;
		d3_light_set(true);
	}
	inline	void	D3_2DDRAW(VEC3 *p0, VEC3 *p1, float u0, float v0, float u1, float v1)
	{
		D3_2DDRAW(p0, p1, u0, v0, u1, v1, D3_COL(255, 255, 255, 255));
	}
	inline	void	D3_2DDRAW(VEC2 *p0, VEC2 *p1, float u0, float v0, float u1, float v1)
	{
		D3_2DDRAW(&VEC3(p0->x, p0->y, 0), &VEC3(p1->x, p1->y, 0), u0, v0, u1, v1, D3_COL(255, 255, 255, 255));
	}
	inline	void	D3_2DDRAW(void)
	{
		D3_2DDRAW(&VEC3(0, 0, 0.9999f), &VEC3((float)d3.sRenderW, (float)d3.sRenderH, 0.9999f), 0, 0, 1, 1, D3_COL(255, 255, 255, 255));
	}
	inline	void	D3_2DDRAW(VEC2 *p0, VEC2 *p1, float u0, float v0, float u1, float v1, DWORD c)
	{
		D3_2DDRAW(&VEC3(p0->x, p0->y, 0), &VEC3(p1->x, p1->y, 0), u0, v0, u1, v1, c);
	}
	inline	void	D3_2DDRAW(VEC3 *p0, VEC3 *p1, D3_UV *pUV, DWORD c)
	{
		D3_2DDRAW(p0, p1, pUV->fU0, pUV->fV0, pUV->fU1, pUV->fV1, c);
	}
	inline	void	D3_2DDRAW(VEC2 *p0, VEC2 *p1, D3_UV *pUV, DWORD c)
	{
		D3_2DDRAW(&VEC3(p0->x, p0->y, 0), &VEC3(p1->x, p1->y, 0), pUV->fU0, pUV->fV0, pUV->fU1, pUV->fV1, c);
	}
	inline	void	D3_2DDRAW(VEC3 *p0, VEC3 *p1)
	{
		D3_2DDRAW(p0, p1, 0, 0, 1, 1, D3_COL_NORMAL);
	}
	inline	void	D3_2DDRAW(VEC2 *p0, VEC2 *p1)
	{
		D3_2DDRAW(&VEC3(p0->x, p0->y, 0), &VEC3(p1->x, p1->y, 0), 0, 0, 1, 1, D3_COL_NORMAL);
	}

	//--- 2D�œh��Ԃ� ----------------------------------------------
	//	p0,	c0 = ����
	//	p1,	c1 = �E��
	inline	void	D3_2DBOX_FILL(VEC3 *p0, VEC3 *p1, D3DCOLOR c)
	{
		#ifdef	__2D_RESIZE
			float	x0 = D3_CONV_X(p0->x);
			float	x1 = D3_CONV_X(p1->x);
			float	y0 = D3_CONV_Y(p0->y);
			float	y1 = D3_CONV_Y(p1->y);
			D3_VTX_TL_	vt[] = {
								x0, y0, 0, 1, c,
								x1, y0, 0, 1, c,
								x1, y1, 0, 1, c,
								x0, y1, 0, 1, c
								};
		#else
			D3_VTX_TL_	vt[] = {
								p0->x, p0->y, 0, 1,	c,
								p1->x, p0->y, 0, 1,	c,
								p1->x, p1->y, 0, 1,	c,
								p0->x, p1->y, 0, 1,	c
								};
		#endif
		d3.dev->SetTexture(0, NULL);
		d3.dev->SetFVF(D3_FVF_TL);
		d3.dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,	2, vt, sizeof(D3_VTX_TL));
		d3.iPolyCount += 2;
	}
	inline	void	D3_2DBOX_FILL(VEC2 *p0, VEC2 *p1, D3DCOLOR c)
	{
		D3_2DBOX_FILL(&VEC3(p0->x, p0->y, 0), &VEC3(p1->x, p1->y, 0), c);
	}

	//--- 2D������`�� ---------------------------------------------------
	//	p1,	c1 = �n�_
	//	p2,	c2 = �I�_
	inline	void	D3_2DLINE(VEC2 *p0, VEC2 *p1, D3DCOLOR c)
	{
		D3_VTX_TL_	vt[] =
		{
			p0->x, p0->y, 0, 1,	c,
			p1->x, p1->y, 0, 1,	c
		};
		d3.dev->SetTexture(0, NULL);
		d3.dev->SetFVF(D3_FVF_TL);
		d3.dev->DrawPrimitiveUP(D3DPT_LINESTRIP, 1,	vt,	sizeof(D3_VTX_TL));
	}
	inline	void	D3_2DLINE(VEC3 *p0, VEC3 *p1, D3DCOLOR c)
	{
		D3_2DLINE((VEC2*)p0, (VEC2*)p1, c);
	}
	//--- �R�c�����̓o�^ ----------------------------------------------
	#define	D3_3DLINE(p0, p1, col) d3_3d_line_set(p0, p1, col)

	// bSw = true :	���C����K����O�ɕ`��
	inline	void	D3_3DLINE_FLUSH(MTX *pmtx, bool	bSw){ d3_3d_line_flush(pmtx, bSw);}
	inline	void	D3_3DLINE_FLUSH(bool bSw){ d3_3d_line_flush(&d3.mtxIdentity, bSw);}
	inline	void	D3_3DLINE_FLUSH(void){     d3_3d_line_flush(&d3.mtxIdentity, false);}

	//--- 2D�Ń��C�� --------------------------------------------------
	//	p0,	c0 = ����
	//	p1,	c1 = �E��
	inline	void	D3_2DBOX(VEC3 *p0, VEC3 *p1, D3DCOLOR c)
	{
		#ifdef	__2D_RESIZE
			float	x0 = D3_CONV_X(p0->x);
			float	x1 = D3_CONV_X(p1->x);
			float	y0 = D3_CONV_Y(p0->y);
			float	y1 = D3_CONV_Y(p1->y);
			D3_VTX_TL_	vt[] =
			{
				x0, y0, p0->z, 1, c,
				x1, y0, p1->z, 1, c,
				x1, y1, p1->z, 1, c,
				x0, y1, p0->z, 1, c,
				x0, y0, p0->z, 1, c
			};
		#else
			D3_VTX_TL_	vt[] =
			{
				p0->x, p0->y, p0->z, 1,	c,
				p1->x, p0->y, p1->z, 1,	c,
				p1->x, p1->y, p1->z, 1,	c,
				p0->x, p1->y, p0->z, 1,	c,
				p0->x, p0->y, p0->z, 1,	c
			};
			d3.dev->SetTexture(0, NULL);
			d3.dev->SetFVF(D3_FVF_TL);
			d3.dev->DrawPrimitiveUP(D3DPT_LINESTRIP, 4,	vt,	sizeof(D3_VTX_TL));
		#endif
	}
	inline	void	D3_2DBOX(VEC2 *p0, VEC2 *p1, D3DCOLOR c)
	{
		D3_2DBOX(&VEC3(p0->x, p0->y, 0), &VEC3(p1->x, p1->y, 0), c);
	}

	//--- 3D ��	BOX	�h��Ԃ��ăe�N�X�`���[�𒣂� ------------------------------------------------
	//	p0,	c0 = ����
	//	p1,	c1 = �E��
	inline	void	D3_3DDRAW(VEC3 *p0, VEC3 *p1, VEC3 *p2, VEC3 *p3, float u0, float v0, float u1, float v1, float u2, float v2, float u3, float v3, DWORD c)
	{
		D3_VTX_LX_	vt[] =
		{
			p0->x, p0->y, p0->z, c,	u0,	v0,
			p1->x, p1->y, p1->z, c,	u1,	v1,
			p2->x, p2->y, p2->z, c,	u2,	v2,
			p3->x, p3->y, p3->z, c,	u3,	v3
		};
		d3.dev->SetFVF(			D3_FVF_LX);
		d3.dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vt, sizeof(D3_VTX_LX));
		d3.iPolyCount += 2;
	}
	inline	void	D3_3DDRAW(VEC3 *p0, VEC3 *p1, float u0, float v0, float u1, float v1, DWORD c)
	{
		D3_VTX_LX_	vt[] =
		{
			p0->x, p0->y, p0->z, c,	u0,	v1,
			p0->x, p1->y, p0->z, c,	u0,	v0,
			p1->x, p1->y, p0->z, c,	u1,	v0,
			p1->x, p0->y, p0->z, c,	u1,	v1
		};
		d3.dev->SetFVF(			D3_FVF_LX);
		d3.dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vt, sizeof(D3_VTX_LX));
		d3.iPolyCount += 2;
	}
	inline	void	D3_3DDRAW(VEC3 *p0, VEC3 *p1, DWORD c)
	{
		D3_3DDRAW(p0, p1, 0, 0, 1, 1, c);
	}
	inline	void	D3_3DDRAW(VEC3 *p0, VEC3 *p1)
	{
		D3_3DDRAW(p0, p1, 0, 0, 1, 1, D3_COL_NORMAL);
	}

	//--- �Q�c���C���̉~��`�� --------------------------------------------------------------------
	//	����	pos	 = �\�����W
	//			r	 = �傫��
	//			iDiv = ������(3 �` 35)
	//			cc	 = �F
	inline	void	D3_CIRCLE(VEC3 *pos, float r, int iDiv, DWORD cc)
	{
		D3_VTX_TL_	vt[36 + 1];
		float		dr = D3DX_PI / (iDiv / 2.f);
		int			i;
		for(i =	0; i < iDiv; i++)
		{
			float	s, c;
			SINCOS(dr *	(iDiv - i), &s, &c);
			vt[i].x	  = pos->x + r * c;
			vt[i].y   = pos->y + r * s;
			vt[i].z   = pos->z;
			vt[i].rhw =	1.0f;
			vt[i].c	  = cc;
		}
		vt[i] =	vt[0];							// iDiv �ځA�~�����
		d3_tex_set(-1);
		D3_RENDER(D3DPT_LINESTRIP, vt, TL, iDiv);
		d3.iPolyCount += iDiv;
	}

	inline	void	D3_CIRCLE(VEC3 *pos, float r, DWORD cc)
	{
		D3_CIRCLE(pos, r, 18, cc);
	}

	inline	void	D3_CIRCLE(VEC2 *pos, float r, int iDiv, DWORD cc)
	{
		D3_CIRCLE(&VEC3(pos->x, pos->y, 0), r, iDiv, cc);
	}

	inline	void	D3_CIRCLE(VEC2 *pos, float r, DWORD cc)
	{
		D3_CIRCLE(&VEC3(pos->x, pos->y, 0), r, 18, cc);
	}

	//--- �Q�c�̉~��`��(�h��Ԃ�) --------------------------------------------------------------
	//	����	pos	 =	�\�����W
	//			r	 =	�傫��
	//			iDiv = ������(3 �` 36)
	//			cc   =	�h��Ԃ��F
	inline	void	D3_CIRCLE_FILL(VEC3 *pos, float r, int iDiv, DWORD cc)
	{

		D3_VTX_TL_	vt[36 +	2];
		//float		dr = D3DX_PI / (iDiv / 2.f);
		
		vt[0].x	  =	pos->x;
		vt[0].y	  =	pos->y;
		vt[0].z	  =	pos->z;
		vt[0].rhw =	1.0f;
		vt[0].c	  =	cc;
		int			i;
		for(i =	0; i < iDiv; i++)
		{
			float	s, c;
			//SINCOS(dr *	(iDiv - i), &s, &c);
			s = sinf(D3DXToRadian((360.f / (float)iDiv) * (float)i));
			c = cosf(D3DXToRadian((360.f / (float)iDiv) * (float)i));
			vt[i + 1].x	  = pos->x + r * c;
			vt[i + 1].y	  =	pos->y + r * s;
			vt[i + 1].z	  =	pos->z;
			vt[i + 1].rhw =	1.0f;
			vt[i + 1].c	  =	cc;
		}
		vt[i + 1] =	vt[1];							// iDiv �ځA�~�����

		D3_TEXTURE(0, -1);
		D3_RENDER(D3DPT_TRIANGLEFAN, vt, TL, iDiv);
		d3.iPolyCount += iDiv;
	}

	inline	void	D3_CIRCLE_FILL(VEC3 *pos, float r, DWORD cc)
	{
		D3_CIRCLE_FILL(pos, r, 18, cc);
	}

	inline	void	D3_CIRCLE_FILL(VEC2 *pos, float r, int iDiv, DWORD cc)
	{
		D3_CIRCLE_FILL(&VEC3(pos->x, pos->y, 0), r, iDiv, cc);
	}

	inline	void	D3_CIRCLE_FILL(VEC2 *pos, float r, DWORD cc)
	{
		D3_CIRCLE_FILL(&VEC3(pos->x, pos->y, 0), r, 18, cc);
	}

	//--- �R�c�̉~��`��(�h��Ԃ�) --------------------------------------------------------------
	//	����	pos	  = �\�����W
	//			r	  = �傫��
	//			iDiv  = ������(3 �` 35)
	//			dwIn  = �����̓h��Ԃ��F
	//			dwOut = �O���̓h��Ԃ��F
	inline	void	D3_3DCIRCLE_FILL(VEC3 *pos, float r, int iDiv, DWORD dwIn, DWORD dwOut)
	{

		D3_VTX_L_	vt[36 +	2];
		float		fD = D3DX_PI / (iDiv / 2.f);
		
		vt[0].x	  =	pos->x;
		vt[0].y	  =	pos->y;
		vt[0].z	  =	pos->z;
		vt[0].c	  =	dwIn;
		int			i;
		for(i =	0; i < iDiv; i++)
		{
			float	s, c;
			SINCOS(fD *	(iDiv - i), &s, &c);
			//s = sinf(R((360.f / (float)iDiv) * (float)(iDiv - i)));
			//c = cosf(R((360.f / (float)iDiv) * (float)(iDiv - i)));
			vt[i + 1].x	  = pos->x + r * c;
			vt[i + 1].y	  =	pos->y + r * s;
			vt[i + 1].z	  =	pos->z;
			vt[i + 1].c	  =	dwOut;
		}
		vt[i + 1] =	vt[1];							// iDiv �ځA�~�����

		D3_TEXTURE(-1);
		D3_RENDER(D3DPT_TRIANGLEFAN, vt, L, iDiv);
		d3.iPolyCount += iDiv;
	}

	inline	void	D3_3DCIRCLE_FILL(VEC3 *pos, float r, DWORD cc)
	{
		D3_3DCIRCLE_FILL(pos, r, 18, cc, cc);
	}

	inline	void	D3_3DCIRCLE_FILL(VEC2 *pos, float r, int iDiv, DWORD cc)
	{
		D3_3DCIRCLE_FILL(&VEC3(pos->x, pos->y, 0), r, iDiv, cc, cc);
	}

	inline	void	D3_3DCIRCLE_FILL(VEC2 *pos, float r, DWORD cc)
	{
		D3_3DCIRCLE_FILL(&VEC3(pos->x, pos->y, 0), r, 18, cc, cc);
	}

	//--- 3D �œh��Ԃ��O�p�`	-------------------------------------
	//	p0 = ����
	//	p1 = �E��
	inline	void	D3_3DTRI_FILL(VEC3 *p0, VEC3 *p1, VEC3 *p2,	D3DCOLOR c)
	{
		d3_light_set(false);
		D3_VTX_L_	vt[] = {
								p0->x, p0->y, p0->z, c,
								p1->x, p1->y, p1->z, c,
								p2->x, p2->y, p2->z, c
							};
		d3.dev->SetTexture(0, NULL);
		d3.dev->SetFVF(    D3_FVF_L);
		d3.dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 1, vt, sizeof(D3_VTX_L));
		d3.iPolyCount += 1;
		d3_light_set(true);
	}
	
	//--- �q�b�g�p�̍��W�v�Z ---------------------------------------------
	//	w	  =	�q�b�g���[�N
	//	world =	���[���h�s��
	inline	void	D3_HIT_VEC_SET(VEC3 *vecOut, VEC3 *vecIn, MTX *world)
	{
		for(int	i =	0; i < 8; i++){ D3DXVec3TransformCoord(&vecOut[i], &vecIn[i], world);}
	}

	//--- �q�b�g�͈͂�`�� -----------------------------------------------
	//		vecPos	= ���S���W
	//		fRadius	= ���a
	inline	void	D3_HIT_FRAME_SPHERE_DRAW(VEC3 *vecPos, float fRadius)
	{
		#ifdef	__HIT_FRAME

			d3_stat_culling_set(false);
			d3_tex_set(			-1);

			//D3DMATERIAL9	*m = &d3.MaterialNormal;		// �}�e���A����Ԃɂ���
			//m->Diffuse.g = m->Diffuse.b = 0;
			//m->Diffuse.a = 0.5f;
			d3_material_set(&d3.matNormal);

			MTX		mtxWorld, mtxTrans, mtxScale;
			D3DXMatrixTranslation(&mtxTrans, vecPos);
			D3DXMatrixScaling(    &mtxScale, fRadius);
			
			d3_stat_fill_set(FALSE);
			d3_light_set(false);
			mtxWorld =	mtxScale * mtxTrans;
			d3_stat_transform_set(&mtxWorld);
			
			d3.lpMeshSphere->DrawSubset(0);
			d3_stat_fill_set(TRUE);
											// �}�e���A�������ɖ߂�
			d3_stat_culling_set(true);
			d3_light_set(       true);
		#endif
	}





