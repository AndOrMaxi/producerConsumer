#include "p3040185-proco.h"

void * producer(void * args) 
{
	struct thread_data *pro_data;
	pro_data = (struct thread_data *) args;	//gia na deixnei sta orismata tou producer
	
	int nums;		//topikos metrhths
	unsigned int seed = (unsigned int)pro_data->id * proco_input.s; //anagnwristiko nhmatos paragwgou epi koinos sporos
	int temp = 0;		//proswrinh topikh metablhth pou pairnei thn timh tou pseydotyxaiou ari8mou pou paragetai se ka8e loop
	int numbers[pro_data->n];	//pinakas gia thn apo8hkeysh twn pseydotyxaiwn ari8mwn, xwrhtikothtas n (n ari8mous 8a paragei o producer)
	
	for(nums=0; nums<pro_data->n; nums++)
	{
		temp = rand_r(&seed)%256;//ypologismos pseydotyxaiou ari8mou apo 0 ews 255(tyxaio range gia na bgainoun mikroteroi ari8moi)
		numbers[nums] = temp;//krataw tous ari8mous kai se pinaka, gia na ektypwsei meta o producer tous ari8mous pou parhgage
		
		pthread_mutex_lock(&mutex_queue);//desmeyw thn oura gia na eisagw ton ari8mo pou parax8hke se ayto to loop
		
		while(queue->count == queue->capacity)//elegxw an h oura einai gemath kai an nai, perimenw na adeiasei mia 8esh
		{
			pthread_cond_wait(&queue_full, &mutex_queue);
		}
			
		cb_push_back(queue, (void*)&temp);//eisagwgh tou ari8mou sthn oura
		
		pthread_cond_signal(&queue_empty);//stelnw shma se opoion katanalwth perimenei na papsei na einai adeia h oura
			
		pthread_mutex_lock(&mutex_file_p);//desmeyw to arxeio twn paragwgwn gia na mhn mporei na grapsei allos paragwgos
		
		fp1 = fopen(prod, "a+");//anoigw to arxeio twn paragwgwn gia na pros8esw grammh me to id tou producer kai ton ari8mo pou parhgage
		
		if (fp1 == NULL) 
		{
			printf("Error opening file!\n");
			exit(1);
		}
		fprintf(fp1,"Producer %d, %d\n", (int)pro_data->id, temp);//kanw append th grammh sto arxeio
		
		fclose(fp1);//kleinw to arxeio
		
		pthread_mutex_unlock(&mutex_file_p);//apodesmeyw to arxeio twn paragwgwn	
			
		pthread_mutex_unlock(&mutex_queue);//apodesmeyw thn oura
		
		sleep(1);//gia pio dikaih enallagh twn nhmatwn twn paragwgwn
	}
	
	pthread_mutex_lock(&mutex_prod_printed);//apokleistikh prosbash sth metablhth prod_printed gia na elegksei thn timh ths o paragwgos
	
	while((int)(pro_data->id) != (prod_printed + 1))//elegxei an to id tou paragwgou einai to epomeno pou 8a typwsei sthn konsola
	{	
		pthread_cond_wait(&pro_to_print,&mutex_prod_printed);	//mexri na er8ei h seira tou perimenei ta nhmata me mikrotera anagnwristika na typwsoun
		
	}
	
	pthread_mutex_unlock(&mutex_prod_printed);//apodesmeysh ths metablhths prod_printed
		
	pthread_mutex_lock(&mutex_console);//desmeyw thn konsola
	
	nums=0;//mhdenizw to metrhth gia na ton ksanaxrhsimopoihsw
	printf("Producer %d: ", (int)pro_data->id);//diatrexw ton pinaka me tous ari8mous pou parhgage o paragwgos kai typwnw sthn  o8onh th grammh pou tou antistoixei
	if (pro_data->n!=1)
	{
		for(nums=0; nums<(pro_data->n-1); nums++)
		{	
			printf("%d, ",numbers[nums]);
		}
	}
	printf("%d\n",numbers[nums]);
	
	
	pthread_mutex_lock(&mutex_prod_printed);//apokleistikh prosbash sth metablhth prod_printed gia na aykshsei thn timh ths o paragwgos
	
	prod_printed++;//akoma enas paragwgos oloklhrwse thn ektypwsh sthn konsola
	pthread_cond_broadcast(&pro_to_print);//enhmerwnw tous paragwgous pou einai sthn anamonh na elegksoun an hr8e h seira tous na typwsoun
	
	pthread_mutex_unlock(&mutex_prod_printed);//apodesmeysh ths metablhths prod_printed
	
	
	if(prod_printed==proco_input.p)//elegxw an exoun typwsei oloi oi paragwgoi
	{
		pthread_cond_broadcast(&print_finished);//an nai, enhmerwnw tous katanalwtes na arxisoun me th seira na typwnoun
	}
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
			pthread_cond_wait(&queue_empty, &mutex_queue);//anamonh mexri na paragei kapoios producer enan ari8mo kai na papsei na einai adeia h oura
		}
		
		cb_pop_front(queue, &temp);	//eksagwgh ari8mou apo thn oura			
		
		pthread_cond_signal(&queue_full);//eidopoihsh tou paragwgou oti h oura epapse na einai gemath
		
		numbers[consumed++]=temp;//krataw ton ari8mo gia thn ektypwsh meta olwn oswn katanalw8hkan apo ayton ton katanalwth
		
		pthread_mutex_lock(&mutex_counter); //desmeysh tou metrhth twn katanalwmenwn gia apokleistikh prossbash apo ayton ton katanalwth
		
		counter++;//aykshsh tou metrhth twn katanalwmenwn
			
		pthread_mutex_unlock(&mutex_counter);//apodesmeysh tou counter
			
		pthread_mutex_lock(&mutex_file_c);	//apokleistikh prosbash sto arxeio twn katanalwtwn
		
		fp2 = fopen(cons, "a+");//anoigma tou arxeiou twn katanalwtwn gia na grapsw ton ari8mo pou afairesa apo thn oura

		if (fp2 == NULL) 
		{
			printf("Error opening file!\n");
			exit(1);
		}	
		fprintf(fp2, "Consumer %d, %d\n", (int)id, temp);//grafw sto arxeio ton ari8mo parhgaga se ayto to loop
		fclose(fp2);//kleinw to arxeio
			
		pthread_mutex_unlock(&mutex_file_c);//apodesmeysh tou arxeiou twn katanalwtwn
	
				
		pthread_mutex_unlock(&mutex_queue);//apodesmeysh ths ouras
		
		sleep(proco_input.c);//gia na moirazontai kalytera oi katanalwtes thn oura, analoga me to plh8os tous
	}	
	
	pthread_mutex_lock(&mutex_cons_printed);//apokleistikh prosbash sth metablhth cons_printed gia na elegksw thn timh ths
	
	while((int)(id) != (cons_printed + 1))//elegxos an o katanalwths me ayto to anagnwristiko einai o epomenos pou 8a typwsei sthn konsola
	{	
		pthread_cond_wait(&con_to_print,&mutex_cons_printed);//anamonh mexri na er8ei h seira tou katanalwth na typwsei
	}
	
	pthread_mutex_unlock(&mutex_cons_printed);//apodesmeysh ths metablhths
	while(prod_printed<proco_input.p)//elegxw an exoun oloklhrwsei thn ektypwsh oloi h paragwgoi
	{
		pthread_cond_wait(&print_finished, &mutex_console);//perimenw mexri na me eidopoihsei o teleytaios paragwgos pou typwse
	}
	pthread_mutex_lock(&mutex_console);//apokleistikh prosbash sthn konsola
	
	
	
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
	
	pthread_mutex_lock(&mutex_cons_printed);//desmeysh ths metablhths cons_printed gia na aykshsei o katanalwths thn timh ths
	
	cons_printed++;//allos enas katanalwths typwse sthn o8onh
	pthread_cond_broadcast(&con_to_print);//enhmerwnw gia na arxisei na typwnei o epomenos
		
	pthread_mutex_unlock(&mutex_cons_printed);//apodesmeysh ths cons_printed
	
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
		
		if (proco_input.n>=10)//elegxos an to plh8os twn ari8mwn pou 8a paragei o producer einai apo 10 kai panw
		{
			//ta onomata twn arxeiwn
			cons = "./consumers_out.txt";
			prod = "./producers_in.txt";
			
			//elegxos an yparxoun hdh ta arxeia kai diagrafh tous
			if(remove(cons) == 0)
				  printf("File %s  existed and deleted.\n", cons);
			if(remove(prod) == 0)
				  printf("File %s  existed and deleted.\n", prod);
				  
			total_nums = proco_input.p * proco_input.n;//synoliko plh8os ari8mwn pou 8a parax8oun
			counter = 0;//mhdenismos tou metrhth twn katanalwmenwn
			prod_printed = 0; 
			cons_printed = 0;
		
			pthread_t producers[proco_input.p];//pinakas nhmatwn paragwgwn
			pthread_t consumers[proco_input.c];//pinakas nhmatwn katanalwtwn
			//arxikopoihseis
			pthread_mutex_init(&mutex_queue, NULL);
			pthread_mutex_init(&mutex_file_p, NULL);
			pthread_mutex_init(&mutex_file_c, NULL);
			pthread_mutex_init(&mutex_console, NULL);
			pthread_mutex_init(&mutex_counter, NULL);
			pthread_mutex_init(&mutex_prod_printed, NULL);
			pthread_mutex_init(&mutex_cons_printed, NULL);
			
			pthread_cond_init(&queue_empty, NULL);
			pthread_cond_init(&queue_full, NULL);
			pthread_cond_init(&print_finished, NULL);
			pthread_cond_init(&pro_to_print, NULL);
			pthread_cond_init(&con_to_print, NULL);
			
			pthread_attr_t attr;
			  
			pthread_attr_init(&attr);//dhmiourgia domhs xarakthristikwn twn nhmatwn
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);//8eloume na einai joinable ta nhmata paidia gia na mplokarei to arxiko nhma kai na ta perimenei na oloklhrwsoun
			
			queue = (circular_buffer*)malloc(sizeof (struct circular_buffer));
			cb_init(queue, proco_input.q, 1);
			struct thread_data d[proco_input.p];//pinakas parametrwn paragwgwn
			int i;//metrhths
			
			for(i=0;i<proco_input.p;i++)//loop gia dhmiourgia olwn twn nhmatwn paragwgwn
			{
				d[i].id = i+1;
				d[i].n = proco_input.n;
				pthread_create(&producers[i], &attr, producer, (void *)(&d[i]));//pernw tis 2 parametrous tou paragwgou mesw ths struct d	
			}	
				
			for(i=0;i<proco_input.c;i++)	//loop gia dhmiourgia olwn twn nhmatwn katanalwtwn
				pthread_create(&consumers[i], &attr, consumer, (void *)(i+1));	//pernw to anagnwristiko tou katanalwth
				
			
		
			pthread_attr_destroy(&attr);	
			
			
			
			for(i=0;i<proco_input.p;i++)
				pthread_join(producers[i], NULL);
			for(i=0;i<proco_input.c;i++)
				pthread_join(consumers[i], NULL);
			
			pthread_exit(NULL);	
			
			pthread_mutex_destroy(&mutex_queue);
			pthread_mutex_destroy(&mutex_file_p);
			pthread_mutex_destroy(&mutex_file_c);
			pthread_mutex_destroy(&mutex_console);
			pthread_mutex_destroy(&mutex_counter);
			pthread_mutex_destroy(&mutex_prod_printed);
			pthread_mutex_destroy(&mutex_cons_printed);
			
			pthread_cond_destroy(&queue_empty);
			pthread_cond_destroy(&queue_full);
			pthread_cond_destroy(&print_finished);
			pthread_cond_destroy(&pro_to_print);
			pthread_cond_destroy(&con_to_print);
			
			cb_free(queue);
		 }
		 else
		 {//mhnyma la8os eisagwghs orismatwn
			printf("Error! Every producer should produce over 10 numbers \n");
			
			return 0;	
		}	
	}
	else
	{//mhnyma la8os eisagwghs orismatwn
		printf("Error! Correct usage is: ./proco2 <p> <c> <n> <q> <s> \n");
		return 0;
	}
	
}

