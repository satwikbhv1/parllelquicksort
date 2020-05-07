#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void swap(int arr[], int low, int high)
{
	int temp=arr[low];
	arr[low]=arr[high];
	arr[high]=temp;
}
int partition(int arr[], int low, int high)  
{  
    int pivot = arr[high];
    int i=low-1;
    for(int j=low;j<=high-1;j++)  
    {  
        if(arr[j] < pivot)  
        {  
		i++;
		swap(arr,i,j);  
        }  
    }  
    swap(arr,i+1,high);  
    return (i+1);  
}  
void quicksort(int arr[], int low, int high)
{
	int pivot=low;
	int lchild=-1;
	int rchild=-1;
	if (high>low) 
	{
		int status;
		pivot=partition(arr,low,high);
		lchild=fork();
		if(lchild<0) 
		{
			perror("fork");
			exit(1);
		}
		if (lchild==0) 
		{
			printf("process id is: %d\n",getpid());
			quicksort(arr,low,pivot-1);
			//sleep(2);
			exit(0);
		} 
		else 
		{
			rchild=fork();
			if (rchild<0)
			{
				perror("fork");
				exit(1);
			}
			if (rchild==0) 
			{
				printf("process id is: %d\n",getpid());
				quicksort(arr,pivot+1,high);
				sleep(2);
				exit(0);
			}
		}
		waitpid(lchild,&status,0);
		waitpid(rchild,&status,0);
	}
}
int main()
{
	int *arr;
	int length;
	int shm_id;
	size_t shm_size;
	printf("process id is: %d\n",getpid());
	printf("Enter the size of the array: ");
	scanf("%d",&length);
	key_t key = IPC_PRIVATE;
	shm_size = length * sizeof(int);
	if((shm_id=shmget(key,shm_size,IPC_CREAT|0666))==-1) 
	{
		perror("shmget");
		exit(1);
	}
	if((arr=shmat(shm_id,NULL,0))==(int*)-1)
	{
		perror("shmat");
		exit(1);
	}
	printf("Enter the elements of the array: ");
	for(int i=0;i<length;i++)
	{
		scanf("%d",&arr[i]);
	}
	quicksort(arr,0,length-1);
	printf("The elements of the array after sorting(parallel quicksort) are : ");
	for (int i=0;i<length;i++)
	{
		printf("%d ",arr[i]);
	}
	printf("\n");
	if (shmdt(arr)==-1)
	{
		perror("shmdt");
		exit(1);
	}
	if (shmctl(shm_id,IPC_RMID,NULL)==-1)
 	{
		perror("shmctl");
		exit(1);
	}
	sleep(20);
	return 0;
}
