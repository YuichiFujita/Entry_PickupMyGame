//======================================================================================================================
//
//	������ [field.cpp]
//	Author�F���c�E��
//
//======================================================================================================================
//**********************************************************************************************************************
//	�C���N���[�h�t�@�C��
//**********************************************************************************************************************
#include "main.h"
#include "field.h"

//**********************************************************************************************************************
//	�}�N����`
//**********************************************************************************************************************
#define MAX_FIELD	(4)	// �g�p����|���S���� (���̍ő吔)

//**********************************************************************************************************************
//	�R���X�g��`
//**********************************************************************************************************************
const char *apTextureField[] =		// �e�N�X�`���̑��΃p�X
{
	"02_data\\02_TEXTURE\\field000.jpg",	// ���̃e�N�X�`���̑��΃p�X
};

//**********************************************************************************************************************
//	�񋓌^��` (TEXTURE_FIELD)
//**********************************************************************************************************************
typedef enum
{
	TEXTURE_FIELD_NORMAL = 0,		// �� (�ʏ�)
	TEXTURE_FIELD_MAX,				// ���̗񋓌^�̑���
} TEXTURE_FIELD;

//**********************************************************************************************************************
//	�\���̒�` (Field)
//**********************************************************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;				// �ʒu
	D3DXVECTOR3 rot;				// ����
	D3DXMATRIX  mtxWorld;			// ���[���h�}�g���b�N�X
	bool        bUse;				// �g�p��
} Field;

//**********************************************************************************************************************
//	�O���[�o���ϐ�
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9      g_apTextureField[TEXTURE_FIELD_MAX] = {};	// �e�N�X�`���ւ̃|�C���^
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffField = NULL;						// ���_�o�b�t�@�ւ̃|�C���^

Field g_aField[MAX_FIELD];			// ���̏��

//======================================================================================================================
//	���̏���������
//======================================================================================================================
void InitField(void)
{
	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^
	VERTEX_3D *pVtx;							// ���_���ւ̃|�C���^

	// �e�N�X�`���̓ǂݍ���
	for (int nCntField = 0; nCntField < TEXTURE_FIELD_MAX; nCntField++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		D3DXCreateTextureFromFile(pDevice, apTextureField[nCntField], &g_apTextureField[nCntField]);
	}

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer
	( // ����
		sizeof(VERTEX_3D) * 4 * MAX_FIELD,		// �K�v���_��
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,							// ���_�t�H�[�}�b�g
		D3DPOOL_MANAGED,
		&g_pVtxBuffField,
		NULL
	);

	// ���̏��̏�����
	for (int nCntField = 0; nCntField < MAX_FIELD; nCntField++)
	{ // ���̍ő�\�������J��Ԃ�

		g_aField[nCntField].pos  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �ʒu
		g_aField[nCntField].rot  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ����
		g_aField[nCntField].bUse = false;							// �g�p��
	}

	//------------------------------------------------------------------------------------------------------------------
	//	���_���̏�����
	//------------------------------------------------------------------------------------------------------------------
	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffField->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntField = 0; nCntField < MAX_FIELD; nCntField++)
	{ // ���̍ő�\�������J��Ԃ�

		// ���_���W�̐ݒ�
		pVtx[0].pos = D3DXVECTOR3(-200.0f, 0.0f,  200.0f);
		pVtx[1].pos = D3DXVECTOR3( 200.0f, 0.0f,  200.0f);
		pVtx[2].pos = D3DXVECTOR3(-200.0f, 0.0f, -200.0f);
		pVtx[3].pos = D3DXVECTOR3( 200.0f, 0.0f, -200.0f);

		// �@���x�N�g���̐ݒ�
		pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

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
	g_pVtxBuffField->Unlock();
}

//======================================================================================================================
//	���̏I������
//======================================================================================================================
void UninitField(void)
{
	// �e�N�X�`���̔j��
	for (int nCntField = 0; nCntField < TEXTURE_FIELD_MAX; nCntField++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		if (g_apTextureField[nCntField] != NULL)
		{ // �ϐ� (g_apTextureField) ��NULL�ł͂Ȃ��ꍇ

			g_apTextureField[nCntField]->Release();
			g_apTextureField[nCntField] = NULL;
		}
	}

	// ���_�o�b�t�@�̔j��
	if (g_pVtxBuffField != NULL)
	{ // �ϐ� (g_pVtxBuffField) ��NULL�ł͂Ȃ��ꍇ

		g_pVtxBuffField->Release();
		g_pVtxBuffField = NULL;
	}
}

//======================================================================================================================
//	���̍X�V����
//======================================================================================================================
void UpdateField(void)
{

}

//======================================================================================================================
//	���̕`�揈��
//======================================================================================================================
void DrawField(void)
{
	// �ϐ���錾
	D3DXMATRIX mtxRot, mtxTrans;				// �v�Z�p�}�g���b�N�X

	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^

	for (int nCntField = 0; nCntField < MAX_FIELD; nCntField++)
	{ // ���̍ő�\�������J��Ԃ�

		if (g_aField[nCntField].bUse == true)
		{ // �����g�p����Ă���ꍇ

			// ���[���h�}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&g_aField[nCntField].mtxWorld);

			// �����𔽉f
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aField[nCntField].rot.y, g_aField[nCntField].rot.x, g_aField[nCntField].rot.z);
			D3DXMatrixMultiply(&g_aField[nCntField].mtxWorld, &g_aField[nCntField].mtxWorld, &mtxRot);

			// �ʒu�𔽉f
			D3DXMatrixTranslation(&mtxTrans, g_aField[nCntField].pos.x, g_aField[nCntField].pos.y, g_aField[nCntField].pos.z);
			D3DXMatrixMultiply(&g_aField[nCntField].mtxWorld, &g_aField[nCntField].mtxWorld, &mtxTrans);

			// ���[���h�}�g���b�N�X�̐ݒ�
			pDevice->SetTransform(D3DTS_WORLD, &g_aField[nCntField].mtxWorld);

			// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
			pDevice->SetStreamSource(0, g_pVtxBuffField, 0, sizeof(VERTEX_3D));

			// ���_�t�H�[�}�b�g�̐ݒ�
			pDevice->SetFVF(FVF_VERTEX_3D);

			// �e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, g_apTextureField[TEXTURE_FIELD_NORMAL]);

			// �|���S���̕`��
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntField * 4, 2);
		}
	}
}

//======================================================================================================================
//	���̐ݒ菈��
//======================================================================================================================
void SetField(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	for (int nCntField = 0; nCntField < MAX_FIELD; nCntField++)
	{ // �ǂ̍ő�\�������J��Ԃ�

		if (g_aField[nCntField].bUse == false)
		{ // �ǂ��g�p����Ă��Ȃ��ꍇ

			// �����̈ʒu����
			g_aField[nCntField].pos = pos;

			// �����̌�������
			g_aField[nCntField].rot = rot;

			// �g�p���Ă����Ԃɂ���
			g_aField[nCntField].bUse = true;

			// �����𔲂���
			break;
		}
	}
}