//======================================================================================================================
//
//	�������� [tornado.cpp]
//	Author�F���c�E��
//
//======================================================================================================================
//**********************************************************************************************************************
//	�C���N���[�h�t�@�C��
//**********************************************************************************************************************
#include "main.h"
#include "tornado.h"
#include "calculation.h"

//**********************************************************************************************************************
//	�}�N����`
//**********************************************************************************************************************
#define MAX_TORNADO	(128)	// �����̍ő吔
#define MAX_OFFSET	(2)		// �I�t�Z�b�g�̐�

//**********************************************************************************************************************
//	�R���X�g��`
//**********************************************************************************************************************
const char *apTextureTornado[] =	// �e�N�X�`���̑��΃p�X
{
	"02_data\\02_TEXTURE\\orbit000.jpg",	// �����̃e�N�X�`���̑��΃p�X
};

//**********************************************************************************************************************
//	�\���̒�` (Tornado)
//**********************************************************************************************************************
typedef struct
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff;	// ���_�o�b�t�@
	D3DXVECTOR3 pos;		// �ʒu
	D3DXVECTOR3 rot;		// ����
	D3DXVECTOR3 direRot;	// �g���l�[�h�̐�������
	TORNADOTYPE type;		// ���
	D3DXMATRIX  mtxWorld;	// ���[���h�}�g���b�N�X
	D3DXMATRIX *pMtxParent;	// �e�̃}�g���b�N�X
	int   nNumVtx;		// �K�v���_��
	int   nNumAround;	// �Q�̎���
	int   nPattern;		// �Q�̕�����
	float fMoveRot;		// �����̕ύX��
	float fThickness;	// �|���S���̑���
	float fOuterPlus;	// �|���S���O���� y���W���Z��
	float fSetWidth;	// �������̉������
	float fSetAlpha;	// �������̓����x
	float fAddWidth;	// ������̉��Z��
	float fAddHeight;	// �c����̉��Z��
	float fSubAlpha;	// �����x�̌��Z��
	float fGrowWidth;	// ������̐�����
	float fGrowHeight;	// �c����̐�����
	float fGrowAlpha;	// �����x�̐�����
	bool  bUse;			// �g�p��
}Tornado;

//**********************************************************************************************************************
//	�v���g�^�C�v�錾
//**********************************************************************************************************************
void DeleteTornado(Tornado *pTornado);	// �����̍폜����
void SetVtxTornado(Tornado *pTornado);	// �����̒��_���̐ݒ菈��

//**********************************************************************************************************************
//	�O���[�o���ϐ�
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9 g_apTextureTornado[TORNADOTYPE_MAX] = {};	// �e�N�X�`���ւ̃|�C���^

Tornado g_aTornado[MAX_TORNADO];	// �����̏��

