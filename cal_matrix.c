#include "cal_matrix.h"
#include "ln_alg_mem.h"
#include "ln_log.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef LN_DEBUG
#include "ln_debug.h"
#endif


Kmat* KmMatMalloc(int row, int col)
{
	Kmat* mat = (Kmat*)lnAlgMalloc(row * sizeof(Kmat*));

	mat->row  = row;
	mat->col  = col;
	
	mat->data = (float**)lnAlgMalloc(row * sizeof(float*));
	if (mat->data == NULL)
	{
		LN_LOG(LN_LOG_ERROR, "TPD KmMatMalloc Malloc FAILED.\n");
		return NULL;
	}

	for (int i = 0; i < row; i++)
	{
		mat->data[i] = (float*)lnAlgMalloc(col * sizeof(float));

		if (mat->data[i] == NULL)
		{
			LN_LOG(LN_LOG_ERROR, "TPD KmMatMalloc Malloc FAILED.\n");
			for (int j = 0; j < i; j++) lnAlgFree(mat->data[j]);
			return NULL;
		}
	}

	return mat;
}

void KmMatFree(Kmat* mat)
{
	if (mat == NULL)
	{
		return;
	}

	for (int i = 0; i < mat->row; i++) lnAlgFree(mat->data[i]);
	
	lnAlgFree(mat->data);

	lnAlgFree(mat);

	return;
}

void KmSetValue(Kmat* m1, float* data)
{
	for (int row = 0; row < m1->row; row++) {
		for (int col = 0; col < m1->col; col++) {
			/*int d = row * m1->col + col;
			float c=data[11];*/
			// printf("row[%d] * m1->col[%d] + col[%d] = %d\n", row, m1->col, col, row * m1->col + col);
			m1->data[row][col] = data[row * m1->col + col];
		}
	}

	return;
}

void KmEye(Kmat* mat, float a)
{
	for (int row = 0; row < mat->row; row++) memset(mat->data[row], 0, mat->col);
	for (int i = 0; i < min(mat->row, mat->col); i++) mat->data[i][i] = a;

	return;
}

void KmAdd(Kmat* m1, Kmat* m2, Kmat* out)
{
	if ((m1->col != m2->col) || (m1->row != m2->row))
	{
		lnLog(LN_LOG_ERROR, "TPD KmAdd Unmatchable Matrix Size.\n");
		return;
	}
	
	for (int row = 0; row < m1->row; row++)
	{
		for (int col = 0; col < m1->col; col++)
		{
			out->data[row][col] = m1->data[row][col] + m2->data[row][col];
		}
	}

	return;
}

void KmSub(Kmat* m1, Kmat* m2, Kmat* out)
{
	if (!(m1->col == m2->col && m1->row == m2->row))
	{
		lnLog(LN_LOG_ERROR, "TPD KmSub Unmatchable Matrix Size.\n");
		return;
	}
	
	for (int row = 0; row < m1->row; row++)
	{
		for (int col = 0; col < m1->col; col++)
		{
			out->data[row][col] = m1->data[row][col] - m2->data[row][col];
		}
	}

	return;
}

void KmMul(Kmat* m1, Kmat* m2, Kmat* out)
{
	if (m1->col != m2->row)
	{
		lnLog(LN_LOG_ERROR, "TPD KmMul Unmatchable Matrix Size.\n");
		return;
	}

	if ((out->row != m1->row) || (out->col != m2->col))
	{
		lnLog(LN_LOG_ERROR, "TPD KmMul Unmatchable Matrix Size.\n");
		return;
	}

	for (int row = 0; row < m1->row; row++)
	{
		for (int col = 0; col < m2->col; col++)
		{
			float sum = 0;
			for (int i = 0; i < m1->col; i++)
			{
				sum += m1->data[row][i] * m2->data[i][col];
			}
			out->data[row][col] = sum;
		}
	}

	return;
}

void KmTrans(Kmat* mat, Kmat* mat_trans)
{
	if ((mat->row != mat_trans->col) || (mat->col != mat_trans->row))
	{
		lnLog(LN_LOG_ERROR, "TPD KmTrans Unmatchable Matrix Size.\n");
		return;
	}

	for (int row = 0; row < mat->col; row++)
	{
		for (int col = 0; col < mat->row; col++) mat_trans->data[row][col] = mat->data[col][row];
	}

	return;
}

// LU inverse
void KmInv(Kmat* m1, Kmat* out)//
{
	float A[3][3] =
	{ 
		{ m1->data[0][0], m1->data[0][1], m1->data[0][2] },
		{ m1->data[1][0], m1->data[1][1], m1->data[1][2] },
		{ m1->data[2][0], m1->data[2][1], m1->data[2][2] }
	};

	float L[3][3]   = { .0f };
	float U[3][3]   = { .0f };
	float Lni[3][3] = { .0f };
	float Uni[3][3] = { .0f };
	float Ani[3][3] = { .0f };

	int size = 3;
	for (int i = 0; i < size; i++) L[i][i] = 1.0;
	for (int j = 0; j < size; j++) U[0][j] = A[0][j];
	for (int i = 1; i < size; i++) L[i][0] = A[i][0] / U[0][0];

	for (int k = 1; k < size; k++)
	{
		for (int j = k; j < size; j++)
		{
			float s = 0.0;
			for (int t = 0; t < k; t++)
			{
				s += L[k][t] * U[t][j];
			}
			U[k][j] = A[k][j] - s;
		}

		for (int i = k; i < size; i++)
		{
			float s = 0.0;
			for (int t = 0; t < k; t++)
			{
				s += L[i][t] * U[t][k];
			}
			L[i][k] = (A[i][k] - s) / U[k][k];
		}
	}

	for (int j = 0; j < size; j++)
	{
		for (int i = j; i < size; i++)
		{
			if (i == j)
			{
				Lni[i][j] = 1 / L[i][j];
			}
			else if (i < j)
			{
				Lni[i][j] = 0;
			}
			else
			{
				float s = 0.0;
				for (int k = j; k < i; k++)
				{
					s += L[i][k] * Lni[k][j];
				}
				Lni[i][j] = -Lni[j][j] * s;
			}
		}
	}

	for (int j = 0; j < size; j++)
	{
		for (int i = j; i >= 0; i--)
		{
			if (i == j) 
			{
				Uni[i][j] = 1 / U[i][j];
			}
			else if (i > j)
			{
				Uni[i][j] = 0;
			}
			else
			{
				float s = 0.0;
				for (int k = i + 1; k <= j; k++)
				{
					s += U[i][k] * Uni[k][j];
				}
				Uni[i][j] = -1 / U[i][i] * s;
			}
		}
	}

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			for (int k = 0; k < size; k++)
			{
				Ani[i][j] += Uni[i][k] * Lni[k][j];
			}
		}
	}

	KmSetValue(out, Ani);
	
	return;
}


void KmClone(Kmat* m1, Kmat* out)
{
	// KmInitial(out, m1->row, m1->col);
	if ((m1->row != out->row) || (m1->col != out->col))
	{
		lnLog(LN_LOG_ERROR, "TPD KmClone Unmatchable Matrix Size.\n");
		return;
	}	

	for (int row = 0; row < m1->row; row++) {
		for (int col = 0; col < m1->col; col++)
			out->data[row][col] = m1->data[row][col];
	}

	return;
}
