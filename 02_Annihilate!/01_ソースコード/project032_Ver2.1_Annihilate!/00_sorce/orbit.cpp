//======================================================================================================================
//
//	�O�Տ��� [orbit.cpp]
//	Author�F���c�E��
//
//======================================================================================================================
//**********************************************************************************************************************
//	�C���N���[�h�t�@�C��
//**********************************************************************************************************************
#include "main.h"
#include "orbit.h"
#include "game.h"

//**********************************************************************************************************************
//	�}�N����`
//**********************************************************************************************************************
#define MAX_ORBIT	(128)	// �O�Ղ̍ő吔
#define MAX_OFFSET	(2)		// �I�t�Z�b�g�̐�
#define MAX_VERTEX	(40)	// �ێ����钸�_�̍ő吔

//**********************************************************************************************************************
//	�R���X�g��`
//**********************************************************************************************************************
const char *apTextureOrbit[] =	// �e�N�X�`���̑��΃p�X
{
	NULL,									// NULL
	"02_data\\02_TEXTURE\\orbit000.jpg",	// �O�Ղ̃e�N�X�`���̑��΃p�X
};

const D3DXVECTOR3 aOffset[][MAX_OFFSET]	// �I�t�Z�b�g�̈ʒu�ύX��
{
	{ D3DXVECTOR3(0.0f, 0.0f, 0.0f),   D3DXVECTOR3(0.0f, 10.0f, 0.0f) },	// �G�̃I�t�Z�b�g (�c)
	{ D3DXVECTOR3(-6.5f, 0.0f, 0.0f),  D3DXVECTOR3(6.5f, 0.0f, 0.0f)  },	// �G�̃I�t�Z�b�g (��)
};

//**********************************************************************************************************************
//	�\���̒�` (Orbit)
//**********************************************************************************************************************
typedef struct
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff;		// ���_�o�b�t�@
	D3DXMATRIX *pMtxParent;					// �e�̃}�g���b�N�X
	D3DXVECTOR3 aOffset[MAX_OFFSET];		// ���[�̃I�t�Z�b�g
	D3DXCOLOR   aCol[MAX_OFFSET];			// ���[�̊�F
	D3DXMATRIX  aMtxWorldPoint[MAX_OFFSET];	// ���[�̃��[���h�}�g���b�N�X
	D3DXVECTOR3 aPosPoint[MAX_VERTEX];		// �v�Z��̊e���_���W
	D3DXCOLOR   aColPoint[MAX_VERTEX];		// �e���_�J���[
	ORBITTYPE   type;						// ���
	int        *pOrbitIDParent;				// �O�Ղ̐e�̋O�ՃC���f�b�N�X
	bool       *pUseParent;					// �O�Ղ̐e�̎g�p��
	bool        bInit;						// ��������
	bool        bUse;						// �g�p��
}Orbit;

//**********************************************************************************************************************
//	�O���[�o���ϐ�
//**********************************************************************************************************************
LPDIRECT3DTEXTURE9 g_apTextureOrbit[ORBITTYPE_MAX] = {};	// �e�N�X�`���ւ̃|�C���^

Orbit g_aOrbit[MAX_ORBIT];	// �O�Ղ̏��

