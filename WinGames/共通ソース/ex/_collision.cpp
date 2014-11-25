//
//	衝突判定
//



#define	EPSIRON		0.00001f // 誤差



/*------------------------------------------------------------------------------*
| <<< 点が面のどちら側にあるか調べる >>>
|	入力	pplane  = 面
|			pvecPos = 点
|	戻り値	0  < 裏側
|			0  > 表側
|			0 == 面の上
*------------------------------------------------------------------------------*/
float	collision_plane_point(PLANE *pplane, VEC3 *pvecPos)
{

	// 平面の法線と点ベクトルとの内積
	return calc_vec3_dot(&VEC3(pplane->a, pplane->b, pplane->c), pvecPos);
}



/*------------------------------------------------------------------------------*
| <<< 点が線分のどちら側にあるか調べる >>>
|	入力	pvecPoint = 調べたい座標
|			pvecLine0, pvecLine1 = 線
|	戻り値	-1 : 左側
|			 1 : 右側
|			 0 : 線の上
*------------------------------------------------------------------------------*/
int		collision_line_point(VEC2 *pvecPoint, VEC2 *pvecLine0, VEC2 *pvecLine1)
{
					// ↓外積の計算です
	float	fRet = ((pvecPoint->x - pvecLine0->x) * (pvecPoint->y - pvecLine1->y) - (pvecPoint->y - pvecLine0->y) * (pvecPoint->x - pvecLine1->x));
	if(fRet < 0){ return  1;}
	if(fRet > 0){ return -1;}
	return 0;
}



/*------------------------------------------------------------------------------*
| <<< 直線と線分との交差判定 >>>
|	入力	pvecLine0 - pvecLine1 = 調べたい直線
|			pvecRay0  - pvecRay1  = 交差判定をする線分
|	戻り値	TRUE	: 交差している
|			FALSE	: 交差していない
*------------------------------------------------------------------------------*/
BOOL	collision_line_ray(VEC2 *pvecLine0, VEC2 *pvecLine1, VEC2 *pvecRay0, VEC2 *pvecRay1)
{

	// (x0,y0)-(x1,y1)において、x0,y0が小さい値になるように並べ替え
	// 以下の矩形チェックに使うため、線の向きが変わっても気にしない
	float	fLineX0 = pvecLine0->x;
	float	fLineX1 = pvecLine1->x;
	if(fLineX0 > fLineX1){ SWAPF(fLineX0, fLineX1);}
	float	fLineY0 = pvecLine0->y;
	float	fLineY1 = pvecLine1->y;
	if(fLineY0 > fLineY1){ SWAPF(fLineY0, fLineY1);}

	float	fRayX0 = pvecRay0->x;
	float	fRayX1 = pvecRay1->x;
	if(fRayX0 > fRayX1){ SWAPF(fRayX0, fRayX1);}
	float	fRayY0 = pvecRay0->y;
	float	fRayY1 = pvecRay1->y;
	if(fRayY0 > fRayY1){ SWAPF(fRayY0, fRayY1);}

	//--- 調べる必要のないものはここではじく -----------------
	// 矩形を作り枠の外にあるかをチェック
	if(fLineX0 < fRayX0 && fLineX1 < fRayX0){ return FALSE;}
	if(fLineX0 > fRayX1 && fLineX1 > fRayX1){ return FALSE;}
	if(fLineY0 < fRayY0 && fLineY1 < fRayY0){ return FALSE;}
	if(fLineY0 > fRayY1 && fLineY1 > fRayY1){ return FALSE;}

	//if(pvecLine0->x < pvecRay0->x && pvecLine1->x < pvecRay0->x){ return FALSE;}
	//if(pvecLine0->x > pvecRay1->x && pvecLine1->x > pvecRay1->x){ return FALSE;}
	//if(pvecLine0->y < pvecRay0->y && pvecLine1->y < pvecRay0->y){ return FALSE;}
	//if(pvecLine0->y > pvecRay1->y && pvecLine1->y > pvecRay1->y){ return FALSE;}

	//--- 外積を調べる ---------------------------------------
	VEC2	v0 = *pvecRay1  - *pvecRay0;
	VEC2	v1 = *pvecLine0 - *pvecRay0;
	VEC2	v2 = *pvecLine1 - *pvecRay0;

	//float	a = D3DXVec2CCW(&v0, &v1);		// 戻り値が正なら左
	//float	b = D3DXVec2CCW(&v0, &v2);		// 戻り値が負なら右
	float	a = calc_vec2_cross(&v0, &v1);	// 戻り値が正なら左
	float	b = calc_vec2_cross(&v0, &v2);	// 戻り値が負なら右

	if(a == 0.f || b == 0.f){ return TRUE;}	// 直線の上
	if(a >  0.f && b <  0.f){ return TRUE;}	// 交差
	if(a <  0.f && b >  0.f){ return TRUE;}	// 交差
	return FALSE;
}



