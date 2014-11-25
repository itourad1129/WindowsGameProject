//
//	視錘台関係
//


#ifndef	__FRUSTUM


//--- ダミー --------------------------------------------------------------------------------------
void	frustum_init(void);
int		frustum_hit_check(D3_HIT *pHit, MTX *mWorld){     return 0;}
int		frustum_hit_check(VEC3 *apvecPos, MTX *mtxWorld){ return 0;}

//-------------------------------------------------------------------------------------------------

#else



/*----------------------------------------------------------------------*
| <<< 視錘台の初期化 >>>
|	※カメラが動くならば、毎周期実行のこと
*----------------------------------------------------------------------*/
void	frustum_init(void)
{
	MTX		mat;

	D3DXMatrixInverse(&mat,	NULL, &d3.mtxViewProjection);
	d3.vecFrustum[0] = VEC3(-1.0f, -1.0f, 0.0f);	// xyz
	d3.vecFrustum[1] = VEC3( 1.0f, -1.0f, 0.0f);	// Xyz
	d3.vecFrustum[2] = VEC3(-1.0f,  1.0f, 0.0f);	// xYz
	d3.vecFrustum[3] = VEC3( 1.0f,  1.0f, 0.0f);	// XYz
	d3.vecFrustum[4] = VEC3(-1.0f, -1.0f, 1.0f);	// xyZ
	d3.vecFrustum[5] = VEC3( 1.0f, -1.0f, 1.0f);	// XyZ
	d3.vecFrustum[6] = VEC3(-1.0f,  1.0f, 1.0f);	// xYZ
	d3.vecFrustum[7] = VEC3( 1.0f,  1.0f, 1.0f);	// XYZ

	for(int	i =	0; i < 8; i++){ D3DXVec3TransformCoord(&d3.vecFrustum[i], &d3.vecFrustum[i], &mat);}
											// 視錘台用の平面式を作る
	D3DXPlaneFromPoints(&d3.planeFrustum[0], &d3.vecFrustum[0],	&d3.vecFrustum[1], &d3.vecFrustum[2]);	// Near
	D3DXPlaneFromPoints(&d3.planeFrustum[1], &d3.vecFrustum[6],	&d3.vecFrustum[7], &d3.vecFrustum[5]);	// Far
	D3DXPlaneFromPoints(&d3.planeFrustum[2], &d3.vecFrustum[2],	&d3.vecFrustum[6], &d3.vecFrustum[4]);	// Left
	D3DXPlaneFromPoints(&d3.planeFrustum[3], &d3.vecFrustum[7],	&d3.vecFrustum[3], &d3.vecFrustum[5]);	// Right
	D3DXPlaneFromPoints(&d3.planeFrustum[4], &d3.vecFrustum[2],	&d3.vecFrustum[3], &d3.vecFrustum[6]);	// Top
	D3DXPlaneFromPoints(&d3.planeFrustum[5], &d3.vecFrustum[1],	&d3.vecFrustum[0], &d3.vecFrustum[4]);	// Bottom
}



