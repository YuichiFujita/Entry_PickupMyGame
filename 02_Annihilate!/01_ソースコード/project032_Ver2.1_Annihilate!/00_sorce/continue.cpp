//======================================================================================================================
//
//	�R���e�j���[���� [continue.cpp]
//	Author�F���c�E��
//
//======================================================================================================================
//**********************************************************************************************************************
//	�C���N���[�h�t�@�C��
//**********************************************************************************************************************
#include "main.h"
#include "input.h"
#include "sound.h"
#include "fade.h"

#include "continue.h"

//**********************************************************************************************************************
//	�}�N����`
//**********************************************************************************************************************
#define MAX_CONTINUE		(3)			// �g�p����|���S����

#define CONTINUE_POS_X		(365.0f)	// �R���e�j���[�̐�΍��W (x)
#define CONTINUE_POS_Y		(575.5f)	// �R���e�j���[�̐�΍��W (y)
#define CONTINUE_WIDTH		(340.0f)	// �R���e�j���[�̉��� / 2
#define CONTINUE_HEIGHT		(115.0f)	// �R���e�j���[�̏c�� / 2

#define CON_YESNO_POS_X		(220.0f)	// �I�����̐�΍��W (x)
#define CON_YESNO_POS_Y		(630.0f)	// �I�����̐�΍��W (y)
#define CON_YESNO_SPACE		(290.0f)	// �I�����Ԃ̕� (x)
#define CON_YESNO_WIDTH		(120.0f)	// �I�����̉��� / 2
#define CON_YESNO_HEIGHT	(50.0f)		// �I�����̏c�� / 2

//**********************************************************************************************************************
//	�R���X�g��`
//**********************************************************************************************************************
const char *apTextureContinue[] =		// �e�N�X�`���̑��΃p�X
{
	"02_data\\02_TEXTURE\\continue000.png",	// �R���e�j���[�w�i�̃e�N�X�`�����΃p�X
	"02_data\\02_TEXTURE\\continue001.png",	// YES�̃e�N�X�`�����΃p�X
	"02_data\\02_TEXTURE\\continue002.png",	// NO�̃e�N�X�`�����΃p�X
};

//**********************************************************************************************************************
//	�񋓌^��` (CONTINUESELECT)
//**********************************************************************************************************************
typedef enum
{
	CONTINUESELECT_YES = 0,				// YES���
	CONTINUESELECT_NO,					// NO���
	CONTINUESELECT_MAX					// ���̗񋓌^�̑���
} CONTINUESELECT;

//**********************************************************************************************************************
//	�񋓌^��` (TEXTURE_CONTINUE)
//**********************************************************************************************************************
typedef enum
{
	TEXTURE_CONTINUE_BG,				// �R���e�j���[�̔w�i
	TEXTURE_CONTINUE_YES,				// YES
	TEXTURE_CONTINUE_NO,				// NO
	TEXTURE_CONTINUE_MAX,				// ���̗񋓌^�̑���
} TEXTURE_CONTINUE;

//**********************************************************************************************************************
//	�v���g�^�C�v�錾
//**********************************************************************************************************************
void SelectContinue(void);	// �R���e�j���[�̑��쏈��

//**********************************************************************************************************************
//	�O���[�o���ϐ�
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9      g_apTextureContinue[TEXTURE_CONTINUE_MAX] = {};	// �e�N�X�`���ւ̃|�C���^
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffContinue = NULL;						// ���_�o�b�t�@�ւ̃|�C���^

int  g_nSelectContinue;		// ���݂̑I�𒆃��j���[
int  g_nOldSelectContinue;	// �O��̑I�𒆃��j���[

