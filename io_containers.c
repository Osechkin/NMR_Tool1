#include "io_containers.h"


void STACK8_init(STACK8 *stack)
{
	stack->head = 0;
	stack->cnt = 0;
}

void STACK8_free(STACK8 stack)
{
	while (!STACK8_empty(stack))
	{
		STACK8_pop(&stack);
	}
}

int STACK8_empty(STACK8 stack)
{
	return stack.head == 0;
}

void STACK8_push(uint8_t x, STACK8 *stack)
{
	Node8 *p = stack->head;
    stack->head = (Node8*)malloc(sizeof(Node8));
    stack->head->item = x;
    stack->head->next = p;
    stack->cnt++;
}

uint8_t STACK8_pop(STACK8 *stack)
{
    uint8_t v = stack->head->item;
    Node8 *t = stack->head->next;

    free(stack->head);
    stack->head = t;
    --stack->cnt;

    return v;
}

void STACK8_add(uint8_t *a, int size, STACK8 stack)
{
	int i = 0;
    for (i = 0; i < size; i++)
    {
        uint8_t t = a[i];
        STACK8_push(t, &stack);
    }
}

int STACK8_count(STACK8 stack)
{
    return stack.cnt;
}


// max is maximum queue size. If max = 0 then the queue size is unlimited
void QUEUE8_init(int max, QUEUE8 *queue)
{
	queue->head = 0;
	queue->cnt = 0;
	queue->max = max;
}

void QUEUE8_free(QUEUE8 *queue)
{
	QUEUE8_remove(queue->cnt, queue);
}

int QUEUE8_count(QUEUE8 *queue)
{
    return queue->cnt;
}

void QUEUE8_remove(int size, QUEUE8 *queue)
{
    if (size > queue->cnt) size = queue->cnt;
    int i = 0;
    for (i = 0; i < size; i++)
    {
        QUEUE8_get(queue);
    }
}

void QUEUE8_clear(QUEUE8 *queue)
{
	int i = 0;
	int cnt = queue->cnt;
	for (i = 0; i < cnt; i++)
	{
		QUEUE8_get(queue);
	}
}

int QUEUE8_empty(QUEUE8 *queue)
{
    return queue->head == 0;
}

void QUEUE8_put(uint8_t x, QUEUE8 *queue)
{
    Node8 *t = queue->tail;
    queue->tail = (Node8*)malloc(sizeof(Node8));
    queue->tail->item = x;
    queue->tail->next = 0;

    if (queue->head == 0)
    {
     	queue->head = queue->tail;
    }
    else t->next = queue->tail;

    queue->cnt++;
    if (queue->cnt > queue->max && queue->max > 0) QUEUE8_get(queue);
}

uint8_t QUEUE8_get(QUEUE8 *queue)
{
    uint8_t v = queue->head->item;
    Node8 *t = queue->head->next;

    free(queue->head);
    queue->head = t;
    --queue->cnt;

    return v;
}

uint8_t QUEUE8_get_last(QUEUE8 *queue)
{
	uint8_t v = queue->tail->item;
	Node8 *t = queue->tail;

	int n = queue->cnt;
	if (n > 2)
	{
		Node8 *pre_last = queue->head;
		int i;
		for (i = 0; i < n-2; i++)
		{
		    pre_last = pre_last->next;
		    v = pre_last->item;
		}
		free(pre_last->next);
		//free(queue->tail);
		queue->tail = pre_last;
		queue->cnt = n-1;
	}
	else if (n == 2)
	{
		free(queue->tail);
		queue->tail = queue->head;
	}
	else if (n == 1)
	{
		free(queue->head);
		queue->cnt = 0;
	}

	return v;
}

uint8_t QUEUE8_at(int n, QUEUE8 *queue)
{
    if (n >= queue->cnt) n = queue->cnt - 1;

    uint8_t v = queue->head->item;
    if (n == 0) return v;

    Node8 *index = queue->head->next;
    int i = 0;
    for (i = 1; i <= n; i++)
    {
        v = index->item;
        index = index->next;
    }

    return v;
}

void QUEUE8_add(uint8_t *a, int size, QUEUE8 *queue)
{
	int i = 0;
    for (i = 0; i < size; i++)
    {
        uint8_t t = a[i];
        QUEUE8_put(t, queue);
    }
}

int QUEUE8_maxsize(QUEUE8 *queue)
{
    return queue->max;
}


// max is maximum queue size. If max = 0 then the queue size is unlimited
void QUEUEPtr_init(int max, QUEUEPtr *queue)
{
	queue->head = 0;
	queue->cnt = 0;
	queue->max = max;
}

void QUEUEPtr_free(QUEUEPtr *queue)
{
	QUEUEPtr_remove(queue->cnt, queue);
}

int QUEUEPtr_count(QUEUEPtr *queue)
{
    return queue->cnt;
}

void QUEUEPtr_remove(int size, QUEUEPtr *queue)
{
    if (size > queue->cnt) size = queue->cnt;
    int i = 0;
    for (i = 0; i < size; i++)
    {
    	QUEUEPtr_get(queue);
    }
}

void QUEUEPtr_clear(QUEUEPtr *queue)
{
	int i = 0;
	int cnt = queue->cnt;
	for (i = 0; i < cnt; i++)
	{
		QUEUEPtr_get(queue);
	}
}

int QUEUEPtr_empty(QUEUEPtr *queue)
{
    return queue->head == 0;
}

