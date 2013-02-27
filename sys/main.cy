#include <conf.h>
#include <kernel.h>
#include <q.h>
#include <sched.h>
#include <stdio.h>
#include <math.h>

#include <proc.h>

#define LOOP	50

int prA, prB, prC;
int proc_a(), proc_b(), proc_c();
int proc(char c);
int proc_basic(char c);
volatile int a_cnt = 0;
volatile int b_cnt = 0;
volatile int c_cnt = 0;
volatile int s = 0;

int main() {
	int i,j,proc;
	int count = 0;

	srand(1234);

kprintf("queue:\n");
proc = q[rdyhead].qnext;
do{
kprintf("%s->", proctab[proc].pname);
proc = q[proc].qnext;
}while(proc < NPROC);
kprintf("\n");

			prA = create(proc_a, 2000, 10, "proc A", 1, 'A');
			prB = create(proc_b, 2000, 20, "proc B", 1, 'B');
			prC = create(proc_c, 2000, 30, "proc C", 1, 'C');
			resume(prA);

kprintf("queue:\n");
proc = q[rdyhead].qnext;
do{
kprintf("%s->", proctab[proc].pname);
proc = q[proc].qnext;
}while(proc < NPROC);
kprintf("\n");

			resume(prB);
kprintf("queue:\n");
proc = q[rdyhead].qnext;
do{
kprintf("%s->", proctab[proc].pname);
proc = q[proc].qnext;
}while(proc < NPROC);
kprintf("\n");
			resume(prC);		
kprintf("queue:\n");
proc = q[rdyhead].qnext;
do{
kprintf("%s->", proctab[proc].pname);
proc = q[proc].qnext;
}while(proc < NPROC);
kprintf("\n");
		sleep(10);
		kill(prA);
		kill(prB);
		kill(prC);

kprintf("\nTest2 RESULT: A = %d, B = %d, C = %d\n", a_cnt, b_cnt, c_cnt);

return 0;

}

proc_basic(char c) {
	int count = 0;

	//kprintf("\nStart %c...\n", c);
	while (count++ < 1000) {
		kprintf("%c", c);
	}
}


proc_a(c)
	char c; {
	int i;
	kprintf("Start... %c\n", c);
	if (s < 4) {
		b_cnt = 0;
		c = 0;
	}

	while (1) {
		for (i = 0; i < 10000; i++)
			;
		a_cnt++;
	}
}

proc_b(c)
	char c; {
	int i;
	if (s < 4) {
		a_cnt = 0;
		c_cnt = 0;
	}

	kprintf("Start... %c\n", c);
	while (1) {
		for (i = 0; i < 10000; i++)
			;
		b_cnt++;
	}
}

proc_c(c)
	char c; {
	int i;
	if (s < 4) {
		a_cnt = 0;
		b_cnt = 0;
	}

	kprintf("Start... %c\n", c);
	while (1) {
		for (i = 0; i < 10000; i++)
			;
		c_cnt++;
	}
}

proc(char c) {
	int i;
	int count = 0;

	while (count++ < LOOP) {
		kprintf("%c", c);
		if (s == 4 || s == 5 || s == 7) {
			for (i = 0; i < 10000000; i++)
				;
		} else if (s == 6) {
			sleep(1);
		}
	}

}