/*------------------------------------------------------------------------------*
| <<< 円と点の衝突判定 >>>
|	入力	pvecCircle, fR = 円の中心座標と半径
|			pvecPoint      = 点座標
|	戻り値	TRUE : ヒットしている
*------------------------------------------------------------------------------*/
BOOL	collision_circle_point(VEC2 *pvecCircle, float fR, VEC2 *pvecPoint)
{

	float	x = pvecCircle->x - pvecPoint->x;
	float	y = pvecCircle->y - pvecPoint->y;
	float	r = fR;
	return (x * x + y * y) < (r * r);
}



/*------------------------------------------------------------------------------*
| <<< 円と線の衝突判定 >>>
|	入力	pvecCircle, fR       = 円の中心座標と半径
|			pvecLine0, pvecLine1 = 直線の始点と終点
|	戻り値	TRUE : ヒットしている
*------------------------------------------------------------------------------*/
BOOL	collision_circle_line(VEC2 *pvecCircle, float fR, VEC2 *pvecLine0, VEC2 *pvecLine1)
{

	//--- 直線の端が円の範囲内ならばヒット --------------------------
	if(collision_circle_point(pvecCircle, fR, pvecLine0)){ return TRUE;}
	if(collision_circle_point(pvecCircle, fR, pvecLine1)){ return TRUE;}

	//--- 円の中点と直線までの距離を求める --------------------------
	// 点から直線に垂直な単位ベクトル(t1)を求める。
	VEC2	s1, s2, s3, t1, t2, v;

	// ベクトル line1 - line0
	s1.x = pvecLine1->x - pvecLine0->x;
	s1.y = pvecLine1->y - pvecLine0->y;

	// ベクトル cir - line0
	s2.x = pvecCircle->x - pvecLine0->x;
	s2.y = pvecCircle->y - pvecLine0->y;

	// ベクトル s1 と s2 の外積(２次元空間なのでＺ軸に平行なベクトル)
	float	z = s1.x * s2.y - s1.y * s2.x;

	// 円の中心が直線上に存在するとき外積は０になる
	if(z == 0)
	{
		v.x = pvecCircle->x;
		v.y = pvecCircle->y;
	}
	else
	{
		// ベクトル s1 と s3 の外積(line1 - line0 の垂直線になる)
		s3.x =  (s1.y * z);
		s3.y = -(s1.x * z);

		// 垂直線ベクトルの正規化
		calc_vec2_normalize(&t1, &s3);

		// 内積から距離を算出
		t2.x = pvecLine0->x - pvecCircle->x;
		t2.y = pvecLine0->y - pvecCircle->y;
		float	len = calc_vec2_dot(&t1, &t2);

		// 交点と円の中心の距離が半径以上なら衝突していない
		if(len > fR){ return FALSE;}

		// 円の中心から line1 - line0 に引いた垂直線の足の座標を求める
		v.x = pvecCircle->x + (t1.x * len);
		v.y = pvecCircle->y + (t1.y * len);
	}

	// v がライン間に衝突とみなす
	if((pvecLine0->x < v.x) && (v.x < pvecLine1->x)){ return TRUE;}
	if((pvecLine1->x < v.x) && (v.x < pvecLine0->x)){ return TRUE;}
	if((pvecLine0->y < v.y) && (v.y < pvecLine1->y)){ return TRUE;}
	if((pvecLine1->y < v.y) && (v.y < pvecLine0->y)){ return TRUE;}

	return FALSE;
}



