//
//	マップ
//

#include	"main.h"

//--- 変数定義 ----------------------------------------------------------
char		chMap[	 MAP_SIZE];			// 仮想マップのデータをコピーして使用する
char		chMapBak[MAP_SIZE];			// 仮想マップのバックアップ用
/*------------------------------------------------------------------------------*
| <<< マップ読み込み >>>
|	入力	iStage = 読み込み番号
*-------------------------------------------------------------------------------*/
void	MapLoad(int iStage)
{
	// 面データをコピー
	memcpy(chMap,	&chStage[iStage][2], sizeof(chMap));

	// マップのバックアップ作成
	memcpy(chMapBak, &chStage[iStage][2], sizeof(chMap));

	// プレイヤー初期座標
	player->SetPos((float)chStage[iStage][0] * BLK_SIZE, (float)chStage[iStage][1] * BLK_SIZE);
}

/*------------------------------------------------------------------------------*
| <<< マップ描画 >>>
*-------------------------------------------------------------------------------*/
void	MapDraw(void)
{
	//RECT	r;

	for(int y = 0; y < MAP_H; y++)		// 縦にループ
	{
		for(int x = 0; x < MAP_W; x++)	// 横にループ
		{
			int		a = 0;
			char	b = chMap[ADR(x, y)];	// マクロ ADR を使いアドレス計算し、マップチップを得る

			float	fX = (float)x * BLK_SIZE + BLK_SIZE / 2.f;
			float	fY = (float)y * BLK_SIZE + BLK_SIZE / 2.f;

			if(b == '@'){ a = X_WALL;}		// 壁		※半角アットマーク
			if(b == 'o'){ a = X_NIMOTU;}		//荷物
			if(b == 'x'){ a = X_MOKUTEKICHI;}		//荷物を運ぶ位置
			if(b == '-'){continue;}		//なにもないところ
			if(b == ' '){continue;}		//倉庫の床


			MTX		mtxWorld;
			D3DXMatrixTranslation(&mtxWorld, ( fX - __SCR_W / 2 + BLK_SIZE / 2.f) / 56.0f,		// x 座標
												0,												// y 座標
												-( fY - __SCR_H / 2 + BLK_SIZE / 2.f) / 56.0f);// z 座標(マイナスが付きます)
			d3_3d_draw(a, &mtxWorld);

			// ここで荷物、床、目的地が描画されるように処理してください

			// 画像切り出し
			//SetRect(&r, a * BLK_SIZE, 0, a * BLK_SIZE + BLK_SIZE, BLK_SIZE);

			// 描画
			
			//d3_spr_draw(TEX_MAP, &r, &VEC3(fX, fY, 0));
		}
	}
}