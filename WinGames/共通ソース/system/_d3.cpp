//
//	DirectGraphics 関係のプログラム
//

//--- このファイルに含まれる処理 ------------------------------------
//	[[[	２Ｄ描画系 ]]]
//	[[[	３Ｄ描画系 ]]]
//	[[[	スプライト系 ]]]
//	[[[	レンダリング系 ]]]
//	[[[	文字系 ]]]
//	[[[	ＢＭＰ系 ]]]
//	[[[	テクスチャー系 ]]]
//	[[[	オフスクリーン系 ]]]
//	[[[	初期化、終了系 ]]]

												// 最適化スイッチ
#define	OPTIMIZE	 D3DXMESHOPT_ATTRSORT
//D3DXMESHOPT_COMPACT	| D3DXMESHOPT_ATTRSORT;// |	D3DXMESHOPT_VERTEXCACHE

//--- ライブラリィ --------------------------------------------------------------------------------
//d3d9.lib d3dx9.lib  dxguid.lib d3dxof.lib
#pragma	comment(lib, "d3d9.lib")
#pragma	comment(lib, "d3dx9.lib")
#pragma	comment(lib, "dxguid.lib")
#pragma	comment(lib, "d3dxof.lib")


//--- DirectX 管理用 ------------------------------------------------------------------------------
D3_WORK	d3;


//--- バンプのフォーマット ------------------------------------------------------------------------
D3DVERTEXELEMENT9	d3_decl_bump[] =
{
	{0,	 0,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0,	12,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	  0},
	{0,	24,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0},
	{0,	36,	D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	D3DDECL_END()
};



/**************************************************************************************************
	[[[	２Ｄ描画系 ]]]
**************************************************************************************************/


/*------------------------------------------------------------------------------*
| <<< ２Ｄの円を描く >>>
|	入力	pvecPos = 表示座標
|			fR		= 大きさ
|			ucDiv	= 分割数(3 ～ 36)
|			dwCol	= 色
*------------------------------------------------------------------------------*/
void	d3_2d_circle_draw(VEC3 *pvecPos, float fR, uint uiDiv, DWORD dwCol)
{

	D3_VTX_TL_	vt[36 + 1];
	float		dr = D3DX_PI / (uiDiv / 2.f);
	uint		i;
	for(i =	0; i < uiDiv; i++)
	{
		float	s, c;
		SINCOS(dr *	(uiDiv - i), &s, &c);
		#ifdef	__2D_RESIZE
			vt[i].x = D3_CONV_X(pvecPos->x + fR * c);
			vt[i].y = D3_CONV_Y(pvecPos->y + fR * s);
		#else
			vt[i].x = pvecPos->x + fR * c;
			vt[i].y = pvecPos->y + fR * s;
		#endif
		vt[i].z   = pvecPos->z;
		vt[i].rhw =	1.0f;
		vt[i].c	  = dwCol;
	}
	vt[i] =	vt[0];							// iDiv 個目、円を閉じる
	d3_tex_set(-1);
	D3_RENDER(D3DPT_LINESTRIP, vt, TL, uiDiv);
	d3.iPolyCount += uiDiv;
}
void	d3_2d_circle_draw(VEC2 *pvecPos, float fR, uint uiDiv, DWORD dwCol)
{
	d3_2d_circle_draw(&VEC3(pvecPos->x, pvecPos->y, 0), fR, uiDiv, dwCol);
}
void	d3_2d_circle_draw(VEC2 *pvecPos, float fR, DWORD dwCol)
{
	d3_2d_circle_draw(&VEC3(pvecPos->x, pvecPos->y, 0), fR, 32, dwCol);
}


/*------------------------------------------------------------------------------*
| <<< ２Ｄの円を描き、塗りつぶす >>>
|	入力	pvecPos = 表示座標
|			fR		= 大きさ
|			uiDiv	= 分割数(3 ～ 36)
|			dwCol	= 色
*------------------------------------------------------------------------------*/
void	d3_2d_circle_fill_draw(VEC3 *pvecPos, float fR, uint uiDiv, DWORD dwCol)
{

	D3_VTX_TL_	vt[36 +	2];

	#ifdef	__2D_RESIZE
		vt[0].x	  =	D3_CONV_X(pvecPos->x);
		vt[0].y	  =	D3_CONV_Y(pvecPos->y);
		vt[0].z	  =	pvecPos->z;
	#else
		vt[0].x	  =	pvecPos->x;
		vt[0].y	  =	pvecPos->y;
		vt[0].z	  =	pvecPos->z;
	#endif

	vt[0].rhw =	1.0f;
	vt[0].c	  =	dwCol;
	uint	i;
	float	dr = D3DX_PI / (uiDiv / 2.f);
	for(i =	0; i < uiDiv; i++)
	{
		float	s, c;
		SINCOS(dr *	(float)i, &s, &c);
		#ifdef	__2D_RESIZE
			vt[i + 1].x	= D3_CONV_X(pvecPos->x + fR *  s);
			vt[i + 1].y	= D3_CONV_Y(pvecPos->y + fR * -c);
		#else
			vt[i + 1].x	= pvecPos->x + fR *  s;
			vt[i + 1].y	= pvecPos->y + fR * -c;
		#endif
		vt[i + 1].z	  =	pvecPos->z;
		vt[i + 1].rhw =	1.0f;
		vt[i + 1].c	  =	dwCol;
	}
	vt[i + 1] =	vt[1];							// iDiv 個目、円を閉じる

	d3_tex_set(-1);
	D3_RENDER(D3DPT_TRIANGLEFAN, vt, TL, uiDiv);
	d3.iPolyCount += uiDiv;
}
void	d3_2d_circle_fill_draw(VEC2 *pvecPos, float fR, uint uiDiv, DWORD dwCol)
{
	d3_2d_circle_fill_draw(&VEC3(pvecPos->x, pvecPos->y, 0), fR, uiDiv, dwCol);
}
void	d3_2d_circle_fill_draw(VEC2 *pvecPos, float fR, DWORD dwCol)
{
	d3_2d_circle_fill_draw(&VEC3(pvecPos->x, pvecPos->y, 0), fR, 32, dwCol);
}



/*------------------------------------------------------------------------------*
| <<< ２Ｄ四角形塗りつぶし描画 >>>
|	入力	(iX0, iY0)-(iX1,iY1) = 四角形の左上と右下
|			fZ					 = Z 位置
|			dwCol				 = 塗りつぶし色
*------------------------------------------------------------------------------*/
void	d3_2d_box_fill_draw(int iX0, int iY0, int iX1, int iY1, float fZ, DWORD dwCol)
{

	#ifdef	__2D_RESIZE
		float	x0 = D3_CONV_X(iX0);
		float	x1 = D3_CONV_X(iX1);
		float	y0 = D3_CONV_Y(iY0);
		float	y1 = D3_CONV_Y(iY1);
		D3_VTX_TL_	vt[] =
		{
			x0, y0, fZ, 1, dwCol,
			x1, y0, fZ, 1, dwCol,
			x1, y1, fZ, 1, dwCol,
			x0, y1, fZ, 1, dwCol
		};
	#else
		D3_VTX_TL_	vt[] =
		{
			(float)iX0, (float)iY0, fZ, 1, dwCol,
			(float)iX1, (float)iY0, fZ, 1, dwCol,
			(float)iX1, (float)iY1, fZ, 1, dwCol,
			(float)iX0, (float)iY1, fZ, 1, dwCol
		};
	#endif
	d3_tex_set(-1);
	D3_RENDER(D3DPT_TRIANGLEFAN, vt, TL, 2);
	d3.iPolyCount += 2;
}



/*------------------------------------------------------------------------------*
| <<< ２Ｄ四角形テクスチャー貼り付け描画 >>>
|	入力	(iX0,iY0)～(iX3,iY3) = 四角形の四頂点
|			(iU0,iV0)～(iU3,iV3) = UV の四頂点
|			uiBlendMode 		= 合成モード
|			uiAlp				= (0 ～ 255)半透明値
*------------------------------------------------------------------------------*/
void	d3_2d_box_fill_tex_draw(VEC2 *pvecP, VEC2 *pvecUV, uint uiBlendType, uint uiAlp)
{

	d3_light_set(false);

	DWORD	dwCol = D3_COL(uiAlp, 255, 255, 255);

	#ifdef	__2D_RESIZE
		float	x0 = D3_CONV_X(pvecP[0].x);
		float	y0 = D3_CONV_Y(pvecP[0].y);
		float	x1 = D3_CONV_X(pvecP[1].x);
		float	y1 = D3_CONV_Y(pvecP[1].y);
		float	x2 = D3_CONV_X(pvecP[2].x);
		float	y2 = D3_CONV_Y(pvecP[2].y);
		float	x3 = D3_CONV_X(pvecP[3].x);
		float	y3 = D3_CONV_Y(pvecP[3].y);
		D3_VTX_TLX_	vt[] =
		{
			x0, y0, 0, 1, dwCol, pvecUV[0].x, pvecUV[0].y,
			x1, y1, 0, 1, dwCol, pvecUV[1].x, pvecUV[1].y,
			x2, y2, 0, 1, dwCol, pvecUV[2].x, pvecUV[2].y,
			x3, y3, 0, 1, dwCol, pvecUV[3].x, pvecUV[3].y,
		};
	#else
		D3_VTX_TLX_	vt[] =
		{
			pvecP[0].x, pvecP[0].y, 0, 1, dwCol, pvecUV[0].x, pvecUV[0].y,
			pvecP[1].x, pvecP[1].y, 0, 1, dwCol, pvecUV[1].x, pvecUV[1].y,
			pvecP[2].x, pvecP[2].y, 0, 1, dwCol, pvecUV[2].x, pvecUV[2].y,
			pvecP[3].x, pvecP[3].y, 0, 1, dwCol, pvecUV[3].x, pvecUV[3].y,
		};
	#endif

	d3_stat_transform_set(&d3.mtxIdentity);
	D3_POLY_RENDER(TRIANGLEFAN, vt, TLX, 2);
	d3_light_set(true);
}



/*------------------------------------------------------------------------------*q
| <<< ２Ｄ四角形テクスチャー貼り付け描画 >>>
|	入力	(iX0,iY0)-(iX1,iY1) = 時計周りに四頂点
|			bAlphaGreat			= true : 透明部分を切り抜きます
|			uiAlp				= アルファ値(0 ～ 255)
*------------------------------------------------------------------------------*/
void	d3_2d_box_fill_tex_draw(int iX0, int iY0, int iX1, int iY1, int iZ, RECT *prect, uint uiBlendType, uint uiAlp)
{

	DWORD	dwCol  = D3_COL_WHITE(uiAlp);

	//--- UV を求める -----------------------------------------------
	float	u0, v0, u1, v1;
	if(prect != NULL)
	{							// ↓ 0.5f を足したり引いたりで UV のつなぎ目を消去してます
		u0 = ((float)prect->left   + 0.5f) / d3.texInfo[d3.iTexCurrent].fWidth;
		v0 = ((float)prect->top    + 0.5f) / d3.texInfo[d3.iTexCurrent].fHeight;
		u1 = ((float)prect->right  - 0.5f) / d3.texInfo[d3.iTexCurrent].fWidth;
		v1 = ((float)prect->bottom - 0.5f) / d3.texInfo[d3.iTexCurrent].fHeight;
	}
	else
	{
		u0 = 0.5f / d3.texInfo[d3.iTexCurrent].fWidth;
		v0 = 0.5f / d3.texInfo[d3.iTexCurrent].fHeight;
		u1 = (d3.texInfo[d3.iTexCurrent].fOriWidth  - 0.5f) / d3.texInfo[d3.iTexCurrent].fWidth;
		v1 = (d3.texInfo[d3.iTexCurrent].fOriHeight - 0.5f) / d3.texInfo[d3.iTexCurrent].fHeight;
	}

	#ifdef	__2D_RESIZE
		float	x0 = D3_CONV_X(iX0);
		float	x1 = D3_CONV_X(iX1);
		float	y0 = D3_CONV_Y(iY0);
		float	y1 = D3_CONV_Y(iY1);
		D3_VTX_TLX_	vt[] =
		{
			x0, y0, 0, 1, dwCol,  u0, v0,
			x1, y0, 0, 1, dwCol,  u1, v0,
			x1, y1, 0, 1, dwCol,  u1, v1,
			x0, y1, 0, 1, dwCol,  u0, v1
		};
	#else
		D3_VTX_TLX_	vt[] =
		{
			D3_CONV_X(iX0), D3_CONV_Y(iY0), 0, 1, dwCol,  u0, v0,
			D3_CONV_X(iX1), D3_CONV_Y(iY0), 0, 1, dwCol,  u1, v0,
			D3_CONV_X(iX1), D3_CONV_Y(iY1), 0, 1, dwCol,  u1, v1,
			D3_CONV_X(iX0), D3_CONV_Y(iY1), 0, 1, dwCol,  u0, v1
		};
	#endif
	D3_RENDER(D3DPT_TRIANGLEFAN, vt, TLX, 2);
	d3.iPolyCount += 2;
}

void	d3_2d_box_fill_tex_draw(int iX0, int iY0, int iX1, int iY1, int iZ, BOOL bAlphaGreat, uint uiAlp)
{
	d3_2d_box_fill_tex_draw(iX0, iY0, iX1, iY1, iZ, NULL, D3_BLEND_NORMAL, uiAlp);
}



/*------------------------------------------------------------------------------*
| <<< ２Ｄ四角形テクスチャー貼り付け描画 >>>
|	入力	(iX0,iY0),(iX1,iY1),(iX2,iY2),(iX3,iY3) = 時計周りに四頂点
|			bAlphaGreat								= true : 透明部分を切り抜きます
|			uiAlp									= アルファ値(0 ～ 255)
*------------------------------------------------------------------------------*/
void	d3_2d_box_fill_tex_draw(int iX0, int iY0, int iX1, int iY1, int iX2, int iY2, int iX3, int iY3, int iZ, BOOL bAlphaGreat, uint uiAlp)
{

	DWORD	dwCol = D3_COL_WHITE(uiAlp);

	#ifdef	__2D_RESIZE
		float	x0 = D3_CONV_X(iX0), y0 = D3_CONV_Y(iY0);
		float	x1 = D3_CONV_X(iX1), y1 = D3_CONV_Y(iY1);
		float	x2 = D3_CONV_X(iX2), y2 = D3_CONV_Y(iY2);
		float	x3 = D3_CONV_X(iX3), y3 = D3_CONV_Y(iY3);
		D3_VTX_TLX_	vt[] =
		{
			x0, y0, 0, 1, dwCol, 0, 0,
			x1, y1, 0, 1, dwCol, 1, 0,
			x2, y2, 0, 1, dwCol, 1, 1,
			x3, y3, 0, 1, dwCol, 0, 1,
		};
	#else
		D3_VTX_TLX_	vt[] =
		{
			(float)iX0, (float)iY0, 0, 1, dwCol, 0, 0,
			(float)iX1, (float)iY1, 0, 1, dwCol, 1, 0,
			(float)iX2, (float)iY2, 0, 1, dwCol, 1, 1,
			(float)iX3, (float)iY3, 0, 1, dwCol, 0, 1,
		};
	#endif

	D3_RENDER(D3DPT_TRIANGLEFAN, vt, TLX, 2);
	d3.iPolyCount += 2;
}




/*----------------------------------------------------------------------*
| <<< ２Ｄ三角形の描画 >>>
|	入力	pvecPos0, pvecPos1, pvecPos2 = 座標
|			dwCol						 = 色
*----------------------------------------------------------------------*/
void	d3_2d_triangle_fill_draw(VEC3 *pvecP0, VEC3 *pvecP1, VEC3 *pvecP2, DWORD dwCol)
{

	#ifdef	__2D_RESIZE
		float	x0 = D3_CONV_X(pvecP0->x);
		float	y0 = D3_CONV_Y(pvecP0->y);
		float	x1 = D3_CONV_X(pvecP1->x);
		float	y1 = D3_CONV_Y(pvecP1->y);
		float	x2 = D3_CONV_X(pvecP2->x);
		float	y2 = D3_CONV_Y(pvecP2->y);
		D3_VTX_TL_	vt[] =
		{
			x0, y0, 0, 1, dwCol,
			x1, y1, 0, 1, dwCol,
			x2, y2, 0, 1, dwCol
		};
	#else
		D3_VTX_TL_	vt[] =
		{
			pvecP0->x, pvecP0->y, 0, 1,	dwCol,
			pvecP1->x, pvecP1->y, 0, 1,	dwCol,
			pvecP2->x, pvecP2->y, 0, 1,	dwCol
		};
	#endif
	d3_tex_set(-1);
	D3_RENDER(D3DPT_TRIANGLELIST, vt, TL, 1);
	d3.iPolyCount++;
}



/*----------------------------------------------------------------------*
| <<< ２Ｄ三角形描画 >>>
|	入力	pvecPos0, pvecPos1, pvecPos2 = 座標
|			dwCol						 = 色
*----------------------------------------------------------------------*/
void	d3_2d_triangle_fill_draw(VEC3 *pvecP0, VEC3 *pvecP1, VEC3 *pvecP2, COL4 *pCol)
{

	float	a = pCol->a * 255.f; if(a > 255.f){ a = 255.f;}
	float	r = pCol->r * 255.f; if(r > 255.f){ r = 255.f;}
	float	g = pCol->g * 255.f; if(g > 255.f){ g = 255.f;}
	float	b = pCol->b * 255.f; if(b > 255.f){ b = 255.f;}

	d3_2d_triangle_fill_draw(pvecP0, pvecP1, pvecP2, D3_COL((uchar)a, (uchar)r, (uchar)g, (uchar)b));
}



/*----------------------------------------------------------------------*
| <<< ３Ｄ三角形描画 >>>
|	入力	pvecPos0, pvecPos1, pvecPos2 = 座標
|			dwCol						 = 色
*----------------------------------------------------------------------*/
void	d3_3d_triangle_fill_draw(VEC3 *pvecP0, VEC3 *pvecP1, VEC3 *pvecP2, COL *pCol)
{

	d3_3d_triangle_fill_draw(pvecP0, pvecP2, pvecP1, D3_COL((uchar)pCol->a, (uchar)pCol->r, (uchar)pCol->g, (uchar)pCol->b));
}



/*------------------------------------------------------------------------------*
| <<< 頂点情報より、２Ｄ図形を描画 >>>
|	入力	pvecPos    = 表示座標
|			fR		   = 大きさ
|			fAngle     = 向き
|			pcol	   = 色
|			iPolyCount = ポリ数
|			pfVertex   = 頂点テーブル
|			psIndex    = インデックステーブル
*------------------------------------------------------------------------------*/
void	d3_2d_shape_draw(VEC3 *pvecPos, float fScale, float fXScale, float fYScale, float fAngle, COL *pcol, int iPolyCount, float *pfVertex, short *psIndex)
{

	MTX		world, trans, scl, rot;


	d3_material_set(      &d3.matNormal);	// 材質設定
	d3_stat_transform_set(&d3.mtxIdentity);

	// 行列設定
	MTX		mtxWorld, mtxTrans, mtxRot, mtxScale;
	/*
	calc_mtx_rot_z(&mtxRot,   fAngle);
	calc_mtx_trans(&mtxTrans, pvecPos);
	calc_mtx_scale(&mtxScale, fScale * fXScale, -fScale * fYScale, 1);
	calc_mtx_multi(&mtxWorld, &mtxScale, &mtxRot);
	calc_mtx_multi(&mtxWorld, &mtxWorld, &mtxTrans);
	*/

	D3DXMatrixRotationZ(  &mtxRot,   fAngle);
	D3DXMatrixTranslation(&mtxTrans, pvecPos);
	D3DXMatrixScaling(    &mtxScale, fScale * fXScale, -fScale * fYScale, 1);
	
	mtxWorld = mtxScale * mtxRot * mtxTrans;
	
	D3_VTX_TL_	*pvtxBuf = (D3_VTX_TL_*)malloc(iPolyCount * sizeof(D3_VTX_TL_) * 3);
	D3_VTX_TL_	*p       = pvtxBuf;

	d3_tex_set(-1);
	d3_stat_fvf_set(D3_FVF_TL);

	DWORD	c = D3_COL(pcol->a, pcol->r, pcol->g, pcol->b);
	for(int i =	0; i < iPolyCount * 3; i++)
	{
		//calc_vec2_transform_coord((VEC2*)&p->x, (VEC2*)&pfVertex[*psIndex], &mtxWorld);

		D3DXVec2TransformCoord((VEC2*)&p->x, (VEC2*)&pfVertex[*psIndex], &mtxWorld);
		p->z   = pvecPos->z;
		p->rhw = 1;
		p->c   = c;
		p++;
		psIndex++;
	}

	d3_stat_culling_set(false);
	d3.dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST,	iPolyCount, pvtxBuf, sizeof(D3_VTX_TL));
	d3_stat_culling_set(true);
	d3.iPolyCount += iPolyCount;
	free(pvtxBuf);
}



