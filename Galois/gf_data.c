#include "gf_data.h"


void gfdata_init(GF_Data *gf_data, int sz)
{	
	GF *gf = (GF*)malloc(sizeof(GF));
	GF_init(8, gf);
	GF_generate(gf);
	gf_data->gf = gf;
	
	if (sz > 0) gf_data->gf_polys = (GFPoly**)calloc(sz, sizeof(GFPoly*));	
	int i;
	for (i = 0; i < sz; i++)
	{					
		gf_data->gf_polys[i] = GFPoly_alloc(); 
		guint8 errs = (guint8)(i+1);
		GFPoly_genpoly(errs, gf, gf_data->gf_polys[i]);		
	}
	
	gf_data->index = 0;	
	gf_data->index_hdr = 0;
	gf_data->index_body = 0;
	gf_data->index_ftr = 0;

	gf_data->size = sz;
}

void gfdata_destroy(GF_Data *gf_data)
{
	GF_destroy(gf_data->gf);
	free(gf_data->gf);

	int i;
	if (gf_data->size > 0) for (i = 0; i < gf_data->size; i++)
	{
		GFPoly_destroy(gf_data->gf_polys[i]);
		free(gf_data->gf_polys[i]);
	}

	free(gf_data->gf_polys);	
}
