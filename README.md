# csc256-mp1
Oct.1 2019 by Junhan Duan (jduan3)

### How to attach the patch to ther kernel ###
git clone a clean version of the linux kernel.
then run "patch -p1 < mypatch" since the turn-in file is named "mypatch".
----NOTE!!!----
The run "patch -p1 < mypatch" will ONLY CHANGE THE prinfo.c
You might need to add file (prinfo.h, test_1.c, test_2.c, test_3.c) by yourself, thanks!

### Introduction ###
This project is a system call added to the kernel to print the process info given pid.

### Files modified or added to the kernel/floppy ###
- Modified file "prinfo.c" in the directory "./linux-5.3.2/kernel".
- Added file "prinfo.h" to the directory "./linux-5.3.2/include/linux".
- Added file "prinfo.h" to the directory "./floppy" (!!!)
- Modified file "test.c" in the directory "./floppy"
- Added file "test_1.c" to the directory "./floppy"
- Added file "test_2.c" to the directory "./floppy"
- Added file "test_3.c" to the directory "./floppy"

### Expriment description ###
The test files are under the directory "./floppy". 
The "test_(num).c" files are compiled to "(num)" while testing inside the kernel. You might need to recompile them when testing.
Note: Please include the flg "-static" when compiling the ".c" files under "./floppy" or they would not be recognized by the kernel when mounted.
The "test.c" program takes a pid as an input and will print the info of the process whose pid is equal to the input pid.
The "test_1.c" program prints out its process ID and then calls the sleep() system call to put itself to sleep for 100 seconds. The user can test this program by using "test.c" with its pid printed.
The "test_2.c" program calls the system call (181) with its own pid as input and prints out its own state.
The "test_3.c" program prints out its process ID, uses sigblock() to block SIGUSR2, sends SIGUSR2 to itself two times, and then calls prinfo() to look up its pending signal set.

### Expriment result ###
All the results are generated within one kernel run.
Analysis in the next section

--------------------------------------test_1--------------------------------------
root@OpenWrt:/mnt# ./1 &
root@OpenWrt:/mnt# pid: 703
root@OpenWrt:/mnt# jobs
[1]+  Running                    ./1
root@OpenWrt:/mnt# ./test 703
state: 1
pid: 703
parent pid: 321
youngest child pid: 0
younger sibling pid: 0
older sibling pid: 704
start time: 38621503452
user time: 0
system time: 880640
children user time: 0
children system time: 0
uid: 0
comm: 1
signal: 0
file descriptors: 3

--------------------------------------test_2--------------------------------------
root@OpenWrt:/mnt# ./2
state: 0
pid: 705
parent pid: 321
youngest child pid: 0
younger sibling pid: 0
older sibling pid: 0
start time: 116777858641
user time: 0
system time: 5062550
children user time: 0
children system time: 0
uid: 0
comm: 2
signal: 0
file descriptors: 3

root@OpenWrt:/mnt# ./2
state: 0
pid: 707
parent pid: 321
youngest child pid: 0
younger sibling pid: 0
older sibling pid: 0
start time: 161149320057
user time: 0
system time: 0
children user time: 0
children system time: 0
uid: 0
comm: 2
signal: 0
file descriptors: 3

root@OpenWrt:/mnt# ./2
state: 0
pid: 708
parent pid: 321
youngest child pid: 0
younger sibling pid: 0
older sibling pid: 0
start time: 161692152672
user time: 0
system time: 0
children user time: 0
children system time: 0
uid: 0
comm: 2
signal: 0
file descriptors: 3

--------------------------------------test_3--------------------------------------
root@OpenWrt:/mnt# ./3
state: 0
pid: 709
parent pid: 321
youngest child pid: 0
younger sibling pid: 0
older sibling pid: 716
start time: 213144934975
user time: 0
system time: 4467008
children user time: 0
children system time: 0
uid: 0
comm: 3
signal: 2048
file descriptors: 3

### Analysis ###
test_2:
	state: The current state of the process. 0 means that the program is running. Since in the test all the process info are printed while the process is at running state so they are all 0. (I tried to suspend a program and tested it the state is 260)
	pid: The current process pid, since in the test the pid are entered as an input so it cant go wrong. In the test_# program I used geepid() to gain the current process pid.
	parent pid: I used the real_parent pid to ensure the stability. parent all of their parent pid are 321 since they are running under the shell (/bin/ash).
	The start time number stands for nanoseconds.
	youngest child pid: 0; younger sibling pid: 0; older sibling pid: 0; means that it has no youngest child, no younger sibling and no older sibling.
	The user time number and the system time unmber stands for nanoseconds.
	Since it has no child, so child total user/system time are both 0.
	uid stands for "who" create the process, in this case uid=0 means that the process is created by root.
	comm stands for its name (the first argument within the process)
	signal stands for its pending signal number, in this case it is 0.
	file descriptors stands for its number of open file descriptors, the test_2 will open 3 file descriptors.

test_3: The value is signal is 2048.
	The reason why that the signal cannoe tbe queued in linux is that linux uses an array o unsigned long to bitmap the signal and when a signal is pending the corresponding bit is set to 1.
	In the array, each element is a bitmap of current pending signal so the same type of signal can only be calculated once. 
	Moreover, under this circumstance, a simple unsigned long cannot reprensent more than one instance.