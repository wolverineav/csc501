#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>

#define DEFAULT_LOCK_PRIO 20
int s;
void reader (char *msg, int lck, int lprio)
{
        int     ret, i;
	kprintf("%s starts with priority:%d\n", msg, proctab[currpid].pinh);
        kprintf ("%s: to acquire read lock\n", msg);
	if(s==1){
		wait(lck);
	} else {
	        lock (lck, READ, lprio);
	}
        kprintf ("%s: acquired lock,goto sleep 2s and then print %s 5 times.\n", msg);
	sleep(2);
	ret = 5000000;
	i = 1;
        while(ret){
		if(!(ret%1000000)){
			kprintf("%d %s\n", i, msg);
			i++;
		}
		ret--;
	}
	kprintf("%s executed with priority %d\n", msg, proctab[currpid].pinh);
        kprintf ("%s: to release lock\n", msg);
	if(s==1){
		signal(lck);
	} else {
		releaseall (1, lck);
	}
}

void writer (char *msg, int lck, int lprio)
{
	int i;
	kprintf("%s starts with priority %d\n", msg, proctab[currpid].pinh);
	kprintf("%s goes to sleep for 5s\n", msg);
	sleep(5);
	kprintf ("%s: to acquire write lock, waiting on lower priority reader :(\n", msg);
	if(s==1){
		wait(lck);
	} else {
	        lock(lck, WRITE, lprio);
	}
        kprintf ("%s: acquired lock, write X and return\n", msg);
	kprintf("%s wrote x :)\n", msg);
        kprintf ("%s: to release lock\n", msg);
	if(s==1){
		signal(lck);
	} else {
	        releaseall (1, lck);
	}
}

void infinite_loop(char *msg)
{
	int i;

	kprintf("%s started with priority %d\nDoing some never ending job..\n", msg, proctab[currpid].pinh);
	while(1){
		i++;
	}
}

void test ()
{
        int     lck;
        int     rd1, ip1;
        int     wr1;

	kprintf("Example High priority Writer stuck on Low priority Reader.\n");

	if(s==1){
		kprintf("using semaphores\n");
		lck = screate(1);
	} else {
		kprintf("using lock\n");
	        lck  = lcreate();
	}

	rd1 = create(reader, 2000, 20, "reader", 3, "reader", lck, 30);
	ip1 = create(infinite_loop, 2000, 40, "infinite_loop", 1, "infinite_loop");
        wr1 = create(writer, 2000, 60, "writer", 3, "writer", lck, 20);
	
        kprintf("MAIN - Start reader..\n");
        resume(rd1);
        sleep (1);

        kprintf("MAIN - Start writer..\n");
        resume(wr1);
        sleep (1);

        kprintf("MAIN - Start infinite loop process and goto sleep\n");
        resume (ip1);

        sleep (20);
}

int main( )
{
	int i;
	int count = 0;
	char buf[8];

	kprintf("1. Semaphore\n2. Lock\n");
	kprintf("Please Input:\n");
	while ((i = read(CONSOLE, buf, sizeof(buf))) <1);
	buf[i] = 0;
	s = atoi(buf);
	test();
}