//======================================================================================================================
//	�R���e�j���[�̏���������
//======================================================================================================================
void InitContinue(void)
{
	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^
	VERTEX_2D        *pVtx;						// ���_���ւ̃|�C���^

	for (int nCntContinue = 0; nCntContinue < TEXTURE_CONTINUE_MAX; nCntContinue++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		// �e�N�X�`���̓ǂݍ���
		D3DXCreateTextureFromFile(pDevice, apTextureContinue[nCntContinue], &g_apTextureContinue[nCntContinue]);
	}

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer
	( // ����
		sizeof(VERTEX_2D) * 4 * MAX_CONTINUE,	// �K�v���_��
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,							// ���_�t�H�[�}�b�g
		D3DPOOL_MANAGED,
		&g_pVtxBuffContinue,
		NULL
	);

	// �O���[�o���ϐ��̒l��������
	g_nSelectContinue    = CONTINUESELECT_YES;	// ���݂̑I�𒆃��j���[��������
	g_nOldSelectContinue = CONTINUESELECT_YES;	// �O��̑I�𒆃��j���[��������

	//------------------------------------------------------------------------------------------------------------------
	//	���_���̏�����
	//------------------------------------------------------------------------------------------------------------------
	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffContinue->Lock(0, 0, (void**)&pVtx, 0);

	//------------------------------------------------------------------------------------------------------------------
	//	�w�i�̏�����
	//------------------------------------------------------------------------------------------------------------------
	// ���_���W��ݒ�
	pVtx[0].pos = D3DXVECTOR3(CONTINUE_POS_X - CONTINUE_WIDTH, CONTINUE_POS_Y - CONTINUE_HEIGHT, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(CONTINUE_POS_X + CONTINUE_WIDTH, CONTINUE_POS_Y - CONTINUE_HEIGHT, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(CONTINUE_POS_X - CONTINUE_WIDTH, CONTINUE_POS_Y + CONTINUE_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(CONTINUE_POS_X + CONTINUE_WIDTH, CONTINUE_POS_Y + CONTINUE_HEIGHT, 0.0f);

	// rhw �̐ݒ�
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// ���_�J���[�̐ݒ�
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//------------------------------------------------------------------------------------------------------------------
	//	�Z���N�g���j���[�̏�����
	//------------------------------------------------------------------------------------------------------------------
	for (int nCntContinue = 0; nCntContinue < CONTINUESELECT_MAX; nCntContinue++)
	{ // �I�����̍��ڐ����J��Ԃ�

		// ���_���W��ݒ�
		pVtx[4].pos = D3DXVECTOR3(CON_YESNO_POS_X + (nCntContinue * CON_YESNO_SPACE) - CON_YESNO_WIDTH, CON_YESNO_POS_Y - CON_YESNO_HEIGHT, 0.0f);
		pVtx[5].pos = D3DXVECTOR3(CON_YESNO_POS_X + (nCntContinue * CON_YESNO_SPACE) + CON_YESNO_WIDTH, CON_YESNO_POS_Y - CON_YESNO_HEIGHT, 0.0f);
		pVtx[6].pos = D3DXVECTOR3(CON_YESNO_POS_X + (nCntContinue * CON_YESNO_SPACE) - CON_YESNO_WIDTH, CON_YESNO_POS_Y + CON_YESNO_HEIGHT, 0.0f);
		pVtx[7].pos = D3DXVECTOR3(CON_YESNO_POS_X + (nCntContinue * CON_YESNO_SPACE) + CON_YESNO_WIDTH, CON_YESNO_POS_Y + CON_YESNO_HEIGHT, 0.0f);

		// rhw �̐ݒ�
		pVtx[4].rhw = 1.0f;
		pVtx[5].rhw = 1.0f;
		pVtx[6].rhw = 1.0f;
		pVtx[7].rhw = 1.0f;

		// ���_�J���[�̐ݒ�
		pVtx[4].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.0f);
		pVtx[5].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.0f);
		pVtx[6].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.0f);
		pVtx[7].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.0f);

		// �e�N�X�`�����W�̐ݒ�
		pVtx[4].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[5].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[6].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[7].tex = D3DXVECTOR2(1.0f, 1.0f);

		// ���_�f�[�^�̃|�C���^�� 4���i�߂�
		pVtx += 4;
	}

	// ���_�o�b�t�@���A�����b�N����
	g_pVtxBuffContinue->Unlock();
}

