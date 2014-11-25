//
//	アンドゥ処理
//

#include	"main.h"


#define	UNDO_MAX	100				// アンドゥの最大級
#define UNDO_SIZE	(MAP_SIZE + 3)

//--- アンドゥ用 ------------------------------------------------------------------

static	char	cUndoDat[UNDO_MAX][UNDO_SIZE];


/*----------------------------------------------------------------------*
| <<< アンドゥー処理 >>>
|	入力	iSw = UNDO_RESET  : 初期化する
|				= UNDO_SET	  : UNDO データをリセットする
|				= UNDO_GET	  : 実際に UNDO する
*-----------------------------------------------------------------------*/
void	Undo(int	iSw)
{
	switch(iSw)
	{
		//--- 初期化する 面の最初にやる
	case UNDO_RESET:
		memset(cUndoDat, 0, sizeof(cUndoDat));
		break;

	//--- 一歩進むごとにやる
	case UNDO_SET:
		// データの一番古い物を放棄する
		memmove(&cUndoDat[1][0], &cUndoDat[0][0], UNDO_SIZE * (UNDO_MAX - 1));

		// UNDO 用にデータ設定
		cUndoDat[0][0] = (char)(player->GetXPos() / BLK_SIZE);
		cUndoDat[0][1] = (char)(player->GetYPos() / BLK_SIZE);
		cUndoDat[0][2] = (char)player->GetDir();
		memcpy(&cUndoDat[0][3], chMap, MAP_SIZE);
		break;

	//--- 前のデータを取り出したいときに実行
	case UNDO_GET:
		if(cUndoDat [0][0] == 0){break;}
		player->SetPos((float)(cUndoDat[0][0] * BLK_SIZE), (float)(cUndoDat[0][1] * BLK_SIZE));
		player->SetDir(cUndoDat[0][2]);

		memcpy(chMap, &cUndoDat[0][3], MAP_SIZE);
		memcpy(&cUndoDat[0][0], &cUndoDat[1][0], UNDO_SIZE * (UNDO_MAX -1));
		memset(&cUndoDat[UNDO_MAX - 1][0], 0,	UNDO_SIZE);
		break;
	}
}