/*----------------------------------------------------------------------*
| <<< ２ＤでＢＯＸを塗りつぶしてテクスチャーを張る >>>
|	入力	pvecP0, pvecP1			= 左上と右下の座標
|			(fU0, fV0) - (fU1, fV1) = UV の左上と右下
|			dwCol					= 色
|	 　p0
|		*-----*
|		|     |
|		*-----*
|	          p1
|	※テクスチャーは直前で d3_tex_set として定義されたものになる
*----------------------------------------------------------------------*/
void	d3_2d_box_fill_tex_draw(VEC2 *pvecP0, VEC2 *pvecP1, float fU0, float fV0, float fU1, float fV1, DWORD dwCol)
{

	d3_light_set(false);

	#ifdef	__2D_RESIZE
		float	x0 = D3_CONV_X(pvecP0->x);
		float	x1 = D3_CONV_X(pvecP1->x);
		float	y0 = D3_CONV_Y(pvecP0->y);
		float	y1 = D3_CONV_Y(pvecP1->y);
		D3_VTX_TLX_	vt[] = {x0, y1, 0, 1, dwCol, fU0, fV1,
							x0, y0, 0, 1, dwCol, fU0, fV0,
							x1, y0, 0, 1, dwCol, fU1, fV0,
							x1, y1, 0, 1, dwCol, fU1, fV1};
	#else
		D3_VTX_TLX_	vt[] = {pvecP0->x, pvecP1->y, 0, 1, dwCol, fU0, fV1,
							pvecP0->x, pvecP0->y, 0, 1, dwCol, fU0, fV0,
							pvecP1->x, pvecP0->y, 0, 1, dwCol, fU1, fV0,
							pvecP1->x, pvecP1->y, 0, 1, dwCol, fU1, fV1};
	#endif

	d3.dev->SetTransform(D3DTS_WORLD, &d3.mtxIdentity);

	D3_POLY_RENDER(TRIANGLEFAN, vt, TLX, 2);
	d3_light_set(true);
}
void	d3_2d_box_fill_tex_draw(VEC2 *pvecP0, VEC2 *pvecP1, float fU0, float fV0, float fU1, float fV1)
{

	d3_2d_box_fill_tex_draw(pvecP0, pvecP1, fU0, fV0, fU1, fV1, D3_COL_NORMAL);
}
void	d3_2d_box_fill_tex_draw(VEC2 *pvecP0, VEC2 *pvecP1, DWORD dwCol)
{
	float	fU0 = 0;
	float	fV0 = 0;
	float	fU1 = d3.texInfo[d3.iTexCurrent].fU;
	float	fV1 = d3.texInfo[d3.iTexCurrent].fV;

	d3_2d_box_fill_tex_draw(pvecP0, pvecP1, fU0, fV0, fU1, fV1, dwCol);
}
void	d3_2d_box_fill_tex_draw(VEC2 *pvecP0, VEC2 *pvecP1)
{
	d3_2d_box_fill_tex_draw(pvecP0, pvecP1, D3_COL_NORMAL);
}



/*----------------------------------------------------------------------*
| <<< ２ＤでＢＯＸを塗りつぶしてテクスチャーを張る >>>
|	入力	apvecPos = 各頂点の配列
|			afUV     = UV の配列
|			dwCol	 = 色
|
|	  apvecPos[0]	apvecPos[1]
|		*-------------*
|		|             |
|		*-------------*
|	  apvecPos[3]	apvecPos[2]
|
|	  afUV[0]		afUV[2]
|		*-------------*
|		|             |
|		*-------------*
|	  afUV[6]		afUV[4]
|
|	※テクスチャーは直前で d3_tex_set として定義されたものになる
*----------------------------------------------------------------------*/
void	d3_2d_box_fill_tex_draw(VEC2 *apvecPos, float *afUV, DWORD dwCol)
{

	d3_light_set(false);

	#ifdef	__2D_RESIZE
		float	x0 = D3_CONV_X(apvecPos[0].x);
		float	y0 = D3_CONV_Y(apvecPos[0].y);
		float	x1 = D3_CONV_X(apvecPos[1].x);
		float	y1 = D3_CONV_Y(apvecPos[1].y);
		float	x2 = D3_CONV_X(apvecPos[2].x);
		float	y2 = D3_CONV_Y(apvecPos[2].y);
		float	x3 = D3_CONV_X(apvecPos[3].x);
		float	y3 = D3_CONV_Y(apvecPos[3].y);
		D3_VTX_TLX_	vt[] =
		{
			x0, y0, 0, 1, dwCol, afUV[0], afUV[1],
			x1, y1, 0, 1, dwCol, afUV[2], afUV[3],
			x2, y2, 0, 1, dwCol, afUV[6], afUV[7],
			x3, y3, 0, 1, dwCol, afUV[4], afUV[5]
		};
	#else
		D3_VTX_TLX_	vt[] =
		{
			apvecPos[0].x, apvecPos[0].y, 0, 1, dwCol, afUV[0], afUV[1],
			apvecPos[1].x, apvecPos[1].y, 0, 1, dwCol, afUV[2], afUV[3],
			apvecPos[2].x, apvecPos[2].y, 0, 1, dwCol, afUV[4], afUV[5],
			apvecPos[3].x, apvecPos[3].y, 0, 1, dwCol, afUV[6], afUV[7],
		};
	#endif

	d3_stat_transform_set(&d3.mtxIdentity);
	D3_POLY_RENDER(TRIANGLEFAN, vt, TLX, 2);
	d3_light_set(true);
}
void	d3_2d_box_fill_tex_draw(VEC2 *apvecPos, float *afUV)
{

	d3_2d_box_fill_tex_draw(apvecPos, afUV, D3_COL_NORMAL);
}
void	d3_2d_box_fill_tex_draw(VEC2 *apvecPos, DWORD dwCol)
{
	float	afUV[8] =
	{
		0,								0,
		d3.texInfo[d3.iTexCurrent].fU,	0,
		d3.texInfo[d3.iTexCurrent].fU,	d3.texInfo[d3.iTexCurrent].fV,
		0,								d3.texInfo[d3.iTexCurrent].fV
	};
	d3_2d_box_fill_tex_draw(apvecPos, afUV, dwCol);
}
void	d3_2d_box_fill_tex_draw(VEC2 *apvecPos)
{
	d3_2d_box_fill_tex_draw(apvecPos, D3_COL_NORMAL);
}



/*----------------------------------------------------------------------*
| <<< ２ＤでＢＯＸを塗りつぶし(テクスチャー無し) >>>
|	入力	pvecP0, pvecP1 = 左上と右下の座標
|			pCol		   = 色
|	 　p0
|		*-----*
|		|     |
|		*-----*
|	          p1
*----------------------------------------------------------------------*/
void	d3_2d_box_fill_draw(VEC2 *pvecP0, VEC2 *pvecP1, DWORD dwCol)
{

	#ifdef	__2D_RESIZE
		float	x0 = D3_CONV_X(pvecP0->x);
		float	x1 = D3_CONV_X(pvecP1->x);
		float	y0 = D3_CONV_Y(pvecP0->y);
		float	y1 = D3_CONV_Y(pvecP1->y);
		D3_VTX_TL_	vt[] = {
							x0, y0, 0, 1, dwCol,
							x1, y0, 0, 1, dwCol,
							x1, y1, 0, 1, dwCol,
							x0, y1, 0, 1, dwCol
							};
	#else
		D3_VTX_TL_	vt[] = {
							pvecP0->x, pvecP0->y, 0, 1,	dwCol,
							pvecP1->x, pvecP0->y, 0, 1,	dwCol,
							pvecP1->x, pvecP1->y, 0, 1,	dwCol,
							pvecP0->x, pvecP1->y, 0, 1,	dwCol
							};
	#endif

	d3_tex_set(-1);
	D3_POLY_RENDER(TRIANGLEFAN, vt, TL, 2);
}



/*----------------------------------------------------------------------*
| <<< ２ＤでＢＯＸを描く(ラインのみ) >>>
|	入力	pvecP0, pvecP1 = 左上と右下の座標
|			pCol		   = 色
|	 　p0
|		*-----*
|		|     |
|		*-----*
|	          p1
*----------------------------------------------------------------------*/
void	d3_2d_box_draw(VEC2 *pvecP0, VEC2 *pvecP1, DWORD dwCol)
{

	#ifdef	__2D_RESIZE
		float	x0 = D3_CONV_X(pvecP0->x);
		float	x1 = D3_CONV_X(pvecP1->x);
		float	y0 = D3_CONV_Y(pvecP0->y);
		float	y1 = D3_CONV_Y(pvecP1->y);
		D3_VTX_TL_	vt[] = {x0, y1, 0, 1, dwCol,
							x0, y0, 0, 1, dwCol,
							x1, y0, 0, 1, dwCol,
							x1, y1, 0, 1, dwCol,
							x0, y1, 0, 1, dwCol};
	#else
		D3_VTX_TL_	vt[] = {pvecP0->x, pvecP1->y, 0, 1, dwCol,
							pvecP0->x, pvecP0->y, 0, 1, dwCol,
							pvecP1->x, pvecP0->y, 0, 1, dwCol,
							pvecP1->x, pvecP1->y, 0, 1, dwCol,
							pvecP0->x, pvecP1->y, 0, 1, dwCol};
	#endif

	d3_tex_set(-1);
	D3_POLY_RENDER(LINESTRIP, vt, TL, 4);
}



/**************************************************************************************************
	[[[	３Ｄ描画系 ]]]
**************************************************************************************************/

#if 0
	/*------------------------------------------------------------------------------*q
	| <<< 球を登録 >>>
	|	入力	pvecPos   = 座標
	|			fRadius   = 半径
	|			pCol      = 色
	*------------------------------------------------------------------------------*/
	void	d3_3d_sphere_set(VEC3 *pvecPos, float fRadius, COL *pCol)
	{

		if(d3.usSphere < D3_SPHERE_MAX)
		{
			D3_SPHERE	*s = &d3.sphere[d3.usSphere];
			
			// 行列設定
			MTX		trans, scale;
			D3DXMatrixScaling(    &scale, fRadius);
			D3DXMatrixTranslation(&trans, pvecPos);
			s->mtxWorld = scale * trans;

			// 色設定
			s->col = COL4(pCol->r, pCol->g, pCol->b, pCol->a) / 255.f;
			d3.usSphere++;			
		}
	}



	/*------------------------------------------------------------------------------*q
	| <<< 登録された球を描画 >>>
	*------------------------------------------------------------------------------*/
	void	d3_3d_sphere_flush(void)
	{

		//--- シェーダー切り替え ----------------------------------------
		d3.eff = d3.lpEffect[D3_SHADER_TYPE_NORMAL];
		if(d3.eff == NULL){ return;}

		//--- テクニック ------------------------------------------------
		if(!SHADER_TECH_SET(d3.eff, "Normal")){ return;}

		//--- ライト ----------------------------------------------------
		SHADER_INT_SET(			 "g_iLightSw",		   d3.bLight);

		//--- プロジェクション＆ワールド行列 ----------------------------
		SHADER_MATRIX_SET("g_mtxViewProjection", &d3.mtxViewProjection);

		//--- テクスチャー＆マテリアル ----------------------------------
		SHADER_TEXTURE_SET("texDecale",    d3.tex[TEX_WHITE]);
		SHADER_VEC4_SET(   "g_vecAmbient", &VEC4(0.0f, 0.0f, 0.0f, 0.0f));

		//--- 描画 ------------------------------------------------------
		D3_SPHERE	*s = d3.sphere;
		for(int i = 0; i < d3.usSphere; i++, s++)
		{
			// ライト
			SHADER_LIGHT_INVERSE_SET("g_vecLightDirection", &s->mtxWorld, NULL);
			// ワールド行列
			SHADER_MATRIX_SET("g_mtxWorld",	  &s->mtxWorld);
			// 色
			SHADER_VEC4_SET(  "g_vecDiffuse", (VEC4*)&s->col);
			// アルファ
			SHADER_FLOAT_SET( "g_fAlp",       s->col.a);

			// 描画
			SHADER_BEGIN();
			SHADER_PASS_BEGIN(0);
			d3.lpMeshSphere->DrawSubset(0);
			SHADER_PASS_END();
			SHADER_END();
		}
		d3.usSphere = 0;	
	}
#endif



/*----------------------------------------------------------------------*
| <<< ３Ｄ球の描画 >>>
|	入力	pvecPos   = 座標
|			fRadius   = 半径
|			pmtxWorld = ワールド行列
|			pCol      = 色
|	※シェーダー「00.fx」を使います
*----------------------------------------------------------------------*/
void	d3_3d_sphere_draw(VEC3 *pvecPos, float fRadius, MTX *pmtxWorld, COL4 *pCol)
{

	MTX		mtxWorld2, mtxTrans, mtxScale;
	D3DXMatrixTranslation(&mtxTrans, *pvecPos);
	D3DXMatrixScaling(    &mtxScale, fRadius);
	if(*pmtxWorld == NULL){ mtxWorld2 = mtxScale * mtxTrans;}
	else				  { mtxWorld2 = mtxScale * mtxTrans * *pmtxWorld;}

	//--- シェーダー切り替え ----------------------------------------
	d3.eff = d3.lpEffect[D3_SHADER_TYPE_NORMAL];
	if(d3.eff == NULL){ return;}

	//--- テクニック ------------------------------------------------
	if(!SHADER_TECH_SET(d3.eff, "Normal")){ return;}

	//--- ライト ----------------------------------------------------
	SHADER_LIGHT_INVERSE_SET("g_vecLightDirection", &mtxWorld2, NULL);
	SHADER_INT_SET(			 "g_iLightSw",		   d3.bLight);

	//--- プロジェクション＆ワールド行列 ----------------------------
	SHADER_MATRIX_SET("g_mtxViewProjection", &d3.mtxViewProjection);
	SHADER_MATRIX_SET("g_mtxWorld",		     &mtxWorld2);
	SHADER_INT_SET(	  "g_iNumBones",	     4);

	//--- テクスチャー＆マテリアル ----------------------------------
	//D3_ALPHA_SW_DIRECT(true);
	SHADER_FLOAT_SET(  "g_fAlp",       pCol->a);
	SHADER_TEXTURE_SET("texDecale",    d3.tex[TEX_WHITE]);
	SHADER_VEC4_SET(   "g_vecDiffuse", (VEC4*)pCol);
	SHADER_VEC4_SET(   "g_vecAmbient", &VEC4(0.0f, 0.0f, 0.0f, 0.0f));

	//--- 描画 ------------------------------------------------------
	SHADER_BEGIN();
	SHADER_PASS_BEGIN(0);
	d3.lpMeshSphere->DrawSubset(0);
	SHADER_PASS_END();
	SHADER_END();
}



/*----------------------------------------------------------------------*
| <<< ３Ｄ球の描画 >>>
|	入力	pvecPos   = 座標
|			fRadius   = 半径
|			pCol      = 色
|	※シェーダー「00.fx」を使います
*----------------------------------------------------------------------*/
void	d3_3d_sphere_draw(VEC3 *pvecPos, float fRadius, COL4 *pCol)
{
	d3_3d_sphere_draw(pvecPos, fRadius, NULL, pCol);
}



