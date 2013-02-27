/* sched.c - exp_dist_nextproc, reallocate, linux_next_proc */

#include <kernel.h>
#include <q.h>
#include <proc.h>
#include <sched.h>

int exp_dist_nextproc(int rprio)
{
	int proc, prev_proc;

	proc = q[rdytail].qprev;
	prev_proc = q[proc].qprev;

	while( (rprio < q[prev_proc].qkey) && (prev_proc < NPROC) ){
		if(q[proc].qkey != q[prev_proc].qkey){
			proc = prev_proc;
		}
		prev_proc = q[prev_proc].qprev;
	};
	if(proc >= NPROC)
		return(NULLPROC);
	else
		return(proc);
}

void reallocate()
{
	int proc;

	q[rdyhead].qnext = rdytail;
	q[rdytail].qprev = rdyhead;

	for(proc = 0; proc < NPROC; proc++){
	  if(proctab[proc].pstate != PRFREE){

		proctab[proc].counter = proctab[proc].pprio + \
			(int) proctab[proc].counter/2;

		if(proctab[proc].pstate == PRREADY){
		  insert(proc, rdyhead, (proctab[proc].pprio + \
			proctab[proc].counter));
		}	
	  }
	}
}

int linux_next_proc()
{
	int proc;

	proc = q[rdytail].qprev;
	while((proc < NPROC) && (proctab[proc].counter < 1) ){
		proc = q[proc].qprev;
	}
	if(q[proc].qkey > 0){
		return(proc);
	} else {
		/* epoch */
		reallocate();
		return(q[rdytail].qprev);
	}
}
