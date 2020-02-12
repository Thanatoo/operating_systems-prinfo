/*	Created by Junhan Duan (jduan3)
 *	Oct.25, 2019
 */

struct prinfo {
	/* current state of process */
	long state;

	/* process id (input) */
	pid_t pid;

	/* process id of parent*/
	pid_t parent_pid;	

	/* process id of youngest child */
	pid_t youngest_child_pid;

	/* process id of the oldest among younger siblings */
	pid_t younger_sibling_pid;

	/* process id of the youngest among older siblings */
	pid_t older_sibling_pid;

	/* process start time */
	unsigned long start_time;

	/* CPU time spent in user mode */
	unsigned long user_time;

	/* CPU time spent in system mode */
	unsigned long sys_time;	

	/* total user time of children */
	unsigned long cutime;	

	/* total system time of children */
	unsigned long cstime;	

	/* user id of process owner */
	long uid;				

	/* name of program executed */
	char comm[16];			

	/* The set of pending signals */
	unsigned long signal;		

	/* Number of open file descriptors */
	unsigned long num_open_fds;	
};