/*------------------------------------------------------------------------------*
| <<< 円と円の衝突判定 >>>
|	入力	pvecCircleA, fRadiusA = 円の中心座標と半径
|			pvecCircleB, fRadiusB = 円の中心座標と半径
|	戻り値	TRUE : ヒットしている
*------------------------------------------------------------------------------*/
BOOL	collision_circle_circle(VEC2 *pvecCircleA, float fRadiusA, VEC2 *pvecCircleB, float fRadiusB)
{

	float	x = pvecCircleA->x - pvecCircleB->x;
	float	y = pvecCircleA->y - pvecCircleB->y;
	float	r = fRadiusA       + fRadiusB;
	return	(x * x + y * y) < (r * r);
}



/*------------------------------------------------------------------------------*
| <<< 四角形と円の衝突判定 >>>	※時計の反対周りで定義のこと
|	入力	pvecBox0～3 = 四角形の四頂点
|			pCircle		= 円の中心座標
|			fR			= 円の半径
|	戻り値	TRUE : ヒットしている
*------------------------------------------------------------------------------*/
BOOL	collision_box_circle(VEC2 *pvecBox0, VEC2 *pvecBox1, VEC2 *pvecBox2, VEC2 *pvecBox3, VEC2 *pvecCircle, float fR)
{

	// 外積を使い円の中心が四角形に含まれているか調べる
	if(collision_line_point(pvecCircle, pvecBox0, pvecBox1) == 1
	&& collision_line_point(pvecCircle, pvecBox1, pvecBox2) == 1
	&& collision_line_point(pvecCircle, pvecBox2, pvecBox3) == 1
	&& collision_line_point(pvecCircle, pvecBox3, pvecBox0) == 1)
	{
		return TRUE;
	}

	// 四角形の辺と円の交差判定
	if(collision_circle_line(pvecCircle, fR, pvecBox0, pvecBox1)){ return TRUE;}
	if(collision_circle_line(pvecCircle, fR, pvecBox1, pvecBox2)){ return TRUE;}
	if(collision_circle_line(pvecCircle, fR, pvecBox2, pvecBox3)){ return TRUE;}
	if(collision_circle_line(pvecCircle, fR, pvecBox3, pvecBox0)){ return TRUE;}

	return FALSE;
}



/*------------------------------------------------------------------------------*
| <<< 四角形と点の衝突判定 >>>	※時計の反対周りで定義のこと
|	入力	pvecBox0～3 = 四角形の四頂点
|			pPoint		= 円の中心座標
|			fR			= 円の半径
|	戻り値	TRUE : ヒットしている
*------------------------------------------------------------------------------*/
BOOL	collision_box_point(VEC2 *pvecBox0, VEC2 *pvecBox1, VEC2 *pvecBox2, VEC2 *pvecBox3, VEC2 *pvecPoint)
{

	// 外積を使い円の中心が四角形に含まれているか調べる
	if(collision_line_point(pvecPoint, pvecBox0, pvecBox1) == 1
	&& collision_line_point(pvecPoint, pvecBox1, pvecBox2) == 1
	&& collision_line_point(pvecPoint, pvecBox2, pvecBox3) == 1
	&& collision_line_point(pvecPoint, pvecBox3, pvecBox0) == 1)
	{
		return TRUE;
	}
	
	return FALSE;
}



