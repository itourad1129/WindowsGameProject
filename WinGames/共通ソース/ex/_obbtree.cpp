//
//	OBB(Oriented Bounding Box)
//


//#include	<float.h>
//#include	<stdio.h>
//#include	<math.h>

//--- 変数の定義 ----------------------------------------------------------------------------------
		int		iOBBTreeLevel		= 0;				// OBBTree の精度(値が大きいと細かくなる)
static	int		iOBBTreeHitBoxCount = 0;

//--- OBBTree クラス ----------------------------------------------------------------------------------

/*------------------------------------------------------------------------------*
| <<< 初期化 >>>
*------------------------------------------------------------------------------*/
COBBTree::COBBTree()
{
	pOBBTopNode = new OBBNODE;
}


/*------------------------------------------------------------------------------*
| <<< 終了処理 >>>
*------------------------------------------------------------------------------*/
COBBTree::~COBBTree()
{
	ReleaseNodes(pOBBTopNode);
}


/*------------------------------------------------------------------------------*
| <<< メモリ開放 >>>
*------------------------------------------------------------------------------*/
void	COBBTree::ReleaseNodes(OBBNODE *pOBBNode)
{
	if(pOBBNode->pOBBNodeL) ReleaseNodes(pOBBNode->pOBBNodeL);
	if(pOBBNode->pOBBNodeR) ReleaseNodes(pOBBNode->pOBBNodeR);
	delete	pOBBNode;
	pOBBNode = NULL;
}


/*------------------------------------------------------------------------------*
| <<< OBBTree の作成、頂点情報を小分けにする >>>
*------------------------------------------------------------------------------*/
bool	COBBTree::Create(LPD3DXMESH pMesh, int iLevel)
{
	// 頂点取得
	int		iVertexNum = pMesh->GetNumVertices();

	std::vector< VEC3 >Vertex;
	LPDIRECT3DVERTEXBUFFER9	pVB;
	pMesh->GetVertexBuffer(&pVB);

	D3DVERTEXBUFFER_DESC	VDesc;
	pVB->GetDesc(&VDesc);
	int		iFVFSize = VDesc.Size / iVertexNum;
	float	*pf;
	pVB->Lock(0, 0, (void **)&pf, D3DLOCK_DISCARD);
	int	i;
	for(i = 0; i < iVertexNum; i++)
	{
		VEC3	vec;
		vec.x = *(pf++);
		vec.y = *(pf++);
		vec.z = *(pf++);
		Vertex.push_back(vec);

		// 頂点フォーマットはfloat かDWORDのデータしかないので4バイト単位で
		// アクセス可能だが( pf += ( FVFSize - 4 * 3 ) / 4 )
		// 念のためバイト単位でアクセスする
		pf = (float *)((char *)pf + (iFVFSize - 4 * 3));
	}
	pVB->Unlock();

	// 3角ポリゴン取得
	int		iFaceNum = pMesh->GetNumFaces();
	FACES	faces;
	LPDIRECT3DINDEXBUFFER9	pIB;
	pMesh->GetIndexBuffer(&pIB);

	D3DINDEXBUFFER_DESC	IDesc;
	pIB->GetDesc(&IDesc);

	WORD	*pw = NULL;
	DWORD	*pd = NULL;
	if(     IDesc.Format == D3DFMT_INDEX16) pIB->Lock(0, 0, (void**)&pw, D3DLOCK_DISCARD);
	else if(IDesc.Format == D3DFMT_INDEX32) pIB->Lock(0, 0, (void**)&pd, D3DLOCK_DISCARD);
	else return false;

	for(i = 0; i < iFaceNum; i++)
	{
		FACE	face;
		if(IDesc.Format == D3DFMT_INDEX16)
		{
			face.Vertex[0] = Vertex[*(pw++)];
			face.Vertex[1] = Vertex[*(pw++)];
			face.Vertex[2] = Vertex[*(pw++)];
		}
		else if(IDesc.Format == D3DFMT_INDEX32)
		{
			face.Vertex[0] = Vertex[*(pd++)];
			face.Vertex[1] = Vertex[*(pd++)];
			face.Vertex[2] = Vertex[*(pd++)];
		}
		face.Center = (face.Vertex[0] + face.Vertex[1] + face.Vertex[2] ) / 3;
		faces.push_back(face);
	}
	pIB->Unlock();
	Create(faces, iLevel, pOBBTopNode);
	return true;
}


