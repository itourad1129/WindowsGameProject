//
// 雨ら処理
//

#include	"main.h"

/*-------------------------------------------------------------------*
| <<< カメラ設定 >>>
*--------------------------------------------------------------------*/
void	Camera(void)
{
									//どこ			どこから	うえはどちらから
	D3DXMatrixLookAtLH(&d3.mtxView, &d3.vecCamera, &d3.vecEye, &d3.vecUp);
												//↓視野角
	D3DXMatrixPerspectiveFovLH(&d3.mtxProjection, R(45), (float)__SCR_W / (float)__SCR_H, D3_VIEW_NEAR, D3_VIEW_FAR);
												// View 行列
	d3.dev->SetTransform(D3DTS_VIEW, &d3.mtxView);
												// 射影行列
	d3.dev->SetTransform(D3DTS_PROJECTION,  &d3.mtxProjection);
												// View射影行列
	d3.mtxViewProjection = d3.mtxView * d3.mtxProjection;
												// View射影転置行列
	D3DXMatrixTranspose(&d3.mtxViewProjectionTranspose, &d3.mtxViewProjection);

	//--- 以下、パーティクルの時に必要になります。--------------------
	// View 逆行列を求めている
	float		tmp;
	D3DXMatrixInverse(&d3.mtxViewInverse, &tmp, &d3.mtxView);
	d3.mtxBillboard = d3.mtxViewInverse;
}