//======================================================================================================================
//	�R���e�j���[�̏I������
//======================================================================================================================
void UninitContinue(void)
{
	// �e�N�X�`���̔j��
	for (int nCntContinue = 0; nCntContinue < TEXTURE_CONTINUE_MAX; nCntContinue++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		if (g_apTextureContinue[nCntContinue] != NULL)
		{ // �ϐ� (g_apTextureContinue) ��NULL�ł͂Ȃ��ꍇ

			g_apTextureContinue[nCntContinue]->Release();
			g_apTextureContinue[nCntContinue] = NULL;
		}
	}

	// ���_�o�b�t�@�̔j��
	if (g_pVtxBuffContinue != NULL)
	{ // �ϐ� (g_pVtxBuffContinue) ��NULL�ł͂Ȃ��ꍇ

		g_pVtxBuffContinue->Release();
		g_pVtxBuffContinue = NULL;
	}
}

//======================================================================================================================
//	�R���e�j���[�̍X�V����
//======================================================================================================================
void UpdateContinue(float fAlpha, ALPHASTATE state)
{
	// �|�C���^��錾
	VERTEX_2D *pVtx;	// ���_���ւ̃|�C���^

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffContinue->Lock(0, 0, (void**)&pVtx, 0);

	switch (state)
	{ // �����x�̕ύX��Ԃ��Ƃ̏���
	case ALPHASTATE_CHANGE:	// �����x�̕ύX���

		// ���_�J���[�̐ݒ�
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);

		for (int nCntContinue = 0; nCntContinue < CONTINUESELECT_MAX; nCntContinue++)
		{ // �I�����̍��ڐ����J��Ԃ�

			// ���_�J���[�̐ݒ�
			pVtx[4].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, fAlpha);
			pVtx[5].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, fAlpha);
			pVtx[6].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, fAlpha);
			pVtx[7].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, fAlpha);

			// ���_�f�[�^�̃|�C���^�� 4���i�߂�
			pVtx += 4;
		}

		// �����𔲂���
		break;

	case ALPHASTATE_NONE:	// �������Ă��Ȃ����

		// �R���e�j���[�̑��쏈��
		SelectContinue();

		// ���_�f�[�^�̃|�C���^��O�t���[���ɃZ���N�g���Ă����v�f�̒��_�Ɉړ�
		pVtx += (g_nOldSelectContinue * 4);

		// ���_�J���[�̍X�V
		pVtx[4].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[5].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[6].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[7].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);

		// ���_�f�[�^�̃|�C���^��擪�A�h���X�ɖ߂�
		pVtx -= (g_nOldSelectContinue * 4);

		// ���_�f�[�^�̃|�C���^�����t���[���ŃZ���N�g���Ă���v�f�̒��_�Ɉړ�
		pVtx += (g_nSelectContinue * 4);

		// ���_�J���[�̍X�V
		pVtx[4].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[5].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[6].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[7].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// ���݃Z���N�g���̗v�f����
		g_nOldSelectContinue = g_nSelectContinue;

		// �����𔲂���
		break;
	}

	// ���_�o�b�t�@���A�����b�N����
	g_pVtxBuffContinue->Unlock();
}

//======================================================================================================================
//	�R���e�j���[�̕`�揈��
//======================================================================================================================
void DrawContinue(float fAlpha)
{
	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, g_pVtxBuffContinue, 0, sizeof(VERTEX_2D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntContinue = 0; nCntContinue < MAX_CONTINUE; nCntContinue++)
	{ // �g�p����|���S�������J��Ԃ�

		// �e�N�X�`���̐ݒ�
		pDevice->SetTexture(0, g_apTextureContinue[nCntContinue]);

		// �|���S���̕`��
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntContinue * 4, 2);
	}
}

