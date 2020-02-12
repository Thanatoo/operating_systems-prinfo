// SPDX-License-Identifier: GPL-2.0-only
/*	Created by Junhan Duan (jduan3)
 *	Oct.25, 2019
 */
#include <linux/sched.h>
#include <linux/prinfo.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include <linux/signal.h>
#include <linux/rcupdate.h>
#include <linux/sched/task.h>
#include <linux/cred.h>
#include <linux/fdtable.h>
#include <linux/fs.h>
#include <linux/uidgid.h>
#include <linux/signal_types.h>


/*	Calculate the length of an array
 *	return -1 if the input is null
 */
unsigned long array_length(struct file** head) {
	if (*head != NULL) {
		int i;
		for (i = 0; head[i] != NULL; i ++);
		return i;
	}
	return -1;
}

void commcpy(char* to, char* from, int length) {
	int i;
	for (i = 0; i < length-1; i ++) 
	{
		to[i] = from[i];
	}
	to[length-1] = '\0';
}

/*	Added a system call (No.181 in syscall table) that stores a process's information
 *	into data structure prinfo. 
 */
SYSCALL_DEFINE1(prinfo, struct prinfo *, p_info) {

	/* Variables */
	struct task_struct *p;												/* process task struct */
	struct task_struct *pc;												/* child process used to iterate through the youngest child */
	struct task_struct *sb;												/* sibling proces used to iterate through the siblings */
	unsigned long youngest_start_time = -1;								/* youngest_start_time timeval */
	unsigned long younger_start_time = -1;								/* younger_start_time: oldest among younger siblings */
	unsigned long older_start_time = -1;								/* older_start_time: youngest among older siblings */


	/* Does not enter the program if the u_info is a null pointer */
	if (p_info != NULL) {

		/*	When using the system call, user need to assign printfo a pid number
		 *	which will be the process that user wants to get information from. Thus
		 *	find the process that user wants by searching through the tasks and 
		 *	then store the inforamtion to the prinfo structure
		 */
		rcu_read_lock();
		/* Iterate through the tasks, find the task's pid equals to info->pid */
		for_each_process(p) {

			if (p_info->pid == p->pid) {

				/* state */
				p_info->state = p->state;

				/* pid already given*/

				/* parent_id */
				p_info->parent_pid = p->real_parent->pid;

				/* cutime & cstime */		
				p_info->cutime = 0;
				p_info->cstime = 0;

				/*	List for each child, find the youngest one 
			 	 *	the function list_for_each_entry takes a loop cursor, a head, and
			 	 *	an instance here the cursor is pc and the instance is member, the
			 	 *	head is p->children's address
			 	 */			 	
				list_for_each_entry(pc, &(p->children), sibling) {

					/* If start time uninitialized, initialze it */
					if (youngest_start_time == -1) {
						youngest_start_time = pc->start_time;
						p_info->youngest_child_pid = pc->pid;
					}

					/* Iterate through the tasks and update the youngest process */
					if (pc->start_time > youngest_start_time) {
						youngest_start_time = pc->start_time;
						p_info->youngest_child_pid = pc->pid;
					}

					/* Update the cutime & cstime */
					p_info->cutime += pc->utime;
					p_info->cstime += pc->stime;
				}

			 	/*	Search through the processes tree, if find the process starting time
			 	 *	is greater, then it should be the older process. Thus search through
			 	 *	the youngest sibling and vice versa
			 	 */
			 	list_for_each_entry(sb, &(p->sibling), sibling) {

			 		if (sb->start_time > p->start_time) {
			 			/* If start time uninitialized, initialze it */
			 			if (older_start_time == -1) {
			 				older_start_time = sb->start_time;
			 				p_info->older_sibling_pid = sb->pid;
			 			}
			 			/* Iterate through the tasks and update */
			 			if (sb->start_time < p->start_time) {
			 				older_start_time = sb->start_time;
			 				p_info->older_sibling_pid = sb->pid;
			 			}
			 		}

			 		/* If start time uninitialized, initialze it */
			 		else if (sb->start_time < p->start_time) {
			 			if (younger_start_time == -1) {
			 				younger_start_time = sb->start_time;
			 				p_info->younger_sibling_pid = sb->pid;
			 			}
			 			/* Iterate through the tasks and update */
			 			if (sb->start_time > p->start_time) {
			 				younger_start_time = sb->start_time;
			 				p_info->younger_sibling_pid = sb->pid;
			 			}
			 		}
			 	}

				/* start_time */
				p_info->start_time = p->start_time;

				/* user_time */
				p_info->user_time = p->utime;

				/* sys_time */
				p_info->sys_time = p->stime;

				/* uid */
				p_info->uid = p->real_cred->uid.val;

				/* comm[16] */
				commcpy(p_info->comm, p->comm, 16);

				/* signal */
				// p_info->signal = p->pending.signal.sig[0];			/* idk why but seems that this one does not work */
				p_info->signal = p->signal->shared_pending.signal.sig[0];
				/* num_open_fds */
				p_info->num_open_fds = array_length(p->files->fdt->fd);

				/* Break the loop after operation is done */
				break;
			}

			if (p == &init_task) {
				return ESRCH;
			}

		}
		rcu_read_unlock();

		return 0;
	}

	/* Return the EINVAL (invalid argument) if u_info is null */
	else {
		return EINVAL;
	}

}