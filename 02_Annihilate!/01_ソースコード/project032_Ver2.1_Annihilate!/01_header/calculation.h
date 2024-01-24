//=============================================================================
//
//	�ėp�֐��w�b�_�[ [calculation.h]
//	Author�F���c�E��
//
//=============================================================================
#ifndef _CALCULATION_H_		// ���̃}�N����`������Ă��Ȃ��ꍇ
#define _CALCULATION_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

//*****************************************************************************
//	�C���N���[�h�t�@�C��
//*****************************************************************************
#include "main.h"

//*****************************************************************************
//	�v���g�^�C�v�錾
//*****************************************************************************
void  VecSizePos(D3DXVECTOR3 *sizePos, D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fHeight);													// �l���_�̈ʒu�̌v�Z����
float LineOuterProduct(D3DXVECTOR3 posLeft, D3DXVECTOR3 posRight, D3DXVECTOR3 pos);																		// �O�ς̍��E���f����
void  CollisionOuterProduct(D3DXVECTOR3 *Targetpos, D3DXVECTOR3 *TargetposOld, D3DXVECTOR3 WallRpos, D3DXVECTOR3 WallLpos, bool *bUse, int *nShadow);	// �O�ς̓����蔻��
bool  CollisionSector(D3DXVECTOR3 centerPos, D3DXVECTOR3 targetPos, float fCenterRot, float fRadius, float fAngle);										// ��`�̓����蔻��

void NormalizeRot(float *pRot);	// �����̐��K��

#endif