/*------------------------------------------------------------------------------*
| <<< 四角形と四角形の衝突判定 >>>
|	入力	p0, p1 = 四角形の左上と右下
|			p2, p3 = 四角形の左上と右下
|	戻り値	TRUE : ヒットしている
*------------------------------------------------------------------------------*/
BOOL	collision_box_box(VEC2 *p0, VEC2 *p1, VEC2 *p2, VEC2 *p3)
{

	if(p0->x > p3->x){ return FALSE;}
	if(p1->x < p2->x){ return FALSE;}
	if(p0->y > p3->y){ return FALSE;}
	if(p1->y < p2->y){ return FALSE;}
	return TRUE;
}



/*------------------------------------------------------------------------------*
| <<< 四角形と線の衝突判定 >>>		※直線と線分との交差判定
|	入力	pvecBox0 ～ pvecBox3  = 四角形(時計回り)
|			pvecLine0 - pvecLine1 = 直線
|	戻り値	TRUE or FALSE
*------------------------------------------------------------------------------*/
BOOL	collision_box_line(VEC2 *pvecBox0,  VEC2 *pvecBox1, VEC2 *pvecBox2, VEC2 *pvecBox3,
					   VEC2 *pvecLine0, VEC2 *pvecLine1)
{

	//--- 直線の両端が矩形の中にあるか調べる ------------------------
	if(collision_box_point(pvecBox0, pvecBox1, pvecBox2, pvecBox3, pvecLine0)){ return TRUE;}
	if(collision_box_point(pvecBox0, pvecBox1, pvecBox2, pvecBox3, pvecLine1)){ return TRUE;}

	//--- 各辺との交差判定
	if(collision_line_ray(pvecBox0, pvecBox1, pvecLine0, pvecLine1)){ return TRUE;}
	if(collision_line_ray(pvecBox1, pvecBox2, pvecLine0, pvecLine1)){ return TRUE;}
	if(collision_line_ray(pvecBox2, pvecBox3, pvecLine0, pvecLine1)){ return TRUE;}
	if(collision_line_ray(pvecBox0, pvecBox3, pvecLine0, pvecLine1)){ return TRUE;}

	return	FALSE;
}



/*------------------------------------------------------------------------------*
| <<< 球と球の衝突判定 >>>
|	入力	pvecSphere			= 球0 の中心座標
|			fRadius				= 球0 の大きさ
|			pvecSphere			= 球1 の中心座標
|			fRadius				= 球1 の大きさ
|	戻り値	TRUE : ヒットしている
*------------------------------------------------------------------------------*/
BOOL	collision_sphere_sphere(VEC3 *pvecSphere0, float fRadius0, VEC3 *pvecSphere1, float fRadius1)
{

	if(calc_vec3_length(&VEC3(*pvecSphere0 - *pvecSphere1)) < fRadius0 + fRadius1){ return true;}
	else																		  { return false;}
}



