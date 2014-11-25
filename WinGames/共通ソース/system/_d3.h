//
// DirectGraphics 関係のプログラム
//

//--- ハードに関する設定

#define	D3_SOFTWARE_ONLY			0		// 1 : ソフトウェアのみでジオメトリ計算
#define	D3_HZ						60		// フルスクリーン時のデフォルトディスプレイ周波数
#define	D3_REZO_H					480		// フルスクリーン時のデフォルトの縦の最低解像度
#define	D3_FMT_DISPLAY_MAX			2		// fmtDisplay の数
#define	D3_FMT_STENCIL_MAX			6		// ステンシルバッファの登録数
											// バックバッファのデフォルトフォーマット
#define	D3_FORMAT					D3DFMT_X8R8G8B8
											// 環境光
#define	D3_AMBIENT					D3_COL(255, 51, 51, 51)

//-- テクスチャー関係
#define	D3_TEX_MAX					512		// ユーザーが使用可能なテクスチャーの最大数
#define	D3_TEX_MAX2					1024	// こちらはメッシュを含めたテスクチャー数

//--- 各種エフェクト
#define	D3_FUR_COUNT				20		// ファーの描画数

//--- フォント
#define	D3_FONT_MAX					8		// 文字の種類
#define	D3_TEXT_MAX					16		// 文字列の登録数

//--- メッシュ
#define	D3_MESH_MAX					512		// Ｘファイルの読み込み最大数

//--- オフスクリーン
#define	D3_OFFSCREEN_BAK_MAX		4		// オフスクリーンの入れ子数

//--- ビュー
#define	D3_VIEW_NEAR				__NEAR	// 手前のクリッピング領域
#define	D3_VIEW_FAR					__FAR	// 奥のクリッピング領域

//--- ライン
#ifndef	D3_3DLINE_MAX
	#define	D3_3DLINE_MAX			4096	// ３Ｄライン登録用
#endif

//--- 置き換え ------------------------------------------------------------------------------------
#define	D3_LPVTX_BUF				LPDIRECT3DVERTEXBUFFER9
#define	D3_LPINDEX_BUF				LPDIRECT3DINDEXBUFFER9
#define	D3_LPTEX					LPDIRECT3DTEXTURE9

//--- テスクチャー用 ------------------------------------------------------------------------------
	enum
	{
		TEX_RESERVE    = D3_TEX_MAX - 40 - 1,	// 予約されたテクスチャー

		#ifdef	__PRINTF
			TEX_MOJI_,							// 文字用
		#endif
		#ifdef	__AFONT
			TEX_AFONT,							// アンチフォント
			TEX_AFONT_END,
		#endif

		TEX_WHITE,								// 白(テクスチャーがないときの使用する)
	};

//--- シェーダータイプ ----------------------------------------------------------------------------
	enum
	{										// この値によりシェーディングが決まる
		D3_SHADER_TYPE_OFF				= -1,	// オフ
		D3_SHADER_TYPE_NORMAL,					// 00(通常)
		D3_SHADER_TYPE_MAX				= 50,
	};

//--- フィルター ----------------------------------------------------------------------------------
	enum
	{
		D3_FILTER_LINEAR = D3DTEXF_LINEAR,
		D3_FILTER_POINT  = D3DTEXF_POINT,
		D3_LINEAR		 = D3DTEXF_LINEAR,
		D3_POINT		 = D3DTEXF_POINT,
	};

//--- 文字表示用 ----------------------------------------------------------------------------------
	struct	D3_TEXT
	{
		uchar		ucFont;					// フォント番号
		short		sX, sY;
		D3DCOLOR	dwCol;
		bool		bShadow;				// true : 影をつける
		char		str[512];
	};

//--- 球用　---------------------------------------------------------------------------------------
	struct	D3_SPHERE
	{
		MTX		mtxWorld;					// 球の行列
		COL4	col;						// 色
	};

//--- ヒットチェック用 ----------------------------------------------------------------------------
	struct	D3_HIT
	{
		VEC3		vecLocal[  9];			// ボックスチェック用のローカル座標
		VEC3		vecWorld[  9];			// ボックスチェック用のワールド座標
		D3DXPLANE	planeWorld[6];			// あたり判定用平面式
		VEC3		vecMin,	vecMax;			// モデルの大きさ
		float		fRadius;				// 半径
		VEC3		vecCenter;				// 中心座標
	};

