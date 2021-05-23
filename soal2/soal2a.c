#include<stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
int main()
{
	key_t key = 1234;
	int (*value)[6];
	int shmid = shmget(key, sizeof(int[4][6]), IPC_CREAT | 0666);
  	value = shmat(shmid, NULL, 0);

//	int a[4][3] = {{1,2,3},
//			{1,2,3},
//			{1,2,3},
//			{0,0,0}};
//	int b[3][6] = {{1,2,3,1,2,3},
//			{1,2,3,1,2,3},
//			{0,0,0,0,0,0}};

	int a[4][3], b[3][6];
	int c[4][6]={0};
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
				value[i][j] += a[i][k]*b[k][j];
			}
		}
	}

	for(i=0;i<4;i++){
		for(j=0;j<6;j++){
			printf("%d\t",value[i][j]);
		}
		printf("\n");
	}

//	shmdt(value);
//	shmctl(shmid, IPC_RMID, NULL);
}
