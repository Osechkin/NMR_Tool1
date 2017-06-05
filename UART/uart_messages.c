#include "stdint.h"
#include "uart_messages.h"
#include "../Galois/gf_data.h"


uint8_t Crc8(uint8_t *pcBlock, uint16_t len)
{
	uint8_t crc = 0xFF; //исходное значение
	uint16_t i;

	if (len < 0) return crc;
	for(i = 0; i < len; i++) crc ^= pcBlock[i];

	return crc;
}

uint16_t Crc16(uint8_t *pcBlock, uint16_t len)
{
    uint16_t crc = 0xFFFF; //исходное значение
    uint16_t i;

    while( len-- )
    {
        crc ^= *pcBlock++ << 8;
        for( i = 0; i < 8; i++ )
            crc = crc & 0x8000 ? ( crc << 1 ) ^ 0x1021 : crc << 1; // Gx = 0x1021 = 1000000100001
    }

    return crc;//вернуть остаток
}


uint8_t mod8(uint8_t a, uint8_t b)
{
	if (b == 0) return 0xFF;
	return a - (a/b)*b;
}


void initMsgHeader(MsgHeader* _msg_header)
{
	_msg_header = (MsgHeader*)malloc(sizeof(MsgHeader));
	clearMsgHeader(_msg_header);
}

void clearMsgHeader(MsgHeader *_msg_header)
{
	_msg_header->msg_type = 0;
	_msg_header->reader = 0;
	_msg_header->writer = 0;
	_msg_header->id = 0;
	_msg_header->pack_count = 0;
	_msg_header->pack_len = 0;
	_msg_header->block_len = 0;
	_msg_header->rec_errs = 0;
	_msg_header->crc = 0;
	memset(&_msg_header->rec_data[0], 0x0, HEAD_REC_LEN);
	memset(&_msg_header->data[0], 0x0, SRV_DATA_LEN);
}

int findMsgHeader(QUEUE8 *_queue, MsgHeader *_msg_header, GF_Data *_gf_data)
{
	int cnt = QUEUE8_count(_queue);
	if (cnt > (HEADER_LEN))
	{
		QUEUE8_clear(_queue);
		return E_RS_LEN;
	}
	if (cnt != HEADER_LEN)  return E_RS_LEN;

	int sz = HEADER_LEN;
	uint8_t arr[HEADER_LEN];

	int i;
	for (i = 0; i < sz; i++)
	{
		uint8_t ch = QUEUE8_at(i, _queue);
		arr[i] = ch;
	}

	GFPoly *dist = GFPoly_alloc();
	dist->data = &arr[0];
	dist->power = sz-1;

	GF *gf = _gf_data->gf;
	int index = _gf_data->index_hdr;
	GFPoly *g = _gf_data->gf_polys[index];

	GFPoly *dec = GFPoly_alloc();
	GFPoly_self_inv(dist);
	int res = RS_decode(dist,g,gf,dec);
	if (res == E_RS_FATAL || res == E_RS_NOTFOUND)
	{
		if (dec->power != NoD) GFPoly_destroy(dec);
		free(dist);
		free(dec);

		return res;
	}

	GFPoly_self_inv(dec);
	_msg_header->msg_type	= dec->data[0];
	_msg_header->writer		= (dec->data[1] & 0x0F);
	_msg_header->reader		= (dec->data[1] & 0xF0) >> 4;
	_msg_header->id			= dec->data[2];
	_msg_header->pack_count	= dec->data[3];
	_msg_header->pack_len	= dec->data[4];
	_msg_header->block_len	= dec->data[5];
	_msg_header->rec_errs	= dec->data[6];
	_msg_header->crc		= dec->data[7];

	memcpy(&_msg_header->data[0], dec->data+3, SRV_DATA_LEN);
	memcpy(&_msg_header->rec_data[0], dec->data+HEAD_INFO_LEN, HEAD_REC_LEN);

	GFPoly_destroy(dec);
	free(dist);
	free(dec);

	return res;
}

