/* lcreate.c - lcreate, newlock */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

LOCAL int newlock();

/*------------------------------------------------------------------------
 * lcreate  --  create and initialize a lock, returning its id 
 *------------------------------------------------------------------------
 */
int lcreate(void)
{
	STATWORD ps;    
	int	lockid;

	disable(ps);
	if ( (lockid=newlock())==SYSERR ) {
		restore(ps);
		return(SYSERR);
	}
	/* sqhead and sqtail were initialized at system startup */
	restore(ps);
	return(lockid);
}

/*------------------------------------------------------------------------
 * newlock  --  allocate an unused lock and return its index
 *------------------------------------------------------------------------
 */
LOCAL int newlock()
{
	int	lockid;
	int	i;
	
	for (i=0 ; i<NLOCK ; i++) {
		lockid=nextlock--;
		if (nextlock < 0)
			//here lies the code for reused locks
			nextlock = NLOCK-1;
		if (locktab[lockid].lstate==LFREE) {
			locktab[lockid].lstate = LUSED;
			return(lockid);
		}
	}
	return(SYSERR);
}