/*------------------------------------------------------------------------------*
| <<< メッシュが視錐台の中にあるかチェック >>>
|	入力	pHit   = メッシュのヒットワーク
|			mWorld = メッシュのワールド座標
|	戻り値	0 :	視錐台の中
|			1 :	一部分が切れている
|			2 :	視錐台の外
*------------------------------------------------------------------------------*/
int		frustum_hit_check(D3_HIT *pHit, MTX *mWorld)
{

	for(int	i =	0; i < 8; i++) D3DXVec3TransformCoord(&pHit->vecWorld[i], &pHit->vecLocal[i], mWorld);
	pHit->vecWorld[8].x	= mWorld->_41;		// バウンディングボックスの中心点
	pHit->vecWorld[8].y	= mWorld->_42;
	pHit->vecWorld[8].z	= mWorld->_43;
	
	//--- 面の作成 --------------------------------------------------
	D3DXPlaneFromPoints(&pHit->planeWorld[0], &pHit->vecWorld[0], &pHit->vecWorld[1], &pHit->vecWorld[2]);	// Near
	D3DXPlaneFromPoints(&pHit->planeWorld[1], &pHit->vecWorld[6], &pHit->vecWorld[7], &pHit->vecWorld[5]);	// Far
	D3DXPlaneFromPoints(&pHit->planeWorld[2], &pHit->vecWorld[2], &pHit->vecWorld[6], &pHit->vecWorld[4]);	// Left
	D3DXPlaneFromPoints(&pHit->planeWorld[3], &pHit->vecWorld[7], &pHit->vecWorld[3], &pHit->vecWorld[5]);	// Right
	D3DXPlaneFromPoints(&pHit->planeWorld[4], &pHit->vecWorld[2], &pHit->vecWorld[3], &pHit->vecWorld[6]);	// Top
	D3DXPlaneFromPoints(&pHit->planeWorld[5], &pHit->vecWorld[1], &pHit->vecWorld[0], &pHit->vecWorld[4]);	// Bottom

	//---------------------------------------------------------------
	WORD	bOutside[8];
	int		out	= 0;

	ZeroMemory(&bOutside, sizeof(bOutside));
	// bitfieldの中の6つの円錐台平面、および結果(１の場合、外部)
	// すべてに対するチェック境界頂点
	float	aa[8];

	for(int	iPoint = 0;	iPoint < 8;	iPoint++)
	{
		for(int	iPlane = 0;	iPlane < 6;	iPlane++)
		{
			aa[iPoint] = 
			   d3.planeFrustum[iPlane].a * pHit->vecWorld[iPoint].x
			+  d3.planeFrustum[iPlane].b * pHit->vecWorld[iPoint].y
			+  d3.planeFrustum[iPlane].c * pHit->vecWorld[iPoint].z
			+  d3.planeFrustum[iPlane].d;

			if(d3.planeFrustum[iPlane].a * pHit->vecWorld[iPoint].x
			+  d3.planeFrustum[iPlane].b * pHit->vecWorld[iPoint].y
			+  d3.planeFrustum[iPlane].c * pHit->vecWorld[iPoint].z
			+  d3.planeFrustum[iPlane].d < 0)
			{
				bOutside[iPoint] |=	(1 << iPlane);
				out++;
			}
		}
	}
	if(out == 0){ return 0;}

	if((bOutside[0]	& bOutside[1] &	bOutside[2]	& bOutside[3]
	&	bOutside[4]	& bOutside[5] &	bOutside[6]	& bOutside[7]) != 0)
	{
		return 2;
	}
	return 1;
}



/*------------------------------------------------------------------------------*
| <<< 立方体の八頂点が視錐台の中にあるかチェック >>>
|	入力	apvecPos = 立方体の八頂点
|			mtxWorld = メッシュのワールド座標
|	戻り値	0 :	視錐台の中
|			1 :	一部分が切れている
|			2 :	視錐台の外
*------------------------------------------------------------------------------*/
int		frustum_hit_check(VEC3 *apvecPos, MTX *mtxWorld)
{

	//--- 立方体の八頂点をトランスフォーム --------------------------
	VEC3	avecP[8];
	for(int	i =	0; i < 8; i++) D3DXVec3TransformCoord(&avecP[i], &apvecPos[i], mtxWorld);

	//--- 箱の定義 --------------------------------------------------
	D3DXPLANE	planeWorld[6];
	D3DXPlaneFromPoints(&planeWorld[0], &avecP[0], &avecP[1], &avecP[2]);	// Near
	D3DXPlaneFromPoints(&planeWorld[1], &avecP[6], &avecP[7], &avecP[5]);	// Far
	D3DXPlaneFromPoints(&planeWorld[2], &avecP[2], &avecP[6], &avecP[4]);	// Left
	D3DXPlaneFromPoints(&planeWorld[3], &avecP[7], &avecP[3], &avecP[5]);	// Right
	D3DXPlaneFromPoints(&planeWorld[4], &avecP[2], &avecP[3], &avecP[6]);	// Top
	D3DXPlaneFromPoints(&planeWorld[5], &avecP[1], &avecP[0], &avecP[4]);	// Bottom

	//--- 判定 ------------------------------------------------------
	WORD	adwOut[8];
	int		iOut = 0;

	ZeroMemory(&adwOut, sizeof(adwOut));

	// bitfieldの中の6つの円錐台平面、および結果(１の場合、外部)
	// すべてに対するチェック境界頂点
	//float	aa[8];

	for(int	iPoint = 0;	iPoint < 8;	iPoint++)
	{
		for(int	iPlane = 0;	iPlane < 6;	iPlane++)
		{
	/*		aa[iPoint] = 
			   d3.planeFrustum[iPlane].a * avecP[iPoint].x
			+  d3.planeFrustum[iPlane].b * avecP[iPoint].y
			+  d3.planeFrustum[iPlane].c * avecP[iPoint].z
			+  d3.planeFrustum[iPlane].d;
*/
			if(d3.planeFrustum[iPlane].a * avecP[iPoint].x
			+  d3.planeFrustum[iPlane].b * avecP[iPoint].y
			+  d3.planeFrustum[iPlane].c * avecP[iPoint].z
			+  d3.planeFrustum[iPlane].d < 0)
			{
				adwOut[iPoint] |= (1 << iPlane);
				iOut++;
			}
		}
	}
	if(iOut == 0){ return 0;}
	if((adwOut[0] & adwOut[1] &	adwOut[2] & adwOut[3] & adwOut[4] & adwOut[5] &	adwOut[6] & adwOut[7]) != 0){ return 2;}

	return 1;
}



