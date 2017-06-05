/*
 * galua.c
 *
 *  Created on: Oct 30, 2013
 *      Author: Osechkin
 */

#include <stdlib.h>
#include <stdio.h>
#include "string.h"

#include "galua.h"


int GF_init(guint8 _pow, GF *gf)
{
    if (_pow > 8 || _pow < 2) return E_GF_RANGE;

    int gf_base = 1 << _pow;			// число элементов поля Галуа
    gf->power = _pow;

    gf->alpha = (guint8*)calloc(gf_base, sizeof(guint8));

    gf->index = (guint8*)calloc(gf_base, sizeof(guint8));

    memset(gf->alpha, 0x0, gf_base*sizeof(guint8));
    memset(gf->index, 0x0, gf_base*sizeof(guint8));

    gf->poly = (guint8*)calloc(gf->power+1, sizeof(guint8));

    if (gf->power == 4)
    {
        guint8 _p[] = {1, 1, 0, 0, 1};
        memcpy(gf->poly, &_p[0], (gf->power+1)*sizeof(guint8));
    }
    else if (gf->power == 8)
    {
        // порождающий полином согласно Стандарту ECMA-130: P(x) = x8 + x4 + x3 + x2 + 1
        guint8 _p[] = {1, 0, 1, 1, 1, 0, 0, 0, 1 };      // обратный порядок ! Т.е. p[0] = a0, p[1] = a1 и т.д.
        memcpy(gf->poly, &_p[0], (gf->power+1)*sizeof(guint8 ));
    }

    return E_GF_OK;
}

void GF_destroy(GF *gf)
{
    free(gf->alpha);
    free(gf->index);
    free(gf->poly);
}


void GF_generate(GF *gf)
{
    guint16 i;
    guint16 mask = 1;
    gf->alpha[gf->power] = 0;

    guint16 m = gf->power;
    for (i = 0; i < m; i++)
    {
        gf->alpha[i] = mask;
        gf->index[gf->alpha[i]] = i;

        if (gf->poly[i] != 0) gf->alpha[m] ^= mask;
        mask <<= 1;
    }

    gf->index[gf->alpha[m]] = m;
    mask >>= 1;

    int gf_base = 1 << gf->power;				// число элементов поля Галуа
    for (i = m+1; i < gf_base; i++)
    {
        if (gf->alpha[i-1] >= mask) gf->alpha[i] = gf->alpha[m] ^ ((gf->alpha[i-1]^mask)<<1);
        else gf->alpha[i] = gf->alpha[i-1]<<1;

        gf->index[gf->alpha[i]] = i;
    }
    gf->index[0] = 0;
}

guint16 GF_base(GF *gf)
{
    return (guint16)(1 << gf->power);
}

guint8 GF_add(guint8 a, guint8 b, GF *gf)
{
    return a ^ b;
}

guint8 GF_sub(guint8 a, guint8 b, GF *gf)
{
    return GF_add(a,b,gf);
}

guint8 GF_mul(guint8 a, guint8 b, GF *gf)
{
    if (a == 0 || b == 0) return 0;

    int gf_base = 1 << gf->power;		// число элементов поля Галуа
    int pow = gf->index[a] + gf->index[b];      // a = 2^index[a], b = 2^index[b]
    if (pow >= gf_base-1) pow %= gf_base-1;

    return gf->alpha[pow];
}

guint8 GF_div(guint8 a, guint8 b, GF *gf)
{
    if (a == 0) return 0;
    if (b == 0) return 0;                       // на ноль делить нельзя!

    int gf_base = 1 << gf->power;		// число элементов поля Галуа
    int pow = gf->index[a] - gf->index[b];      // a = 2^index[a], b = 2^index[b]
    if (pow < 0) pow += gf_base-1;		// приводим разность к модулю GF

    return gf->alpha[pow];
}

guint8 GF_log(guint8 a, GF *gf)
{
    if (a == 0) return 0;

    return gf->index[a];
}

guint8 GF_sqr(guint8 a, GF *gf)
{
    return GF_mul(a,a,gf);
}

guint8 GF_inv(guint8 a, GF *gf)
{
    return GF_div(1,a,gf);
}

guint8 GF_pow2(guint8 a, GF *gf)
{
    return gf->alpha[a];
}

