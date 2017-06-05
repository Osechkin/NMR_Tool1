/*
 * galua.h
 *
 *  Created on: Oct 30, 2013
 *      Author: Osechkin
 */

#ifndef GALUA_H_
#define GALUA_H_

#include "rscoding_common.h"
//#include "stdint.h"

typedef struct
{     
    guint8	power;          // число бит на один элемент поля Галуа GF(2^power)
    guint8* alpha;
    guint8* index;
    guint8* poly;           // порождающий полином
} GF;

typedef struct
{
    guint8*   data;           // коэффициенты полинома
    guint32   power;          // степень полинома (= число его коэффициентов-1)    
} GFPoly;


// Galois field 
int GF_init(guint8 _pow, GF *gf);
void GF_destroy(GF *gf);
void GF_generate(GF *gf);
guint16 GF_base(GF *gf);
guint8 GF_add(guint8 a, guint8 b, GF *gf);
guint8 GF_sub(guint8 a, guint8 b, GF *gf);
guint8 GF_mul(guint8 a, guint8 b, GF *gf);
guint8 GF_div(guint8 a, guint8 b, GF *gf);
guint8 GF_log(guint8 a, GF *gf);
guint8 GF_sqr(guint8 a, GF *gf);
guint8 GF_inv(guint8 a, GF *gf);
guint8 GF_pow2(guint8 a, GF *gf);
guint8 GF_pow(guint8 pow, guint8 a, GF *gf);


// Galois polynome
GFPoly *GFPoly_alloc();
int GFPoly_init(guint8 pow, GFPoly *gf_poly);
int GFPoly_set(guint8* arr, guint32 len, GFPoly *gf_poly);
int GFPoly_set_inv(guint8* arr, guint32 len, GFPoly *gf_poly);
int GFPoly_self_inv(GFPoly *gf_poly);
void GFPoly_destroy(GFPoly* gf_poly);
void GFPoly_free(GFPoly *gf_poly);
guint8 GFPoly_deg(GFPoly *gf_poly);
int GFPoly_add(GFPoly *gfp1, GFPoly *gfp2, GF *gf, GFPoly *dst);
int GFPoly_add_elems(GFPoly *gfp1, GFPoly *gfp2, GF *gf, GFPoly *dst);
int GFPoly_mul(GFPoly *gfp1, GFPoly *gfp2, GF *gf, GFPoly *dst);
int GFPoly_div(GFPoly *a, GFPoly *g, GF *gf, GFPoly *q, GFPoly *r);
int GFPoly_mulN(guint8 N, GFPoly *gfp1, GF *gf);
int GFPoly_derivation(GFPoly *gfp1, GFPoly *gfp2);
int GFPoly_shr(guint8 N, GFPoly *gfp1);
int GFPoly_copy(GFPoly *dst, GFPoly *src);
int GFPoly_concat(GFPoly *gfp1, GFPoly *gfp2, GF *gf, GFPoly *out);
int GFPoly_reduce(GFPoly *gfp1);
int GFPoly_genpoly(guint8 t, GF *gf, GFPoly *gfp);
int GFPoly_solve(GFPoly *a, GF *gf, GFPoly *out);

void showGFPoly(GFPoly *gfp1);

#endif /* GALUA_H_ */
