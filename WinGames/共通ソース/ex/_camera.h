

#ifndef	__CAMERA_
#define	__CAMERA_

//
//	カメラ処理
//

//--- 構造体定義 ----------------------------------------------------------------------------------
struct	CAMERA_WORK
{
	float	fMoveScale;						// 移動時のスケール
	float	fViewAngle;						// カメラの視野角
	float	fTheta,	fRho;					// カメラの回転角度
											// カメラの回転行列
	VEC3	vecRot;							// マップの回転値、移動値
	float	fOrthoScale;					// 0 以上ならば正射影行列になる
};


//--- 外部参照宣言 --------------------------------------------------------------------------------
extern	CAMERA_WORK	camera;

extern	void	camera_init(void);			// カメラ初期化
extern	void	camera_main(void);			// カメラメイン処理
extern	void	camera_move_main(void);		// デバッグカメラ移動


/*------------------------------------------------------------------------------*
| <<< デバッグカメラ、移動スケールセット >>>
|	入力	fScale = カメラの移動スケール
*------------------------------------------------------------------------------*/
inline	void	camera_move_scale_set(float fScale){ camera.fMoveScale = fScale;}



#endif			//	__CAMERA_