//======================================================================================================================
//	�����̏���������
//======================================================================================================================
void InitTornado(void)
{
	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^

	// �e�N�X�`���̓ǂݍ���
	for (int nCntTornado = 0; nCntTornado < TORNADOTYPE_MAX; nCntTornado++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		D3DXCreateTextureFromFile(pDevice, apTextureTornado[nCntTornado], &g_apTextureTornado[nCntTornado]);
	}

	// �����̏��̏�����
	for (int nCntTornado = 0; nCntTornado < MAX_TORNADO; nCntTornado++)
	{ // �����̍ő�\�������J��Ԃ�

		// ��{���̏�����
		g_aTornado[nCntTornado].pos		= D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �ʒu
		g_aTornado[nCntTornado].rot		= D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ����
		g_aTornado[nCntTornado].direRot	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �g���l�[�h�̐�������
		g_aTornado[nCntTornado].type	= TORNADOTYPE_NORMAL;				// ���

		g_aTornado[nCntTornado].pMtxParent	= NULL;		// �e�̃}�g���b�N�X
		g_aTornado[nCntTornado].nNumVtx		= 0;		// �K�v���_��
		g_aTornado[nCntTornado].nNumAround	= 0;		// �Q�̎���
		g_aTornado[nCntTornado].nPattern	= 0;		// �Q�̕�����
		g_aTornado[nCntTornado].fMoveRot	= 0.0f;		// �����̕ύX��
		g_aTornado[nCntTornado].fThickness	= 0.0f;		// �|���S���̑���
		g_aTornado[nCntTornado].fOuterPlus	= 0.0f;		// �|���S���O���� y���W���Z��
		g_aTornado[nCntTornado].fSetWidth	= 0.0f;		// �������̉������
		g_aTornado[nCntTornado].fSetAlpha	= 0.0f;		// �������̓����x
		g_aTornado[nCntTornado].fAddWidth	= 0.0f;		// ������̉��Z��
		g_aTornado[nCntTornado].fAddHeight	= 0.0f;		// �c����̉��Z��
		g_aTornado[nCntTornado].fSubAlpha	= 0.0f;		// �����x�̌��Z��
		g_aTornado[nCntTornado].fGrowWidth	= 0.0f;		// ������̐�����
		g_aTornado[nCntTornado].fGrowHeight	= 0.0f;		// �c����̐�����
		g_aTornado[nCntTornado].fGrowAlpha	= 0.0f;		// �����x�̐�����
		g_aTornado[nCntTornado].bUse		= false;	// �g�p��

		// ���_���̏�����
		g_aTornado[nCntTornado].pVtxBuff = NULL;
	}
}

//======================================================================================================================
//	�����̏I������
//======================================================================================================================
void UninitTornado(void)
{
	// �e�N�X�`���̔j��
	for (int nCntTornado = 0; nCntTornado < TORNADOTYPE_MAX; nCntTornado++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		if (g_apTextureTornado[nCntTornado] != NULL)
		{ // �ϐ� (g_apTextureTornado) ��NULL�ł͂Ȃ��ꍇ

			g_apTextureTornado[nCntTornado]->Release();
			g_apTextureTornado[nCntTornado] = NULL;
		}
	}

	// ���_�o�b�t�@�̔j��
	for (int nCntTornado = 0; nCntTornado < MAX_TORNADO; nCntTornado++)
	{ // �����̍ő�\�������J��Ԃ�

		if (g_aTornado[nCntTornado].pVtxBuff != NULL)
		{ // �ϐ� (g_aTornado[nCntTornado].pVtxBuff) ��NULL�ł͂Ȃ��ꍇ

			g_aTornado[nCntTornado].pVtxBuff->Release();
			g_aTornado[nCntTornado].pVtxBuff = NULL;
		}
	}
}

//======================================================================================================================
//	�����̍X�V����
//======================================================================================================================
void UpdateTornado(void)
{
	for (int nCntTornado = 0; nCntTornado < MAX_TORNADO; nCntTornado++)
	{ // �����̍ő�\�������J��Ԃ�

		if (g_aTornado[nCntTornado].bUse == true)
		{ // �������g�p����Ă���ꍇ

			// �����̌�����ύX
			g_aTornado[nCntTornado].rot.y -= g_aTornado[nCntTornado].fMoveRot;

			// �����𐳋K��
			NormalizeRot(&g_aTornado[nCntTornado].rot.y);

			// �����𐬒�������
			g_aTornado[nCntTornado].fSetWidth  += g_aTornado[nCntTornado].fGrowWidth * g_aTornado[nCntTornado].nNumVtx;	// ���_����̉�����ʂ����Z
			g_aTornado[nCntTornado].fAddWidth  += g_aTornado[nCntTornado].fGrowWidth;	// �����̉��̍L��������Z
			g_aTornado[nCntTornado].fAddHeight += g_aTornado[nCntTornado].fGrowHeight;	// �����̏c�̍L��������Z
			g_aTornado[nCntTornado].fSetAlpha  -= g_aTornado[nCntTornado].fGrowAlpha;	// �����̓����x�����Z (�����ɂ��Ă���)

			// �����̒��_���̐ݒ�
			SetVtxTornado(&g_aTornado[nCntTornado]);

			if (g_aTornado[nCntTornado].fSetAlpha <= 0.0f)
			{ // �����̓����ɂȂ����ꍇ

				// �����̍폜
				DeleteTornado(&g_aTornado[nCntTornado]);
			}
		}
	}
}

//======================================================================================================================
//	�����̕`�揈��
//======================================================================================================================
void DrawTornado(void)
{
	// �ϐ���錾
	D3DXMATRIX mtxRot, mtxTrans;	// �v�Z�p�}�g���b�N�X
	D3DXMATRIX mtxOrigin;			// �������̃}�g���b�N�X

	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^

	// ���C�e�B���O�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// �|���S���̗��ʂ�\����Ԃɂ���
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ���e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// ���e�X�g�̗L�� / �����̐ݒ�
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// ���e�X�g�̐ݒ�
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);					// ���e�X�g�̎Q�ƒl�ݒ�

	for (int nCntTornado = 0; nCntTornado < MAX_TORNADO; nCntTornado++)
	{ // �����̍ő�\�������J��Ԃ�

		if (g_aTornado[nCntTornado].bUse == true)
		{ // �������g�p����Ă���ꍇ

			//----------------------------------------------------------------------------------------------------------
			//	�������̃}�g���b�N�X�����߂�
			//----------------------------------------------------------------------------------------------------------
			// �������̃}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&mtxOrigin);

			// �����𔽉f
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aTornado[nCntTornado].direRot.y, g_aTornado[nCntTornado].direRot.x, g_aTornado[nCntTornado].direRot.z);
			D3DXMatrixMultiply(&mtxOrigin, &mtxOrigin, &mtxRot);	// ��������

			// �ʒu�𔽉f
			D3DXMatrixTranslation(&mtxTrans, g_aTornado[nCntTornado].pos.x, g_aTornado[nCntTornado].pos.y, g_aTornado[nCntTornado].pos.z);
			D3DXMatrixMultiply(&mtxOrigin, &mtxOrigin, &mtxTrans);	// �����ʒu

			//----------------------------------------------------------------------------------------------------------
			//	���[���h�}�g���b�N�X�����߂�
			//----------------------------------------------------------------------------------------------------------
			// ���[���h�}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&g_aTornado[nCntTornado].mtxWorld);

			// �����𔽉f
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aTornado[nCntTornado].rot.y, g_aTornado[nCntTornado].rot.x, g_aTornado[nCntTornado].rot.z);
			D3DXMatrixMultiply(&g_aTornado[nCntTornado].mtxWorld, &g_aTornado[nCntTornado].mtxWorld, &mtxRot);	// ��]��

			//----------------------------------------------------------------------------------------------------------
			//	�}�g���b�N�X���|�����킹��
			//----------------------------------------------------------------------------------------------------------
			if (g_aTornado[nCntTornado].pMtxParent != NULL)
			{ // �e�̃}�g���b�N�X�����݂���ꍇ

				// �e�̃}�g���b�N�X�Ɗ|�����킹��
				D3DXMatrixMultiply(&mtxOrigin, &mtxOrigin, g_aTornado[nCntTornado].pMtxParent);
			}

			// �������̃}�g���b�N�X�Ɗ|�����킹��
			D3DXMatrixMultiply(&g_aTornado[nCntTornado].mtxWorld, &g_aTornado[nCntTornado].mtxWorld, &mtxOrigin);

			//----------------------------------------------------------------------------------------------------------
			//	������`��
			//----------------------------------------------------------------------------------------------------------
			// ���[���h�}�g���b�N�X�̐ݒ�
			pDevice->SetTransform(D3DTS_WORLD, &g_aTornado[nCntTornado].mtxWorld);

			// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
			pDevice->SetStreamSource(0, g_aTornado[nCntTornado].pVtxBuff, 0, sizeof(VERTEX_3D));

			// ���_�t�H�[�}�b�g�̐ݒ�
			pDevice->SetFVF(FVF_VERTEX_3D);

			// �e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, g_apTextureTornado[g_aTornado[nCntTornado].type]);

			// �|���S���̕`��
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, g_aTornado[nCntTornado].nNumVtx - 2);
		}
	}

	// ���C�e�B���O��L���ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// �|���S���̕\�ʂ݂̂�\����Ԃɂ���
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// ���e�X�g�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);		// ���e�X�g�̗L�� / �����̐ݒ�
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// ���e�X�g�̐ݒ�
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);					// ���e�X�g�̎Q�ƒl�ݒ�
}