//--- テクスチャー＆メッシュ読み込み情報	-------------------------------------------------------
	struct	D3_INFO						// テクスチャーの諸情報
	{
		//--- テクスチヤー用 ------------------------------------------
		float		fU,		   fV;				// オリジナルの画像をフルに表示する際の	U1 V1 の値
		float		fOriWidth, fOriHeight;		// オリジナルの画像サイズ
		float		fWidth,	   fHeight;			// 画像のサイズ
		ushort		usFormat;					// 画像のフォーマット
		bool		bOffscreen;					// true	: オフスクリーン面である
		//--- メッシュ用 ----------------------------------------------
		bool		bSkin;						// true	: スキンメッシュ
		uchar		ucSkinAnimationSet;			// アニメーショントラック数
		uchar		ucSubset;					// サブセット数
		ushort		usVertex;					// 頂点数、	   最適化前、最適化後
		ushort		usFaces;					// ポリゴン数、最適化前、最適化後

		//--- 共通 ----------------------------------------------------
		bool		bResource;					// true : リソース
		bool		bLoad;						// true : 読み込み済みなら
		bool		bError;						// true : 読み込みエラー
		ushort		usUseCount;					// ここの値が０以上ならば、他の箇所(メッシュなど)で読み込んでいる
		bool		bRepair;					// true	: ロスト時に復帰させる
		char		strFullPath[MAX_PATH + 1];	// フルパス
		FILETIME	ftLastWrite;				// タイプスタンプ用(タイムスタンプが変更になると読み込みするよう)
	};

//--- UV ------------------------------------------------------------------------------------------
	struct	D3_UV
	{
		float	fU0, fV0, fU1, fV1;
	};

//--- 色 ------------------------------------------------------------------------------------------
	#define	D3_COL(__a,	__r, __g, __b)	D3DCOLOR_ARGB((int)(__a), (int)(__r), (int)(__g), (int)(__b))
	#define	D3_COL_NORMAL				D3DCOLOR_ARGB(255, 255,	255, 255)
	#define	D3_COL_ALL(	   __a)			D3DCOLOR_ARGB(__a, __a,	__a, __a)
	#define	D3_COL_WHITE(  __a)			D3DCOLOR_ARGB(__a, 255,	255, 255)
	#define	D3_COL_BLACK(  __a)			D3DCOLOR_ARGB(__a,	 0,	  0,   0)
	#define	D3_COL_RED(	   __a)			D3DCOLOR_ARGB(__a, 255,	  0,   0)
	#define	D3_COL_GREEN(  __a)			D3DCOLOR_ARGB(255,	 0,	__a,   0)
	#define	D3_COL_BLUE(   __a)			D3DCOLOR_ARGB(__a,	 0,	  0, 255)
	#define	D3_COL_YELLOW( __a)			D3DCOLOR_ARGB(__a, 255,	255,   0)

//--- 固定パイプライン設定 ------------------------------------------------------------------------
	//--- 未ライティング、テクスチャ有り-----------------------------
										// 頂点シェーダー FVF
	#define	D3_FVF_DEFAULT	(D3DFVF_XYZ	| D3DFVF_NORMAL	| D3DFVF_TEX1)
	struct	D3_VTX_DEFAULT
	{
		VEC3	p;							// 座標
		VEC3	n;							// 法線
		VEC2	uv;							// テクスチャー座標
	};
	struct	D3_VTX_DEFAULT_
	{
		float	x, y, z, nx, ny, nz;		// 座標、法線
		float	u, v;						// テクスチャー座標
	};

	//--- ２Ｄ用 ----------------------------------------------------
	#define	D3_FVF_TL		(D3DFVF_XYZRHW | D3DFVF_DIFFUSE)	// 座標変換済み、ライティング済み
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

	//--- ライン、ポイント用 ----------------------------------------
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

	//--- 座標変換済み、ライティング済み、テクスチャ有り --------------
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

	//--- ライティング済み、テクスチャ有り-----------------------------
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

//--- ＢＭＰ用 ------------------------------------------------------------------------------------
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
		char		strName[256];		// ファイル名
		BMP_INFO	info;				// ＢＭＰ情報
		BMP_HEAD	head;				// ＢＭＰヘッダ
		BMP_COL		col[256];			// ＢＭＰカラー
		union
		{
			uchar	*ucBuf;				// 画像データ格納位置
			uchar	*pucBuf;
		};
		ushort		*usBuf16;			// 画像データ格納位置
		uchar		*ucBuf24;			// 画像データ格納位置
		uchar		*ucBuf32;			// 画像データ格納位置
		ushort		usXbak;				// 横の補正前の値
		uchar		ucDat;				// 横の余りに入れる値
	};									// ＢＭＰ切り出し用のワーク

