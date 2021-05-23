#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <pthread.h>

int arr[2];

long long faktorial(int a)
{
	if(a==0)
		return 1;
	else
		return (a * faktorial(a-1));
}

void *operasi(void* args)
{
	if(arr[0] >= arr[1]){
		printf("%lld\t", faktorial(arr[0])/faktorial(arr[0]-arr[1]) );
	} else
	if(arr[0] < arr[1]){
		printf("%lld\t", faktorial(arr[0]) );
	} else 
	if(arr[0]==0 || arr[1]==0){
		printf("0\t");
	}

//	printf("%d", arr[0]);
}

int main()
{
	key_t key = 1234;
	int (*value)[6];
	int shmid = shmget(key, sizeof(int[4][6]), IPC_CREAT | 0666);
	value = shmat(shmid, NULL, 0);

	int i,j;

	int copy[4][6] = {{5,4,6,4,6,5},
			{5,4,6,4,6,5},
			{5,4,6,4,6,5},
			{0,0,0,0,0,0}};

	pthread_t tid[24];
	int index=0;
	for(i=0;i<4;i++){
		for(j=0;j<6;j++){
			long long *val = malloc(sizeof(long long[4][6]));
		        *val = value[i][j];
		        arr[0] = value[i][j];
			arr[1] = copy[i][j];
		        pthread_create(&tid[index], NULL, &operasi, NULL);
		        sleep(1);
		        index++;
//			printf("%d ", value[i][j]);
		}
		printf("\n");
	}


	for (i = 0; i < index; i++) {
        pthread_join(tid[i], NULL);
	}

	shmdt(value);
	shmctl(shmid, IPC_RMID, NULL);

	return 0;
}
