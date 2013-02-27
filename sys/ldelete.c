/* ldelete.c - ldelete */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * ldelete  --  delete a lock by releasing its table entry
 *------------------------------------------------------------------------
 */
SYSCALL ldelete(int lockid)
{
	STATWORD ps;    
	int	pid, i;
	struct	lentry	*lptr;

	disable(ps);
	if (isbadlock(lockid) || locktab[lockid].lstate==LFREE) {
		restore(ps);
		return(SYSERR);
	}
	kprintf("ldelete:%d\n", lockid);
	lptr = &locktab[lockid];
	lptr->lstate = LFREE;
	/* set the 'history' mask for the processes currently	*
	 * holding the lock.					*/
	if(lptr->umask) {
		for(i=0; i<25; i++){
			if(ismaskset(lptr->umask, i)){
				proctab[i+25].lockret = DELETED;
				kprintf("pid:%d\n", i+25);
				if(lockid < 25){
				   proctab[i+25].lhmask = setmask(proctab[i+25].lhmask, lockid);
				} else{
				   proctab[i+25].uhmask = setmask(proctab[i+25].uhmask, lockid-25);
				}
			}
		}
	}
	if(lptr->lmask) {
		for(i=0; i<25; i++){
			if(ismaskset(lptr->lmask, i)){
				proctab[i].lockret = DELETED;
				kprintf("pid:%d\n", i);
				if(lockid < 25)
				   proctab[i].lhmask = setmask(proctab[i].lhmask, lockid);
				else
				   proctab[i].uhmask = setmask(proctab[i].uhmask, lockid-25);
			}
		}
	}
	/* For all processes in the Lock queue, perform the same operation	*
	 * as given above.							*/
	if (nonempty(lptr->lqhead)) {
		while( (pid=getfirst(lptr->lqhead)) != EMPTY)
		  {
		    proctab[pid].lockret = DELETED;
		    if(lockid < 25){
			proctab[pid].lhmask = setmask(proctab[pid].lhmask, lockid);
		    } else {
			proctab[pid].uhmask = setmask(proctab[pid].uhmask, lockid-25);
		    }
		    ready(pid,RESCHNO);
		  }
		resched();
	}
	restore(ps);
	return(OK);
}
