/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <math.h>
#include <sched.h>


unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
extern int a_cnt, b_cnt, c_cnt;
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	int rand_prio;
	int next_proc;

	/* no switch needed if current process priority higher than next*/
	/* next process chosen based on scheduler policy		*/

    if(LINUXSCHED == scheduler_class){

	optr = &proctab[currpid];
	if(optr->pstate == PRCURR) optr->pstate = PRREADY;

	if(optr->pstate == PRREADY){
	  if(preempt <= 0){
		insert(currpid,rdyhead,optr->pprio);
		optr->counter = 0;
	  } else {
		insert(currpid,rdyhead,	(optr->pprio + preempt));
		optr->counter = preempt;
	  }
	}
	optr->counter = preempt;
	nptr = &proctab[ (currpid = dequeue(linux_next_proc())) ];
	preempt = nptr->counter;
	if(optr == nptr){
		nptr->pstate = PRCURR;
		return(OK);
	}

     } else {
	
	if(EXPDISTSCHED == scheduler_class){

		rand_prio = (int) expdev(0.1);
		next_proc = exp_dist_nextproc(rand_prio);

		if ( ( (optr= &proctab[currpid])->pstate == PRCURR) && \
			( (next_proc == NULLPROC) || \
			( (q[next_proc].qkey > optr->pprio) && \
			  (optr->pprio > rand_prio) ) ) ) {
			#ifdef  RTCLOCK
		        	preempt = QUANTUM;
			#endif
			return(OK);
		}
	} else {
		if ( ( (optr= &proctab[currpid])->pstate == PRCURR) && \
				(lastkey(rdytail)<optr->pprio)) {
			return(OK);
		}
	}

	/* force context switch */

	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	}

	/* remove highest priority process at end of ready list */
	/* highest priority decided according to scheduler	*/

	if(EXPDISTSCHED == scheduler_class){
		nptr = &proctab[(currpid = exp_dist_getlast(next_proc))];
	} else {
		nptr = &proctab[ (currpid = getlast(rdytail)) ];
	}

	nptr->pstate = PRCURR;		/* mark it currently running	*/
	#ifdef	RTCLOCK
		preempt = QUANTUM;	/* reset preemption counter 	*/
	#endif
    }

	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

	/* The OLD process returns here when resumed. */
	return OK;
}