/*------------------------------------------------------------------------------*
| <<<  >>>
*------------------------------------------------------------------------------*/
void	COBBTree::Create(FACES &Faces, int Level, OBBNODE *pOBBNode)
{

	CreateOBB(Faces, pOBBNode->OBBData);

	// 最長の軸の中心に対して三角ポリゴンが右にいるか左にいるか切り分ける
	// 最長の軸でポリゴン群をまったく分割できない場合は別の軸で再チャレンジ
	FACES	FacesL, FacesR;
	for(int j = 0; j < 3; j++)
	{
		FacesL.clear();
		FacesR.clear();

		for(int i = 0; i < (int)Faces.size(); i++)
		{
			VEC4	Vec4;
			D3DXVec3Transform(&Vec4, &Faces[i].Center, &pOBBNode->OBBData.Matrix);
			float	v[3] = {Vec4.x, Vec4.y, Vec4.z};

			if(v[j] < pOBBNode->OBBData.Length[j] / 2) FacesL.push_back(Faces[i]);
			else									   FacesR.push_back(Faces[i]);
		}
		if(FacesL.size() > 0 && FacesR.size() > 0) break;
	}

	Level--;
	if(Level < 0) return;

	if(FacesL.size() > 0)
	{
		pOBBNode->pOBBNodeL = new OBBNODE;
		Create(FacesL, Level, pOBBNode->pOBBNodeL);
	}
	if(FacesR.size() > 0)
	{
		pOBBNode->pOBBNodeR = new OBBNODE;
		Create(FacesR, Level, pOBBNode->pOBBNodeR);
	}
}