//======================================================================================================================
//	�O�Ղ̏���������
//======================================================================================================================
void InitOrbit(void)
{
	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^
	VERTEX_3D *pVtx;							// ���_���ւ̃|�C���^

	// �e�N�X�`���̓ǂݍ���
	for (int nCntOrbit = 0; nCntOrbit < ORBITTYPE_MAX; nCntOrbit++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		D3DXCreateTextureFromFile(pDevice, apTextureOrbit[nCntOrbit], &g_apTextureOrbit[nCntOrbit]);
	}

	// �O�Ղ̏��̏�����
	for (int nCntOrbit = 0; nCntOrbit < MAX_ORBIT; nCntOrbit++)
	{ // �O�Ղ̍ő�\�������J��Ԃ�

		//--------------------------------------------------------------------------------------------------------------
		//	���_���̏�����
		//--------------------------------------------------------------------------------------------------------------
		// ���_�o�b�t�@�̐���
		pDevice->CreateVertexBuffer
		( // ����
			sizeof(VERTEX_3D) * MAX_VERTEX,		// �K�v���_��
			D3DUSAGE_WRITEONLY,					// �g�p���@
			FVF_VERTEX_3D,						// ���_�t�H�[�}�b�g
			D3DPOOL_MANAGED,					// �������̎w��
			&g_aOrbit[nCntOrbit].pVtxBuff,		// ���_�o�b�t�@�ւ̃|�C���^
			NULL
		);

		// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
		g_aOrbit[nCntOrbit].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntWidth = 0; nCntWidth < (int)(MAX_VERTEX * 0.5f); nCntWidth++)
		{ // �ێ����鉡���̒��_�̍ő吔���J��Ԃ�

			for (int nCntHeight = 0; nCntHeight < MAX_OFFSET; nCntHeight++)
			{ // �ێ�����c���̒��_�̍ő吔���J��Ԃ�

				// ���_���W�̐ݒ�
				pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

				// �@���x�N�g���̐ݒ�
				pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

				// ���_�J���[�̐ݒ�
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

				// �e�N�X�`�����W�̐ݒ�
				pVtx[0].tex = D3DXVECTOR2
				( // ����
					1.0f * (nCntWidth % MAX_OFFSET),	// u
					1.0f * nCntHeight					// v
				);

				// ���_�f�[�^�̃|�C���^��i�߂�
				pVtx += 1;
			}
		}

		// ���_�o�b�t�@���A�����b�N����
		g_aOrbit[nCntOrbit].pVtxBuff->Unlock();

		//--------------------------------------------------------------------------------------------------------------
		//	��{���̏�����
		//--------------------------------------------------------------------------------------------------------------
		// �A�h���X��������
		g_aOrbit[nCntOrbit].pOrbitIDParent = NULL;		// �O�Ղ̐e�̋O�ՃC���f�b�N�X
		g_aOrbit[nCntOrbit].pMtxParent     = NULL;		// �e�̃}�g���b�N�X
		g_aOrbit[nCntOrbit].pUseParent     = NULL;		// �O�Ղ̐e�̎g�p��

		// �^�U�󋵂�������
		g_aOrbit[nCntOrbit].bInit = false;		// ��������
		g_aOrbit[nCntOrbit].bUse  = false;		// �g�p��

		// ��ނ�������
		g_aOrbit[nCntOrbit].type = ORBITTYPE_NONE;

		for (int nCntOff = 0; nCntOff < MAX_OFFSET; nCntOff++)
		{ // �I�t�Z�b�g�̐����J��Ԃ�

			g_aOrbit[nCntOrbit].aOffset[nCntOff] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// ���[�̃I�t�Z�b�g
			g_aOrbit[nCntOrbit].aCol[nCntOff]    = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);	// ���[�̊�F
		}

		for (int nCntVtx = 0; nCntVtx < MAX_VERTEX; nCntVtx++)
		{ // �ێ����钸�_�̍ő吔���J��Ԃ�

			g_aOrbit[nCntOrbit].aPosPoint[nCntVtx] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// �v�Z��̊e���_���W
			g_aOrbit[nCntOrbit].aColPoint[nCntVtx] = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);	// �e���_�J���[
		}
	}
}

//======================================================================================================================
//	�O�Ղ̏I������
//======================================================================================================================
void UninitOrbit(void)
{
	// �e�N�X�`���̔j��
	for (int nCntOrbit = 0; nCntOrbit < ORBITTYPE_MAX; nCntOrbit++)
	{ // �g�p����e�N�X�`�������J��Ԃ�

		if (g_apTextureOrbit[nCntOrbit] != NULL)
		{ // �ϐ� (g_apTextureOrbit) ��NULL�ł͂Ȃ��ꍇ

			g_apTextureOrbit[nCntOrbit]->Release();
			g_apTextureOrbit[nCntOrbit] = NULL;
		}
	}

	// ���_�o�b�t�@�̔j��
	for (int nCntOrbit = 0; nCntOrbit < MAX_ORBIT; nCntOrbit++)
	{ // �O�Ղ̍ő�\�������J��Ԃ�

		if (g_aOrbit[nCntOrbit].pVtxBuff != NULL)
		{ // �ϐ� (g_aOrbit[nCntOrbit].pVtxBuff) ��NULL�ł͂Ȃ��ꍇ

			g_aOrbit[nCntOrbit].pVtxBuff->Release();
			g_aOrbit[nCntOrbit].pVtxBuff = NULL;
		}
	}
}