#if 0
	/*------------------------------------------------------------------------------*
	| <<< 直方体が視錐台上にあるかチェック >>>
	|	入力	a, b, c, d = 調べたいポイントの番号
	|			matWorld   = ワールド行列
	|	戻り値	0 :	表示ＯＫ
	|			1 :	マップが一部分切れている
	|			2 :	マップが隠れている
	*------------------------------------------------------------------------------*/
	#define	__MIN4(ans,	a, b, c, d)	{ans = a; if(ans > b) ans =	b; if(ans >	c) ans = c;	if(ans > d)	ans	= d;}
	#define	__MAX4(ans,	a, b, c, d)	{ans = a; if(ans < b) ans =	b; if(ans <	c) ans = c;	if(ans < d)	ans	= d;}

	//int		d3_hit_view_check(VEC3 *p0, VEC3 *p1, VEC3 *p2,	VEC3 *p3, MTX *matWorld)
	int		d3_frustum_hit_check(VEC3 *p0, VEC3 *p1, VEC3 *p2,	VEC3 *p3, MTX *matWorld)
	{

		VEC3		vecWorld[  8], vecLocal[8];
		D3DXPLANE	planeWorld[6];
		float		min_x, max_x, min_y, max_y,	min_z, max_z;

		__MIN4(min_x, p0->x, p1->x,	p2->x, p3->x);
		__MAX4(max_x, p0->x, p1->x,	p2->x, p3->x);
		__MIN4(min_y, p0->y, p1->y,	p2->y, p3->y);
		__MAX4(max_y, p0->y, p1->y,	p2->y, p3->y);
		__MIN4(min_z, p0->z, p1->z,	p2->z, p3->z);
		__MAX4(max_z, p0->z, p1->z,	p2->z, p3->z);
		vecLocal[0]	= VEC3(min_x, min_y, min_z);
		vecLocal[1]	= VEC3(max_x, min_y, min_z);
		vecLocal[2]	= VEC3(min_x, max_y, min_z);
		vecLocal[3]	= VEC3(max_x, max_y, min_z);
		vecLocal[4]	= VEC3(min_x, min_y, max_z);
		vecLocal[5]	= VEC3(max_x, min_y, max_z);
		vecLocal[6]	= VEC3(min_x, max_y, max_z);
		vecLocal[7]	= VEC3(max_x, max_y, max_z);
		for(int	i =	0; i < 8; i++) D3DXVec3TransformCoord(&vecWorld[i],	&vecLocal[i], matWorld);

		//--- 箱の定義 ------------------------------------------------
		D3DXPlaneFromPoints(&planeWorld[0],	&vecWorld[0], &vecWorld[1],	&vecWorld[2]);	// Near
		D3DXPlaneFromPoints(&planeWorld[1],	&vecWorld[6], &vecWorld[7],	&vecWorld[5]);	// Far
		D3DXPlaneFromPoints(&planeWorld[2],	&vecWorld[2], &vecWorld[6],	&vecWorld[4]);	// Left
		D3DXPlaneFromPoints(&planeWorld[3],	&vecWorld[7], &vecWorld[3],	&vecWorld[5]);	// Right
		D3DXPlaneFromPoints(&planeWorld[4],	&vecWorld[2], &vecWorld[3],	&vecWorld[6]);	// Top
		D3DXPlaneFromPoints(&planeWorld[5],	&vecWorld[1], &vecWorld[0],	&vecWorld[4]);	// Bottom
		
		//---------------------------------------------------------------
		BYTE	bOutside[8];
		int		out	= 0;
		ZeroMemory(&bOutside, sizeof(bOutside));

		// bitfieldの中の6つの円錐台平面、および結果(１の場合、外部)
		// すべてに対するチェック境界頂点
		for(int	iPoint = 0;	iPoint < 8;	iPoint++)
		{
			for(int	iPlane = 0;	iPlane < 6;	iPlane++)
			{
				if(d3.planeFrustum[iPlane].a * vecWorld[iPoint].x
				+  d3.planeFrustum[iPlane].b * vecWorld[iPoint].y
				+  d3.planeFrustum[iPlane].c * vecWorld[iPoint].z
				+  d3.planeFrustum[iPlane].d < 0)
				{
					bOutside[iPoint] |=	(1 << iPlane);
					out++;
				}
			}
		}
		if(out == 0) return	0;
		if((bOutside[0]	& bOutside[1] &	bOutside[2]	& bOutside[3]
		&&	bOutside[4]	& bOutside[5] &	bOutside[6]	& bOutside[7]) != 0) return	2;
		return 1;
	}



	/*------------------------------------------------------------------------------*
	| <<< d3_frustum_hit_check で利用するボックスを描画する >>>
	|	入力	a, b, c, d = 調べたいポイントの番号
	|			matWorld   = ワールド行列
	*------------------------------------------------------------------------------*/
	//void	d3_hit_view_check_draw(VEC3 *p0, VEC3 *p1, VEC3	*p2, VEC3 *p3, MTX *matWorld)
	void	d3_frustum_box_draw(VEC3 *p0, VEC3 *p1, VEC3 *p2, VEC3 *p3, MTX *matWorld)
	{
		VEC3		vecWorld[  8], vecLocal[8];
		D3DXPLANE	planeWorld[6];
		float		min_x, max_x, min_y, max_y,	min_z, max_z;


   		__MIN4(min_x, p0->x, p1->x,	p2->x, p3->x);
		__MAX4(max_x, p0->x, p1->x,	p2->x, p3->x);
		__MIN4(min_y, p0->y, p1->y,	p2->y, p3->y);
		__MAX4(max_y, p0->y, p1->y,	p2->y, p3->y);
		__MIN4(min_z, p0->z, p1->z,	p2->z, p3->z);
		__MAX4(max_z, p0->z, p1->z,	p2->z, p3->z);
		vecLocal[0]	= VEC3(min_x, min_y, min_z);
		vecLocal[1]	= VEC3(max_x, min_y, min_z);
		vecLocal[2]	= VEC3(min_x, max_y, min_z);
		vecLocal[3]	= VEC3(max_x, max_y, min_z);
		vecLocal[4]	= VEC3(min_x, min_y, max_z);
		vecLocal[5]	= VEC3(max_x, min_y, max_z);
		vecLocal[6]	= VEC3(min_x, max_y, max_z);
		vecLocal[7]	= VEC3(max_x, max_y, max_z);
		for(int	i =	0; i < 8; i++) D3DXVec3TransformCoord(&vecWorld[i],	&vecLocal[i], matWorld);

		//--- ボックス描画 --------------------------------------------
		DWORD	c =	D3DCOLOR_ARGB(255, 0, 255, 0);
		D3_3DLINE(&vecWorld[0], &vecWorld[1], c);
		D3_3DLINE(&vecWorld[2], &vecWorld[0], c);
		D3_3DLINE(&vecWorld[2], &vecWorld[3], c);
		D3_3DLINE(&vecWorld[3], &vecWorld[1], c);

		D3_3DLINE(&vecWorld[4], &vecWorld[5], c);
		D3_3DLINE(&vecWorld[6], &vecWorld[4], c);
		D3_3DLINE(&vecWorld[6], &vecWorld[7], c);
		D3_3DLINE(&vecWorld[7], &vecWorld[5], c);

		D3_3DLINE(&vecWorld[4], &vecWorld[0], c);
		D3_3DLINE(&vecWorld[6], &vecWorld[2], c);
	}
#endif


#endif				// end of __FRUSTUM