void QUEUEPtr_put(void* x, QUEUEPtr *queue)
{
    NodePtr *t = queue->tail;
    queue->tail = (NodePtr*)malloc(sizeof(NodePtr));
    queue->tail->item = x;
    queue->tail->next = 0;

    if (queue->head == 0)
    {
     	queue->head = queue->tail;
    }
    else t->next = queue->tail;

    queue->cnt++;
    if (queue->cnt > queue->max && queue->max > 0) QUEUEPtr_get(queue);
}

void* QUEUEPtr_get(QUEUEPtr *queue)
{
    void* v = queue->head->item;
    NodePtr *t = queue->head->next;

    free(queue->head->item);
    free(queue->head);
    queue->head = t;
    --queue->cnt;

    return v;
}

void* QUEUEPtr_at(int n, QUEUEPtr *queue)
{
    if (n >= queue->cnt) n = queue->cnt - 1;

    void* v = queue->head->item;
    if (n == 0) return v;

    NodePtr *index = queue->head->next;
    int i = 0;
    for (i = 1; i <= n; i++)
    {
        v = index->item;
        index = index->next;
    }

    return v;
}

int QUEUEPtr_maxsize(QUEUEPtr *queue)
{
    return queue->max;
}


void STACKPtr_init(STACKPtr *stack)
{
	stack->head = 0;
	stack->cnt = 0;
}

void STACKPtr_remove(STACKPtr *stack)
{
	STACKPtr_pop(stack);
}

void STACKPtr_clear(STACKPtr *stack)
{
	int i;
	for (i = 0; i < stack->cnt; i++)
	{
		STACKPtr_pop(stack);
	}
}

int STACKPtr_empty(STACKPtr *stack)
{
	return stack->cnt == 0;
}

void STACKPtr_push(void* x, STACKPtr *stack)
{
	NodePtr *p = stack->head;
	stack->head = (NodePtr*)malloc(sizeof(NodePtr));
	stack->head->item = x;
	stack->head->next = p;
	stack->cnt++;
}

void* STACKPtr_pop(STACKPtr *stack)
{
	void* v = stack->head->item;
	NodePtr *t = stack->head->next;

	free(stack->head);
	stack->head = t;
	--stack->cnt;

	return v;
}

void* STACKPtr_first(STACKPtr *stack)
{
	void *v = 0;
	if (stack->cnt > 0) v = stack->head->item;

	return v;
}

int STACKPtr_count(STACKPtr *stack)
{
	return stack->cnt;
}



void STACKPtrF_init(STACKPtrF *stack)
{
	stack->head = 0;
	stack->cnt = 0;
}

void STACKPtrF_remove(STACKPtrF *stack)
{
	STACKPtrF_pop(stack);
}

void STACKPtrF_clear(STACKPtrF *stack)
{
	int i;
	for (i = 0; i < stack->cnt; i++)
	{
		STACKPtrF_pop(stack);
	}
}

int STACKPtrF_empty(STACKPtrF *stack)
{
	return stack->cnt == 0;
}

void STACKPtrF_push(float* x, STACKPtrF *stack)
{
	NodePtrF *p = stack->head;
	stack->head = (NodePtrF*)malloc(sizeof(NodePtrF));
	stack->head->item = x;
	stack->head->next = p;
	stack->cnt++;
}

float* STACKPtrF_pop(STACKPtrF *stack)
{
	float* v = stack->head->item;
	NodePtrF *t = stack->head->next;

	free(stack->head);
	stack->head = t;
	--stack->cnt;

	return v;
}

float* STACKPtrF_first(STACKPtrF *stack)
{
	float *v = 0;
	if (stack->cnt > 0) v = stack->head->item;

	return v;
}

int STACKPtrF_count(STACKPtrF *stack)
{
	return stack->cnt == 0;
}

void STACKPtrF_swap(STACKPtrF *stack)
{
	float *x1 = STACKPtrF_pop(stack);
	float *x2 = STACKPtrF_pop(stack);
	STACKPtrF_push(x1, stack);
	STACKPtrF_push(x2, stack);
}


void BUFFER8_init(BUFFER8 *buff8)
{
	buff8->buff = &temp_buff[0];
	buff8->head = buff8->buff;
	buff8->tail = buff8->buff;
	buff8->len = 0;
}

void BUFFER8_clear(BUFFER8 *buff8)
{
	memset(buff8->head, 0x0, buff8->len*sizeof(uint8_t));

	buff8->head = buff8->buff;
	buff8->tail = buff8->buff;
	buff8->len = 0;
}

int BUFFER8_empty(BUFFER8 *buff8)
{
	return (buff8->len == 0);
}

void BUFFER8_put(uint8_t x, BUFFER8 *buff8)
{
	if (buff8->len == 0)
	{
		*(buff8->tail) = x;
		buff8->len++;
	}
	else
	{
		if ((buff8->tail - buff8->buff) >= BUFF8_SIZE-2)
		{
			memcpy(buff8->buff, buff8->head, buff8->len*sizeof(uint8_t));
			buff8->head = buff8->buff;
			buff8->tail = buff8->head + buff8->len;
			*(buff8->tail) = x;
			buff8->len++;
		}
		else
		{
			buff8->tail++;
			*(buff8->tail) = x;
			buff8->len++;
		}
	}
}

uint8_t BUFFER8_get(BUFFER8 *buff8)
{
	if (buff8->len == 0) return 0;

	uint8_t x = *(buff8->head);
	if (buff8->len == 1) buff8->len = 0;
	else
	{
		buff8->head++;
		--(buff8->len);
	}

	return x;
}


int BUFFER8_count(BUFFER8 *buff8)
{
	return buff8->len;
}