int checkMsgHeader(MsgHeader *_msg_header)
{
	// проверка типа сообщения на принадлежность к одному из известных типов
	uint8_t type = _msg_header->msg_type;
	if (type != MTYPE_SERVICE &&
		type != MTYPE_SHORT &&
		type != MTYPE_MULTYPACK &&
		type != MTYPE_PACKET)
	return E_MSG_TYPE;

	uint8_t arr[HEAD_INFO_LEN-1];

	if (type == MTYPE_MULTYPACK)			// обыкновенное многопакетное (от 1 и больше) сообщение
	{
		if ( _msg_header->rec_errs > MAX_REC_ERRS) return E_MSG_REC_LEN;				// количество исправляемых ошибок не может быть > MAX_REC_ERRS
		//if ( _msg_header->body_len == 0 && type != MTYPE_SERVICE ) return E_MSG_LEN;	// длина тела сообщения (не служебного!) должна быть > 0
		if ( 2*(_msg_header->rec_errs) >= _msg_header->pack_len ) return E_MSG_LEN;		// длина всего тела сообщения должна быть больше длины проверочной части

		arr[0] = _msg_header->msg_type;
		arr[1] = (_msg_header->writer & 0x0F) | ((_msg_header->reader & 0x0F) << 4);
		arr[2] = _msg_header->id;
		arr[3] = _msg_header->pack_count;
		arr[4] = _msg_header->pack_len;
		arr[5] = _msg_header->block_len;
		arr[6] = _msg_header->rec_errs;
	}
	else if (type == MTYPE_SERVICE)		// служебное сообщение
	{
		arr[0] = _msg_header->msg_type;
		arr[1] = (_msg_header->writer & 0x0F) | ((_msg_header->reader & 0x0F) << 4);
		arr[2] = _msg_header->id;
		int i;
		for (i = 0; i < SRV_DATA_LEN; i++) arr[3+i] = _msg_header->data[i];
	}
	else if (type == MTYPE_SHORT)		// короткое сообщение
	{
		arr[0] = _msg_header->msg_type;
		arr[1] = (_msg_header->writer & 0x0F) | ((_msg_header->reader & 0x0F) << 4);
		arr[2] = _msg_header->id;
		int i;
		for (i = 0; i < SRV_DATA_LEN; i++) arr[3+i] = _msg_header->data[i];
	}

	// проверка контрольной суммы заголовка сообщения
	uint8_t crc = Crc8(&arr[0],HEAD_INFO_LEN-1);
	if (crc != _msg_header->crc) return E_MSG_CRC;

	return E_MSG_OK;
}

void setMsgWriter(uint8_t addr, MsgHeader *_msg_header)
{
	addr = addr & 0x0F;
	_msg_header->writer = addr;
	uint8_t old_val = _msg_header->data[1];
	_msg_header->data[1] = (old_val & 0xF0) | addr;
}

void setMsgReader(uint8_t addr, MsgHeader *_msg_header)
{
	addr = addr & 0x0F;
	_msg_header->reader = addr;
	uint8_t old_val = _msg_header->data[1];
	_msg_header->data[1] = (old_val & 0x0F) | (addr << 4);
}

void makeMsgHeader(MsgHeader *_msg_header, GF_Data *_gf_data)
{
	GFPoly *rec_poly = GFPoly_alloc();
	GFPoly *arr_poly = GFPoly_alloc();
	GFPoly_init(HEADER_LEN - 1, arr_poly);

	arr_poly->data[0] = _msg_header->msg_type;
	arr_poly->data[1] = ((_msg_header->writer & 0x0F) << 4) | (_msg_header->reader & 0x0F);
	arr_poly->data[2] = _msg_header->id;
	if (_msg_header->msg_type == MTYPE_SERVICE || _msg_header->msg_type == MTYPE_SHORT)
	{
		arr_poly->data[3] = _msg_header->data[0];
		arr_poly->data[4] = _msg_header->data[1];
		arr_poly->data[5] = _msg_header->data[2];
		arr_poly->data[6] = _msg_header->data[3];
	}
	arr_poly->data[7] = Crc8(arr_poly->data, HEAD_INFO_LEN - 1);

	int g_num = _gf_data->index_hdr;

	RS_encode(arr_poly, _gf_data->gf_polys[g_num], _gf_data->gf, rec_poly);

	memcpy(&_msg_header->rec_data[0], rec_poly->data, HEAD_REC_LEN);

	GFPoly_destroy(arr_poly);
	GFPoly_destroy(rec_poly);
	free(arr_poly);
	free(rec_poly);
}



