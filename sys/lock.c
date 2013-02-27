/* lock.c - lock */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

int lock(int lockid, int locktype, int waitpriority)
{
	STATWORD ps;    
	struct	lentry	*lptr;
	struct	pentry	*pptr;
	int tailproc;

	disable(ps);
	if (isbadlock(lockid) || (lptr = &locktab[lockid])->lstate==LFREE) {
		restore(ps);
		return(SYSERR);
	}
	pptr = &proctab[currpid];
	/* If lock was deleted previously when process was either waiting on it	*
	 * or accessing the information, it returns error			*/
	if( (lockid <25 && ismaskset(pptr->lmask, lockid)) || \
		(lockid >= 25 && ismaskset(pptr->umask, lockid-25)) ) {
                restore(ps);
                return(SYSERR);
	}

	if(lptr->nreaders > 0){
	/* select based on who currently has lock*/
	  tailproc = lastid(lptr->lqtail);
	  if( (WRITE == locktype) || (WRITE == lptr->currop) || \
		( (q[tailproc].qkey > waitpriority) || \
		( q[tailproc].locktype == READ && q[tailproc].qkey == waitpriority) ) ) {
		/* In this condition, queue the process	*/
	        pptr->pstate = PRLOCK;
	        pptr->plock = lockid;
	        pptr->lockret = OK;
	        linsert(locktype, waitpriority, lptr->lqtail);
		if(pptr->pinh > lptr->lprio){
			chlprio(lockid, pptr->pinh);
		}
		resched();
		restore(ps);
		return pptr->lockret;
	  }
	  if(pptr->pinh < q[tailproc].qkey){
		chpprio(currpid, q[tailproc].qkey);
	  }
	}
	/* nobody holds the lock currently, let the process acquire Lock */
	if(currpid < 25){
		lptr->lmask = setmask(lptr->lmask, currpid);
	} else {
		lptr->umask = setmask(lptr->umask, currpid-25);
	}
	if(lockid < 25){
		pptr->lmask = setmask(pptr->lmask, lockid);
	} else {
		pptr->umask = setmask(pptr->umask, lockid-25);
	}
	lptr->currop = locktype;
	lptr->nreaders++;
	lptr->lprio = pptr->pinh;
	restore(ps);
	return(OK);
}
