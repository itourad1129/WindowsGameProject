//
// �A�v���P�[�V�����̏��ݒ�
//


#define	__SEPARATOR			1		// �t�@�C�����j���[�̃Z�p���[�^�[�ʒu
#define	__NEAR			    0.01f	// ��O�̃N���b�s���O�̈�
#define	__FAR				1000.f	// ���̃N���b�s���O�̈�
									// ���_�V�F�[�_�[�̃o�[�W����
#define	__VS_VERSION		D3DVS_VERSION(2, 0)
									// �s�N�Z���V�F�[�_�[�̃o�[�W����
#define	__PS_VERSION		D3DPS_VERSION(2, 0)


#define	__SCR_W				640		// �E�B���h�[�����T�C�Y
#define	__SCR_H				480
#define	__RENDER_X0			  0		// �����_�����O�͈�
#define	__RENDER_Y0			  0
#define	__RENDER_X1			640
#define	__RENDER_Y1			480

#define	__SAVE_WINDOW				// ����ް�ʒu�ۑ�
#define	__WINDOW					// �E�B���h�[���[�h�ŋN��
#define	__WINCURSOR					// �E�B���h�[���}�E�X�\��
#define	__ESC_TO_EXIT				// ESC �ŃA�v���I��
#define	__2D_RESIZE					// __SCR_W,__SCR_H �Ń��T�C�Y����

//--- ���C�u�����B --------------------------------------------------------------------------------
#define	__D3						// Direct3D �g�p(�K���L���ɂ��Ă�������)
#define	__PAD						// �L�[�{�[�h�A�W���C�p�b�g�g�p(�}�E�X�͕�)
#define	__SND						// �T�E���h�g�p
#define	__MQO						// ���^�Z�R�C���p���C�u�����B
#define	__CALC						// �Z�p���C�u�����B
#define	__COLLISION					// �Փ˔��胉�C�u�����B
#define	__SKIN						// �X�L�����b�V�����C�u�����B

//--- �f�o�b�O���[�h���̃X�C�b�` ------------------------------------------------------------------
#ifdef	_DEBUG
	#define	__ERROR_MES_OUTPUT		// �G���[���b�Z�[�W���o�͂֕\��
	#define	__SYSTEM_DEBUG			// �V�X�e���f�o�b�O���[�h(TAB�ő�����A1 �ŃX�g�b�v�A2 �ň�R�}�i��)
	#define	__SHOW_FPS				// FPS �\��
	#define	__HIT_FRAME				// �q�b�g�g�`��

//--- �����[�X���[�h���̃X�C�b�` ------------------------------------------------------------------
#else
	#define	__EXIT_MENU				// �I���m�F�\��
#endif


//--- �C���N���[�h --------------------------------------------------------------------------------
#include	"resource.h"
#include	"_sys.h"
#include	"includes.h"