/*------------------------------------------------------------------------------*
| <<< 球と線分の交差判定 >>>
|	入力	pvecSphere			= 球の中心座標
|			fRadius				= 球の大きさ
|			pvecLine0,pvecLine1 = 線分の始点と終点 
|	戻り値	TRUE : ヒットしている
*------------------------------------------------------------------------------*/
#if 0
BOOL	collision_sphere_line(VEC3 *pvecSphere, float fRadius, VEC3 *pvecLine0, VEC3 *pvecLine1)
{

	//--- 直線の端が球に含まれているかチェック ----------------------
	float	r  = fRadius;
	float	x0 = pvecSphere->x - pvecLine0->x;
	float	y0 = pvecSphere->y - pvecLine0->y;
	float	z0 = pvecSphere->z - pvecLine0->z;
	float	x1 = pvecSphere->x - pvecLine1->x;
	float	y1 = pvecSphere->y - pvecLine1->y;
	float	z1 = pvecSphere->z - pvecLine1->z;
	if(((x0 * x0) + (y0 * y0) + (z0 * z0)) < (r * r)){ return TRUE;}
	if(((x1 * x1) + (y1 * y1) + (z1 * z1)) < (r * r)){ return TRUE;}

	//--- 球の中点と直線までの距離を求める --------------------------

	// 点から直線に垂直な単位ベクトル(t1)を求める。
	VEC3	s1, s2, s3, s4, t1, t2, v;

	// ベクトル la - lb
	s1 = *pvecLine1 - *pvecLine0;

	// ベクトル la - cir
	s2 = *pvecSphere - *pvecLine0;

	// s1, s2 の外積 s3
	calc_vec3_cross(&s3, &s1, &s2);

	// 円の中心が線分上ではない？
	if(calc_vec3_length_sq(&s3) > 0.00001f)
	{
		// s1, s3 の外積 s4 = 垂直線
		calc_vec3_cross(&s4, &s1, &s3);

		// 正規化
		calc_vec3_normalize(&t1, &s4);

		// 内積から距離を算出
		t2 = *pvecLine0 - *pvecSphere;

		float	fLen = calc_vec3_dot(&t1, &t2);

		// 交点と円の中心の距離が半径以上ならＮＧ
		if(fLen > r){ return FALSE;}

		// 球の中心(cir)から直線(pLine->a, pLine->b)に引いた垂直線の足の座標(v)
		v = *pvecSphere + t1 * fLen;
	}
	// 中心が線分上
	else
	{
		v = *pvecSphere;
	}

	// v が pvecLine0 と pvecLine1 の間にあればＯＫ
	if((pvecLine0->x < v.x) && (v.x < pvecLine1->x)){ return TRUE;}
	if((pvecLine1->x < v.x) && (v.x < pvecLine0->x)){ return TRUE;}
	if((pvecLine0->y < v.y) && (v.y < pvecLine1->y)){ return TRUE;}
	if((pvecLine1->y < v.y) && (v.y < pvecLine0->y)){ return TRUE;}
	if((pvecLine0->z < v.z) && (v.z < pvecLine1->z)){ return TRUE;}
	if((pvecLine1->z < v.z) && (v.z < pvecLine0->z)){ return TRUE;}

	//if ( (pLine->Pt0.Px < v.x) && (v.x < pLine->Pt1.Px) ) return TRUE;
	//if ( (pLine->Pt1.Px < v.x) && (v.x < pLine->Pt0.Px) ) return TRUE;
	//if ( (pLine->Pt0.Py < v.y) && (v.y < pLine->Pt1.Py) ) return TRUE;
	//if ( (pLine->Pt1.Py < v.y) && (v.y < pLine->Pt0.Py) ) return TRUE;
	//if ( (pLine->Pt0.Pz < v.z) && (v.z < pLine->Pt1.Pz) ) return TRUE;
	//if ( (pLine->Pt1.Pz < v.z) && (v.z < pLine->Pt0.Pz) ) return TRUE;

	return FALSE;
}
#endif



/*------------------------------------------------------------------------------*
| <<< 球と線分の交差判定 >>>
|	入力	pvecSphere = 球の中心座標
|			fRadius	   = 球の大きさ
|			pvecLine   = 線分の始点
|			pvecDir	   = 線分の向き
|	戻り値	TRUE : ヒットしている
*------------------------------------------------------------------------------*/
BOOL	collision_sphere_ray(VEC3 *pvecSphere, float fR, VEC3 *pvecLine, VEC3 *pvecDir)
{

	VEC3	e = *pvecSphere;
	VEC3	v;
	D3DXVec3Normalize(&v, pvecDir);
	
	VEC3	r = *pvecLine - *pvecSphere;
	VEC3	a = D3DXVec3Dot(    &e, &v) * v - e;
	VEC3	c = r - D3DXVec3Dot(&r, &v) * v;

	float	fAlpha = D3DXVec3LengthSq(&a);
	float	fBeta  = D3DXVec3Dot(     &a, &c);
	float	fOmega = D3DXVec3LengthSq(&c) - fR * fR;

	// 衝突判定
	float	tmp = fBeta * fBeta - fAlpha * fOmega;

	// 衝突しないか止まっている場合は不正値を返す
	if(fabs(fAlpha) <= EPSIRON || tmp < 0)
	{
		// 既にめり込んでいる場合は衝突を返す
		if(fOmega < 0.0f){ return TRUE;}
		return FALSE;
	}

	// 現在の位置でめり込んでいるかチェック
	if(fOmega < 0.0f){ return TRUE;}

	// 指定の間では衝突しない
	return FALSE;
}



