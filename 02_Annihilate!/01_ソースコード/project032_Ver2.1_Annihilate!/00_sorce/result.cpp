//======================================================================================================================
//
//	���U���g���� [result.cpp]
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

#include "enemy.h"
#include "result.h"
#include "score.h"
#include "timer.h"
#include "value.h"
#include "weed.h"

//**********************************************************************************************************************
//	�}�N����`
//**********************************************************************************************************************
#define MAX_RESULT		(1)			// �g�p����|���S����
#define MAX_VAL_RES		(3)			// ���U���g�̐��l�\���̍ő吔

#define VAL_MAX_KILL	(999)		// �������̍ő�l
#define VAL_KILL_DIGIT	(3)			// �������̌���
#define VAL_MAX_WEED	(999)		// �����萔�̍ő�l
#define VAL_WEED_DIGIT	(3)			// �����萔�̌���

#define KILLENEMY_MUL	(150)		// �G�̓������̃X�R�A���Z���̔{��
#define KILLWEED_MUL	(200)		// �G���̊����萔�̃X�R�A���Z���̔{��

#define RESULT_POS_X	(365.0f)	// ���U���g�̐�΍��W (x)
#define RESULT_POS_Y	(229.5f)	// ���U���g�̐�΍��W (y)
#define RESULT_WIDTH	(340.0f)	// ���U���g�̉��� / 2
#define RESULT_HEIGHT	(200.0f)	// ���U���g�̏c�� / 2

#define VAL_RES_WIDTH	(42.0f)		// ���U���g�̐��l�̉��� / 2
#define VAL_RES_HEIGHT	(42.0f)		// ���U���g�̐��l�̏c�� / 2
#define VAL_RES_SPACE	(66.0f)		// ���U���g�̐��l�Ԃ̕� (x)

#define RES_KILL_POS_X	(528.0f)	// �������̐�΍��W (x)
#define RES_KILL_POS_Y	(212.0f)	// �������̐�΍��W (y)
#define RES_WEED_POS_X	(528.0f)	// �����萔�̐�΍��W (x)
#define RES_WEED_POS_Y	(292.0f)	// �����萔�̐�΍��W (y)
#define RES_SCO_POS_X	(660.0f)	// �X�R�A�̐�΍��W (x)
#define RES_SCO_POS_Y	(372.0f)	// �X�R�A�̐�΍��W (y)

//**********************************************************************************************************************
//	�R���X�g��`
//**********************************************************************************************************************
const char *apTextureResult[] =		// �e�N�X�`���̑��΃p�X
{
	"02_data\\02_TEXTURE\\result000.png",	// ���U���g�w�i�̃e�N�X�`�����΃p�X
};

//**********************************************************************************************************************
//	�񋓌^��` (TEXTURE_RESULT)
//**********************************************************************************************************************
typedef enum
{
	TEXTURE_RESULT_BG,				// ���U���g�̔w�i
	TEXTURE_RESULT_MAX,				// ���̗񋓌^�̑���
} TEXTURE_RESULT;

//**********************************************************************************************************************
//	�O���[�o���ϐ�
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9      g_apTextureResult[TEXTURE_RESULT_MAX] = {};	// �e�N�X�`���ւ̃|�C���^
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffResult = NULL;					// ���_�o�b�t�@�ւ̃|�C���^

int g_nTotalScore;		// �g�[�^���X�R�A