/*------------------------------------------------------------------------------*
| <<< 与えられたポリゴン群を包含するOBBを作成 >>>
*------------------------------------------------------------------------------*/
void	COBBTree::CreateOBB(std::vector < FACE > &Face, OBBDATA &OBBData)
{

	std::vector < VEC3 > Vertex;

	for(int j = 0; j < (int)Face.size(); j++)
	{
		for(int i = 0; i < 3; i++)
		{
			Vertex.push_back(Face[j].Vertex[i]);
		}
	}

	int	Size = (int)Vertex.size();
	
	// 平均
	VEC3	m(0, 0, 0);
	for(int i = 0; i < Size; i++) m += Vertex[i];
	m /= (float)Size;

	// 共分散行列
	float	C11 = 0, C22 = 0, C33 = 0, C12 = 0, C13 = 0, C23 = 0;
	for(int i = 0; i < Size; i++)
	{
		C11 += (Vertex[i].x - m.x) * (Vertex[i].x - m.x);
		C22 += (Vertex[i].y - m.y) * (Vertex[i].y - m.y);
		C33 += (Vertex[i].z - m.z) * (Vertex[i].z - m.z);
		C12 += (Vertex[i].x - m.x) * (Vertex[i].y - m.y);
		C13 += (Vertex[i].x - m.x) * (Vertex[i].z - m.z);
		C23 += (Vertex[i].y - m.y) * (Vertex[i].z - m.z);
	}
	C11 /= Size;
	C22 /= Size;
	C33 /= Size;
	C12 /= Size;
	C13 /= Size;
	C23 /= Size;

	float	Matrix[3][3] =
	{
		{C11, C12, C13},
		{C12, C22, C23},
		{C13, C23, C33},
	};

	// jacobi法で固有値 & 固有ベクトルを算出
	float	EigenVectors[3][3];
	float	EigenValue[  3];
	Jacobi(Matrix, EigenVectors, EigenValue);

	// 固有値を降順でソート
	struct	SORT
	{
		int		ID;
		float	Value;
	}Sort[3] = {{0, EigenValue[0]}, {1, EigenValue[1]}, {2, EigenValue[2]}};

	for(int j = 0; j < 2; j++)
	{
		for(int i = 2; i > j; i--)
		{
			if(Sort[i - 1].Value < Sort[i].Value)
			{
				SORT	a   = Sort[i    ];
				Sort[i    ] = Sort[i - 1];
				Sort[i - 1] = a;
			}
		}
	}

	for(int i = 0; i < 3; i++)
	{
		OBBData.Axis[i].x = EigenVectors[0][Sort[i].ID];
		OBBData.Axis[i].y = EigenVectors[1][Sort[i].ID];
		OBBData.Axis[i].z = EigenVectors[2][Sort[i].ID];
	}

	// 境界ボックスを算出
	float	min[3] = { FLT_MAX,  FLT_MAX,  FLT_MAX};
	float	max[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
	for(int j = 0; j < 3; j++)
	{
		for(int i = 0; i < Size; i++)
		{
			float	a = D3DXVec3Dot(&Vertex[i], &OBBData.Axis[j]);
			if(min[j] > a) min[j] = a;
			if(max[j] < a) max[j] = a;
		}
	}

	OBBData.Center = OBBData.Axis[0] * ((min[0] + max[0] ) / 2)
				   + OBBData.Axis[1] * ((min[1] + max[1] ) / 2)
				   + OBBData.Axis[2] * ((min[2] + max[2] ) / 2);

	for(int i = 0; i < 3; i++) OBBData.Length[i] = max[i] - min[i];
	VEC3 G = OBBData.Center
		  - (OBBData.Axis[0] * OBBData.Length[0]
		  +  OBBData.Axis[1] * OBBData.Length[1]
		  +  OBBData.Axis[2] * OBBData.Length[2]) / 2;

	OBBData.Matrix = MTX(
		 OBBData.Axis[0].x, OBBData.Axis[1].x, OBBData.Axis[2].x, 0,
		 OBBData.Axis[0].y, OBBData.Axis[1].y, OBBData.Axis[2].y, 0,
		 OBBData.Axis[0].z, OBBData.Axis[1].z, OBBData.Axis[2].z, 0,
		-D3DXVec3Dot(&OBBData.Axis[0], &G), -D3DXVec3Dot(&OBBData.Axis[1], &G), -D3DXVec3Dot(&OBBData.Axis[2], &G), 1);
}


/*------------------------------------------------------------------------------*
| <<< 頂点を内包する BOX を描画 >>>
*------------------------------------------------------------------------------*/
void	COBBTree::DrawOBB(OBBDATA OBBData)
{

	VEC3	G		=  OBBData.Center
					- (OBBData.Axis[0] * OBBData.Length[0]
					+  OBBData.Axis[1] * OBBData.Length[1]
					+  OBBData.Axis[2] * OBBData.Length[2]) / 2;

	VEC3	Axis[3] = {OBBData.Axis[0] * OBBData.Length[0],
					   OBBData.Axis[1] * OBBData.Length[1],
					   OBBData.Axis[2] * OBBData.Length[2]};

	DWORD	dwCol = D3_COL(255, 255, 0, 0);
	VERTEX_XYZ_COLOR	Vertex[8] =
	{
		{G.x,									  G.y,									   G.z,										dwCol},
		{G.x + Axis[1].x,						  G.y + Axis[1].y,						   G.z + Axis[1].z,							dwCol},
		{G.x + Axis[1].x + Axis[2].x,			  G.y + Axis[1].y + Axis[2].y,			   G.z + Axis[1].z + Axis[2].z,				dwCol},
		{G.x + Axis[2].x,						  G.y + Axis[2].y,						   G.z + Axis[2].z,							dwCol},

		{G.x + Axis[0].x,						  G.y + Axis[0].y,						   G.z + Axis[0].z,							dwCol},
		{G.x + Axis[0].x + Axis[1].x,			  G.y + Axis[0].y + Axis[1].y,			   G.z + Axis[0].z + Axis[1].z,				dwCol},
		{G.x + Axis[0].x + Axis[1].x + Axis[2].x, G.y + Axis[0].y + Axis[1].y + Axis[2].y, G.z + Axis[0].z + Axis[1].z + Axis[2].z, dwCol},
		{G.x + Axis[0].x + Axis[2].x,			  G.y + Axis[0].y + Axis[2].y,			   G.z + Axis[0].z + Axis[2].z,				dwCol},
	};
	ushort	Index[24] =
	{
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7
	};

	d3.dev->SetFVF(D3DFVF_XYZ_COLOR);
	d3.dev->SetTexture(0, NULL);
    d3.dev->SetRenderState(D3DRS_LIGHTING, FALSE);
	d3.dev->DrawIndexedPrimitiveUP(D3DPT_LINELIST, 0, 8, 12, Index, D3DFMT_INDEX16, Vertex, sizeof(VERTEX_XYZ_COLOR));
    d3.dev->SetRenderState(D3DRS_LIGHTING, TRUE);
}


/*------------------------------------------------------------------------------*
| <<< 頂点を内包する BOX を描画 >>>
*------------------------------------------------------------------------------*/
void	COBBTree::Draw(OBBNODE *pOBBNode, int iDrawLevel, int iLevel)
{
	if(iLevel == iDrawLevel)
	{
		DrawOBB(pOBBNode->OBBData);
		return;
	}
	iLevel++;

	if(pOBBNode->pOBBNodeL) Draw(pOBBNode->pOBBNodeL, iDrawLevel, iLevel);
	if(pOBBNode->pOBBNodeR) Draw(pOBBNode->pOBBNodeR, iDrawLevel, iLevel);
}


/*------------------------------------------------------------------------------*
| <<< ヒットボックスを得る >>>
*------------------------------------------------------------------------------*/
void	COBBTree::HitBoxGet(OBBNODE *pOBBNode, int iDrawLevel, int iLevel, int iNum, bool bDraw, int *pCount)
{

	if(iLevel == iDrawLevel)
	{
		if(iNum == *pCount || iNum == -1)
		{
			OBBDATA		*d = &pOBBNode->OBBData;
			VEC3	G		=  d->Center
							- (d->Axis[0] * d->Length[0]
							+  d->Axis[1] * d->Length[1]
							+  d->Axis[2] * d->Length[2]) / 2;
			
			VEC3	Axis[3] = {d->Axis[0] * d->Length[0],
							   d->Axis[1] * d->Length[1],
							   d->Axis[2] * d->Length[2]};

			VEC3	v[8];

			v[4] = VEC3(G.x,									 G.y,									  G.z									 );
			v[0] = VEC3(G.x + Axis[1].x,						 G.y + Axis[1].y,						  G.z + Axis[1].z						 );
			v[1] = VEC3(G.x + Axis[1].x + Axis[2].x,			 G.y + Axis[1].y + Axis[2].y,			  G.z + Axis[1].z + Axis[2].z			 );
			v[5] = VEC3(G.x + Axis[2].x,						 G.y + Axis[2].y,						  G.z + Axis[2].z						 );
			v[7] = VEC3(G.x + Axis[0].x,						 G.y + Axis[0].y,						  G.z + Axis[0].z						 );
			v[3] = VEC3(G.x + Axis[0].x + Axis[1].x,			 G.y + Axis[0].y + Axis[1].y,			  G.z + Axis[0].z + Axis[1].z			 );
			v[2] = VEC3(G.x + Axis[0].x + Axis[1].x + Axis[2].x, G.y + Axis[0].y + Axis[1].y + Axis[2].y, G.z + Axis[0].z + Axis[1].z + Axis[2].z);
			v[6] = VEC3(G.x + Axis[0].x + Axis[2].x,			 G.y + Axis[0].y + Axis[2].y,			  G.z + Axis[0].z + Axis[2].z			 );

			MTX		m;
			d3.dev->GetTransform(D3DTS_WORLD, &m);
			int	i;
			for(i = 0; i < 8; i++)
			{
				D3DXVec3TransformCoord(&v[i], &v[i], &m);
			}
			if(bDraw)
			{
				DWORD	c =	D3_COL(255, 255, 255, 0);
				D3_3DLINE(&v[0], &v[1], c);
				D3_3DLINE(&v[1], &v[2], c);
				D3_3DLINE(&v[2], &v[3], c);
				D3_3DLINE(&v[3], &v[0], c);

				D3_3DLINE(&v[4], &v[5], c);
				D3_3DLINE(&v[5], &v[6], c);
				D3_3DLINE(&v[6], &v[7], c);
				D3_3DLINE(&v[7], &v[4], c);

				D3_3DLINE(&v[0], &v[4], c);
				D3_3DLINE(&v[1], &v[5], c);
				D3_3DLINE(&v[2], &v[6], c);
				D3_3DLINE(&v[3], &v[7], c);
			}
			for(i = 0; i < 8; i++)
			{
				vecHit.push_back(v[i]);
			}
		}
		*pCount = *pCount + 1;
		return;
	}
	iLevel++;

	if(pOBBNode->pOBBNodeL) HitBoxGet(pOBBNode->pOBBNodeL, iDrawLevel, iLevel, iNum, bDraw, pCount);
	if(pOBBNode->pOBBNodeR) HitBoxGet(pOBBNode->pOBBNodeR, iDrawLevel, iLevel, iNum, bDraw, pCount);
}


/*------------------------------------------------------------------------------*
| <<< BOX と BOX の判定 >>>
*------------------------------------------------------------------------------*/
bool	COBBTree::IsCollision(OBBDATA &BoxA, OBBDATA &BoxB)
{

	// BoxAの3軸がxyz軸になる空間に変換
	VEC4	Axis[3];
	MTX		Matrix = BoxA.Matrix;
	Matrix._41 = 0; Matrix._42 = 0; Matrix._43 = 0;

	for(int i = 0; i < 3; i++)
	{
		D3DXVec3Transform(&Axis[i], &BoxB.Axis[i], &Matrix);
	}

	VEC4	VecAtoBTrans;
	D3DXVec3Transform(&VecAtoBTrans, &(BoxB.Center - BoxA.Center), &Matrix);	// 平行移動成分はキャンセルされなければならないのでBoxA.MatrixではなくMatrixを使う

	// 無駄な計算を省くために下ごしらえ
	float	B[3][3] =
	{
		{Axis[0].x, Axis[0].y, Axis[0].z},
		{Axis[1].x, Axis[1].y, Axis[1].z},
		{Axis[2].x, Axis[2].y, Axis[2].z}
	};

	float	absB[3][3] =
	{
		{fabs(B[0][0]), fabs(B[0][1]), fabs(B[0][2])},
		{fabs(B[1][0]), fabs(B[1][1]), fabs(B[1][2])},
		{fabs(B[2][0]), fabs(B[2][1]), fabs(B[2][2])}
	};

	float	T[3] = {VecAtoBTrans.x, VecAtoBTrans.y, VecAtoBTrans.z}; 

	float	a[3] = {BoxA.Length[0] / 2, BoxA.Length[1] / 2, BoxA.Length[2] / 2};
	float	b[3] = {BoxB.Length[0] / 2, BoxB.Length[1] / 2, BoxB.Length[2] / 2};

	// 以下、判定ルーチン

	// 分離軸 A.Axis[?]
	for(int i = 0; i < 3; i++)
	{
		if(fabs(T[i]) > a[i] + b[0] * absB[0][i] + b[1] * absB[1][i] + b[2] * absB[2][i]) return false;
	}

	// 分離軸 B.Axis[?]
	for(int i = 0; i < 3; i++)
	{
		if(fabs(T[0] * B[i][0] + T[1] * B[i][1] + T[2] * B[i][2]) > b[i] + a[0] * absB[i][0] + a[1] * absB[i][1] + a[2] * absB[i][2]) return false;
	}

	// 分離軸 A.Axis[?] x B.Axis[?]
	for(int j0 = 0; j0 < 3; j0++)
	{
		int	j1 = (j0 + 1) % 3;
		int	j2 = (j0 + 2) % 3;
		for(int i0 = 0; i0 < 3; i0++)
		{
			int	i1 = (i0 + 1) % 3;
			int	i2 = (i0 + 2) % 3;
			if(fabs(-T[j1] * B[i0][j2] + T[j2] * B[i0][j1]) > a[j1] * absB[i0][j2] + a[j2] * absB[i0][j1] + b[i1] * absB[i2][j0] + b[i2] * absB[i1][j0]) return false;
		}
	}
	return true;
}


/*------------------------------------------------------------------------------*
| <<< 当たり判定 >>>
*------------------------------------------------------------------------------*/
bool	COBBTree::CheckCollision(OBBNODE *pNodeA, OBBNODE *pNodeB, MTX &TransMat, MTX &RotMat)
{
	// BoxAから見た座標系にBoxBを変換
	OBBDATA	OBBDataB = pNodeB->OBBData;

	VEC4	Center;
	D3DXVec3Transform( &Center, &OBBDataB.Center, &TransMat);
	OBBDataB.Center.x = Center.x;
	OBBDataB.Center.y = Center.y;
	OBBDataB.Center.z = Center.z;

	for(int i = 0; i < 3; i++)
	{
		VEC4 Vec;
		D3DXVec3Transform( &Vec, &OBBDataB.Axis[i], &RotMat);
		OBBDataB.Axis[i].x = Vec.x;
		OBBDataB.Axis[i].y = Vec.y;
		OBBDataB.Axis[i].z = Vec.z;
	}

	OBBNODE	*pAL = pNodeA->pOBBNodeL,
			*pAR = pNodeA->pOBBNodeR,
			*pBL = pNodeB->pOBBNodeL,
			*pBR = pNodeB->pOBBNodeR;
	bool	Result = false;
	if(IsCollision(pNodeA->OBBData, OBBDataB))
	{
		if(!pAL && !pAR && !pBL && !pBR)
		{
			Result = true;
		}
		else
		{
			if( pAL &&  pBL) if(Result = CheckCollision( pAL, pBL, TransMat, RotMat)) goto EXIT;
			if( pAL &&  pBR) if(Result = CheckCollision( pAL, pBR, TransMat, RotMat)) goto EXIT;
			if( pAR &&  pBL) if(Result = CheckCollision( pAR, pBL, TransMat, RotMat)) goto EXIT;
			if( pAR &&  pBR) if(Result = CheckCollision( pAR, pBR, TransMat, RotMat)) goto EXIT;

			if(!pAL &&  pBL) if(Result = CheckCollision( pNodeA, pBL, TransMat, RotMat)) goto EXIT;
			if(!pAL &&  pBR) if(Result = CheckCollision( pNodeA, pBR, TransMat, RotMat)) goto EXIT;
			// 上二つと等価なので省略
		//	if( !pAR &&  pBL ) if( Result = CheckCollision( pNodeA, pBL, TransMat, RotMat ) ) goto EXIT;
		//	if( !pAR &&  pBR ) if( Result = CheckCollision( pNodeA, pBR, TransMat, RotMat ) ) goto EXIT;

			if( pAL && !pBL) if(Result = CheckCollision( pAL, pNodeB, TransMat, RotMat)) goto EXIT;
			if( pAR && !pBL) if(Result = CheckCollision( pAR, pNodeB, TransMat, RotMat)) goto EXIT;
			// 上二つと等価なので省略
		//	if(  pAL && !pBR ) if( Result = CheckCollision( pAL, pNodeB, TransMat, RotMat ) ) goto EXIT;
		//	if(  pAR && !pBR ) if( Result = CheckCollision( pAR, pNodeB, TransMat, RotMat ) ) goto EXIT;
		}
	}
EXIT:
	return Result;
}


/*------------------------------------------------------------------------------*
| <<< A を大きいもの、B を小さい物としないと当たらない… >>>
*------------------------------------------------------------------------------*/
bool	COBBTree::CheckCollision(COBBTree &OBBTreeA, MTX &TransMatA,
								 COBBTree &OBBTreeB, MTX &TransMatB)
{
	MTX		TransMat, InvMat;
	D3DXMatrixMultiply(&TransMat, &TransMatB, D3DXMatrixInverse(&InvMat, NULL, &TransMatA));

	MTX		RotMat = TransMat;
	RotMat._41 = 0; RotMat._42 = 0; RotMat._43 = 0;
	return CheckCollision(OBBTreeA.pOBBTopNode, OBBTreeB.pOBBTopNode, TransMat, RotMat);
}


/*------------------------------------------------------------------------------*
| <<<  >>>
*------------------------------------------------------------------------------*/
void	COBBTree::Rotate(float a[3][3], float s, float tau, int i, int j, int k, int l)
{
	float	h, g;
	g		= a[i][j];
	h		= a[k][l];
	a[i][j] = g - s * (h + g * tau);
	a[k][l] = h + s * (g - h * tau);
}


/*------------------------------------------------------------------------------*
| <<<  >>>
*------------------------------------------------------------------------------*/
bool	COBBTree::Jacobi(float a[3][3], float v[3][3], float d[3])
{
	int		n = 3;
	int		i, j, iq, ip;
	float	tresh, theta, tau, t, sm, s, h, g, c, b[3], z[3];

	for(ip = 0; ip < n; ip++)
	{
		for(iq = 0; iq < n; iq++) v[ip][iq] = 0.0f;
		v[ip][ip] = 1.0f;
	}
	for(ip = 0; ip < n; ip++)
	{
		b[ip] = d[ip] = a[ip][ip];
		z[ip] = 0.0f;
	}
	for(i = 0; i < 50; i++)
	{
		sm = 0.0f;
		for(ip = 0; ip < n - 1; ip++)
		{
			for(iq = ip + 1; iq < n; iq++) sm += (float)fabs(a[ip][iq]);
		}

		if(sm == 0.0f) return true;
		if(i  <  3   ) tresh = 0.2f * sm / (n * n);
		else		   tresh = 0.0f;

		for(ip = 0; ip < n - 1; ip++)
		{
			for(iq = ip + 1; iq < n; iq++)
			{
				g = 100.0f * (float)fabs(a[ip][iq]);
				if(i > 3 && (fabs(d[ip]) + g) == fabs(d[ip]) && (fabs(d[iq]) + g) == fabs(d[iq]))
				{
					a[ip][iq] = 0.0f;
				}
				else if(fabs(a[ip][iq]) > tresh)
				{
					h = d[iq] - d[ip];
					if((fabs(h) + g) == fabs(h))
					{
						t = a[ip][iq] / h;
					}
					else
					{
						theta = 0.5f * h / a[ip][iq];
						t = 1.0f / ((float)fabs(theta) + (float)sqrt(1.0f + theta * theta));
						if(theta < 0.0f) t = -t;
					}
					c         = 1.0f / (float)sqrt(1 + t * t);
					s         = t   * c;
					tau       = s   / (1.0f + c);
					h         = t   * a[ip][iq];
					z[ip]    -= h;
					z[iq]    += h;
					d[ip]    -= h;
					d[iq]    += h;
					a[ip][iq] = 0.0f;

					for(j = 0;      j < ip; j++) Rotate(a, s, tau, j, ip, j,  iq);
					for(j = ip + 1; j < iq; j++) Rotate(a, s, tau, ip, j, j,  iq);
					for(j = iq + 1; j < n;  j++) Rotate(a, s, tau, ip, j, iq, j );
					for(j = 0;      j < n;  j++) Rotate(v, s, tau, j, ip, j,  iq);
				}
			}
		}
		for(ip = 0; ip < n; ip++)
		{
			b[ip] += z[ip];
			d[ip]  = b[ip];
			z[ip]  = 0.0f;
		}
	}
	return false;
}

