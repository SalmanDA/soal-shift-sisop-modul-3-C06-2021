#include<stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
int main()
{
	key_t key = 1234;
	int *value;
	int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
	value = shmat(shmid, NULL, 0);

	int a[4][3],b[3][6],c[4][6]={0};
	int i,j,k;

	for(i=0;i<4;i++){
		for(j=0;j<3;j++){
			scanf("%d",&a[i][j]);
		}
	}

	for(i=0;i<3;i++){
		for(j=0;j<6;j++){
			scanf("%d",&b[i][j]);
		}
	}

	for(i=0;i<4;i++){
		for(j=0;j<6;j++){
			for(k=0;k<3;k++){
				c[i][j] += a[i][k]*b[k][j];
			}
		}
	}

	for(i=0;i<4;i++){
		for(j=0;j<6;j++){
			printf("%d ",c[i][j]);
		}
		printf("\n");
	}

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 5; j++){
			*value = c[i][j];
			sleep(1);
		}
	}
	shmdt(value);
	shmctl(shmid, IPC_RMID, NULL);
}
