//======================================================================================================================
//
//	�������� [explosion.cpp]
//	Author�F���c�E��
//
//======================================================================================================================
//**********************************************************************************************************************
//	�C���N���[�h�t�@�C��
//**********************************************************************************************************************
#include "explosion.h"
#include "sound.h"

//**********************************************************************************************************************
//	�}�N����`
//**********************************************************************************************************************
#define MAX_EXPLOSION	(64)	// �g�p����|���S���� (�����̍ő吔)

#define EXP_WIDTH		(45)	// �����̉��� / 2
#define EXP_HEIGHT		(45)	// �����̏c�� / 2
#define ANIM_TIME		(6)		// �����̃A�j���[�V�����ҋ@����
#define ANIM_MAX		(7)		// �����̍ő�A�j���[�V����

//**********************************************************************************************************************
//	�R���X�g��`
//**********************************************************************************************************************
const char *apTextureExplosion[] =		// �e�N�X�`���̑��΃p�X
{
	"02_data\\02_TEXTURE\\explosion000.png",	// �����̃e�N�X�`�����΃p�X
};

//**********************************************************************************************************************
//	�񋓌^��` (TEXTURE_EXPLOSION)
//**********************************************************************************************************************
typedef enum
{
	TEXTURE_EXPLOSION_NORMAL = 0,		// ���� (�ʏ�)
	TEXTURE_EXPLOSION_MAX,				// ���̗񋓌^�̑���
} TEXTURE_EXPLOSION;

//**********************************************************************************************************************
//	�\���̒�` (Explosion)
//**********************************************************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;					// �ʒu
	D3DXMATRIX  mtxWorld;				// ���[���h�}�g���b�N�X
	int         nCounterAnim;			// �A�j���[�V�����J�E���^�[
	int         nPatternAnim;			// �A�j���[�V�����p�^�[��
	bool        bUse;					// �g�p��
}Explosion;

//**********************************************************************************************************************
//	�O���[�o���ϐ�
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9		g_apTextureExplosion[TEXTURE_EXPLOSION_MAX] = {};	// �e�N�X�`���ւ̃|�C���^
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffExplosion = NULL;							// ���_�o�b�t�@�ւ̃|�C���^

Explosion g_aExplosion[MAX_EXPLOSION];	// �����̏��

//======================================================================================================================
//	�����̏���������
//======================================================================================================================
void InitExplosion(void)
{
	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^
	VERTEX_3D *pVtx;							// ���_���ւ̃|�C���^

	// �e�N�X�`���̓ǂݍ���
	for (int nCntExplosion = 0; nCntExplosion < TEXTURE_EXPLOSION_MAX; nCntExplosion++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		D3DXCreateTextureFromFile(pDevice, apTextureExplosion[nCntExplosion], &g_apTextureExplosion[nCntExplosion]);
	}

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer
	( // ����
		sizeof(VERTEX_3D) * 4 * MAX_EXPLOSION,	// �K�v���_��
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,							// ���_�t�H�[�}�b�g
		D3DPOOL_MANAGED,
		&g_pVtxBuffExplosion,
		NULL
	);

	// �����̏��̏�����
	for (int nCntExplosion = 0; nCntExplosion < MAX_EXPLOSION; nCntExplosion++)
	{ // �����̍ő�\�������J��Ԃ�

		g_aExplosion[nCntExplosion].pos          = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �ʒu
		g_aExplosion[nCntExplosion].nCounterAnim = 0;								// �A�j���[�V�����J�E���^�[
		g_aExplosion[nCntExplosion].nPatternAnim = 0;								// �A�j���[�V�����p�^�[��
		g_aExplosion[nCntExplosion].bUse         = false;							// �g�p��
	}

	//------------------------------------------------------------------------------------------------------------------
	//	���_���̏�����
	//------------------------------------------------------------------------------------------------------------------
	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffExplosion->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntExplosion = 0; nCntExplosion < MAX_EXPLOSION; nCntExplosion++)
	{ // �����̍ő�\�������J��Ԃ�

		// ���_���W�̐ݒ�
		pVtx[0].pos = D3DXVECTOR3(-EXP_WIDTH,  EXP_HEIGHT, 0.0f);
		pVtx[1].pos = D3DXVECTOR3( EXP_WIDTH,  EXP_HEIGHT, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-EXP_WIDTH, -EXP_HEIGHT, 0.0f);
		pVtx[3].pos = D3DXVECTOR3( EXP_WIDTH, -EXP_HEIGHT, 0.0f);

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
		pVtx[0].tex = D3DXVECTOR2(0.0f,   0.0f);
		pVtx[1].tex = D3DXVECTOR2(0.125f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f,   1.0f);
		pVtx[3].tex = D3DXVECTOR2(0.125f, 1.0f);

		// ���_�f�[�^�̃|�C���^�� 4���i�߂�
		pVtx += 4;
	}

	// ���_�o�b�t�@���A�����b�N����
	g_pVtxBuffExplosion->Unlock();
}