//--- DirectX 管理用 ------------------------------------------------------------------------------
	struct	D3_WORK
	{
		LPDIRECT3D9				d;						// Direct3D	の元
		LPDIRECT3DDEVICE9		dev;					// デバイス

		D3DDISPLAYMODE			deskTopDisplayMode;		// 現在のディスプレイモード
		D3DDISPLAYMODE			displayMode;			// ディスプレイモード
		D3DPRESENT_PARAMETERS	para;					// プレゼントパラメーター(Direct3D の設定)
		D3DCAPS9				caps;					// アダプターの情報
		//DWORD					dwSwapEffectMode;		// Present のモード。D3DSWAPEFFECT 列挙型が入ります
		//												// D3DSWAPEFFECT_FLIP	 ウィンドウモード時(画面のキャプチャーができる)	※DEF
		//												// D3DSWAPEFFECT_COPY	 フルスクリーン時
		//												// D3DSWAPEFFECT_DISCARD ツール時(d3.rectRender が使用可能)

		bool					bWindow;				// ウィンドウモードなら	true
		bool					bWindowBak;				// __EXIT_MENU 有効時に使用されます
		bool					bRender;				// レンダリング要求フラグ
		bool					bMovieEncode; 			// ムービーを１フレームエンコードした
		bool					bPresent;				// 通常は true だが、セパレーター移動時などに false とすると画面がちらつかない
		bool					bPaintPresent;			// WM_PAINT 時に画像の転送を行う(初期値true);
		bool					bReRender;				// 再レンダリング要求フラグ(テクスチャー読み込み時などに発生)
		bool					bDeviceChange;			// true : デバイス変更中
		uchar					ucPresentCount;			// 画面の切り替えカウント
		uchar					ucAdapter;				// 使用するアダプター番号
		uchar					ucVertexProcessing;		// 頂点の計算方法
		D3DDEVTYPE				deviceType;				// デバイスタイプ HAL or REF or	SW
		D3DFORMAT				fmtDisplay;				// ディスプレイフォーマット
		D3DFORMAT				fmtBackBuffer;			// バックバッファフォーマット
		D3DFORMAT				fmtStencil;				// ステンシルフォーマット
		D3DFORMAT				fmtTexture;				// 読み込みテクスチャーのフォーマット
		RECT					*rectRender;			// レンダリング時の転送先範囲を示す変数のポインタ(NULLなら全画面)
		short					sRenderW, sRenderH;		// レンダリングサイズ
		short					sFullScreenRenderW;		// こちらはフルスクリーン時のレンダリングサイズ
		short					sFullScreenRenderH;

		float					fZoom;					// 画面をズーム用の値(縦横比が同じになるように拡大時には同値を使う)
		VEC2					vecOfs;					// 画面中央へのオフセット値

		//--- ビュー関係 ----------------------------------------------
		D3DVIEWPORT9			viewport;
		MTX						mtxViewProjectionTranspose;	// mtxViewProjectionの転置行列
		MTX						mtxViewProjection;			// d3.mtxView * d3.mtxProjection としたもの
		MTX						mtxProjection;				// 射影行列
		MTX						mtxProjectionInverse;		// 射影逆行列
		MTX						mtxView, mtxViewBak;		// ビュー行列
		MTX						mtxViewInverse;				// ビュー逆行列(ビルボードで使用)
		MTX						mtxIdentity;				// 単位行列
		MTX						mtxBillboard;				// ビルボード用(41,42,43 の内容を保障しない)
		VEC3					vecEye;						// 注視座標
		VEC3					vecCamera;					// カメラ座標
		VEC3					vecUp;						// カメラの向き

		//--- 最適化フラグ --------------------------------------------
		DWORD					dwMeshOptimized;

		//--- フォグ関係 ----------------------------------------------
		DWORD					dwClearCol;					// フォグの色

		//--- マテリアル ------------------------------------------------
		D3DMATERIAL9			matNormal;					// (新)

		//--- ライト ----------------------------------------------------
		D3DLIGHT9				light;
		MTX						mtxLightViewProjection;		// ライトの行列
		VEC4					vecAmbient;					// 環境光

		//--- メッシュ関係 --------------------------------------------
		LPD3DXMESH				lpMeshSphere;						// ヒットチェックの球のメッシュ
		LPD3DXMESH				lpMesh[			D3_MESH_MAX];		// メッシュ
		short					sMeshCopyType[  D3_MESH_MAX][128];	// メッシュのコピーのタイプ(D3_SHADER_TYPE ～が入る)
		LPD3DXMESH				lpMeshCopy[		D3_MESH_MAX];		// メッシュのコピー用(おもにバンプなどで使用
		MATERIAL				*matMesh[		D3_MESH_MAX];		// メッシュのマテリアル
		short					sMeshTexNum[    D3_MESH_MAX][128];	// メッシュの使用するテクスチャー番号(ひとつのメッシュが使えるのテクスチャーは１２８枚)
		short					sMeshTexCopyNum[D3_MESH_MAX][128];	// メッシュの使用するバンプテクスチャー番号
		int						iPolyCount;							// ポリゴン総数
		int						iPolyCountMax;						// ポリゴン最高数
		D3_INFO					meshInfo[		D3_MESH_MAX];		// メッシュの情報
		
		//--- ヒットチェック ----------------------------------------
		D3_HIT					hitWork[D3_MESH_MAX];				// ヒットチェック用ワーク
		VEC3					vecFrustum[	 8];					// corners of the view frustum
		D3DXPLANE				planeFrustum[6];					// planes of the view frustum

		//--- テクスチャー関係 --------------------------------------
		int							iTexCurrent;					// カレントテクスチャー番号
		D3_INFO						texInfo[D3_TEX_MAX2];			// テクスチャーの情報
		LPDIRECT3DTEXTURE9			tex[	D3_TEX_MAX2];			// テクスチャー
		LPDIRECT3DVOLUMETEXTURE9	vol[	D3_TEX_MAX2];			// ボリュームテクスチャー

		//--- フォントー関係 ----------------------------------------
		LPD3DXFONT				font[	 D3_FONT_MAX];				// フォント
		D3DXFONT_DESC			fontInfo[D3_FONT_MAX];				// フォント作成時のデータ
		D3_TEXT					textWork[D3_TEXT_MAX];

		//--- ライン ------------------------------------------------
		int						iLineCount;							// 登録制の３Ｄライン数
		D3_VTX_L				vtxLine[D3_3DLINE_MAX];				// 登録制のラインバッファ

		//--- 頂点・ピクセルシェーダー ------------------------------
		D3_INFO					shaderInfo[D3_SHADER_TYPE_MAX];		// シェーダーの情報
		bool					bLegacy;							// true:実行環境は VS11、PS11のみ動作
		uchar					ucShaderType;						// シェーダーのタイプ
		LPD3DXEFFECT			lpEffect[D3_SHADER_TYPE_MAX];		// エフェクト（HLSL）
		D3DXHANDLE				hEffect[ D3_SHADER_TYPE_MAX];		// エフェクトのハンドル
		LPD3DXEFFECT			eff;								// 現在使用中のエフェクト
		bool					bAlphaSw;							// true : アルファ有効
		bool					bAlphaTest;							// true : アルファテスト有効
		bool					bLight;								// true : ライト有効
		uchar					ucCulling;							// true : カリング有効
		uchar					ucSprFilter;						// スプライトのフィルタータイプ(D3DTEXF_POINT or D3DTEXF_LINEAR)
		float					fLightIntensity;					// バンプの時に使うライト(値が大きいほど明るい)

		//--- オフスクリーン ------------------------------------------
		int						iOffscreenBackUpCount;				// オフスクリーンの入れ子になっている数
		LPDIRECT3DSURFACE9		lpOffscreenZBuffer[	   D3_TEX_MAX];	// Ｚバッファ
		LPDIRECT3DSURFACE9		lpOffscreenOldSurface[ D3_OFFSCREEN_BAK_MAX];
		LPDIRECT3DSURFACE9		lpOffscreenOldZBuffer[ D3_OFFSCREEN_BAK_MAX];
		LPDIRECT3DSURFACE9		lpOffscreenNewTarget[  D3_OFFSCREEN_BAK_MAX];
		DWORD					*pdwOffscreenRepairBuf[D3_TEX_MAX];	// サーフェイス保存用
		bool					bOffScreenFlagBak[     D3_TEX_MAX];	// スクリーン切り替え時にフラグを保存する
	};

	extern	D3_WORK	d3;

