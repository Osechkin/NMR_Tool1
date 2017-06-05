#include <stdlib.h>
#include <stdio.h>
#include "string.h"

#include "rscoding.h"


int RS_encode(GFPoly *a, GFPoly *g, GF *gf, GFPoly *out)
{
    if (!a) return E_RS_FATAL;
    if (!g) return E_RS_FATAL;
    if (!a->data) return E_RS_FATAL;
    if (!g->data) return E_RS_FATAL;
    if (a->power == NoD) return E_RS_FATAL;
    if (g->power == NoD) return E_RS_FATAL;
    if (!gf) return E_RS_FATAL;

    GFPoly *q = GFPoly_alloc();
    GFPoly *aa = GFPoly_alloc();
    int res = GFPoly_copy(aa, a);
    if (res != E_GFP_OK)
    {
        GFPoly_destroy(q);
        GFPoly_destroy(aa);
        free(q);
        free(aa);

        return E_RS_FATAL;
    }

    guint8 errs = GFPoly_deg(g); 
    res = GFPoly_shr(errs, aa);
    if (res != E_GFP_OK)
    {
        GFPoly_destroy(q);
        GFPoly_destroy(aa);
        free(q);
        free(aa);

        return E_RS_FATAL;
    }
    
    res = GFPoly_div(aa,g,gf,q,out);
	//res = E_RS_FATAL;
    if (res != E_GFP_OK)
    {
        GFPoly_destroy(q);
        GFPoly_destroy(aa);
        free(q);
        free(aa);

        return E_RS_FATAL;
    }

	if (out->power < (g->power-1))
	{
		/*guint8 new_sz = g->power;
		guint8 new_pos = g->power - out->power - 1;
		guint8 *np = (guint8*)calloc(new_sz, sizeof(guint8));
		memcpy(np+new_pos, out->data, (out->power+1)*sizeof(guint8));*/
		
		guint8 *np = (guint8*)calloc(g->power, sizeof(guint8));
		memcpy(np, out->data, (out->power+1)*sizeof(guint8));		

		GFPoly_destroy(out);		
		out->data = np;
		out->power = g->power-1;		
	}

    GFPoly_destroy(q);
    GFPoly_destroy(aa);
    free(q);
    free(aa);

    return E_RS_OK;
}

int RS_get_syndrome(GFPoly *a, guint8 g_pow, GF *gf, GFPoly *out)
{
    if (!a) return E_GFP_FATAL;
    if (!a->data) return E_GFP_FATAL;
    if (a->power == NoD) return E_GFP_NODATA;
    if (!gf) return E_GFP_FATAL;

    out->data = (guint8*)calloc(g_pow, sizeof(guint8));
    //memset(out->data, 0x0, (g_pow)*sizeof(guint8));
    out->power = g_pow-1;

    guint8 i, j;
    for (i = 0; i < g_pow; i++)
    {
        guint8 sum = 0;
        for (j = 0; j <= a->power; j++)
        {                        
            guint8 p0 = GF_pow2(j,gf);
            guint8 p1 = GF_pow(i+1, p0, gf);
			guint8 val = a->data[j];
            guint8 p2 = GF_mul(p1,val,gf);
            sum = GF_add(sum,p2,gf);
        }
        out->data[i] = sum;
    }

    return E_GFP_OK;
}

// Berlekamp–Massey algorithm
int RS_find_locator_poly(GFPoly *syn, guint8 g_pow, GF *gf, GFPoly *out)
{
    if (!syn) return E_GFP_FATAL;
    if (!syn->data) return E_GFP_FATAL;
    if (!gf) return E_GFP_FATAL;

    GFPoly *Lmda = (GFPoly*)malloc(sizeof(GFPoly));
    GFPoly_init(0,Lmda);
    Lmda->data[0] = 1;          // Ë(x) = 1

    GFPoly *B = (GFPoly*)malloc(sizeof(GFPoly));
    GFPoly_init(1,B);
    B->data[0] = 0;             // B(x) = x
    B->data[1] = 1;

    int L = 0;
    int qq = 0;
    int m = -1;

    int t = g_pow;
    
    while (qq < t)
    {
        guint8 i, j;
        guint8 sum = 0;
        for (i = 0; i <= L; i++)
        {
            sum = GF_add(sum, GF_mul(syn->data[qq-i],Lmda->data[i], gf), gf);
        }

        if (sum != 0)
        {
            GFPoly *Lmda_t = GFPoly_alloc();

            GFPoly *B_t = GFPoly_alloc();
			GFPoly_copy(B_t, B);

            GFPoly_mulN(sum, B_t, gf);
            GFPoly_add(Lmda, B_t, gf, Lmda_t);

            if (L < qq-m)
            {
                int L_t = qq - m;
                m = qq - L;
                L = L_t;

                GFPoly_destroy(B);
                GFPoly_destroy(B_t);
                GFPoly_copy(B_t, Lmda);

                guint8 invDq = GF_inv(sum, gf);
                GFPoly_mulN(invDq, B_t, gf);
                GFPoly_copy(B, B_t);
            }

            GFPoly_destroy(Lmda);
            GFPoly_copy(Lmda, Lmda_t);

            GFPoly_destroy(Lmda_t);
            free(Lmda_t);

            GFPoly_destroy(B_t);
            free(B_t);
        }

        GFPoly_shr(1, B);
        qq++;
    }

    int res = E_GFP_OK;
    if (Lmda->power != L) res = E_GFP_NOTFOUND;

    res = GFPoly_copy(out, Lmda);
    res = GFPoly_reduce(out);

	GFPoly_destroy(Lmda);
	GFPoly_destroy(B);
	free(Lmda);
	free(B);

    return res;
}

