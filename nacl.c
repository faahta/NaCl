#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>


int k;
typedef struct sodium{	
	int *Na;
	sem_t *sem;
}sodium_t;

typedef struct chlorine {
	int index_cl;
	int *Cl;
	sem_t *sem;
	pthread_mutex_t lock_cl;
}chlorine_t;

sodium_t sodium;
chlorine_t chlorine;

static void *na_threads(void *arg){
	
	int *id = (int *)arg;	
	sem_wait(sodium.sem);
	pthread_mutex_lock(&chlorine.lock_cl);
		int cl_id = chlorine.Cl[chlorine.index_cl];
		sodium.Na[chlorine.index_cl] = *id;
		printf("Na %d Cl %d - ",sodium.Na[chlorine.index_cl], cl_id);
	pthread_mutex_unlock(&chlorine.lock_cl);
			
	sem_post(chlorine.sem);
	
}

static void *cl_threads(void *arg){
	int *id = (int *)arg;
	sleep(2);
	pthread_mutex_lock(&chlorine.lock_cl);
		chlorine.Cl[chlorine.index_cl] = *id;
	pthread_mutex_unlock(&chlorine.lock_cl);
	sem_post(sodium.sem);
	sem_wait(chlorine.sem);	
	
	pthread_mutex_lock(&chlorine.lock_cl);	
		int na_id = sodium.Na[chlorine.index_cl];
		printf("Na %d Cl %d \n",na_id,chlorine.Cl[chlorine.index_cl]);
	pthread_mutex_unlock(&chlorine.lock_cl);
				
	//sleep(2);
	pthread_mutex_lock(&chlorine.lock_cl);
		chlorine.index_cl++;
	pthread_mutex_unlock(&chlorine.lock_cl);
	
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
}

void init(){
	chlorine.index_cl =0;
	sodium.Na = (int *)malloc(k * sizeof(int));
	chlorine.Cl = (int *)malloc(k * sizeof(int));

	sodium.sem = (sem_t *)malloc(sizeof(sem_t));
	sem_init(sodium.sem, 0, 0);
	chlorine.sem = (sem_t *)malloc(sizeof(sem_t));
	sem_init(chlorine.sem, 0, 0);

	pthread_mutex_init(&chlorine.lock_cl, NULL);
}

int main(int argc, char *argv[]){
	k = atoi(argv[1]);
	pthread_t th_na,th_cl;
	init();
	pthread_create(&th_na, NULL, sodium_thread, (void *)NULL);
	pthread_create(&th_cl, NULL, chlorine_thread, (void *)NULL);
	pthread_exit(0);	
}