guint8 GF_pow(guint8 pow, guint8 a, GF *gf)
{
    guint8 c = 1;
    guint16 i;
    for (i = 0; i < pow; i++)
    {
        c = GF_mul(c,a,gf);
    }

    return c;
}


// полиномы Галуа
GFPoly *GFPoly_alloc()
{	
    GFPoly *gf_poly = (GFPoly*)malloc(sizeof(GFPoly));
    gf_poly->data = 0;
    gf_poly->power = NoD;

    return gf_poly;
}

int GFPoly_init(guint8 pow, GFPoly *gf_poly)
{
    if (!gf_poly) return E_GFP_FATAL;

    if (pow == NoD)
    {
        gf_poly->data = 0;
        gf_poly->power = NoD;
        return E_GFP_NODATA;
    }

    gf_poly->data = (guint8*)calloc(pow+1, sizeof(guint8));
    //memset(gf_poly->data, 0x0, (pow+1)*sizeof(guint8));
    gf_poly->power = pow;

    return E_GFP_OK;
}

int GFPoly_set(guint8* arr, guint32 len, GFPoly *gf_poly)
{
    if (!arr) return E_GFP_FATAL;
    if (!gf_poly) return E_GFP_FATAL;
    if (len == 0) return E_GFP_NODATA;

    gf_poly->data = (guint8*)calloc(len, sizeof(guint8));
    memcpy(gf_poly->data, arr, len*sizeof(guint8));
    gf_poly->power = len-1;

    return E_GFP_OK;
}

int GFPoly_set_inv(guint8* arr, guint32 len, GFPoly *gf_poly)
{
    if (!arr) return E_GFP_FATAL;
    if (!gf_poly) return E_GFP_FATAL;
    if (len == 0) return E_GFP_NODATA;

    gf_poly->data = (guint8*)calloc(len, sizeof(guint8));
    guint16 i;
    for (i = 0; i < len; i++) gf_poly->data[i] = arr[len-i-1];
    gf_poly->power = len-1;

    return E_GFP_OK;
}

int GFPoly_self_inv(GFPoly *gf_poly)
{	
	if (!gf_poly) return E_GFP_FATAL;
	if (!gf_poly->data) return E_GFP_FATAL;
	if (!gf_poly->power == NoD) return E_GFP_NODATA;

	guint16 i;
	guint16 sz = gf_poly->power+1;
	for (i = 0; i < (guint16)(sz/2); i++) 
	{
		guint8 a = gf_poly->data[i];
		gf_poly->data[i] = gf_poly->data[sz-i-1];
		gf_poly->data[sz-i-1] = a;
	}
	
	return E_GFP_OK;
}

void GFPoly_destroy(GFPoly* gf_poly)
{
    if (!gf_poly) return;
    if (!gf_poly->data) return;

    free(gf_poly->data);
    gf_poly->data = 0;
    gf_poly->power = NoD;
}

void GFPoly_free(GFPoly *gf_poly)
{
    if (!gf_poly) return;

    free(gf_poly);
    gf_poly = 0;
}

guint8 GFPoly_deg(GFPoly *gf_poly)
{
    if (!gf_poly) return E_GFP_FATAL;
    else return gf_poly->power;
}

int GFPoly_add(GFPoly *gfp1, GFPoly *gfp2, GF *gf, GFPoly *dst)
{
	int res = GFPoly_add_elems(gfp1, gfp2, gf, dst);
	if (res == E_GFP_OK) res = GFPoly_reduce(dst);

	return res;
}

int GFPoly_add_elems(GFPoly *gfp1, GFPoly *gfp2, GF *gf, GFPoly *dst)
{
	if(!gfp1) return E_GFP_FATAL;
	if (!gfp1->data) return E_GFP_FATAL;
	if (gfp1->power == NoD) return E_GFP_NODATA;

	if(!gfp2) return E_GFP_FATAL;
	if (!gfp2->data) return E_GFP_FATAL;
	if (gfp2->power == NoD) return E_GFP_NODATA;

	if (!gf) return E_GFP_FATAL;

	guint8 u = gfp1->power;
	guint8 v = gfp2->power;
	guint8 max;
	if (u >= v) max = u; else max = v;

	if (dst->power != NoD) GFPoly_destroy(dst);
	dst->data = (guint8*)calloc(max+1, sizeof(guint8));
	dst->power = max;

	guint16 i;
	for (i = 0; i <= max; i++)
	{
		if (i > u) dst->data[i] = gfp2->data[i];
		else if (i > v) dst->data[i] = gfp1->data[i];
		else 
		{
			guint8 v = GF_add(gfp1->data[i], gfp2->data[i], gf);
			dst->data[i] = v;
		}
	}

	return E_GFP_OK;
}