/*----------------------------------------------------------------------*
| <<< ３Ｄ三角形の描画 >>>
|	入力	pvecPos0, pvecPos1, pvecPos2 = 座標
|			dwCol						 = 色
*----------------------------------------------------------------------*/
void	d3_3d_triangle_fill_draw(VEC3 *pvecP0, VEC3 *pvecP1, VEC3 *pvecP2, DWORD dwCol)
{

	#ifdef	__2D_RESIZE
		VEC3	p0(D3_CONV_X(pvecP0->x), D3_CONV_Y(pvecP0->y), pvecP0->z);
		VEC3	p1(D3_CONV_X(pvecP1->x), D3_CONV_Y(pvecP1->y), pvecP1->z);
		VEC3	p2(D3_CONV_X(pvecP2->x), D3_CONV_Y(pvecP2->y), pvecP2->z);
		D3_VTX_L_	vt[] =
		{
			pvecP0->x, pvecP0->y, pvecP0->z, dwCol,
			pvecP1->x, pvecP1->y, pvecP1->z, dwCol,
			pvecP2->x, pvecP2->y, pvecP2->z, dwCol,
		};
	#else
		D3_VTX_L_	vt[] =
		{
			pvecP0->x, pvecP0->y, pvecP0->z, dwCol,
			pvecP1->x, pvecP1->y, pvecP1->z, dwCol,
			pvecP2->x, pvecP2->y, pvecP2->z, dwCol,
		};
	#endif
	d3_tex_set(-1);
	D3_RENDER(D3DPT_TRIANGLELIST, vt, L, 1);
	d3.iPolyCount++;
}



/*------------------------------------------------------------------------------*
| <<< ３Ｄラインの登録 >>>
|	入力	pvecP0, pvecP1 = 始点と終点
|			dwCol          = 色
|	※登録関数なので描画の際には d3_3d_line_flush を呼び出しのこと
*------------------------------------------------------------------------------*/
void	d3_3d_line_set(VEC3 *p0, VEC3 *p1, DWORD c)
{
	if(d3.iLineCount < D3_3DLINE_MAX)
	{
		d3.vtxLine[d3.iLineCount].p	= *p0;
		d3.vtxLine[d3.iLineCount].c	= c;
		d3.iLineCount++;
		d3.vtxLine[d3.iLineCount].p	= *p1;
		d3.vtxLine[d3.iLineCount].c	= c;
		d3.iLineCount++;
	}		
}



/*------------------------------------------------------------------------------*
| <<< d3_3d_line_set で登録された３Ｄラインの描画 >>>
|	入力	pmtxWorld = 描画行列
|			bSw = true :	ラインを必ず手前に描画
|	※登録関数なので描画の際には d3_3d_line_flush を呼び出しのこと
*------------------------------------------------------------------------------*/
void	d3_3d_line_flush(MTX *pmtx, bool bSw)
{

	if(d3.iLineCount > 0)
	{
		d3_light_set(   false);
		d3_tex_set(     -1);
		d3_shader_set(  D3_SHADER_TYPE_OFF);
		d3_stat_fvf_set(D3_FVF_L);
		if(bSw)
		{
			d3_stat_zwrite_set(false);
			d3_stat_zread_set( false);
		}
		//D3_TRANSFORM(pmtx);
		d3_stat_transform_set(pmtx);
		d3.dev->DrawPrimitiveUP(D3DPT_LINELIST,	d3.iLineCount /	2, d3.vtxLine, sizeof(D3_VTX_L));
		d3.iLineCount =	0;

		d3_stat_zwrite_set(true);
		d3_stat_zread_set( true);
		d3_light_set(      true);
	}
}



/*------------------------------------------------------------------------------*
| <<< ３Ｄラインの描画 >>>
|	入力	pvecP0, pvecP1 = 始点と終点
|			dwCol          = 色
*------------------------------------------------------------------------------*/
void	d3_3d_line_draw(VEC3 *p0, VEC3 *p1, DWORD c)
{

	D3_VTX_L	vtx[2];

	vtx[0].p = *p0;
	vtx[0].c = c;
	vtx[1].p = *p1;
	vtx[1].c = c;

	d3_tex_set(     -1);
	d3_stat_fvf_set(D3_FVF_L);
	d3.dev->DrawPrimitiveUP(D3DPT_LINELIST,	1, vtx, sizeof(D3_VTX_L));
}



/*------------------------------------------------------------------------------*
| <<< ２Ｄラインの描画 >>>
|	入力	pvecP0, pvecP1 = 始点と終点
|			dwCol          = 色
*------------------------------------------------------------------------------*/
void	d3_2d_line_draw(VEC2 *pvecP0, VEC2 *pvecP1, DWORD dwCol)
{

	#ifdef	__2D_RESIZE
		VEC2	p0(D3_CONV_X(pvecP0->x), D3_CONV_Y(pvecP0->y));
		VEC2	p1(D3_CONV_X(pvecP1->x), D3_CONV_Y(pvecP1->y));
		D3_VTX_TL_	vt[] =
		{
			p0.x, p0.y, 0, 1, dwCol,
			p1.x, p1.y, 0, 1, dwCol
		};
	#else
		D3_VTX_TL_	vt[] =
		{
			pvecP0->x, pvecP0->y, 0, 1,	dwCol,
			pvecP1->x, pvecP1->y, 0, 1,	dwCol
		};
	#endif

	d3_tex_set(    -1);
	d3.dev->SetFVF(D3_FVF_TL);
	d3.dev->DrawPrimitiveUP(D3DPT_LINESTRIP, 1,	vt,	sizeof(D3_VTX_TL));
}



/**************************************************************************************************
	[[[	スプライト系 ]]]
	※自前でスプライトを描画する。
	　中心座標のみで描画できるのが便利。
**************************************************************************************************/

#define	SPR_PERS	800					// スプライトのパース値(値が小さいと奥行き感は付きますがＵＶがゆがみます)

/*------------------------------------------------------------------------------*
| <<< 自前スプライトセット >>>
|	入力	iTex      = テクスチャー番号
|			pRect     = 画像の切り出し座標
|			pvecPos   = 表示座標
|			pmtxWorld = 行列
|			dwCol     = カラー
*------------------------------------------------------------------------------*/
void	d3_spr_draw(int iTex, RECT *prect, VEC3 *pvecPos, MTX *pmtxWorld, DWORD dwCol)
{

	//--- RECT チェック ---------------------------------------------
	if(prect == NULL)
	{
		RECT	a = {0, 0, (uint)d3.texInfo[iTex].fOriWidth, (uint)d3.texInfo[iTex].fOriHeight}; 
		prect	  = &a;
	}
	//--- 横幅、縦幅を得る ------------------------------------------
	float	w0  =  (prect->right  - prect->left) / 2.f;
	float	h0  = -(prect->bottom - prect->top ) / 2.f;
	float	w1  =  (prect->right  - prect->left) / 2.f;
	float	h1  = -(prect->bottom - prect->top ) / 2.f;

	//--- UV を求める -----------------------------------------------
	float	u0, v0, u1, v1;
	if(prect != NULL)
	{							// ↓ 0.5f を足したり引いたりで UV のつなぎ目を消去してます
		u0 = ((float)prect->left   + 0.5f) / d3.texInfo[iTex].fWidth;
		v0 = ((float)prect->top    + 0.5f) / d3.texInfo[iTex].fHeight;
		u1 = ((float)prect->right  - 0.5f) / d3.texInfo[iTex].fWidth;
		v1 = ((float)prect->bottom - 0.5f) / d3.texInfo[iTex].fHeight;
	}
	else
	{
		u0 = 0.5f / d3.texInfo[iTex].fWidth;
		v0 = 0.5f / d3.texInfo[iTex].fHeight;
		u1 = (d3.texInfo[iTex].fOriWidth  - 0.5f) / d3.texInfo[iTex].fWidth;
		v1 = (d3.texInfo[iTex].fOriHeight - 0.5f) / d3.texInfo[iTex].fHeight;
	}

	//--- ポリゴンの四頂点を得る ------------------------------------
	//	0 *---* 1			0 = 左上、1 = 右上
	//	  |   |				2 = 右下、3 = 左下
	//  3 *---* 2
	float	x[4], y[4];
	x[0] = -w0, y[0] =  h0;
	x[1] =  w0, y[1] =  h0;
	x[2] =  w0, y[2] = -h0;
	x[3] = -w0, y[3] = -h0;

	//--- 四頂点に行列を適用する ------------------------------------
	VEC3	avecPos[4];
	int		i;
	if(*pmtxWorld != NULL)
	{
		for(i = 0; i < 4; i++){ D3DXVec3TransformCoord(&avecPos[i], &VEC3(x[i], y[i], 0), pmtxWorld);}
	}
	else
	{
		for(i = 0; i < 4; i++){ avecPos[i] = VEC3(x[i], y[i], 0);}
	}

	//--- ２Ｄ→３Ｄ変換 --------------------------------------------
	// かなり適当に計算
	for(i = 0; i < 4; i++)
	{
		// パースの計算
		avecPos[i].z  = (avecPos[i].z + SPR_PERS) / (SPR_PERS * 2);
		avecPos[i].x *= avecPos[i].z * 2.0f;
		avecPos[i].y *= avecPos[i].z * 2.0f;

		// 座標指定のものへ
		if(pvecPos != NULL){ avecPos[i] += *pvecPos;}
		else			   { avecPos[i] += VEC3(d3.sRenderW, d3.sRenderH, 0) / 2.f;}

	}
	// Z 座標をチェック(0.0fで手前、1.0f で一番奥になる)
	float	fZ = pvecPos->z;
	if(     fZ < 0.0f){ fZ = 0.0f;}
	else if(fZ > 1.0f){ fZ = 1.0f;}

	//--- 描画登録 --------------------------------------------------
	// 頂点バッファ
	#ifdef	__2D_RESIZE
		D3_VTX_TLX_	vt[] =
		{
			D3_CONV_X(avecPos[0].x), D3_CONV_Y(avecPos[0].y), fZ, 1, dwCol, u0, v0,
			D3_CONV_X(avecPos[1].x), D3_CONV_Y(avecPos[1].y), fZ, 1, dwCol, u1, v0,
			D3_CONV_X(avecPos[2].x), D3_CONV_Y(avecPos[2].y), fZ, 1, dwCol, u1, v1,
			D3_CONV_X(avecPos[3].x), D3_CONV_Y(avecPos[3].y), fZ, 1, dwCol, u0, v1,
		};
	#else
		D3_VTX_TLX_	vt[] =
		{
			avecPos[0].x, avecPos[0].y, avecPos[0].z, 1, dwCol, u0, v0,
			avecPos[1].x, avecPos[1].y, avecPos[1].z, 1, dwCol, u1, v0,
			avecPos[2].x, avecPos[2].y, avecPos[2].z, 1, dwCol, u1, v1,
			avecPos[3].x, avecPos[3].y, avecPos[3].z, 1, dwCol, u0, v1,
		};
	#endif

	d3_tex_set(iTex);
	D3_POLY_RENDER(TRIANGLEFAN, vt, TLX, 2);
}

void	d3_spr_draw(int iTex, RECT *prect, VEC3 *pvecPos, MTX *pmtxWorld)
{
	d3_spr_draw(iTex, prect, pvecPos, pmtxWorld, D3_COL_NORMAL);
}

void	d3_spr_draw(int iTex, RECT *prect, VEC3 *pvecPos, DWORD dwCol)
{
	d3_spr_draw(iTex, prect, pvecPos, NULL, dwCol);
}

void	d3_spr_draw(int iTex, RECT *prect, VEC3 *pvecPos)
{
	d3_spr_draw(iTex, prect, pvecPos, NULL, D3_COL_NORMAL);
}



/**************************************************************************************************
	[[[	レンダリング系 ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< レンダリングサブ >>>
|	入力	sub	 = 描画関数
*------------------------------------------------------------------------------*/
void	d3_render_sub(void *sub)
{
	//--- デバイスのテスト ----------------------------------------
	HRESULT	hr = d3.dev->TestCooperativeLevel();
	if(hr == D3DERR_DEVICELOST || hr == D3DERR_DEVICENOTRESET)
	{
		sys_pause();
		sys.cResizeCount  = 0;
		d3.bWindow		  = TRUE;			// デバイスロスト時には、必ずウィンドウモードにする
		sys.bScreenChange =	true;
	}
	//--- メイン処理 ----------------------------------------------
	else
	{
		void(*func)(void);
		func = (void(*)(void))sub;
		(*func)();
	}
}



/*------------------------------------------------------------------------------*
| <<< レンダリングメイン >>>
*------------------------------------------------------------------------------*/
void	d3_render(void)
{
	#ifdef	__GAME_RENDER
		if(sys.bFrameSkip){ return;}
		d3_render_sub(game_render);					// 通常描画
	#endif
}



/**************************************************************************************************
	[[[	文字系 ]]]
**************************************************************************************************/


/*------------------------------------------------------------------------------*
| <<< フォントの作成 >>>
|	入力	iNum        = 管理番号
|			iW, iH      = 文字のサイズ
|			str	        = フォント名
|			bItalic     = true : イタリック
|			fWeightMult = 文字の太さの倍率
*------------------------------------------------------------------------------*/
void	d3_font_create(int iNum, int iW, int iH, char *str, bool bItalic, float fWeightMult)
{

	if(d3.font[iNum] != NULL){ SAFE_RELEASE(d3.font[iNum]);}

	d3.fontInfo[iNum].Width		      = iW;
	d3.fontInfo[iNum].Height		  = iH;
	d3.fontInfo[iNum].MipLevels	      = 0;
	d3.fontInfo[iNum].Weight          = (uint)((float)FW_NORMAL * fWeightMult);	// 字の太さみたい
	d3.fontInfo[iNum].Italic		  = bItalic;
	d3.fontInfo[iNum].CharSet		  = SHIFTJIS_CHARSET;
	d3.fontInfo[iNum].OutputPrecision = OUT_DEFAULT_PRECIS;
	d3.fontInfo[iNum].Quality		  = DEFAULT_QUALITY;
	d3.fontInfo[iNum].PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;
	if(str != NULL) strcpy(d3.fontInfo[iNum].FaceName, str);
	else			strcpy(d3.fontInfo[iNum].FaceName, "ＭＳ ゴシック");
	D3DXCreateFontIndirect(d3.dev, &d3.fontInfo[iNum], &d3.font[iNum]);
}
void	d3_font_create(int iNum, int iW, int iH, char *str, bool bItalic)
{
	d3_font_create(iNum, iW, iH, str, bItalic, 1.0f);
}
void	d3_font_create(int iNum, int iW, int iH, char *str)
{
	d3_font_create(iNum, iW, iH, str, false, 1.0f);
}
void	d3_font_create(int iW, int iH, char *str)
{
	d3_font_create(0, iW, iH, str, false, 1.0f);
}


/*------------------------------------------------------------------------------*
| <<< フォントの開放 >>>
*------------------------------------------------------------------------------*/
void	d3_font_free(void)
{
	//--- フォントの開放 ------------------------------------------
	for(int	i =	0; i < D3_FONT_MAX; i++){ SAFE_RELEASE(d3.font[i]);}
}


/*------------------------------------------------------------------------------*
| <<< 文字描画 >>>
|	入力	iFontNum = 作成する文字ＩＤ
|			fX,	fY	 = 文字サイズ
|			dwCol    = 色
|			Format   = 文字列
*------------------------------------------------------------------------------*/
void	d3_font_draw(int iFontNum, float fX, float fY, DWORD dwCol, char* Format,...)
{

	//--- 文字作成 --------------------------------------------------
    char	strBuf[512];
	va_list	ap;
	va_start(ap,	 Format);
	vsprintf(strBuf, Format, ap);
	va_end(ap);

	//--- 描画 ------------------------------------------------------
	RECT	r      = {(uint)(fX * d3.fZoom + d3.vecOfs.x), (uint)(fY * d3.fZoom + d3.vecOfs.y), 0, 0};
	uint	uiFlag = DT_NOCLIP;
	d3.font[iFontNum]->DrawText(NULL, strBuf, -1, &r, uiFlag, dwCol);
}


/*------------------------------------------------------------------------------*
| <<< 文字描画 >>>
|	入力	iFontNum = 作成する文字ＩＤ
|			fX,	fY	 = 文字サイズ
|			iLen     = 表示文字数
|			dwCol    = 色
|			Format   = 文字列
*------------------------------------------------------------------------------*/
void	d3_font_draw(int iFontNum, float fX, float fY, int iLen, DWORD dwCol, char* Format,...)
{

	if(d3.font[iFontNum] == NULL){ return;}

	//--- 文字作成 --------------------------------------------------
    char	strBuf[512];
	va_list	ap;
	va_start(ap,	 Format);
	vsprintf(strBuf, Format, ap);
	va_end(ap);

	//--- 描画 ------------------------------------------------------
	//RECT	r      = {(uint)fX, (uint)fY, 0, 0};
	RECT	r      = {(uint)(fX * d3.fZoom + d3.vecOfs.x), (uint)(fY * d3.fZoom + d3.vecOfs.y), 0, 0};
	uint	uiFlag = DT_NOCLIP;
	d3.font[iFontNum]->DrawText(NULL, strBuf, iLen, &r, uiFlag, dwCol);
}


/*------------------------------------------------------------------------------*
| <<< 文字描画 >>>
|	入力	iFontNum = 作成する文字ＩＤ
|			r		 = 文字の表示領域
|			bCenter  = true : 中央に描画
|			dwCol    = 色
|			Format   = 文字列
*------------------------------------------------------------------------------*/
void	d3_font_draw(int iFontNum, RECT *prect, bool bCenter, DWORD dwCol, char* Format,...)
{

	if(d3.font[iFontNum] == NULL){ return;}

	//--- 文字作成 --------------------------------------------------
    char	strBuf[512];
	va_list	ap;
	va_start(ap,	 Format);
	vsprintf(strBuf, Format, ap);
	va_end(ap);

	//--- 描画 ------------------------------------------------------
	uint	uiFlag = DT_NOCLIP + DT_CENTER * bCenter;
	RECT	r = *prect;
	d3.font[iFontNum]->DrawText(NULL, strBuf, -1, &r, uiFlag, dwCol);
}


