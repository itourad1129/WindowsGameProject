//
//	DirectGraphics �֌W�̃v���O����
//

//--- ���̃t�@�C���Ɋ܂܂�鏈�� ------------------------------------
//	[[[	�Q�c�`��n ]]]
//	[[[	�R�c�`��n ]]]
//	[[[	�X�v���C�g�n ]]]
//	[[[	�����_�����O�n ]]]
//	[[[	�����n ]]]
//	[[[	�a�l�o�n ]]]
//	[[[	�e�N�X�`���[�n ]]]
//	[[[	�I�t�X�N���[���n ]]]
//	[[[	�������A�I���n ]]]

												// �œK���X�C�b�`
#define	OPTIMIZE	 D3DXMESHOPT_ATTRSORT
//D3DXMESHOPT_COMPACT	| D3DXMESHOPT_ATTRSORT;// |	D3DXMESHOPT_VERTEXCACHE

//--- ���C�u�����B --------------------------------------------------------------------------------
//d3d9.lib d3dx9.lib  dxguid.lib d3dxof.lib
#pragma	comment(lib, "d3d9.lib")
#pragma	comment(lib, "d3dx9.lib")
#pragma	comment(lib, "dxguid.lib")
#pragma	comment(lib, "d3dxof.lib")


//--- DirectX �Ǘ��p ------------------------------------------------------------------------------
D3_WORK	d3;


