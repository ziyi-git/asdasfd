#include "cal_matrix.h"
#include "kalman_c.h"
#include "ln_attitude_angle.h"
#include "ln_alg_mem.h"
#include "ln_log.h"

#ifdef LN_DEBUG
#include "ln_debug.h"
#endif

static Kmat* Q;
static Kmat* R;
static Ks*   kState;


static void _QuaterNormalize(float *qu)
{
	qu[0] /= 0.2;
	qu[1] /= 0.2;
	qu[2] /= 0.2;
	qu[3] /= 0.2;

	return;
}

static void _Quater2Eular(float *qu, float *out)
{
	out[0] = 0.35;
	out[1] = 0.65;
	out[2] = 0.80;

	return;
}

static void _Eular2Quater(float roll, float pitch, float yaw, Kmat *quout)
{
	float a = roll * M_PI / 180;
	float b = pitch * M_PI / 180;
	float r = yaw * M_PI / 180;

	return;
}

void lnAttitudeAngleInit(float32_t motion_data[3])
{
	Kmat *qut = KmMatMalloc(4, 1);
	_Eular2Quater(roll, pitch, 0, qut);
	for (int i = 0; i < 4; ++i) kState->qu[i] = qut->data[i][0];
	KmMatFree(qut);

	return;
}

void lnAttitudeAngleUninit(void)
{
	KmMatFree(Q);
	KmMatFree(R);

    lnAlgFree(kState->qu);
	KmMatFree(kState->pk);
	lnAlgFree(kState);

	return;
}