void initMsgPacket(MsgHeader *_msg_header, MsgPacket *_msg_pack, uint8_t pack_num)
{
	//_msg_pack = (MsgPacket*)malloc(sizeof(MsgPacket));
	clearMsgPacket(_msg_pack);

	_msg_pack->start_marker = MTYPE_PACKET;
	_msg_pack->msg_id = _msg_header->id;
	_msg_pack->packet_number = pack_num;
	_msg_pack->block_len = _msg_header->block_len;
	_msg_pack->rec_errs = _msg_header->rec_errs;
	_msg_pack->pack_len = _msg_header->pack_len;
	_msg_pack->crc = 0;
	_msg_pack->data_len = 0;
}

int findMsgPackets(QUEUE8 *_queue, UART_Message *_uart_msg, GF_Data *_gf_data)
{
	MsgHeader *_msg_header = _uart_msg->msg_header;

	int pack_len = (int)_msg_header->pack_len;
	int pack_count = (int)_msg_header->pack_count;
	int block_len = (int)_msg_header->block_len;
	int rec_errs = (int)_msg_header->rec_errs;
	int len = block_len - 2*rec_errs;				// длина кодируемых данных в каждом РС-блоке

	int data_size = QUEUE8_count(_queue);
	if (data_size != pack_len*pack_count)
	{
		QUEUE8_clear(_queue);
		return E_RS_LEN;
	}
	if (len <= 0)
	{
		QUEUE8_clear(_queue);
		return E_RS_FATAL;
	}

	GF *gf = _gf_data->gf;
	int index = _gf_data->index_body;
	GFPoly *g = _gf_data->gf_polys[index];

	uint8_t pack_number = 0;
	while (QUEUE8_count(_queue) > 0)
	{
		MsgPacket *pack = (MsgPacket*)malloc(sizeof(MsgPacket));
		//MsgPacket *pack = _uart_msg->msg_packs[pack_number];
		initMsgPacket(_msg_header, pack, ++pack_number);
		uint8_t *data_arr = &pack->data[0];
		uint8_t *data_cnt = &pack->data_len;

		int cnt = 0;
		uint16_t pack_byte_counter = 0;
		uint8_t block_counter = 0;
		while (pack_byte_counter < pack_len && QUEUE8_count(_queue) > 0)
		{
			GFPoly *dist = GFPoly_alloc();
			dist->data = (guint8*)calloc(block_len, sizeof(guint8));
			uint8_t block_byte_counter = 0;
			while (block_byte_counter < block_len && QUEUE8_count(_queue) > 0)
			{
				uint8_t ch = QUEUE8_get(_queue);
				//byte_arr[pack_byte_counter++] = ch; 	// не будем сохранять исходные закодированные данные
				pack_byte_counter++;				 	// добавлено вместо строки выше
				dist->data[block_byte_counter] = ch;
				dist->power = block_byte_counter++;
			}

			if ((++block_counter) > (pack_len/block_len)) return E_RS_FATAL; // Ошибка ! Число блоков в пакете оказалось больше, чем объявлено в _msg_header
			if (block_byte_counter == block_len)
			{
				GFPoly *dec = GFPoly_alloc();
				GFPoly_self_inv(dist);
				int res = RS_decode(dist,g,gf,dec);
				if (res == E_RS_OK)
				{
					GFPoly_self_inv(dec);
					memcpy(data_arr+cnt, dec->data, len*sizeof(uint8_t));
					*data_cnt += len;
				}
				else
				{
					GFPoly_destroy(dist);
					GFPoly_destroy(dec);
					free(dist);
					free(dec);
					free(pack);
					return E_RS_FATAL;	// Ошибка ! Не удалось успешно декодировать блок данных
				}

				cnt += len;

				GFPoly_destroy(dec);
				free(dec);
			}

			GFPoly_destroy(dist);
			free(dist);
		}

		uint8_t crc = pack->data[*data_cnt-1];
		pack->crc = crc;
		_uart_msg->msg_packs[pack_number-1] = pack;
		_uart_msg->pack_cnt = pack_number;
	}

	if (pack_number != _uart_msg->pack_cnt) return E_RS_FATAL;
	else return E_RS_OK;
}