//--- �o���v�̃t�H�[�}�b�g ------------------------------------------------------------------------
D3DVERTEXELEMENT9	d3_decl_bump[] =
{
	{0,	 0,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0,	12,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	  0},
	{0,	24,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0},
	{0,	36,	D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	D3DDECL_END()
};



/**************************************************************************************************
	[[[	�Q�c�`��n ]]]
**************************************************************************************************/


/*------------------------------------------------------------------------------*
| <<< �Q�c�̉~��`�� >>>
|	����	pvecPos = �\�����W
|			fR		= �傫��
|			ucDiv	= ������(3 �` 36)
|			dwCol	= �F
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
	vt[i] =	vt[0];							// iDiv �ځA�~�����
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
| <<< �Q�c�̉~��`���A�h��Ԃ� >>>
|	����	pvecPos = �\�����W
|			fR		= �傫��
|			uiDiv	= ������(3 �` 36)
|			dwCol	= �F
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
	vt[i + 1] =	vt[1];							// iDiv �ځA�~�����

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
| <<< �Q�c�l�p�`�h��Ԃ��`�� >>>
|	����	(iX0, iY0)-(iX1,iY1) = �l�p�`�̍���ƉE��
|			fZ					 = Z �ʒu
|			dwCol				 = �h��Ԃ��F
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
| <<< �Q�c�l�p�`�e�N�X�`���[�\��t���`�� >>>
|	����	(iX0,iY0)�`(iX3,iY3) = �l�p�`�̎l���_
|			(iU0,iV0)�`(iU3,iV3) = UV �̎l���_
|			uiBlendMode 		= �������[�h
|			uiAlp				= (0 �` 255)�������l
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
| <<< �Q�c�l�p�`�e�N�X�`���[�\��t���`�� >>>
|	����	(iX0,iY0)-(iX1,iY1) = ���v����Ɏl���_
|			bAlphaGreat			= true : ����������؂蔲���܂�
|			uiAlp				= �A���t�@�l(0 �` 255)
*------------------------------------------------------------------------------*/
void	d3_2d_box_fill_tex_draw(int iX0, int iY0, int iX1, int iY1, int iZ, RECT *prect, uint uiBlendType, uint uiAlp)
{

	DWORD	dwCol  = D3_COL_WHITE(uiAlp);

	//--- UV �����߂� -----------------------------------------------
	float	u0, v0, u1, v1;
	if(prect != NULL)
	{							// �� 0.5f �𑫂������������� UV �̂Ȃ��ڂ��������Ă܂�
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
| <<< �Q�c�l�p�`�e�N�X�`���[�\��t���`�� >>>
|	����	(iX0,iY0),(iX1,iY1),(iX2,iY2),(iX3,iY3) = ���v����Ɏl���_
|			bAlphaGreat								= true : ����������؂蔲���܂�
|			uiAlp									= �A���t�@�l(0 �` 255)
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
| <<< �Q�c�O�p�`�̕`�� >>>
|	����	pvecPos0, pvecPos1, pvecPos2 = ���W
|			dwCol						 = �F
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
| <<< �Q�c�O�p�`�`�� >>>
|	����	pvecPos0, pvecPos1, pvecPos2 = ���W
|			dwCol						 = �F
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
| <<< �R�c�O�p�`�`�� >>>
|	����	pvecPos0, pvecPos1, pvecPos2 = ���W
|			dwCol						 = �F
*----------------------------------------------------------------------*/
void	d3_3d_triangle_fill_draw(VEC3 *pvecP0, VEC3 *pvecP1, VEC3 *pvecP2, COL *pCol)
{

	d3_3d_triangle_fill_draw(pvecP0, pvecP2, pvecP1, D3_COL((uchar)pCol->a, (uchar)pCol->r, (uchar)pCol->g, (uchar)pCol->b));
}



/*------------------------------------------------------------------------------*
| <<< ���_�����A�Q�c�}�`��`�� >>>
|	����	pvecPos    = �\�����W
|			fR		   = �傫��
|			fAngle     = ����
|			pcol	   = �F
|			iPolyCount = �|����
|			pfVertex   = ���_�e�[�u��
|			psIndex    = �C���f�b�N�X�e�[�u��
*------------------------------------------------------------------------------*/
void	d3_2d_shape_draw(VEC3 *pvecPos, float fScale, float fXScale, float fYScale, float fAngle, COL *pcol, int iPolyCount, float *pfVertex, short *psIndex)
{

	MTX		world, trans, scl, rot;


	d3_material_set(      &d3.matNormal);	// �ގ��ݒ�
	d3_stat_transform_set(&d3.mtxIdentity);

	// �s��ݒ�
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
| <<< �Q�c�ła�n�w��h��Ԃ��ăe�N�X�`���[�𒣂� >>>
|	����	pvecP0, pvecP1			= ����ƉE���̍��W
|			(fU0, fV0) - (fU1, fV1) = UV �̍���ƉE��
|			dwCol					= �F
|	 �@p0
|		*-----*
|		|     |
|		*-----*
|	          p1
|	���e�N�X�`���[�͒��O�� d3_tex_set �Ƃ��Ē�`���ꂽ���̂ɂȂ�
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
| <<< �Q�c�ła�n�w��h��Ԃ��ăe�N�X�`���[�𒣂� >>>
|	����	apvecPos = �e���_�̔z��
|			afUV     = UV �̔z��
|			dwCol	 = �F
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
|	���e�N�X�`���[�͒��O�� d3_tex_set �Ƃ��Ē�`���ꂽ���̂ɂȂ�
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
| <<< �Q�c�ła�n�w��h��Ԃ�(�e�N�X�`���[����) >>>
|	����	pvecP0, pvecP1 = ����ƉE���̍��W
|			pCol		   = �F
|	 �@p0
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
| <<< �Q�c�ła�n�w��`��(���C���̂�) >>>
|	����	pvecP0, pvecP1 = ����ƉE���̍��W
|			pCol		   = �F
|	 �@p0
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
	[[[	�R�c�`��n ]]]
**************************************************************************************************/

#if 0
	/*------------------------------------------------------------------------------*q
	| <<< ����o�^ >>>
	|	����	pvecPos   = ���W
	|			fRadius   = ���a
	|			pCol      = �F
	*------------------------------------------------------------------------------*/
	void	d3_3d_sphere_set(VEC3 *pvecPos, float fRadius, COL *pCol)
	{

		if(d3.usSphere < D3_SPHERE_MAX)
		{
			D3_SPHERE	*s = &d3.sphere[d3.usSphere];
			
			// �s��ݒ�
			MTX		trans, scale;
			D3DXMatrixScaling(    &scale, fRadius);
			D3DXMatrixTranslation(&trans, pvecPos);
			s->mtxWorld = scale * trans;

			// �F�ݒ�
			s->col = COL4(pCol->r, pCol->g, pCol->b, pCol->a) / 255.f;
			d3.usSphere++;			
		}
	}



	/*------------------------------------------------------------------------------*q
	| <<< �o�^���ꂽ����`�� >>>
	*------------------------------------------------------------------------------*/
	void	d3_3d_sphere_flush(void)
	{

		//--- �V�F�[�_�[�؂�ւ� ----------------------------------------
		d3.eff = d3.lpEffect[D3_SHADER_TYPE_NORMAL];
		if(d3.eff == NULL){ return;}

		//--- �e�N�j�b�N ------------------------------------------------
		if(!SHADER_TECH_SET(d3.eff, "Normal")){ return;}

		//--- ���C�g ----------------------------------------------------
		SHADER_INT_SET(			 "g_iLightSw",		   d3.bLight);

		//--- �v���W�F�N�V���������[���h�s�� ----------------------------
		SHADER_MATRIX_SET("g_mtxViewProjection", &d3.mtxViewProjection);

		//--- �e�N�X�`���[���}�e���A�� ----------------------------------
		SHADER_TEXTURE_SET("texDecale",    d3.tex[TEX_WHITE]);
		SHADER_VEC4_SET(   "g_vecAmbient", &VEC4(0.0f, 0.0f, 0.0f, 0.0f));

		//--- �`�� ------------------------------------------------------
		D3_SPHERE	*s = d3.sphere;
		for(int i = 0; i < d3.usSphere; i++, s++)
		{
			// ���C�g
			SHADER_LIGHT_INVERSE_SET("g_vecLightDirection", &s->mtxWorld, NULL);
			// ���[���h�s��
			SHADER_MATRIX_SET("g_mtxWorld",	  &s->mtxWorld);
			// �F
			SHADER_VEC4_SET(  "g_vecDiffuse", (VEC4*)&s->col);
			// �A���t�@
			SHADER_FLOAT_SET( "g_fAlp",       s->col.a);

			// �`��
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
| <<< �R�c���̕`�� >>>
|	����	pvecPos   = ���W
|			fRadius   = ���a
|			pmtxWorld = ���[���h�s��
|			pCol      = �F
|	���V�F�[�_�[�u00.fx�v���g���܂�
*----------------------------------------------------------------------*/
void	d3_3d_sphere_draw(VEC3 *pvecPos, float fRadius, MTX *pmtxWorld, COL4 *pCol)
{

	MTX		mtxWorld2, mtxTrans, mtxScale;
	D3DXMatrixTranslation(&mtxTrans, *pvecPos);
	D3DXMatrixScaling(    &mtxScale, fRadius);
	if(*pmtxWorld == NULL){ mtxWorld2 = mtxScale * mtxTrans;}
	else				  { mtxWorld2 = mtxScale * mtxTrans * *pmtxWorld;}

	//--- �V�F�[�_�[�؂�ւ� ----------------------------------------
	d3.eff = d3.lpEffect[D3_SHADER_TYPE_NORMAL];
	if(d3.eff == NULL){ return;}

	//--- �e�N�j�b�N ------------------------------------------------
	if(!SHADER_TECH_SET(d3.eff, "Normal")){ return;}

	//--- ���C�g ----------------------------------------------------
	SHADER_LIGHT_INVERSE_SET("g_vecLightDirection", &mtxWorld2, NULL);
	SHADER_INT_SET(			 "g_iLightSw",		   d3.bLight);

	//--- �v���W�F�N�V���������[���h�s�� ----------------------------
	SHADER_MATRIX_SET("g_mtxViewProjection", &d3.mtxViewProjection);
	SHADER_MATRIX_SET("g_mtxWorld",		     &mtxWorld2);
	SHADER_INT_SET(	  "g_iNumBones",	     4);

	//--- �e�N�X�`���[���}�e���A�� ----------------------------------
	//D3_ALPHA_SW_DIRECT(true);
	SHADER_FLOAT_SET(  "g_fAlp",       pCol->a);
	SHADER_TEXTURE_SET("texDecale",    d3.tex[TEX_WHITE]);
	SHADER_VEC4_SET(   "g_vecDiffuse", (VEC4*)pCol);
	SHADER_VEC4_SET(   "g_vecAmbient", &VEC4(0.0f, 0.0f, 0.0f, 0.0f));

	//--- �`�� ------------------------------------------------------
	SHADER_BEGIN();
	SHADER_PASS_BEGIN(0);
	d3.lpMeshSphere->DrawSubset(0);
	SHADER_PASS_END();
	SHADER_END();
}



/*----------------------------------------------------------------------*
| <<< �R�c���̕`�� >>>
|	����	pvecPos   = ���W
|			fRadius   = ���a
|			pCol      = �F
|	���V�F�[�_�[�u00.fx�v���g���܂�
*----------------------------------------------------------------------*/
void	d3_3d_sphere_draw(VEC3 *pvecPos, float fRadius, COL4 *pCol)
{
	d3_3d_sphere_draw(pvecPos, fRadius, NULL, pCol);
}



/*----------------------------------------------------------------------*
| <<< �R�c�O�p�`�̕`�� >>>
|	����	pvecPos0, pvecPos1, pvecPos2 = ���W
|			dwCol						 = �F
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
| <<< �R�c���C���̓o�^ >>>
|	����	pvecP0, pvecP1 = �n�_�ƏI�_
|			dwCol          = �F
|	���o�^�֐��Ȃ̂ŕ`��̍ۂɂ� d3_3d_line_flush ���Ăяo���̂���
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
| <<< d3_3d_line_set �œo�^���ꂽ�R�c���C���̕`�� >>>
|	����	pmtxWorld = �`��s��
|			bSw = true :	���C����K����O�ɕ`��
|	���o�^�֐��Ȃ̂ŕ`��̍ۂɂ� d3_3d_line_flush ���Ăяo���̂���
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
| <<< �R�c���C���̕`�� >>>
|	����	pvecP0, pvecP1 = �n�_�ƏI�_
|			dwCol          = �F
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
| <<< �Q�c���C���̕`�� >>>
|	����	pvecP0, pvecP1 = �n�_�ƏI�_
|			dwCol          = �F
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
	[[[	�X�v���C�g�n ]]]
	�����O�ŃX�v���C�g��`�悷��B
	�@���S���W�݂̂ŕ`��ł���̂��֗��B
**************************************************************************************************/

#define	SPR_PERS	800					// �X�v���C�g�̃p�[�X�l(�l���������Ɖ��s�����͕t���܂����t�u���䂪�݂܂�)

/*------------------------------------------------------------------------------*
| <<< ���O�X�v���C�g�Z�b�g >>>
|	����	iTex      = �e�N�X�`���[�ԍ�
|			pRect     = �摜�̐؂�o�����W
|			pvecPos   = �\�����W
|			pmtxWorld = �s��
|			dwCol     = �J���[
*------------------------------------------------------------------------------*/
void	d3_spr_draw(int iTex, RECT *prect, VEC3 *pvecPos, MTX *pmtxWorld, DWORD dwCol)
{

	//--- RECT �`�F�b�N ---------------------------------------------
	if(prect == NULL)
	{
		RECT	a = {0, 0, (uint)d3.texInfo[iTex].fOriWidth, (uint)d3.texInfo[iTex].fOriHeight}; 
		prect	  = &a;
	}
	//--- �����A�c���𓾂� ------------------------------------------
	float	w0  =  (prect->right  - prect->left) / 2.f;
	float	h0  = -(prect->bottom - prect->top ) / 2.f;
	float	w1  =  (prect->right  - prect->left) / 2.f;
	float	h1  = -(prect->bottom - prect->top ) / 2.f;

	//--- UV �����߂� -----------------------------------------------
	float	u0, v0, u1, v1;
	if(prect != NULL)
	{							// �� 0.5f �𑫂������������� UV �̂Ȃ��ڂ��������Ă܂�
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

	//--- �|���S���̎l���_�𓾂� ------------------------------------
	//	0 *---* 1			0 = ����A1 = �E��
	//	  |   |				2 = �E���A3 = ����
	//  3 *---* 2
	float	x[4], y[4];
	x[0] = -w0, y[0] =  h0;
	x[1] =  w0, y[1] =  h0;
	x[2] =  w0, y[2] = -h0;
	x[3] = -w0, y[3] = -h0;

	//--- �l���_�ɍs���K�p���� ------------------------------------
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

	//--- �Q�c���R�c�ϊ� --------------------------------------------
	// ���Ȃ�K���Ɍv�Z
	for(i = 0; i < 4; i++)
	{
		// �p�[�X�̌v�Z
		avecPos[i].z  = (avecPos[i].z + SPR_PERS) / (SPR_PERS * 2);
		avecPos[i].x *= avecPos[i].z * 2.0f;
		avecPos[i].y *= avecPos[i].z * 2.0f;

		// ���W�w��̂��̂�
		if(pvecPos != NULL){ avecPos[i] += *pvecPos;}
		else			   { avecPos[i] += VEC3(d3.sRenderW, d3.sRenderH, 0) / 2.f;}

	}
	// Z ���W���`�F�b�N(0.0f�Ŏ�O�A1.0f �ň�ԉ��ɂȂ�)
	float	fZ = pvecPos->z;
	if(     fZ < 0.0f){ fZ = 0.0f;}
	else if(fZ > 1.0f){ fZ = 1.0f;}

	//--- �`��o�^ --------------------------------------------------
	// ���_�o�b�t�@
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
	[[[	�����_�����O�n ]]]
**************************************************************************************************/

/*------------------------------------------------------------------------------*
| <<< �����_�����O�T�u >>>
|	����	sub	 = �`��֐�
*------------------------------------------------------------------------------*/
void	d3_render_sub(void *sub)
{
	//--- �f�o�C�X�̃e�X�g ----------------------------------------
	HRESULT	hr = d3.dev->TestCooperativeLevel();
	if(hr == D3DERR_DEVICELOST || hr == D3DERR_DEVICENOTRESET)
	{
		sys_pause();
		sys.cResizeCount  = 0;
		d3.bWindow		  = TRUE;			// �f�o�C�X���X�g���ɂ́A�K���E�B���h�E���[�h�ɂ���
		sys.bScreenChange =	true;
	}
	//--- ���C������ ----------------------------------------------
	else
	{
		void(*func)(void);
		func = (void(*)(void))sub;
		(*func)();
	}
}



/*------------------------------------------------------------------------------*
| <<< �����_�����O���C�� >>>
*------------------------------------------------------------------------------*/
void	d3_render(void)
{
	#ifdef	__GAME_RENDER
		if(sys.bFrameSkip){ return;}
		d3_render_sub(game_render);					// �ʏ�`��
	#endif
}



/**************************************************************************************************
	[[[	�����n ]]]
**************************************************************************************************/


/*------------------------------------------------------------------------------*
| <<< �t�H���g�̍쐬 >>>
|	����	iNum        = �Ǘ��ԍ�
|			iW, iH      = �����̃T�C�Y
|			str	        = �t�H���g��
|			bItalic     = true : �C�^���b�N
|			fWeightMult = �����̑����̔{��
*------------------------------------------------------------------------------*/
void	d3_font_create(int iNum, int iW, int iH, char *str, bool bItalic, float fWeightMult)
{

	if(d3.font[iNum] != NULL){ SAFE_RELEASE(d3.font[iNum]);}

	d3.fontInfo[iNum].Width		      = iW;
	d3.fontInfo[iNum].Height		  = iH;
	d3.fontInfo[iNum].MipLevels	      = 0;
	d3.fontInfo[iNum].Weight          = (uint)((float)FW_NORMAL * fWeightMult);	// ���̑����݂���
	d3.fontInfo[iNum].Italic		  = bItalic;
	d3.fontInfo[iNum].CharSet		  = SHIFTJIS_CHARSET;
	d3.fontInfo[iNum].OutputPrecision = OUT_DEFAULT_PRECIS;
	d3.fontInfo[iNum].Quality		  = DEFAULT_QUALITY;
	d3.fontInfo[iNum].PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;
	if(str != NULL) strcpy(d3.fontInfo[iNum].FaceName, str);
	else			strcpy(d3.fontInfo[iNum].FaceName, "�l�r �S�V�b�N");
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
| <<< �t�H���g�̊J�� >>>
*------------------------------------------------------------------------------*/
void	d3_font_free(void)
{
	//--- �t�H���g�̊J�� ------------------------------------------
	for(int	i =	0; i < D3_FONT_MAX; i++){ SAFE_RELEASE(d3.font[i]);}
}


/*------------------------------------------------------------------------------*
| <<< �����`�� >>>
|	����	iFontNum = �쐬���镶���h�c
|			fX,	fY	 = �����T�C�Y
|			dwCol    = �F
|			Format   = ������
*------------------------------------------------------------------------------*/
void	d3_font_draw(int iFontNum, float fX, float fY, DWORD dwCol, char* Format,...)
{

	//--- �����쐬 --------------------------------------------------
    char	strBuf[512];
	va_list	ap;
	va_start(ap,	 Format);
	vsprintf(strBuf, Format, ap);
	va_end(ap);

	//--- �`�� ------------------------------------------------------
	RECT	r      = {(uint)(fX * d3.fZoom + d3.vecOfs.x), (uint)(fY * d3.fZoom + d3.vecOfs.y), 0, 0};
	uint	uiFlag = DT_NOCLIP;
	d3.font[iFontNum]->DrawText(NULL, strBuf, -1, &r, uiFlag, dwCol);
}


/*------------------------------------------------------------------------------*
| <<< �����`�� >>>
|	����	iFontNum = �쐬���镶���h�c
|			fX,	fY	 = �����T�C�Y
|			iLen     = �\��������
|			dwCol    = �F
|			Format   = ������
*------------------------------------------------------------------------------*/
void	d3_font_draw(int iFontNum, float fX, float fY, int iLen, DWORD dwCol, char* Format,...)
{

	if(d3.font[iFontNum] == NULL){ return;}

	//--- �����쐬 --------------------------------------------------
    char	strBuf[512];
	va_list	ap;
	va_start(ap,	 Format);
	vsprintf(strBuf, Format, ap);
	va_end(ap);

	//--- �`�� ------------------------------------------------------
	//RECT	r      = {(uint)fX, (uint)fY, 0, 0};
	RECT	r      = {(uint)(fX * d3.fZoom + d3.vecOfs.x), (uint)(fY * d3.fZoom + d3.vecOfs.y), 0, 0};
	uint	uiFlag = DT_NOCLIP;
	d3.font[iFontNum]->DrawText(NULL, strBuf, iLen, &r, uiFlag, dwCol);
}


/*------------------------------------------------------------------------------*
| <<< �����`�� >>>
|	����	iFontNum = �쐬���镶���h�c
|			r		 = �����̕\���̈�
|			bCenter  = true : �����ɕ`��
|			dwCol    = �F
|			Format   = ������
*------------------------------------------------------------------------------*/
void	d3_font_draw(int iFontNum, RECT *prect, bool bCenter, DWORD dwCol, char* Format,...)
{

	if(d3.font[iFontNum] == NULL){ return;}

	//--- �����쐬 --------------------------------------------------
    char	strBuf[512];
	va_list	ap;
	va_start(ap,	 Format);
	vsprintf(strBuf, Format, ap);
	va_end(ap);

	//--- �`�� ------------------------------------------------------
	uint	uiFlag = DT_NOCLIP + DT_CENTER * bCenter;
	RECT	r = *prect;
	d3.font[iFontNum]->DrawText(NULL, strBuf, -1, &r, uiFlag, dwCol);
}


/*------------------------------------------------------------------------------*
| <<< �����o�^ >>>					���g������ printf �Ɠ���
|	����	iNum   = �o�^�ԍ�
|			iX, iY = �`����W
|			dwCol  = �F
|			Format = �ȍ~�Aprintf �Ɠ���
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
| <<< �����o�^ >>>					���g������ printf �Ɠ���
|	����	iNum   = �o�^�ԍ�
|			iX, iY = �`����W
|			dwCol  = �F
|			Format = �ȍ~�Aprintf �Ɠ���
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
| <<< �f�o�b�N�p�����o�^ >>>		���g������ printf �Ɠ���
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
| <<< �f�o�b�N�p�����o�^ >>>		���g������ printf �Ɠ���
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
| <<< �f�o�b�N�p�����ɉe�ݒ� >>>
|	����	iNum    = �o�^�ԍ�
|			bShadow = true or false
*------------------------------------------------------------------------------*/
void	d3_print_shadow_set(int iNum, bool bShadow)
{
	d3.textWork[iNum].bShadow = bShadow;
}



/*------------------------------------------------------------------------------*
| <<< d3_printf �œo�^�����������S�ĕ`�悷�� >>>
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
| <<< �����\���I�t >>>
|	����	iNum = �o�^�ԍ�
*------------------------------------------------------------------------------*/
void	d3_print_off(int iNum)
{

	if(iNum >= D3_TEXT_MAX) return;

	d3.textWork[iNum].str[0] = NULL;
}



/*------------------------------------------------------------------------------*
| <<< �S�����\���I�t >>>
*------------------------------------------------------------------------------*/
void	d3_print_off_all(void)
{

	for(int	i =	0; i < D3_TEXT_MAX; i++) d3.textWork[i].str[0] = NULL;
}


/*------------------------------------------------------------------------------*
| <<< �R�c���C���̏����� >>>
*------------------------------------------------------------------------------*/
void	d3_3d_line_init(void)
{
	d3.iLineCount =	0;
}



/*------------------------------------------------------------------------------*
| <<< �R�c���C���̕`�� >>>
*------------------------------------------------------------------------------*/
void	d3_3d_line_draw(void)
{
	d3_light_set(false);
	d3_tex_set(-1);
	d3_stat_fvf_set(D3_FVF_L);
	d3.dev->DrawPrimitiveUP(D3DPT_LINESTRIP, d3.iLineCount - 1,	d3.vtxLine,	sizeof(D3_VTX_L));
}



/**************************************************************************************************
	[[[	�a�l�o�n ]]]
**************************************************************************************************/


/*----------------------------------------------------------------------*
| <<< �J���[�l�� R8G8B8	�t�H�[�}�b�g�ɕϊ����� >>>
|	����	col	   = �F
|			format = �t�H�[�}�b�g
|	�߂�l	X8R8G8 �̃f�[�^
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
| <<< �a�l�o�t�@�C���̃w�b�_�t�@�C����ǂݏo�� >>>
|	����	*f0	  =	�t�@�C���|�C���^
|			*s	  =	�G���[���b�Z�[�W�i�[�p������̃A�h���X
|	�o��	*s	  =	�G���[���b�Z�[�W�i�[�p������
|	�߂�l	false =	���o�����s
|			true  =	���o������
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
	if(w->head.usType != 19778)				// �a�l�o�t�@�C���ł͂Ȃ�
	{
		strcat(s, "�̓r�b�g�}�b�v�t�@�C���ł͂���܂���B");
		return false;
	}
	if(w->info.uiComp != 0)					// ���k����Ă���
	{
		strcat(s, "�͈��k����Ă܂��B���k�t�@�C���̓T�|�[�g����Ă��܂���B");
		return false;
	}

	int		_w = w->info.uiXsize;			// �o�b�t�@�̒��ł̉���
	int		_h = w->info.uiYsize;			// �o�b�t�@�̒��ł̉���

	switch(w->info.usBitCount)
	{
	case 4:										// �P�U�F
		w->info.uiSizeImage = (((_w + 3) / 4) * 4) * _h / 2;
		w->info.uiClrUsed	= 16;
		break;
	case 8:										// �Q�T�U�F
		w->info.uiSizeImage = _w * _h;
		w->info.uiClrUsed	= 256;
		break;
	case 16:									// �n�C�J���[
//		w->info.uiSizeImage = _w * _h	* 2;
		break;
	case 24:									// �t���J���[
		_w = (((w->info.uiXsize * 3) + 3) / 4) * 4;
		w->info.uiSizeImage = _w * _h;
		break;
	case 32:									// �t���J���[�R�Q�r�b�g
		w->info.uiSizeImage = _w * _h * 4;
		break;
	}
	return true;
}



/*-----------------------------------------------------------------------*
|	<<<	�a�l�o�f�[�^�ǂݏo�� >>>
|	����	*w	  =	�a�l�o�̏��t�@�C��
|			*file =	�ǂݏo���t�@�C��
|	�o��	*buf  =	�ϊ���̃f�[�^���i�[�����
*-----------------------------------------------------------------------*/
bool	d3_bmp_read(BMP_WORK *w, char *file)
{

	uint	j;
	DWORD	rs;

	HANDLE	handle = CreateFile(file, GENERIC_READ,	0, NULL, OPEN_EXISTING,	FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS, NULL);
	if(handle == INVALID_HANDLE_VALUE) return sys_error("%s ������܂���B", file);
	strcpy(w->strName, file);
	if((j =	GetFileSize(handle,	NULL)) == 0){ CloseHandle(handle); return sys_error("%s �̃T�C�Y���O�o�C�g�ł��B", file);}
	if(!d3_bmp_head_read(handle, w,	file)){	  CloseHandle(handle); return sys_error("%s �ɃG���[����",			   file);}
	if((w->ucBuf = (uchar *)malloc(w->info.uiSizeImage)) == NULL)
	{
		CloseHandle(handle); return	sys_error("%s �ǂݍ��ݎ��Ƀ������[�s���B", file);
	}
	w->usBuf16 = (ushort *)w->ucBuf;
	w->ucBuf24 = (uchar  *)w->ucBuf;
	w->ucBuf32 = (uchar  *)w->ucBuf;
	if(w->info.usBitCount <= 8)				// �Q�T�U�F�ȉ��Ȃ�p���b�g�����[�h
	{
		ReadFile(handle, w->col, sizeof(BMP_COL) * w->info.uiClrUsed, &rs, NULL);
	}
	ReadFile(handle, w->ucBuf, w->info.uiSizeImage,	&rs, NULL);
	CloseHandle(handle);
	return true;
}



/*-----------------------------------------------------------------------*
|	<<<	�a�l�o�̉摜�f�[�^���R�Q�r�b�g�J���[�֕ϊ� >>>
|	����	*w		= �a�l�o�̏��t�@�C��
|	�o��	*w->buf	= �ϊ���̃f�[�^���i�[�����
*-----------------------------------------------------------------------*/
void	d3_bmp_change_32color(BMP_WORK	*w)
{

	int		_w = w->info.uiXsize;			// �o�b�t�@�̒��ł̉���
	int		_h = w->info.uiYsize;			// �o�b�t�@�̒��ł̉���

	if(w->info.usBitCount != 32)
	{
		uchar	*buf = (uchar *)malloc(w->info.uiXsize * w->info.uiYsize * 4);
		if(buf == NULL){ sys_error("d3_bmp_change_32color ���Ń������[���m�ەs�\�B");}
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
|	<<<	�a�l�o�̉摜�f�[�^���c�ɋt�����ɂ��� >>>	���f�[�^�Ƃ��Ďg���₷���Ȃ�l��
|	����	*w		= �a�l�o�̏��t�@�C��
|	�o��	*w->buf	= �ϊ���̃f�[�^���i�[�����
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
	if(len == 0){	 sys_error("%s �͕s���Ȃa�l�o�t�@�C���ł��B",                w->strName); return;}
	buf	= (uchar *)malloc(w->info.uiXsize * w->info.uiYsize	* len);
	if(buf == NULL){ sys_error("%s : d3_bmp_x_reverse ���Ń������[���m�ەs�\�B", w->strName); return;}
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
|	<<<	�a�l�o�̉摜�f�[�^���c�ɋt�����ɂ��� >>>	���f�[�^�Ƃ��Ďg���₷���Ȃ�l��
|	����	*w		= �a�l�o�̏��t�@�C��
|	�o��	*w->buf	= �ϊ���̃f�[�^���i�[�����
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
	if(len == 0){	 sys_error("%s �͕s���Ȃa�l�o�t�@�C���ł��B",				 w->strName); return;}
	buf	= (uchar *)malloc(w->info.uiXsize * w->info.uiYsize	* len);
	if(buf == NULL){ sys_error("%s : d3_bmp_y_reverse ���Ń������[���m�ەs�\�B", w->strName); return;}
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
| <<< �a�l�o�̃������[����� >>>
|	����	*w	 = �a�l�o�̏��t�@�C��
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
| <<< �a�l�o�w�b�_�����ǂݏo�� >>>
|	����	*s	 = �ǂݏo���t�@�C��
*-----------------------------------------------------------------------*/
bool	d3_bmp_info_read(BMP_INFO *w, char	*s)
{

	DWORD	rs;


	HANDLE	handle = CreateFile(s, GENERIC_READ, 0,	NULL, OPEN_EXISTING,
									FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS, NULL);
	if(handle == INVALID_HANDLE_VALUE) return false;
	ReadFile(handle, w,	14,	&rs, NULL);			// �ŏ��̂P�S�o�C�g�͓ǂݔ�΂�
	ReadFile(handle, w,	40,	&rs, NULL);
	CloseHandle(handle);
	return TRUE;
}



/**************************************************************************************************
	[[[	�e�N�X�`���[�n ]]]
**************************************************************************************************/


/*----------------------------------------------------------------------*
| <<< �g�p�\�ȉ摜�t�H�[�}�b�g�𓾂� >>>
|	����	sw	 = 0 : ���ʂ̃e�N�X�`���[
|				   1 : �����_�����O��
|	�߂�l	�g�p�\�ȉ摜�t�H�[�}�b�g
*----------------------------------------------------------------------*/
D3DFORMAT	d3_tex_format_search(int sw)
{
	int		Usage;

	if(sw == 0){ Usage = 0;}
	else	   { Usage = D3DUSAGE_RENDERTARGET;}

	D3DFORMAT	tbl[] =
	{//	���ׂ���̫�ϯ�	   �o�b�N�o�b�t�@�ϊ��p
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
	// CheckDeviceFormat(0,	D3DDEVTYPE_HAL,	D3DFMT_A8R8G8B8, ,,	D3DFMT_A8R8G8B8) �Ƃ���� D3_OK �����Ȃ��B
	// CheckDeviceFormat(0,	D3DDEVTYPE_HAL,	D3DFMT_X8R8G8B8, ,,	D3DFMT_A8R8G8B8) �Ƃ��Ē��ׂ�K�v������

	//--- �摜�t�H�[�}�b�g�𒲂ׂ� -----------------------------
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
| <<< �e�N�X�`���[�ǂݍ��݈ꊇ�Ǘ� >>>	�������̃t�@�C���͓ǂݍ��܂��Ƀ|�C���^��Ԃ�
|	����	iNum		= �ǂݍ��ݔԍ�
|			strFile		= �ǂݍ��݃t�@�C����(���\�[�X�O�Ԃ���̓ǂݍ��݂��\)
|			bErrorCheck	= false	: �G���[���b�Z�[�W��\�����Ȃ�
|	�o��	infoOut		= �ǂݍ��ݏ��
|	�߂�l	true  :	�ǂݍ���
|			false :	�ǂݍ���ł��Ȃ�
*----------------------------------------------------------------------*/
static	bool	TextureLoad(int iNum, char *strFile, D3DXIMAGE_INFO *infoOut, bool bErrorCheck)
{
	if(sys.hResource[0] !=	NULL)			// dll �t�@�C�����ǂݍ���
	{
		D3DXCreateTextureFromResourceEx(
			d3.dev,								// �f�o�C�X
			sys.hResource[0], strFile,			// �ǂݍ��݃t�@�C����
			D3DX_DEFAULT, D3DX_DEFAULT,			// �摜�̃T�C�Y
			1,									// �~�b�v�}�b�v���x��
			0,									// �e�N�X�`���[�̎g�p���@
			d3.fmtTexture,						// �e�N�X�`���[�̃t�H�[�}�b�g
			D3DPOOL_MANAGED,					// �e�N�X�`���[�̊Ǘ����@
			D3DX_FILTER_NONE,					// �t�B���^�[
			D3DX_FILTER_NONE,					// �~�b�v�}�b�v�̃t�B���^�[
			__COLORKEY,							// �����F(�����ɂ���F)
			infoOut,							// �e�N�X�`���[�̏�������|�C���^
			NULL,								// �p���b�g�𓾂�
			&d3.tex[iNum]);						// �e�N�X�`���[�̃|�C���^
	}
	if(d3.tex[iNum] == NULL)			// ���\�[�X���ǂݍ���
	{
		char	name[_MAX_PATH + 1];
		sys_div_path(NULL, name, NULL, strFile);
		D3DXCreateTextureFromResourceEx(
			d3.dev,								// �f�o�C�X
			NULL, name,							// �ǂݍ��݃t�@�C����
			D3DX_DEFAULT, D3DX_DEFAULT,			// �摜�̃T�C�Y
			1,									// �~�b�v�}�b�v���x��
			0,									// �e�N�X�`���[�̎g�p���@
			d3.fmtTexture,						// �e�N�X�`���[�̃t�H�[�}�b�g
			D3DPOOL_MANAGED,					// �e�N�X�`���[�̊Ǘ����@
			D3DX_FILTER_NONE,					// �t�B���^�[
			D3DX_FILTER_NONE,					// �~�b�v�}�b�v�̃t�B���^�[
			__COLORKEY,							// �����F(�����ɂ���F)
			infoOut,							// �e�N�X�`���[�̏�������|�C���^
			NULL,								// �p���b�g�𓾂�
			&d3.tex[iNum]);						// �e�N�X�`���[�̃|�C���^
	}
	if(d3.tex[iNum] == NULL)			// �t�@�C�����ǂݍ���
	{
		D3DXCreateTextureFromFileEx(
			d3.dev,	strFile,					// �f�o�C�X�A�ǂݍ��݃t�@�C����
			D3DX_DEFAULT, D3DX_DEFAULT,			// �摜�̃T�C�Y
			1,									// �~�b�v�}�b�v���x��
			0,									// �e�N�X�`���[�̎g�p���@
			d3.fmtTexture,						// �e�N�X�`���[�̃t�H�[�}�b�g
			D3DPOOL_MANAGED,					// �e�N�X�`���[�̊Ǘ����@
			D3DX_FILTER_NONE,					// �t�B���^�[
			D3DX_FILTER_NONE,					// �~�b�v�}�b�v�̃t�B���^�[
			__COLORKEY,							// �����ɂ���F
			infoOut,							// �e�N�X�`���[�̏�������|�C���^
			NULL,								// �p���b�g�𓾂�
			&d3.tex[iNum]);						// �e�N�X�`���[�̃|�C���^
	}

	if(bErrorCheck && d3.tex[iNum] == NULL){ return sys_error("TextureLoad: %d�� (%s) ���ǂݍ��߂܂���ł����B", iNum, strFile);}

	return d3.tex[iNum] != NULL;
}



/*----------------------------------------------------------------------*
| <<< �e�N�X�`���[�ǂ݂��� >>>
|	����	iNum		= �ǂݍ��ݔԍ�			��-1�̎��͋󂫔ԍ���T���ēǂݍ��ށB���b�V�����Ɏg�p
|			strFile		= �ǂݍ��݃t�@�C����( NULL �̏ꍇ�� d3.texInfo ���ǂݍ���)
|			bLoadAtOnce = true : �����ǂݍ��݁Afalse : D3_TEXTURE �����s�����Ƃ��ɓǂݍ���(�ǂݍ��ݎ����ɂ͕\������Ȃ��̂Œ���)
|			bErrMessage = true : �G���[���b�Z�[�W�\��
|	�߂�l	-1  == �ǂݍ���łȂ�
|			 0  != �ǂݍ��ݔԍ�
*----------------------------------------------------------------------*/
int		d3_tex_load(int iNum, char *strFile, bool bLoadAtOnce, bool bErrMessage)
{

	char	strPath[_MAX_PATH + 1];

	if(strFile != NULL)
	{
		//--- �g���q���Ȃ����̂̓��\�[�X�Ƃ݂Ȃ� ------------------------
		//bool	resource = true;
		d3.texInfo[iNum].bResource = true;
		int		i				   = 0;
		while(strFile[i] != NULL)
		{
			if(ISKANJI(strFile[i])){    i += 2;}
			else if(strFile[i] == '.'){ d3.texInfo[iNum].bResource = false; break;}
			else{						i++;}
		}

		//--- �t�@�C�������쐬 ------------------------------------------
		//	�@���\�[�X�̏ꍇ�ɂ͂����R�s�[
		if(!d3.texInfo[iNum].bResource)
		{
			char	strName[_MAX_FNAME + 1];
			char	strExt[ _MAX_EXT   + 1];
			sys_div_path(strPath, strName, strExt, strFile);
			sprintf(strPath, "%s%s%s", strPath, strName, strExt);
		}
		else{ strcpy(strPath, strFile);}

		//--- �����Ȃ�΁A�ǂݍ��܂Ȃ��ŏI�� ----------------------------
		if(iNum != -1)
		{
			if(d3.tex[iNum] != NULL)
			{
				if(strcmp(d3.texInfo[iNum].strFullPath, strPath) == 0) return iNum;
			}
		}
		//--- ���łɓǂݍ���ł���t�@�C�����𒲂ׂ� --------------------
		//	  ���łɓǂݍ���ł���ꍇ�ɂ͂��̔ԍ���Ԃ��ă��^�[���B
		//	�@�ǂݍ���ł��Ȃ��ꍇ�ɂ́A�V�K�ɓǂݍ��ޔԍ��� iID �ɃZ�b�g����
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
			if(i == D3_TEX_MAX2){ return -1;}			// �������łɓǂݍ��ݏꏊ���Ȃ�
		}
	}
	else
	{
		strcpy(strPath, d3.texInfo[iNum].strFullPath);	// �t�@�C�����ۑ�
	}

	if(strlen(strPath) == 0){ return -1;}

	//--- �����A�ǂݍ��݂Ŗ����ꍇ ----------------------------------
	if(!bLoadAtOnce)
	{
		strcpy(d3.texInfo[iNum].strFullPath, strPath);	// �t�@�C�����ۑ�
		d3.texInfo[iNum].bLoad = false;
		return -1;
	}

	//--- �ǂݍ��� --------------------------------------------------
	D3DXIMAGE_INFO	info;									// �ǂݍ��߂Ȃ������ꍇ�� -1 ��Ԃ�
	d3_tex_free(iNum);
	if(!TextureLoad(iNum, strPath, &info, bErrMessage)){ return -1;}

	//--- �T�C�Y���Q�̕␔�ɂ��� ------------------------------------
	d3.texInfo[iNum].fOriWidth  = (float)info.Width;		// �I���W�i���̃e�N�X�`���[�T�C�Y
	d3.texInfo[iNum].fOriHeight = (float)info.Height;
															// �e�N�X�`���[���Q�̗ݏ�Ŗ�����΂����Ȃ������ׂ�
	//if(!(d3.caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL))	// �ۗ��A���̏��������ł͂��߂݂���
	{																	// �e�N�X�`���[�� D3DUSAGE_DYNAMIC �œǂݍ��ޕK�v����
		uint	i;
		for(i =	1; i <=	d3.caps.MaxTextureWidth; i *= 2)	// �e�N�X�`���[�̃T�C�Y��␳
		{
			if(info.Width <= i){ info.Width	= i; break;}
		}
		for(i =	1; i <=	d3.caps.MaxTextureHeight; i	*= 2)	// �e�N�X�`���[�̃T�C�Y��␳
		{
			if(info.Height <= i){ info.Height =	i; break;}
		}
	}
	d3.texInfo[iNum].fWidth	    = (float)info.Width;		// �e�N�X�`���[�T�C�Y
	d3.texInfo[iNum].fHeight	= (float)info.Height;
	d3.texInfo[iNum].usFormat   = info.Format;				// �e�N�X�`���[�t�H�[�}�b�g
	d3.texInfo[iNum].bOffscreen = false;
	d3.texInfo[iNum].fU		    = d3.texInfo[iNum].fOriWidth  / d3.texInfo[iNum].fWidth;
	d3.texInfo[iNum].fV		    = d3.texInfo[iNum].fOriHeight / d3.texInfo[iNum].fHeight;
	d3.texInfo[iNum].bLoad		= true;						// �ǂݍ��݃t���O�I��

	strcpy(d3.texInfo[iNum].strFullPath, strPath);			// �t�@�C�����ۑ�
	d3.bReRender = true;									// �ă����_�����O�v��

	//--- �^�C���X�^���v�o�^ ----------------------------------------
	sys_timestamp_get(&d3.texInfo[iNum].ftLastWrite, d3.texInfo[iNum].strFullPath);

	return iNum;											// �ǂݍ��߂��̂œǂݍ��ݔԍ���Ԃ�
}



/*----------------------------------------------------------------------*
| <<< �e�N�X�`���[�ǂ݂��� >>>
|	����	iNum		= �ǂݍ��ݔԍ�
|			file		= �ǂݍ��݃t�@�C����
|	�߂�l	-1  == �ǂݍ���łȂ�
|			 0  != �ǂݍ��ݔԍ�
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
| <<< �{�����[���e�N�X�`���[�ǂ݂��� >>>
|	����	iVol    = �ǂݍ��ݔԍ�
|			strFile = �ǂݍ��݃t�@�C����
|	�߂�l	-1  == �ǂݍ���łȂ�
|			 0  != �ǂݍ��ݔԍ�
*----------------------------------------------------------------------*/
int		d3_vol_load(int iVol, char *strFile)
{


	SAFE_RELEASE(d3.vol[iVol]);

	char	path[_MAX_PATH + 1];

	if(strFile != NULL)
	{
		//--- �t�@�C�������쐬 ------------------------------------------
		char	name[_MAX_FNAME	+ 1];
		char	ext[ _MAX_EXT	+ 1];
		sys_div_path(path, name, ext, strFile);
		sprintf(path, "%s%s%s",	path, name,	ext);

		D3DXCreateVolumeTextureFromFile(d3.dev, path, &d3.vol[iVol]);

		if(d3.vol[iVol] == NULL) return sys_error("d3_vol_load: iVol�� (%s) ���ǂݍ��߂܂���ł����B", strFile);

		//strcpy(d3.texInfo[iNum].strFullPath, path);				// �t�@�C�����ۑ�

		//--- �^�C���X�^���v�o�^ ----------------------------------------
		//sys_timestamp_get(&d3.texInfo[iNum].ftLastWrite, d3.texInfo[iNum].strFullPath);

		return iVol;
	}
	return -1;
}



/*----------------------------------------------------------------------*
| <<< �e�N�X�`���[�S��� >>>
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
| <<< �e�N�X�`���[�S��� >>>
|	����	iNum = �J���ԍ�			�� -1 �Ȃ�S�J��
*----------------------------------------------------------------------*/
void	d3_tex_free(int iNum)
{
	if(iNum == -1)						// �S�J��
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
| <<< �e�N�X�`���[�̈�쐬 >>>			���@1024�ȏ�͋֕�
|	����	iNum = �e�N�X�`���[�h�c
|			w, h = �̈�̃T�C�Y
|			fm	 = �쐬�������摜�t�H�[�}�b�g
*----------------------------------------------------------------------*/
bool	d3_tex_create(int iNum, int w, int h, D3DFORMAT fmt)
{

	uint	i;

	SAFE_RELEASE(d3.tex[iNum]);
	d3.texInfo[iNum].strFullPath[0]	= NULL;

	if(w > (int)d3.caps.MaxTextureWidth	){ w = d3.caps.MaxTextureWidth;}
	if(h > (int)d3.caps.MaxTextureHeight){ h = d3.caps.MaxTextureHeight;}
	for(i =	1; i <=	d3.caps.MaxTextureWidth; i *= 2)	// �e�N�X�`���[�̃T�C�Y��␳
	{
		if(w <=	(int)i)
		{
			w =	i;
			break;
		}
	}
	for(i =	1; i <=	d3.caps.MaxTextureHeight; i	*= 2)	// �e�N�X�`���[�̃T�C�Y��␳
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
	{										// ��]�ʂ�̃e�N�X�`���[���쐬���Ă݂�
		if(FAILED(D3DXCreateTexture(d3.dev,	w,	h, 1, 0, fmt, D3DPOOL_MANAGED, &d3.tex[iNum])))
		{									// ���s����
			return false;
		}
		else								// ��������
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
											// �e�N�X�`���[���[�h��	R5G6B5 �Ƃ���
											// ���s���邱�Ƃ͂܂������͂�
	if(FAILED(D3DXCreateTexture(d3.dev,	w, h, 1, 0,	D3DFMT_R5G6B5, D3DPOOL_MANAGED,	&d3.tex[iNum])))
	{
		return false;						// ����ł����s�����B
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
| <<< �I�t�X�N���[���̎w����W�𓾂� >>>
|	����	iNum = ��荞��œ]������e�N�X�`���[�ԍ�
*------------------------------------------------------------------------------*/
static	LPDIRECT3DSURFACE9	suf = NULL;
static	LPDIRECT3DSURFACE9	pSurface;
DWORD	*d3_offscreen_capture_begin(int iNum)
{

	D3DSURFACE_DESC		desc;
	D3DLOCKED_RECT		rect;

	if(d3.tex[iNum] == NULL){ return NULL;}

	d3.tex[iNum]->GetLevelDesc(0, &desc);	// �����_�[�e�N�X�`���̃t�H�[�}�b�g���擾
											// �e�N�X�`�����T�[�t�F�C�X��
	d3.tex[iNum]->GetSurfaceLevel(0, &pSurface);
											// �����_�[�e�N�X�`���Ɠ����t�H�[�}�b�g�̃T�[�t�F�C�X���V�X�e���������Ɋm��
	HRESULT	b = d3.dev->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format, D3DPOOL_SYSTEMMEM, &suf, NULL);
											// �����_�[�^�Q�b�g�̃T�[�t�F�C�X���V�X�e���������ɓ]��
	d3.dev->GetRenderTargetData(pSurface, suf);

	suf->GetDesc(&desc);

	suf->LockRect(&rect, NULL, 0);			// �T�[�t�F�C�X���b�N

	#if 0
		// �g�����̗�
		DWORD	*buf = (DWORD*)rect.pBits;				//���b�N������`�̐擪�̃|�C���^
		for(uint y = 0; y < desc.Height; y++)
		{
			for(uint x = 0; x < desc.Width; x++)
			{
				if(buf[x]==0xff000000) buf[x] = 0xffffffff;	//���𔒂�
				else if(buf[x]==0xffffffff) buf[x] = 0xff000000;	//��������
			}
			buf += rect.Pitch / sizeof(DWORD);	//���s
		}
	#endif

	return	(DWORD*)rect.pBits;
}


/*------------------------------------------------------------------------------*
| <<< ��ʂ��I�t�X�N���[���̎w����W�𓾂� >>>
*------------------------------------------------------------------------------*/
void	d3_offscreen_capture_end(void)
{

	suf->UnlockRect();
											// ����Ń����_�����O�e�N�X�`���ɂɏ����߂�
	d3.dev->UpdateSurface(suf, NULL, pSurface, NULL);
	SAFE_RELEASE(pSurface);
	SAFE_RELEASE(suf);						// �I���������
}



/*------------------------------------------------------------------------------*
| <<< ��ʂ��L���v�`���[���� >>>

|	����	iNum = ��荞��œ]������e�N�X�`���[�ԍ�
*------------------------------------------------------------------------------*/
void	d3_screen_capture(int iNum)
{
	// �T�[�t�F�C�X���e�N�X�`���[�̓]�������܂��s���Ȃ��̂ŁA
	// ��������t�@�C���ɕۑ����đΉ�

	d3_offscreen_free(iNum);


	LPDIRECT3DSURFACE9	pScrSurface;	//�T�[�t�F�C�X
	HRESULT				hr;

	hr = d3.dev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pScrSurface);

	// BMP�ɃZ�[�u
	FOLDER_SET();
	hr = D3DXSaveSurfaceToFile("screen.bmp", D3DXIFF_BMP, pScrSurface, NULL, NULL);
	
	d3_tex_free(iNum);
	d3_tex_load(iNum, "screen.bmp");
//	remove("screen.bmp");

	//d3_offscreen_create(iNum, SCR_W, SCR_H);
	//d3_offscreen_begin(iNum);
	//D3_CLEAR(D3_COL(255, 0,255, 0));

	//LPDIRECT3DSURFACE9	pDstSurface;	//�T�[�t�F�C�X
											// �e�N�X�`�����T�[�t�F�C�X��
	//d3.tex[iNum]->GetSurfaceLevel(0, &pDstSurface);

	//RECT	r;
	//SetRect(&r,  0, 0, SCR_W, SCR_H);
	//d3.dev->StretchRect(pSurface, NULL, pDstSurface, NULL, D3DTEXF_NONE);
	//d3_offscreen_end(  iNum);

	// �o�b�N�o�b�t�@��\��
	//d3.dev->Present(NULL, NULL, NULL, NULL);
	
	SAFE_RELEASE(pScrSurface);
	//SAFE_RELEASE(pDstSurface);


// ���Ԃ�A�ǂ������Ȃ�
	/*
	//--- �T�[�t�F�C�X����o�b�t�@�փR�s�[ -----------------------
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
	[[[	�I�t�X�N���[���n ]]]
**************************************************************************************************/


/*----------------------------------------------------------------------*
| <<< �w��I�t�X�N���[�����J�� >>>
|	����	iNum   = �e�N�X�`���[�ԍ�
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
| <<< �I�t�X�N���[�����J�� >>>
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
| <<< �I�t�X�N���[�����t�@�C���֕ۑ� >>>	�� BMP �`��
|	����	iNum    = �t�@�C���ԍ�
|			strFile = �t�@�C����(�g����t���Ă�������)
*----------------------------------------------------------------------*/
void	d3_offscreen_save(int iNum, char *strFile)
{

	if(d3.texInfo[iNum].bRepair &&	d3.texInfo[iNum].bOffscreen)
	{
		D3DXSaveTextureToFile(strFile, D3DXIFF_BMP, d3.tex[iNum], NULL);
	}
}



/*----------------------------------------------------------------------*
| <<< �I�t�X�N���[���̃T�[�t�F�C�X�ɓǂݍ��� >>>	�� BMP �`��
|	����	iNum    = �t�@�C���ԍ�
|			strFile = �t�@�C����(�g����t���Ă�������)
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
					d3.dev,							// �f�o�C�X
					str,							// �ǂݍ��݃t�@�C����
					D3DX_DEFAULT, D3DX_DEFAULT,		// �摜�̃T�C�Y
					1,								// �~�b�v�}�b�v���x��
					0,								// �e�N�X�`���[�̎g�p���@
													// �e�N�X�`���[�̃t�H�[�}�b�g
					(enum _D3DFORMAT)d3.texInfo[iNum].usFormat,
					D3DPOOL_MANAGED,				// �e�N�X�`���[�̊Ǘ����@
					D3DX_FILTER_NONE,				// �t�B���^�[
					D3DX_FILTER_NONE,				// �~�b�v�}�b�v�̃t�B���^�[
					D3DCOLOR_ARGB(255, 0, 0, 0),	// �����ɂ���F
					NULL,							// �e�N�X�`���[�̏�������|�C���^
					NULL,							// �p���b�g�𓾂�
					&tex);							// �e�N�X�`���[�̃|�C���^

		if(tex != NULL)
		{
			// �e�N�X�`���[�֓]��
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
| <<< �I�t�X�N���[����ۑ��p����������� >>>
|	����	iNum = �e�N�X�`���[�ԍ�
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
| <<< �I�t�X�N���[�����������ɕۑ����� >>>
|	�����������m�ۂ��ă����_�����O�����摜��ۑ�����Bd3_offscreen_repair �ōē]��
|	����	iNum = �e�N�X�`���[�ԍ�
*----------------------------------------------------------------------*/
void	d3_offscreen_mem_save(int iNum)
{

	d3.bOffScreenFlagBak[iNum] = d3.texInfo[iNum].bOffscreen;
	if(d3.texInfo[iNum].bRepair &&	d3.texInfo[iNum].bOffscreen)
	{
		// ���������
		d3_offscreen_mem_free(iNum);

		// �������擾
		int	iSize = (int)d3.texInfo[iNum].fWidth * (int)d3.texInfo[iNum].fHeight * sizeof(DWORD);
		d3.pdwOffscreenRepairBuf[iNum] = (DWORD *)malloc(iSize);

		// �T�[�t�F�C�X�]��
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
| <<< �I�t�X�N���[�����C�� >>>
|	��d3_offscreen_save�őޔ����������_�����O�����摜���I�t�X�N���[���֓]������
*----------------------------------------------------------------------*/
static	void	d3_offscreen_repair(void)
{

	//--- �I�t�X�N���[�����쐬 --------------------------------------
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

	//--- �Z�[�u�����I�t�X�N���[���摜��ǂݍ��݁A�\��t�� ----------
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
| <<< �I�t�X�N���[���T�[�t�F�C�X�̍쐬 >>>
|	����	iNum   = �e�N�X�`���[�ԍ�
|			iW,	iH = �T�C�Y				���Q�`�Q�O�S�W�܂�
|			fmt	   = �摜�t�H�[�}�b�g
|	�߂�l	false =	�쐬���s
*----------------------------------------------------------------------*/
static	bool	d3_offscreen_create_(int iNum, int iW, int iH, D3DFORMAT fmt)
{

	//--- ��������ł͍쐬���Ȃ� ----------------------------------
	if(d3.texInfo[iNum].fWidth	   == (float)iW
	&& d3.texInfo[iNum].fHeight	   == (float)iH
	&& d3.texInfo[iNum].bOffscreen == true){ return true;}

	HRESULT	hr;
	int		i;

	if(iW >	(int)d3.caps.MaxTextureWidth ){ iW = d3.caps.MaxTextureWidth;}
	if(iH >	(int)d3.caps.MaxTextureHeight){ iH = d3.caps.MaxTextureHeight;}

	//--- �I���W�i���̃e�N�X�`���[�T�C�Y ----------------------------
	int iWBak = iW;
	int iHBak = iH;

	//--- �e�N�X�`���[�̃T�C�Y��␳ --------------------------------
	for(i =	2; i <=	(int)d3.caps.MaxTextureWidth;  i *=	2){	if(iW <= i){iW = i;	break;}}
	for(i =	2; i <=	(int)d3.caps.MaxTextureHeight; i *=	2){	if(iH <= i){iH = i;	break;}}
	d3.texInfo[iNum].strFullPath[0] = NULL;
	SAFE_RELEASE(d3.tex[iNum]);					// �\�ߓǂݍ��܂�Ă��e�N�X�`���[�͊J������
	SAFE_RELEASE(d3.lpOffscreenZBuffer[iNum]);	// �X�e���V�����J������

	//D3DFORMAT	fmt	= d3_tex_format_search(1);
	if(fmt == D3DFMT_UNKNOWN){ return false;}	// �摜�t�H�[�}�b�g���Ή��ō쐬�ł����B
	hr = d3.dev->CreateTexture(iW, iH, 1, D3DUSAGE_RENDERTARGET, fmt, D3DPOOL_DEFAULT, &d3.tex[iNum], NULL);
	if(FAILED(hr)){ return sys_error("�I�t�X�N���[�� %d �Ԃ̍쐬�Ɏ��s���܂����B", iNum);}
												// �X�e���V���̓o�b�N�o�b�t�@�Ɠ������̂��g��
	hr = d3.dev->CreateDepthStencilSurface(iW, iH, d3.fmtStencil, D3DMULTISAMPLE_NONE, 0, true,	&d3.lpOffscreenZBuffer[iNum], NULL);	
	if(FAILED(hr)){ return sys_error("�I�t�X�N���[�� %d �Ԃ̃X�e���V���o�b�t�@�̍쐬�Ɏ��s���܂����B", iNum);}
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
| <<< �I�t�X�N���[���T�[�t�F�C�X�̍쐬 >>>
|	����	iNum   = �e�N�X�`���[�ԍ�
|			iW,	iH = �T�C�Y				���Q�`�Q�O�S�W�܂�
|	�߂�l	false =	�쐬���s
*----------------------------------------------------------------------*/
bool	d3_offscreen_create(int iNum, int iW, int iH, D3DFORMAT	fmt)
{
	return d3_offscreen_create_(iNum, iW, iH, fmt);
}



/*----------------------------------------------------------------------*
| <<< �I�t�X�N���[���T�[�t�F�C�X�̍쐬 >>>
|	����	iNum   = �e�N�X�`���[�ԍ�
|			iW,	iH = �T�C�Y				���Q�`�Q�O�S�W�܂�
|	�߂�l	false =	�쐬���s
*----------------------------------------------------------------------*/
bool	d3_offscreen_create(int iNum, int iW, int iH)
{
	return d3_offscreen_create_(iNum, iW, iH, d3_tex_format_search(1));
}



/*----------------------------------------------------------------------*
| <<< �I�t�X�N���[���T�[�t�F�C�X�J�n >>>
|	����	iNum = �e�N�X�`���[�ԍ�
*----------------------------------------------------------------------*/
#ifdef	__2D_RESIZE
	static	float	fZoomBak;
	static	VEC2	vecBak;
#endif

bool	d3_offscreen_begin(int iNum)
{

	if(d3.tex[iNum] == NULL){ return sys_error("�I�t�X�N���[���T�[�t�F�C�X %d �Ԃ��쐬����Ă��܂���B", iNum);}

	if(d3.iOffscreenBackUpCount	> D3_OFFSCREEN_BAK_MAX - 1){ return sys_error("�I�t�X�N���[���T�[�t�F�C�X�̓�������I�[�o�[���Ă��܂��B");}

	#ifdef	__2D_RESIZE
		if(d3.iOffscreenBackUpCount == 0)
		{
			fZoomBak  = d3.fZoom;
			vecBak	  = d3.vecOfs;
			d3.fZoom  = 1;
			d3.vecOfs = VEC2(0, 0);
		}
	#endif

	// ����q�ɂȂ��Ă��������炷
	d3.iOffscreenBackUpCount++;
	for(int	i =	0; i < d3.iOffscreenBackUpCount; i++)
	{
		d3.lpOffscreenOldSurface[i + 1]	= d3.lpOffscreenOldSurface[i];
		d3.lpOffscreenOldZBuffer[i + 1]	= d3.lpOffscreenOldZBuffer[i];
		d3.lpOffscreenNewTarget[ i + 1]	= d3.lpOffscreenNewTarget[ i];
	}
											// ���݂̃X�N���[���ݒ��ޔ�
	d3.dev->GetRenderTarget(0, &d3.lpOffscreenOldSurface[	 0]);
	d3.dev->GetDepthStencilSurface(&d3.lpOffscreenOldZBuffer[0]);
											// �V�����X�N���[���ݒ���Z�b�g
	d3.tex[iNum]->GetSurfaceLevel(0, &d3.lpOffscreenNewTarget[0]);

	HRESULT	hr;
	hr = d3.dev->SetRenderTarget(0,	d3.lpOffscreenNewTarget[0]);
	hr = d3.dev->SetDepthStencilSurface(d3.lpOffscreenZBuffer[iNum]);
	//HRESULT	hr = d3.dev->SetRenderTarget(d3.lpOffscreenNewTarget[0], d3.lpOffscreenZBuffer[no]);
	if(FAILED(hr)){ return sys_error("�I�t�X�N���[���T�[�t�F�C�X %d �Ԃ��J�n�ł��܂���B", iNum);}

	d3.dev->BeginScene();					// �V�[���̊J�n
	return true;
}



/*----------------------------------------------------------------------*
| <<< �I�t�X�N���[���T�[�t�F�C�X�I�� >>>
|	����	iNum = �e�N�X�`���[�ԍ�
*----------------------------------------------------------------------*/
void	d3_offscreen_end(int iNum)
{

	if(d3.tex[iNum] == NULL){ return;}

	d3.dev->EndScene();
	HRESULT	hr;
	hr = d3.dev->SetRenderTarget(    0,	d3.lpOffscreenOldSurface[0]);
	hr = d3.dev->SetDepthStencilSurface(d3.lpOffscreenOldZBuffer[0]);
	//HRESULT	hr = d3.dev->SetRenderTarget(d3.lpOffscreenOldSurface[0], d3.lpOffscreenOldZBuffer[0]);
	if(hr != S_OK){ sys_error("d3_offscreen_end �ŃG���[ �e�N�X�`���[�ԍ��� %d", iNum);}
											// ����q�ɂȂ��Ă��������炷
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
	[[[	�������A�I���n ]]]
**************************************************************************************************/


/*----------------------------------------------------------------------*
| <<< �r���[�|�[�g������ >>>			���N�����A�E�B���h�E�T�C�Y�ύX���ɌĂڂ��I
*----------------------------------------------------------------------*/
void	d3_view_port_set(void)
{

	d3.viewport.X	   = 0;
	d3.viewport.Y	   = 0;
	d3.viewport.Width  = d3.para.BackBufferWidth;	// �t���X�N���[�������l������ d3.sRender �͎g��Ȃ�
	d3.viewport.Height = d3.para.BackBufferHeight;
	d3.viewport.MinZ   = 0.0f;
	d3.viewport.MaxZ   = 1.0f;
	d3.dev->SetViewport(&d3.viewport);
}



/*------------------------------------------------------------------------------*
| <<< �g�p�\�ȃf�v�X�^�X�e���V���o�b�t�@�̃t�H�[�}�b�g������ >>>
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
| <<< �����_�����O���̃f�o�C�X�̔\�͂��o�� >>>y
|	���́A�o��	*str = �f�o�C�X�̔\�͂��L����������
*------------------------------------------------------------------------------*/
void	d3_caps_get(char *str)
{

	char	d3_type_str[2][17]	= 				// �f�o�C�X�^�C�v	HAL	or REF
	{
		"D3DDEVTYPE_HAL",
		"D3DDEVTYPE_REF"
	};
	char	d3_processing_str[3][38] =			// �o�[�e�b�N�X�^�C�v
	{
		"D3DCREATE_HARDWARE_VERTEXPROCESSING",
		"D3DCREATE_MIXED_VERTEXPROCESSING",
		"D3DCREATE_SOFTWARE_VERTEXPROCESSING"
	};
	char	yes_no[2][9] =
	{
		"�g���Ȃ�",
		"�g����"
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
		"���_�V�F�[�_�[ver = %2d.%02d\n"
		"�ő�v���~�e�B�u  = %d\n" 
		"�ő僉�C�g        = %d\n"
		"�A���`�G�C���A�X  = %s\n"
		//"�t�H�O:\n"
		//"   ���_     = %s\n"
		//"   �s�N�Z�� = %s\n"
		"\n"
		"�e�N�X�`���[:\n"
		"   ���p�\������   = %d KB\n"
		"   �ő�T�C�Y       = %d x %d\n"
		"   �ő�X�e�[�W     = %d\n"
		"   �A���t�@�u�����h = %s\n"
		"   �~�b�v�}�b�v     = %s\n"
		"   �L���[�u�}�b�v   = %s\n",
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
| <<< �f�B�X�v���C�A�_�v�^�̃o�����[�^�[�쐬 >>>&
|	����	bWindow	= true �Ȃ�΃E�B���h�E�������s��
*------------------------------------------------------------------------------*/
static	void	DisplaySet(bool	bWindow)
{

	ZeroMemory(&d3.para, sizeof(d3.para));
	d3.para.MultiSampleType	   = D3DMULTISAMPLE_NONE;
	d3.para.MultiSampleQuality = 0;

	// ������͉摜�L���v�`���[�Ɏg�p(d3.rectRender�͎g�p�ł��Ȃ��Ȃ�܂�)
	// �悤�� FILP���g��Ȃ��ƃo�b�t�@�ɊG�����܂�Ȃ��Ƃ�������
	// D3DSWAPEFFECT_FLIP		��ʂ̃L���v�`���[��
	// D3DSWAPEFFECT_COPY		d3.rectRender ���g�p��(�c�[���Ȃ�)
	
	d3.para.SwapEffect         = D3DSWAPEFFECT_COPY;//(enum _D3DSWAPEFFECT)d3.dwSwapEffectMode;
	d3.para.BackBufferCount	   = 1;

	d3.para.hDeviceWindow	   = sys.hWnd;
	d3.para.Flags			   = 0;
	if(bWindow)							// �E�B���h�E���[�h
	{
		d3.para.BackBufferWidth			   = d3.sRenderW;
		d3.para.BackBufferHeight		   = d3.sRenderH;
		d3.para.Windowed				   = true;
		d3.para.BackBufferFormat		   = d3.fmtBackBuffer;
		d3.para.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
											// �E�B���h�E���[�h���̉�ʍX�V�ݒ�
											// ���̐ݒ�� vsync ��҂��Ȃ�
											// ���������A�E�B���h�E���[�h���� vsync ��҂ĂȂ�
#ifdef	__NON_VSYNC_WAIT
		d3.para.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
#else
		d3.para.PresentationInterval       = D3DPRESENT_INTERVAL_DEFAULT;
#endif
	}
	else
	{									// �t���X�N���[��
		//--- �ȉ��Ńr�f�I�{�[�h���T�|�[�g���Ă����ʃ��[�h���t���X�N���[�����̉𑜓x�����߂�
		D3DDISPLAYMODE	mode;
		int				iMax = d3.d->GetAdapterModeCount(d3.ucAdapter, d3.fmtDisplay);
		for(int	i =	0; i < iMax; i++)
		{
			if(d3.d->EnumAdapterModes(d3.ucAdapter,	d3.fmtDisplay, i, &mode) == D3D_OK)
			{
				if(mode.RefreshRate	!= D3_HZ    ){ continue;}	// ���t���b�V�����[�g��	60MHz�Œ�
				if(mode.Height		<  D3_REZO_H){ continue;}	// �𑜓x���c 480 �ȉ��͖���

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
											// ���̐ݒ�̎��� vsync �҂��ɂȂ�(�t���X�N���[�����̂�)
											// ���̐ݒ�łȂ��Ɖ�ʂ̏�������������
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

	//--- �t���X�N���[�����̃��T�C�Y�l���v�Z ------------------------
	// ����ŁANDS�AiPhone �d�l�̎��ł���ʂ̒����Ɏ��܂�
	#ifdef	__2D_RESIZE
	{
		// �c���̔䗦�v�Z
		float	x  = (float)(__RENDER_X1 - __RENDER_X0);
		float	y  = (float)(__RENDER_Y1 - __RENDER_Y0);
		float	fW = (float)d3.para.BackBufferWidth  / x;
		float	fH = (float)d3.para.BackBufferHeight / y;
		
		// �{���̒Ⴂ���̂��̗p
		if(fW < fH){ d3.fZoom = fW;}
		else	   { d3.fZoom = fH;}

		// ��ʒ����ֈړ�����悤�ɃI�t�Z�b�g�l���v�Z
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
| <<< �A�_�v�^�[�̃p�����[�^�[��ݒ肷�� >>>
*------------------------------------------------------------------------------*/
static	D3DFORMAT	d3_display_format_tbl[]	=	// ��]����f�B�X�v���C�E�t�H�[�}�b�g
{
	D3DFMT_X8R8G8B8, D3DFMT_R5G6B5
};
D3DFORMAT	d3_stencil_tbl[] =					// �X�e���V���o�b�t�@�`�F�b�N�p
{
	//D3DFMT_D15S1, D3DFMT_D24S8,	D3DFMT_D24X4S4,	D3DFMT_D24X8, D3DFMT_D32, D3DFMT_D16
	D3DFMT_D24S8, D3DFMT_D15S1, D3DFMT_D24X4S4,	D3DFMT_D24X8, D3DFMT_D32, D3DFMT_D16
};

void	d3_adapter_init(void)
{
										// �f�B�X�v���C�̏�Ԃ𓾂�
	d3.d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,	&d3.displayMode);
										// �f�X�N�g�b�v�̏�Ԃ𓾂�
	d3.d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,	&d3.deskTopDisplayMode);

	if(d3.deviceType ==	0)				// ����������Ă��Ȃ����ɂ͈ȉ������s
	{
		d3.ucAdapter		  =	D3DADAPTER_DEFAULT;
		d3.deviceType		  =	D3DDEVTYPE_HAL;
		d3.fmtDisplay		  =	d3.deskTopDisplayMode.Format;
		d3.fmtBackBuffer	  =	d3.deskTopDisplayMode.Format;
		d3.ucVertexProcessing =	D3DCREATE_SOFTWARE_VERTEXPROCESSING;

		#if	!D3_SOFTWARE_ONLY
			if((d3.caps.DevCaps	& D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0)		// T & L �T�|�[�g���H
			{
				//if(d3.caps.TextureCaps & D3DPTEXTURECAPS_ALPHAPALETTE
				//|| d3.caps.TextureCaps & D3DPTEXTURECAPS_ALPHA)
				//{									// D3DCREATE_HARDWARE_VERTEXPROCESSING ��ǉ�
				//	d3.ucVertexProcessing =	D3DCREATE_HARDWARE_VERTEXPROCESSING;
				//}
				d3.ucVertexProcessing =	D3DCREATE_HARDWARE_VERTEXPROCESSING;
			}
		#endif
	}
	DisplaySet(d3.bWindow);				// �f�B�X�v���C�̑��̃p�����[�^�[���Z�b�g

	//--- �E�B���h�E��ԂŋN�����\���`�F�b�N���� ------------------
	if(FAILED(d3.d->CheckDeviceType(d3.ucAdapter, d3.deviceType, d3.fmtDisplay,	d3.fmtBackBuffer, true)))
	{										// �t���X�N���[���𒲂ׂ�
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
	//--- �X�e���V���o�b�t�@�𒲍����� ------------------------------
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
| <<< �t���X�N���[���ɍœK�ȉ�ʃ��[�h�𓾂� >>>
|
|	�����̏ꍇ�̍œK�ȉ�ʃ��[�h�Ƃ̓f�B�X�v���C�̎��g����؂�ւ��邱�Ƃ̂Ȃ�
|	  �f�X�N�g�b�v�̃T�C�Y�Ɠ������g���ƃT�C�Y��Ԃ�
|
|	����	iAdapter	   = �A�_�v�^�[�ԍ��i�O���f�t�H���g�j
|			fmt			   = �ݒ肵�����f�B�X�v���C�t�H�[�}�b�g()
|	�o��	*sOutW, *sOutH = �œK�ȉ�ʃT�C�Y
|	�߂�l	�t���X�N���[���̉𑜓x���w���ԍ�
*------------------------------------------------------------------------------*/
static	void	FullScreenSizeGet(short *sOutW, short *sOutH, int iAdapter, D3DFORMAT fmt)
{

	D3DDISPLAYMODE	mode;

	//--- �f�X�N�g�b�v�̃T�C�Y�𓾂�
	HDC		hDC = GetDC(GetActiveWindow());
	short	sW  = GetDeviceCaps(hDC, HORZRES);	// ��
	short	sH  = GetDeviceCaps(hDC, VERTRES);	// �c
    ReleaseDC(GetActiveWindow(), hDC);

	//--- �f�����؂�ւ��邱�Ƃ��ł���悤�Ƀf�X�N�g�b�v�Ɠ����T�C�Y��T��
	int	iMax   = d3.d->GetAdapterModeCount(iAdapter, fmt);
	int	iCount = 0;
	for(int	i =	0; i < iMax; i++)
	{
		if(d3.d->EnumAdapterModes(iAdapter,	fmt, i,	&mode) == D3D_OK)
		{
			if(mode.RefreshRate	!= D3_HZ    ) continue;	// ���t���b�V�����[�g��	60MHz�Œ�
			if(mode.Height		<  D3_REZO_H) continue;	// �𑜓x���c 480 �ȉ��͖���
			if(mode.Width  == sW && mode.Height == sH)
			{
				*sOutW = sW;
				*sOutH = sH;
				return;
			}
		}
	}

	//--- �f�X�N�g�b�v�Ɠ����T�C�Y����������Ȃ��̂Ŗ���ȃ��[�h��Ԃ�
	*sOutW = 640;
	*sOutH = 480;
}



/*------------------------------------------------------------------------------*
| <<< DirectGraphics ������	>>>
|	����	bWindow	= TRUE or FALSE
*------------------------------------------------------------------------------*/
bool	d3_init(bool bWindow)
{

	//--- �f�o�C�X�̏����� ------------------------------------------
	d3.d = Direct3DCreate9(D3D_SDK_VERSION);
	if(d3.d	== NULL){ return sys_error("d3_init : �������Ɏ��s���܂����B�I�����܂��B");}
											// �A�_�v�^�[�̏�Ԃ𓾂�
	d3.d->GetDeviceCaps(D3DADAPTER_DEFAULT,	D3DDEVTYPE_HAL,	&d3.caps);

#ifdef	__VS_VERSION
	if(d3.caps.VertexShaderVersion < __VS_VERSION)
	{
		//if(d3.caps.MaxVertexBlendMatrices < 4){ return sys_error("�{�[�����g���܂���B�I�����܂�");}

		#ifdef	UNDER_20_OPERATION
			sys_error("���̃r�f�I�{�[�h�͒��_�V�F�[�_�[2.0�ɑΉ����Ă��܂���B\r\n�R�c�\���̊ȗ������s���܂��B");
			d3.bLegacy = true;
		#else
			return sys_error("d3_init : �r�f�I�{�[�h�����_�V�F�[�_�[2.0�ɑΉ����Ă��܂���B�I�����܂��B");
		#endif
	}
	d3.ucShaderType = D3_SHADER_TYPE_NORMAL;
#endif
											// ��ʐ؂�ւ����̃��X�|���X��ǂ����邽�߂ɁA�f�X�N�g�b�v�̃T�C�Y���t���X�N���[���T�C�Y�Ƃ���
	FullScreenSizeGet(&d3.sFullScreenRenderW, &d3.sFullScreenRenderH, 0, D3_FORMAT);

	//--- �f�o�C�X�̍쐬 ------------------------------------------
	d3.fZoom = 1;							// �t���X�N���[�����̔{��(d3_init�̒��Ōv�Z���s���̂ŏ������͂�����)
	d3_adapter_init();						// �A�_�v�^�[�̃p�����[�^�[��ݒ肷��
	if(FAILED(d3.d->CreateDevice(d3.ucAdapter, d3.deviceType, sys.hWnd, d3.ucVertexProcessing, &d3.para, &d3.dev)))
	{
		return sys_error("d3_init : Direct3D �f�o�C�X�̍쐬�Ɏ��s���܂����B\r\n�I�����܂��B");
	}
	d3.bPresent		 = true;				// ���C���E�B���h�E�ɉ摜�̓]�����s��
	d3.bPaintPresent = true;				// WM_PAINT ���ɉ摜�̓]�����s��

	//--- �\�ߐF�X�Ȏ��_�̍s����쐬���Ă��� ------------------------
	D3DXMatrixIdentity(&d3.mtxIdentity);	// �P�ʍs��

	//--- �J�����ݒ� ------------------------------------------------
	d3.vecCamera = VEC3(0, 0, -10);			// �f�t�H���g�̃J�������W
	d3.vecEye	 = VEC3(0, 0,	0);			//			   �������W
	d3.vecUp	 = VEC3(0, 1,   0);			// �J�����̌���

	//--- �r���[�{�[�g�쐬 ------------------------------------------
	d3_view_port_set();

	//--- ���C�g�ݒ� ------------------------------------------------
	memset(&d3.light, 0, sizeof(D3DLIGHT9));
	d3.light.Type =	D3DLIGHT_DIRECTIONAL;
	VEC4	*a;
	a =	(VEC4 *)&d3.light.Diffuse;  *a = VEC4(1.0f, 1.0f, 1.0f, 1.0f);	// rgba	�̏���
	a =	(VEC4 *)&d3.light.Specular; *a = VEC4(0.5f, 0.5f, 0.5f, 0.5f);	// rgba
	a =	(VEC4 *)&d3.light.Ambient;  *a = VEC4(0.2f, 0.2f, 0.2f, 0.2f);	// rgba
	d3.light.Direction = VEC3( 1.0f, -1.f,  1.0f);
	d3.light.Position  = VEC3(-1.0f, -1.f, -1.0f);
	d3.light.Range	   = 1000.0f;
	d3.fLightIntensity =    5.0f;
	d3_light_set(true);

	//--- ���� ----------------------------------------------------
	{
		float	a	  = 0.1f;				// ���ꂪ�f�t�H���g�̊���
		int		b	  = (int)(255.f * 0.1f);
		d3.dev->SetRenderState(D3DRS_AMBIENT, D3_COL(b, b, b, b));
		d3.vecAmbient = VEC4(a, a, a, a);
	}
	//--- �}�e���A���ݒ� --------------------------------------------
	D3DMATERIAL9	*m = &d3.matNormal;
	m->Diffuse.r  =	1.0f; m->Diffuse.g	= 1.0f;	m->Diffuse.b  =	1.0f; m->Diffuse.a	= 1.0f;
	m->Ambient.r  =	1.0f; m->Ambient.g	= 1.0f;	m->Ambient.b  =	1.0f; m->Ambient.a	= 1.0f;
	m->Specular.r =	1.0f; m->Specular.g	= 1.0f;	m->Specular.b =	1.0f; m->Specular.a	= 1.0f;
	m->Emissive.r =	1.0f; m->Emissive.g	= 1.0f; m->Emissive.b =	1.0f; m->Emissive.a = 1.0f;
	m->Power	  =	5.0f;

	//--- �V�F�[�_�[�ݒ� --------------------------------------------
#ifdef	__VS_VERSION
	d3_tex_load(TEX_WHITE, "WHITE", true);
	d3_shader_set(D3_SHADER_TYPE_NORMAL);	// �m�[�}���^�C�v�̂݃R���p�C�����Ă���
#endif
	d3_shader_set(D3_SHADER_TYPE_OFF);
											// ���̈�s�������Ɩʂ������Ȃ�܂��B
	d3.dev->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	d3_stat_dither_set(true);

	//--- �e�N�X�`���[�֌W ------------------------------------------
	d3.fmtTexture = d3_tex_format_search(0);// �g�p�\�ȉ摜�t�H�[�}�b�g�𓾂�
	d3_stat_filter_set(D3_FILTER_LINEAR);

	//--- �q�b�g�����쐬 -------------------------------------------
#ifdef	__VS_VERSION
											// ���\�[�X���쐬���Ă݂�
	HRESULT	h = D3DXLoadMeshFromXResource(NULL, "SPHERE", "MESH", D3DXMESH_MANAGED, d3.dev, NULL, NULL, NULL, NULL, &d3.lpMeshSphere);
											// ���߂Ȃ�Ύ��͂�
	if(FAILED(h)){ D3DXCreateSphere(d3.dev, 1.f, 6, 6, &d3.lpMeshSphere, NULL);}
#endif

	//--- �����t�H���g�쐬 ------------------------------------------
	d3_font_create(0, 8, 16, NULL, false);
	d3.textWork[0].bShadow = true;

	//--- ��ʂ��N���A�[ --------------------------------------------
	#ifdef	__WINDOW_COLOR
		DWORD	d     = GetSysColor(COLOR_BTNFACE);
		DWORD	dwCol = D3_COL(255, (d & 255), ((d >> 8) & 255), (d >> 16));
	#else
		DWORD	dwCol = D3_COL(255, 0, 0, 0);
	#endif

	return true;
}



/*------------------------------------------------------------------------------*
| <<< DirectGraphics �̏I������	>>>
*------------------------------------------------------------------------------*/
bool	d3_end(void)
{

	SAFE_RELEASE(d3.lpMeshSphere);			// �q�b�g��
	d3_font_free();							// �t�H���g�J��
	d3_xfile_free_all();					// ���b�V���폜
	d3_shader_free();						// �V�F�[�_�[�J��
	d3_offscreen_mem_free();				// �I�t�X�N���[�����������
	SAFE_RELEASE(d3.dev);
	SAFE_RELEASE(d3.d);
	return true;
}



/*------------------------------------------------------------------------------*
| <<< �E�B���h�E�X�^�C���ݒ� >>>
|	����	bWindow	= true �Ȃ�΃E�B���h�E�������s��
*------------------------------------------------------------------------------*/
static	void	WindowStyleSet(bool	bWindow)
{
//@@http://bluefish.orz.hm/sdoc/winprog_memo.html#%E3%82%A6%E3%82%A3%E3%83%B3%E3%83%89%E3%82%A6%E4%BD%8D%E7%BD%AE%E3%81%AE%E5%BE%A9%E5%85%83
//http://www.geocities.jp/KY_webid/win32c/060.html
	//--- �E�B���h�E�� ----------------------------------------------
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
		sys_window_size_set();		// sys.iWidth, sys.iHeight ��ݒ�
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
	//--- �t���X�N���[���� ------------------------------------------
	else
	{										// �t���X�N���[�����̓V�X�e�����j���[�Ȃ�
		SetWindowLong(sys.hWnd,	GWL_STYLE, WS_POPUP);
		//SetWindowPos( sys.hWnd,	HWND_NOTOPMOST,	0, 0, d3.sFullScreenRenderW, d3.sFullScreenRenderH, SWP_SHOWWINDOW);
		MENU_VIEW(false);
	}
}



/*------------------------------------------------------------------------------*
| <<< �X�N���[���؂�ւ� >>>		�� d3 �̊e�������Ĕ��f����
|	����	bStyle = true :�E�B���h�E�X�^�C����ύX����
*------------------------------------------------------------------------------*/
void	d3_screen_change(bool bStyle)
{


	if(d3.dev == NULL){ return;}
	//d3_offscreen_mem_save();				// �����ŃI�t�X�N���[����ۑ����Ă��A�X���[�v���Ή��ł��Ȃ��̂�
											// �C�ӂŕۑ�������悤�ɕύX(2012.03.31)
	ShowWindow(	  sys.hWnd,	SW_HIDE);

	d3_offscreen_free();
	DisplaySet(d3.bWindow);					// �V�����f�B�X�v���C�p�����[�^�[�ݒ�
	d3.bDeviceChange = true;				// WM_DISPLAYCHANGE ���荞�݋֎~
	int	i;		
	for(i =	D3_SHADER_TYPE_NORMAL; i < D3_SHADER_TYPE_MAX; i++)
	{
		if(d3.lpEffect[i - D3_SHADER_TYPE_NORMAL]){ d3.lpEffect[i - D3_SHADER_TYPE_NORMAL]->OnResetDevice();}
	}
	// �����Avista�ŃG���[���ł�\���L��
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

	d3.dev->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);	// ���̈�s�������Ɩʂ������Ȃ�܂��B
	d3.dev->SetRenderState(D3DRS_AMBIENT,		   D3_AMBIENT);
	d3_stat_dither_set(true);
	d3_offscreen_repair();									// �I�t�X�N���[�����C��

	//--- ��ʂ��N���A�[ --------------------------------------------
	d3_render_begin();										// �����_�[�J�n
	d3_render_end(false);									// �����_�����O�I��

	ShowWindow(	  sys.hWnd,	SW_SHOW);

	d3.bDeviceChange = false;								// WM_DISPLAYCHANGE ���荞�݋���
}



/*------------------------------------------------------------------------------*
| <<< �����[�h�������ݒ肷�� >>>	���t�@�C�������擾���Ă���
*------------------------------------------------------------------------------*/
void	d3_reload_set(void)
{

	int	i;

	//--- �e�X�N�`���[���ēǂݍ��ݐݒ�ɂ��� ------------------------
	for(i =	0; i < D3_TEX_MAX; i++)
	{
		if(!d3.texInfo[i].bResource){ d3.texInfo[i].bLoad = false;}
	}

	//--- ���b�V�����ēǂݍ��ݐݒ�ɂ��� ----------------------------
	for(i =	0; i < D3_MESH_MAX; i++)
	{
		if(!d3.meshInfo[i].bResource){ d3.meshInfo[i].bLoad = false;}
	}

	//--- �V�F�[�_�[��������� --------------------------------------
	for(i = 0; i < D3_SHADER_TYPE_MAX; i++){ d3_shader_free(i);}
	d3_shader_set(D3_SHADER_TYPE_NORMAL);
	d3_shader_set(D3_SHADER_TYPE_OFF);
}



/*------------------------------------------------------------------------------*
| <<< �e�N�X�`���[�ݒ�(�����Œx���ǂݍ��݂̃e�N�X�`���[��ǂݍ���ł��� >>>
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
| <<< �e�N�X�`���[�̋󂫂�T�� >>>
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
| <<< �����_�����O�I�� >>>
*------------------------------------------------------------------------------*/
void	d3_render_end(BOOL bPrint)
{
	//---------------------------------------------------------------

	#ifdef	__MOVIE
		extern	void	movie_to_texture_main(void);
		movie_to_texture_main();		// ���[�r�[�̃��C������
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

		// �]�����}�X�N����
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

		// ���e�}�X�N
		if((x1 - x0) > 0)
		{
			D3_VTX_TL_	vt0[] = {x0, y0, 0, 1, c,  x1, y0, 0, 1, c,  x1, y1, 0, 1, c,  x0, y1, 0, 1, c};
			D3_VTX_TL_	vt1[] = {x2, y2, 0, 1, c,  x3, y2, 0, 1, c,  x3, y3, 0, 1, c,  x2, y3, 0, 1, c};
			d3.dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vt0, sizeof(D3_VTX_TL));
			d3.dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vt1, sizeof(D3_VTX_TL));
			d3.iPolyCount += 2;
		}

		// �㉺�}�X�N
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

	//--- �t���b�V�� ------------------------------------------------
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