//======================================================================================================================
//	���U���g�̏���������
//======================================================================================================================
void InitResult(void)
{
	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^
	VERTEX_2D        *pVtx;						// ���_���ւ̃|�C���^

	for (int nCntResult = 0; nCntResult < TEXTURE_RESULT_MAX; nCntResult++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		// �e�N�X�`���̓ǂݍ���
		D3DXCreateTextureFromFile(pDevice, apTextureResult[nCntResult], &g_apTextureResult[nCntResult]);
	}

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer
	( // ����
		sizeof(VERTEX_2D) * 4 * MAX_RESULT,		// �K�v���_��
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,							// ���_�t�H�[�}�b�g
		D3DPOOL_MANAGED,
		&g_pVtxBuffResult,
		NULL
	);

	// �O���[�o���ϐ���������
	g_nTotalScore = 0;			// �g�[�^���X�R�A

	//------------------------------------------------------------------------------------------------------------------
	//	���_���̏�����
	//------------------------------------------------------------------------------------------------------------------
	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffResult->Lock(0, 0, (void**)&pVtx, 0);

	// ���_���W��ݒ�
	pVtx[0].pos = D3DXVECTOR3(RESULT_POS_X - RESULT_WIDTH, RESULT_POS_Y - RESULT_HEIGHT, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(RESULT_POS_X + RESULT_WIDTH, RESULT_POS_Y - RESULT_HEIGHT, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(RESULT_POS_X - RESULT_WIDTH, RESULT_POS_Y + RESULT_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(RESULT_POS_X + RESULT_WIDTH, RESULT_POS_Y + RESULT_HEIGHT, 0.0f);

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

	// ���_�o�b�t�@���A�����b�N����
	g_pVtxBuffResult->Unlock();
}

//======================================================================================================================
//	���U���g�̏I������
//======================================================================================================================
void UninitResult(void)
{
	// �e�N�X�`���̔j��
	for (int nCntResult = 0; nCntResult < TEXTURE_RESULT_MAX; nCntResult++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		if (g_apTextureResult[nCntResult] != NULL)
		{ // �ϐ� (g_apTextureResult) ��NULL�ł͂Ȃ��ꍇ

			g_apTextureResult[nCntResult]->Release();
			g_apTextureResult[nCntResult] = NULL;
		}
	}

	// ���_�o�b�t�@�̔j��
	if (g_pVtxBuffResult != NULL)
	{ // �ϐ� (g_pVtxBuffResult) ��NULL�ł͂Ȃ��ꍇ

		g_pVtxBuffResult->Release();
		g_pVtxBuffResult = NULL;
	}
}

//======================================================================================================================
//	���U���g�̍X�V����
//======================================================================================================================
void UpdateResult(float fAlpha)
{
	// �|�C���^��錾
	VERTEX_2D *pVtx;	// ���_���ւ̃|�C���^

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffResult->Lock(0, 0, (void**)&pVtx, 0);

	// ���_�J���[�̐ݒ�
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);

	// ���_�o�b�t�@���A�����b�N����
	g_pVtxBuffResult->Unlock();

}

//======================================================================================================================
//	���U���g�̕`�揈��
//======================================================================================================================
void DrawResult(float fAlpha)
{
	// �ϐ��z���錾
	D3DXVECTOR3 aPosValue[MAX_VAL_RES] =					// ���U���g�\���̈ʒu�p
	{ // �����l
		D3DXVECTOR3(RES_KILL_POS_X, RES_KILL_POS_Y, 0.0f),	// �������̈ʒu
		D3DXVECTOR3(RES_WEED_POS_X, RES_WEED_POS_Y, 0.0f),	// �������̈ʒu
		D3DXVECTOR3(RES_SCO_POS_X,  RES_SCO_POS_Y,  0.0f)	// �X�R�A�̈ʒu
	};

	int aValue[MAX_VAL_RES]      = { GetKillEnemy(), GetLeaveWeed(), GetTotalScore() };	// ���U���g�\���̒l�p
	int aMaxValue[MAX_VAL_RES]   = { VAL_MAX_KILL,   VAL_MAX_WEED,   VAL_MAX_SCORE   };	// ���U���g�\���̒l�̐����p
	int aDigitValue[MAX_VAL_RES] = { VAL_KILL_DIGIT, VAL_WEED_DIGIT, VAL_SCO_DIGIT   };	// ���U���g�\���̌����p

	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, g_pVtxBuffResult, 0, sizeof(VERTEX_2D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	//------------------------------------------------------------------------------------------------------------------
	//	�w�i�̕`��
	//------------------------------------------------------------------------------------------------------------------
	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, g_apTextureResult[TEXTURE_RESULT_BG]);

	// �|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//------------------------------------------------------------------------------------------------------------------
	//	���l�̕`��
	//------------------------------------------------------------------------------------------------------------------
	for (int nCntResult = 0; nCntResult < MAX_VAL_RES; nCntResult++)
	{ // ���U���g�̐��l�\���̍ő吔���J��Ԃ�

		// ���l�̐ݒ�
		SetValue
		( // ����
			aPosValue[nCntResult],	// �ʒu
			aValue[nCntResult],		// �l
			aMaxValue[nCntResult],	// �l�̍ő�l
			VAL_RES_WIDTH,			// ����
			VAL_RES_HEIGHT,			// �c��
			VAL_RES_SPACE,			// ���l�Ԃ̕�
			fAlpha					// ���l
		);

		// ���l�̕`��
		DrawValue(aDigitValue[nCntResult], VALUETYPE_BORDER);
	}
}

//======================================================================================================================
//	�g�[�^���X�R�A�̐ݒ菈��
//======================================================================================================================
void SetTotalScore(void)
{
	// �ϐ���錾
	int nKillEnemy = GetKillEnemy();	// �G�̓�����
	int nKillWeed  = GetLeaveWeed();	// �G���̗]�萔
	int nScore     = GetScore();		// �ŏI�X�R�A

	// �g�[�^���X�R�A�����߂�
	g_nTotalScore = nScore + (nKillEnemy * KILLENEMY_MUL) - (nKillWeed * KILLWEED_MUL);

	// �g�[�^���X�R�A�̕␳
	if (g_nTotalScore < 0)
	{ // �g�[�^���X�R�A�� 0��菬�����ꍇ

		// �g�[�^���X�R�A�� 0�ɕ␳
		g_nTotalScore = 0;
	}
	else if (g_nTotalScore > VAL_MAX_SCORE)
	{ // �g�[�^���X�R�A���ő�X�R�A���傫���ꍇ

		// �g�[�^���X�R�A���ő�X�R�A�ɕ␳
		g_nTotalScore = VAL_MAX_SCORE;
	}
}

//======================================================================================================================
//	�g�[�^���X�R�A�̎擾����
//======================================================================================================================
int GetTotalScore(void)
{
	// �g�[�^���X�R�A�̒l��Ԃ�
	return g_nTotalScore;
}