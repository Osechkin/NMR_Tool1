#ifndef RSCODING_H
#define RSCODING_H

#include "galua.h"
#include "rscoding_common.h"


int RS_encode(GFPoly *a, GFPoly *g, GF *gf, GFPoly *out);

int RS_get_syndrome(GFPoly *a, guint8 g_pow, GF *gf, GFPoly *out);

// To find a polynome of locators.
// Input parameters:
// - syn is error syndrome polynome;
// - g_pow is max errors which can be found (= a half of regenerative polynome power) ;
// - gf is Glois field;
// Output parameters:
// - out is the locator polynome;
// Returns error code: E_RS_OK, E_RS_FATAL or E_RS_NOTFOUND (if locators were not found)
int RS_find_locator_poly(GFPoly *syn, guint8 g_pow, GF *gf, GFPoly *out);

// To find error locators
int RS_find_locators(GFPoly *a, GF *gf, GFPoly *out);

// Recover error vector for distorted values
int RS_errs_recover(GFPoly *syn, GFPoly *locs, guint8 a_pow, GF *gf, GFPoly *out);

// Decode encoded and distorted message
// Input parameters:
// - in is input message (initial distorted message loaded from UART or other port)
// - g is generative polynome
// - gf is Galois Field
// Output parameters:
// - out is output decoded
// Returns error code: E_RS_OK, E_RS_FATAL or E_RS_NOTFOUND (if cannot decode)
int RS_decode(GFPoly *in, GFPoly *g, GF *gf, GFPoly *out);

#endif // RSCODING_H