/*------------------------------------------------------------------------------*
| <<< 文字登録 >>>					※使い方は printf と同じ
|	入力	iNum   = 登録番号
|			iX, iY = 描画座標
|			dwCol  = 色
|			Format = 以降、printf と同じ
*------------------------------------------------------------------------------*/
void	d3_printf(int iNum, int iX, int iY, D3DCOLOR dwCol, char* Format,...)
{
	if(iNum >= D3_TEXT_MAX){ return;}

	d3.textWork[iNum].sX    = (short)d3.vecOfs.x + iX;
	d3.textWork[iNum].sY    = (short)d3.vecOfs.y + iY;
	d3.textWork[iNum].dwCol = dwCol;

	vsprintf(d3.textWork[iNum].str, Format, (char *)(&Format + 1));
}



/*------------------------------------------------------------------------------*
| <<< 文字登録 >>>					※使い方は printf と同じ
|	入力	iNum   = 登録番号
|			iX, iY = 描画座標
|			dwCol  = 色
|			Format = 以降、printf と同じ
*------------------------------------------------------------------------------*/
void	d3_printf(int iNum, int iX, int iY, char* Format,...)
{
	if(iNum >= D3_TEXT_MAX){ return;}

	d3.textWork[iNum].sX    = (short)d3.vecOfs.x + iX;
	d3.textWork[iNum].sY    = (short)d3.vecOfs.y + iY;
	d3.textWork[iNum].dwCol = D3_COL_NORMAL;

	vsprintf(d3.textWork[iNum].str, Format, (char *)(&Format + 1));
}



/*------------------------------------------------------------------------------*
| <<< デバック用文字登録 >>>		※使い方は printf と同じ
*------------------------------------------------------------------------------*/
void	d3_printf(int iX, int iY, char* Format,...)
{
	int		iNum = 0;
	d3.textWork[iNum].sX    = (short)d3.vecOfs.x + iX;
	d3.textWork[iNum].sY    = (short)d3.vecOfs.y + iY;
	d3.textWork[iNum].dwCol = D3_COL_NORMAL;

	vsprintf(d3.textWork[iNum].str, Format, (char *)(&Format + 1));
}



/*------------------------------------------------------------------------------*
| <<< デバック用文字登録 >>>		※使い方は printf と同じ
*------------------------------------------------------------------------------*/
void	d3_printf(char* Format,...)
{
	int		iNum = 0;
	d3.textWork[iNum].sX	= (short)d3.vecOfs.x;
	d3.textWork[iNum].sY	= (short)d3.vecOfs.y;
	d3.textWork[iNum].dwCol = D3_COL_NORMAL;
	vsprintf(d3.textWork[iNum].str, Format, (char *)(&Format + 1));
}



/*------------------------------------------------------------------------------*
| <<< デバック用文字に影設定 >>>
|	入力	iNum    = 登録番号
|			bShadow = true or false
*------------------------------------------------------------------------------*/
void	d3_print_shadow_set(int iNum, bool bShadow)
{
	d3.textWork[iNum].bShadow = bShadow;
}



/*------------------------------------------------------------------------------*
| <<< d3_printf で登録した文字列を全て描画する >>>
*------------------------------------------------------------------------------*/
void	d3_print(void)
{

	for(int	i =	0; i < D3_TEXT_MAX; ++i)
	{
		RECT	r;
		D3_TEXT	*w = &d3.textWork[i];
		if(strlen(w->str) ==  0){ continue;}
		if(w->sX          == -1){ continue;}
		if(d3.textWork[i].bShadow)
		{
			SetRect(&r,	w->sX + 1, w->sY + 1, d3.sRenderW, d3.sRenderH);
			d3.font[w->ucFont]->DrawText(NULL, w->str, -1, &r, DT_WORDBREAK, D3_COL((w->dwCol >> 24), 4, 4, 4));
		}
		SetRect(&r,	w->sX, w->sY, d3.sRenderW, d3.sRenderH);
		d3.font[w->ucFont]->DrawText(NULL, w->str, -1, &r, DT_WORDBREAK, w->dwCol);
	}
}



/*------------------------------------------------------------------------------*
| <<< 文字表示オフ >>>
|	入力	iNum = 登録番号
*------------------------------------------------------------------------------*/
void	d3_print_off(int iNum)
{

	if(iNum >= D3_TEXT_MAX) return;

	d3.textWork[iNum].str[0] = NULL;
}



/*------------------------------------------------------------------------------*
| <<< 全文字表示オフ >>>
*------------------------------------------------------------------------------*/
void	d3_print_off_all(void)
{

	for(int	i =	0; i < D3_TEXT_MAX; i++) d3.textWork[i].str[0] = NULL;
}


/*------------------------------------------------------------------------------*
| <<< ３Ｄラインの初期化 >>>
*------------------------------------------------------------------------------*/
void	d3_3d_line_init(void)
{
	d3.iLineCount =	0;
}



/*------------------------------------------------------------------------------*
| <<< ３Ｄラインの描画 >>>
*------------------------------------------------------------------------------*/
void	d3_3d_line_draw(void)
{
	d3_light_set(false);
	d3_tex_set(-1);
	d3_stat_fvf_set(D3_FVF_L);
	d3.dev->DrawPrimitiveUP(D3DPT_LINESTRIP, d3.iLineCount - 1,	d3.vtxLine,	sizeof(D3_VTX_L));
}



/**************************************************************************************************
	[[[	ＢＭＰ系 ]]]
**************************************************************************************************/


/*----------------------------------------------------------------------*
| <<< カラー値を R8G8B8	フォーマットに変換する >>>
|	入力	col	   = 色
|			format = フォーマット
|	戻り値	X8R8G8 のデータ
*----------------------------------------------------------------------*/
D3DCOLOR	d3_change_xrgb(DWORD col, D3DFORMAT format)
{

	D3DCOLOR	c;
	DWORD		r,g,b;

	switch(format)
	{
	case D3DFMT_R8G8B8:
		c =	0xff000000 | (col >> 8);
		break;

	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
		c =	0xff000000 | col;
		break;

	case D3DFMT_R5G6B5:
		r =	min(0xff, ((col	& 0xf800) >> 11) * 8);
		g =	min(0xff, ((col	& 0x07e0) >>  5) * 4);
		b =	min(0xff, (	col	& 0x001f	   ) * 8);
		c =	0xff000000 | r | g | b;
		break;

	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
		r =	min(0xff, ((col	& 0x7e00) >> 11) * 8);
		g =	min(0xff, ((col	& 0x03e0) >>  5) * 8);
		b =	min(0xff, (	col	& 0x001f	   ) * 8);
		c =	0xff000000 | r | g | b;
		break;

	case D3DFMT_A4R4G4B4:
		r =	min(0xff, ((col	& 0x0f00) >> 8)	* 16);
		g =	min(0xff, ((col	& 0x00f0) >> 4)	* 16);
		b =	min(0xff, (	col	& 0x000f	  )	* 16);
		c =	0xff000000 | r | g | b;
		break;

	default:
		c =	0;
	}
	return c;
}



/*-----------------------------------------------------------------------*
| <<< ＢＭＰファイルのヘッダファイルを読み出し >>>
|	入力	*f0	  =	ファイルポインタ
|			*s	  =	エラーメッセージ格納用文字列のアドレス
|	出力	*s	  =	エラーメッセージ格納用文字列
|	戻り値	false =	取り出し失敗
|			true  =	取り出し成功
*-----------------------------------------------------------------------*/
bool	d3_bmp_head_read(HANDLE	f0,	BMP_WORK *w, char *s)
{

	DWORD	rs;


	ReadFile(f0, &w->head.usType,	   sizeof(w->head.usType),		&rs, NULL);
	ReadFile(f0, &w->head.uiSize,	   sizeof(w->head.uiSize),	    &rs, NULL);
	ReadFile(f0, &w->head.usReserved1, sizeof(w->head.usReserved1), &rs, NULL);
	ReadFile(f0, &w->head.usReserved2, sizeof(w->head.usReserved2), &rs, NULL);
	ReadFile(f0, &w->head.uiOffBits,   sizeof(w->head.uiOffBits),	&rs, NULL);

	ReadFile(f0, &w->info, 40, &rs,	NULL);
	if(w->head.usType != 19778)				// ＢＭＰファイルではない
	{
		strcat(s, "はビットマップファイルではありません。");
		return false;
	}
	if(w->info.uiComp != 0)					// 圧縮されている
	{
		strcat(s, "は圧縮されてます。圧縮ファイルはサポートされていません。");
		return false;
	}

	int		_w = w->info.uiXsize;			// バッファの中での横幅
	int		_h = w->info.uiYsize;			// バッファの中での横幅

	switch(w->info.usBitCount)
	{
	case 4:										// １６色
		w->info.uiSizeImage = (((_w + 3) / 4) * 4) * _h / 2;
		w->info.uiClrUsed	= 16;
		break;
	case 8:										// ２５６色
		w->info.uiSizeImage = _w * _h;
		w->info.uiClrUsed	= 256;
		break;
	case 16:									// ハイカラー
//		w->info.uiSizeImage = _w * _h	* 2;
		break;
	case 24:									// フルカラー
		_w = (((w->info.uiXsize * 3) + 3) / 4) * 4;
		w->info.uiSizeImage = _w * _h;
		break;
	case 32:									// フルカラー３２ビット
		w->info.uiSizeImage = _w * _h * 4;
		break;
	}
	return true;
}



/*-----------------------------------------------------------------------*
|	<<<	ＢＭＰデータ読み出し >>>
|	入力	*w	  =	ＢＭＰの情報ファイル
|			*file =	読み出しファイル
|	出力	*buf  =	変換後のデータが格納される
*-----------------------------------------------------------------------*/
bool	d3_bmp_read(BMP_WORK *w, char *file)
{

	uint	j;
	DWORD	rs;

	HANDLE	handle = CreateFile(file, GENERIC_READ,	0, NULL, OPEN_EXISTING,	FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS, NULL);
	if(handle == INVALID_HANDLE_VALUE) return sys_error("%s がありません。", file);
	strcpy(w->strName, file);
	if((j =	GetFileSize(handle,	NULL)) == 0){ CloseHandle(handle); return sys_error("%s のサイズが０バイトです。", file);}
	if(!d3_bmp_head_read(handle, w,	file)){	  CloseHandle(handle); return sys_error("%s にエラー発生",			   file);}
	if((w->ucBuf = (uchar *)malloc(w->info.uiSizeImage)) == NULL)
	{
		CloseHandle(handle); return	sys_error("%s 読み込み時にメモリー不足。", file);
	}
	w->usBuf16 = (ushort *)w->ucBuf;
	w->ucBuf24 = (uchar  *)w->ucBuf;
	w->ucBuf32 = (uchar  *)w->ucBuf;
	if(w->info.usBitCount <= 8)				// ２５６色以下ならパレットもロード
	{
		ReadFile(handle, w->col, sizeof(BMP_COL) * w->info.uiClrUsed, &rs, NULL);
	}
	ReadFile(handle, w->ucBuf, w->info.uiSizeImage,	&rs, NULL);
	CloseHandle(handle);
	return true;
}



/*-----------------------------------------------------------------------*
|	<<<	ＢＭＰの画像データを３２ビットカラーへ変換 >>>
|	入力	*w		= ＢＭＰの情報ファイル
|	出力	*w->buf	= 変換後のデータが格納される
*-----------------------------------------------------------------------*/
void	d3_bmp_change_32color(BMP_WORK	*w)
{

	int		_w = w->info.uiXsize;			// バッファの中での横幅
	int		_h = w->info.uiYsize;			// バッファの中での横幅

	if(w->info.usBitCount != 32)
	{
		uchar	*buf = (uchar *)malloc(w->info.uiXsize * w->info.uiYsize * 4);
		if(buf == NULL){ sys_error("d3_bmp_change_32color 内でメモリーが確保不能。");}
		else
		{
			int	x, y;
			int	size = w->info.uiXsize * w->info.uiYsize;
			switch(w->info.usBitCount)
			{
			case 4:
				_w = (((w->info.uiXsize + 3) / 4) * 4) / 2;
				for(y =	0; y < (int)w->info.uiYsize; y++)
				{
					for(x =	0; x < (int)w->info.uiXsize; x++)
					{
						if(w->info.uiXsize & 1)
						{
							int	hi	= (w->ucBuf[(w->info.uiYsize - y - 1) * _w + x / 2]) >> 4;
							buf[(y * w->info.uiXsize + x) * 4 + 0] = 255;
							buf[(y * w->info.uiXsize + x) * 4 + 1] = w->col[hi].ucR;
							buf[(y * w->info.uiXsize + x) * 4 + 2] = w->col[hi].ucG;
							buf[(y * w->info.uiXsize + x) * 4 + 3] = w->col[hi].ucB;
						}
						else
						{
							int	low	= (w->ucBuf[(w->info.uiYsize - y - 1) * _w + x / 2]) & 15;
							buf[(y * w->info.uiXsize + x) * 4 + 4] = 255;
							buf[(y * w->info.uiXsize + x) * 4 + 5] = w->col[low].ucR;
							buf[(y * w->info.uiXsize + x) * 4 + 6] = w->col[low].ucG;
							buf[(y * w->info.uiXsize + x) * 4 + 7] = w->col[low].ucB;
						}
					}
				}
				break;
			case 8:
				_w = ((w->info.uiXsize + 3) / 4) * 4;
				for(y =	0; y < (int)w->info.uiYsize; y++)
				{
					for(x =	0; x < (int)w->info.uiXsize; x++)
					{
						buf[(y * w->info.uiXsize + x) * 4 + 0] = 255;
						buf[(y * w->info.uiXsize + x) * 4 + 1] = w->col[w->ucBuf[(w->info.uiYsize - y - 1) * _w + x]].ucR;
						buf[(y * w->info.uiXsize + x) * 4 + 2] = w->col[w->ucBuf[(w->info.uiYsize - y - 1) * _w + x]].ucG;
						buf[(y * w->info.uiXsize + x) * 4 + 3] = w->col[w->ucBuf[(w->info.uiYsize - y - 1) * _w + x]].ucB;
					}
				}
				break;
			case 16:
/*				for(i =	0; i < size; i++)
				{
					buf[i *	4 +	0] = 255;
					buf[i *	4 +	1] = ((w->buf16[i] >> 10) &	31)	* 8;
					buf[i *	4 +	2] = ((w->buf16[i] >>  5) &	31)	* 8;
					buf[i *	4 +	3] = ((w->buf16[i] >>  0) &	31)	* 8;
				}
*/				break;
			case 24:
				_w = (((w->info.uiXsize * 3) + 3) / 4) * 4;
				for(y =	0; y < (int)w->info.uiYsize; y++)
				{
					for(x =	0; x < (int)w->info.uiXsize; x++)
					{
						buf[(y * w->info.uiXsize + x) * 4 + 0] = 255;
						buf[(y * w->info.uiXsize + x) * 4 + 1] = w->ucBuf24[((w->info.uiYsize - y - 1) * _w + x * 3) + 2];
						buf[(y * w->info.uiXsize + x) * 4 + 2] = w->ucBuf24[((w->info.uiYsize - y - 1) * _w + x * 3) + 1];
						buf[(y * w->info.uiXsize + x) * 4 + 3] = w->ucBuf24[((w->info.uiYsize - y - 1) * _w + x * 3) + 0];
					}
				}
				break;
			}
			w->info.usBitCount  = 32;
			w->info.uiSizeImage = w->info.uiXsize *	w->info.uiYsize * 4;
			free(w->ucBuf);
			w->ucBuf	 = buf;
			w->usBuf16 = (ushort *)buf;
			w->ucBuf24 = (uchar  *)buf;
			w->ucBuf32 = (uchar  *)buf;
		}
	}
}



/*-----------------------------------------------------------------------*
|	<<<	ＢＭＰの画像データを縦に逆向きにする >>>	※データとして使いやすくなる様に
|	入力	*w		= ＢＭＰの情報ファイル
|	出力	*w->buf	= 変換後のデータが格納される
*-----------------------------------------------------------------------*/
void	d3_bmp_x_reverse(BMP_WORK *w)
{

	int		i, j, k, len = 0;
	uchar	*buf;

	switch(w->info.usBitCount)
	{
	case 8:
		len	= 1;
		break;
	case 16:
		len	= 2;
		break;
	case 24:
		len	= 3;
		break;
	case 32:
		len	= 4;
		break;
	}
	if(len == 0){	 sys_error("%s は不正なＢＭＰファイルです。",                w->strName); return;}
	buf	= (uchar *)malloc(w->info.uiXsize * w->info.uiYsize	* len);
	if(buf == NULL){ sys_error("%s : d3_bmp_x_reverse 内でメモリーが確保不能。", w->strName); return;}
	for(i =	0; i < (int)w->info.uiYsize; i++)
	{
		j =	0;
		for(k =	w->info.uiXsize - 1; k >= 0; k--, j++)
		{
			memcpy(&buf[     (i * w->info.uiXsize + k) * len],
				   &w->ucBuf[(i * w->info.uiXsize + j) * len], len);
		}
	}
	free(w->ucBuf);
	w->ucBuf   = buf;
	w->usBuf16 = (ushort *)buf;
	w->ucBuf24 = (uchar  *)buf;
	w->ucBuf32 = (uchar  *)buf;
}



/*-----------------------------------------------------------------------*
|	<<<	ＢＭＰの画像データを縦に逆向きにする >>>	※データとして使いやすくなる様に
|	入力	*w		= ＢＭＰの情報ファイル
|	出力	*w->buf	= 変換後のデータが格納される
*-----------------------------------------------------------------------*/
void	d3_bmp_y_reverse(BMP_WORK *w)
{

	int		i, j, len =	0;
	uchar	*buf;

	switch(w->info.usBitCount)
	{
	case 8:
		len	= 1;
		break;
	case 16:
		len	= 2;
		break;
	case 24:
		len	= 3;
		break;
	case 32:
		len	= 4;
		break;
	}
	if(len == 0){	 sys_error("%s は不正なＢＭＰファイルです。",				 w->strName); return;}
	buf	= (uchar *)malloc(w->info.uiXsize * w->info.uiYsize	* len);
	if(buf == NULL){ sys_error("%s : d3_bmp_y_reverse 内でメモリーが確保不能。", w->strName); return;}
	j =	0;
	for(i =	w->info.uiYsize - 1; i >= 0; i--, j++)
	{
		memcpy(&buf[     j * w->info.uiXsize * len],
			   &w->ucBuf[i * w->info.uiXsize * len], w->info.uiXsize * len);
	}
	free(w->ucBuf);
	w->ucBuf   = buf;
	w->usBuf16 = (ushort *)buf;
	w->ucBuf24 = (uchar  *)buf;
	w->ucBuf32 = (uchar  *)buf;
}



