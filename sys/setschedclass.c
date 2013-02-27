/* setschedclass.c - set scheduler class */

#include <kernel.h>
#include <sched.h>

/*------------------------------------------------------------------------
 * setschedclass  --  set the scheduler_class for the kernel
 *------------------------------------------------------------------------
 */
void  setschedclass(int sched_class){
	scheduler_class = sched_class;
}
