# LinuxSubprocesses
A practice project for linux, showing communication between processes using signals and different methods of sending data between them.

The program creates 3 subprocesses p1-3: 
p1 reads contents of a file from a given path and sends them to p2 via pipe.
p2 encodes the recieved date to hexadecimal and sends it to p3 via fifo.
p3 reads the data and prints it.

these actions can be affected by sending signals to the subprocesses, causing them to be sent to the parent process which then sends an "effective signal" to the subprocess expected to react to the given command:

//s1 - 3.sigquit, (effective 2.sigint): sending sig 3 to any subprocess causes the parent to kill all 3 subprocesses with sig 2, then close.

//s2 - 23.SIGURG (effective 10.SIGUSR1): sending sig 23 to any subprocess causes the parent to send sig 10 to all subprocesses, pausing communication between processes.

//s3 - 18.SIGCONT (effective 12.SIGUSR2): sending sig 18 to any subprocess causes the parent to send sig 12 to all subprocesses, resuming communication between processes.

//s4 - 4.sigill, (effective 17.sigchld): sending sig 4 to any subprocess causes the parent to send sig 17 to only p2, causing it to stop converting the data and send it to p3 unconverted.

The program in tl.c allows to verify if the data conversion was done correctly.
