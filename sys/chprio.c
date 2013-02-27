/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  * chprio  --  change the scheduling priority of a process
 *   *------------------------------------------------------------------------
 *    */
SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	pptr->pprio = newprio;
	if(EXPDISTSCHED == scheduler_class){
		dequeue(pid);
		insert(pid, rdyhead, pptr->pprio);
	}
	/* If legitimate call to chprio is made, and its	*
	 * priority is > pinh, change pinh			*/
	if(pptr->pinh < pptr->pprio){
		chpprio(pid, pptr->pprio);
	}
	restore(ps);
	return(newprio);
}