/*------------------------------------------------------------------------------*
| <<< レイと三角形の交差判定 >>>		※裏面は FALSE を返します
|	入力	pvecPos						 = 開始位置
|			pvecDir						 = 例の方向ベクトル
|			pvecTriA, pvecTriB, pvecTriC = 三角形の任意の頂点
|	出力	pvecOut						 = 交差座標
|	戻り値	TRUE : 衝突している
*------------------------------------------------------------------------------*/
BOOL	collision_triangle_ray(VEC3 *pvecOut, VEC3 *pvecPos, VEC3 *pvecDir, VEC3 *apvecTri)
{

	VEC3	vecP;							// *pvecDir と avecEdge[1](apvecTri[2] - apvecTri[0]) の外積
	VEC3	vecQ;
	VEC3	vecT;							// *pvecPos と avecTri[0] を結ぶベクトル
	float	fDet;							// avecEdge[0]((apvecTri[1] - apvecTri[0]) と *pvecDir の内積
	float	fU, fV;//, fT;

	//--- apvecEdge[0] を共有する２ベクトルを算出 -------------------
	VEC3	avecEdge[2];
	avecEdge[0] = apvecTri[1] - apvecTri[0];
	avecEdge[1] = apvecTri[2] - apvecTri[0];

	//--- U パラメーターを計算 --------------------------------------
	calc_vec3_cross(&vecP, pvecDir, &avecEdge[1]);

    // 内積が０に近い場合は三角形の平面にある
	fDet = calc_vec3_dot(&avecEdge[0], &vecP);
	if(fDet > 0)
	{
		vecT  = *pvecPos - apvecTri[0];
	}
	else
	{
		vecT  = apvecTri[0] - *pvecPos;
		fDet *= -1;
	}
	if(fDet < 0.0001f){ return FALSE;}

	// U パラメーターおよびテスト境界を計算
	fU = calc_vec3_dot(&vecT, &vecP);
	if((fU < 0.0f) || (fU > fDet)){ return FALSE;}

	//--- V パラメーター判定＆計算 ----------------------------------
	calc_vec3_cross(&vecQ, &vecT, &avecEdge[0]);

	// V パラメーターおよびテスト境界を計算
	fV = calc_vec3_dot(pvecDir, &vecQ);
	if((fV < 0.0f) || ((fU + fV) > fDet)){ return FALSE;}

	//--- 交点の計算 ------------------------------------------------
	if(pvecOut != NULL)
	{
		//fT				= calc_vec3_dot(&avecEdge[1], &vecQ);
		float	fDetInv = 1.0f / fDet;
		fU			   *= fDetInv;
		fV			   *= fDetInv;
		*pvecOut		= apvecTri[0] + (fU * (apvecTri[1] - apvecTri[0])) + (fV * (apvecTri[2] - apvecTri[0]));
	}

	//--- 表裏判定 --------------------------------------------------
	// 線分がまたがっているか判定をする
	PLANE	plane;
	calc_plane_create(&plane, &apvecTri[0], &apvecTri[1], &apvecTri[2]);
	VEC3	v = *pvecPos - apvecTri[0];
	float	a = collision_plane_point(&plane, &v);

	calc_vec3_normalize(&v, pvecDir);
	v		  = (*pvecPos + v * 10000.f) - apvecTri[0];
	float	b = collision_plane_point(&plane, &v);

	// またがっていない
	if(a >= 0 || b < 0){ return FALSE;}

	return TRUE;
}




#if 0


