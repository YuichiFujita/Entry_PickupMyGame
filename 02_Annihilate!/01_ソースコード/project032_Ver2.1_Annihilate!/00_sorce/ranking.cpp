//======================================================================================================================
//
//	�����L���O���� [ranking.cpp]
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

#include "ranking.h"
#include "result.h"
#include "score.h"
#include "value.h"

//**********************************************************************************************************************
//	�}�N����`
//**********************************************************************************************************************
#define RANK_SETUP_BIN	"02_data\\01_BIN\\ranking.bin"	// �����L���O�Z�b�g�A�b�v�p�̃o�C�i���t�@�C���̑��΃p�X

#define MAX_RANKING		(1)			// �g�p����|���S����
#define MAX_VAL_RANK	(5)			// �����L���O�̐��l�\���̍ő吔
#define NONE_RANK		(-1)		// ���ʍX�V�̏����l

#define RANKING_POS_X	(995.0f)	// �����L���O�̐�΍��W (x)
#define RANKING_POS_Y	(360.0f)	// �����L���O�̐�΍��W (y)
#define RANKING_WIDTH	(260.0f)	// �����L���O�̉��� / 2
#define RANKING_HEIGHT	(330.0f)	// �����L���O�̏c�� / 2

#define VAL_RANK_WIDTH	(42.0f)		// �����L���O�̐��l�̉��� / 2
#define VAL_RANK_HEIGHT	(42.0f)		// �����L���O�̐��l�̏c�� / 2
#define VAL_RANK_SPACE	(66.0f)		// �����L���O�̐��l�Ԃ̕� (y)

#define RANK_SCO_POS_X	(1205.0f)	// �X�R�A�̐�΍��W (x)
#define RANK_SCO_POS_Y	(212.0f)	// �X�R�A�̐�΍��W (y)
#define RANK_SCO_SPACE	(96.0f)		// �X�R�A�Ԃ̕� (y)

//**********************************************************************************************************************
//	�R���X�g��`
//**********************************************************************************************************************
const char *apTextureRanking[] =	// �e�N�X�`���̑��΃p�X
{
	"02_data\\02_TEXTURE\\ranking000.png",	// �����L���O�w�i�̃e�N�X�`�����΃p�X
};

//**********************************************************************************************************************
//	�񋓌^��` (TEXTURE_RANKING)
//**********************************************************************************************************************
typedef enum
{
	TEXTURE_RANKING_BG,				// �����L���O�̔w�i
	TEXTURE_RANKING_MAX,			// ���̗񋓌^�̑���
} TEXTURE_RANKING;

//**********************************************************************************************************************
//	�v���g�^�C�v�錾
//**********************************************************************************************************************
void WirteRanking(void);			// �����L���O�̏����o������
void ReadRanking(void);				// �����L���O�̓ǂݍ��ݏ���
void SortRanking(void);				// �����L���O�̃\�[�g����
void CurrentRanking(void);			// ����̃X�R�A�̏��ʔ���

//**********************************************************************************************************************
//	�O���[�o���ϐ�
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9      g_apTextureRanking[TEXTURE_RANKING_MAX] = {};	// �e�N�X�`���ւ̃|�C���^
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffRanking = NULL;						// ���_�o�b�t�@�ւ̃|�C���^

int g_nCurrentRankID;		// ����̃X�R�A�̏��ʂ̃C���f�b�N�X
int g_aRank[MAX_VAL_RANK];	// �����L���O�̒l

