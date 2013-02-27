/* queue.c - dequeue, enqueue */

#include <conf.h>
#include <kernel.h>
#include <q.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * enqueue  --	insert an item at the tail of a list
 *------------------------------------------------------------------------
 */
int enqueue(int item, int tail)
/*	int	item;			- item to enqueue on a list	*/
/*	int	tail;			- index in q of list tail	*/
{
	struct	qent	*tptr;		/* points to tail entry		*/
	struct	qent	*mptr;		/* points to item entry		*/

	tptr = &q[tail];
	mptr = &q[item];
	mptr->qnext = tail;
	mptr->qprev = tptr->qprev;
	q[tptr->qprev].qnext = item;
	tptr->qprev = item;
	return(item);
}


/*------------------------------------------------------------------------
 *  dequeue  --  remove an item from the head of a list and return it
 *------------------------------------------------------------------------
 */
int dequeue(int item)
{
	struct	qent	*mptr;		/* pointer to q entry for item	*/

	mptr = &q[item];
	q[mptr->qprev].qnext = mptr->qnext;
	q[mptr->qnext].qprev = mptr->qprev;
	mptr->locktype = NOP;
	return(item);
}

int printq(int head)
{
	int curr;
	curr = q[head].qnext;
	kprintf("qnext:%d\n",curr);
	while(curr < NPROC){
		kprintf("R/W:%d, pname:%s->", q[curr].locktype, proctab[curr].pname);
		curr = q[curr].qnext;
		kprintf("nextis:%d\t", curr);
	}
	kprintf("\n");
}