/*-----------------------------------------------------------------------*
| <<< ＢＭＰのメモリーを解放 >>>
|	入力	*w	 = ＢＭＰの情報ファイル
*-----------------------------------------------------------------------*/
void	d3_bmp_free(BMP_WORK *w)
{

	if(w->ucBuf != NULL)
	{
		free(w->ucBuf);
		w->ucBuf = NULL;
	}
}



/*-----------------------------------------------------------------------*
| <<< ＢＭＰヘッダ部分読み出し >>>
|	入力	*s	 = 読み出しファイル
*-----------------------------------------------------------------------*/
bool	d3_bmp_info_read(BMP_INFO *w, char	*s)
{

	DWORD	rs;


	HANDLE	handle = CreateFile(s, GENERIC_READ, 0,	NULL, OPEN_EXISTING,
									FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS, NULL);
	if(handle == INVALID_HANDLE_VALUE) return false;
	ReadFile(handle, w,	14,	&rs, NULL);			// 最初の１４バイトは読み飛ばす
	ReadFile(handle, w,	40,	&rs, NULL);
	CloseHandle(handle);
	return TRUE;
}



/**************************************************************************************************
	[[[	テクスチャー系 ]]]
**************************************************************************************************/


/*----------------------------------------------------------------------*
| <<< 使用可能な画像フォーマットを得る >>>
|	入力	sw	 = 0 : 普通のテクスチャー
|				   1 : レンダリング面
|	戻り値	使用可能な画像フォーマット
*----------------------------------------------------------------------*/
D3DFORMAT	d3_tex_format_search(int sw)
{
	int		Usage;

	if(sw == 0){ Usage = 0;}
	else	   { Usage = D3DUSAGE_RENDERTARGET;}

	D3DFORMAT	tbl[] =
	{//	調べたいﾌｫｰﾏｯﾄ	   バックバッファ変換用
		D3DFMT_A8R8G8B8,
		D3DFMT_A1R5G5B5,
		D3DFMT_A4R4G4B4,
		D3DFMT_A8R3G3B2,
		D3DFMT_X8R8G8B8,
		D3DFMT_R8G8B8,
		D3DFMT_X1R5G5B5,
		D3DFMT_R5G6B5,
		D3DFMT_X4R4G4B4,
	};
	// CheckDeviceFormat(0,	D3DDEVTYPE_HAL,	D3DFMT_A8R8G8B8, ,,	D3DFMT_A8R8G8B8) とすると D3_OK がこない。
	// CheckDeviceFormat(0,	D3DDEVTYPE_HAL,	D3DFMT_X8R8G8B8, ,,	D3DFMT_A8R8G8B8) として調べる必要がある

	//--- 画像フォーマットを調べる -----------------------------
	for(int	i =	0; i < 9; i++)
	{
		D3DFORMAT	fmt	= D3DFMT_R5G6B5;
		//if(d3.fmtBackBuffer	<= D3DFMT_X8R8G8B8){ fmt = D3DFMT_X8R8G8B8;}
		if(d3.d->CheckDeviceFormat(d3.ucAdapter, d3.deviceType,	fmt, Usage,	D3DRTYPE_TEXTURE, tbl[i]) == D3D_OK)
		{
			return tbl[i];
		}
	}
	return D3DFMT_UNKNOWN;
}



/*----------------------------------------------------------------------*
| <<< テクスチャー読み込み一括管理 >>>	※同名のファイルは読み込まずにポインタを返す
|	入力	iNum		= 読み込み番号
|			strFile		= 読み込みファイル名(リソース０番からの読み込みも可能)
|			bErrorCheck	= false	: エラーメッセージを表示しない
|	出力	infoOut		= 読み込み情報
|	戻り値	true  :	読み込んだ
|			false :	読み込んでいない
*----------------------------------------------------------------------*/
static	bool	TextureLoad(int iNum, char *strFile, D3DXIMAGE_INFO *infoOut, bool bErrorCheck)
{
	if(sys.hResource[0] !=	NULL)			// dll ファイルより読み込み
	{
		D3DXCreateTextureFromResourceEx(
			d3.dev,								// デバイス
			sys.hResource[0], strFile,			// 読み込みファイル名
			D3DX_DEFAULT, D3DX_DEFAULT,			// 画像のサイズ
			1,									// ミップマップレベル
			0,									// テクスチャーの使用方法
			d3.fmtTexture,						// テクスチャーのフォーマット
			D3DPOOL_MANAGED,					// テクスチャーの管理方法
			D3DX_FILTER_NONE,					// フィルター
			D3DX_FILTER_NONE,					// ミップマップのフィルター
			__COLORKEY,							// 抜き色(透明にする色)
			infoOut,							// テクスチャーの情報をえるポインタ
			NULL,								// パレットを得る
			&d3.tex[iNum]);						// テクスチャーのポインタ
	}
	if(d3.tex[iNum] == NULL)			// リソースより読み込み
	{
		char	name[_MAX_PATH + 1];
		sys_div_path(NULL, name, NULL, strFile);
		D3DXCreateTextureFromResourceEx(
			d3.dev,								// デバイス
			NULL, name,							// 読み込みファイル名
			D3DX_DEFAULT, D3DX_DEFAULT,			// 画像のサイズ
			1,									// ミップマップレベル
			0,									// テクスチャーの使用方法
			d3.fmtTexture,						// テクスチャーのフォーマット
			D3DPOOL_MANAGED,					// テクスチャーの管理方法
			D3DX_FILTER_NONE,					// フィルター
			D3DX_FILTER_NONE,					// ミップマップのフィルター
			__COLORKEY,							// 抜き色(透明にする色)
			infoOut,							// テクスチャーの情報をえるポインタ
			NULL,								// パレットを得る
			&d3.tex[iNum]);						// テクスチャーのポインタ
	}
	if(d3.tex[iNum] == NULL)			// ファイルより読み込み
	{
		D3DXCreateTextureFromFileEx(
			d3.dev,	strFile,					// デバイス、読み込みファイル名
			D3DX_DEFAULT, D3DX_DEFAULT,			// 画像のサイズ
			1,									// ミップマップレベル
			0,									// テクスチャーの使用方法
			d3.fmtTexture,						// テクスチャーのフォーマット
			D3DPOOL_MANAGED,					// テクスチャーの管理方法
			D3DX_FILTER_NONE,					// フィルター
			D3DX_FILTER_NONE,					// ミップマップのフィルター
			__COLORKEY,							// 透明にする色
			infoOut,							// テクスチャーの情報をえるポインタ
			NULL,								// パレットを得る
			&d3.tex[iNum]);						// テクスチャーのポインタ
	}

	if(bErrorCheck && d3.tex[iNum] == NULL){ return sys_error("TextureLoad: %d番 (%s) が読み込めませんでした。", iNum, strFile);}

	return d3.tex[iNum] != NULL;
}



/*----------------------------------------------------------------------*
| <<< テクスチャー読みこみ >>>
|	入力	iNum		= 読み込み番号			※-1の時は空き番号を探して読み込む。メッシュ時に使用
|			strFile		= 読み込みファイル名( NULL の場合は d3.texInfo より読み込み)
|			bLoadAtOnce = true : 即時読み込み、false : D3_TEXTURE を実行したときに読み込み(読み込み周期には表示されないので注意)
|			bErrMessage = true : エラーメッセージ表示
|	戻り値	-1  == 読み込んでない
|			 0  != 読み込み番号
*----------------------------------------------------------------------*/
int		d3_tex_load(int iNum, char *strFile, bool bLoadAtOnce, bool bErrMessage)
{

	char	strPath[_MAX_PATH + 1];

	if(strFile != NULL)
	{
		//--- 拡張子がないものはリソースとみなす ------------------------
		//bool	resource = true;
		d3.texInfo[iNum].bResource = true;
		int		i				   = 0;
		while(strFile[i] != NULL)
		{
			if(ISKANJI(strFile[i])){    i += 2;}
			else if(strFile[i] == '.'){ d3.texInfo[iNum].bResource = false; break;}
			else{						i++;}
		}

		//--- ファイル名を作成 ------------------------------------------
		//	　リソースの場合にはただコピー
		if(!d3.texInfo[iNum].bResource)
		{
			char	strName[_MAX_FNAME + 1];
			char	strExt[ _MAX_EXT   + 1];
			sys_div_path(strPath, strName, strExt, strFile);
			sprintf(strPath, "%s%s%s", strPath, strName, strExt);
		}
		else{ strcpy(strPath, strFile);}

		//--- 同名ならば、読み込まないで終了 ----------------------------
		if(iNum != -1)
		{
			if(d3.tex[iNum] != NULL)
			{
				if(strcmp(d3.texInfo[iNum].strFullPath, strPath) == 0) return iNum;
			}
		}
		//--- すでに読み込んでいるファイルかを調べる --------------------
		//	  すでに読み込んでいる場合にはその番号を返してリターン。
		//	　読み込んでいない場合には、新規に読み込む番号を iID にセットする
		else
		{
			for(i = 0; i < D3_TEX_MAX2; i++)
			{
				if(strcmp(d3.texInfo[i].strFullPath, strPath) == 0){ return i;}
			}
			for(i = D3_TEX_MAX; i < D3_TEX_MAX2; i++)
			{
				if(d3.tex[i] == NULL){ iNum = i; break;}
			}
			if(i == D3_TEX_MAX2){ return -1;}			// もうすでに読み込み場所がない
		}
	}
	else
	{
		strcpy(strPath, d3.texInfo[iNum].strFullPath);	// ファイル名保存
	}

	if(strlen(strPath) == 0){ return -1;}

	//--- 即時、読み込みで無い場合 ----------------------------------
	if(!bLoadAtOnce)
	{
		strcpy(d3.texInfo[iNum].strFullPath, strPath);	// ファイル名保存
		d3.texInfo[iNum].bLoad = false;
		return -1;
	}

	//--- 読み込み --------------------------------------------------
	D3DXIMAGE_INFO	info;									// 読み込めなかった場合は -1 を返す
	d3_tex_free(iNum);
	if(!TextureLoad(iNum, strPath, &info, bErrMessage)){ return -1;}

	//--- サイズを２の補数にする ------------------------------------
	d3.texInfo[iNum].fOriWidth  = (float)info.Width;		// オリジナルのテクスチャーサイズ
	d3.texInfo[iNum].fOriHeight = (float)info.Height;
															// テクスチャーが２の累乗で無ければいけないか調べる
	//if(!(d3.caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL))	// 保留、この条件だけではだめみたい
	{																	// テクスチャーを D3DUSAGE_DYNAMIC で読み込む必要あり
		uint	i;
		for(i =	1; i <=	d3.caps.MaxTextureWidth; i *= 2)	// テクスチャーのサイズを補正
		{
			if(info.Width <= i){ info.Width	= i; break;}
		}
		for(i =	1; i <=	d3.caps.MaxTextureHeight; i	*= 2)	// テクスチャーのサイズを補正
		{
			if(info.Height <= i){ info.Height =	i; break;}
		}
	}
	d3.texInfo[iNum].fWidth	    = (float)info.Width;		// テクスチャーサイズ
	d3.texInfo[iNum].fHeight	= (float)info.Height;
	d3.texInfo[iNum].usFormat   = info.Format;				// テクスチャーフォーマット
	d3.texInfo[iNum].bOffscreen = false;
	d3.texInfo[iNum].fU		    = d3.texInfo[iNum].fOriWidth  / d3.texInfo[iNum].fWidth;
	d3.texInfo[iNum].fV		    = d3.texInfo[iNum].fOriHeight / d3.texInfo[iNum].fHeight;
	d3.texInfo[iNum].bLoad		= true;						// 読み込みフラグオン

	strcpy(d3.texInfo[iNum].strFullPath, strPath);			// ファイル名保存
	d3.bReRender = true;									// 再レンダリング要求

	//--- タイムスタンプ登録 ----------------------------------------
	sys_timestamp_get(&d3.texInfo[iNum].ftLastWrite, d3.texInfo[iNum].strFullPath);

	return iNum;											// 読み込めたので読み込み番号を返す
}



/*----------------------------------------------------------------------*
| <<< テクスチャー読みこみ >>>
|	入力	iNum		= 読み込み番号
|			file		= 読み込みファイル名
|	戻り値	-1  == 読み込んでない
|			 0  != 読み込み番号
*----------------------------------------------------------------------*/
int		d3_tex_load(int iNum, char *strFile)
{
	return	d3_tex_load(iNum, strFile, !sys.bAfterLoad, true);
}

int		d3_tex_load(int iNum, char *strFile, bool bLoadAtOnce)
{
	return	d3_tex_load(iNum, strFile, bLoadAtOnce, true);
}



/*----------------------------------------------------------------------*
| <<< ボリュームテクスチャー読みこみ >>>
|	入力	iVol    = 読み込み番号
|			strFile = 読み込みファイル名
|	戻り値	-1  == 読み込んでない
|			 0  != 読み込み番号
*----------------------------------------------------------------------*/
int		d3_vol_load(int iVol, char *strFile)
{


	SAFE_RELEASE(d3.vol[iVol]);

	char	path[_MAX_PATH + 1];

	if(strFile != NULL)
	{
		//--- ファイル名を作成 ------------------------------------------
		char	name[_MAX_FNAME	+ 1];
		char	ext[ _MAX_EXT	+ 1];
		sys_div_path(path, name, ext, strFile);
		sprintf(path, "%s%s%s",	path, name,	ext);

		D3DXCreateVolumeTextureFromFile(d3.dev, path, &d3.vol[iVol]);

		if(d3.vol[iVol] == NULL) return sys_error("d3_vol_load: iVol番 (%s) が読み込めませんでした。", strFile);

		//strcpy(d3.texInfo[iNum].strFullPath, path);				// ファイル名保存

		//--- タイムスタンプ登録 ----------------------------------------
		//sys_timestamp_get(&d3.texInfo[iNum].ftLastWrite, d3.texInfo[iNum].strFullPath);

		return iVol;
	}
	return -1;
}



/*----------------------------------------------------------------------*
| <<< テクスチャー全解放 >>>
*----------------------------------------------------------------------*/
void	d3_tex_free_all(void)
{
	for(int	i =	0; i < D3_TEX_MAX;	++i)
	{
		if(d3.texInfo[i].bOffscreen)
		{
			d3_offscreen_free(i);
		}
		else if(d3.tex[i])
		{
			SAFE_RELEASE(d3.tex[i]);
			d3.texInfo[ i].strFullPath[0] = NULL;
			d3.meshInfo[i].bResource	  = false;
			d3.meshInfo[i].bLoad		  = false;
		}
	}
}



/*----------------------------------------------------------------------*
| <<< テクスチャー全解放 >>>
|	入力	iNum = 開放番号			※ -1 なら全開放
*----------------------------------------------------------------------*/
void	d3_tex_free(int iNum)
{
	if(iNum == -1)						// 全開放
	{
		d3_tex_free_all();
	}
	else
	{
		d3.texInfo[ iNum].bLoad						 = false;
		d3.texInfo[ iNum].strFullPath[0]			 = NULL;
		d3.texInfo[ iNum].ftLastWrite.dwLowDateTime  = 0;
		d3.texInfo[ iNum].ftLastWrite.dwHighDateTime = 0;
		d3.texInfo[iNum].bResource					 = false;
		d3.texInfo[iNum].bLoad						 = false;
		if(d3.texInfo[iNum].bOffscreen)
		{
			d3_offscreen_free(iNum);
		}
		else
		{
			SAFE_RELEASE(d3.tex[iNum]);
		}
	}
}



/*----------------------------------------------------------------------*
| <<< テクスチャー領域作成 >>>			※　1024以上は禁物
|	入力	iNum = テクスチャーＩＤ
|			w, h = 領域のサイズ
|			fm	 = 作成したい画像フォーマット
*----------------------------------------------------------------------*/
bool	d3_tex_create(int iNum, int w, int h, D3DFORMAT fmt)
{

	uint	i;

	SAFE_RELEASE(d3.tex[iNum]);
	d3.texInfo[iNum].strFullPath[0]	= NULL;

	if(w > (int)d3.caps.MaxTextureWidth	){ w = d3.caps.MaxTextureWidth;}
	if(h > (int)d3.caps.MaxTextureHeight){ h = d3.caps.MaxTextureHeight;}
	for(i =	1; i <=	d3.caps.MaxTextureWidth; i *= 2)	// テクスチャーのサイズを補正
	{
		if(w <=	(int)i)
		{
			w =	i;
			break;
		}
	}
	for(i =	1; i <=	d3.caps.MaxTextureHeight; i	*= 2)	// テクスチャーのサイズを補正
	{
		if(h <=	(int)i)
		{
			h =	i;
			break;
		}
	}

	d3.texInfo[iNum].fWidth	 = 0;
	d3.texInfo[iNum].fHeight = 0;

	if(d3.d->CheckDeviceFormat(d3.ucAdapter, d3.deviceType,	d3.fmtBackBuffer, 0, D3DRTYPE_TEXTURE, fmt)	== D3D_OK)
	{										// 希望通りのテクスチャーを作成してみる
		if(FAILED(D3DXCreateTexture(d3.dev,	w,	h, 1, 0, fmt, D3DPOOL_MANAGED, &d3.tex[iNum])))
		{									// 失敗した
			return false;
		}
		else								// 成功した
		{
			d3.texInfo[iNum].fWidth	    = (float)w;
			d3.texInfo[iNum].fHeight    = (float)h;
			d3.texInfo[iNum].fOriWidth	= (float)w;
			d3.texInfo[iNum].fOriHeight = (float)h;
			d3.texInfo[iNum].usFormat   = fmt;
			d3.texInfo[iNum].bOffscreen	= false;
			return true;
		}
	}
											// テクスチャーモードを	R5G6B5 とする
											// 失敗することはまず無いはず
	if(FAILED(D3DXCreateTexture(d3.dev,	w, h, 1, 0,	D3DFMT_R5G6B5, D3DPOOL_MANAGED,	&d3.tex[iNum])))
	{
		return false;						// それでも失敗した。
	}
	d3.texInfo[iNum].fWidth		= (float)w;
	d3.texInfo[iNum].fHeight	= (float)h;
	d3.texInfo[iNum].fOriWidth	= (float)w;
	d3.texInfo[iNum].fOriHeight = (float)h;
	d3.texInfo[iNum].usFormat	= D3DFMT_R5G6B5;
	d3.texInfo[iNum].bOffscreen	= false;
	return true;
}



