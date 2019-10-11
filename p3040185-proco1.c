#include "p3040185-proco.h"

void * producer(void * args) 
{
	struct thread_data *pro_data;
	pro_data = (struct thread_data *) args;//gia na deixnei sta orismata tou producer
	
	
	int nums;		//topikos metrhths
	unsigned int seed = (unsigned int)pro_data->id * proco_input.s; //anagnwristiko nhmatos paragwgou epi koinos sporos
	int temp = 0;		//proswrinh topikh metablhth pou pairnei thn timh tou pseydotyxaiou ari8mou pou paragetai se ka8e loop
	int numbers[proco_input.n];	//pinakas gia thn apo8hkeysh twn pseydotyxaiwn ari8mwn, xwrhtikothtas n (n ari8mous 8a paragei o producer
	
	for(nums=0; nums<proco_input.n; nums++)//ypologismos n pseydotyxaiwn ari8mwn kai eisagwgh toys sthn oura
	{
		temp = rand_r(&seed)%256;//ypologismos pseydotyxaiou ari8mou apo 0 ews 255(tyxaio range gia na bgainoun mikroteroi ari8moi)
		numbers[nums] = temp;//krataw tous ari8mous kai se pinaka, gia na ektypwsei meta o producer tous ari8mous pou parhgage
		
		pthread_mutex_lock(&mutex_queue);//desmeyw thn oura gia na eisagw ton ari8mo pou parax8hke se ayto to loop
		
		while(queue->count == queue->capacity)//elegxw an h oura einai gemath kai an nai, perimenw na adeiasei mia 8esh
		{
			pthread_cond_wait(&queue_full, &mutex_queue);
		}
			
		cb_push_back(queue, (void*)&temp);//eisagwgh tou ari8mou sthn oura
		
		pthread_cond_signal(&queue_empty);//stelnw shma ston katanalwth pou perimenei na papsei na einai adeia h oura
				
		fp1 = fopen(prod, "a+");//anoigw to arxeio twn paragwgwn gia na pros8esw grammh me to id tou producer kai ton ari8mo pou parhgage
		
		if (fp1 == NULL) 
		{
			printf("Error opening file!\n");
			exit(1);
		}
		fprintf(fp1,"Producer %d, %d\n", (int)pro_data->id, temp);//kanw append th grammh sto arxeio
		
		fclose(fp1);//kleinw to arxeio
	
			
		pthread_mutex_unlock(&mutex_queue);//apodesmeyw thn oura
		
	}
			
	pthread_mutex_lock(&mutex_console);//desmeyw thn konsola
	
	nums=0;//mhdenizw to metrhth gia na ton ksanaxrhsimopoihsw
	printf("Producer %d: ", (int)pro_data->id);//diatrexw ton pinaka me tous ari8mous pou parhgage o paragwgos kai typwnw sthn  o8onh th grammh pou tou antistoixei
	if (proco_input.n!=1)
	{
		for(nums=0; nums<(proco_input.n-1); nums++)
		{	
			printf("%d, ",numbers[nums]);
		}
	}
	printf("%d\n",numbers[nums]);
		
	pthread_mutex_unlock(&mutex_console);//apodesmeysh ths konsolas
		
	pthread_exit(NULL);
}

//----------------------------------------------------------------------------------------------

void * consumer(void * id) 
{
	int nums; //topikos metrhths
	int consumed=0; //metrhths tou plh8ous twn ari8mwn pou exei katanalwsei o katanalwths
	
	int temp = 0; //proswrinh topikh metablhth pou pairnei thn timh tou ari8mou pou katanalwnetai se ka8e loop
	
	int numbers[total_nums];//pinakas gia thn apo8hkeysh twn stoixeiwn pou katanalwse o consumer
	
	while (counter != total_nums)//oso den exoun katanalw8ei oloi oi ari8moi pou exoun parax8ei
	{
		pthread_mutex_lock(&mutex_queue);//desmeysh ths ouras gia apokleistikh prosbash
		
		while (queue->count == 0)//elegxos an h oura einai adeia
		{
			pthread_cond_wait(&queue_empty, &mutex_queue);//anamonh mexri na paragei o producer enan ari8mo kai na papsei na einai adeia h oura
		}
		
		cb_pop_front(queue, &temp);		//eksagwgh ari8mou apo thn oura	
		
		pthread_cond_signal(&queue_full);//eidopoihsh tou paragwgou oti h oura epapse na einai gemath
		
		numbers[consumed++]=temp;//krataw ton ari8mo gia thn ektypwsh meta olwn oswn katanalw8hkan
						
		counter++;//aykshsh tou metrhth twn katanalwmenwn
				
		fp2 = fopen(cons, "a+");//anoigma tou arxeiou twn katanalwtwn gia na grapsw ton ari8mo pou afairesa apo thn oura

		if (fp2 == NULL) 
		{
			printf("Error opening file!\n");
			exit(1);
		}	
		fprintf(fp2, "Consumer %d, %d\n", (int)id, temp);//grafw sto arxeio ton ari8mo parhgaga se ayto to loop
		fclose(fp2);//kleinw to arxeio
				
		pthread_mutex_unlock(&mutex_queue);//apodesmeysh ths ouras
		
	}	
		
	pthread_mutex_lock(&mutex_console);//desmeysh ths konsolas
			
	nums=0;//mhdenismos tou metrhth gia epanaxrhsimopoihsh
	printf("Consumer %d: ", (int)id);//diatrexw ton pinaka gia na typwsw sthn o8onh tous ari8mous pou katanalwse o consumer
	
	if (consumed>1)
	{		
		for(nums = 0; nums < consumed-1; nums++)
		{	
			printf("%d, ",numbers[nums]);
		}
	}
	printf("%d\n",numbers[nums]);
		
	
	pthread_mutex_unlock(&mutex_console);//apodesmeysh ths o8onhs
	
	
	pthread_exit(NULL);
}