//--- マクロ --------------------------------------------------------------------------------------
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


//--- DirectX ステイタス設定系 --------------------------------------------------------------------

	/*------------------------------------------------------------------------------*
	| <<< 抜き設定 >>>
	|	入力	iDat  = 0 : 抜き無し
	|			iDat != 0 : RGB 256 段階とした時の黒い成分をどれだけ抜くかの値
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
	| <<< アルファ設定 >>>
	|	入力	bSw = true : 色抜き抜き有効
	*------------------------------------------------------------------------------*/
	inline	void	d3_stat_alpha_set(bool bSw)
	{
		d3.bAlphaSw = bSw;
		d3.dev->SetRenderState(D3DRS_ALPHABLENDENABLE, (bSw));
	}

	/*------------------------------------------------------------------------------*
	| <<< アルファ設定 >>>	d3.bAlphaSw に値を書き込まず設定
	|	入力	bSw = true : 色抜き抜き有効
	*------------------------------------------------------------------------------*/
	inline	void	d3_stat_alpha_direct_set(bool bSw)
	{
		d3.dev->SetRenderState(D3DRS_ALPHABLENDENABLE, (bSw));
	}

	/*------------------------------------------------------------------------------*
	| <<< FVF 設定 >>>
	|	入力	dwFvf = FVF 値
	*------------------------------------------------------------------------------*/
	inline	void	d3_stat_fvf_set(DWORD dwFvf)
	{
   		d3.dev->SetFVF(dwFvf);
	}

	/*------------------------------------------------------------------------------*
	| <<< フィルター設定 >>>
	|	入力	bSw = true : カリング有効
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
	| <<< カリング設定 >>>
	|	入力	bSw = true : カリング有効
	*------------------------------------------------------------------------------*/
	inline	void	d3_stat_culling_set(bool bSw)
	{
		d3.ucCulling = (uchar)bSw * D3DCULL_CCW;
		d3.dev->SetRenderState(D3DRS_CULLMODE, bSw ? D3DCULL_CCW : D3DCULL_NONE);
	}

	/*------------------------------------------------------------------------------*
	| <<< Ｚバッファの読み書き設定 >>>
	|	入力	bRead  = true : Ｚ読み込み有効
	|			bWrite = true : Ｚ書き込み有効
	*------------------------------------------------------------------------------*/
	inline	void	d3_stat_zread_write_set(bool bRead, bool bWrite)
	{
		d3.dev->SetRenderState(D3DRS_ZWRITEENABLE, bRead);
		d3.dev->SetRenderState(D3DRS_ZENABLE,      bWrite);
	}

	/*------------------------------------------------------------------------------*
	| <<< Ｚバッファの読み設定 >>>
	|	入力	bRead  = true : Ｚ読み込み有効
	*------------------------------------------------------------------------------*/
	inline	void	d3_stat_zread_set(bool bRead)
	{
		d3.dev->SetRenderState(D3DRS_ZWRITEENABLE, bRead);
	}

	/*------------------------------------------------------------------------------*
	| <<< Ｚバッファの書き設定 >>>
	|	入力	bWrite = true : Ｚ書き込み有効
	*------------------------------------------------------------------------------*/
	inline	void	d3_stat_zwrite_set(bool bWrite)
	{
		d3.dev->SetRenderState(D3DRS_ZENABLE,      bWrite);
	}

	/*------------------------------------------------------------------------------*
	| <<< 固定パイプラインにワールド座標を送る >>>
	|	入力	dwType = D3DTS_VIEW or D3DTS_PROJECTION or D3DTS_WORLD
	|			pmtx   = 行列
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
	| <<< 塗りつぶしモード設定 >>>				* 旧 D3_FILLMODE
	|	入力	bFill  = true : 塗りつぶす
	*------------------------------------------------------------------------------*/
	inline	void	d3_stat_fill_set(BOOL bFill)
	{
		d3.dev->SetRenderState(D3DRS_FILLMODE, bFill ==	FALSE ?	D3DFILL_WIREFRAME :	D3DFILL_SOLID);
	}

	/*------------------------------------------------------------------------------*
	| <<< ライト設定 >>>
	|	入力	bSw = true : ライト有効
	*------------------------------------------------------------------------------*/
	inline	void	d3_light_set(bool bSw)
	{
		d3.bLight = bSw;
		d3.dev->SetRenderState(D3DRS_LIGHTING, bSw);
	}

	/*------------------------------------------------------------------------------*
	| <<< マテリアル設定 >>>
	|	入力	m = マテリアル
	*------------------------------------------------------------------------------*/
	inline	void	d3_material_set(MATERIAL *m)
	{
		d3.dev->SetMaterial(m);
	}
	
	// テクスチャー
	extern	bool	d3_tex_set(int iTexNum);

	// テクスチャーの空きをさがす
	extern	int		d3_tex_unused_get(void);


	inline	bool	D3_TEXTURE(int iID, int iTexNum)
	{
		return d3_tex_set(iTexNum);
	}
	inline	bool	D3_TEXTURE(int iTexNum)
	{
		return d3_tex_set(iTexNum);
	}
												// テクスチャーが読み込まれているか調べる
	inline	bool	d3_texture_check(int iTexNum)
	{
		return (d3.tex[iTexNum] != NULL);
	}

												// ディザリング
	//#define	D3_DITHER()			d3.dev->SetRenderState(D3DRS_DITHERENABLE, d3.bDither);

												// ディザリング
	//#define	D3_DITHER_SW(a)		d3.dev->SetRenderState(D3DRS_DITHERENABLE,	a);
	inline	void	d3_stat_dither_set(bool bSw)
	{
		//d3.bDither = bSw;
		d3.dev->SetRenderState(D3DRS_DITHERENABLE, bSw);
	}

	/*inline	void	D3_SHADER_GOURAUD(bool b)	// シェーディングモード
	{
		d3.dev->SetRenderState(D3DRS_SHADEMODE, b == false ? D3DSHADE_FLAT : D3DSHADE_GOURAUD);
	}*/
											// シェーディングモード
	inline	void	d3_stat_gouraud(bool bSw)
	{
		d3.dev->SetRenderState(D3DRS_SHADEMODE, bSw ? D3DSHADE_GOURAUD : D3DSHADE_FLAT);
	}

	enum								// 合成モード
	{
		D3_BLEND_NONE,						// ベタ
		D3_BLEND_NORMAL,					// 通常(透過あり)
		D3_BLEND_ADD,						// 加算
		D3_BLEND_DEC,						// 減算
	};

	inline	void	d3_stat_blend_set(uint uiType)
	{
		switch(uiType)
		{
		// ベタ
		case D3_BLEND_NONE:
			d3.dev->SetRenderState(D3DRS_ALPHABLENDENABLE,  TRUE);
			d3.dev->SetRenderState(D3DRS_BLENDOP,		    D3DBLENDOP_ADD);
			d3.dev->SetRenderState(D3DRS_SRCBLEND,		    D3DBLEND_SRCALPHA);
			d3.dev->SetRenderState(D3DRS_DESTBLEND,		    D3DBLEND_INVSRCALPHA);
			d3.dev->SetTextureStageState(0,	D3DTSS_ALPHAOP,	D3DTOP_MODULATE);
			d3_stat_alpha_greate_set(0);
			break;
		// 通常(透過あり)
		case D3_BLEND_NORMAL:
			d3.dev->SetRenderState(D3DRS_ALPHABLENDENABLE,  TRUE);
			d3.dev->SetRenderState(D3DRS_BLENDOP,		    D3DBLENDOP_ADD);
			d3.dev->SetRenderState(D3DRS_SRCBLEND,		    D3DBLEND_SRCALPHA);
			d3.dev->SetRenderState(D3DRS_DESTBLEND,		    D3DBLEND_INVSRCALPHA);
			d3.dev->SetTextureStageState(0,	D3DTSS_ALPHAOP,	D3DTOP_MODULATE);
			d3_stat_alpha_greate_set(16);
			break;
		// 加算
		case D3_BLEND_ADD:
			d3.dev->SetRenderState(D3DRS_ALPHABLENDENABLE,  TRUE);
			d3.dev->SetRenderState(D3DRS_BLENDOP,		    D3DBLENDOP_ADD);
			d3.dev->SetRenderState(D3DRS_SRCBLEND,		    D3DBLEND_SRCALPHA);
			d3.dev->SetRenderState(D3DRS_DESTBLEND,		    D3DBLEND_ONE);
			d3.dev->SetTextureStageState(0,	D3DTSS_ALPHAOP,	D3DTOP_MODULATE);
			break;
		// 減算
		case D3_BLEND_DEC:
			d3.dev->SetRenderState(D3DRS_ALPHABLENDENABLE,  TRUE);
			d3.dev->SetRenderState(D3DRS_BLENDOP,		    D3DBLENDOP_REVSUBTRACT);
			d3.dev->SetRenderState(D3DRS_SRCBLEND,		    D3DBLEND_ONE);
			d3.dev->SetRenderState(D3DRS_DESTBLEND,		    D3DBLEND_ONE);
			d3.dev->SetTextureStageState(0,	D3DTSS_ALPHAOP,	D3DTOP_MODULATE);
			break;
		}
	}

												// ビルボード変換
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
	
	//--- レンダリング開始 ------------------------------------------
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


