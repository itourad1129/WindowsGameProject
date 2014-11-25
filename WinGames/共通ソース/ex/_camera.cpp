//
//	カメラ処理
//


//--- 変数定義 ------------------------------------------------------------------------------------
						// ↓カメラの視野角(VC6.0ではなぜかエラーが)
CAMERA_WORK		camera;// = {R(45)};		// スケールだけ数値設定
static	BOOL	bInit     = false;


//--- デバッグカメラ移動用関数 --------------------------------------------------------------------

static	BOOL	bMoveCam1st = false;		// デバッグカメラ初期化フラグ
static	VEC3	vecMoveCamRot;				// デバッグカメラの回転値
static	VEC3	vecDebugCamPos;				// デバッグカメラの注視座標
static	VEC3	vecDebugCamOfs;				// デバッグカメラの注視座標のオフセット
static	float	fMoveCamDist;				// デバッグカメラの注視座標までの距離



/*------------------------------------------------------------------------------*
| <<< カメラ初期化 >>>
*------------------------------------------------------------------------------*/
void	camera_init(void)
{

	camera.fMoveScale = 0.1f;				// デバッグカメラの移動値
	camera.fViewAngle = R(45);				// アングルを初期化
	bInit			  = true;
}



/*------------------------------------------------------------------------------*
| <<< カメラ設定 >>>
|	入力	bDefault = true	: デフォルトのカメラを使用
*------------------------------------------------------------------------------*/
void	camera_main(void)
{

	//--- 初期化が必要な場合 --------------------------------------
	if(!bInit){ camera_init();}

	//--- 注視行列、射影行列の作成 --------------------------------
#ifdef	__RIGHT_HAND
	D3DXMatrixLookAtRH(&d3.mtxView,	&d3.vecCamera, &d3.vecEye, &d3.vecUp);
	if(camera.fOrthoScale > 0)
	{
		D3DXMatrixOrthoRH(&d3.mtxProjection, d3.sRenderW / camera.fOrthoScale, d3.sRenderH / camera.fOrthoScale, D3_VIEW_NEAR, D3_VIEW_FAR);
	}
	else
	{
		D3DXMatrixPerspectiveFovRH(&d3.mtxProjection, camera.fViewAngle, (float)d3.sRenderW / (float)d3.sRenderH, D3_VIEW_NEAR, D3_VIEW_FAR);
	}
#else
	D3DXMatrixLookAtLH(&d3.mtxView,	&d3.vecCamera, &d3.vecEye, &d3.vecUp);
	if(camera.fOrthoScale > 0)
	{
		D3DXMatrixOrthoLH(&d3.mtxProjection, d3.sRenderW / camera.fOrthoScale, d3.sRenderH / camera.fOrthoScale, D3_VIEW_NEAR, D3_VIEW_FAR);
	}
	else
	{
		D3DXMatrixPerspectiveFovLH(&d3.mtxProjection, camera.fViewAngle, (float)d3.viewport.Width / (float)d3.viewport.Height, D3_VIEW_NEAR, D3_VIEW_FAR);
	}
#endif
											// 各行列を設定
	d3.dev->SetTransform(D3DTS_VIEW,	   &d3.mtxView);
	d3.dev->SetTransform(D3DTS_PROJECTION, &d3.mtxProjection);
											// 頂点シェーダー用に計算しておく
	d3.mtxViewProjection = d3.mtxView *	d3.mtxProjection;
											// d3d_view_proj_mat の転置行列
	D3DXMatrixTranspose(&d3.mtxViewProjectionTranspose,	&d3.mtxViewProjection);

	//--- 逆行列の作成 ----------------------------------------------
	D3DXMatrixInverse(&d3.mtxViewInverse,       NULL, &d3.mtxView);
	D3DXMatrixInverse(&d3.mtxProjectionInverse, NULL, &d3.mtxProjection);

	//--- 以下、パーティクルの時に必要になります ------------------
	float	tmp;
	D3DXMatrixInverse(&d3.mtxViewInverse, &tmp,	&d3.mtxView);
	d3.mtxBillboard	= d3.mtxViewInverse;
}



/*------------------------------------------------------------------------------*
| <<< デバッグカメラ移動 >>>
|	※カメラは移動のみ、回転は対象オブジェクトをローカルで回す
*------------------------------------------------------------------------------*/
void	camera_move_main(void)
{

	//--- 初期化処理 ------------------------------------------------
	if(!bMoveCam1st)
	{
		vecDebugCamPos  = d3.vecEye;		// 注視座標のオフセット

		// 角度を得る
		GET_ANGLE(&vecMoveCamRot.y, &vecMoveCamRot.x, &d3.vecCamera, &d3.vecEye);
		vecMoveCamRot.z = 0;

		// 距離を得る
		fMoveCamDist    = D3DXVec3Length(&(d3.vecEye - d3.vecCamera));
		bMoveCam1st     = true;
	}

	//--- 右ドラッグでカメラ回転、ホイールでズーム ------------------
	if(pad.bRightPush && (pad.sMoveX != 0 || pad.sMoveY != 0 || pad.sMoveZ != 0))
	{
		fMoveCamDist -= (float)pad.sMoveZ * camera.fMoveScale;

		#ifdef	__RIGHT_HAND
			vecMoveCamRot -= VEC3(R(-pad.sMoveY), R(pad.sMoveX), 0);
		#else
			vecMoveCamRot += VEC3(R(-pad.sMoveY), R(pad.sMoveX), 0);
		#endif
	}

	//--- ホイールドラッグで移動 ------------------------------------
	if(pad.bCenterPush && (pad.sMoveX != 0 || pad.sMoveY != 0) && camera.fMoveScale > 0)
	{
		float	fPitch, fYaw;
		GET_ANGLE(&fYaw, &fPitch, &d3.vecEye, &d3.vecCamera);

		MTX	m;
		D3DXMatrixRotationYawPitchRoll(&m, fYaw, fPitch, 0);

		VEC3	a((float)pad.sMoveX, -(float)pad.sMoveY, 0);
		D3DXVec3TransformCoord(&a, &a, &m);
		vecDebugCamOfs -= a * camera.fMoveScale;
	}

	//--- カメラ座標計算 --------------------------------------------
	MTX		m;
	D3DXMatrixRotationYawPitchRoll(&m, &vecMoveCamRot);

	VEC3	a(0, 0, fMoveCamDist);
	D3DXVec3TransformCoord(        &a, &a, &m);
	
	d3.vecCamera = vecDebugCamPos + vecDebugCamOfs + a;
	d3.vecEye    = vecDebugCamPos + vecDebugCamOfs;
}

