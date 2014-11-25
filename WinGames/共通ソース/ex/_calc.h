//
//	色々な計算をマクロ化しています
//

#ifndef	__CALC_H

#define	__CALC_H


/*------------------------------------------------------------------------------*
| <<< sin cos を同時に得る >>>
*------------------------------------------------------------------------------*/
//	sin	と cos		※出力は float 型
__inline	void	__cdecl	SINCOS(float *pfSinOut, float *pfCosOut, float fR)
{
	__asm
	{
		fld	fR
		fsincos
		mov	eax, pfCosOut
		mov	edx, pfSinOut
		fstp	dword ptr [eax]
		fstp	dword ptr [edx]
	}
}

/*------------------------------------------------------------------------------*
| <<< 三角関数テーブルを作成 >>>
|	入力	iDiv 分割数
*------------------------------------------------------------------------------*/
extern	void	calc_sincos_init(int iDiv);

/*------------------------------------------------------------------------------*
| <<< sin >>>
|	入力	fRad = ラジアン
*------------------------------------------------------------------------------*/
extern	float	calc_sin_get(float fRad);

/*------------------------------------------------------------------------------*
| <<< cos >>>
|	入力	fRad = ラジアン
*------------------------------------------------------------------------------*/
extern	float	calc_cos_get(float fRad);

/*------------------------------------------------------------------------------*
| <<< ２Ｄベクトルの外積を求める >>>		※ D3DXVec2CCW と同等の関数
|	入力	p0, p1 = ２Ｄベクトル
|	戻り値	0  > 右側
|			0  < 左側
|			0 == 重なっている
*------------------------------------------------------------------------------*/
inline	float	calc_vec2_cross(VEC2 *p0, VEC2 *p1)
{
	return p0->x * p1->y - p0->y * p1->x;
}

