
#include <linux/unistd.h>

#include <syscall.h>

#include <sys/types.h>

#include <stdio.h>



struct process

{

	int pid;

	int depth;

};



struct process a[512];



int main()

{

	int i, j;



	printf("the result is:%d\n", syscall(223, &a));



	for (i = 0; i < 512; i++)

	{

		for (j = 0; j < a[i].depth; j++)

			printf("|-");

		printf("%d\n", a[i].pid);



		if (a[i + 1].pid == 0)

			break;

	}



	return 0;

}