/*------------------------------------------------------------------------------*
| <<< オフスクリーンの指定座標を得る >>>
|	入力	iNum = 取り込んで転送するテクスチャー番号
*------------------------------------------------------------------------------*/
static	LPDIRECT3DSURFACE9	suf = NULL;
static	LPDIRECT3DSURFACE9	pSurface;
DWORD	*d3_offscreen_capture_begin(int iNum)
{

	D3DSURFACE_DESC		desc;
	D3DLOCKED_RECT		rect;

	if(d3.tex[iNum] == NULL){ return NULL;}

	d3.tex[iNum]->GetLevelDesc(0, &desc);	// レンダーテクスチャのフォーマットを取得
											// テクスチャをサーフェイスに
	d3.tex[iNum]->GetSurfaceLevel(0, &pSurface);
											// レンダーテクスチャと同じフォーマットのサーフェイスをシステムメモリに確保
	HRESULT	b = d3.dev->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format, D3DPOOL_SYSTEMMEM, &suf, NULL);
											// レンダータゲットのサーフェイスをシステムメモリに転送
	d3.dev->GetRenderTargetData(pSurface, suf);

	suf->GetDesc(&desc);

	suf->LockRect(&rect, NULL, 0);			// サーフェイスロック

	#if 0
		// 使い方の例
		DWORD	*buf = (DWORD*)rect.pBits;				//ロックした矩形の先頭のポインタ
		for(uint y = 0; y < desc.Height; y++)
		{
			for(uint x = 0; x < desc.Width; x++)
			{
				if(buf[x]==0xff000000) buf[x] = 0xffffffff;	//黒を白に
				else if(buf[x]==0xffffffff) buf[x] = 0xff000000;	//白を黒に
			}
			buf += rect.Pitch / sizeof(DWORD);	//改行
		}
	#endif

	return	(DWORD*)rect.pBits;
}


/*------------------------------------------------------------------------------*
| <<< 画面をオフスクリーンの指定座標を得る >>>
*------------------------------------------------------------------------------*/
void	d3_offscreen_capture_end(void)
{

	suf->UnlockRect();
											// これでレンダリングテクスチャにに書き戻す
	d3.dev->UpdateSurface(suf, NULL, pSurface, NULL);
	SAFE_RELEASE(pSurface);
	SAFE_RELEASE(suf);						// 終わったら解放
}



/*------------------------------------------------------------------------------*
| <<< 画面をキャプチャーする >>>

|	入力	iNum = 取り込んで転送するテクスチャー番号
*------------------------------------------------------------------------------*/
void	d3_screen_capture(int iNum)
{
	// サーフェイス→テクスチャーの転送がうまく行かないので、
	// いったんファイルに保存して対応

	d3_offscreen_free(iNum);


	LPDIRECT3DSURFACE9	pScrSurface;	//サーフェイス
	HRESULT				hr;

	hr = d3.dev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pScrSurface);

	// BMPにセーブ
	FOLDER_SET();
	hr = D3DXSaveSurfaceToFile("screen.bmp", D3DXIFF_BMP, pScrSurface, NULL, NULL);
	
	d3_tex_free(iNum);
	d3_tex_load(iNum, "screen.bmp");
//	remove("screen.bmp");

	//d3_offscreen_create(iNum, SCR_W, SCR_H);
	//d3_offscreen_begin(iNum);
	//D3_CLEAR(D3_COL(255, 0,255, 0));

	//LPDIRECT3DSURFACE9	pDstSurface;	//サーフェイス
											// テクスチャをサーフェイスに
	//d3.tex[iNum]->GetSurfaceLevel(0, &pDstSurface);

	//RECT	r;
	//SetRect(&r,  0, 0, SCR_W, SCR_H);
	//d3.dev->StretchRect(pSurface, NULL, pDstSurface, NULL, D3DTEXF_NONE);
	//d3_offscreen_end(  iNum);

	// バックバッファを表示
	//d3.dev->Present(NULL, NULL, NULL, NULL);
	
	SAFE_RELEASE(pScrSurface);
	//SAFE_RELEASE(pDstSurface);


// たぶん、どうさしない
	/*
	//--- サーフェイスからバッファへコピー -----------------------
	LPDIRECT3DSURFACE9	pSrc  =	NULL, pDst = NULL;
	RECT				rect  =	{0,	0, SCR_W - 1, SCR_H	- 1};
	POINT				point =	{0,	0};

	d3_tex_create(iNum, SCR_W, SCR_H, d3.fmtBackBuffer);
	d3.dev->GetRenderTarget(0, &pSrc);
	d3.tex[iNum]->GetSurfaceLevel(0,	&pDst);
	d3.dev->UpdateSurface(pSrc,	&rect, pDst, &point);

	SAFE_RELEASE(pSrc);
	SAFE_RELEASE(pDst);
	*/
}



/**************************************************************************************************
	[[[	オフスクリーン系 ]]]
**************************************************************************************************/


/*----------------------------------------------------------------------*
| <<< 指定オフスクリーンを開放 >>>
|	入力	iNum   = テクスチャー番号
*----------------------------------------------------------------------*/
void	d3_offscreen_free(int iNum)
{

	if(d3.texInfo[iNum].bOffscreen)
	{
		d3.texInfo[iNum].bOffscreen     = false;
		d3.texInfo[iNum].strFullPath[0] = NULL;
		SAFE_RELEASE(d3.lpOffscreenZBuffer[iNum]);
		SAFE_RELEASE(d3.tex[iNum]);
	}
}



/*----------------------------------------------------------------------*
| <<< オフスクリーンを開放 >>>
*----------------------------------------------------------------------*/
void	d3_offscreen_free(void)
{

	for(int	i =	0; i < D3_TEX_MAX; i++)
	{
		if(d3.texInfo[i].bOffscreen)
		{
			d3.texInfo[i].bOffscreen = false;
			SAFE_RELEASE(d3.lpOffscreenZBuffer[i]);
			SAFE_RELEASE(d3.tex[i]);
		}
	}
}



/*----------------------------------------------------------------------*
| <<< オフスクリーンをファイルへ保存 >>>	※ BMP 形式
|	入力	iNum    = ファイル番号
|			strFile = ファイル名(拡張を付けてください)
*----------------------------------------------------------------------*/
void	d3_offscreen_save(int iNum, char *strFile)
{

	if(d3.texInfo[iNum].bRepair &&	d3.texInfo[iNum].bOffscreen)
	{
		D3DXSaveTextureToFile(strFile, D3DXIFF_BMP, d3.tex[iNum], NULL);
	}
}



/*----------------------------------------------------------------------*
| <<< オフスクリーンのサーフェイスに読み込み >>>	※ BMP 形式
|	入力	iNum    = ファイル番号
|			strFile = ファイル名(拡張を付けてください)
*----------------------------------------------------------------------*/
void	d3_offscreen_load(int iNum, char *strFile)
{

	d3_render_begin_scene();
	d3_stat_zread_set(  false);
	d3_stat_zwrite_set( false);
	d3_light_set(       false);
	d3_stat_culling_set(false);

	sys_folder_push();
	sys_folder_set();

	if(d3.texInfo[iNum].bOffscreen)
	{
		LPDIRECT3DTEXTURE9		tex;
		char					str[MAX_PATH + 1];
		sprintf(str, "%04d.bmp", iNum);
		D3DXCreateTextureFromFileEx(
					d3.dev,							// デバイス
					str,							// 読み込みファイル名
					D3DX_DEFAULT, D3DX_DEFAULT,		// 画像のサイズ
					1,								// ミップマップレベル
					0,								// テクスチャーの使用方法
													// テクスチャーのフォーマット
					(enum _D3DFORMAT)d3.texInfo[iNum].usFormat,
					D3DPOOL_MANAGED,				// テクスチャーの管理方法
					D3DX_FILTER_NONE,				// フィルター
					D3DX_FILTER_NONE,				// ミップマップのフィルター
					D3DCOLOR_ARGB(255, 0, 0, 0),	// 透明にする色
					NULL,							// テクスチャーの情報をえるポインタ
					NULL,							// パレットを得る
					&tex);							// テクスチャーのポインタ

		if(tex != NULL)
		{
			// テクスチャーへ転送
			d3_offscreen_begin(iNum);
			d3.dev->SetTexture(0, tex);
			d3_stat_filter_set(D3_FILTER_POINT);
			d3_stat_alpha_set( false);
			D3_2DDRAW_(&VEC3(                      0,                       0, 0),
					   &VEC3(d3.texInfo[iNum].fWidth, d3.texInfo[iNum].fHeight, 0),
					   0, 0, 1,	1, D3_COL_NORMAL);
			SAFE_RELEASE(    tex);
			d3_offscreen_end(iNum);
		}
	}

	sys_folder_pop();
	d3_stat_zread_set(  true);
	d3_stat_zwrite_set( true);
	d3_light_set(       true);
	d3_stat_culling_set(true);
	d3_render_end_scene();
}



/*----------------------------------------------------------------------*
| <<< オフスクリーンを保存用メモリを解放 >>>
|	入力	iNum = テクスチャー番号
*----------------------------------------------------------------------*/
void	d3_offscreen_mem_free(int iNum)
{
	if(  d3.pdwOffscreenRepairBuf[iNum] == NULL){ return;}
	free(d3.pdwOffscreenRepairBuf[iNum]);
	d3.pdwOffscreenRepairBuf[     iNum] = NULL;
}

void	d3_offscreen_mem_free(void)
{
	for(int	i =	0; i < D3_TEX_MAX;	i++)
	{
		d3_offscreen_mem_free(i);
	}
}



/*----------------------------------------------------------------------*
| <<< オフスクリーンをメモリに保存する >>>
|	※メモリを確保してレンダリングした画像を保存する。d3_offscreen_repair で再転送
|	入力	iNum = テクスチャー番号
*----------------------------------------------------------------------*/
void	d3_offscreen_mem_save(int iNum)
{

	d3.bOffScreenFlagBak[iNum] = d3.texInfo[iNum].bOffscreen;
	if(d3.texInfo[iNum].bRepair &&	d3.texInfo[iNum].bOffscreen)
	{
		// メモリ解放
		d3_offscreen_mem_free(iNum);

		// メモリ取得
		int	iSize = (int)d3.texInfo[iNum].fWidth * (int)d3.texInfo[iNum].fHeight * sizeof(DWORD);
		d3.pdwOffscreenRepairBuf[iNum] = (DWORD *)malloc(iSize);

		// サーフェイス転送
		DWORD	*buf = d3_offscreen_capture_begin(iNum);
		memcpy(d3.pdwOffscreenRepairBuf[iNum], buf, iSize);
		d3_offscreen_capture_end();
	}
}

void	d3_offscreen_mem_save(void)
{
	for(int	i =	0; i < D3_TEX_MAX;	i++)
	{
		d3_offscreen_mem_save(i);
	}
}



/*----------------------------------------------------------------------*
| <<< オフスクリーンを修復 >>>
|	※d3_offscreen_saveで退避したレンダリングした画像をオフスクリーンへ転送する
*----------------------------------------------------------------------*/
static	void	d3_offscreen_repair(void)
{

	//--- オフスクリーンを作成 --------------------------------------
	int	i;
	for(i =	0; i < D3_TEX_MAX;	i++)
	{
		if(d3.bOffScreenFlagBak[i])
		{
			bool	bBak = d3.texInfo[i].bRepair;
			d3_offscreen_create(i, (int)d3.texInfo[i].fOriWidth, (int)d3.texInfo[i].fOriHeight, (D3DFORMAT)d3.texInfo[i].usFormat);
			d3.texInfo[i].bRepair =	bBak;
		}
	}

	//--- セーブしたオフスクリーン画像を読み込み、貼り付け ----------
	for(i =	0; i < D3_TEX_MAX;	i++)
	{
		if(d3.texInfo[i].bRepair &&	d3.texInfo[i].bOffscreen)
		{
			int	iSize = (int)d3.texInfo[i].fWidth * (int)d3.texInfo[i].fHeight * sizeof(DWORD);
			DWORD	*buf = d3_offscreen_capture_begin(i);

			memcpy(buf, d3.pdwOffscreenRepairBuf[i], iSize);
			d3_offscreen_capture_end();
		}
	}
}



/*----------------------------------------------------------------------*
| <<< オフスクリーンサーフェイスの作成 >>>
|	入力	iNum   = テクスチャー番号
|			iW,	iH = サイズ				※２～２０４８まで
|			fmt	   = 画像フォーマット
|	戻り値	false =	作成失敗
*----------------------------------------------------------------------*/
static	bool	d3_offscreen_create_(int iNum, int iW, int iH, D3DFORMAT fmt)
{

	//--- 同一条件では作成しない ----------------------------------
	if(d3.texInfo[iNum].fWidth	   == (float)iW
	&& d3.texInfo[iNum].fHeight	   == (float)iH
	&& d3.texInfo[iNum].bOffscreen == true){ return true;}

	HRESULT	hr;
	int		i;

	if(iW >	(int)d3.caps.MaxTextureWidth ){ iW = d3.caps.MaxTextureWidth;}
	if(iH >	(int)d3.caps.MaxTextureHeight){ iH = d3.caps.MaxTextureHeight;}

	//--- オリジナルのテクスチャーサイズ ----------------------------
	int iWBak = iW;
	int iHBak = iH;

	//--- テクスチャーのサイズを補正 --------------------------------
	for(i =	2; i <=	(int)d3.caps.MaxTextureWidth;  i *=	2){	if(iW <= i){iW = i;	break;}}
	for(i =	2; i <=	(int)d3.caps.MaxTextureHeight; i *=	2){	if(iH <= i){iH = i;	break;}}
	d3.texInfo[iNum].strFullPath[0] = NULL;
	SAFE_RELEASE(d3.tex[iNum]);					// 予め読み込まれてたテクスチャーは開放する
	SAFE_RELEASE(d3.lpOffscreenZBuffer[iNum]);	// ステンシルも開放する

	//D3DFORMAT	fmt	= d3_tex_format_search(1);
	if(fmt == D3DFMT_UNKNOWN){ return false;}	// 画像フォーマット未対応で作成できず。
	hr = d3.dev->CreateTexture(iW, iH, 1, D3DUSAGE_RENDERTARGET, fmt, D3DPOOL_DEFAULT, &d3.tex[iNum], NULL);
	if(FAILED(hr)){ return sys_error("オフスクリーン %d 番の作成に失敗しました。", iNum);}
												// ステンシルはバックバッファと同じものを使う
	hr = d3.dev->CreateDepthStencilSurface(iW, iH, d3.fmtStencil, D3DMULTISAMPLE_NONE, 0, true,	&d3.lpOffscreenZBuffer[iNum], NULL);	
	if(FAILED(hr)){ return sys_error("オフスクリーン %d 番のステンシルバッファの作成に失敗しました。", iNum);}
	d3.texInfo[iNum].fWidth	    = (float)iW;
	d3.texInfo[iNum].fHeight	= (float)iH;
	d3.texInfo[iNum].fOriWidth  = (float)iWBak;
	d3.texInfo[iNum].fOriHeight	= (float)iHBak;

	d3.texInfo[iNum].fU			= (float)iWBak / (float)iW;
	d3.texInfo[iNum].fV			= (float)iHBak / (float)iH;
	d3.texInfo[iNum].bOffscreen = true;
	d3.texInfo[iNum].bRepair	= false;
	d3.texInfo[iNum].usFormat   = fmt;

	d3_offscreen_begin(iNum);
	d3.dev->Clear(0, NULL, D3DCLEAR_TARGET,	0, 1.0f, D3_COL_BLACK(255));
	d3_offscreen_end(  iNum);
	return true;
}



/*----------------------------------------------------------------------*
| <<< オフスクリーンサーフェイスの作成 >>>
|	入力	iNum   = テクスチャー番号
|			iW,	iH = サイズ				※２～２０４８まで
|	戻り値	false =	作成失敗
*----------------------------------------------------------------------*/
bool	d3_offscreen_create(int iNum, int iW, int iH, D3DFORMAT	fmt)
{
	return d3_offscreen_create_(iNum, iW, iH, fmt);
}



/*----------------------------------------------------------------------*
| <<< オフスクリーンサーフェイスの作成 >>>
|	入力	iNum   = テクスチャー番号
|			iW,	iH = サイズ				※２～２０４８まで
|	戻り値	false =	作成失敗
*----------------------------------------------------------------------*/
bool	d3_offscreen_create(int iNum, int iW, int iH)
{
	return d3_offscreen_create_(iNum, iW, iH, d3_tex_format_search(1));
}



/*----------------------------------------------------------------------*
| <<< オフスクリーンサーフェイス開始 >>>
|	入力	iNum = テクスチャー番号
*----------------------------------------------------------------------*/
#ifdef	__2D_RESIZE
	static	float	fZoomBak;
	static	VEC2	vecBak;
#endif

bool	d3_offscreen_begin(int iNum)
{

	if(d3.tex[iNum] == NULL){ return sys_error("オフスクリーンサーフェイス %d 番が作成されていません。", iNum);}

	if(d3.iOffscreenBackUpCount	> D3_OFFSCREEN_BAK_MAX - 1){ return sys_error("オフスクリーンサーフェイスの入れ個数がオーバーしています。");}

	#ifdef	__2D_RESIZE
		if(d3.iOffscreenBackUpCount == 0)
		{
			fZoomBak  = d3.fZoom;
			vecBak	  = d3.vecOfs;
			d3.fZoom  = 1;
			d3.vecOfs = VEC2(0, 0);
		}
	#endif

	// 入れ子になっているやつをずらす
	d3.iOffscreenBackUpCount++;
	for(int	i =	0; i < d3.iOffscreenBackUpCount; i++)
	{
		d3.lpOffscreenOldSurface[i + 1]	= d3.lpOffscreenOldSurface[i];
		d3.lpOffscreenOldZBuffer[i + 1]	= d3.lpOffscreenOldZBuffer[i];
		d3.lpOffscreenNewTarget[ i + 1]	= d3.lpOffscreenNewTarget[ i];
	}
											// 現在のスクリーン設定を退避
	d3.dev->GetRenderTarget(0, &d3.lpOffscreenOldSurface[	 0]);
	d3.dev->GetDepthStencilSurface(&d3.lpOffscreenOldZBuffer[0]);
											// 新しいスクリーン設定をセット
	d3.tex[iNum]->GetSurfaceLevel(0, &d3.lpOffscreenNewTarget[0]);

	HRESULT	hr;
	hr = d3.dev->SetRenderTarget(0,	d3.lpOffscreenNewTarget[0]);
	hr = d3.dev->SetDepthStencilSurface(d3.lpOffscreenZBuffer[iNum]);
	//HRESULT	hr = d3.dev->SetRenderTarget(d3.lpOffscreenNewTarget[0], d3.lpOffscreenZBuffer[no]);
	if(FAILED(hr)){ return sys_error("オフスクリーンサーフェイス %d 番が開始できません。", iNum);}

	d3.dev->BeginScene();					// シーンの開始
	return true;
}