//======================================================================================================================
//	�O�Ղ̍X�V����
//======================================================================================================================
void UpdateOrbit(void)
{
	for (int nCntOrbit = 0; nCntOrbit < MAX_ORBIT; nCntOrbit++)
	{ // �O�Ղ̍ő�\�������J��Ԃ�

		if (g_aOrbit[nCntOrbit].bUse == true && g_aOrbit[nCntOrbit].pUseParent != NULL)
		{ // �O�Ղ��g�p����Ă��銎�A�O�Ղ̐e�̎g�p�󋵃A�h���X���ݒ肳��Ă���ꍇ

			if (*g_aOrbit[nCntOrbit].pUseParent == false)
			{ // �O�Ղ̐e���g�p����Ă��Ȃ��ꍇ

				// �O�Ղ��g�p���Ă��Ȃ���Ԃɂ���
				g_aOrbit[nCntOrbit].bUse = false;
			}
			else
			{ // �O�Ղ̐e���g�p����Ă���ꍇ

				if (g_aOrbit[nCntOrbit].pOrbitIDParent != NULL)
				{ // �O�Ղ̐e�̋O�ՃC���f�b�N�X�A�h���X���ݒ肳��Ă���ꍇ

					if (*g_aOrbit[nCntOrbit].pOrbitIDParent != nCntOrbit)
					{ // �O�Ղ̐e�̋O�ՃC���f�b�N�X�ƁA���݂̋O�ՃC���f�b�N�X�̒l���Ⴄ�ꍇ

						// �O�Ղ��g�p���Ă��Ȃ���Ԃɂ���
						g_aOrbit[nCntOrbit].bUse = false;

						// [��] �g�p���Ȃ���ԂɂȂ����e�����t���[�����œ����C���f�b�N�X���g����
						//      �܂��g�p���ꂽ�ꍇ�ɋO�Ղ���̐e�ɓ��������Ă��܂����߁A���̖h�~�B
					}
				}
			}
		}
	}
}

