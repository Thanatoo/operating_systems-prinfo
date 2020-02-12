/*	Created by Junhan Duan (jduan3)
 *	Oct.25, 2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <signal.h>
#include <stddef.h>
#include <sys/wait.h>
#include "prinfo.h"

/*	This is a program that prints out its process ID, uses sigblock()
 *	to block SIGUSR2, sends SIGUSR2 to itself two times, and then
 *	calls prinfo() to look up its pending signal set
 */
int main(int argc, char **argv) {

	/* Create a prinfo struct in user space */
	struct prinfo *u_info = malloc(sizeof(struct prinfo));
	if (u_info == NULL) {
		return ENOMEM;
	}
	
	/* Set the signal block */
	sigset_t mask;
    sigemptyset(&mask);
        
	/* Block the signal */
    sigaddset(&mask, SIGUSR2);
	sigprocmask(SIG_SETMASK, &mask, NULL);
	
	/* Set the user pid */
	u_info->pid = getpid();

	/* Print process pid so that can it can be tested later */
	// printf ("pid: %d\n", getpid());
	
	/* Send SIGUSR2 to the process twice */
	kill(u_info->pid, SIGUSR2);
	kill(u_info->pid, SIGUSR2);

	/* Call our new system call */
	int error = syscall (181, u_info);

	if (error == 0) {
			/* print the process info */
			printf("state: %ld\n", u_info->state);
			printf("pid: %d\n", u_info->pid);
			printf("parent pid: %d\n", u_info->parent_pid);
			printf("youngest child pid: %d\n", u_info->youngest_child_pid);
			printf("younger sibling pid: %d\n", u_info->younger_sibling_pid);
			printf("older sibling pid: %d\n", u_info->older_sibling_pid);
			printf("start time: %ld\n", u_info->start_time);
			printf("user time: %lu\n", u_info->user_time);
			printf("system time: %lu\n", u_info->sys_time);
			printf("children user time: %lu\n", u_info->cutime);
			printf("children system time: %lu\n", u_info->cstime);
			printf("uid: %ld\n", u_info->uid);
			printf("comm: %s\n", u_info->comm);
			printf("signal: %lu\n", u_info->signal);
			printf("file descriptors: %ld\n", u_info->num_open_fds);

		}

		/* If the sysacall returns error, exit the program */
		else {
			printf("Error when calling syscall(181)!\nError code: %d", error);
			return error;
		}
	
	/* Suspend the process */
	// kill(u_info->pid, SIGTSTP);
	
	return 0;
}