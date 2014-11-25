


/**************************************************************************************************
	[[[	メッシュ読み込み、作成系 ]]]
**************************************************************************************************/
extern	float	gr_xfile_xmin_get(int iXNum);								// ＸファイルのＸの最小値を得る
extern	float	gr_xfile_xmax_get(int iXNum);								// ＸファイルのＸの最大値を得る
extern	float	gr_xfile_ymin_get(int iXNum);								// ＸファイルのＹの最小値を得る
extern	float	gr_xfile_ymax_get(int iXNum);								// ＸファイルのＹの最大値を得る
extern	float	gr_xfile_zmin_get(int iXNum);								// ＸファイルのＺの最小値を得る
extern	float	gr_xfile_zmax_get(int iXNum);								// ＸファイルのＺの最大値を得る

extern	void	d3_hit_work_get(D3_HIT	*pHit, LPD3DXMESH pMesh);
extern	void	d3_xfile_free(	  int iNum);
extern	void	d3_xfile_free_all(void);
extern	void	d3_mesh_make(int iNum,	LPD3DXMESH pMesh);
extern	bool	d3_xfile_check(int iNum);
extern	bool	d3_xfile_load(int iNum, char *pstrFile, bool bLoadAtOnce, bool bSysMem, bool bErrMessage);
extern	bool	d3_xfile_load(int iNum, char *pstrFile, bool bLoadAtOnce, bool bSysMem);
extern	bool	d3_xfile_load(int iNum, char *pstrFile, bool bLoadAtOnce);
extern	bool	d3_xfile_load(int iNum, char *pstrFile);
extern	bool	d3_mesh_tween(int iOutNum, int iIn0Num, int iIn1Num, float fS);
extern	bool	d3_text_create(int iNum, TCHAR *pstrFont, int size, float extrusion, char *str);



extern	bool			d3_mesh_intersection_check(VEC3 *pOutCrossPos, float *pOutCrossDist, int iNum, int iMat, VEC3 *pPos, VEC3 *pDir, float fSize, MTX *mtx);
extern	bool			d3_mesh_intersection_check(VEC3 *pOutCrossPos, float *pOutCrossDist, int iNum, int iMat, VEC3 *pPos, VEC3 *pDir, float fSize);
extern	bool			d3_mesh_intersection_check(float *pOutCrossDist, int iNum, int iMat, VEC3 *pPos, VEC3 *pDir, float fSize);
extern	bool			d3_mesh_intersection_check(int iNum, int iMat, VEC3 *pPos, VEC3 *pDir, float fSize, MTX *mtx);
extern	bool			d3_mesh_intersection_check(int iNum, int iMat, VEC3 *pPos, VEC3 *pDir, float fSize);
extern	bool			d3_mesh_intersection_check(int iNum, int iMat, VEC3 *pPos, VEC3 *pDir);
extern	D3_VTX_DEFAULT	*d3_mesh_intersection_face(void);
extern	int				d3_mesh_intersection_face_num_get(void);

extern	bool	d3_hit_box_check(VEC3 *a, VEC3 *b);
extern	void	d3_hit_box_draw( VEC3 *pp);
extern	bool	d3_hit_intersect(VEC3 *out, VEC3 *pos, VEC3	*dir, VEC3 *v0, VEC3 *v1, VEC3 *v2);
