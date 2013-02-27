/* releaseall.c - releaseall */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  releaseall -- signal 'numlocks' # of locks
 *------------------------------------------------------------------------
 */
int releaseall(int numlocks, long args, ...)
{
	STATWORD ps;    
	struct	lentry	*lptr;
	int lockid, tailproc, writer, reader;
        unsigned long *lockids;

        lockids = (unsigned long *)((&args) + (numlocks-1));

	disable(ps);
	for(; numlocks > 0; numlocks--){
	  /* Process each LockDescriptor passed */
	  lockid = *lockids--;
	  if (isbadlock(lockid) || (lptr = &locktab[lockid])->lstate==LFREE) {
		restore(ps);
		return(SYSERR);
	  }
	  if(lockid < 25){
		proctab[currpid].lmask = setmask(proctab[currpid].lmask, lockid);
	  } else {
		proctab[currpid].umask = setmask(proctab[currpid].umask, lockid-25);
	  }

	  /* re-assign lock only when nobody is using lock.	*
	   * While it is used by Readers, LOCK function 	*
	   * handles whom to insert according to priority	*/
	  if(--(lptr->nreaders) == 0){
		/* Nobody holding the lock, return lprio to 0	*/
		lptr->lprio = 0;
		/* If no process waiting, return	*/
		if(nonempty(lptr->lqhead)){
		  /* get the first Writer if present or HEAD	*/
		  writer = firstwriter(lptr->lqtail);
		  /* get the first "Eligible" Reader		*/
		  reader = firstreader(writer);
		  if(writer < NPROC){
		  /* If Writer in wait queue...		*/
			if((proctab[writer].lockcount == 3 && reader == writer) || \
				(q[writer].qnext >= NPROC) ){
			/* come here only when there is NO Reader		*
			 * OR no eligible high priority Reader when count is 3	*/
                                proctab[writer].plock = 0;
                                proctab[writer].lockcount = 0;
                                q[writer].locktype = NOP;
                                lptr->currop = WRITE;
				lptr->nreaders++;
				lptr->lprio = proctab[writer].pinh;
				if(lockid < 25){
					proctab[writer].lmask = setmask(proctab[writer].lmask, lockid);
				} else {
					proctab[writer].umask = setmask(proctab[writer].umask, lockid-25);
				}
	                        if(currpid < 25){
	                                lptr->lmask = setmask(lptr->lmask, currpid);
	                        } else{
	                                lptr->umask = setmask(lptr->umask, currpid-25);
				}
				/* call chprio with 0 new priority so that it works	*
				 * as per my logic.					*/
				//chpprio(writer, 0);
                                ready(dequeue(writer), RESCHNO);
				goto outofloop;
                                //restore(ps);
                                //return(OK);
			}
			/* Increment count for each pass, but not beyond 3	*/
			if((proctab[writer].lockcount < 3) && \
				(reader != q[writer].qnext) && (reader != writer))
				proctab[writer].lockcount++;
		  }
		  if(reader == writer){	/* count is not 3 and no high priority Reader */
			reader = q[writer].qnext;
			if(proctab[writer].lockcount < 3)
				proctab[writer].lockcount++;
		  }
		  do{
			/* lqtail changes as we remove processes		*
			 * 'reader' points to the last proc to be assigned lock	*/
			tailproc = lastid(lptr->lqtail);
			proctab[tailproc].plock = 0;
			q[tailproc].locktype = NOP;
			lptr->nreaders++;
			if(lptr->lprio > proctab[tailproc].pinh)
				lptr->lprio = proctab[tailproc].pinh;
			if(currpid < 25)
				lptr->lmask = setmask(lptr->lmask, currpid);
			else
				lptr->umask = setmask(lptr->umask, currpid-25);
                        if(lockid < 25){
	                        proctab[tailproc].lmask = setmask(proctab[tailproc].lmask, lockid);
                        } else {
                                proctab[tailproc].umask = setmask(proctab[tailproc].umask, lockid-25);
                        }

			ready(dequeue(tailproc), RESCHNO);
		  } while(tailproc != reader);
		  lptr->currop = READ;
		}
	  }
	  outofloop:	kprintf("");
	}
	chpprio(currpid, 0);
	resched();
	restore(ps);
	return(OK);
}
