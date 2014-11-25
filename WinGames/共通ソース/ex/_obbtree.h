
#ifdef	__OBBTREE

#ifndef __COBBTREE_H__
#define __COBBTREE_H__

#include <d3dx9.h>
#pragma	warning(disable:4786)
#include <vector>

#define for if( 0 );else for

struct FACE
{
	VEC3	Vertex[3];
	VEC3	Center;
};

typedef std::vector < FACE > FACES;

struct	OBBDATA
{
	D3DXMATRIX	Matrix;
	VEC3		Axis[3];
	float		Length[3];
	VEC3		Center;
};

struct	OBBNODE
{
	OBBNODE	*pOBBNodeL, *pOBBNodeR;
	OBBDATA	OBBData;
	OBBNODE(){pOBBNodeL = NULL; pOBBNodeR = NULL;}
};


#define	D3DFVF_XYZ_COLOR	(D3DFVF_XYZ | D3DFVF_DIFFUSE)

struct	VERTEX_XYZ_COLOR
{
    FLOAT		x, y, z;
	D3DCOLOR	color;
};

class COBBTree
{
public:
	COBBTree();
	~COBBTree();
	bool	Create(LPD3DXMESH pMesh, int Level);
	void	Create(FACES &Face, int Level, OBBNODE *pOBBNode);
	void	Draw(int iDrawLevel)
	{
		Draw(pOBBTopNode, iDrawLevel, 0);
	};
	void	HitBoxGet(int iDrawLevel, int iNum, bool bDraw, int *pCount)
	{
		HitBoxGet(pOBBTopNode, iDrawLevel, 0, iNum, bDraw, pCount);
	};
	bool	CheckCollision(COBBTree &OBBTreeA, MTX &TransMatA, COBBTree &OBBTreeB, MTX &TransMatB);

private:
	void	CreateOBB(std::vector < FACE > &Face, OBBDATA &OBBData);
	void	Rotate(float a[3][3], float s, float tau, int i, int j, int k, int l);
	bool	Jacobi(float a[3][3], float v[3][3], float d[3]);
	void	DrawOBB(OBBDATA OBBData);
	void	Draw(OBBNODE *pOBBNode, int DrawLevel, int Level);
	void	HitBoxGet(OBBNODE *pOBBNode, int iDrawLevel, int iLevel, int iNum, bool bDraw, int *pCount);
	void	ReleaseNodes(OBBNODE *pOBBNode);
	bool	IsCollision(OBBDATA &BoxA, OBBDATA &BoxB);
	bool	CheckCollision(OBBNODE *pNodeA, OBBNODE *pNodeB, MTX &TransMat, MTX &RotMat);

public:
	
	std::vector	< D3DXVECTOR3 > vecHit;		// ヒットボックス用
	OBBNODE	*pOBBTopNode;
};


#endif // __COBBTREE_H__



#endif