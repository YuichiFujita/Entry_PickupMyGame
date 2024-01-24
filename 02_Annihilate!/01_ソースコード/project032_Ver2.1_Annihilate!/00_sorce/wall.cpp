//======================================================================================================================
//
//	�Ǐ��� [wall.cpp]
//	Author�F���c�E��
//
//======================================================================================================================
//**********************************************************************************************************************
//	�C���N���[�h�t�@�C��
//**********************************************************************************************************************
#include "main.h"
#include "wall.h"

//**********************************************************************************************************************
//	�}�N����`
//**********************************************************************************************************************
#define MAX_WALL		(4)			// �g�p����|���S���� (�ǂ̍ő吔)

//**********************************************************************************************************************
//	�R���X�g��`
//**********************************************************************************************************************
const char *apTextureWall[] =		// �e�N�X�`���̑��΃p�X
{
	"02_data\\02_TEXTURE\\pillar000.jpg",	// �ǂ̃e�N�X�`���̑��΃p�X
};

//**********************************************************************************************************************
//	�񋓌^��` (TEXTURE_WALL)
//**********************************************************************************************************************
typedef enum
{
	TEXTURE_WALL_NORMAL = 0,		// �� (�ʏ�)
	TEXTURE_WALL_MAX,				// ���̗񋓌^�̑���
} TEXTURE_WALL;

//**********************************************************************************************************************
//	�\���̒�` (Wall)
//**********************************************************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;				// �ʒu
	D3DXVECTOR3 rot;				// ����
	D3DXMATRIX  mtxWorld;			// ���[���h�}�g���b�N�X
	bool        bUse;				// �g�p��
} Wall;

//**********************************************************************************************************************
//	�O���[�o���ϐ�
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9      g_apTextureWall[TEXTURE_WALL_MAX] = {};	// �e�N�X�`���ւ̃|�C���^
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffWall = NULL;					// ���_�o�b�t�@�ւ̃|�C���^

Wall g_aWall[MAX_WALL];				// �ǂ̏��

//======================================================================================================================
//	�ǂ̏���������
//======================================================================================================================
void InitWall(void)
{
	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^
	VERTEX_3D *pVtx;							// ���_���ւ̃|�C���^

	// �e�N�X�`���̓ǂݍ���
	for (int nCntWall = 0; nCntWall < TEXTURE_WALL_MAX; nCntWall++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		D3DXCreateTextureFromFile(pDevice, apTextureWall[nCntWall], &g_apTextureWall[nCntWall]);
	}

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer
	( // ����
		sizeof(VERTEX_3D) * 4 * MAX_WALL,		// �K�v���_��
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,							// ���_�t�H�[�}�b�g
		D3DPOOL_MANAGED,
		&g_pVtxBuffWall,
		NULL
	);

	// �ǂ̏��̏�����
	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{ // �ǂ̍ő�\�������J��Ԃ�

		g_aWall[nCntWall].pos  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// �ʒu
		g_aWall[nCntWall].rot  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// ����
		g_aWall[nCntWall].bUse = false;								// �g�p��
	}

	//------------------------------------------------------------------------------------------------------------------
	//	���_���̏�����
	//------------------------------------------------------------------------------------------------------------------
	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffWall->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{ // �ǂ̍ő�\�������J��Ԃ�

		// ���_���W�̐ݒ�
		pVtx[0].pos = D3DXVECTOR3(-400.0f, 200.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3( 400.0f, 200.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-400.0f, 0.0f,   0.0f);
		pVtx[3].pos = D3DXVECTOR3( 400.0f, 0.0f,   0.0f);

		// �@���x�N�g���̐ݒ�
		pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

		// ���_�J���[�̐ݒ�
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		// ���_�f�[�^�̃|�C���^�� 4���i�߂�
		pVtx += 4;
	}

	// ���_�o�b�t�@���A�����b�N����
	g_pVtxBuffWall->Unlock();
}

//======================================================================================================================
//	�ǂ̏I������
//======================================================================================================================
void UninitWall(void)
{
	// �e�N�X�`���̔j��
	for (int nCntWall = 0; nCntWall < TEXTURE_WALL_MAX; nCntWall++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		if (g_apTextureWall[nCntWall] != NULL)
		{ // �ϐ� (g_apTextureWall) ��NULL�ł͂Ȃ��ꍇ

			g_apTextureWall[nCntWall]->Release();
			g_apTextureWall[nCntWall] = NULL;
		}
	}

	// ���_�o�b�t�@�̔j��
	if (g_pVtxBuffWall != NULL)
	{ // �ϐ� (g_pVtxBuffWall) ��NULL�ł͂Ȃ��ꍇ

		g_pVtxBuffWall->Release();
		g_pVtxBuffWall = NULL;
	}
}

//======================================================================================================================
//	�ǂ̍X�V����
//======================================================================================================================
void UpdateWall(void)
{

}

//======================================================================================================================
//	�ǂ̕`�揈��
//======================================================================================================================
void DrawWall(void)
{
	// �ϐ���錾
	D3DXMATRIX mtxRot, mtxTrans;				// �v�Z�p�}�g���b�N�X

	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^

	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{ // �ǂ̍ő�\�������J��Ԃ�

		if (g_aWall[nCntWall].bUse == true)
		{ // �ǂ��g�p����Ă���ꍇ

			// ���[���h�}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&g_aWall[nCntWall].mtxWorld);

			// �����𔽉f
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aWall[nCntWall].rot.y, g_aWall[nCntWall].rot.x, g_aWall[nCntWall].rot.z);
			D3DXMatrixMultiply(&g_aWall[nCntWall].mtxWorld, &g_aWall[nCntWall].mtxWorld, &mtxRot);

			// �ʒu�𔽉f
			D3DXMatrixTranslation(&mtxTrans, g_aWall[nCntWall].pos.x, g_aWall[nCntWall].pos.y, g_aWall[nCntWall].pos.z);
			D3DXMatrixMultiply(&g_aWall[nCntWall].mtxWorld, &g_aWall[nCntWall].mtxWorld, &mtxTrans);

			// ���[���h�}�g���b�N�X�̐ݒ�
			pDevice->SetTransform(D3DTS_WORLD, &g_aWall[nCntWall].mtxWorld);

			// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
			pDevice->SetStreamSource(0, g_pVtxBuffWall, 0, sizeof(VERTEX_3D));

			// ���_�t�H�[�}�b�g�̐ݒ�
			pDevice->SetFVF(FVF_VERTEX_3D);

			// �e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, g_apTextureWall[TEXTURE_WALL_NORMAL]);

			// �|���S���̕`��
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntWall * 4, 2);
		}
	}
}

//======================================================================================================================
//	�ǂ̐ݒ菈��
//======================================================================================================================
void SetWall(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{ // �ǂ̍ő�\�������J��Ԃ�

		if (g_aWall[nCntWall].bUse == false)
		{ // �ǂ��g�p����Ă��Ȃ��ꍇ

			// �����̈ʒu����
			g_aWall[nCntWall].pos = pos;

			// �����̌�������
			g_aWall[nCntWall].rot = rot;

			// �g�p���Ă����Ԃɂ���
			g_aWall[nCntWall].bUse = true;

			// �����𔲂���
			break;
		}
	}
}