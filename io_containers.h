/*
 * io_containers.h
 *
 *  Created on: 09.09.2013
 *      Author: Andrey
 */

#ifndef IO_CONTAINERS_H_
#define IO_CONTAINERS_H_

#include "stdint.h"
#include "stdlib.h"


typedef struct Node8
{
	uint8_t item;
	struct Node8 *next;
} Node8;

typedef struct NodePtr
{
	void *item;
	struct NodePtr *next;
} NodePtr;

typedef struct NodePtrF
{
	float *item;
	struct NodePtrF *next;
} NodePtrF;


/********* STACK of uint8_t data ***********/
typedef struct STACK8
{
	Node8 *head;
	int cnt;
} STACK8;

void STACK8_init(STACK8 *stack);
void STACK8_free(STACK8 stack);
int STACK8_empty(STACK8 stack);
void STACK8_push(uint8_t x, STACK8 *stack);
uint8_t STACK8_pop(STACK8 *stack);
void STACK8_add(uint8_t *a, int size, STACK8 stack);
int STACK8_count(STACK8 stack);
/*******************************************/


/********* QUEUE of uint8_t data ***********/
typedef struct QUEUE8
{
	struct Node8 *head;
	struct Node8 *tail;
	int cnt;
	int max;
} QUEUE8;

void QUEUE8_init(int max, QUEUE8 *queue);
void QUEUE8_free(QUEUE8 *queue);
int QUEUE8_count(QUEUE8 *queue);
void QUEUE8_remove(int size, QUEUE8 *queue);
void QUEUE8_clear(QUEUE8 *queue);
int QUEUE8_empty(QUEUE8 *queue);
void QUEUE8_put(uint8_t x, QUEUE8 *queue);
uint8_t QUEUE8_get(QUEUE8 *queue);
uint8_t QUEUE8_get_last(QUEUE8 *queue);
uint8_t QUEUE8_at(int n, QUEUE8 *queue);
void QUEUE8_add(uint8_t *a, int size, QUEUE8 *queue);
int QUEUE8_maxsize(QUEUE8 *queue);
/********************************************/


/********** QUEUE of void* data *************/
typedef struct QUEUEPtr
{
	struct NodePtr *head;
	struct NodePtr *tail;
	int cnt;
	int max;
} QUEUEPtr;

void QUEUEPtr_init(int max, QUEUEPtr *queue);
void QUEUEPtr_free(QUEUEPtr *queue);
int QUEUEPtr_count(QUEUEPtr *queue);
void QUEUEPtr_remove(int size, QUEUEPtr *queue);
void QUEUEPtr_clear(QUEUEPtr *queue);
int QUEUEPtr_empty(QUEUEPtr *queue);
void QUEUEPtr_put(void* x, QUEUEPtr *queue);
void* QUEUEPtr_get(QUEUEPtr *queue);
void* QUEUEPtr_at(int n, QUEUEPtr *queue);
/*********************************************/


/*********** STACK of void* data *************/
typedef struct STACKPtr
{
	struct NodePtr *head;
	int cnt;
} STACKPtr;

void STACKPtr_init(STACKPtr *stack);
void STACKPtr_remove(STACKPtr *stack);
void STACKPtr_clear(STACKPtr *stack);
int STACKPtr_empty(STACKPtr *stack);
void STACKPtr_push(void* x, STACKPtr *stack);
void* STACKPtr_pop(STACKPtr *stack);
void* STACKPtr_first(STACKPtr *stack);
int STACKPtr_count(STACKPtr *stack);
/*********************************************/


/*********** STACK of float* data *************/
typedef struct STACKPtrF
{
	struct NodePtrF *head;
	int cnt;
} STACKPtrF;

void STACKPtrF_init(STACKPtrF *stack);
void STACKPtrF_remove(STACKPtrF *stack);
void STACKPtrF_clear(STACKPtrF *stack);
int STACKPtrF_empty(STACKPtrF *stack);
void STACKPtrF_push(float* x, STACKPtrF *stack);
float* STACKPtrF_pop(STACKPtrF *stack);
float* STACKPtrF_first(STACKPtrF *stack);
int STACKPtrF_count(STACKPtrF *stack);
void STACKPtrF_swap(STACKPtrF *stack);
/*********************************************/


/*********** BUFFER of uint8 data ************/
#pragma DATA_SECTION(temp_buff, "sharedram_data");

#define BUFF8_SIZE		16384

static uint8_t temp_buff[BUFF8_SIZE];
typedef struct BUFFER8
{
	uint8_t 	*buff;
	uint8_t		*head;
	uint8_t		*tail;
	uint32_t	len;
} BUFFER8;

void BUFFER8_init(BUFFER8 *buff8);
void BUFFER8_clear(BUFFER8 *buff8);
int BUFFER8_empty(BUFFER8 *buff8);
void BUFFER8_put(uint8_t x, BUFFER8 *buff8);
uint8_t BUFFER8_get(BUFFER8 *buff8);
int BUFFER8_count(BUFFER8 *buff8);
/*********************************************/


#endif /* IO_CONTAINERS_H_ */
