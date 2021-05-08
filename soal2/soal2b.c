#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <pthread.h>

int arr[2];

int faktorial(int a)
{
	if(a==0)
		return 1;
	else
		return (a * faktorial(a-1));
}

void *operasi(void* args)
{
	if(arr[0] >= arr[1]){
		printf("%d", faktorial(arr[0])/faktorial(arr[0]-arr[1]) );
	} else
	if(arr[0] < arr[1]){
		printf("%d", faktorial(arr[0]) );
	} else 
	if(arr[0]==0 || arr[1]==0){
		printf("0");
	}

//	printf("%d", arr[0]);
}

int main()
{
	key_t key = 1234;
	int *value;
	int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
	value = shmat(shmid, NULL, 0);

	int c[4][6],copy[4][6];
	int i,j;

	for(i=0;i<4;i++){
		for(j=0;j<6;j++){
			scanf("%d",&copy[i][j]);
		}
	}
	
	pthread_t tid[24];
	int index=0;
	for(i=0;i<4;i++){
		for(j=0;j<6;j++){
			c[i][j]=*value;
			arr[0]=c[i][j];
			arr[1]=copy[i][j];
			pthread_create(&tid[index], NULL, &operasi, NULL);
			printf("\t");
			index++;
			sleep(1);
		}
		printf("\n");
	}

	for (int i = 0; i < index; i++) {
	        pthread_join(tid[i], NULL);
	}

	shmdt(value);
	shmctl(shmid, IPC_RMID, NULL);

	return 0;
}
