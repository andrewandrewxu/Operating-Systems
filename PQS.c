#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h> 
#include <unistd.h>
#include <time.h>

pthread_mutex_t mtx,mtx2,mtx3, mtx4, mtx5, mtx6;
pthread_cond_t cond,cond2,cond3;


//useconds_t usec;
clock_t startTime;
//int currenttime;
int turn=0;
int Interrupt=0;
typedef struct _mystruct{
	int cusnumber;
	int arrivetime;
	int servicetime;
	int priority;
	pthread_t threadhandle;
}Customer;

Customer GlobeServe;
Customer customers[30];
Customer Line[30];
int linenum=0;
int pickedone=0;
int i;


void swap(int tmp){
	Customer *temp;
	temp = &Line[tmp-1];
	Line[tmp-1] = Line[tmp];
	Line[tmp] = *temp;
}

void sort(){
	int  tmp;
	tmp = linenum;
	//Customer temp[30];
	tmp =0;
	if(tmp != 0){
		while(tmp != 0){
			if(Line[tmp].priority > Line[tmp-1].priority){
				swap(tmp);
				tmp--;
			}
			else if(Line[tmp].priority == Line[tmp-1].priority){
				if(Line[tmp].arrivetime < Line[tmp-1].arrivetime){
					swap(tmp);
					tmp--;
				}
				else if(Line[tmp].arrivetime == Line[tmp-1].arrivetime){
					if(Line[tmp].servicetime < Line[tmp-1].servicetime){
						swap(tmp);
						tmp--;
					}
					else if(Line[tmp].servicetime == Line[tmp-1].servicetime){
						if(Line[tmp].cusnumber < Line[tmp-1].cusnumber){
							swap(tmp);
							tmp =0;
						}
					}
					else{
						tmp=0;
					}
				}
				else{
					tmp =0;
				}
			}
			else{
				tmp =0;
			}
		}
		
	}
	
}

void AddtoLine(int index){
	pthread_mutex_lock(&mtx5);
	Line[linenum] = customers[index];

	sort();
	
	linenum++;
	
	pthread_mutex_unlock(&mtx5);

}




void GoServe(int index){
	clock_t current_t;
	//Customer* customer= (Customer*)customers[];
	pthread_mutex_lock(&mtx2);				//mtx2 make sure that only one customer get served
		//printf("THIS IS INDEX  go serve  %d\n", index);
		//printf("%d\n", customers[index].servicetime/10);
		current_t = clock();
		turn =1;	
		printf("The clerk starts serving customer %2d at time %.2f. \n", customers[index].cusnumber, (float)((clock()-startTime)/CLOCKS_PER_SEC ));				//
		GlobeServe = customers[index];		//make the global customer know who is being served
		printf("  %d  \n ", GlobeServe.priority);
		//printf("GLOBAL === %d\n", GlobeServe.cusnumber);
		
		while(((float)((clock()-current_t)/CLOCKS_PER_SEC) <= (float)customers[index].servicetime/10)&&(Interrupt==0));
		turn =0;
		

	if(Interrupt ==1 ){
		pthread_mutex_lock(&mtx4);		// only one change the interrupt
		Interrupt = 0;
		pthread_mutex_unlock(&mtx4);

		customers[index].servicetime = customers[index].servicetime - (int)(clock() - startTime);

		pthread_mutex_lock(&mtx3);
		pthread_cond_signal(&cond2);
		pthread_mutex_unlock(&mtx3);

	}

	printf("The clerk finishes the service to customer %2d at time %d. \n", customers[index].cusnumber, (int)((clock()-startTime)/CLOCKS_PER_SEC));
	
	
	
	pthread_mutex_unlock(&mtx2);
	
}