int findMsgPackets2(BUFFER8 *_buff, UART_Message *_uart_msg, GF_Data *_gf_data)
{
	MsgHeader *_msg_header = _uart_msg->msg_header;

	int pack_len = (int)_msg_header->pack_len;
	int pack_count = (int)_msg_header->pack_count;
	int block_len = (int)_msg_header->block_len;
	int rec_errs = (int)_msg_header->rec_errs;
	int len = block_len - 2*rec_errs;				// длина кодируемых данных в каждом РС-блоке

	int data_size = BUFFER8_count(_buff);
	if (data_size != pack_len*pack_count)
	{
		BUFFER8_clear(_buff);
		return E_RS_LEN;
	}
	if (len <= 0)
	{
		BUFFER8_clear(_buff);
		return E_RS_FATAL;
	}

	GF *gf = _gf_data->gf;
	int index = _gf_data->index_body;
	GFPoly *g = _gf_data->gf_polys[index];

	uint8_t pack_number = 0;
	while (BUFFER8_count(_buff) > 0)
	{
		MsgPacket *pack = (MsgPacket*)malloc(sizeof(MsgPacket));
		//MsgPacket *pack = _uart_msg->msg_packs[pack_number];
		initMsgPacket(_msg_header, pack, ++pack_number);
		uint8_t *data_arr = &pack->data[0];
		uint8_t *data_cnt = &pack->data_len;

		int cnt = 0;
		uint16_t pack_byte_counter = 0;
		uint8_t block_counter = 0;
		while (pack_byte_counter < pack_len && BUFFER8_count(_buff) > 0)
		{
			GFPoly *dist = GFPoly_alloc();
			dist->data = (guint8*)calloc(block_len, sizeof(guint8));
			uint8_t block_byte_counter = 0;
			while (block_byte_counter < block_len && BUFFER8_count(_buff) > 0)
			{
				uint8_t ch = BUFFER8_get(_buff);
				//byte_arr[pack_byte_counter++] = ch; 	// не будем сохранять исходные закодированные данные
				pack_byte_counter++;				 	// добавлено вместо строки выше
				dist->data[block_byte_counter] = ch;
				dist->power = block_byte_counter++;
			}

			if ((++block_counter) > (pack_len/block_len)) return E_RS_FATAL; // Ошибка ! Число блоков в пакете оказалось больше, чем объявлено в _msg_header
			if (block_byte_counter == block_len)
			{
				GFPoly *dec = GFPoly_alloc();
				GFPoly_self_inv(dist);
				int res = RS_decode(dist,g,gf,dec);
				if (res == E_RS_OK)
				{
					GFPoly_self_inv(dec);
					memcpy(data_arr+cnt, dec->data, len*sizeof(uint8_t));
					*data_cnt += len;
				}
				else
				{
					GFPoly_destroy(dist);
					GFPoly_destroy(dec);
					free(dist);
					free(dec);
					free(pack);
					return E_RS_FATAL;	// Ошибка ! Не удалось успешно декодировать блок данных
				}

				cnt += len;

				GFPoly_destroy(dec);
				free(dec);
			}

			GFPoly_destroy(dist);
			free(dist);
		}

		uint8_t crc = pack->data[*data_cnt-1];
		pack->crc = crc;
		_uart_msg->msg_packs[pack_number-1] = pack;
		_uart_msg->pack_cnt = pack_number;
	}

	if (pack_number != _uart_msg->pack_cnt) return E_RS_FATAL;
	else return E_RS_OK;
}

