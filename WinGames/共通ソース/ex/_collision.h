//
//	衝突判定ヘッダ
//

#ifndef	__COLLISION_H

#define	__COLLISION_H


/*------------------------------------------------------------------------------*
| <<< 点が面のどちら側にあるか調べる >>>
|	入力	pplane  = 面
|			pvecPos = 点
|	戻り値	0  < 裏側
|			0  > 表側
|			0 == 面の上
*------------------------------------------------------------------------------*/
extern	float	collision_plane_point(PLANE *pplane, VEC3 *pvecPos);



/*------------------------------------------------------------------------------*
| <<< 点が線分のどちら側にあるか調べる >>>
|	入力	pvecPoint = 調べたい座標
|			pvecLine0, pvecLine1 = 線
|	戻り値	-1 : 左側
|			 1 : 右側
|			 0 : 線の上
*------------------------------------------------------------------------------*/
extern	int		collision_line_point(VEC2 *pvecPoint, VEC2 *pvecLine0, VEC2 *pvecLine1);

/*------------------------------------------------------------------------------*
| <<< 直線と線分との交差判定 >>>
|	入力	pvecLine0 - pvecLine1 = 調べたい直線
|			pvecRay0  - pvecRay1  = 交差判定をする線分	※ 処理を簡潔にするために p0 必ず左上、p1 は右下となるようにする
|	戻り値	TRUE	: 交差している
|			FALSE	: 交差していない
*------------------------------------------------------------------------------*/
extern	BOOL	collision_line_ray(VEC2 *pvecLine0, VEC2 *pvecLine1, VEC2 *pvecRay0, VEC2 *pvecRay1);


/*------------------------------------------------------------------------------*
| <<< 円と点の衝突判定 >>>
|	入力	pvecCircle, fR = 円の中心座標と半径
|			pvecPoint      = 点座標
|	戻り値	TRUE : ヒットしている
*------------------------------------------------------------------------------*/
extern	BOOL	collision_circle_point(VEC2 *pvecCircle, float fR, VEC2 *pvecPoint);

/*------------------------------------------------------------------------------*
| <<< 円と線の衝突判定 >>>
|	入力	pvecCircle, fR       = 円の中心座標と半径
|			pvecLine0, pvecLine1 = 直線の始点と終点
|	戻り値	TRUE : ヒットしている
*------------------------------------------------------------------------------*/
extern	BOOL	collision_circle_line(VEC2 *pvecCircle, float fR, VEC2 *pvecLine0, VEC2 *pvecLine1);

/*------------------------------------------------------------------------------*
| <<< 円と円の衝突判定 >>>
|	入力	pvecCircleA, fRadiusA = 円の中心座標と半径
|			pvecCircleB, fRadiusB = 円の中心座標と半径
|	戻り値	TRUE : ヒットしている
*------------------------------------------------------------------------------*/
extern	BOOL	collision_circle_circle(VEC2 *pvecCircleA, float fRadiusA, VEC2 *pvecCircleB, float fRadiusB);



/*------------------------------------------------------------------------------*
| <<< 四角形と円の衝突判定 >>>
|	入力	pvecBox0, pvecBox01 = 四角形の左上、右下
|			pvecCircle			= 円の中心座標
|			fR					= 円の半径
|	戻り値	TRUE : ヒットしている
*------------------------------------------------------------------------------*/
extern	BOOL	collision_box_circle(VEC2 *pvecBox0, VEC2 *pvecBox1, VEC2 *pvecBox2, VEC2 *pvecBox3, VEC2 *pvecCircle, float fR);

/*------------------------------------------------------------------------------*
| <<< 四角形と点の衝突判定 >>>	※時計の反対周りで定義のこと
|	入力	pvecBox0～3 = 四角形の四頂点
|			pvecPoint	= 点の座標
|	戻り値	TRUE : ヒットしている
*------------------------------------------------------------------------------*/
extern	BOOL	collision_box_point(VEC2 *pvecBox0, VEC2 *pvecBox1, VEC2 *pvecBox2, VEC2 *pvecBox3, VEC2 *pvecPoint);

/*------------------------------------------------------------------------------*
| <<< 四角形と四角形の衝突判定 >>>
|	入力	p0, p1 = 四角形の左上と右下
|			p2, p3 = 四角形の左上と右下
|	戻り値	TRUE : ヒットしている
*------------------------------------------------------------------------------*/
extern	BOOL	collision_box_box(VEC2 *p0, VEC2 *p1, VEC2 *p2, VEC2 *p3);

/*------------------------------------------------------------------------------*
| <<< 四角形と線の衝突判定 >>>		※直線と線分との交差判定
|	入力	pvecBox0 ～ pvecBox3  = 四角形(時計回り)
|			pvecLine0 - pvecLine1 = 直線
|	戻り値	TRUE or FALSE
*------------------------------------------------------------------------------*/
extern	BOOL	collision_box_line(VEC2 *pvecBox0,  VEC2 *pvecBox1, VEC2 *pvecBox2, VEC2 *pvecBox3, VEC2 *pvecLine0, VEC2 *pvecLine1);



/*------------------------------------------------------------------------------*
| <<< 球と球の衝突判定 >>>
|	入力	pvecSphere			= 球0 の中心座標
|			fRadius				= 球0 の大きさ
|			pvecSphere			= 球1 の中心座標
|			fRadius				= 球1 の大きさ
|	戻り値	TRUE : ヒットしている
*------------------------------------------------------------------------------*/
extern	BOOL	collision_sphere_sphere(VEC3 *pvecSphere0, float fRadius0, VEC3 *pvecSphere1, float fRadius1);

/*------------------------------------------------------------------------------*
| <<< 球と線分の交差判定 >>>
|	入力	pvecSphere			= 球の中心座標
|			fRadius				= 球の大きさ
|			pvecLine0,pvecLine1 = 線分の始点と終点 
|	戻り値	TRUE : ヒットしている
*------------------------------------------------------------------------------*/
#if 0
extern	BOOL	collision_sphere_line(VEC3 *pvecSphere, float fRadius, VEC3 *pvecLine0, VEC3 *pvecLine1);
#endif

/*------------------------------------------------------------------------------*
| <<< 球と線分の交差判定 >>>
|	入力	pvecSphere			= 球の中心座標
|			fRadius				= 球の大きさ
|			pvecLine0,pvecLine1 = 線分の始点と終点 
|	戻り値	TRUE : ヒットしている
*------------------------------------------------------------------------------*/
extern	BOOL	collision_sphere_ray(VEC3 *pvecPos, float fR, VEC3 *pvecLine, VEC3 *pvecDir);



/*------------------------------------------------------------------------------*
| <<< 線分と三角形の交差判定 >>>		※裏面は FALSE を返します
|	入力	pvecA, pvecB				 = 線分
|			pvecTriA, pvecTriB, pvecTriC = 三角形の任意の頂点
|	出力	pvecOut						 = 交差座標
|	戻り値	TRUE : 衝突している
*------------------------------------------------------------------------------*/
extern	BOOL	collision_triangle_ray(VEC3 *pvecOut, VEC3 *pvecPos, VEC3 *pvecDir, VEC3 *apvecTri);


#endif					// end _collision.h