//======================================================================================================================
//	�����L���O�̏���������
//======================================================================================================================
void InitRanking(void)
{
	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^
	VERTEX_2D        *pVtx;						// ���_���ւ̃|�C���^

	for (int nCntRanking = 0; nCntRanking < TEXTURE_RANKING_MAX; nCntRanking++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		// �e�N�X�`���̓ǂݍ���
		D3DXCreateTextureFromFile(pDevice, apTextureRanking[nCntRanking], &g_apTextureRanking[nCntRanking]);
	}

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer
	( // ����
		sizeof(VERTEX_2D) * 4 * MAX_RANKING,	// �K�v���_��
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,							// ���_�t�H�[�}�b�g
		D3DPOOL_MANAGED,
		&g_pVtxBuffRanking,
		NULL
	);

	// �O���[�o���ϐ���������
	g_nCurrentRankID = NONE_RANK;	// ����̃X�R�A�̏��ʂ̃C���f�b�N�X

	// �O���[�o���ϐ��z���������
	for (int nCntRanking = 0; nCntRanking < MAX_VAL_RANK; nCntRanking++)
	{ // �����L���O�̐��l�\���̍ő吔���J��Ԃ�

		g_aRank[nCntRanking] = 0;	// �����L���O�̒l
	}

	//------------------------------------------------------------------------------------------------------------------
	//	���_���̏�����
	//------------------------------------------------------------------------------------------------------------------
	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffRanking->Lock(0, 0, (void**)&pVtx, 0);

	// ���_���W��ݒ�
	pVtx[0].pos = D3DXVECTOR3(RANKING_POS_X - RANKING_WIDTH, RANKING_POS_Y - RANKING_HEIGHT, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(RANKING_POS_X + RANKING_WIDTH, RANKING_POS_Y - RANKING_HEIGHT, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(RANKING_POS_X - RANKING_WIDTH, RANKING_POS_Y + RANKING_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(RANKING_POS_X + RANKING_WIDTH, RANKING_POS_Y + RANKING_HEIGHT, 0.0f);

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
	g_pVtxBuffRanking->Unlock();
}

//======================================================================================================================
//	�����L���O�̏I������
//======================================================================================================================
void UninitRanking(void)
{
	// �e�N�X�`���̔j��
	for (int nCntRanking = 0; nCntRanking < TEXTURE_RANKING_MAX; nCntRanking++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		if (g_apTextureRanking[nCntRanking] != NULL)
		{ // �ϐ� (g_apTextureRanking) ��NULL�ł͂Ȃ��ꍇ

			g_apTextureRanking[nCntRanking]->Release();
			g_apTextureRanking[nCntRanking] = NULL;
		}
	}

	// ���_�o�b�t�@�̔j��
	if (g_pVtxBuffRanking != NULL)
	{ // �ϐ� (g_pVtxBuffRanking) ��NULL�ł͂Ȃ��ꍇ

		g_pVtxBuffRanking->Release();
		g_pVtxBuffRanking = NULL;
	}
}

//======================================================================================================================
//	�����L���O�̍X�V����
//======================================================================================================================
void UpdateRanking(float fAlpha)
{
	// �|�C���^��錾
	VERTEX_2D *pVtx;	// ���_���ւ̃|�C���^

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffRanking->Lock(0, 0, (void**)&pVtx, 0);

	// ���_�J���[�̐ݒ�
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);

	// ���_�o�b�t�@���A�����b�N����
	g_pVtxBuffRanking->Unlock();
}

//======================================================================================================================
//	�����L���O�̕`�揈��
//======================================================================================================================
void DrawRanking(float fAlpha)
{
	// �ϐ���錾
	D3DXVECTOR3 posValue;	// �����L���O�\���̈ʒu�p

	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, g_pVtxBuffRanking, 0, sizeof(VERTEX_2D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	//------------------------------------------------------------------------------------------------------------------
	//	�w�i�̕`��
	//------------------------------------------------------------------------------------------------------------------
	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, g_apTextureRanking[TEXTURE_RANKING_BG]);

	// �|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//------------------------------------------------------------------------------------------------------------------
	//	���l�̕`��
	//------------------------------------------------------------------------------------------------------------------
	for (int nCntRanking = 0; nCntRanking < MAX_VAL_RANK; nCntRanking++)
	{ // �����L���O�̐��l�\���̍ő吔���J��Ԃ�

		// �����L���O�\���̈ʒu��ݒ�
		posValue = D3DXVECTOR3(RANK_SCO_POS_X, RANK_SCO_POS_Y + (nCntRanking * RANK_SCO_SPACE), 0.0f);

		// ���l�̐ݒ�
		SetValue
		( // ����
			posValue,				// �ʒu
			g_aRank[nCntRanking],	// �l
			VAL_MAX_SCORE,			// �l�̍ő�l
			VAL_RANK_WIDTH,			// ����
			VAL_RANK_HEIGHT,		// �c��
			VAL_RANK_SPACE,			// ���l�Ԃ̕�
			fAlpha					// ���l
		);

		if (g_nCurrentRankID == nCntRanking)
		{ // ���ʂ��X�V����Ă���ꍇ

			// ���l�̕`��
			DrawValue(VAL_SCO_DIGIT, VALUETYPE_GREEN);
		}
		else
		{ // ���ʂ��X�V����Ă��Ȃ��ꍇ

			// ���l�̕`��
			DrawValue(VAL_SCO_DIGIT, VALUETYPE_BORDER);
		}
	}
}

//======================================================================================================================
//	�����L���O�̐ݒ菈��
//======================================================================================================================
void SetRanking(void)
{
	// �����L���O�̓ǂݍ���
	ReadRanking();

	if (GetTotalScore() >= g_aRank[MAX_VAL_RANK - 1])
	{ // ����̃X�R�A�������L���O5�ʂ̃X�R�A���傫���ꍇ

		// �����L���O�̃\�[�g
		SortRanking();

		// �����L���O�̏����o��
		WirteRanking();

		// ����̃X�R�A�̏��ʔ���
		CurrentRanking();
	}
}

//======================================================================================================================
//	�����L���O�̏����o������
//======================================================================================================================
void WirteRanking(void)
{
	// �|�C���^��錾
	FILE *pFile;	// �t�@�C���|�C���^

	// �o�C�i���t�@�C���������o�������ŊJ��
	pFile = fopen(RANK_SETUP_BIN, "wb");

	if (pFile != NULL)
	{ // �t�@�C�����J�����ꍇ

		// �t�@�C���ɐ��l�������o��
		fwrite(&g_aRank[0], sizeof(int), MAX_VAL_RANK, pFile);

		// �t�@�C�������
		fclose(pFile);
	}
	else
	{ // �t�@�C�����J���Ȃ������ꍇ

		// �G���[���b�Z�[�W�{�b�N�X
		MessageBox(NULL, "�����L���O�t�@�C���̏����o���Ɏ��s�I", "�x���I", MB_ICONWARNING);
	}
}

//======================================================================================================================
//	�����L���O�̓ǂݍ��ݏ���
//======================================================================================================================
void ReadRanking(void)
{
	// �|�C���^��錾
	FILE *pFile;	// �t�@�C���|�C���^

	// �o�C�i���t�@�C����ǂݍ��ݕ����ŊJ��
	pFile = fopen(RANK_SETUP_BIN, "rb");

	if (pFile != NULL)
	{ // �t�@�C�����J�����ꍇ

		// �t�@�C���̐��l��ǂݍ���
		fread(&g_aRank[0], sizeof(int), MAX_VAL_RANK, pFile);

		// �t�@�C�������
		fclose(pFile);
	}
	else
	{ // �t�@�C�����J���Ȃ������ꍇ

		// �G���[���b�Z�[�W�{�b�N�X
		MessageBox(NULL, "�����L���O�t�@�C���̓ǂݍ��݂Ɏ��s�I", "�x���I", MB_ICONWARNING);
	}
}

//======================================================================================================================
//	�����L���O�̃\�[�g����
//======================================================================================================================
void SortRanking(void)
{
	// �ϐ���錾
	int nNowMaxID = 0;	// �ő�l�������Ă���z��̗v�f�ԍ��̋L���p
	int nSortKeep = 0;	// �\�[�g����ւ����̑���p

	// ����̃X�R�A�������L���O5�ʂɑ��
	g_aRank[MAX_VAL_RANK - 1] = GetTotalScore();

	for (int nCntRanking = 0; nCntRanking < (MAX_VAL_RANK - 1); nCntRanking++)
	{ // �����L���O�̑��� -1�񕪌J��Ԃ�

		// ���݂̌J��Ԃ������� (�v�f1�Ƃ���)
		nNowMaxID = nCntRanking;

		for (int nCntSort = nCntRanking + 1; nCntSort < MAX_VAL_RANK; nCntSort++)
		{ // �����L���O�̐��l�\���̍ő吔���J��Ԃ�

			if (g_aRank[nNowMaxID] < g_aRank[nCntSort])
			{ // �ő�l�ɐݒ肳��Ă���l���A���݂̒l�̂ق����傫���ꍇ

				// ���݂̗v�f�ԍ����ő�l�ɐݒ�
				nNowMaxID = nCntSort;
			}
		}

		if (nCntRanking != nNowMaxID)
		{ // �ő�l�̗v�f�ԍ��ɕϓ����������ꍇ

			// �l�̓���ւ�
			nSortKeep            = g_aRank[nCntRanking];
			g_aRank[nCntRanking] = g_aRank[nNowMaxID];
			g_aRank[nNowMaxID]   = nSortKeep;
		}
	}
}

//======================================================================================================================
//	����̃X�R�A�̏��ʔ���
//======================================================================================================================
void CurrentRanking(void)
{
	for (int nCntRanking = 0; nCntRanking < MAX_VAL_RANK; nCntRanking++)
	{ // �����L���O�̐��l�\���̍ő吔���J��Ԃ�

		if (GetTotalScore() == g_aRank[nCntRanking] && g_aRank[nCntRanking] > 0)
		{ // ����̃X�R�A�Ɠ����l���A����̃X�R�A�� 0���傫���ꍇ

			// ����̃X�R�A�̏��ʂ̃C���f�b�N�X��ݒ�
			g_nCurrentRankID = nCntRanking;

			// �����𔲂���
			break;
		}
	}
}