//======================================================================================================================
//	�����̐ݒ菈��
//======================================================================================================================
int SetTornado
(
	D3DXVECTOR3 pos,		// �ʒu
	D3DXVECTOR3 direRot,	// ��������
	TORNADOTYPE type,		// ���
	D3DXMATRIX *pMtxParent,	// �e�̃}�g���b�N�X
	int   nNumAround,		// �Q�̎���
	int   nPattern,			// �Q�̕�����
	float fMoveRot,			// �����̕ύX��
	float fThickness,		// �|���S���̑���
	float fOuterPlus,		// �|���S���O���� y���W���Z��
	float fSetWidth,		// �������̉������
	float fSetAlpha,		// �������̓����x
	float fAddWidth,		// ������̉��Z��
	float fAddHeight,		// �c����̉��Z��
	float fSubAlpha,		// �����x�̌��Z��
	float fGrowWidth,		// ������̐�����
	float fGrowHeight,		// �c����̐�����
	float fGrowAlpha		// �����x�̐�����
)
{
	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^

	for (int nCntTornado = 0; nCntTornado < MAX_TORNADO; nCntTornado++)
	{ // �����̍ő�\�������J��Ԃ�

		if (g_aTornado[nCntTornado].bUse == false)
		{ // �������g�p����Ă��Ȃ��ꍇ

			// ��{����ݒ�
			g_aTornado[nCntTornado].pos		= pos;		// �ʒu
			g_aTornado[nCntTornado].direRot	= direRot;	// �g���l�[�h�̐�������
			g_aTornado[nCntTornado].type	= type;		// ���

			g_aTornado[nCntTornado].pMtxParent	= pMtxParent;	// �e�̃}�g���b�N�X
			g_aTornado[nCntTornado].nNumAround	= nNumAround;	// �Q�̎���
			g_aTornado[nCntTornado].nPattern	= nPattern;		// �Q�̕�����

			g_aTornado[nCntTornado].fMoveRot	= fMoveRot;		// �����̕ύX��
			g_aTornado[nCntTornado].fThickness	= fThickness;	// �|���S���̑���
			g_aTornado[nCntTornado].fOuterPlus	= fOuterPlus;	// �|���S���O���� y���W���Z��

			g_aTornado[nCntTornado].fSetWidth	= fSetWidth;	// �������̉������
			g_aTornado[nCntTornado].fSetAlpha	= fSetAlpha;	// �������̓����x

			g_aTornado[nCntTornado].fAddWidth	= fAddWidth;	// ������̉��Z��
			g_aTornado[nCntTornado].fAddHeight	= fAddHeight;	// �c����̉��Z��
			g_aTornado[nCntTornado].fSubAlpha	= fSubAlpha;	// �����x�̌��Z��

			g_aTornado[nCntTornado].fGrowWidth	= fGrowWidth;	// ������̐�����
			g_aTornado[nCntTornado].fGrowHeight	= fGrowHeight;	// �c����̐�����
			g_aTornado[nCntTornado].fGrowAlpha	= fGrowAlpha;	// �����x�̐�����

			// ������������
			g_aTornado[nCntTornado].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			// ���_����������
			if (g_aTornado[nCntTornado].pVtxBuff == NULL)
			{ // ���_�o�b�t�@���g�p����Ă��Ȃ��ꍇ

				// �K�v���_�����v�Z
				g_aTornado[nCntTornado].nNumVtx = nNumAround * nPattern * MAX_OFFSET;

				// ���_�o�b�t�@�̐���
				pDevice->CreateVertexBuffer
				( // ����
					sizeof(VERTEX_3D) * g_aTornado[nCntTornado].nNumVtx,	// �K�v���_��
					D3DUSAGE_WRITEONLY,					// �g�p���@
					FVF_VERTEX_3D,						// ���_�t�H�[�}�b�g
					D3DPOOL_MANAGED,					// �������̎w��
					&g_aTornado[nCntTornado].pVtxBuff,	// ���_�o�b�t�@�ւ̃|�C���^
					NULL
				);

				// �����̒��_���̐ݒ�
				SetVtxTornado(&g_aTornado[nCntTornado]);
			}
			else { assert(false); }	// �g�p��

			// �g�p���Ă����Ԃɂ���
			g_aTornado[nCntTornado].bUse = true;

			// �g�p����C���f�b�N�X��Ԃ�
			return nCntTornado;
		}
	}

	// �g�p�s�̃C���f�b�N�X��Ԃ�
	return NONE_IDX;
}

