# Lab 1 - Group 32

## Purpose

The purpose of this lab was to program a functioning shell from pre-written skeleton code. The lab introduces the programmer to kernel level c-programming through common linux functions and system calls necessary to complete the lab. When solving the lab, issues regarding signal propagation and file descriptor manipulation were encountered. In turn, solving these issues led to a broader understanding of specifically linux process internals.

This report is about the implementation of a shell named lsh using c. For this shell there was a list of requirements which needed to be implemented to pass the lab, among these were CTRL-C/D handling, I/O handling, piping support and that the shell has no zombies.

The current version of the code passes all tests in the test script as well as functioning correctly according to our understanding of the instructions. The tasks were solved in the order of 1, 2, 6, 3, 5, 4, 7, and 8.

## Ctrl-D Handling
CTRL-D (EOF) is triggered if the line is empty, sending SIGQUIT to all children ignoring whether the processes are foreground or background. In this case SIGQUIT forces all processes to close. This ensures that no processes are left behind when closing the shell. This implementation was completed very quickly after starting work on the lab and gave us no real challenge in terms of difficulty but served as a good introduction. 

## Basic Commands
The basic commands are read using a readline function to retrieve what command to execute. A function called stripwhite is then used to remove any additional spaces from the read line. If it is not a built-in command the shell forks of a child process which calls execvp() to change the execution state and look for corresponding command binary. The parent process then either waits for the execution if a foreground process or leaves the execution if it is a background process.

## Built-ins
Built-in commands are read in the same way as the basic commands but are caught in a if/else  statement. If exit is found the code returns 0 and thereby exits. In case cd is read the chdir() function is called to change the working directory as long as the directory exists. If not, the code prints a message notifying in the shell that the directory was invalid. 

## Background Execution
Background execution is handled by checking the background flag in the Command structure. If the flag is set, the shell does not wait for the child process to finish, allowing it to run in the background. This is done by not calling waitpid for background processes. 

## Piping & I/O Redirection
File descriptors are integral to the implementation of piping and I/O redirection. 

Piping: When setting up a pipe between commands, the shell duplicates the write end of the previous pipe to the standard output. Similarly, the read end of the current pipe is duplicated to the standard input. After duplicating, the original pipe file descriptors are closed to avoid file descriptor leaks. Important is that when piping inputs and outputs between commands, two pipes need to be open at the same time. The old pipe for the read end of the new process and a new pipe for the write end of the next pipe. This was implemented by keeping track of two fd[2] values and letting the parent process switch the old filedescriptors with the new ones before starting a new process.

I/O Redirection: If the command struct has a specified stdinput, stdoutput, or stderror, the function open is called to open the file corresponding to the string of stdinput/stdoutput/stderror. The file descriptor returned by the open function is duplicated to the standard input, output, or error using dup2. For example, dup2(fd, 1) redirects the standard output to the file with filedescriptor fd. After duplicating, the original file descriptor is closed. Important to note is if the file to redirect to does not exist, one could question whether or not earlier commands in the pipe should be executed or not. In our implementation the commands are handled without checking if the file exists. In fact there is no error handling at all on the open system call which means the program crashes if the file does not exist. In case of a software release this would not be a good way of coding.

## Ctrl-C Handling
For handling the CTRL-C to shut down ongoing processes while leaving background processes unharmed we use the signal() function. signal() tells the shell to handle the specified signal, in this case SIGINT, in a certain way. If the current process is a background process signal is used to ignore the signal SIGINT using SIG_IGN. To reset to the usual function SIG_DFL (default) is used. The default behavior is activated if the process is a foreground process and should be terminated by SIGINT.  Compared to the CTRL-D the shell isnt closed down and only foreground processes are stopped.


## No Zombies
Zombie processes are dealt with through two measures. The first one is the signal(SIGCHLD, SIG_IGN) which makes the system automatically reap terminated processes. This makes it so that finished processes arent ignored which is what constitutes a zombie process. For foreground processes the results from their child processes are awaited using waitpid for each process. This ensures that the process doesnt exit while any child processes are active. If that happened zombie processes would also be created. There was also a test which before being removed assured no zombies were left after closing the shell with ctrl+D. This was solved by setting the signal SIGQUIT to ignore in the parent process, and propagating SIGQUIT to all children by sending SIGQUIT to the parent.


## Challenges encountered
Some members of the group found the implementation of pipes and I/O redirection somewhat challenging. We spent considerable time understanding how file descriptors work and how to copy them in order to achieve piping and I/O redirection. The initial code we wrote for implementing pipes did not work for more than two commands due to incorrect closure of the pipes.

Due to vague instructions, enabling the CTRL-C functionality took much longer than expected. A  long standing issue was that CTRL-C worked on a lone foreground process but didnt if there were any background processes active concurrently. The bug found in the testing code also contributed to prolonging the work on the task and consequently CTRL-C was the last task solved.

## Feedback:
One point of feedback is that instructions sometimes felt unclear and tests didnt give enough feedback as to what went wrong. A lot of time was spent with one or more tests failing in some way that wasnt properly explained in terms of how it should work. The tests were certainly very useful but could've been even more so if they gave more information. To solve one issue the relevant code was rewritten almost entirely but still the test said the same thing causing confusion. Perhaps these problems were exacerbated by a problem on our part which was that we couldnt get to any lab consultation sessions together until after we were already finished. 

Overall understanding the uses of different signals took time. Both their purpose and how they affected the shell took time reading up upon and testing. Reading man-pages for each function is also time consuming effort which easily couldve been removed by a short explanation in a readme file or similar. A list of recommended signals and short introductions to the main functions and relevant uses. 

The difficulty level of the lab was within reason but still a serious challenge. A criticism heard from people whove taken the course earlier is that the first lab is by far the most difficult. Perhaps a soft start wouldve been a better idea and then later raise the challenge level of each successive lab instead of throwing everyone in the deep end.  

## Conclusion
Overall this lab taught us a lot about the subject while being challenging. The information about the lab and the testing software was sometimes a bit lacking but nothing that couldn't be solved. Each test is in the current version passing which strongly indicates that weve solved each task. Nothing we see while testing the program manually indicates problems either.  
