//
//	�^�u���b�g����
//
//
//	�^�u���b�g�����ɂ� �^�u���b�g�h���C�o�[�wWintab32.dll�x���K�v�ɂȂ�܂��B
//	�^�u���b�g�����삵�Ȃ������ŁA�^�u���b�g�������Ăяo���ƃG���[���������܂��B
//	�wWintab32.dll�x���wWidows\\System32�x�܂��́A���s�t�@�C���Ɠ����t�H���_�[�ɒu���΃G���[�͉���ł��܂��B
//	�wWintab32.dll�x�͍Ĕz�z�\�� DLL �t�@�C���ł��B


//#include	"main.h"


//--- �萔��` ------------------------------------------------------------------------------------
#define	PACKETDATA		(PK_CURSOR | PK_X | PK_Y | PK_BUTTONS | PK_NORMAL_PRESSURE | PK_ORIENTATION)
#define	PACKETMODE		PK_BUTTONS
#include	<pktdef.h>


//--- ���C�v�����B --------------------------------------------------------------------------------
#pragma comment(lib,"wintab32.lib")
#pragma comment(lib,"wntab32x.lib")

//--- �ϐ���` ------------------------------------------------------------------------------------
#define	NPACKETQSIZE	32			// �^�u���b�g�̃f�[�^�o�b�t�@�T�C�Y

static	HCTX	hctxTablet;
static	PACKET	pacektBuf[NPACKETQSIZE];	// �p�P�b�g��M�p
static	POINT	ptNew;						// �y���̍��W�l�i�[�p
static	POINT	ptOld;						// ���O�̍��W�l�i�[�p

		TABLET_WORK	tablet;					// �^�u���b�g���[�N
static	TABLET_WORK	*tb = &tablet;




/*------------------------------------------------------------------------------*
| <<< �^�u���b�g������ >>>
|	�� �}�E�X�C�x���g�ɘA������^�C�v
|	�@ �^�u���b�g�̃C�x���g�́A�}�E�X�̃C�x���g�����B
|	   �^�u���b�g�̏��� WndProc �̃}�E�X�C�x���g��蓾����
*------------------------------------------------------------------------------*/
HCTX	TabletInit(void)
{

	// �f�t�H���g�ݒ�̎擾
	LOGCONTEXT	lcMine;
	WTInfo(WTI_DEFSYSCTX, 0, &lcMine);
  
	// �ݒ�l�̏C��
	wsprintf(lcMine.lcName, "tablet program %x", GetCurrentProcessId());
	lcMine.lcPktData   = PACKETDATA;
	lcMine.lcPktMode   = PACKETMODE;
	lcMine.lcMoveMask  = PACKETDATA;
	lcMine.lcBtnUpMask = lcMine.lcBtnDnMask;

	// �}�E�X���[�h�p�̐ݒ�
	lcMine.lcOutOrgX = 0;
	lcMine.lcOutExtX = GetSystemMetrics(SM_CXSCREEN);
	lcMine.lcOutOrgY = 0;
	lcMine.lcOutExtY = -GetSystemMetrics(SM_CYSCREEN);
  
	// �p�P�b�g��M�J�n
	hctxTablet = WTOpen(sys.hWnd, &lcMine, TRUE);

	// ��M�o�b�t�@�̐ݒ�(�{���́u�o�b�t�@���ӂ�̃G���[�������K�v�v�������ł�)
	WTQueueSizeSet(hctxTablet, NPACKETQSIZE);

	return	hctxTablet;
}



/*------------------------------------------------------------------------------*
| <<< �^�u���b�g�̃C�x���g >>>
|	����	hWnd		   = �E�B���h�E�n���h��
|			uiMsg		   = ���b�Z�[�W
|			wParam, lParam = �p�����[�^�[
*------------------------------------------------------------------------------*/
void	TabletProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{

	switch(uiMsg)
	{
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
		// �ŏ������͏������Ȃ�
		if(IsIconic(hWnd))
		{
			break;
		}
		else
		{
			// �^�u���b�g�̏��𓾂�(������ iMax �񏈗����悤)
			int	iMax = WTPacketsGet(hctxTablet, NPACKETQSIZE, &pacektBuf);
			for(int i = 0; i < iMax; i++)
			{
				// �p�P�b�g�̌������������Ȃ���΂Ȃ�Ȃ��̂����A�ȗ����̂��߁A�Ō�̃p�P�b�g�݂̂ɂ���B
				//i--;

				// �O��̃f�[�^�ێ�
				ptOld  = ptNew;

				// �V�����p�P�b�g�̕ێ�
				ptNew.x = (UINT)pacektBuf[i].pkX;
				ptNew.y = (UINT)pacektBuf[i].pkY;

				//--- �}���`���j�^�[�p�Ƀf�B�X�v���C�ʒu�𖈓x�v�Z���� --------
				POINT			ptWindow ={      0,       0};
				POINT			ptIn    = {ptNew.x, ptNew.y};
				GetCursorPos(&ptIn);
				HMONITOR		hMonitor = MonitorFromPoint(ptIn, MONITOR_DEFAULTTONEAREST);
				MONITORINFOEX	stMonInfoEx;
				stMonInfoEx.cbSize = sizeof(stMonInfoEx);
				GetMonitorInfo(hMonitor, &stMonInfoEx);
				ClientToScreen(hWnd, &ptWindow);
				//-------------------------------------------------------------

				// �N���C�A���g���W�ւ̕ϊ�
				tb->iX = ptIn.x - ptWindow.x;
				tb->iY = ptIn.y - ptWindow.y;

				// �N���b�N���
				//tb->bClick = tb->bPush ^ tb->bTabletBak;
				//tb->bTabletBak   = tb->bPush;

				// �������
				if(     HIWORD(pacektBuf[i].pkButtons) == TBN_DOWN){ tb->bPush = true; }
				else if(HIWORD(pacektBuf[i].pkButtons) == TBN_UP  ){ tb->bPush = false;}

				// ���������
				//tb->bRelease = tb->bPush ^ tb->bTabletBak;

				// �^�u���b�g���W�A�M���A�ړ���
				//tb->iTabletXBak     = tb->iX;						// ���W
				//tb->iTabletYBak     = tb->iY;

				tb->iPressure = pacektBuf[i].pkNormalPressure;	// �M��

				//tb->iTabletMoveX    = tb->iTabletXBak - tb->iX;	// �ړ���
				//tb->iTabletMoveY    = tb->iTabletYBak - tb->iY;
			}
		}
		break;
	}
}