/*------------------------------------------------------------------------------*
|	<<< ベクトルの外積を計算 >>>
|	入力	pvecA, pvecB = ベクトル
|	出力	pvecOut = 外積
*------------------------------------------------------------------------------*/
inline	void	calc_vec3_cross(VEC3 *pvecOut, VEC3 *pvecA, VEC3 *pvecB)
{
	#ifndef	__WII
		D3DXVec3Cross(pvecOut, pvecA, pvecB);
	#else
		VECCrossProduct((Vec*)pvecA, (Vec*)pvecB, (Vec*)pvecOut);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< ベクトルを正規化する >>>
|	入力	pvecA   = ベクトル
|	出力	pvecOut = 正規化されたベクトル
*------------------------------------------------------------------------------*/
inline	void	calc_vec4_normalize(VEC4 *pvecOut, VEC4 *pvecA)
{

	D3DXVec4Normalize(pvecOut, pvecA);
}

/*------------------------------------------------------------------------------*
|	<<< ベクトルを正規化する >>>
|	入力	pvecA   = ベクトル
|	出力	pvecOut = 正規化されたベクトル
*------------------------------------------------------------------------------*/
inline	void	calc_vec3_normalize(VEC3 *pvecOut, VEC3 *pvecA)
{
	#ifndef	__WII
		D3DXVec3Normalize(pvecOut, pvecA);
	#else
		//VECNormalize((Vec*)pvecA, (Vec*)pvecOut);
		nw4r::math::VEC3Normalize(pvecOut, pvecA);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< ２Ｄベクトルを正規化する >>>
|	入力	pvecA   = ベクトル
|	出力	pvecOut = 正規化されたベクトル
*------------------------------------------------------------------------------*/
inline	void	calc_vec2_normalize(VEC2 *pvecOut, VEC2 *pvecA)
{
	#ifndef	__WII
		D3DXVec2Normalize(pvecOut, pvecA);
	#else
		nw4r::math::VEC2Normalize(pvecOut, pvecA);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< ベクトルの内積を計算 >>>
|	入力	pvecA, pvecB = ベクトル
|	戻り値	内積の値
*------------------------------------------------------------------------------*/
inline	float	calc_vec3_dot(VEC3 *pvecA, VEC3 *pvecB)
{
	#ifndef	__WII
		return D3DXVec3Dot(pvecA, pvecB);
	#else
		return VECDotProduct((Vec*)pvecA, (Vec*)pvecB);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< ベクトルの内積を計算 >>>
|	入力	pvecA, pvecB = ベクトル
|	戻り値	内積の値
*------------------------------------------------------------------------------*/
inline	float	calc_vec2_dot(VEC2 *pvecA, VEC2 *pvecB)
{
	#ifndef	__WII
		return D3DXVec2Dot(pvecA, pvecB);
	#else
		return (pvecA->x * pvecB->x) + (pvecA->y * pvecB->y);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< ベクトルの長さを計算 >>>
|	入力	pvecA = ベクトル
|	戻り値	長さ
*------------------------------------------------------------------------------*/
inline	float	calc_vec3_length(VEC3 *pvecA)
{

	//return (float)sqrt((x * x) + (y * y) + (z * z));

	#ifndef	__WII
		return D3DXVec3Length(pvecA);
	#else
		return nw4r::math::VEC3Len(pvecA);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< ベクトルの長さの二乗を返す >>>	
|	入力	pvecA = ベクトル
|	戻り値	長さ
*------------------------------------------------------------------------------*/
inline	float	calc_vec3_length_sq(VEC3 *pvecA)
{
	
	// 以下のように sqrt を使わないので高速
	// return (x * x) + (y * y) + (z * z);

	#ifndef	__WII
		return D3DXVec3LengthSq(pvecA);
	#else
		return nw4r::math::VEC3Len(pvecA);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< ベクトルの長さを計算 >>>
|	入力	pvecA = ベクトル
|	戻り値	長さ
*------------------------------------------------------------------------------*/
inline	float	calc_vec2_length(VEC2 *pvecA)
{

	//return (float)sqrt((x * x) + (y * y));

	#ifndef	__WII
		return D3DXVec2Length(pvecA);
	#else
		return nw4r::math::VEC2Len(pvecA);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< ベクトルの長さの二乗を返す >>>	
|	入力	pvecA = ベクトル
|	戻り値	長さ
*------------------------------------------------------------------------------*/
inline	float	calc_vec2_length_sq(VEC2 *pvecA)
{
	
	// 以下のように sqrt を使わないので高速
	// return ((x * x) + (y * y);

	#ifndef	__WII
		return D3DXVec2LengthSq(pvecA);
	#else
		return nw4r::math::VEC2Len(pvecA);
	#endif
}


/*------------------------------------------------------------------------------*
|	<<< 行列同士の掛け算 >>>
|	入力	pmtxA, pmtxB = 掛け合わせる行列
|	出力	pmtxOut = 平行行列
*------------------------------------------------------------------------------*/
inline	void	calc_mtx_multi(MTX *pmtxOut, MTX *pmtxA, MTX *pmtxB)
{
	#ifndef	__WII
		D3DXMatrixMultiply(pmtxOut, pmtxA, pmtxB);
	#else
		//nw4r::math::MTX34Mult(pmtxOut, pmtxA, pmtxB);
		MTXConcat(*pmtxB, *pmtxA, *pmtxOut);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< 平行移動行列を作成 >>>
|	入力	fX, fY, fZ = 平行移動パラメータ
|	出力	pmtxOut    = 平行行列
*------------------------------------------------------------------------------*/
inline	void	calc_mtx_trans(MTX *pmtxOut, float fX, float fY, float fZ)
{
	#ifndef	__WII
		D3DXMatrixTranslation(pmtxOut, fX, fY, fZ);
	#else
		MTXTrans(*pmtxOut, fX, fY, fZ);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< 平行移動行列を作成 >>>
|	入力	pvecVec = 平行移動パラメータ
|	出力	pmtxOut = 平行行列
*------------------------------------------------------------------------------*/
inline	void	calc_mtx_trans(MTX *pmtxOut, VEC3 *pvecTrans)
{
	#ifndef	__WII
		D3DXMatrixTranslation(pmtxOut, pvecTrans);
	#else
		//nw4r::math::MTX34RotXYZRad(pmtxOut, pvecRot->x, pvecRot->y, pvecRot->z);
		MTXTrans(*pmtxOut, pvecTrans->x, pvecTrans->y, pvecTrans->z);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< ３軸の回転行列を作成 >>>
|	入力	pvecRot = 回転ベクトル
|	出力	pmtxOut = 回転行列
*------------------------------------------------------------------------------*/
inline	void	calc_mtx_yaw_pitch_roll(MTX *pmtxOut, VEC3 *pvecRot)
{
	#ifndef	__WII
		D3DXMatrixRotationYawPitchRoll(pmtxOut, pvecRot);
	#else
		nw4r::math::MTX34RotXYZRad(pmtxOut, pvecRot->x, pvecRot->y, pvecRot->z);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< ３軸の回転行列を作成 >>>
|	入力	fYaw, fPitch, fRoll = 各軸の回転値
|	出力	pmtxOut             = 回転行列
*------------------------------------------------------------------------------*/
inline	void	calc_mtx_yaw_pitch_roll(MTX *pmtxOut, float fYaw, float fPitch, float fRoll)
{
	#ifndef	__WII
		D3DXMatrixRotationYawPitchRoll(pmtxOut, fYaw, fPitch, fRoll);
		//MTX	mtxX, mtxY, mtxZ;
		//D3DXMatrixRotationX(&mtxX, fPitch);
		//D3DXMatrixRotationY(&mtxY, fYaw);
		//D3DXMatrixRotationZ(&mtxZ, fRoll);
		//*pmtxOut = mtxZ * mtxX * mtxY;
	#else
		// 下の場合だと、行列の掛ける順番が違うのか思うような値が帰ってこない
		#if 0
			OSInitFastCast();
			nw4r::math::MTX34RotXYZRad(pmtxOut, -fPitch, -fYaw, -fRoll);
		#else
			MTX	mtxX, mtxY, mtxZ;
			MTXRotRad((MtxPtr)mtxX, 'x', -fPitch);
			MTXRotRad((MtxPtr)mtxY, 'y', -fYaw);
			MTXRotRad((MtxPtr)mtxZ, 'z', -fRoll);
			MTXConcat(mtxZ, mtxX, mtxZ);
			MTXConcat(mtxZ, mtxY, *pmtxOut);
		#endif
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< Ｘ軸の回転行列を作成 >>>
|	入力	fRotX   = 回転値
|	出力	pmtxOut = 回転行列
*------------------------------------------------------------------------------*/
inline	void	calc_mtx_rot_x(MTX *pmtxOut, float fRotX)
{
	#ifndef	__WII
		D3DXMatrixRotationX(pmtxOut, fRotX);
	#else
		MTXRotRad((MtxPtr)pmtxOut, 'x', fRotX);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< Ｙ軸の回転行列を作成 >>>
|	入力	fRotY   = 回転値
|	出力	pmtxOut = 回転行列
*------------------------------------------------------------------------------*/
inline	void	calc_mtx_rot_y(MTX *pmtxOut, float fRotY)
{
	#ifndef	__WII
		D3DXMatrixRotationY(pmtxOut, fRotY);
	#else
		MTXRotRad((MtxPtr)pmtxOut, 'y', fRotY);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< Ｚ軸の回転行列を作成 >>>
|	入力	fRotZ   = 回転値
|	出力	pmtxOut = 回転行列
*------------------------------------------------------------------------------*/
inline	void	calc_mtx_rot_z(MTX *pmtxOut, float fRotZ)
{
	#ifndef	__WII
		D3DXMatrixRotationZ(pmtxOut, fRotZ);
	#else
		MTXRotRad((MtxPtr)pmtxOut, 'z', fRotZ);
	#endif
}

/*------------------------------------------------------------------------------*
|	<<< 拡大行列を作成 >>>
|	入力	fScale  = 拡大値
|	出力	pmtxOut = 回転行列
*------------------------------------------------------------------------------*/
inline	void	calc_mtx_scale(MTX *pmtxOut, float fScale)
{
	#ifndef	__WII
		D3DXMatrixScaling(pmtxOut, fScale);
	#else
		MTXScale((MtxPtr)pmtxOut, fScale, fScale, fScale);
	#endif
}
inline	void	calc_mtx_scale(MTX *pmtxOut, float fXScale, float fYScale, float fZScale)
{
	#ifndef	__WII
		D3DXMatrixScaling(pmtxOut, fXScale, fYScale, fZScale);
	#else
		MTXScale((MtxPtr)pmtxOut, fXScale, fYScale, fZScale);
	#endif
}

/*------------------------------------------------------------------------------*
| <<< ２Ｄベクトルを座標変換 >>>
|	入力	pvecIn  = ベクトル
|			pmtx    = 行列
|	出力	pvecOut = ベクトル
*------------------------------------------------------------------------------*/
extern	void	calc_vec2_transform_coord(VEC2 *pvecOut, VEC2 *pvecIn, MTX *pmtx);

/*------------------------------------------------------------------------------*
| <<< ３Ｄベクトルを座標変換 >>>
|	入力	pvecIn  = ベクトル
|			pmtx    = 行列
|	出力	pvecOut = ベクトル
*------------------------------------------------------------------------------*/
extern	void	calc_vec3_transform_coord(VEC3 *pvecOut, VEC3 *pvecIn, MTX *pmtx);

/*------------------------------------------------------------------------------*
| <<< ３Ｄ座標をスクリーン座標へ変換 >>>	※D3DXVec3Projectと同じ動作をします
|	入力	pvecPos   = ３Ｄ座標
|			pmtxWorld = ワールド行列
|	出力	pvecOut	  = ２Ｄ座標
|	※ビューポートはカット
*------------------------------------------------------------------------------*/
extern	void	calc_vec3_project(VEC3 *pvecOut, VEC3 *pvecPos, MTX *pmtxWorld);

/*------------------------------------------------------------------------------*
| <<< ２Ｄベクトルを Catmull-Rom で補完 >>>
|	入力	p0, p1, p2, p3 = 位置ベクトル
|			s			   = 係数(0.0f ～ 1.0f)
|	戻り値	pvecOut		   = 演算結果
*------------------------------------------------------------------------------*/
extern	void	calc_vec2_catmullrom(VEC2 *pvecOut, VEC2 *p0, VEC2 *p1, VEC2 *p2, VEC2 *p3, float s);

/*------------------------------------------------------------------------------*
| <<< 逆行列を作成 >>>				※D3DXMatrixInverseとほぼ同じ動作をします
|	入力	pmtxIn  = 逆行列にしたい行列
|	出力	pmtxOut = 逆行列
*------------------------------------------------------------------------------*/
extern	void	calc_mtx_inverse(MTX *pmtxOut, MTX *pmtxIn);




/*------------------------------------------------------------------------------*
| <<< 平面よりベクトルを取り出す >>>
|	入力	pplane  = 平面
|	出力	pvecOut = ベクトル
*------------------------------------------------------------------------------*/
inline	void	calc_plane_to_vec(VEC3 *pvecOut, PLANE *pplane)
{
	#ifndef	__WII
		//VEC3	vecPlane(plane.a,   plane.b,   plane.c);
		*pvecOut = VEC3(pplane->a, pplane->b, pplane->c);
	#else
		//VEC3	vecPlane(plane.N.x, plane.N.y, plane.N.z);
		*pvecOute = VEC3(pplane->N.x, pplane->N.y, pplane->N.z);
	#endif
}

/*------------------------------------------------------------------------------*
| <<< ３座標より平面を作成する >>>			※ D3DX の D3DXPlaneFromPoints と同じです。
|	入力	pvecA, pvecB, pvecC = 頂点
|	出力	pplaneOut = 面
*------------------------------------------------------------------------------*/
extern	void	calc_plane_create(PLANE *pplaneOut, VEC3 *pvecP0, VEC3 *pvecP1, VEC3 *pvecP2);

/*------------------------------------------------------------------------------*
| <<< 平面とベクトルの内積を計算 >>>		※ D3DX のD3DXPlaneDotNormal と同じです。
|	入力	pplane = 平面
|			pvec   = ベクトル
|	戻り値	内積の値
*------------------------------------------------------------------------------*/
extern	float	calc_plane_vec3_dot(PLANE *pplane, VEC3 *pvec);




/*------------------------------------------------------------------------------*
| <<< 自前で射影行列の作成 >>>				※ D3DXMatrixPerspectiveFovLH
|	※ d3.mtxProjection に作成します。
*------------------------------------------------------------------------------*/
extern	MTX	*calc_mtx_perspective(MTX *pOut, float fFov, float fZNear, float fZFar);

/*------------------------------------------------------------------------------*
| <<< 自前でビュー行列の作成 >>>				※ D3DXMatrixLookAtLH
|	※ d3.mtxView に作成します
*------------------------------------------------------------------------------*/
extern	MTX	*calc_mtx_lookat(MTX *pOut, VEC3 *pCam, VEC3 *pAt, VEC3 *pUp);

/*------------------------------------------------------------------------------*
| <<< ３Ｄ座標をスクリーン座標へ変換 >>>	※D3DXVec3Projectと同じ動作をします
|	入力	pvecPos        = ３Ｄ座標
|			pmtxProjection = 射影行列
|			pmtxView	   = ビュー行列
|			pmtxWorld	   = ワールド行列
|	出力	pvecOut		   = ２Ｄ座標
|	※ビューポートはカット
*------------------------------------------------------------------------------*/
extern	void	calc_vec3_projection(VEC3 *pvecOut, VEC3 *pvecPos, MTX *pmtxProjection, MTX *pmtxView, MTX *pmtxWorld);

/*------------------------------------------------------------------------------*
| <<< ２つのベクトルより角度を得る >>>
|	入力	a, b = ベクトル
|	戻り値	角度（ラジアン）
*------------------------------------------------------------------------------*/
extern	float	calc_vec3_vec_to_vec_angle(VEC3 *a, VEC3 *b);

/*------------------------------------------------------------------------------*
| <<< 面と点ベクトルとの内積を計算 >>>		※ D3DXPlaneDotNormal
|	入力	pplaneA = 面
|			pvecB   = 頂点ベクトル
|	戻り値	面と頂点ベクトルの内積
*------------------------------------------------------------------------------*/
extern	float	calc_plane_dot_normal(PLANE *pplaneA, VEC3 *pvecB);

/*------------------------------------------------------------------------------*
| <<< 平面と点との距離を求める >>>
|	入力	pvecNormal = 面の法線
|			pvecP0     = 面上の座標
|			pvecP1     = 面上には無い座標
|	戻り値	面と p1 との距離
|
|	法線ベクトル n とベクトル p - p0 とのなす角をθとすると、
|	n と p - p0 との内積は n・(p - p0) = |n||p-p0|cosθと表すことができる。
|	ここで、平面と点との距離 h は
|	θを用いて　h = |p-p0| cosθと表すことができるから、
|	これらの式を整理すると、h = n・(p - p0) / |n| となり、
|	この式から平面と点との距離が求まる。
*------------------------------------------------------------------------------*/
extern	float	calc_plane_vec_distance(VEC3 *pvecNormal, VEC3 *pvecP0, VEC3 *pvecP1);

/*------------------------------------------------------------------------------*
| <<< 行列からオイラー角を抽出する >>>
|	入力	m = 行列
|	戻り値	yaw pitch roll
*------------------------------------------------------------------------------*/
extern	VEC3	calc_mtx_to_eular(MTX *m);

/*------------------------------------------------------------------------------*
| <<< ベクトルより回転ベクトルを求める >>>
|	入力	v0,	v1 = ベクトル
|	出力	a	   = x,	y に回転角（ラジアン）		
*------------------------------------------------------------------------------*/
extern	void	calc_vec3_vec_to_vec_rotation(VEC3 *a, VEC3 *v0, VEC3 *v1);

/*------------------------------------------------------------------------------*
| <<< スクリーン座標から３Ｄのベクトルを作成 >>>
|	入力	pos		 = 2D 座標			※通常、pos.z は 0 で渡すこと
|			mtxWorld = ワールド行列
|	戻り値			   3D 座標
*------------------------------------------------------------------------------*/
extern	VEC3	calc_vec3_screen_to_vector(VEC3 *pos, MTX *mtxWorld);
extern	VEC3	calc_vec3_screen_to_vector(VEC3 *pos);

/*------------------------------------------------------------------------------*
| <<< ３Ｄベクトルからスクリーン座標を作成 >>>
|	入力	vecPos	 = 3D 座標
|			mtxWorld = ワールド行列
|	出力	vecOut	 = 変換後のベクトル（ 2D 座標 ）
*------------------------------------------------------------------------------*/
extern	void	calc_vec3_vector_to_screen(VEC3 *vecOut, VEC3 *vecPos, MTX *mtxWorld);
extern	void	calc_vec3_vector_to_screen(VEC3 *vecOut, VEC3 *vecPos);


// スプライン
extern	VEC2	*cacl_vec2_spline(VEC2 *pv0, const VEC2 *pv1, const VEC2 *pt1, const VEC2 *pv2, const VEC2 *pt2, float t);



#endif					/// end calc.h