/**************************************************************************************************
	[[[	衝突関係関係 ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< 線と直方体の交差判定 >>>
|	入力	pStart - pEnd = 線分
|			pV0, pV1, pV2 = 三角形それぞれの頂点
|	出力	pOut		  = 交点出力用 (NULLなら出力しない)
|	戻り値	TRUE  : 交差した
|			FALSE : 交差していない
*------------------------------------------------------------------------------*/
BOOL	d3_collision_line_plane(VEC3 *pOut, VEC3 *pStart, VEC3 *pEnd, VEC3 *pV0, VEC3 *pV1, VEC3 *pV2)
{

	//D3_3DTRI_FILL(pP0, pP1, pP2, D3_COL(255, 255, 0, 255));

	VEC3		vecP;
	VEC3		vecQ;
	VEC3		vecT;
	float		fDet;
	float		fInvDet;
	float		fT, fU, fV;

	// 面の作成
	D3DXPLANE	plane;
	D3DXPlaneFromPoints(&plane,	pV0, pV1, pV2);

	// 線分の両端がが平面をまたいで存在しているかをチェック
	float	fDist0 = (plane.a * pStart->x) + (plane.b * pStart->y) + (plane.c * pStart->z) + plane.d;
	float	fDist1 = (plane.a * pEnd->x  ) + (plane.b * pEnd->y  ) + (plane.c * pEnd->z  ) + plane.d;
	if((fDist0 * fDist1) >= 0.0f) return FALSE;

	//VEC3	Start = VEC3(pStart->x, pStart->y, pStart->z);
	//VEC3	Dir   = VEC3(pEnd->x,   pEnd->y,   pEnd->z  ) - Start;
	VEC3	vecDir = *pEnd - *pStart;
	//VEC3	Vx[3];
	//Vx[0] = *pP0;
	//Vx[1] = *pP1;
	//Vx[2] = *pP2;

	// Vx[0]を共有する２ベクトルを算出
	//VEC3	Edge1 = Vx[1] - Vx[0];
	//VEC3	Edge2 = Vx[2] - Vx[0];
	VEC3	vecEdge0 = *pV1 - *pV0;
	VEC3	vecEdge1 = *pV2 - *pV0;

	// Uパラメーターを計算
	D3DXVec3Cross(&vecP, &vecDir, &vecEdge1);

    // 内積が０に近い場合は三角形の平面にある
	fDet = D3DXVec3Dot(&vecEdge0, &vecP);
	//if(fDet > 0) vecT = Start - Vx[0];
	if(fDet > 0) vecT = *pStart - *pV0;
	//else		 vecT = Vx[0] - Start, fDet = -fDet;
	else		 vecT = *pV0    - *pStart, fDet = -fDet;
	if(fDet < 0.0001f) return FALSE;

	// Uパラメーターおよびテスト境界を計算
	fU = D3DXVec3Dot(&vecT, &vecP);
	if((fU < 0.0f) || (fU > fDet)) return FALSE;

	// Vパラメーターをテストする準備をします
	D3DXVec3Cross(&vecQ, &vecT, &vecEdge0);

	// Vパラメーターおよびテスト境界を計算
	fV = D3DXVec3Dot(&vecDir, &vecQ);
	if((fV < 0.0f) || ((fU + fV) > fDet)) return FALSE;

	// 位置情報を保存
	if(pOut != NULL)
	{
		// Tを計算し、スケーリング
		fT      = D3DXVec3Dot(&vecEdge1, &vecQ);
		fInvDet = 1.0f / fDet;
		fT     *= fInvDet;
		fU     *= fInvDet;
		fV     *= fInvDet;
		//pOut->x = Vx[0].x + (fU * (Vx[1].x - Vx[0].x)) + (fV * (Vx[2].x - Vx[0].x));
		//pOut->y = Vx[0].y + (fU * (Vx[1].y - Vx[0].y)) + (fV * (Vx[2].y - Vx[0].y));
		//pOut->z = Vx[0].z + (fU * (Vx[1].z - Vx[0].z)) + (fV * (Vx[2].z - Vx[0].z));
		pOut->x = pV0->x + (fU * (pV1->x - pV0->x)) + (fV * (pV2->x - pV0->x));
		pOut->y = pV0->y + (fU * (pV1->y - pV0->y)) + (fV * (pV2->y - pV0->y));
		pOut->z = pV0->z + (fU * (pV1->z - pV0->z)) + (fV * (pV2->z - pV0->z));
	}
	return TRUE;
}


