#include <kernel.h>
#include <q.h>
#include <lock.h>

struct lentry locktab[NLOCK];	/* lock table                   */
int nextlock;	                /* next lock to use in lcreate  */

int linit(void)
{
/* initialize the state and counters */

	int i, j;
	struct lentry* lptr;

	nextlock = NLOCK-1;

	for(i=0; i<NLOCK; i++) {
		(lptr = &locktab[i])->lstate = LFREE;
		lptr->nreaders = 0;
		lptr->currop = NOP;
		lptr->lqtail = 1 + (lptr->lqhead = newqueue());
		lptr->lprio = 0;
		lptr->lmask = 0;
		lptr->umask = 0;
	}
	kprintf("\nlinit done.\n");
	return OK;
}
