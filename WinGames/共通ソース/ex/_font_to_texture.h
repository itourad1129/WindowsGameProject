//
// �t�H���g���e�N�X�`���[�֓W�J
//

#ifndef	__FONT_TO_TEXTURE_H

#define	__FONT_TO_TEXTURE_H


/*------------------------------------------------------------------------------*
| <<< �e�N�X�`���[�ɃA�E�g���C���t�H���g���쐬 >>>
|	����	iTex        = �������쐬����e�N�X�`���[�ԍ�
|			iX, iY		= �����̕\�����W
|			strFontName = TrueType �t�H���g��
|			str		    = �`�敶��
|			iHeight     = �t�H���g��
|			iWidth      = �t�H���g�̉���
|			bItalic		= TRUE: �Α�
|			iPenSize    = �A�E�g���C���̑���(�s�N�Z��)
|			dwEdge      = �A�E�g���C���̐F
|			dwFill      = �h��Ԃ��F
|			iQuality    = �A���`�G�C���A�X���x( 1 �ȏ�̒l�ɂ��邱��)
*------------------------------------------------------------------------------*/
extern	void	font_to_texture(int iTex, int iX, int iY, char *strFontName, int iHeight, int iWidth, BOOL bItalic, int iPenSize, DWORD dwEdge, DWORD dwFill, int iQuality, char* Format,...);

/*------------------------------------------------------------------------------*
| <<< �e�N�X�`���[���N���A >>>
|	����	iTex = �N���A����e�N�X�`���[
*------------------------------------------------------------------------------*/
extern	void	font_to_texture_clear(int iTex);

/*------------------------------------------------------------------------------*
| <<< �`��I�����W�𓾂� >>>
|	�߂�l	�`��I�����W X
*------------------------------------------------------------------------------*/
extern	int		font_to_texture_get_next_x(void);

/*------------------------------------------------------------------------------*
| <<< �`��I�����W�𓾂� >>>
|	�߂�l	�`��I�����W Y
*------------------------------------------------------------------------------*/
extern	int		font_to_texture_get_next_y(void);

#endif					// end _font_to_texture.h