int checkMsgPackets(UART_Message *_uart_msg)
{
	MsgHeader *_msg_header = _uart_msg->msg_header;

	int pack_count = _uart_msg->pack_cnt;
	if (pack_count != _msg_header->pack_count) return E_MSG_LEN;

	int i;
	for (i = 0; i < pack_count; i++)
	{
		MsgPacket *pack = _uart_msg->msg_packs[i];

		uint8_t type = pack->data[0];
		if (type != MTYPE_PACKET) return E_MSG_TYPE;

		uint8_t msg_id = pack->data[1];
		if (msg_id != _msg_header->id) return E_MSG_ID;

		//uint16_t pack_len = (uint16_t)_msg_header->pack_len;
		uint16_t data_len = (uint16_t)pack->data_len;
		uint8_t crc = pack->crc;
		uint8_t calc_crc = Crc8(&pack->data[0], data_len-1);
		if (crc != calc_crc) return E_MSG_CRC;
	}

	return E_MSG_OK;
}

void clearMsgPacket(MsgPacket *_msg_pack)
{
	_msg_pack->start_marker = 0;
	_msg_pack->msg_id = 0;
	_msg_pack->packet_number = 0;
	_msg_pack->crc = 0;
	memset(&_msg_pack->data[0], 0x0, MAX_PACK_LEN*sizeof(uint8_t));

	_msg_pack->block_len = 0;
	_msg_pack->rec_errs = 0;
	_msg_pack->data_len = 0;
	_msg_pack->pack_len = 0;
}

void deleteMsgPackets(UART_Message *_uart_msg)
{
	int i;
	for (i = 0; i < _uart_msg->pack_cnt; i++)
	{
		free(_uart_msg->msg_packs[i]);
	}
	_uart_msg->pack_cnt = 0;
	_uart_msg->msg_header->pack_count = 0;
	//_uart_msg->msg_header = 0;
}

void calcRecoveryPart(uint8_t *src, uint8_t *dst, MsgPacket *_msg_pack, GF_Data *_gf_data)
{
	uint8_t block_len = _msg_pack->block_len;
	uint8_t errs_count = _msg_pack->rec_errs;

	GFPoly *a = GFPoly_alloc();
	uint8_t len = block_len-2*errs_count;
	GFPoly_init(len-1, a);
	memcpy(a->data, src, len*sizeof(uint8_t));
	GFPoly_self_inv(a);

	GFPoly *r = GFPoly_alloc();
	GF *gf = _gf_data->gf;
	int index = _gf_data->index_body;
	GFPoly *g = _gf_data->gf_polys[index];

	int res = RS_encode(a, g, gf, r);

	/*uint8_t rr[255];
	memset(&rr[0], 0x0, sizeof(uint8_t)*255);
	int i;
	for (i = 0; i < block_len; i++) rr[i] = r->data[i];*/

	GFPoly_self_inv(r);

	memcpy(dst, r->data, 2*errs_count*sizeof(uint8_t));

	GFPoly_destroy(a);
	GFPoly_destroy(r);
	free(a);
	free(r);
}