//----------------------------------------------------------------------------------------------

int main(int argc, char** argv)
{
	if(argc == 6)//elegxos an dinontai swsta ta orismata
	{
		proco_input.p = atoi(argv[1]);//plh8os paragwgwn
		proco_input.c = atoi(argv[2]);//plh8os katanalwtwn
		proco_input.n = atoi(argv[3]);//plh8os paragomenwn ari8mwn apo ton paragwgo
		proco_input.q = atoi(argv[4]);//mege8os ths ouras
		proco_input.s = atoi(argv[5]);//sporos gia th gennhtria
		
		if (proco_input.p==1 && proco_input.c==1 && proco_input.n>=10)//elegxos an exei do8ei 1 paragwgos kai 1 katanalwths kai an to plh8os twn ari8mwn pou 8a paragei o producer einai apo 10 kai panw
		{
			//ta onomata twn arxeiwn
			cons = "./consumers_out.txt";
			prod = "./producers_in.txt";
			
			//elegxos an yparxoun hdh ta arxeia kai diagrafh tous
			if(remove(cons) == 0)
				  printf("File %s  existed and deleted.\n", cons);
			if(remove(prod) == 0)
				  printf("File %s  existed and deleted.\n", prod);
				
			total_nums = proco_input.p * proco_input.n;//synoliko plh8os ari8mwn pou 8a parax8oun, sto proco1 isoutai me n giati exoume 1 mono paragwgo
			counter = 0;//mhdenismos tou metrhth twn katanalwmenwn
			
			pthread_t producers[proco_input.p];//pinakas nhmatwn paragwgwn (sto proco1 exei mono mia 8esh giati p=1)
			pthread_t consumers[proco_input.c];//pinakas nhmatwn katanalwtwn (sto proco1 exei mono mia 8esh giati c=1)
			//arxikopoihseis
			pthread_mutex_init(&mutex_queue, NULL);
			pthread_mutex_init(&mutex_console, NULL);
			
			pthread_cond_init(&queue_empty, NULL);
			pthread_cond_init(&queue_full, NULL);
			
			pthread_attr_t attr;
			  
			pthread_attr_init(&attr);//dhmiourgia domhs xarakthristikwn twn nhmatwn
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);//8eloume na einai joinable ta nhmata paidia gia na ta perimenei to arxiko nhma na oloklhrwsoun
			
			queue = (circular_buffer*)malloc(sizeof (struct circular_buffer));
			cb_init(queue, proco_input.q, 1);//arxikopoihsh ths ouras me xwrhtikothta q
			
			struct thread_data d;		
			d.id=1;//anagnwristiko paragwgou
			d.n = proco_input.n; //plh8os ari8mwn pou 8a paragei
			int id =1;//anagnwristiko katanalwth
			
			pthread_create(&producers[0], &attr, producer, (void *)(&d));	//pernw tis 2 parametrous tou paragwgou mesw ths struct d	
			pthread_create(&consumers[0], &attr, consumer, (void *)(id));	//pernw to anagnwristiko tou katanalwth
				
					
			pthread_attr_destroy(&attr);	
			
		
			pthread_join(producers[0], NULL);		
			pthread_join(consumers[0], NULL);
			
			pthread_exit(NULL);	
			
			pthread_mutex_destroy(&mutex_queue);
			pthread_mutex_destroy(&mutex_console);
			
			pthread_cond_destroy(&queue_empty);
			pthread_cond_destroy(&queue_full);
			
			cb_free(queue);
		 }
		 else
		 {//mhnymata la8os eisagwghs orismatwn
		 	if (proco_input.p!=1 && proco_input.c!=1)
			{
				printf("Error! Enter '1' as the number of producers/consumers \n");
			}
			if (proco_input.n<10)
			{
				 printf("Error! Every producer should produce over 10 numbers \n");
			}
			
			return 0;	
		}	
	}
	else
	{//mhnyma la8os eisagwghs orismatwn
		printf("Error! Correct usage is: ./proco1 1 1 <n> <q> <s> \n");
		return 0;
	}
	
}