//======================================================================================================================
//	�����̐����ݒ菈��
//======================================================================================================================
void SetGrowTornado
(
	int   nID,			// �ύX�C���f�b�N�X
	float fMoveRot,		// �����̕ύX��
	float fGrowWidth,	// ������̐�����
	float fGrowHeight,	// �c����̐�����
	float fGrowAlpha	// �����x�̐�����
)
{
	if (nID > NONE_IDX)
	{ // �g�p�\�ȃC���f�b�N�X�̏ꍇ

		// �����̏���ݒ�
		g_aTornado[nID].fMoveRot	= fMoveRot;		// �����̕ύX��
		g_aTornado[nID].fGrowWidth	= fGrowWidth;	// ������̐�����
		g_aTornado[nID].fGrowHeight	= fGrowHeight;	// �c����̐�����
		g_aTornado[nID].fGrowAlpha	= fGrowAlpha;	// �����x�̐�����
	}
}

//======================================================================================================================
//	�����̍폜����
//======================================================================================================================
void DeleteTornado(Tornado *pTornado)
{
	if (pTornado->pVtxBuff != NULL)
	{ // �g���l�[�h�̒��_�o�b�t�@���g�p���̏ꍇ

		// ���������J��
		pTornado->pVtxBuff->Release();
		pTornado->pVtxBuff = NULL;	// �|�C���^������

		// �g�p���Ă��Ȃ���Ԃɂ���
		pTornado->bUse = false;
	}
	else { assert(false); }	// ��g�p��
}