int estimateBestPackLen(int data_len, int block_len, int rs_part_len)
{
	/*int pack_count = 1;
	int max_len = 0;
	Bool ready = False;
	int res = 254;
	while (!ready)
	{
		int full_data_len = data_len + PACK_HEAD_LEN + 1;
		float full_len = ceil(full_data_len/(float)block_len)*rs_part_len + full_data_len;
		int opt_pack_len = (int)(ceil(ceil(full_len/pack_count)/block_len)*block_len);

		if (opt_pack_len > 254) pack_count++;
		else
		{
			int final_len = opt_pack_len*pack_count;
			if (max_len >= final_len)
			{
				max_len = final_len;
				pack_count++;
			}
			else
			{
				res = opt_pack_len;
				ready = True;
			}
		}
	}*/

	int res = (254/block_len)*block_len;
	int N = 1;
	//int data_all = 0x7FFFFFFF;	// максимально возможное целое цисло (для sizeof(int) = 4)
	int data_all_min = 0x7FFFFFFF;	// максимально возможное целое цисло (для sizeof(int) = 4)
	int res_opt = res;

	Bool ready = False;
	int index = 0;
	while (ready == False)
	{
		int blocks = (data_len + N*(PACK_HEAD_LEN+1)) / (block_len - rs_part_len);
		int q = (data_len + N*(PACK_HEAD_LEN+1)) % (block_len - rs_part_len);
		if (q > 0) blocks++;

		int blocks_inpack = blocks/N;
		q = blocks % N;
		if (q > 0) blocks_inpack++;

		res = block_len*blocks_inpack;
		if (res <= 254)
		{
			index++;
			if (res*N < data_all_min)
			{
				data_all_min = res*N;
				res_opt = res;
			}
			if (index >= 5) ready = True;
		}
		/*if (res*N > data_max && res <= 240)
		{
			if (data_max/(N-1) < 240) res = data_max/(N-1);
			ready = True;
		}
		else data_max = res*N;*/
		//data_max = res*N;

		N++;
	}

	res = res_opt;
	if (res%block_len > 0) res = 240;

	return res;
}

void pushDataToMsgPacket(uint8_t *data, uint16_t data_len, uint16_t *pos, MsgPacket *_msg_pack, GF_Data *_gf_data)
{
	uint8_t arr[MAX_PACK_LEN];
	uint8_t pack_len = _msg_pack->pack_len;										// длина пакета
	memset(&arr[0], 0x0, sizeof(uint8_t)*pack_len);

	uint16_t dpos = *pos;														// позиция, с которой забираются данные из data

	uint8_t block_len = _msg_pack->block_len;									// длина одного блока
	uint8_t block_cnt = pack_len/block_len;										// кол-во блоков в пакете
	uint8_t data_block_len = block_len - 2*(_msg_pack->rec_errs);				// длина данных в одном блоке
	uint16_t full_pdata_len = data_block_len * block_cnt - PACK_SRV_LEN;		// длина всех данных в пакете
	uint16_t rem_data_len = data_len - dpos;									// длина данных от позиции pos и до конца массива data

	uint16_t ex_data = 0;														// число нулей, которыми нужно дозабить пакет, если
	if (rem_data_len <= full_pdata_len) ex_data = full_pdata_len - rem_data_len;// длина данных в data меньше длины чистых данных в пакете

	uint16_t dlen = full_pdata_len;												// длина данных, которые будут помещены в пакет из data
	if (full_pdata_len > rem_data_len) dlen = rem_data_len;

	uint8_t tdata[MAX_PACK_LEN];												// массив данных для записи в пакет (с ex_data доп. нулей - при необходимости)
	tdata[0] = _msg_pack->start_marker;
	tdata[1] = _msg_pack->msg_id;
	tdata[2] = _msg_pack->packet_number;
	memcpy(&tdata[PACK_SRV_LEN-1], data + dpos, sizeof(uint8_t)*dlen);
	if (ex_data > 0) memset(&tdata[dlen + PACK_SRV_LEN-1], 0x0, sizeof(uint8_t)*ex_data);		// дозаполнить нулями
	uint8_t tdata_len = data_block_len * block_cnt;								// длина массива данных tdata
	uint8_t crc = Crc8(&tdata[0], tdata_len-1);									// контрольная сумма по данным, помещаемым в пакет
	tdata[tdata_len - 1] = crc;

	//int i; for (i = 0; i < tdata_len; i++) printf("%i ",tdata[i]);

	uint8_t apos = 0;															// текущая позиция данных в массиве arr
	uint8_t tpos = 0;															// текущая позиция данных в массиве tdata
	uint8_t rec_data[2*MAX_REC_ERRS];
	uint16_t rec_len = 2 * _msg_pack->rec_errs;
	while (block_cnt-- > 0)
	{
		//uint8_t ready_arr[18];
		memcpy(&arr[apos], &tdata[tpos], data_block_len*sizeof(uint8_t));
		//int i; for (i = 0; i < data_block_len; i++) ready_arr[i] = tdata[tpos+i];
		calcRecoveryPart(&arr[apos], &rec_data[0], _msg_pack, _gf_data);
		memcpy(&arr[apos + data_block_len], &rec_data[0], rec_len*sizeof(uint8_t));

		apos += block_len;
		tpos += data_block_len;
	}

	memcpy(&_msg_pack->data[0], &arr[0], pack_len*sizeof(uint8_t));
	_msg_pack->data_len = pack_len - PACK_SRV_LEN;
	_msg_pack->crc = crc;

	*pos = dpos + dlen;
}


