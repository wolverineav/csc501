/* chpprio.c - chpprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * chpprio  --  change the scheduling priority of a process according
 * 		to the Lock that it holds
 *------------------------------------------------------------------------
 */
int chpprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;
	int i, next;

	disable(ps);
	/* made it to bypass priority 0			*/
	if (isbadpid(pid) || newprio<0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}

	/* my chprio request. find highest priority lock */
	if(pptr->umask > 0){
		for(i=0; i<25; i++){
			if(ismaskset(pptr->umask,i) && locktab[i+25].lprio > newprio)
				newprio = locktab[i+25].lprio;
		}
	}
	if(pptr->lmask > 0) {
                for(i=0; i<25; i++){
                        if(ismaskset(pptr->lmask,i) && locktab[i].lprio > newprio)
                                newprio = locktab[i].lprio;
                }
	}
	  /* does not hold any lock */
	if(newprio == 0 || newprio < pptr->pprio)
		newprio = pptr->pprio;
	/* If its waiting on a lock, increase the Lock's priority	*/
	if(pptr->plock > -1){
		next = q[pid].qnext;
		if(isbadpid(next)){
			chlprio(pptr->plock, newprio);
		} else {
			chpprio(next,newprio);
		}
	}
	pptr->pinh = newprio;
	/* dequeue and insert again so that resched works correctly */
	if(PRREADY == pptr->pstate){
		ready(dequeue(pid), RESCHNO);
	}
	restore(ps);
	return(newprio);
}