//======================================================================================================================
//	�R���e�j���[�̃��l�̕␳����
//======================================================================================================================
void RevAlphaContinue(void)
{
	// �|�C���^��錾
	VERTEX_2D *pVtx;	// ���_���ւ̃|�C���^

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffContinue->Lock(0, 0, (void**)&pVtx, 0);

	// ���_�J���[�̐ݒ�
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	for (int nCntContinue = 0; nCntContinue < CONTINUESELECT_MAX; nCntContinue++)
	{ // �I�����̍��ڐ����J��Ԃ�

		// ���_�J���[�̐ݒ�
		pVtx[4].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[5].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[6].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		pVtx[7].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);

		// ���_�f�[�^�̃|�C���^�� 4���i�߂�
		pVtx += 4;
	}

	// ���_�o�b�t�@���A�����b�N����
	g_pVtxBuffContinue->Unlock();
}

//======================================================================================================================
//	�R���e�j���[�̑��쏈��
//======================================================================================================================
void SelectContinue(void)
{
	if (GetFade() == FADE_NONE)
	{ // �t�F�[�h���Ă��Ȃ��ꍇ

		if (GetKeyboardTrigger(DIK_A) == true || GetKeyboardTrigger(DIK_LEFT) == true || GetJoyKeyTrigger(JOYKEY_LEFT, 0) == true)
		{ // ���ړ��̑��삪�s��ꂽ�ꍇ

			// �ϐ� (g_nSelectContinue) �� 1�������Ă��� ( 0�̏ꍇ 1�ɖ߂�)
			g_nSelectContinue = (g_nSelectContinue + (CONTINUESELECT_MAX - 1)) % CONTINUESELECT_MAX;

			// �T�E���h�̍Đ�
			PlaySound(SOUND_LABEL_SE_MOVE);		// SE (�J�[�\���ړ�)
		}
		else if (GetKeyboardTrigger(DIK_D) == true || GetKeyboardTrigger(DIK_RIGHT) == true || GetJoyKeyTrigger(JOYKEY_RIGHT, 0) == true)
		{ // �E�ړ��̑��삪�s��ꂽ�ꍇ

			// �ϐ� (g_nSelectContinue) �� 1�������Ă��� ( 1�̏ꍇ 0�ɖ߂�)
			g_nSelectContinue = (g_nSelectContinue + 1) % CONTINUESELECT_MAX;

			// �T�E���h�̍Đ�
			PlaySound(SOUND_LABEL_SE_MOVE);		// SE (�J�[�\���ړ�)
		}

		if (GetKeyboardTrigger(DIK_RETURN) == true || GetKeyboardTrigger(DIK_SPACE) == true
		||  GetJoyKeyTrigger(JOYKEY_A, 0)  == true || GetJoyKeyTrigger(JOYKEY_B, 0) == true
		||  GetJoyKeyTrigger(JOYKEY_X, 0)  == true || GetJoyKeyTrigger(JOYKEY_Y, 0) == true)
		{ // ����̑��삪�s��ꂽ�ꍇ

			// �T�E���h�̍Đ�
			PlaySound(SOUND_LABEL_SE_DEC_00);	// SE (����00)

			switch (g_nSelectContinue)
			{ // ��ʂ̈ڍs��̊m�F
			case CONTINUESELECT_YES:	// YES�̏ꍇ

				// ���[�h�I�� (�Q�[����ʂɈڍs)
				SetFade(MODE_GAME);

				// �����𔲂���
				break;

			case CONTINUESELECT_NO:		// NO�̏ꍇ

				// ���[�h�I�� (�^�C�g����ʂɈڍs)
				SetFade(MODE_TITLE);

				// �����𔲂���
				break;
			}
		}
	}
}