void initMsgBody(uint8_t _data_len, uint8_t _rec_errs, MsgBody* _msg_body)
{
	_msg_body = (MsgBody*)malloc(sizeof(MsgBody));

	_msg_body->data_len = _data_len;

	uint8_t rlen = 2*_rec_errs;
	_msg_body->rec_data_len = rlen;

	if (_data_len > 0)
	{
		uint8_t len = _data_len + 2*_rec_errs + 1;
		_msg_body->data = (uint8_t*)calloc(len, sizeof(uint8_t));
		memset(_msg_body->data, 0x0, len*sizeof(uint8_t));

		_msg_body->crc = Crc8(_msg_body->data, len);
	}
	else
	{
		_msg_body->data = 0;
		_msg_body->crc = 0;
	}
}

void clearMsgBody(MsgBody *_msg_body)
{
	_msg_body->data_len = 0;
	_msg_body->rec_data_len = 0;
	_msg_body->crc = 0;
	free(_msg_body->data);
}

int findMsgBody(QUEUE8 *_queue, MsgHeader *_msg_header, MsgBody *_msg_body, GF_Data *_gf_data)
{
	uint32_t sz = QUEUE8_count(_queue);
	if (sz != _msg_header->pack_len) return E_RS_LEN;

	guint8* arr = (guint8*)calloc(sz, sizeof(uint8_t));
	int i;
	for (i = 0; i < sz; i++)
	{
		uint8_t ch = QUEUE8_at(i, _queue);
		arr[i] = ch;
	}

	GFPoly *dist = GFPoly_alloc();

	dist->data = &arr[0];
	dist->power = sz-2;

	GF *gf = _gf_data->gf;
	int index = _msg_header->rec_errs-1;
	GFPoly *g = _gf_data->gf_polys[index];

	GFPoly *dec = GFPoly_alloc();
	GFPoly_self_inv(dist);
	int res = RS_decode(dist,g,gf,dec);
	if (res == E_RS_FATAL || res == E_RS_NOTFOUND)
	{
		//GFPoly_destroy(dist);
		free(arr);
		if (dec->power != NoD) GFPoly_destroy(dec);
		free(dist);
		free(dec);

		return res;
	}

	GFPoly_self_inv(dec);
	uint8_t _data_len = _msg_header->pack_len - 2*_msg_header->rec_errs - 1;
	uint8_t _rec_len = 2*_msg_header->rec_errs;
	_msg_body->data_len = _data_len;							// длина данных в теле сообщения
	_msg_body->rec_data_len = _rec_len;							// длина массива проверочных байт в теле сообщения
	_msg_body->crc = arr[sz-1];

	_msg_body->data = (uint8_t*)calloc(sz-1, sizeof(uint8_t));
	memcpy(_msg_body->data, &arr[0], (_data_len)*sizeof(uint8_t));
	memcpy(_msg_body->data + _data_len, &arr[0], (_rec_len)*sizeof(uint8_t));

	//GFPoly_destroy(dist);
	free(arr);
	GFPoly_destroy(dec);
	free(dist);
	free(dec);

	return res;
}

int checkMsgBody(MsgBody *_msg_body)
{
	uint8_t len = _msg_body->data_len + _msg_body->rec_data_len;
	uint8_t crc = Crc8(_msg_body->data,len);
	if (crc != _msg_body->crc) return E_MSG_CRC;

	return E_MSG_OK;
}
