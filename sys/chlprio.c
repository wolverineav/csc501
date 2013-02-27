/* chlprio.c - chlprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * chlprio  --  change the priority of running processes in a lock
 *------------------------------------------------------------------------
 */
int chlprio(int lockid, int newprio)
{
	STATWORD ps;    
	struct	lentry	*lptr;
	int i;

	disable(ps);
	if (isbadlock(lockid) || newprio<=0 ||
	    (lptr = &locktab[lockid])->lstate == LFREE) {
		restore(ps);
		return(SYSERR);
	}

	lptr->lprio = newprio;
	if(lptr->umask > 0){
		for(i=0; i<25; i++){
			if(ismaskset(lptr->umask,i) && proctab[i+25].pinh < newprio){
				chpprio(i+25, newprio);
			}
		}
	}
	if(lptr->lmask > 0) {
                for(i=0; i<25; i++){
                        if(ismaskset(lptr->lmask, i) && proctab[i+1].pinh < newprio)
                                chpprio(i+1, newprio);
		}
	}
	restore(ps);
	return(newprio);
}