int GFPoly_mul(GFPoly *gfp1, GFPoly *gfp2, GF *gf, GFPoly *dst)
{
    if(!gfp1) return E_GFP_FATAL;
    if (!gfp1->data) return E_GFP_FATAL;
    if (gfp1->power == NoD) return E_GFP_NODATA;

    if(!gfp2) return E_GFP_FATAL;
    if (!gfp2->data) return E_GFP_FATAL;
    if (gfp2->power == NoD) return E_GFP_NODATA;

    if (!gf) return E_GFP_FATAL;

    guint8 u = gfp1->power;
    guint8 v = gfp2->power;
    guint16 uv = u + v;
    if (uv > GF_base(gf)) return E_GFP_OVERFLOW;

    guint8 max = u + v;
    dst->data = (guint8*)calloc(max+1, sizeof(guint8));
    dst->power = max;

    guint16 i, j, k;
    for (k = 0; k <= max; k++)
    {
        guint8 sum = 0;
        for (i = 0; i <= u; i++)
        {
            for (j = 0; j <= v; j++)
            {
                if (i+j == k)
                {
                    sum = GF_add(sum, GF_mul(gfp1->data[i],gfp2->data[j],gf), gf);
                }
            }
        }
        dst->data[k] = sum;
    }
    int res = GFPoly_reduce(dst);

    return res;
}

int GFPoly_mulN(guint8 N, GFPoly *gfp1, GF *gf)
{
    if(!gfp1) return E_GFP_FATAL;
    if (!gfp1->data) return E_GFP_FATAL;
    if (gfp1->power == NoD) return E_GFP_NODATA;

    if (!gf) return E_GFP_FATAL;

    guint8 u = gfp1->power;

    guint16 i;
    for (i = 0; i <= u; i++)
    {
        gfp1->data[i] = GF_mul(gfp1->data[i], N, gf);
    }
    int res = GFPoly_reduce(gfp1);

    return res;
}

int GFPoly_div(GFPoly *a, GFPoly *g, GF *gf, GFPoly *q, GFPoly *r)
{
	if (a->data[2] == 100 && a->data[3] == 206 && a->data[4] == 0 && a->data[5] == 63)
	{
		int tt = 0;
	}
    if (!a) return E_GFP_FATAL;
    if (!a->data) return E_GFP_FATAL;
    if (a->power < 1 || a->power == NoD) return E_GFP_NODATA;

    if (!g) return E_GFP_FATAL;
    if (!g->data) return E_GFP_FATAL;
    if (g->power < 1 || g->power == NoD) return E_GFP_NODATA;

    if (!gf) return E_GFP_FATAL;

    guint8 u = a->power;
    guint8 m = g->power;

    GFPoly *q1 = GFPoly_alloc();
    GFPoly *q2 = GFPoly_alloc();
    GFPoly *r1 = GFPoly_alloc();
    GFPoly *r2 = GFPoly_alloc();
    GFPoly_init(0, q2);

    GFPoly_copy(r1, a);
    GFPoly_copy(q1, g);
    GFPoly_reduce(r1);
    GFPoly_reduce(q1);
	while (u-m >= 0)
    {
        guint8 p = u - m;
        guint8 y = GF_div(r1->data[u], q1->data[m], gf);
		
		GFPoly_shr(p, q1);
        GFPoly_mulN(y, q1, gf);
        GFPoly_add(q1,r1,gf,r2);

        //GFPoly_reduce(r2);
				
		GFPoly_copy(r1,r2);
		GFPoly_copy(q1,g);

        u = r1->power;
        m = q1->power;

        q2->data[0] = y;
        if (u-m >= 0) GFPoly_shr(1,q2);   
	}

    GFPoly_reduce(q2);
    GFPoly_reduce(r2);
    GFPoly_set(q2->data, q2->power+1, q);
    GFPoly_set(r2->data, r2->power+1, r);

    GFPoly_destroy(r1);
    GFPoly_destroy(r2);
    GFPoly_destroy(q1);
    GFPoly_destroy(q2);
    free(r1);
    free(r2);
    free(q1);
    free(q2);

    return E_GFP_OK;
}