//======================================================================================================================
//	�����̏I������
//======================================================================================================================
void UninitExplosion(void)
{
	// �e�N�X�`���̔j��
	for (int nCntExplosion = 0; nCntExplosion < TEXTURE_EXPLOSION_MAX; nCntExplosion++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		if (g_apTextureExplosion[nCntExplosion] != NULL)
		{ // �ϐ� (g_apTextureExplosion) ��NULL�ł͂Ȃ��ꍇ

			g_apTextureExplosion[nCntExplosion]->Release();
			g_apTextureExplosion[nCntExplosion] = NULL;
		}
	}

	// ���_�o�b�t�@�̔j��
	if (g_pVtxBuffExplosion != NULL)
	{ // �ϐ� (g_pVtxBuffExplosion) ��NULL�ł͂Ȃ��ꍇ

		g_pVtxBuffExplosion->Release();
		g_pVtxBuffExplosion = NULL;
	}
}

//======================================================================================================================
//	�����̍X�V����
//======================================================================================================================
void UpdateExplosion(void)
{
	// �|�C���^��錾
	VERTEX_3D *pVtx;	// ���_���ւ̃|�C���^

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffExplosion->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntExplosion = 0; nCntExplosion < MAX_EXPLOSION; nCntExplosion++)
	{ // �����̍ő�\�������J��Ԃ�

		if (g_aExplosion[nCntExplosion].bUse == true)
		{ // �������g�p����Ă���ꍇ

			// �J�E���^�[�����Z
			g_aExplosion[nCntExplosion].nCounterAnim++;

			if ((g_aExplosion[nCntExplosion].nCounterAnim % ANIM_TIME) == 0)
			{ // �J�E���^�[���p�^�[����؂�ւ���^�C�~���O�ɂȂ����ꍇ

				// �A�j���[�V�����̃p�^�[�������Z
				g_aExplosion[nCntExplosion].nPatternAnim++;
			}

			if (g_aExplosion[nCntExplosion].nPatternAnim >= ANIM_MAX)
			{ // �A�j���[�V�����̃p�^�[�����p�^�[���̑����ȏ�̏ꍇ

				// �g�p���Ă��Ȃ���Ԃɂ���
				g_aExplosion[nCntExplosion].bUse = false;
			}

			// �e�N�X�`�����W�̐ݒ�
			pVtx[0].tex = D3DXVECTOR2( g_aExplosion[nCntExplosion].nPatternAnim      * 0.125f, 0.0f);
			pVtx[1].tex = D3DXVECTOR2((g_aExplosion[nCntExplosion].nPatternAnim + 1) * 0.125f, 0.0f);
			pVtx[2].tex = D3DXVECTOR2( g_aExplosion[nCntExplosion].nPatternAnim      * 0.125f, 1.0f);
			pVtx[3].tex = D3DXVECTOR2((g_aExplosion[nCntExplosion].nPatternAnim + 1) * 0.125f, 1.0f);
		}

		// ���_�f�[�^�̃|�C���^�� 4���i�߂�
		pVtx += 4;
	}

	// ���_�o�b�t�@���A�����b�N����
	g_pVtxBuffExplosion->Unlock();
}