//======================================================================================================================
//	�����̒��_���̐ݒ菈��
//======================================================================================================================
void SetVtxTornado(Tornado *pTornado)
{
	// �ϐ���錾
	D3DXVECTOR3 vecPos;	// �����̒��_�����x�N�g��
	float fWidth  = pTornado->fSetWidth;	// ���_�����̉������
	float fHeight = 0.0f;					// ���_�����̏c�����
	float fAlpha  = pTornado->fSetAlpha;	// ���_�J���[�̓����x

	// �|�C���^��錾
	VERTEX_3D *pVtx;	// ���_���ւ̃|�C���^

	if (pTornado->pVtxBuff != NULL)
	{ // �g���l�[�h�̒��_�o�b�t�@���g�p���̏ꍇ

		// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
		pTornado->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntWidth = 0; nCntWidth < pTornado->nNumAround; nCntWidth++)
		{ // �Q�̎���̑������J��Ԃ�

			for (int nCntHeight = 0; nCntHeight < pTornado->nPattern; nCntHeight++)
			{ // �Q�̕��������J��Ԃ�

				// ���_�����x�N�g�������߂�
				vecPos.x = sinf(nCntHeight * ((D3DX_PI * 2.0f) / pTornado->nPattern)) * 1.0f;
				vecPos.y = 1.0f;
				vecPos.z = cosf(nCntHeight * ((D3DX_PI * 2.0f) / pTornado->nPattern)) * 1.0f;

				for (int nCntSet = 0; nCntSet < MAX_OFFSET; nCntSet++)
				{ // �I�t�Z�b�g�̑������J��Ԃ�

					// ���_���W�̐ݒ�
					pVtx[0].pos.x = 0.0f + vecPos.x * (fWidth + (nCntSet * pTornado->fThickness));	// x
					pVtx[0].pos.y = 0.0f + vecPos.y * (fHeight + (nCntSet * pTornado->fOuterPlus));	// y
					pVtx[0].pos.z = 0.0f + vecPos.z * (fWidth + (nCntSet * pTornado->fThickness));	// z

					// �@���x�N�g���̐ݒ�
					pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

					if (fAlpha < 0.0f)
					{ // �����x���͈͊O�̏ꍇ

						// �����x��␳
						fAlpha = 0.0f;
					}

					// ���_�J���[�̐ݒ�
					pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha);

					// �e�N�X�`�����W�̐ݒ�
					pVtx[0].tex = D3DXVECTOR2
					( // ����
						1.0f,			// u
						1.0f * nCntSet	// v
					);

					// ���_�f�[�^�̃|�C���^��i�߂�
					pVtx += 1;
				}

				// �c�����L����
				fWidth  += pTornado->fAddWidth;
				fHeight += pTornado->fAddHeight;

				// ���l�����Z
				fAlpha -= pTornado->fSubAlpha;
			}
		}

		// ���_�o�b�t�@���A�����b�N����
		pTornado->pVtxBuff->Unlock();
	}
	else { assert(false); }	// ��g�p��
}

#ifdef _DEBUG	// �f�o�b�O����
#endif