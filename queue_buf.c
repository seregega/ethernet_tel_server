#include <stdint.h>
#include "queue_buf.h"

extern void printf_dos(const char *args, ...);


////////////////////////////////////////////////////////////////////////////////////////////////////
// queue free len
////////////////////////////////////////////////////////////////////////////////////////////////////
QUEUE_DATA_LEN free_queue( struct queue_buffer *queue )
{
//	printf_d("QUEUEADR=%x IN=%d OUT=%d\r\n",queue,queue->in,queue->out);

	if (queue->in == queue->out) return queue->len; // queue buffer all free
	if (queue->in > queue->out){
		return queue->len - (queue->in - queue->out);
	} else {
		return queue->out - queue->in;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// queue data len
////////////////////////////////////////////////////////////////////////////////////////////////////
QUEUE_DATA_LEN datalen_queue( struct queue_buffer *queue ){

	return queue->len - free_queue( queue );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// push data to queue
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int push_data_queue( struct queue_buffer *queue )
{
	QUEUE_DATA_LEN i;

	//printf("push_data_queue golova=%lu   hvost=%lu\n", queue->golova, queue->hvost);

// test mesto est?
	if ( free_queue( queue ) <= queue->rw_len ){
		// no memory
		printf_dos("ERROR push no memory QUEUEADR=%x IN=%d OUT=%d\r\n",queue,queue->in,queue->out);
		return -1;
	}
	for (i=0; i<queue->rw_len; i++)	{
                *(queue->queue + queue->in) = *(queue->rw_buf + i);
		(queue->in)++;
		if ( queue->in >= queue->len ) queue->in = 0;
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// pop data from queue
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int pop_data_queue( struct queue_buffer *queue )
{
	QUEUE_DATA_LEN i;

	//printf("pop_data_queue golova=%lu   hvost=%lu\n", queue->golova, queue->hvost);

// test mesto est?
	if ( (queue->len - free_queue( queue )) < queue->rw_len ){
		printf_dos("ERROR pop no memory QUEUEADR=%x IN=%d OUT=%d\r\n",queue,queue->in,queue->out);
		// no memory
		return -1;
	}

	for (i=0; i< queue->rw_len; i++){
	    *(queue->rw_buf + i) = *(queue->queue + queue->out);
	    (queue->out)++;
	    if ( queue->out >= queue->len ) queue->out = 0;
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// push data to queue. one byte !!!!!
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int push_data_queue_b( struct queue_buffer *queue, unsigned char d )
{
	//printf("push_data_queue_b golova=%lu   hvost=%lu\n", queue->golova, queue->hvost);

// test mesto est?
	if ( free_queue( queue ) <= 1 ){
		// no memory
		return -1;
	}
	*(queue->queue + queue->in) = d;
	(queue->in)++;
	if ( queue->in >= queue->len ) queue->in = 0;
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// pop data from queue. one byte !!!!!
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int pop_data_queue_b( struct queue_buffer *queue, unsigned char *d )
{
	//printf("pop_data_queue_b golova=%lu   hvost=%lu\n", queue->golova, queue->hvost);
// test est mcto izvlekat ?
	if ( (queue->len - free_queue( queue )) == 0 ){
		// no memory
		return -1;
	}
        *d = *(queue->queue + queue->out);
	(queue->out)++;
	if ( queue->out >= queue->len ) queue->out = 0;

	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// read 1 byte bez izvlechenia iz ocheredi. one byte !!!!!
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int read_data_queue_b( struct queue_buffer *queue, unsigned char *d )
{
	//printf("read_data_queue_b golova=%lu   hvost=%lu\n", queue->golova, queue->hvost);

	if ( (queue->len - free_queue( queue )) == 0 ){
		// no data
		return -1;
	}

	*d = *(queue->queue + queue->out);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// reset queue golova+hvost=0
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int reset_queue( struct queue_buffer *queue ){
	queue->in = 0;
	queue->out = 0;
	return 0; 
}