void Pickup(){
	int theone;
	theone = Line[pickedone].cusnumber;
	theone--;
	GoServe(theone);
	pickedone++;
	
}

 void service(int index){
 	//printf("THIS IS INDEX %d\n", index);
 	int mytime, currenttime;
 	mytime = customers[index].arrivetime;

 	if(turn==0){					// service is IDLE

 		GoServe(index);		
 		//pthread_mutex_unlock(&mtx5);
		
		if(Interrupt == 1){
			
			pthread_mutex_lock(&mtx3);
			pthread_cond_wait(&cond2, &mtx3);			//wait for the interrupt people to finish

			GoServe(index);
		}
		else{
			while(Line[linenum-1].cusnumber != 0){
				//printf("THIS IS INDEX  from  PICKUPUPUP   %d\n", index);
				Pickup();
			}
			
		}
		
		//pthread_cond_signal(&cond2,&mtx2);
	}
	else{		//the clerk is busy now
		 	
		if(customers[index].priority > GlobeServe.priority){
			pthread_mutex_lock(&mtx4);		// only one change the interrupt
			Interrupt = 1;
			pthread_mutex_unlock(&mtx4);

			printf("customer %2d interrupts the service of lower-priority customer %2d. \n", customers[index].cusnumber, GlobeServe.cusnumber);

			GoServe(index);
		}
		else{
			printf("customer %2d waits for the finish of customer %2d. \n", customers[index].cusnumber, GlobeServe.cusnumber);
			AddtoLine(index);
		}
	}
 }

void *customer_service(void* ptr){

	// puts(">>>>>>>>>1");
	int index;
	 pthread_mutex_lock(&mtx);
	 pthread_cond_wait(&cond, &mtx);
	 pthread_mutex_unlock(&mtx);


	Customer* cus= (Customer*)ptr;

	usleep(cus->arrivetime*1000000);	
	printf("customer %2d arrives: arrival time (%.2f), service time (%.1f), priority (%2d). \n", cus->cusnumber, (float)cus->arrivetime, (float)cus->servicetime, cus->priority);
	index = cus->cusnumber -1;

	service(index);

	return NULL;
}


int main ( int argc, char *argv[] ){

	pthread_mutex_init(&mtx, 0);			//initialize the mutex and conditional variable
	pthread_cond_init(&cond, 0);
	pthread_mutex_init(&mtx2, 0);
	pthread_cond_init(&cond2, 0);
	pthread_mutex_init(&mtx3, 0);
	pthread_cond_init(&cond3, 0);
	pthread_mutex_init(&mtx4, 0);
	pthread_mutex_init(&mtx5, 0);
	pthread_mutex_init(&mtx6, 0);
 

	char buffer[1024];
	char content[1024];
	int totalnum, i,incre,index;
//|| (argv[1] != "customers.txt")

	

	if(argc != 2) {
		printf("Warning: incorrect file format");
		return -1;
	}
	else{
		FILE *file = fopen( argv[1], "r" );
		if(file == 0){
			printf("Warning: file could not be opened");
			return -1;
		}
		else{
			//get time here

			//currenttime = time(NULL);
			
			
			fgets(buffer, 1024, file);
			totalnum=atoi(&buffer[0]);
			
			incre = 2;
			index = 2;
			//printf("%d\n", totalnum);
			for(i=0; i<totalnum; i++){
				fgets(buffer, 1024, file);
				sscanf(buffer, "%d:%d,%d,%d", &customers[i].cusnumber, &customers[i].arrivetime, &customers[i].servicetime, &customers[i].priority);

				
	
				//Createcustomer(&customers[i]);
			}
			
		}
		fclose(file);
	}

	//printf(" %d %d %d %d\n", customers[1].cusnumber,customers[1].arrivetime,customers[1].servicetime,customers[1].priority  );
	pthread_t customerthread[totalnum];
	int j = 0;
	for(j=0;j < totalnum; j++){
		//printf(" %d %d %d %d\n", customers[j].cusnumber,customers[j].arrivetime,customers[j].servicetime,customers[j].priority  );
		pthread_create(&customerthread[j], NULL, customer_service, &customers[j]);
	}
	

	//puts(">>>>>>>>>2");
	 sleep(1);
	 pthread_cond_broadcast(&cond);
	 startTime = clock();		//get the start time
	 //printf("THI SIS STARTTIEM %.2f\n", (float)startTime);


	for(j=0;j < totalnum; j++){
		pthread_join(customerthread[j], NULL);
	}
	return 0;
}