int GFPoly_derivation(GFPoly *gfp1, GFPoly *gfp2)
{
    if (!gfp1) return E_GFP_FATAL;
    if (!gfp1->data) return E_GFP_FATAL;
    if (gfp1->power == NoD) return E_GFP_NODATA;
    if (!gfp2) return E_GFP_FATAL;

    /*guint32 pow = gfp1->power;
    GFPoly_init((pow+1)/2-1, gfp2);

    if (pow == 0) return E_GFP_OK;
    gfp2->data[0] = gfp1->data[1];
    if (pow < 3) return E_GFP_OK;

    guint32 i;
    guint8 c = 0;
    for (i = 3; i <= pow; i += 2)
    {
        c = gfp1->data[i];
        gfp2->data[i-1] = c;
        gfp2->data[i-2] = 0;
    }*/

    guint8 tau = gfp1->power;
    GFPoly_init(tau-1,gfp2);
    guint16 j;
    for (j = 0; j <= tau-1; j++)
    {
        guint8 c = ((j+1)%2 == 0 ? 0 : 1);
        gfp2->data[j] = c*gfp1->data[j+1];
    }

    int res = GFPoly_reduce(gfp2);

    return res;
}

int GFPoly_shr(guint8 N, GFPoly *gfp1)
{
    if (!gfp1) return E_GFP_FATAL;
    if (!gfp1->data) return E_GFP_FATAL;
    if (gfp1->power == NoD) return E_GFP_NODATA;
    if (N >= NoD) return E_GFP_OVERFLOW;

    int sz = gfp1->power+1;
    if (sz + N > NoD) return E_GFP_OVERFLOW;

    /*guint8 *z;
    z = (guint8*)calloc(sz, sizeof(guint8));
    memcpy(z, gfp1->data, sz*sizeof(guint8));

    GFPoly_destroy(gfp1);
    gfp1->data = (guint8*)calloc(sz+N, sizeof(guint8));
    memset(gfp1->data, 0x0, N*sizeof(guint8));
    memcpy(gfp1->data+N, z, sz*sizeof(guint8));
    gfp1->power = sz+N-1;

    free(z);*/

	guint8 *z;
	z = (guint8*)calloc(sz+N, sizeof(guint8));
	memcpy(z+N, gfp1->data, sz*sizeof(guint8));

	GFPoly_destroy(gfp1);
	gfp1->data = z;
	gfp1->power = sz+N-1;

    return E_GFP_OK;
}

int GFPoly_copy(GFPoly *dst, GFPoly *src)
{
    if (!dst) return E_GFP_FATAL;
    if (!src) return E_GFP_FATAL;
    if (!src->data) return E_GFP_FATAL;
    if (src->power == NoD) return E_GFP_NODATA;

	if (dst->power != NoD) GFPoly_destroy(dst);
    GFPoly_init(src->power,dst);
    memcpy(dst->data, src->data, (src->power+1)*sizeof(guint8));
    dst->power = src->power;

    return E_GFP_OK;
}

int GFPoly_concat(GFPoly *gfp1, GFPoly *gfp2, GF *gf, GFPoly *out)
{
    if (!gfp1) return E_GFP_FATAL;
    if (!gfp1->data) return E_GFP_FATAL;
    if (gfp1->power == NoD) return E_GFP_NODATA;

    if (!gfp2) return E_GFP_FATAL;
    if (!gfp2->data) return E_GFP_FATAL;
    if (gfp2->power == NoD) return E_GFP_NODATA;

    if (!out) return E_GFP_FATAL;

    guint8 len1 = gfp1->power+1;
    guint8 len2 = gfp2->power+1;
    guint16 sum = len1 + len2;
    if (sum >= GF_base(gf)) return E_GFP_OVERFLOW;

    out->data = (guint8*)calloc(sum, sizeof(guint8));
    out->power = sum-1;
    memcpy(out->data, gfp1->data, len1*sizeof(guint8));
    memcpy(out->data+len1, gfp2->data, len2*sizeof(guint8));

    return E_GFP_OK;
}