//======================================================================================================================
//	�����̕`�揈��
//======================================================================================================================
void DrawExplosion(void)
{
		// �ϐ���錾
	D3DXMATRIX mtxTrans;	// �v�Z�p�}�g���b�N�X
	D3DXMATRIX mtxView;		// �r���[�}�g���b�N�X�擾�p

	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^

	// Z�e�X�g�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);		// Z�e�X�g�̐ݒ�
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);			// Z�o�b�t�@�X�V�̗L�� / �����̐ݒ�

	// ���C�e�B���O�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// ���e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// ���e�X�g�̗L�� / �����̐ݒ�
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// ���e�X�g�̐ݒ�
	pDevice->SetRenderState(D3DRS_ALPHAREF, 160);				// ���e�X�g�̎Q�ƒl�ݒ�

	for (int nCntExplosion = 0; nCntExplosion < MAX_EXPLOSION; nCntExplosion++)
	{ // �����̍ő�\�������J��Ԃ�

		if (g_aExplosion[nCntExplosion].bUse == true)
		{ // �������g�p����Ă���ꍇ

			// ���[���h�}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&g_aExplosion[nCntExplosion].mtxWorld);

			// �r���[�}�g���b�N�X���擾
			pDevice->GetTransform(D3DTS_VIEW, &mtxView);

			// �|���S�����J�����ɑ΂��Đ��ʂɌ�����
			D3DXMatrixInverse(&g_aExplosion[nCntExplosion].mtxWorld, NULL, &mtxView);	// �t�s������߂�
			g_aExplosion[nCntExplosion].mtxWorld._41 = 0.0f;
			g_aExplosion[nCntExplosion].mtxWorld._42 = 0.0f;
			g_aExplosion[nCntExplosion].mtxWorld._43 = 0.0f;

			// �ʒu�𔽉f
			D3DXMatrixTranslation(&mtxTrans, g_aExplosion[nCntExplosion].pos.x, g_aExplosion[nCntExplosion].pos.y, g_aExplosion[nCntExplosion].pos.z);
			D3DXMatrixMultiply(&g_aExplosion[nCntExplosion].mtxWorld, &g_aExplosion[nCntExplosion].mtxWorld, &mtxTrans);

			// ���[���h�}�g���b�N�X�̐ݒ�
			pDevice->SetTransform(D3DTS_WORLD, &g_aExplosion[nCntExplosion].mtxWorld);

			// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
			pDevice->SetStreamSource(0, g_pVtxBuffExplosion, 0, sizeof(VERTEX_3D));

			// ���_�t�H�[�}�b�g�̐ݒ�
			pDevice->SetFVF(FVF_VERTEX_3D);

			// �e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, g_apTextureExplosion[TEXTURE_EXPLOSION_NORMAL]);

			// �|���S���̕`��
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntExplosion * 4, 2);
		}
	}

	// Z�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);		// Z�e�X�g�̐ݒ�
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);			// Z�o�b�t�@�X�V�̗L�� / �����̐ݒ�

	// ���C�e�B���O��L���ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// ���e�X�g�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);		// ���e�X�g�̗L�� / �����̐ݒ�
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// ���e�X�g�̐ݒ�
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);					// ���e�X�g�̎Q�ƒl�ݒ�
}

//======================================================================================================================
//	�����̐ݒ菈��
//======================================================================================================================
void SetExplosion(D3DXVECTOR3 pos, SOUNDTYPE sound)
{
	// �|�C���^��錾
	VERTEX_3D *pVtx;	// ���_���ւ̃|�C���^

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	g_pVtxBuffExplosion->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntExplosion = 0; nCntExplosion < MAX_EXPLOSION; nCntExplosion++)
	{ // �����̍ő�\�������J��Ԃ�

		if (g_aExplosion[nCntExplosion].bUse == false)
		{ // �������g�p����Ă��Ȃ��ꍇ

			// �����̈ʒu����
			g_aExplosion[nCntExplosion].pos = pos;

			// �����̍\���̗̂v�f��������
			g_aExplosion[nCntExplosion].nCounterAnim = 0;
			g_aExplosion[nCntExplosion].nPatternAnim = 0;

			// �e�N�X�`�����W�̐ݒ�
			pVtx[0].tex = D3DXVECTOR2(0.0f,   0.0f);
			pVtx[1].tex = D3DXVECTOR2(0.125f, 0.0f);
			pVtx[2].tex = D3DXVECTOR2(0.0f,   1.0f);
			pVtx[3].tex = D3DXVECTOR2(0.125f, 1.0f);

			// �g�p���Ă����Ԃɂ���
			g_aExplosion[nCntExplosion].bUse = true;

			switch (sound)
			{ // ���ʉ��̎�ނ��Ƃ̏���
			case SOUNDTYPE_NONE:		// ���ʉ�����

				// ����

				// �����𔲂���
				break;

			case SOUNDTYPE_EXPLOSION:	// ������

				// �T�E���h�̍Đ�
				PlaySound(SOUND_LABEL_SE_DEATH);	// SE (���S)

				// �����𔲂���
				break;

			case SOUNDTYPE_BREAK:		// �j��

				
				// �T�E���h�̍Đ�
				PlaySound(SOUND_LABEL_SE_BREAK);	// SE (�j��)

				// �����𔲂���
				break;
			}

			// �����𔲂���
			break;
		}

		// ���_�f�[�^�̃|�C���^�� 4���i�߂�
		pVtx += 4;
	}

	// ���_�o�b�t�@���A�����b�N����
	g_pVtxBuffExplosion->Unlock();
}

#ifdef _DEBUG	// �f�o�b�O����
//======================================================================================================================
//	�f�o�b�O�����ꗗ
//======================================================================================================================
//**********************************************************************************************************************
//	�����̑����擾����
//**********************************************************************************************************************
int GetNumExplosion(void)
{
	// �ϐ���錾
	int nNumExplosion = 0;	// �����̑����̊m�F�p

	for (int nCntExplosion = 0; nCntExplosion < MAX_EXPLOSION; nCntExplosion++)
	{ // �����̍ő�\�������J��Ԃ�

		if (g_aExplosion[nCntExplosion].bUse == true)
		{ // �������g�p����Ă���ꍇ

			// �J�E���^�[�����Z
			nNumExplosion++;
		}
	}

	// �ϐ��̒l��Ԃ�
	return nNumExplosion;	// �����̑���
}
#endif