#if 0

	// �^�u���b�g���E�B���h�E��������^�C�v�̏����B
	// �}�E�X�J�[�\���ƘA�����Ȃ��̂ŁA������ƕs�ւ����H
	// ����\�͏�L�̂��̂��D��Ă��܂��B

	static	long	lcOutExtX, lcOutExtY;

	/*------------------------------------------------------------------------------*
	| <<< �^�u���b�g������ >>>
	*------------------------------------------------------------------------------*/
	HCTX	TabletInit2(void)
	{

		LOGCONTEXT	lcMine;
  
		// WTInfo���g�p���ăf�t�H���g�ݒ�l���擾
		WTInfo(WTI_DEFCONTEXT, 0, &lcMine);
  
		// �K�v�ȍ��ڂ�ݒ�
		wsprintf(lcMine.lcName, "WinTab test %x", GetCurrentProcessId());
		lcMine.lcOptions  |= CXO_MESSAGES;  // WT���b�Z�[�W���n�����悤�ɂ���
		lcMine.lcPktData   = PACKETDATA;
		lcMine.lcPktMode   = PACKETMODE;
		lcMine.lcMoveMask  = PACKETDATA;
		lcMine.lcBtnUpMask = lcMine.lcBtnDnMask;

		// �^�u���b�g�̉𑜓x�ݒ� 
		// �^�u���b�g�h���C�o�̃X�P�[�����O�ݒ�Ɋ֌W�Ȃ��^�u���b�g�̕���\���̂��̂���荞��
		// �f�t�H���g�ݒ�l�̓h���C�o�Őݒ肳��Ă���X�P�[�����O�ɂȂ��Ă���
		lcMine.lcOutOrgX = lcMine.lcOutOrgY = 0;
		lcMine.lcOutExtX = lcMine.lcInExtX;
		lcMine.lcOutExtY = lcMine.lcInExtY;
  
		// �^�u���b�g�T�C�Y���i�[���Ă���
		lcOutExtX = lcMine.lcOutExtX;
		lcOutExtY = lcMine.lcOutExtY;
  
		// �p�P�b�g��M�J�n
		return WTOpen(sys.hWnd, &lcMine, TRUE);
	}


	/*------------------------------------------------------------------------------*
	| <<< �^�u���b�g�̃f�[�^��M�C�x���g >>>
	*------------------------------------------------------------------------------*/
	void	TabletReceive(WPARAM wParam, LPARAM lParam)
	{

		//--- WndProc �Ɉȉ������Ă������� ------------------------
		//	case WT_PACKET:{ TabletReceive(wParam, lParam); break;}

		PACKET	pkt;

		if(WTPacket((HCTX)lParam, wParam, &pkt))
		{
			// �N���b�N���
			tb->bClick = tb->bPush ^ tb->bTabletBak;
			tb->bTabletBak   = tb->bPush;

			// �������
			if(     HIWORD(pkt.pkButtons) == TBN_DOWN){ tablet.bPush = true; }
			else if(HIWORD(pkt.pkButtons) == TBN_UP  ){ tablet.bPush = false;}
			
			// �^�u���b�g���W�A�M���A�ړ���
			tb->iTabletXBak     = tb->iX;
			tb->iTabletYBak     = tb->iY;

			tb->iX        =			    (int)(((float)pkt.pkX / (float)lcOutExtX) * (float)d3.sRenderW);
			tb->iY        = d3.sRenderH - (int)(((float)pkt.pkY / (float)lcOutExtY) * (float)d3.sRenderH);

			tb->iTabletPressure = pkt.pkNormalPressure;				// �M��

			tb->iTabletMoveX    = tb->iTabletXBak - tb->iX;	// �ړ���
			tb->iTabletMoveY    = tb->iTabletYBak - tb->iY;
		}
	}
#endif