int GFPoly_reduce(GFPoly *gfp1)
{
    if (!gfp1) return E_GFP_FATAL;
    if (!gfp1->data) return E_GFP_FATAL;
    if (gfp1->power == NoD) return E_GFP_NODATA;

    guint8 hpow = NoD;
    guint16 i;
    for (i = 0; i <= gfp1->power; i++)
    {
        if (gfp1->data[i] > 0) hpow = i;
    }
    if (hpow == gfp1->power) return E_GFP_OK;
    else if (hpow == NoD)
    {
        free(gfp1->data);
        gfp1->data = (guint8*)calloc(1, sizeof(guint8));
        gfp1->data[0] = 0;
		gfp1->power = 0;

        return E_GFP_NODATA;
    }

    guint16 sz = hpow+1;
    guint8 *z;
    z = (guint8*)calloc(sz, sizeof(guint8));
    memcpy(z, gfp1->data, sz*sizeof(guint8));

    GFPoly_destroy(gfp1);
    gfp1->data = (guint8*)calloc(sz, sizeof(guint8));
    memcpy(gfp1->data, z, sz*sizeof(guint8));
    gfp1->power = hpow;
	
    free(z);

    return E_GFP_OK;
}

int GFPoly_genpoly(guint8 t, GF *gf, GFPoly *gfp)
{
    if (!gfp) return E_GFP_FATAL;
    if (gf->power == 4 && t > 7) return E_GFP_OVERFLOW;
    else if (gf->power == 8 && t > 127) return E_GFP_OVERFLOW;

    if (!gf) return E_GFP_FATAL;

    guint8 r = 2*t;
    //GFPoly *tmp1 = (GFPoly*)malloc(sizeof(GFPoly));
    GFPoly *tmp1 = GFPoly_alloc();
    GFPoly_init(r,tmp1);
    tmp1->data[0] = 1;

    //GFPoly *tmp2 = (GFPoly*)malloc(sizeof(GFPoly));
    GFPoly *tmp2 = GFPoly_alloc();

    //GFPoly *p = (GFPoly*)malloc(sizeof(GFPoly));
    GFPoly *p = GFPoly_alloc();
    GFPoly_init(2,p);

    // g(x) = П(x+beta*2^s)=(x+2)(x+4)(x+8)..., where beta = 2^(b-1) = 1 at b = 1; s = 1...r
    guint16 s;
    for (s = 1; s <= r; s++)
    {
        guint8 a = 2;
        guint16 i;
        for (i = 1; i < s; i++)
        {
            a = GF_mul(a,2,gf);
        }

        p->data[0] = a;
        p->data[1] = 1;

        GFPoly_mul(tmp1,p,gf,tmp2);
		GFPoly_destroy(tmp1);
        GFPoly_copy(tmp1,tmp2);

		GFPoly_destroy(tmp2);		
    }

    GFPoly_copy(gfp,tmp1);

    GFPoly_destroy(tmp1);
    GFPoly_destroy(tmp2);
    GFPoly_destroy(p);
    free(tmp1);
    free(tmp2);
    free(p);

    return E_GFP_OK;
}

int GFPoly_solve(GFPoly *a, GF *gf, GFPoly *out)
{
    if (!a) return E_GFP_FATAL;
    if (!a->data) return E_GFP_FATAL;
    if (a->power == NoD) return E_GFP_NODATA;
    if (!out) return E_GFP_FATAL;

    GFPoly_init(a->power, out);

    guint8 rt = 0;
    guint8 gf_base = (guint8)(GF_base(gf)-1);
    int cnt = 0;
    int res = E_GFP_NOTFOUND;

    do
    {
        rt++;
        int j;
        guint8 sum = 0;
        for (j = 0; j <= a->power; j++)
        {
            guint8 _pow = GF_pow(j, rt, gf);
            sum = GF_add(sum, GF_mul(a->data[j], _pow, gf), gf);
        }
        if (sum == 0)
        {
            out->data[cnt++] = rt;
            res = E_GFP_OK;
        }
    } while (rt < gf_base);

    res = GFPoly_reduce(out);

    return res;
}


void showGFPoly(GFPoly *gfp1)
{
    int i;
    for (i = 0; i <= gfp1->power; i++)
    {
        printf("res[%i] = %i\t\n", i, gfp1->data[i]);
    }
    printf("\t\n");
}
