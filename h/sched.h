#ifndef _sched_h_
#define _sched_h_
	void	setschedclass(int sched_class);
	int	getschedclass();

	int	exp_dist_getlast(int proc);
	int	exp_dist_nextproc(int rprio);

	int	linux_next_proc();
	void	reallocate();
#endif

extern int scheduler_class;