//======================================================================================================================
//	�O�Ղ̕`�揈��
//======================================================================================================================
void DrawOrbit(void)
{
	// �ϐ���錾
	D3DXMATRIX mtxIdent;	// �P�ʃ}�g���b�N�X�ݒ�p

	// �P�ʃ}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&mtxIdent);

	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// �f�o�C�X�ւ̃|�C���^
	VERTEX_3D *pVtx;							// ���_���ւ̃|�C���^
	
	// ���C�e�B���O�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// �|���S���̗��ʂ�\����Ԃɂ���
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	for (int nCntOrbit = 0; nCntOrbit < MAX_ORBIT; nCntOrbit++)
	{ // �O�Ղ̍ő�\�������J��Ԃ�

		if (g_aOrbit[nCntOrbit].bUse == true)
		{ // �O�Ղ��g�p����Ă���ꍇ

			//----------------------------------------------------------------------------------------------------------
			//	�I�t�Z�b�g�̏�����
			//----------------------------------------------------------------------------------------------------------
			for (int nCntOff = 0; nCntOff < MAX_OFFSET; nCntOff++)
			{ // �I�t�Z�b�g�̐����J��Ԃ�

				// ���[���h�}�g���b�N�X�̏�����
				D3DXMatrixIdentity(&g_aOrbit[nCntOrbit].aMtxWorldPoint[nCntOff]);

				// �ʒu�𔽉f
				D3DXMatrixTranslation(&g_aOrbit[nCntOrbit].aMtxWorldPoint[nCntOff], g_aOrbit[nCntOrbit].aOffset[nCntOff].x, g_aOrbit[nCntOrbit].aOffset[nCntOff].y, g_aOrbit[nCntOrbit].aOffset[nCntOff].z);

				// �e�̃}�g���b�N�X�Ɗ|�����킹��
				D3DXMatrixMultiply(&g_aOrbit[nCntOrbit].aMtxWorldPoint[nCntOff], &g_aOrbit[nCntOrbit].aMtxWorldPoint[nCntOff], g_aOrbit[nCntOrbit].pMtxParent);
			}

			if (GetPause() == false)
			{ // �|�[�Y���ł͂Ȃ��ꍇ

				//------------------------------------------------------------------------------------------------------
				//	���_���W�ƒ��_�J���[�̏������炷
				//------------------------------------------------------------------------------------------------------
				for (int nCntVtx = MAX_VERTEX - 1; nCntVtx >= MAX_OFFSET; nCntVtx--)
				{ // �ێ����钸�_�̍ő吔���J��Ԃ� (�I�t�Z�b�g���͊܂܂Ȃ�)

					// ���_�������炷
					g_aOrbit[nCntOrbit].aPosPoint[nCntVtx] = g_aOrbit[nCntOrbit].aPosPoint[nCntVtx - MAX_OFFSET];
					g_aOrbit[nCntOrbit].aColPoint[nCntVtx] = g_aOrbit[nCntOrbit].aColPoint[nCntVtx - MAX_OFFSET];
				}

				//------------------------------------------------------------------------------------------------------
				//	�ŐV�̒��_���W�ƒ��_�J���[�̏���ݒ�
				//------------------------------------------------------------------------------------------------------
				for (int nCntOff = 0; nCntOff < MAX_OFFSET; nCntOff++)
				{ // �I�t�Z�b�g�̐����J��Ԃ�

					// ���_���W�̐ݒ�
					g_aOrbit[nCntOrbit].aPosPoint[nCntOff] = D3DXVECTOR3
					( // ����
						g_aOrbit[nCntOrbit].aMtxWorldPoint[nCntOff]._41,	// x
						g_aOrbit[nCntOrbit].aMtxWorldPoint[nCntOff]._42,	// y
						g_aOrbit[nCntOrbit].aMtxWorldPoint[nCntOff]._43		// z
					);

					// ���_�J���[�̐ݒ�
					g_aOrbit[nCntOrbit].aColPoint[nCntOff] = g_aOrbit[nCntOrbit].aCol[nCntOff];
				}
			}

			//----------------------------------------------------------------------------------------------------------
			//	���_���W�ƒ��_�J���[�̏���������
			//----------------------------------------------------------------------------------------------------------
			if (g_aOrbit[nCntOrbit].bInit == false)
			{ // �������ς݂ł͂Ȃ��ꍇ

				for (int nCntVtx = 0; nCntVtx < MAX_VERTEX; nCntVtx++)
				{ // �ێ����钸�_�̍ő吔���J��Ԃ�

					// ���_���W�̐ݒ�
					g_aOrbit[nCntOrbit].aPosPoint[nCntVtx] = D3DXVECTOR3
					( // ����
						g_aOrbit[nCntOrbit].aMtxWorldPoint[nCntVtx % MAX_OFFSET]._41,	// x
						g_aOrbit[nCntOrbit].aMtxWorldPoint[nCntVtx % MAX_OFFSET]._42,	// y
						g_aOrbit[nCntOrbit].aMtxWorldPoint[nCntVtx % MAX_OFFSET]._43	// z
					);

					// ���_�J���[�̐ݒ�
					g_aOrbit[nCntOrbit].aColPoint[nCntVtx] = g_aOrbit[nCntOrbit].aCol[nCntVtx % MAX_OFFSET];
				}

				// �������ς݂ɂ���
				g_aOrbit[nCntOrbit].bInit = true;
			}

			//----------------------------------------------------------------------------------------------------------
			//	���_���̐ݒ�
			//----------------------------------------------------------------------------------------------------------
			// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
			g_aOrbit[nCntOrbit].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			for (int nCntVtx = 0; nCntVtx < MAX_VERTEX; nCntVtx++)
			{ // �ێ����钸�_�̍ő吔���J��Ԃ�

				// ���_���W�̐ݒ�
				pVtx[0].pos = g_aOrbit[nCntOrbit].aPosPoint[nCntVtx];

				// ���_�J���[�̐ݒ�
				pVtx[0].col = g_aOrbit[nCntOrbit].aColPoint[nCntVtx];

				// ���_�f�[�^�̃|�C���^��i�߂�
				pVtx += 1;
			}

			// ���_�o�b�t�@���A�����b�N����
			g_aOrbit[nCntOrbit].pVtxBuff->Unlock();

			//----------------------------------------------------------------------------------------------------------
			//	�|���S���̕`��
			//----------------------------------------------------------------------------------------------------------
			// �P�ʃ}�g���b�N�X�̐ݒ�
			pDevice->SetTransform(D3DTS_WORLD, &mtxIdent);

			// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
			pDevice->SetStreamSource(0, g_aOrbit[nCntOrbit].pVtxBuff, 0, sizeof(VERTEX_3D));

			// ���_�t�H�[�}�b�g�̐ݒ�
			pDevice->SetFVF(FVF_VERTEX_3D);

			// �e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, g_apTextureOrbit[g_aOrbit[nCntOrbit].type]);

			// �|���S���̕`��
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, MAX_VERTEX - 2);
		}
	}

	// ���C�e�B���O��L���ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// �|���S���̕\�ʂ݂̂�\����Ԃɂ���
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//======================================================================================================================
//	�O�Ղ̐ݒ菈��
//======================================================================================================================
int SetOrbit(D3DXMATRIX *pMtxParent, bool *pUse, int *pOrbitID, D3DXCOLOR col, OFFSETTYPE offType, ORBITTYPE texType)
{
	for (int nCntOrbit = 0; nCntOrbit < MAX_ORBIT; nCntOrbit++)
	{ // �O�Ղ̍ő�\�������J��Ԃ�

		if (g_aOrbit[nCntOrbit].bUse == false)
		{ // �O�Ղ��g�p����Ă��Ȃ��ꍇ

			// �����̃A�h���X��ݒ�
			g_aOrbit[nCntOrbit].pOrbitIDParent = pOrbitID;		// �O�Ղ̐e�̋O�ՃC���f�b�N�X
			g_aOrbit[nCntOrbit].pMtxParent     = pMtxParent;	// �e�̃}�g���b�N�X
			g_aOrbit[nCntOrbit].pUseParent     = pUse;			// �O�Ղ̐e�̎g�p��

			// �����̎�ނ�ݒ�
			g_aOrbit[nCntOrbit].type = texType;

			// �������󋵂�������
			g_aOrbit[nCntOrbit].bInit = false;

			// �I�t�Z�b�g�̒��_���̏�����
			for (int nCntOff = 0; nCntOff < MAX_OFFSET; nCntOff++)
			{ // �I�t�Z�b�g�̐����J��Ԃ�

				g_aOrbit[nCntOrbit].aOffset[nCntOff] = aOffset[(int)offType][nCntOff];	// ���[�̃I�t�Z�b�g
				g_aOrbit[nCntOrbit].aCol[nCntOff]    = col;								// ���[�̊�F
			}

			// �ێ����钸�_���̏�����
			for (int nCntVtx = 0; nCntVtx < MAX_VERTEX; nCntVtx++)
			{ // ���_�̍ő吔���J��Ԃ�

				g_aOrbit[nCntOrbit].aPosPoint[nCntVtx] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// �v�Z��̊e���_���W
				g_aOrbit[nCntOrbit].aColPoint[nCntVtx] = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// �e���_�J���[
			}

			// �g�p���Ă����Ԃɂ���
			g_aOrbit[nCntOrbit].bUse = true;

			// �g�p�ł���O�Ղ�ID��Ԃ�
			return nCntOrbit;
		}
	}

	// �O�Ղ̑���������Ȃ��ꍇ -1��Ԃ�
	return NONE_ORBIT;
}

#ifdef _DEBUG	// �f�o�b�O����
#endif