/*----------------------------------------------------------------------*
| <<< オフスクリーンサーフェイス終了 >>>
|	入力	iNum = テクスチャー番号
*----------------------------------------------------------------------*/
void	d3_offscreen_end(int iNum)
{

	if(d3.tex[iNum] == NULL){ return;}

	d3.dev->EndScene();
	HRESULT	hr;
	hr = d3.dev->SetRenderTarget(    0,	d3.lpOffscreenOldSurface[0]);
	hr = d3.dev->SetDepthStencilSurface(d3.lpOffscreenOldZBuffer[0]);
	//HRESULT	hr = d3.dev->SetRenderTarget(d3.lpOffscreenOldSurface[0], d3.lpOffscreenOldZBuffer[0]);
	if(hr != S_OK){ sys_error("d3_offscreen_end でエラー テクスチャー番号は %d", iNum);}
											// 入れ子になっているやつをずらす
	SAFE_RELEASE(d3.lpOffscreenNewTarget[ 0]);
	SAFE_RELEASE(d3.lpOffscreenOldSurface[0]);
	SAFE_RELEASE(d3.lpOffscreenOldZBuffer[0]);
	
	d3.iOffscreenBackUpCount--;
	for(int	i =	0; i < d3.iOffscreenBackUpCount; i++)
	{
		d3.lpOffscreenOldSurface[i]	= d3.lpOffscreenOldSurface[i + 1];
		d3.lpOffscreenOldZBuffer[i]	= d3.lpOffscreenOldZBuffer[i + 1];
		d3.lpOffscreenNewTarget[ i]	= d3.lpOffscreenNewTarget[ i + 1];
	}
	d3.lpOffscreenOldSurface[D3_OFFSCREEN_BAK_MAX - 1]	= NULL;
	d3.lpOffscreenOldZBuffer[D3_OFFSCREEN_BAK_MAX - 1]	= NULL;

	#ifdef	__2D_RESIZE
		if(d3.iOffscreenBackUpCount == 0)
		{
			d3.fZoom  = fZoomBak;
			d3.vecOfs = vecBak;
		}
		d3.viewport.X      = (DWORD)d3.vecOfs.x;
		d3.viewport.Y      = (DWORD)d3.vecOfs.y;
		d3.viewport.Width  = (DWORD)(__SCR_W * d3.fZoom);
		d3.viewport.Height = (DWORD)(__SCR_H * d3.fZoom);
		d3.viewport.MinZ   = 0.0f;
		d3.viewport.MaxZ   = 1.0f;
		d3.dev->SetViewport(&d3.viewport);
	#endif

}



/**************************************************************************************************
	[[[	初期化、終了系 ]]]
**************************************************************************************************/


/*----------------------------------------------------------------------*
| <<< ビューポート初期化 >>>			※起動時、ウィンドウサイズ変更時に呼ぼう！
*----------------------------------------------------------------------*/
void	d3_view_port_set(void)
{

	d3.viewport.X	   = 0;
	d3.viewport.Y	   = 0;
	d3.viewport.Width  = d3.para.BackBufferWidth;	// フルスクリーン時を考慮して d3.sRender は使わない
	d3.viewport.Height = d3.para.BackBufferHeight;
	d3.viewport.MinZ   = 0.0f;
	d3.viewport.MaxZ   = 1.0f;
	d3.dev->SetViewport(&d3.viewport);
}



/*------------------------------------------------------------------------------*
| <<< 使用可能なデプス／ステンシルバッファのフォーマットを検索 >>>
*------------------------------------------------------------------------------*/
static	D3DFORMAT	d3_search_stencil_format(D3DFORMAT format)
{

	HRESULT	hr;
	

	D3DFORMAT	tbl[] =
	{
		D3DFMT_D15S1,
		D3DFMT_D24S8,
		D3DFMT_D24X4S4,
		D3DFMT_D24X8,
		D3DFMT_D16,
		D3DFMT_D32,
		D3DFMT_D16_LOCKABLE,
	};

	for(int	i =	0; i < 7; ++i)
	{
		hr = d3.d->CheckDeviceFormat(d3.ucAdapter, D3DDEVTYPE_HAL, format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE,	tbl[i]);
		if(SUCCEEDED(hr))
		{
			hr = d3.d->CheckDepthStencilMatch(d3.ucAdapter,	D3DDEVTYPE_HAL,	format,	format,	tbl[i]);
			if(SUCCEEDED(hr)) return tbl[i];
		}
	}

	return D3DFMT_UNKNOWN;
}



/*------------------------------------------------------------------------------*
| <<< レンダリング中のデバイスの能力を出力 >>>y
|	入力、出力	*str = デバイスの能力を記した文字列
*------------------------------------------------------------------------------*/
void	d3_caps_get(char *str)
{

	char	d3_type_str[2][17]	= 				// デバイスタイプ	HAL	or REF
	{
		"D3DDEVTYPE_HAL",
		"D3DDEVTYPE_REF"
	};
	char	d3_processing_str[3][38] =			// バーテックスタイプ
	{
		"D3DCREATE_HARDWARE_VERTEXPROCESSING",
		"D3DCREATE_MIXED_VERTEXPROCESSING",
		"D3DCREATE_SOFTWARE_VERTEXPROCESSING"
	};
	char	yes_no[2][9] =
	{
		"使えない",
		"使える"
	};

	int	process;
	if(d3.ucVertexProcessing ==	D3DCREATE_HARDWARE_VERTEXPROCESSING){ process = 0;}
	if(d3.ucVertexProcessing ==	D3DCREATE_MIXED_VERTEXPROCESSING   ){ process = 1;}
	else															{ process = 2;}
	int	device;
	if(d3.deviceType ==	D3DCREATE_HARDWARE_VERTEXPROCESSING){ device = 0;}
	if(d3.deviceType ==	D3DCREATE_MIXED_VERTEXPROCESSING   ){ device = 1;}
	else													{ device = 2;}

	sprintf(str,
		"%s\n%s\n"
		"頂点シェーダーver = %2d.%02d\n"
		"最大プリミティブ  = %d\n" 
		"最大ライト        = %d\n"
		"アンチエイリアス  = %s\n"
		//"フォグ:\n"
		//"   頂点     = %s\n"
		//"   ピクセル = %s\n"
		"\n"
		"テクスチャー:\n"
		"   利用可能メモリ   = %d KB\n"
		"   最大サイズ       = %d x %d\n"
		"   最大ステージ     = %d\n"
		"   アルファブレンド = %s\n"
		"   ミップマップ     = %s\n"
		"   キューブマップ   = %s\n",
		d3_type_str[	   device],
		d3_processing_str[process],
		(d3.caps.VertexShaderVersion & 0xff00) >> 8, (d3.caps.VertexShaderVersion &	0xff),
		d3.caps.MaxPrimitiveCount,
		d3.caps.MaxActiveLights,
		yes_no[((d3.caps.RasterCaps	& D3DPRASTERCAPS_ANISOTROPY) !=	0)],
		//yes_no[(d3.bFog[0] == true)],
		//yes_no[(d3.bFog[1] == true)],
		(d3.dev->GetAvailableTextureMem() /	1024),
		d3.caps.MaxTextureWidth,
		d3.caps.MaxTextureHeight,
		d3.caps.MaxSimultaneousTextures,
		yes_no[((d3.caps.TextureCaps	& D3DPTEXTURECAPS_ALPHA	 ) != 0)],
		yes_no[((d3.caps.TextureCaps	& D3DPTEXTURECAPS_MIPMAP ) != 0)],
		yes_no[((d3.caps.TextureCaps	& D3DPTEXTURECAPS_CUBEMAP) != 0)]
	);
}



/*------------------------------------------------------------------------------*
| <<< ディスプレイアダプタのバラメーター作成 >>>&
|	入力	bWindow	= true ならばウィンドウ処理を行う
*------------------------------------------------------------------------------*/
static	void	DisplaySet(bool	bWindow)
{

	ZeroMemory(&d3.para, sizeof(d3.para));
	d3.para.MultiSampleType	   = D3DMULTISAMPLE_NONE;
	d3.para.MultiSampleQuality = 0;

	// こちらは画像キャプチャーに使用(d3.rectRenderは使用できなくなります)
	// ようは FILPを使わないとバッファに絵がたまらないという理屈
	// D3DSWAPEFFECT_FLIP		画面のキャプチャー可
	// D3DSWAPEFFECT_COPY		d3.rectRender を使用可(ツールなど)
	
	d3.para.SwapEffect         = D3DSWAPEFFECT_COPY;//(enum _D3DSWAPEFFECT)d3.dwSwapEffectMode;
	d3.para.BackBufferCount	   = 1;

	d3.para.hDeviceWindow	   = sys.hWnd;
	d3.para.Flags			   = 0;
	if(bWindow)							// ウィンドウモード
	{
		d3.para.BackBufferWidth			   = d3.sRenderW;
		d3.para.BackBufferHeight		   = d3.sRenderH;
		d3.para.Windowed				   = true;
		d3.para.BackBufferFormat		   = d3.fmtBackBuffer;
		d3.para.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
											// ウィンドウモード時の画面更新設定
											// この設定は vsync を待たない
											// そもそも、ウィンドウモード時は vsync を待てない
#ifdef	__NON_VSYNC_WAIT
		d3.para.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
#else
		d3.para.PresentationInterval       = D3DPRESENT_INTERVAL_DEFAULT;
#endif
	}
	else
	{									// フルスクリーン
		//--- 以下でビデオボードがサポートしている画面モードよりフルスクリーン時の解像度を求める
		D3DDISPLAYMODE	mode;
		int				iMax = d3.d->GetAdapterModeCount(d3.ucAdapter, d3.fmtDisplay);
		for(int	i =	0; i < iMax; i++)
		{
			if(d3.d->EnumAdapterModes(d3.ucAdapter,	d3.fmtDisplay, i, &mode) == D3D_OK)
			{
				if(mode.RefreshRate	!= D3_HZ    ){ continue;}	// リフレッシュレートを	60MHz固定
				if(mode.Height		<  D3_REZO_H){ continue;}	// 解像度を縦 480 以下は無視

		#ifdef	__RENDER_FIXATION
				if(mode.Width  >= (ushort)d3.sFullScreenRenderW
				&& mode.Height >= (ushort)d3.sFullScreenRenderH
		#else
				if(mode.Width  >= (ushort)d3.sRenderW
				&& mode.Height >= (ushort)d3.sRenderH
		#endif
				)
				{
					break;
				}
			}
		}
		//-----------------------------------------------------------
		d3.para.BackBufferWidth			   = mode.Width;
		d3.para.BackBufferHeight		   = mode.Height;
		d3.para.Windowed				   = false;
		d3.para.BackBufferFormat		   = d3.fmtBackBuffer;
		d3.para.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
											// この設定の時は vsync 待ちになる(フルスクリーン時のみ)
											// この設定でないと画面の書き換えが汚い
#ifdef	__NON_VSYNC_WAIT
		d3.para.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
#else
		d3.para.PresentationInterval       = D3DPRESENT_INTERVAL_ONE;
#endif
	}

	d3.ucPresentCount			   = 0;
	d3.para.Flags				   = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	d3.para.EnableAutoDepthStencil = TRUE;
	d3.para.AutoDepthStencilFormat = d3_search_stencil_format(d3.para.BackBufferFormat);

	//--- フルスクリーン時のリサイズ値を計算 ------------------------
	// これで、NDS、iPhone 仕様の時でも画面の中央に収まる
	#ifdef	__2D_RESIZE
	{
		// 縦横の比率計算
		float	x  = (float)(__RENDER_X1 - __RENDER_X0);
		float	y  = (float)(__RENDER_Y1 - __RENDER_Y0);
		float	fW = (float)d3.para.BackBufferWidth  / x;
		float	fH = (float)d3.para.BackBufferHeight / y;
		
		// 倍率の低いものを採用
		if(fW < fH){ d3.fZoom = fW;}
		else	   { d3.fZoom = fH;}

		// 画面中央へ移動するようにオフセット値を計算
		d3.vecOfs.x = ((float)d3.para.BackBufferWidth  - (x * d3.fZoom)) * 0.5f;
		d3.vecOfs.y = ((float)d3.para.BackBufferHeight - (y * d3.fZoom)) * 0.5f;
	}
	#else
		d3.fZoom    = 1;
		d3.vecOfs.x = 0;
		d3.vecOfs.y = 0;
	#endif
}



/*------------------------------------------------------------------------------*
| <<< アダプターのパラメーターを設定する >>>
*------------------------------------------------------------------------------*/
static	D3DFORMAT	d3_display_format_tbl[]	=	// 希望するディスプレイ・フォーマット
{
	D3DFMT_X8R8G8B8, D3DFMT_R5G6B5
};
D3DFORMAT	d3_stencil_tbl[] =					// ステンシルバッファチェック用
{
	//D3DFMT_D15S1, D3DFMT_D24S8,	D3DFMT_D24X4S4,	D3DFMT_D24X8, D3DFMT_D32, D3DFMT_D16
	D3DFMT_D24S8, D3DFMT_D15S1, D3DFMT_D24X4S4,	D3DFMT_D24X8, D3DFMT_D32, D3DFMT_D16
};

void	d3_adapter_init(void)
{
										// ディスプレイの状態を得る
	d3.d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,	&d3.displayMode);
										// デスクトップの状態を得る
	d3.d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,	&d3.deskTopDisplayMode);

	if(d3.deviceType ==	0)				// 初期化されていない時には以下を実行
	{
		d3.ucAdapter		  =	D3DADAPTER_DEFAULT;
		d3.deviceType		  =	D3DDEVTYPE_HAL;
		d3.fmtDisplay		  =	d3.deskTopDisplayMode.Format;
		d3.fmtBackBuffer	  =	d3.deskTopDisplayMode.Format;
		d3.ucVertexProcessing =	D3DCREATE_SOFTWARE_VERTEXPROCESSING;

		#if	!D3_SOFTWARE_ONLY
			if((d3.caps.DevCaps	& D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0)		// T & L サポートか？
			{
				//if(d3.caps.TextureCaps & D3DPTEXTURECAPS_ALPHAPALETTE
				//|| d3.caps.TextureCaps & D3DPTEXTURECAPS_ALPHA)
				//{									// D3DCREATE_HARDWARE_VERTEXPROCESSING を追加
				//	d3.ucVertexProcessing =	D3DCREATE_HARDWARE_VERTEXPROCESSING;
				//}
				d3.ucVertexProcessing =	D3DCREATE_HARDWARE_VERTEXPROCESSING;
			}
		#endif
	}
	DisplaySet(d3.bWindow);				// ディスプレイの他のパラメーターをセット

	//--- ウィンドウ状態で起動が可能かチェックする ------------------
	if(FAILED(d3.d->CheckDeviceType(d3.ucAdapter, d3.deviceType, d3.fmtDisplay,	d3.fmtBackBuffer, true)))
	{										// フルスクリーンを調べる
		for(int	i =	0; i < D3_FMT_DISPLAY_MAX; i++)
		{
			if(SUCCEEDED(d3.d->CheckDeviceType(d3.ucAdapter, d3.deviceType,	d3_display_format_tbl[i], d3_display_format_tbl[i],	false)))
			{
				d3.fmtDisplay	 = 
				d3.fmtBackBuffer = d3_display_format_tbl[i];
				d3.bWindow		 = false;
				break;
			}
		}
	}
	//--- ステンシルバッファを調査する ------------------------------
	int			j	 = 0;
	D3DFORMAT	_1st = D3DFMT_UNKNOWN;
	int			i;
	for(i =	0; i < D3_FMT_STENCIL_MAX; i++)
	{
		if(SUCCEEDED(d3.d->CheckDeviceFormat(d3.ucAdapter, d3.deviceType, d3.fmtDisplay, D3DUSAGE_DEPTHSTENCIL,	D3DRTYPE_SURFACE, d3_stencil_tbl[i])))
		{
			if(d3.fmtStencil ==	d3_stencil_tbl[i]){ break;}
			if(_1st	== D3DFMT_UNKNOWN){ _1st = d3_stencil_tbl[i];}
		}
	}
	if(i ==	D3_FMT_STENCIL_MAX && _1st != D3DFMT_UNKNOWN){ d3.fmtStencil = _1st;}
}



/*------------------------------------------------------------------------------*
| <<< フルスクリーンに最適な画面モードを得る >>>
|
|	※この場合の最適な画面モードとはディスプレイの周波数を切り替えることのない
|	  デスクトップのサイズと同じ周波数とサイズを返す
|
|	入力	iAdapter	   = アダプター番号（０がデフォルト）
|			fmt			   = 設定したいディスプレイフォーマット()
|	出力	*sOutW, *sOutH = 最適な画面サイズ
|	戻り値	フルスクリーンの解像度を指す番号
*------------------------------------------------------------------------------*/
static	void	FullScreenSizeGet(short *sOutW, short *sOutH, int iAdapter, D3DFORMAT fmt)
{

	D3DDISPLAYMODE	mode;

	//--- デスクトップのサイズを得る
	HDC		hDC = GetDC(GetActiveWindow());
	short	sW  = GetDeviceCaps(hDC, HORZRES);	// 横
	short	sH  = GetDeviceCaps(hDC, VERTRES);	// 縦
    ReleaseDC(GetActiveWindow(), hDC);

	//--- 素早く切り替えることができるようにデスクトップと同じサイズを探す
	int	iMax   = d3.d->GetAdapterModeCount(iAdapter, fmt);
	int	iCount = 0;
	for(int	i =	0; i < iMax; i++)
	{
		if(d3.d->EnumAdapterModes(iAdapter,	fmt, i,	&mode) == D3D_OK)
		{
			if(mode.RefreshRate	!= D3_HZ    ) continue;	// リフレッシュレートを	60MHz固定
			if(mode.Height		<  D3_REZO_H) continue;	// 解像度を縦 480 以下は無視
			if(mode.Width  == sW && mode.Height == sH)
			{
				*sOutW = sW;
				*sOutH = sH;
				return;
			}
		}
	}

	//--- デスクトップと同じサイズが見当たらないので無難なモードを返す
	*sOutW = 640;
	*sOutH = 480;
}



