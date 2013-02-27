/* lockfunction.c - includes all the auxillary functions used
 * by the lock and release operation				*/

#include <conf.h>
#include <q.h>
#include <lock.h>

/* returns the first Writer present from the tail	*
 * returns HEAD if no writer present			*/
int firstwriter(int tail)
{
	int writer;
	writer = q[tail].qprev;
	while(q[writer].locktype != WRITE && writer < NPROC){
		writer = q[writer].qprev;
	}
	return(writer);
}

/* returns the first eligible Reader present after the Writer 	*
 * returns Writer if all Readers are of same priority		*/
int firstreader(int proc)
{
	int reader;
	reader = q[proc].qnext;
	while((q[proc].qkey == q[reader].qkey) && (reader < NPROC)){
		reader = q[reader].qnext;
	}
	if (reader < NPROC)
		return(reader);
	else
		return(proc);
}