/*----------------------------------------------------------------------*
| <<< 点と直方体の交差判定 >>>
|	入力	a[8] = 直方体の頂点(頂点の並びは直方体の解説を参照のこと)
|			b    = 点の座標
|	戻り値	TRUE or FALSE
*----------------------------------------------------------------------*/
BOOL	d3_collision_point_cube(VEC3 *a, VEC3 *b)
{

	if(a[0].x <= b->x && a[1].x >= b->x
	&& a[0].y <= b->y && a[3].y >= b->y
	&& a[0].z <= b->z && a[4].z >= b->z)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}



/*----------------------------------------------------------------------*
| <<< 線分と直方体の交差判定 >>>
|	入力	a[8]   = 直方体の頂点(頂点の並びは直方体の解説を参照のこと)
|			b ～ c = ラインの座標
|	戻り値	TRUE or FALSE
*----------------------------------------------------------------------*/
BOOL	d3_collision_line_cube(VEC3 *a, VEC3 *b, VEC3 *c)
{

	BOOL	ret = FALSE;

	//--- とりあえず内包している頂点は TRUE とする ------------------
	if(a[0].x <= b->x && a[1].x >= b->x
	&& a[0].y <= b->y && a[3].y >= b->y
	&& a[0].z <= b->z && a[4].z >= b->z)
	{
		return TRUE;
	}
	if(a[0].x <= c->x && a[1].x >= c->x
	&& a[0].y <= c->y && a[3].y >= c->y
	&& a[0].z <= c->z && a[4].z >= c->z)
	{
		return TRUE;
	}

	//--- 横断している頂点を調べる ----------------------------------
											// 右面(向って右)
	if(d3_collision_line_plane(NULL, b, c, &a[1], &a[2], &a[6])) return TRUE;
	if(d3_collision_line_plane(NULL, b, c, &a[6], &a[5], &a[1])) return TRUE;
											// 左面(向って左)
	if(d3_collision_line_plane(NULL, b, c, &a[0], &a[3], &a[7])) return TRUE;
	if(d3_collision_line_plane(NULL, b, c, &a[7], &a[4], &a[0])) return TRUE;
											// 奥面
	if(d3_collision_line_plane(NULL, b, c, &a[4], &a[7], &a[6])) return TRUE;
	if(d3_collision_line_plane(NULL, b, c, &a[6], &a[5], &a[4])) return TRUE;
											// 手前面
	if(d3_collision_line_plane(NULL, b, c, &a[0], &a[1], &a[2])) return TRUE;
	if(d3_collision_line_plane(NULL, b, c, &a[2], &a[3], &a[0])) return TRUE;
											// 上面
	if(d3_collision_line_plane(NULL, b, c, &a[3], &a[2], &a[6])) return TRUE;
	if(d3_collision_line_plane(NULL, b, c, &a[6], &a[7], &a[3])) return TRUE;
											// 底面
	if(d3_collision_line_plane(NULL, b, c, &a[0], &a[1], &a[5])) return TRUE;
	if(d3_collision_line_plane(NULL, b, c, &a[5], &a[4], &a[0])) return TRUE;

	return FALSE;
}

#endif




/**************************************************************************************************
	[[[	視錘台 ]]]
**************************************************************************************************/

#if 0
/*----------------------------------------------------------------------*
| <<< 視錘台の初期化 >>>
|	※カメラが動くならば、毎周期実行のこと
*----------------------------------------------------------------------*/
void	d3_frustum_init(void)
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
int		d3_frustum_hit_check(D3_HIT *pHit, MTX *mWorld)
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
int		d3_frustum_hit_check(VEC3 *apvecPos, MTX *mtxWorld)
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
	if((adwOut[0] & adwOut[1] &	adwOut[2] & adwOut[3] & adwOut[4] & adwOut[5] &	adwOut[6] & adwOut[7]) != 0) { return 2;}

	return 1;
}

#endif