//--- 関数	---------------------------------------------------------------------------------------
/**************************************************************************************************
	[[[	レンダリング系 ]]]
**************************************************************************************************/
extern	void	d3_render_sub(void *sub);
extern	void	d3_render(void);


/**************************************************************************************************
	[[[	文字系 ]]]
**************************************************************************************************/
																							// フォントの作成
extern	void	d3_font_create(int iNum, int iW, int iH, char *str, bool bItalic, float fWeightMult);
extern	void	d3_font_create(int iNum, int iW, int iH, char *str, bool bItalic);
extern	void	d3_font_create(int iNum, int iW, int iH, char *str);
extern	void	d3_font_create(          int iW, int iH, char *str);
extern	void	d3_font_free(void);															// フォントの開放
																							// 文字描画
extern	void	d3_font_draw(int iFontNum, float fX, float fY,			 DWORD dwCol, char* Format,...);
extern	void	d3_font_draw(int iFontNum, float fX, float fY, int iLen, DWORD dwCol, char* Format,...);
extern	void	d3_font_draw(int iFontNum, RECT *r, bool bCenter, DWORD dwCol, char* Format,...);
extern	void	d3_printf(int iNum, int iX, int iY, D3DCOLOR dwCol, char* Format,...);		// 文字列登録
extern	void	d3_printf(int iNum, int iX, int iY,                 char* Format,...);
extern	void	d3_printf(          int iX, int iY,                 char* Format,...);
extern	void	d3_printf(                                          char* Format,...);
extern	void	d3_print_shadow_set(int iNum, bool bShadow);								// デバック用文字に影設定
extern	void	d3_print(void);																// d3_printf で登録した文字列を全て描画する(内部で使用)
extern	void	d3_print_off(int iNum);														// 文字表示オフ
extern	void	d3_print_off_all(void);														// 全文字表示オフ

