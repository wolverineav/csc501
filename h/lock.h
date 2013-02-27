#ifndef _LOCK_H_
#define _LOCK_H_

#define NLOCK		50	/* number of locks		*/

#define LFREE '\01'		/* this lock is free		*/
#define LUSED '\02'		/* this lock is used		*/

#define NOP 0
#define READ 1
#define WRITE 2

#define ONE 1

struct pcounter{
	int priority;
	int count;
};

struct lentry{			/* lock table entry		*/
	char lstate;		/* the state LFREE or LUSED	*/
	int lprio;
	int lcount;
	int lqhead;		/* q index of head of list	*/
	int lqtail;		/* q index of tail of list	*/
	int nreaders;		/* number of readers		*/
	int currop;
	int lmask;		/* lower 25 bit mask		*/
	int umask;		/* upper 25 bit mask		*/
	struct pcounter counter[NPROC];
};
extern struct lentry locktab[];
extern int nextlock;

/* inline functions for Lock	*/
#define isbadlock(l) (l<0 || l>=NLOCK)
#define setmask(bitmask, lockid) ((bitmask)|(ONE<<(lockid)))
#define ismaskset(bitmask, lockid) ((bitmask)&(ONE<<(lockid)))

	/* functions used to operate on the locks		*/
	int	linit(void);
	int	lcreate(void);
	int	ldelete(int ldes);
	int	linsert(int locktype, int prio, int tail);
	int	lock(int lockid, int locktype, int waitpriority);
	int	releaseall(int numlocks, long args, ...);
	int	firstwriter(int tail);
	int	firstreader(int proc);
	int	chlprio(int lockid, int newprio);
	int	chpprio(int pid, int newprio);
#endif
