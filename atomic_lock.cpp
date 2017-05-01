#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<fcntl.h>
#include<string.h>

#include "timer.h"

#define ATOMIC_LOCK

int* mutex = 0;

typedef struct
{
	char name[4];
    int count;
}data_info;

data_info* data;

void test_atomic_lock_func()
{
	int i = 0;
	pid_t pid = getpid();
	for(i = 0; i < 10000; i++)
    {
#ifdef ATOMIC_LOCK
		while(!(__sync_bool_compare_and_swap (mutex,0, pid))) usleep(1);
#endif
		// 临界区
		data->count++;
#ifdef ATOMIC_LOCK
		__sync_bool_compare_and_swap (mutex, pid, 0);
#endif
    }

	exit(0);
}

int main(int argc, const char *argv[])
{
    Timer timer;
    timer.Start();
    int process_count = 4;
    int i = 0;
    pid_t pid;
    int     stat;
    mutex = (int*)mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE,
    		                         MAP_ANON|MAP_SHARED, -1, 0);
    data = (data_info*)mmap(NULL, sizeof(data_info), PROT_READ|
    		                         PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0);

    for(i = 0; i < process_count; i++)
    {
    	pid = fork();
    	if (pid == 0)
    	{
    		// 子进程
    		test_atomic_lock_func();
    	}
    }

    // 等待所有进程执行完毕
    while(wait(NULL)!=-1)
    	usleep(1);

    // 计算时间
    timer.Stop();
    timer.Cost_time();
    printf("结果:count = %d\n", (data->count));

    return 0;
}
