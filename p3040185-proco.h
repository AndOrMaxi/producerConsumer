#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sys/types.h"

typedef struct circular_buffer
{
    void *buffer;     // data buffer
    void *buffer_end; // end of data buffer
    size_t capacity;  // maximum number of items in the buffer
    size_t count;     // number of items in the buffer
    size_t sz;        // size of each item in the buffer
    void *head;       // pointer to head
    void *tail;       // pointer to tail
} circular_buffer;


struct data
{
	int p;			//to plh8os twn paragwgwn
	int c;			//to plh8os twn katanalwtwn
	int n;			//plh8os tyxaiwn ari8mwn
	int q;			//xwrhtikothta ths ouras
	int s;			//sporos gia th gennhtria pseydotyxaiwn ari8mwn
		
} proco_input;

struct thread_data
{
	int n;//plh8os ari8mwn pou 8a paragei o paragwgos
	int id;//anagnwristiko paragwgou
};

circular_buffer *queue;//oura tyxaiwn ari8mwn

FILE *fp1;
FILE *fp2;
char* cons; //string gia to onoma tou arxeiou twn katanalwtwn
char* prod; //string gia to onoma tou arxeiou twn paragwgwn

int total_nums; //to synoliko plh8os twn ari8mwn pou 8a parax8oun apo olous tous producers
int counter; //metrhths pou ayksanetai ka8e fora pou katanalwnetai enas ari8mos


pthread_mutex_t mutex_queue;		//gia amoibaio apokleismo sthn oura

pthread_mutex_t mutex_console;		//gia amoibaio apokleismo sthn konsola
pthread_mutex_t mutex_counter;		//gia amoibaio apokleismo sto metrhth katanalwmenwn ari8mwn


pthread_cond_t	queue_empty;		//condition variable gia oura adeia	
pthread_cond_t	queue_full;			//condition variable gia oura gemath


//gia proco2 mono
int prod_printed; //metrhths pou deixnei posoi paragwgoi exoun mexri stigmhs typwsei sthn konsola
int cons_printed; //metrhths pou deixnei posoi katanalwtes exoun mexri stigmhs typwsei sthn konsola

pthread_mutex_t mutex_file_p;		//gia amoibaio apokleismo sto arxeio twn paragwgwn
pthread_mutex_t mutex_file_c;		//gia amoibaio apokleismo sto arxeio twn katanalwtwn
pthread_mutex_t mutex_prod_printed;	//gia amoibaio apokleismo sto metrhth paragwgwn pou exoun ektypwsei
pthread_mutex_t mutex_cons_printed;	//gia amoibaio apokleismo sto metrhth katanalwtwn pou exoun ektypwsei

pthread_cond_t	print_finished;		//condition variable gia oloklhrwsh ektypwshs
pthread_cond_t	pro_to_print;		//condition variable gia na typwsoun me seira oi paragwgoi
pthread_cond_t	con_to_print;		//condition variable gia na typwsoun me seira oi katanalwtes

//koines synarthseis
void cb_init(circular_buffer *cb, size_t capacity, size_t sz);

void cb_free(circular_buffer *cb);

void cb_push_back(circular_buffer *cb, const void *item);

void cb_pop_front(circular_buffer *cb, void *item);

//synarthseis pou diaferoun sto proco1 kai proco2
void * producer(void * args) ;

void * consumer(void * id) ;