/**************************************************************************************************
	[[[	描画系 ]]]
**************************************************************************************************/
											// 円描画
extern	void	d3_2d_circle_draw(VEC2 *pvecPos, float fR, uint uiDiv, DWORD dwCol);
extern	void	d3_2d_circle_draw(VEC2 *pvecPos, float fR,             DWORD dwCol);

											// 円を描き塗りつぶす
extern	void	d3_2d_circle_fill_draw(VEC2 *pvecPos, float fR, uint uiDiv, DWORD dwCol);
extern	void	d3_2d_circle_fill_draw(VEC2 *pvecPos, float fR,             DWORD dwCol);

											// ２Ｄ四角形塗りつぶし描画
extern	void	d3_2d_box_fill_draw(int iX0, int iY0, int iX1, int iY1, float fZ, DWORD dwCol);
											// ２Ｄ三角形描画
extern	void	d3_2d_triangle_fill_draw(VEC3 *pvecP0, VEC3 *pvecP1, VEC3 *pvecP2, COL *pCol);
											// ３Ｄ三角形描画
extern	void	d3_3d_triangle_fill_draw(VEC3 *pvecP0, VEC3 *pvecP1, VEC3 *pvecP2, COL *pCol);
											// 頂点情報より、２Ｄ図形を描画
