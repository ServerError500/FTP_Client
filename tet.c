#include "stdio.h"

int main(){
    int num=100;
	int *p;
	p=&num;
	*p=1000;
	printf("%d\n",num);
	printf("p的地址为%p\n",p);

    return 0;
}