int RS_find_locators(GFPoly *a, GF *gf, GFPoly *out)
{
    if (!a) return E_GFP_FATAL;
    if (!a->data) return E_GFP_FATAL;
    if (!gf) return E_GFP_FATAL;
    if (!out) return E_GFP_FATAL;

    int res = GFPoly_solve(a, gf, out);   
    if (res != E_GFP_OK) return res;

    int i;
    for (i = 0; i <= out->power; i++)
    {
        guint8 loc = GF_inv(out->data[i], gf);
        loc = GF_log(loc, gf);
        if (loc == 255) loc = 0;
        out->data[i] = loc;
    }

    return E_GFP_OK;
}

int RS_errs_recover(GFPoly *syn, GFPoly *locs, guint8 a_pow, GF *gf, GFPoly *out)
{
    if (!syn) return E_GFP_FATAL;
    if (!locs) return E_GFP_FATAL;
    if (!syn->data) return E_GFP_FATAL;
    if (!locs->data) return E_GFP_FATAL;
    if (syn->power == NoD) return E_GFP_NODATA;
    if (locs->power == NoD) return E_GFP_NODATA;
    if (!gf) return E_GFP_FATAL;

    GFPoly *Lcs = GFPoly_alloc();
    int res = RS_find_locators(locs, gf, Lcs);
    if (res != E_GFP_OK)
    {
        GFPoly_destroy(Lcs);
        free(Lcs);

        return res;
    }
    if (Lcs->power == NoD)
    {
        GFPoly_destroy(Lcs);
        free(Lcs);

        return E_GFP_NODATA;
    }

    guint8 j;
    guint8 max = 0;
    for (j = 0; j <= Lcs->power; j++) if (Lcs->data[j] > max) max = Lcs->data[j];
    if (max > a_pow)
    {
        GFPoly_destroy(Lcs);
        free(Lcs);

        return E_GFP_BADDATA;
    }

    guint8 q, i;
    guint8 tau = locs->power;
    GFPoly *Omega = GFPoly_alloc();
    GFPoly_init(tau-1, Omega);

    for (q = 0; q <= tau-1; q++)
    {
        guint8 sum = 0;
        for (i = 0; i <= q; i++)
        {
            guint8 prod = GF_mul(locs->data[i], syn->data[q-i], gf);
            sum = GF_add(sum, prod, gf);
        }
        Omega->data[q] = sum;
    }

    res = GFPoly_reduce(Omega);
    if (res != E_GFP_OK)
    {
        GFPoly_destroy(Lcs);
        GFPoly_destroy(Omega);
        free(Lcs);
        free(Omega);

        return res;
    }

    GFPoly *derLc = GFPoly_alloc();
    res = GFPoly_derivation(locs, derLc);
    if (res != E_GFP_OK)
    {
        GFPoly_destroy(Lcs);
        GFPoly_destroy(Omega);
        GFPoly_destroy(derLc);
        free(Lcs);
        free(Omega);
        free(derLc);

        return res;
    }

    guint8 Lpow = Lcs->power;
    guint8 opow = Omega->power;
    guint8 dLpow = derLc->power;

    if (a_pow < Lpow)
    {
        GFPoly_destroy(Lcs);
        GFPoly_destroy(Omega);
        GFPoly_destroy(derLc);
        free(Lcs);
        free(Omega);
        free(derLc);

        return E_GFP_BADDATA;
    }
    GFPoly_init(a_pow, out);

    if (opow == NoD)
    {
        GFPoly_destroy(Lcs);
        GFPoly_destroy(Omega);
        GFPoly_destroy(derLc);
        free(Lcs);
        free(Omega);
        free(derLc);

        return E_GFP_NODATA;
    }
    for (i = 0; i <= Lpow; i++)
    {
        guint8 rti = Lcs->data[i];
        guint8 arg = GF_pow2(rti,gf);
        arg = GF_inv(arg,gf);

        guint8 sum1 = 0;
        for (j = 0; j <= opow; j++)
        {
            guint8 tmp = GF_pow(j,arg,gf);
            tmp = GF_mul(Omega->data[j],tmp,gf);
            sum1 = GF_add(sum1,tmp,gf);
        }

        guint8 sum2 = 0;
        for (j = 0; j <= dLpow; j++)
        {
            guint8 tmp = GF_pow(j,arg,gf);
            tmp = GF_mul(derLc->data[j],tmp,gf);
            sum2 = GF_add(sum2,tmp,gf);
        }

        if (sum2 == 0)
        {
            GFPoly_destroy(Lcs);
            GFPoly_destroy(Omega);
            GFPoly_destroy(derLc);
            free(Lcs);
            free(Omega);
            free(derLc);

            return E_GFP_BADDATA;
        }
        guint8 rs = GF_div(sum1,sum2,gf);
        guint8 index = Lcs->data[i];
        out->data[index] = rs;
    }

    GFPoly_destroy(Lcs);
    GFPoly_destroy(Omega);
    GFPoly_destroy(derLc);
    free(Lcs);
    free(Omega);
    free(derLc);

    return E_GFP_OK;
}

