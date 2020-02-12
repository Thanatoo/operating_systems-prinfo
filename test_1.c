/*	Created by Junhan Duan (jduan3)
 *	Oct.25, 2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include "prinfo.h"

/*	This is a program that prints out its process ID and then calls 
 *	the sleep() system call to put itself to sleep for 100 seconds
 */
int main(int argc, char **argv) {
	printf ("pid: %d\n", getpid());
	sleep (100);
	return 0;
}