/* getschedclass.c - get scheduler class */

#include <kernel.h>
#include <sched.h>

/*------------------------------------------------------------------------
 * getschedclass  --  get the type of scheduler currently running
 *------------------------------------------------------------------------
 */
int getschedclass(){
	return(scheduler_class);
}