extern	void	d3_2d_shape_draw(VEC3 *pvecPos, float fScale, float fAngle, float fXScale, float fYScale, COL *pcol, int iPolyCount, float *pfVertex, short *psIndex);
											// ２Ｄ四角形テクスチャー貼り付け描画
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

																							// ３Ｄ球の描画
extern	void	d3_3d_sphere_draw(VEC3 *pvecPos, float fRadius, MTX *pmtxWorld, COL4 *pCol);
extern	void	d3_3d_sphere_draw(VEC3 *pvecPos, float fRadius, COL4 *pCol);
																							// 三角形の描画
extern	void	d3_2d_triangle_fill_draw(VEC3 *pvecP0, VEC3 *pvecP1, VEC3 *pvecP2, DWORD dwCol);
extern	void	d3_3d_triangle_fill_draw(VEC3 *pvecP0, VEC3 *pvecP1, VEC3 *pvecP2, DWORD dwCol);

extern	void	d3_3d_line_set(VEC3 *p0, VEC3 *p1, DWORD c);								// ライン登録
extern	void	d3_3d_line_flush(MTX *pmtx, bool bSw);
extern	void	d3_3d_line_draw(VEC3 *p0, VEC3 *p1, DWORD c);
extern	void	d3_2d_line_draw(VEC2 *pvecP0, VEC2 *pvecP1, DWORD dwCol);


extern	void	d3_2d_box_fill_draw(VEC2 *pvecP0, VEC2 *pvecP1, DWORD dwCol);				// テクスチャー無しＢＯＸ塗りつぶし描画
extern	void	d3_2d_box_draw(VEC2 *pvecP0, VEC2 *pvecP1, DWORD dwCol);					// テクスチャー無しＢＯＸ描画

/**************************************************************************************************
	[[[	スプライト系 ]]]
**************************************************************************************************/
extern	void	d3_spr_draw(int iTex, RECT *r, VEC3 *vecPos, MTX *mtxWorld, DWORD dwCol);
extern	void	d3_spr_draw(int iTex, RECT *r, VEC3 *vecPos, MTX *mtxWorld);
extern	void	d3_spr_draw(int iTex, RECT *r, VEC3 *vecPos, DWORD dwCol);
extern	void	d3_spr_draw(int iTex, RECT *r, VEC3 *vecPos);

/**************************************************************************************************
	[[[	メッシュ系 ]]]
**************************************************************************************************/

extern	void	d3_3d_draw(int	iNum,			       MTX *mtxWorld, float fAlp);
extern	void	d3_3d_draw(int	iNum,			       MTX *mtxWorld);
extern	void	d3_3d_draw(int	iNum, int iShaderType, MTX *mtxWorld, float fAlp);
extern	void	d3_3d_draw(int	iNum, int iShaderType, MTX *mtxWorld);
																		// 指定番号のサブセットを描画
extern	void	d3_3d_draw_parts(int iNum, int iMatNum, int iShaderType,  MTX *mtxWorld, float fAlp);
extern	void	d3_3d_draw_parts(int iNum, int iMatNum,                   MTX *mtxWorld, float fAlp);
extern	void	d3_3d_draw_parts(int iNum, int iMatNum,                   MTX *mtxWorld);
extern	void	d3_mesh_draw(	 int iNum, int iVsType, MTX	*mtxWorld, float fAlp, int iStart, int iEnd);

/**************************************************************************************************
	[[[	バックバッファをアクセスする系 ]]]
**************************************************************************************************/
extern	D3DCOLOR	d3_pixel_get(int x, int y);
extern	void		d3_texture_to_buffer(int tex_no, uchar	*buf);
extern	void		d3_buffer_to_texture(int tex_no, uchar	*buf);

/************************************************************************
	[[[	ＢＭＰ系 ]]]
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
	[[[	テクスチャー系 ]]]
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
	[[[	オフスクリーン系 ]]]
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
	[[[	初期化、終了系 ]]]
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
	[[[	便利そーな短縮関数 ]]]
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
	[[[	座標計算用マクロ ]]]
**************************************************************************************************/
	// これらは NDS やアイフォンアプリなどウィンドウサイズが不定形なものを、
	// フルスクリーン時に画面中央へ拡大・移動させるためのマクロです。
	
	// 詳しくは『サンプル_不定形ウィンドウ』を参考のこと。

	#define	D3_CONV_X(xx)	((xx) * d3.fZoom + d3.vecOfs.x)
	#define	D3_CONV_Y(yy)	((yy) * d3.fZoom + d3.vecOfs.y)

	#define	D3_CONV_3D_SCALE_X()	((float)__SCR_W / (float)d3.sFullScreenRenderW * d3.fZoom)
	#define	D3_CONV_3D_SCALE_Y()	((float)__SCR_H / (float)d3.sFullScreenRenderH * d3.fZoom)
	#define	D3_CONV_3D_SCALE(mat)																	\
			if(!d3.bWindow) D3DXMatrixScaling(mat, D3_CONV_3D_SCALE_X(), D3_CONV_3D_SCALE_Y(), 1);	\
			else			memcpy(mat, &d3.mtxIdentity, sizeof(MTX));