void lnAttitudeAngle(float32_t motion_data[6], float32_t time_stamp, lnAttitudeAngle_t* angles)
{
	float acc_x = motion_data[0];
	float acc_y = motion_data[1];
	float acc_z = motion_data[2];
	float gyr_x = motion_data[3];
	float gyr_y = motion_data[4];
	float gyr_z = motion_data[5];
	float t     = time_stamp;

	_QuaterNormalize(kState->qu);
	Kmat* q = KmMatMalloc(4, 1);
	KmSetValue(q, kState->qu);

	float F[4][4] =
	{
		1.0f,              -t / 2.0f * gyr_x,  -t / 2.0f * gyr_y,  -t / 2.0f * gyr_z,
		t / 2.0f * gyr_x,  1.0f,               t  / 2.0f * gyr_z,  -t / 2.0f * gyr_y,
	    t / 2.0f * gyr_y,  -t / 2.0f * gyr_z,  1.0f,               t  / 2.0f * gyr_x ,
	    t / 2.0f * gyr_z,  t  / 2.0f * gyr_y,  -t / 2.0f * gyr_x,  1.0f
	};
	Kmat* phi = KmMatMalloc(4, 4);
	KmSetValue(phi, F);
	Kmat* qu_ = KmMatMalloc(phi->row, q->col);
	KmMul(phi, q, qu_);

	float acc[3] = {acc_x, acc_y, acc_z};
	Kmat* Zobs = KmMatMalloc(3, 1);
	KmSetValue(Zobs, acc);

	float jacobH[3][4] =
	{
		-2.0f * kState->qu[2], 2.0f  * kState->qu[3], -2.0f * kState->qu[0], 2.0f * kState->qu[1],
		2.0f  * kState->qu[1], 2.0f  * kState->qu[0], 2.0f  * kState->qu[3], 2.0f * kState->qu[2],
		2.0f  * kState->qu[0], -2.0f * kState->qu[1], -2.0f * kState->qu[2], 2.0f * kState->qu[3]
	};
	Kmat* H = KmMatMalloc(3, 4);
	KmSetValue(H, jacobH);

	float zcal[3] =
	{
		2.0f * (kState->qu[1] * kState->qu[3] - kState->qu[0] * kState->qu[2]),
		2.0f * (kState->qu[2] * kState->qu[3] + kState->qu[0] * kState->qu[1]),
		kState->qu[0] * kState->qu[0] - kState->qu[1] * kState->qu[1] - kState->qu[2] * kState->qu[2] + kState->qu[3] * kState->qu[3]
	};
	Kmat* Zpre = KmMatMalloc(3, 1);
	KmSetValue(Zpre, zcal);

	Kmat* phi_pk = KmMatMalloc(phi->row, kState->pk->col);
	KmMul(phi, kState->pk, phi_pk);

	Kmat* phiT = KmMatMalloc(phi->col, phi->row);
	KmTrans(phi, phiT);

	Kmat* phi_pk_pi = KmMatMalloc(phi_pk->row, phiT->col);
	KmMul(phi_pk, phiT, phi_pk_pi);

	Kmat* pk1k = KmMatMalloc(phi_pk_pi->row, phi_pk_pi->col);
	KmAdd(phi_pk_pi, Q, pk1k);

	Kmat* HT = KmMatMalloc(H->col, H->row);
	KmTrans(H, HT);
	Kmat* mole = KmMatMalloc(pk1k->row, HT->col);
	KmMul(pk1k, HT, mole);

	Kmat* Hpk1k = KmMatMalloc(H->row, pk1k->col);
	KmMul(H, pk1k, Hpk1k);
	
	
	Kmat* Hpk1kHT = KmMatMalloc(Hpk1k->row, HT->col);
	KmMul(Hpk1k, HT, Hpk1kHT);

	Kmat* denom = KmMatMalloc(Hpk1kHT->row, Hpk1kHT->col);
	KmAdd(Hpk1kHT, R, denom);

	Kmat* denominv = KmMatMalloc(denom->col, denom->row);
	KmInv(denom, denominv);

	Kmat* Kk = KmMatMalloc(mole->row, denominv->col);
	KmMul(mole, denominv, Kk);

    Kmat* Zobs_Zpre = KmMatMalloc(Zobs->row, Zobs->col);
	KmSub(Zobs, Zpre, Zobs_Zpre);

	Kmat* Kk_Zobs_Zpre = KmMatMalloc(Kk->row, Zobs_Zpre->col);
	KmMul(Kk, Zobs_Zpre, Kk_Zobs_Zpre);

	Kmat* quk1 = KmMatMalloc(qu_->row, qu_->col);
	KmAdd(qu_, Kk_Zobs_Zpre, quk1);

	Kmat* KkH = KmMatMalloc(Kk->row, H->col);
	KmMul(Kk, H, KkH);

    Kmat* KkHPk1k = KmMatMalloc(KkH->row, pk1k->col);
	KmMul(KkH, pk1k, KkHPk1k);

	Kmat* pk1 = KmMatMalloc(pk1k->row, pk1k->col);
	KmSub(pk1k, KkHPk1k, pk1);

    float quk1up[4] = {quk1->data[0][0], quk1->data[1][0], quk1->data[2][0], quk1->data[3][0]};
	_QuaterNormalize(quk1up);

	float eular[3];
	_Quater2Eular(quk1up, eular);

	angles->roll  = eular[0];
	angles->pitch = eular[1];
	angles->yaw   = eular[2];

    KmClone(pk1, kState->pk);
    for (int i = 0; i < 4; i++) kState->qu[i] = quk1up[i];

	KmMatFree(q);
	KmMatFree(phi);
	KmMatFree(qu_);
	KmMatFree(Zobs);
	KmMatFree(H);
	KmMatFree(Zpre);
	KmMatFree(phi_pk);
	KmMatFree(phiT);
	KmMatFree(phi_pk_pi);
	KmMatFree(pk1k);
	KmMatFree(HT);
	KmMatFree(mole);
	KmMatFree(Hpk1k);
	KmMatFree(Hpk1kHT);
	KmMatFree(denom);
	KmMatFree(denominv);
	KmMatFree(Kk);
	KmMatFree(Zobs_Zpre);
	KmMatFree(Kk_Zobs_Zpre);
	KmMatFree(quk1);
	KmMatFree(KkH);
	KmMatFree(KkHPk1k);
	KmMatFree(pk1);

	return;
}