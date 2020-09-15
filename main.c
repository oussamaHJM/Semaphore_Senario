#ifdef __unix__

#elif defined _WIN32
# include <windows.h>
#define sleep(x) Sleep(1000 * (x))
#endif
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include <time.h>

sem_t mutex,lect,tab2;
int *data = NULL, *datap = NULL;

int rcount = 0,n_lect = 0,n;

unsigned int rand_interval(unsigned int min, unsigned int max)
{
    srand(time(NULL));
    int r;
    const unsigned int range = 1 + max - min;
    const unsigned int buckets = RAND_MAX / range;
    const unsigned int limit = buckets * range;
    do
    {
        r = rand();
    } while (r >= limit);

    return min + (r / buckets);
}

void *reader(void *arg) {
  int f,val;
  //f = ((int) arg);
  f = rand_interval(0,n);
  sem_wait(&lect);
  n_lect = n_lect + 1;
  if(n_lect == 1)
    sem_wait(&mutex);
  sem_post(&lect);
  val = data[f];
  printf("Data liser par le lecteur %d est %d\n",f,val);
  sem_wait(&tab2);
  datap[f]=val;
  sem_post(&tab2);
  sleep(1);
  sem_wait(&lect);
  n_lect = n_lect - 1;
  if (n_lect == 0)
    sem_post(&mutex);
  sem_post(&lect);
  return NULL;
}

void *writer(void *arg)
{
  int f;
  f = ((int) arg);
  sem_wait(&mutex);
  data[f] = rand()%100;//rand_interval(2,10);
  printf("Data ecrite par l'ecrivain%d is %d\n",f,data[f]);
  sleep(1);
  sem_post(&mutex);
  //sem_wait(&tab2);
  data[f+1] = datap[f+1];
  //sem_post(&tab2);
  return NULL;
}

void tableau_rand(int *t,int n){
	int i;
	srand(time(NULL));
	for(i = 0; i<n;i++){
		t[i] = rand()%10;
		printf("DATA[%d] = %d",i,t[i]);
		if(i == n-1){
			printf("\n");
		}
	}
}

void tableau_zero(int *t,int n){
	int i;
	for(i=0;i<n;i++){
		t[i]=0;
	}
}


void printTable(int *t, int n){
    int i=0;
    for(i=0; i < n; i++ ){
        printf("[%d]|", t[i]);
        if(i==n-1)
        printf("END\n");
    }
}

int main(int argc, char **argv)
{
  int i;
  n = 10;// atoi(argv[1]);
  data =(int*) malloc(n*sizeof(int));
  datap=(int*) malloc(n*sizeof(int));
  tableau_rand(data,n);
  tableau_zero(datap,n);

  pthread_t rtid[20],wtid[30];
  sem_init(&mutex,0,1);
  sem_init(&lect,0,1);
  sem_init(&tab2,0,1);
  printTable(data,n);
  printTable(datap,n);
  for(i=0;i<10;i++)
  {
      srand(0);
    if (i<5)
      pthread_create(&wtid[i],NULL,writer,(void *)i);
    pthread_create(&rtid[i],NULL,reader,(void *)i);
  }
  for(i=0;i<10;i++)
  {
      srand(0);
    if(i<5)
      pthread_join(wtid[i],NULL);
    pthread_join(rtid[i],NULL);
  }
  printTable(data,n);
  printTable(datap,n);
  return 0;
}
