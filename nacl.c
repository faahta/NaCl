#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>

#define NA 0
#define CL 1

int k;
typedef struct sodium{
	int count;
	sem_t *s_Na;
	pthread_mutex_t lock_na;
}sodium_t;

typedef struct chlorine {
	int count;
	sem_t *s_Cl;
	pthread_mutex_t lock_cl;
}chlorine_t;

int *list[2];

sodium_t sodium;
chlorine_t chlorine;
pthread_mutex_t mutex_first;
int first = 1;

static void *na_threads(void *arg){
	
	int *id = (int *)arg;	
	//sleep(1);
	int currNa;
	pthread_mutex_lock(&sodium.lock_na);
		currNa = sodium.count++;
	pthread_mutex_unlock(&sodium.lock_na);
	list[NA][currNa] = *id;
	
	sem_post(chlorine.s_Cl);
	sem_wait(sodium.s_Na);
	
	pthread_mutex_lock(&mutex_first);
		if(first){
			first = 0;
			printf("id %d - Na%d Cl%d\t", *id, list[NA][currNa], list[CL][currNa]);
		} else{
			first =1;
			printf("id %d - Na%d Cl%d\n", *id, list[NA][currNa], list[CL][currNa]);
		}
	pthread_mutex_unlock(&mutex_first);

}

static void *cl_threads(void *arg){
	int *id = (int *)arg;	
	//sleep(1);
	int currCl;
	pthread_mutex_lock(&chlorine.lock_cl);
		currCl = chlorine.count++;
	pthread_mutex_unlock(&chlorine.lock_cl);
	list[CL][currCl] = *id;
	
	sem_post(sodium.s_Na);
	sem_wait(chlorine.s_Cl);
	
	pthread_mutex_lock(&mutex_first);
		if(first){
			first = 0;
			printf("id %d - Na%d Cl%d\t", *id, list[NA][currCl], list[CL][currCl]);
		} else{
			first =1;
			printf("id %d - Na%d Cl%d\n", *id, list[NA][currCl], list[CL][currCl]);
		}
	pthread_mutex_unlock(&mutex_first);
}


static void *sodium_thread(void *arg){
	int i;
	pthread_t *na_new;
	na_new = (pthread_t *)malloc(k * sizeof(pthread_t));
	int *pi;
	for(i=0; i < k; i++){
		sleep(rand()%4);
		pi = (int *)malloc(sizeof(int));
		*pi = i;
		pthread_create(&na_new[i], NULL, na_threads, (void *)pi);
	}
	for(i=0; i<k; i++)
		pthread_join(na_new[i], NULL);
	return 0;

}

static void *chlorine_thread(void *arg){
	int i;
	pthread_t *cl_new;
	cl_new = (pthread_t *)malloc(k * sizeof(pthread_t));
	int *pi;
	for(i = k; i <=2*k-1; i++){
		sleep(rand()%4);
		pi = (int *)malloc(sizeof(int));
		*pi = i;
		pthread_create(&cl_new[i-k], NULL, cl_threads, (void *)pi);
		
	}
	for(i=k; i<k; i++)
		pthread_join(cl_new[i], NULL);
	//return 0;
}

void init(){
	sodium.count = 0;
	pthread_mutex_init(&sodium.lock_na, NULL);
	sodium.s_Na = (sem_t *)malloc(sizeof(sem_t));
	sem_init(sodium.s_Na, 0, 0);
	
	chlorine.count = 0;
	pthread_mutex_init(&chlorine.lock_cl, NULL);
	chlorine.s_Cl = (sem_t *)malloc(sizeof(sem_t));
	sem_init(chlorine.s_Cl, 0, 0);
	
	pthread_mutex_init(&mutex_first, NULL);
	
	list[NA] = (int *) malloc(k *sizeof(int));
  	list[CL] = (int *) malloc(k * sizeof(int));
}

int main(int argc, char *argv[]){
	if(argc!=2){
		printf("usage: %s k\n", argv[0]);
		exit(1);
	}
	setbuf(stdout, 0);
	k = atoi(argv[1]);
	pthread_t th_na,th_cl;
	init();
	pthread_create(&th_na, NULL, sodium_thread, (void *)NULL);
	pthread_create(&th_cl, NULL, chlorine_thread, (void *)NULL);
	pthread_exit(0);	
}


