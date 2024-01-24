//=============================================================================
//
//	汎用関数ヘッダー [calculation.h]
//	Author：藤田勇一
//
//=============================================================================
#ifndef _CALCULATION_H_		// このマクロ定義がされていない場合
#define _CALCULATION_H_		// 二重インクルード防止のマクロを定義する

//*****************************************************************************
//	インクルードファイル
//*****************************************************************************
#include "main.h"

//*****************************************************************************
//	プロトタイプ宣言
//*****************************************************************************
void  VecSizePos(D3DXVECTOR3 *sizePos, D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fHeight);													// 四頂点の位置の計算処理
float LineOuterProduct(D3DXVECTOR3 posLeft, D3DXVECTOR3 posRight, D3DXVECTOR3 pos);																		// 外積の左右判断処理
void  CollisionOuterProduct(D3DXVECTOR3 *Targetpos, D3DXVECTOR3 *TargetposOld, D3DXVECTOR3 WallRpos, D3DXVECTOR3 WallLpos, bool *bUse, int *nShadow);	// 外積の当たり判定
bool  CollisionSector(D3DXVECTOR3 centerPos, D3DXVECTOR3 targetPos, float fCenterRot, float fRadius, float fAngle);										// 扇形の当たり判定

void NormalizeRot(float *pRot);	// 向きの正規化

#endif