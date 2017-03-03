#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#define mx 100
#define NUM_THREADS 100

pthread_t thread_id[NUM_THREADS];
pthread_mutex_t mutex1,mutex2;
pthread_cond_t cond1,cond2;
int m[mx][mx],cnt=0,x;
int n,k;

int cnt_row=0,cnt_column=0;		


struct data{
	int start,end,type;
};

struct data params[NUM_THREADS];

void design(int n){
	int i;
	for(i=0;i<7*n+1;i++){
		if(i%7!=0)printf("-");
		else printf("+");
	}
	printf("\n");
}

void print(int m[][mx],int n){
	int i,j;
	design(n);
	for(i=0;i<n;i++){
		for(j=0;j<n;j++){
			printf("| %4d ",m[i][j]);
		}
		printf("|\n");
		design(n);
	}
}

void swap(int *x,int *y){
	int tmp=*x;
	*x=*y;
	*y=tmp;
}


void* start_routine(void* p){
	struct data q=*((struct data*)p);
	int i,j;

	if(q.type==0){
		pthread_mutex_lock(&mutex1);
		while(cnt_column!=x && cnt_column!=0 ){
			pthread_cond_wait(&cond2, &mutex2);
		}	
		pthread_mutex_unlock(&mutex1);
		for(i=q.start;i<=q.end;i++){
			for(j=0;j<n-1;j++)swap(&m[i][j],&m[i][j+1]);		
		}
		q.type=1;
		pthread_mutex_lock(&mutex1);
		cnt_row++;
		if(cnt_row==x){
			pthread_cond_signal(&cond1);
			cnt_row=0;
		}
		pthread_mutex_unlock(&mutex1);
		
		//printf("Check\n");
		//print(m,n);
	}

	
	
	if(q.type==1){
		
		pthread_mutex_lock(&mutex2);
		while(cnt_row!=x && cnt_row!=0){
			pthread_cond_wait(&cond1, &mutex1);
		}	
		pthread_mutex_unlock(&mutex2);
		for(i=q.start;i<=q.end;i++){
			for(j=n-1;j>0;j--)swap(&m[j][i],&m[j-1][i]);
		}
		pthread_mutex_lock(&mutex2);
		cnt_column++;
		if(cnt_column==x){
			pthread_cond_signal(&cond2);
			cnt_column=0;	
		}
		printf("Check  %lu\n row=%d column=%d\n",pthread_self(),cnt_row,cnt_column);		
		print(m,n);

		pthread_mutex_unlock(&mutex2);
		q.type=0;	
	}

}


int main(){
	int i,j;

	scanf("%d",&n);
	for(i=0;i<n;i++)
		for(j=0;j<n;j++)scanf("%d",&m[i][j]);

	printf("\nPrinting the table...\n\n");
	print(m,n);

	printf("\n");

	printf("Enter k and x respectively\n");
	scanf("%d %d",&k,&x);

	pthread_mutex_init(&mutex1, NULL);
	pthread_cond_init(&cond1, NULL);
	pthread_mutex_init(&mutex2, NULL);
	pthread_cond_init(&cond2, NULL);


	for(i=0;i<x;i++){
		params[i].start=i*(n/x);
		params[i].end=(i+1)*(n/x)-1;
		params[i].type=0;
		if(i==x-1)break;
		printf("params[%d].start=%d params[%d].end=%d \n",i,params[i].start,i,params[i].end);
	}
	params[x-1].end=n-1;
	printf("params[%d].start=%d params[%d].end=%d \n",i,params[i].start,i,params[i].end);

	for(i=0;i<x;i++){
		pthread_create(&thread_id[i], NULL, start_routine, (void *) &params[i]);
	}

	for(i=0; i<x; i++){
		pthread_join(thread_id[i], NULL);
	}
	printf("\nPrinting the output table...\n\n");
	print(m,n);


}