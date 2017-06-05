#ifndef GF_DATA_GENERAL_H
#define GF_DATA_GENERAL_H


#include <stdlib.h>

#include "rscoding.h"
#include "galua.h"


typedef struct 
{
	GF*			gf;			// ���� �����
	GFPoly**	gf_polys;	// ������ ���������� �� ����������� �������� ������� �� 1 �� 127	
	int			index;		// ����� �������� ������������ �������� � ��������� gf_polys
	int			index_hdr;	// ����� �������� ������������ �������� � ��������� gf_polys ��� ����������� ��������� ������������ ���������
	int			index_body;	// ����� �������� ������������ �������� � ��������� gf_polys ��� ����������� ������ � ���� ������������ ���������
	int			index_ftr;	// ����� �������� ������������ �������� � ��������� gf_polys ��� ����������� �������� ������������ ���������
	int			size;		// ���������� ��������� ������� gf_polys
} GF_Data;


void gfdata_init(GF_Data *gf_data, int sz);
void gfdata_destroy(GF_Data *gf_data);


#endif // GF_DATA_GENERAL_H