int RS_decode(GFPoly *in, GFPoly *g, GF *gf, GFPoly *out)
{
    GFPoly *syn = GFPoly_alloc();
    int res = RS_get_syndrome(in, g->power, gf, syn);
    if (res != E_GFP_OK)
    {
        GFPoly_destroy(syn);
        free(syn);

        if (res == E_GFP_FATAL) return E_RS_FATAL;
        else return E_RS_NOTFOUND;
    }
    if (syn->power < 1) 
	{
		GFPoly_destroy(syn);
		free(syn);

		return E_RS_NOTFOUND;
	}
	
	out->data = (guint8*)calloc(in->power+1, sizeof(guint8));
	out->power = in->power;

	guint8 i;
	guint8 ok = 0;
	for (i = 0; i <= syn->power; i++) if (syn->data[i] != 0) ok = 1;
	if (ok == 0) 
	{
		memcpy(out->data, in->data, (in->power+1)*sizeof(guint8));

		GFPoly_destroy(syn);
		free(syn);

		return E_RS_OK;
	}

    GFPoly *Lc = GFPoly_alloc();
    res = RS_find_locator_poly(syn, g->power, gf, Lc);
    if (res != E_GFP_OK)
    {
        GFPoly_destroy(syn);
        GFPoly_destroy(Lc);
        free(syn);
        free(Lc);

        if (res == E_GFP_FATAL) return E_RS_FATAL;
        else return E_RS_NOTFOUND;
    }

    GFPoly *locs = GFPoly_alloc();
    res = RS_find_locators(Lc, gf, locs);
    if (res != E_GFP_OK)
    {
        GFPoly_destroy(syn);
        GFPoly_destroy(Lc);
        GFPoly_destroy(locs);
        free(syn);
        free(Lc);
        free(locs);

        if (res == E_GFP_FATAL) return E_RS_FATAL;
        else return E_RS_NOTFOUND;
    }

    GFPoly *E = GFPoly_alloc();
    res = RS_errs_recover(syn, Lc, in->power, gf, E);
    if (res != E_GFP_OK)
    {
        GFPoly_destroy(syn);
        GFPoly_destroy(Lc);
        GFPoly_destroy(locs);
        GFPoly_destroy(E);
        free(syn);
        free(Lc);
        free(locs);
        free(E);

        if (res == E_GFP_FATAL) return E_RS_FATAL;
        else return E_RS_NOTFOUND;
    }

    GFPoly_add_elems(in,E,gf,out);
    if (res != E_GFP_OK)
    {
        GFPoly_destroy(syn);
        GFPoly_destroy(Lc);
        GFPoly_destroy(locs);
        GFPoly_destroy(E);
        free(syn);
        free(Lc);
        free(locs);
        free(E);

        if (res == E_GFP_FATAL) return E_RS_FATAL;
        else return E_RS_NOTFOUND;
    }

    GFPoly_destroy(syn);
    GFPoly_destroy(Lc);
    GFPoly_destroy(locs);
    GFPoly_destroy(E);
    free(syn);
    free(Lc);
    free(locs);
    free(E);

    return E_RS_OK;
}
