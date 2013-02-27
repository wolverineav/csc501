/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev, next, i;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;
	case PRLOCK:	/* release all locks held	*/
		if(pptr->umask != 0){
               		for(i=0; i<25; i++){	
				if(ismaskset(pptr->umask,i))
                                	releaseall(1, i+25);
                	}
		}
		if(pptr->lmask != 0) {
			for(i=0; i<25; i++){
				if(ismaskset(pptr->lmask,i))
					releaseall(1, i);
			}
		}
		/* Remove from any wait queue of Lock	*/
		if(pptr->plock > -1){
			next = q[pid].qnext;
	                if(isbadpid(next)){
			/* It is the first process in the Lock queue,	*
			 * pick the previous process's priority		*/
				next = q[pid].qprev;
				if(isbadpid(next)){
				/* No process in Lock queue, set the Lock prio to 0 */
					locktab[pptr->plock].lprio = 0;
				} else{
					chlprio(pptr->plock, proctab[next].pinh);
				}
	                } else {
	                        chpprio(next, 0);
			}
		}
		dequeue(pid);
		pptr->pstate = PRFREE;
		break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	restore(ps);
	return(OK);
}
