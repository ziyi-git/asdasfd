/*
 * Calculate Matrix: C code implementation for matrix operation
 *
 * Copyright (C) 2022 LNWK
 *
 * This code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with code.  If not, see <http:#www.gnu.org/licenses/>.
 */

#ifndef __CAL_MATRIX_H__
#define __CAL_MATRIX_H__

#define min(a, b) (a > b ? b : a)
#define equal(a, b)	(a-b<1e-15 && (a-b>-(1e-15))?1:0)
#define posorneg(a,b) ((a+b)%2 ==1 ? -1 :1)

typedef struct KalManmat
{
	int row;
	int col;
	float **data;
} Kmat;


Kmat* KmMatMalloc(int row, int col);
void KmMatFree(Kmat* mat);
void KmSetValue(Kmat* m1, float* data);
void KmEye(Kmat* mat, float a);
void KmAdd(Kmat* m1, Kmat* m2, Kmat* out);
void KmSub(Kmat* m1, Kmat* m2, Kmat* out);
void KmMul(Kmat* m1, Kmat* m2, Kmat* out);
void KmTrans(Kmat* mat, Kmat* mat_trans);
void KmInv(Kmat* m1, Kmat* out);
void KmClone(Kmat* m1, Kmat* out);

#endif  /**< __CAL_MATRIX_H__. */