/*------------------------------------------------------------------------------*
| <<< DirectGraphics 初期化	>>>
|	入力	bWindow	= TRUE or FALSE
*------------------------------------------------------------------------------*/
bool	d3_init(bool bWindow)
{

	//--- デバイスの初期化 ------------------------------------------
	d3.d = Direct3DCreate9(D3D_SDK_VERSION);
	if(d3.d	== NULL){ return sys_error("d3_init : 初期化に失敗しました。終了します。");}
											// アダプターの状態を得る
	d3.d->GetDeviceCaps(D3DADAPTER_DEFAULT,	D3DDEVTYPE_HAL,	&d3.caps);

#ifdef	__VS_VERSION
	if(d3.caps.VertexShaderVersion < __VS_VERSION)
	{
		//if(d3.caps.MaxVertexBlendMatrices < 4){ return sys_error("ボーンが使えません。終了します");}

		#ifdef	UNDER_20_OPERATION
			sys_error("このビデオボードは頂点シェーダー2.0に対応していません。\r\n３Ｄ表示の簡略化を行います。");
			d3.bLegacy = true;
		#else
			return sys_error("d3_init : ビデオボードが頂点シェーダー2.0に対応していません。終了します。");
		#endif
	}
	d3.ucShaderType = D3_SHADER_TYPE_NORMAL;
#endif
											// 画面切り替え時のレスポンスを良くするために、デスクトップのサイズをフルスクリーンサイズとする
	FullScreenSizeGet(&d3.sFullScreenRenderW, &d3.sFullScreenRenderH, 0, D3_FORMAT);

	//--- デバイスの作成 ------------------------------------------
	d3.fZoom = 1;							// フルスクリーン時の倍率(d3_initの中で計算を行うので初期化はここで)
	d3_adapter_init();						// アダプターのパラメーターを設定する
	if(FAILED(d3.d->CreateDevice(d3.ucAdapter, d3.deviceType, sys.hWnd, d3.ucVertexProcessing, &d3.para, &d3.dev)))
	{
		return sys_error("d3_init : Direct3D デバイスの作成に失敗しました。\r\n終了します。");
	}
	d3.bPresent		 = true;				// メインウィンドウに画像の転送を行う
	d3.bPaintPresent = true;				// WM_PAINT 時に画像の転送を行う

	//--- 予め色々な視点の行列を作成しておく ------------------------
	D3DXMatrixIdentity(&d3.mtxIdentity);	// 単位行列

	//--- カメラ設定 ------------------------------------------------
	d3.vecCamera = VEC3(0, 0, -10);			// デフォルトのカメラ座標
	d3.vecEye	 = VEC3(0, 0,	0);			//			   注視座標
	d3.vecUp	 = VEC3(0, 1,   0);			// カメラの向き

	//--- ビューボート作成 ------------------------------------------
	d3_view_port_set();

	//--- ライト設定 ------------------------------------------------
	memset(&d3.light, 0, sizeof(D3DLIGHT9));
	d3.light.Type =	D3DLIGHT_DIRECTIONAL;
	VEC4	*a;
	a =	(VEC4 *)&d3.light.Diffuse;  *a = VEC4(1.0f, 1.0f, 1.0f, 1.0f);	// rgba	の順番
	a =	(VEC4 *)&d3.light.Specular; *a = VEC4(0.5f, 0.5f, 0.5f, 0.5f);	// rgba
	a =	(VEC4 *)&d3.light.Ambient;  *a = VEC4(0.2f, 0.2f, 0.2f, 0.2f);	// rgba
	d3.light.Direction = VEC3( 1.0f, -1.f,  1.0f);
	d3.light.Position  = VEC3(-1.0f, -1.f, -1.0f);
	d3.light.Range	   = 1000.0f;
	d3.fLightIntensity =    5.0f;
	d3_light_set(true);

	//--- 環境光 ----------------------------------------------------
	{
		float	a	  = 0.1f;				// これがデフォルトの環境光
		int		b	  = (int)(255.f * 0.1f);
		d3.dev->SetRenderState(D3DRS_AMBIENT, D3_COL(b, b, b, b));
		d3.vecAmbient = VEC4(a, a, a, a);
	}
	//--- マテリアル設定 --------------------------------------------
	D3DMATERIAL9	*m = &d3.matNormal;
	m->Diffuse.r  =	1.0f; m->Diffuse.g	= 1.0f;	m->Diffuse.b  =	1.0f; m->Diffuse.a	= 1.0f;
	m->Ambient.r  =	1.0f; m->Ambient.g	= 1.0f;	m->Ambient.b  =	1.0f; m->Ambient.a	= 1.0f;
	m->Specular.r =	1.0f; m->Specular.g	= 1.0f;	m->Specular.b =	1.0f; m->Specular.a	= 1.0f;
	m->Emissive.r =	1.0f; m->Emissive.g	= 1.0f; m->Emissive.b =	1.0f; m->Emissive.a = 1.0f;
	m->Power	  =	5.0f;

	//--- シェーダー設定 --------------------------------------------
#ifdef	__VS_VERSION
	d3_tex_load(TEX_WHITE, "WHITE", true);
	d3_shader_set(D3_SHADER_TYPE_NORMAL);	// ノーマルタイプのみコンパイルしておく
#endif
	d3_shader_set(D3_SHADER_TYPE_OFF);
											// この一行が無いと面が白くなります。
	d3.dev->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	d3_stat_dither_set(true);

	//--- テクスチャー関係 ------------------------------------------
	d3.fmtTexture = d3_tex_format_search(0);// 使用可能な画像フォーマットを得る
	d3_stat_filter_set(D3_FILTER_LINEAR);

	//--- ヒット球を作成 -------------------------------------------
#ifdef	__VS_VERSION
											// リソースより作成してみる
	HRESULT	h = D3DXLoadMeshFromXResource(NULL, "SPHERE", "MESH", D3DXMESH_MANAGED, d3.dev, NULL, NULL, NULL, NULL, &d3.lpMeshSphere);
											// だめならば自力で
	if(FAILED(h)){ D3DXCreateSphere(d3.dev, 1.f, 6, 6, &d3.lpMeshSphere, NULL);}
#endif

	//--- 文字フォント作成 ------------------------------------------
	d3_font_create(0, 8, 16, NULL, false);
	d3.textWork[0].bShadow = true;

	//--- 画面をクリアー --------------------------------------------
	#ifdef	__WINDOW_COLOR
		DWORD	d     = GetSysColor(COLOR_BTNFACE);
		DWORD	dwCol = D3_COL(255, (d & 255), ((d >> 8) & 255), (d >> 16));
	#else
		DWORD	dwCol = D3_COL(255, 0, 0, 0);
	#endif

	return true;
}



/*------------------------------------------------------------------------------*
| <<< DirectGraphics の終了処理	>>>
*------------------------------------------------------------------------------*/
bool	d3_end(void)
{

	SAFE_RELEASE(d3.lpMeshSphere);			// ヒット球
	d3_font_free();							// フォント開放
	d3_xfile_free_all();					// メッシュ削除
	d3_shader_free();						// シェーダー開放
	d3_offscreen_mem_free();				// オフスクリーンメモリ解放
	SAFE_RELEASE(d3.dev);
	SAFE_RELEASE(d3.d);
	return true;
}



/*------------------------------------------------------------------------------*
| <<< ウィンドウスタイル設定 >>>
|	入力	bWindow	= true ならばウィンドウ処理を行う
*------------------------------------------------------------------------------*/
static	void	WindowStyleSet(bool	bWindow)
{
//@@http://bluefish.orz.hm/sdoc/winprog_memo.html#%E3%82%A6%E3%82%A3%E3%83%B3%E3%83%89%E3%82%A6%E4%BD%8D%E7%BD%AE%E3%81%AE%E5%BE%A9%E5%85%83
//http://www.geocities.jp/KY_webid/win32c/060.html
	//--- ウィンドウ時 ----------------------------------------------
	if(bWindow)
	{
		ShowWindow(	  sys.hWnd,	SW_HIDE);
		SetWindowLong(sys.hWnd,	GWL_STYLE, WS_OVERLAPPEDWINDOW);
		
		sys.rectWindow = sys.rectWindowRestore;
		SetWindowPos( sys.hWnd,	HWND_NOTOPMOST,
						sys.rectWindowRestore.left,
						sys.rectWindowRestore.top,
						sys.rectWindowRestore.right  - sys.rectWindowRestore.left,
						sys.rectWindowRestore.bottom - sys.rectWindowRestore.top,
						SWP_HIDEWINDOW);
		sys_window_size_set();		// sys.iWidth, sys.iHeight を設定
		MENU_VIEW(true);
		if(sys.cWindowStyle == SIZE_RESTORED)
		{
			ShowWindow(sys.hWnd, SW_SHOWNORMAL);
		}
		else
		{
			ShowWindow(sys.hWnd, SW_MAXIMIZE);
		}
	}
	//--- フルスクリーン時 ------------------------------------------
	else
	{										// フルスクリーン時はシステムメニューなし
		SetWindowLong(sys.hWnd,	GWL_STYLE, WS_POPUP);
		//SetWindowPos( sys.hWnd,	HWND_NOTOPMOST,	0, 0, d3.sFullScreenRenderW, d3.sFullScreenRenderH, SWP_SHOWWINDOW);
		MENU_VIEW(false);
	}
}



/*------------------------------------------------------------------------------*
| <<< スクリーン切り替え >>>		※ d3 の各情報を見て判断する
|	入力	bStyle = true :ウィンドウスタイルを変更する
*------------------------------------------------------------------------------*/
void	d3_screen_change(bool bStyle)
{


	if(d3.dev == NULL){ return;}
	//d3_offscreen_mem_save();				// ここでオフスクリーンを保存しても、スリープ時対応できないので
											// 任意で保存をするように変更(2012.03.31)
	ShowWindow(	  sys.hWnd,	SW_HIDE);

	d3_offscreen_free();
	DisplaySet(d3.bWindow);					// 新しいディスプレイパラメーター設定
	d3.bDeviceChange = true;				// WM_DISPLAYCHANGE 割り込み禁止
	int	i;		
	for(i =	D3_SHADER_TYPE_NORMAL; i < D3_SHADER_TYPE_MAX; i++)
	{
		if(d3.lpEffect[i - D3_SHADER_TYPE_NORMAL]){ d3.lpEffect[i - D3_SHADER_TYPE_NORMAL]->OnResetDevice();}
	}
	// ここ、vistaでエラーがでる可能性有り
	for(i =	0; i < D3_FONT_MAX; i++)
	{
		if(d3.font[i]){ d3.font[i]->OnLostDevice();}
	}
	d3.dev->Reset(&d3.para);
	if(bStyle){ WindowStyleSet(d3.bWindow);}
	else	  { UpdateWindow(  sys.hWnd);}
	d3_view_port_set();
	d3_stat_filter_set(D3_FILTER_LINEAR);
	d3_shader_set(     D3_SHADER_TYPE_OFF);

	d3.dev->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);	// この一行が無いと面が白くなります。
	d3.dev->SetRenderState(D3DRS_AMBIENT,		   D3_AMBIENT);
	d3_stat_dither_set(true);
	d3_offscreen_repair();									// オフスクリーン情報修復

	//--- 画面をクリアー --------------------------------------------
	d3_render_begin();										// レンダー開始
	d3_render_end(false);									// レンダリング終了

	ShowWindow(	  sys.hWnd,	SW_SHOW);

	d3.bDeviceChange = false;								// WM_DISPLAYCHANGE 割り込み許可
}



/*------------------------------------------------------------------------------*
| <<< リロードする情報を設定する >>>	※ファイル名を取得しておく
*------------------------------------------------------------------------------*/
void	d3_reload_set(void)
{

	int	i;

	//--- テスクチャーを再読み込み設定にする ------------------------
	for(i =	0; i < D3_TEX_MAX; i++)
	{
		if(!d3.texInfo[i].bResource){ d3.texInfo[i].bLoad = false;}
	}

	//--- メッシュを再読み込み設定にする ----------------------------
	for(i =	0; i < D3_MESH_MAX; i++)
	{
		if(!d3.meshInfo[i].bResource){ d3.meshInfo[i].bLoad = false;}
	}

	//--- シェーダーを解放する --------------------------------------
	for(i = 0; i < D3_SHADER_TYPE_MAX; i++){ d3_shader_free(i);}
	d3_shader_set(D3_SHADER_TYPE_NORMAL);
	d3_shader_set(D3_SHADER_TYPE_OFF);
}



/*------------------------------------------------------------------------------*
| <<< テクスチャー設定(内部で遅延読み込みのテクスチャーを読み込んでいる >>>
*------------------------------------------------------------------------------*/
bool	d3_tex_set(int iTexNum)
{
	if(iTexNum == -1){ d3.dev->SetTexture(0, NULL); return false;}
	if(!d3.texInfo[iTexNum].bLoad && !d3.texInfo[iTexNum].bOffscreen && d3.texInfo[iTexNum].strFullPath[0] != NULL)
	{
		d3_tex_load(iTexNum, NULL, true);
	}
	d3.dev->SetTexture(0, d3.tex[iTexNum]);
	d3.iTexCurrent = iTexNum;
	return d3.tex[iTexNum] == NULL ? false: true;
}



/*------------------------------------------------------------------------------*
| <<< テクスチャーの空きを探す >>>
*------------------------------------------------------------------------------*/
int		d3_tex_unused_get(void)
{
	
	for(int i = TEX_RESERVE - 1; i != 0; i--)
	{
		if(!d3.texInfo[i].bLoad && !d3.texInfo[i].bOffscreen && d3.texInfo[i].strFullPath[0] == NULL)
		{
			return i;
		}
	}
	return -1;
}



/*------------------------------------------------------------------------------*
| <<< レンダリング終了 >>>
*------------------------------------------------------------------------------*/
void	d3_render_end(BOOL bPrint)
{
	//---------------------------------------------------------------

	#ifdef	__MOVIE
		extern	void	movie_to_texture_main(void);
		movie_to_texture_main();		// ムービーのメイン処理
	#endif

	if(bPrint){ d3_print();}

	#ifdef	__2D_RESIZE
		d3.viewport.X      = 0;
		d3.viewport.Y      = 0;
		d3.viewport.Width  = d3.sRenderW;
		d3.viewport.Height = d3.sRenderH;
		d3.viewport.MinZ   = 0.0f;
		d3.viewport.MaxZ   = 1.0f;
		d3.dev->SetViewport(&d3.viewport);

		// 余白をマスクする
		float	x0 = 0;
		float	y0 = 0;
		float	x1 = D3_CONV_X(0)      - 1;
		float	y1 = D3_CONV_Y(__SCR_H);
		float	x2 = D3_CONV_X(__SCR_W - 1);
		float	y2 = 0;
		float	x3 = (float)d3.sRenderW;
		float	y3 = y1;
		DWORD	c  = D3_COL(255, 0, 0, 0);

		d3_stat_alpha_set(  false);
		d3_stat_zwrite_set( false);
		d3_stat_zread_set(  false);
		d3_stat_culling_set(false);
		d3.dev->SetTexture( 0, NULL);
		d3.dev->SetFVF(     D3_FVF_TL);

		// 両脇マスク
		if((x1 - x0) > 0)
		{
			D3_VTX_TL_	vt0[] = {x0, y0, 0, 1, c,  x1, y0, 0, 1, c,  x1, y1, 0, 1, c,  x0, y1, 0, 1, c};
			D3_VTX_TL_	vt1[] = {x2, y2, 0, 1, c,  x3, y2, 0, 1, c,  x3, y3, 0, 1, c,  x2, y3, 0, 1, c};
			d3.dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vt0, sizeof(D3_VTX_TL));
			d3.dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vt1, sizeof(D3_VTX_TL));
			d3.iPolyCount += 2;
		}

		// 上下マスク
		y2 = y1;
		y1 = D3_CONV_Y(0);
		y3 = (float)d3.sRenderH;
		if((y1 - y0) > 0)
		{
			D3_VTX_TL_	vt0[] = {x0, y0, 0, 1, c,  x3, y0, 0, 1, c,  x3, y1, 0, 1, c, x0, y1, 0, 1, c};
			D3_VTX_TL_	vt1[] = {x0, y2, 0, 1, c,  x3, y2, 0, 1, c,  x3, y3, 0, 1, c, x0, y3, 0, 1, c};
			d3.dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vt0, sizeof(D3_VTX_TL));
			d3.dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vt1, sizeof(D3_VTX_TL));
			d3.iPolyCount += 2;
		}
		d3_stat_culling_set(true);
		d3_stat_zwrite_set( true);
		d3_stat_zread_set(  true);
		d3_stat_blend_set(  D3_BLEND_NORMAL);
	#endif

	//--- フラッシュ ------------------------------------------------
	d3_render_end_scene();
	d3_present();

	#ifdef	__2D_RESIZE
		d3.viewport.X      = (DWORD)d3.vecOfs.x;
		d3.viewport.Y      = (DWORD)d3.vecOfs.y;
		d3.viewport.Width  = (DWORD)(__SCR_W * d3.fZoom);
		d3.viewport.Height = (DWORD)(__SCR_H * d3.fZoom);
		d3.viewport.MinZ   = 0.0f;
		d3.viewport.MaxZ   = 1.0f;
		d3.dev->SetViewport(&d3.viewport);
	#endif
}

