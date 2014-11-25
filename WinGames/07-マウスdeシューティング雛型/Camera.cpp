//
// ‰J‚çˆ—
//

#include	"main.h"

/*-------------------------------------------------------------------*
| <<< ƒJƒƒ‰İ’è >>>
*--------------------------------------------------------------------*/
void	Camera(void)
{
									//‚Ç‚±			‚Ç‚±‚©‚ç	‚¤‚¦‚Í‚Ç‚¿‚ç‚©‚ç
	D3DXMatrixLookAtLH(&d3.mtxView, &d3.vecCamera, &d3.vecEye, &d3.vecUp);
												//«‹–ìŠp
	D3DXMatrixPerspectiveFovLH(&d3.mtxProjection, R(45), (float)__SCR_W / (float)__SCR_H, D3_VIEW_NEAR, D3_VIEW_FAR);
												// View s—ñ
	d3.dev->SetTransform(D3DTS_VIEW, &d3.mtxView);
												// Ë‰es—ñ
	d3.dev->SetTransform(D3DTS_PROJECTION,  &d3.mtxProjection);
												// ViewË‰es—ñ
	d3.mtxViewProjection = d3.mtxView * d3.mtxProjection;
												// ViewË‰e“]’us—ñ
	D3DXMatrixTranspose(&d3.mtxViewProjectionTranspose, &d3.mtxViewProjection);
}
