/* linsert.c  -  linsert */

#include <conf.h>
#include <kernel.h>
#include <q.h>
#include <lock.h>
#include <proc.h>

/*------------------------------------------------------------------------
 * linsert.c  --  insert the current process into the waiting queue for
 * the lock specified by tail
 *------------------------------------------------------------------------
 */
int linsert(int locktype, int prio, int tail)
{
        int     next;                   /* runs through list		*/
        int     prev;

        prev = q[tail].qprev;
	while(prev < NPROC && q[prev].qkey > prio){
		/* increment priority of all processes in wait queue	*
		 * whose prio is lower than current process's prio	*/
		if(proctab[currpid].pinh > proctab[prev].pinh){
			proctab[prev].pinh = proctab[currpid].pinh;
		}
		prev = q[prev].qprev;
	}
	/* If inserting WRITE, put it at the end of the 
 	 * equal wait priority processes in the queue		*/
	if(WRITE == locktype) {
		while(q[prev].qkey == prio){
	                if(proctab[currpid].pinh > proctab[prev].pinh){
	                        proctab[prev].pinh = proctab[currpid].pinh;
	                }
			prev = q[prev].qprev;
		}
	} else {
	/* If inserting READ, its sits at the end of all reads
 	 * of the same priority					*/
		while(q[prev].qkey == prio){
			if(q[prev].locktype != READ) {
			/* insert after first WRITE encountered	*/
				break;
			}
	                if(proctab[currpid].pinh > proctab[prev].pinh){
	                        proctab[prev].pinh = proctab[currpid].pinh;
	                }
			prev = q[prev].qprev;
		}
	}
	/* insert it next to the current element in queue	*/
        q[currpid].qprev = prev;
        q[currpid].qnext = next = q[prev].qnext;
        q[currpid].qkey  = prio;	/* sorted by lock wait priority	*/
	q[currpid].locktype = locktype;	/* set locktype in the queue	*/
	q[prev].qnext = currpid;
	q[next].qprev = currpid;

	return(OK);
}