/**************************************************************************************************
	[[[	算術マクロ ]]]
**************************************************************************************************/

	//	sin	と cos		※出力は float 型
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
	[[[	バッファ操作系 ]]]
**************************************************************************************************/

	//--- バッファ作成 --------------------------------------------------------------------------------
	// 入力
	//	__SRC	= 頂点を定義した配列のアドレス
	//	__VB	= 頂点バッファのアドレス
	//	__TYPE	= 描画タイプ
	//	__COUNT	= 頂点の数
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
	
	//--- バッファ描画 ---------------------------------------------------
	// 入力
	//	__DRAW	= ポリゴンの描画タイプ	(D3DPT_TRIANGLEFAN or D3DPT_TRIANGLELIST)
	//	__VB	= 頂点バッファのアドレス
	//	__TYPE	= 描画タイプ
	//	__COUNT	= ポリゴンの数
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
	[[[	便利な描画マクロ ]]]
**************************************************************************************************/

	//--- 2Dで塗りつぶしてテクスチャーを張る -----------------------------
	// ※こちらはフルスクリーン時の補正がありません
	//	p0,	c0 = 左上
	//	p1,	c1 = 右下

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
	//--- 2Dで塗りつぶしてテクスチャーを張る -----------------------------
	//　p0	  p1
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
	
	//	p0,	c0 = 左上
	//	p1,	c1 = 右下
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

	//--- 2Dで塗りつぶし ----------------------------------------------
	//	p0,	c0 = 左上
	//	p1,	c1 = 右下
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

	//--- 2D直線を描画 ---------------------------------------------------
	//	p1,	c1 = 始点
	//	p2,	c2 = 終点
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
	//--- ３Ｄ直線の登録 ----------------------------------------------
	#define	D3_3DLINE(p0, p1, col) d3_3d_line_set(p0, p1, col)

	// bSw = true :	ラインを必ず手前に描画
	inline	void	D3_3DLINE_FLUSH(MTX *pmtx, bool	bSw){ d3_3d_line_flush(pmtx, bSw);}
	inline	void	D3_3DLINE_FLUSH(bool bSw){ d3_3d_line_flush(&d3.mtxIdentity, bSw);}
	inline	void	D3_3DLINE_FLUSH(void){     d3_3d_line_flush(&d3.mtxIdentity, false);}

	//--- 2Dでライン --------------------------------------------------
	//	p0,	c0 = 左上
	//	p1,	c1 = 右下
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

	//--- 3D で	BOX	塗りつぶしてテクスチャーを張る ------------------------------------------------
	//	p0,	c0 = 左上
	//	p1,	c1 = 右下
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

	//--- ２Ｄラインの円を描く --------------------------------------------------------------------
	//	入力	pos	 = 表示座標
	//			r	 = 大きさ
	//			iDiv = 分割数(3 ～ 35)
	//			cc	 = 色
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
		vt[i] =	vt[0];							// iDiv 個目、円を閉じる
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

	//--- ２Ｄの円を描く(塗りつぶす) --------------------------------------------------------------
	//	入力	pos	 =	表示座標
	//			r	 =	大きさ
	//			iDiv = 分割数(3 ～ 36)
	//			cc   =	塗りつぶし色
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
		vt[i + 1] =	vt[1];							// iDiv 個目、円を閉じる

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

	//--- ３Ｄの円を描く(塗りつぶす) --------------------------------------------------------------
	//	入力	pos	  = 表示座標
	//			r	  = 大きさ
	//			iDiv  = 分割数(3 ～ 35)
	//			dwIn  = 内側の塗りつぶし色
	//			dwOut = 外側の塗りつぶし色
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
		vt[i + 1] =	vt[1];							// iDiv 個目、円を閉じる

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

	//--- 3D で塗りつぶし三角形	-------------------------------------
	//	p0 = 左上
	//	p1 = 右下
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
	
	//--- ヒット用の座標計算 ---------------------------------------------
	//	w	  =	ヒットワーク
	//	world =	ワールド行列
	inline	void	D3_HIT_VEC_SET(VEC3 *vecOut, VEC3 *vecIn, MTX *world)
	{
		for(int	i =	0; i < 8; i++){ D3DXVec3TransformCoord(&vecOut[i], &vecIn[i], world);}
	}

	//--- ヒット範囲を描画 -----------------------------------------------
	//		vecPos	= 中心座標
	//		fRadius	= 半径
	inline	void	D3_HIT_FRAME_SPHERE_DRAW(VEC3 *vecPos, float fRadius)
	{
		#ifdef	__HIT_FRAME

			d3_stat_culling_set(false);
			d3_tex_set(			-1);

			//D3DMATERIAL9	*m = &d3.MaterialNormal;		// マテリアルを赤にする
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
											// マテリアルを元に戻す
			d3_stat_culling_set(true);
			d3_light_set(       true);
		#endif
	}





