

# Lab 1 - Group 32

## Purpose

The purpose of this lab was to program a functioning shell from pre-written skeleton code. The lab introduces the programmer to kernel level c-programming through common linux functions and system calls necessary to complete the lab. Through the lab issues such as signal propagation and changing file descriptors was encountered, which further expanded the knowledge of the programmer after being solved.




1. check line == null, send sigquit to all children (both foreground and background)
2. forking, exec
3. check if cmd.background then dont wait for process
4. create pipe, redirect stdin and stdout
5. if cmd.stdin | cmd.stdout | cmd.stderr -> redirect to specified fd
6. check if "exit" | "cd" -> return 0 | chdir()
7. parent -> SIG_IGN on SIGINT, child -> !cmd.background (SIG_DFL on SIGINT)
8. SIG_IGN on SIG_CHLD

met all specifications: yes
order: 1, 2, 6, 3, 5, 4, 7, 8
signal handling for sigint and sig_child (tried handlers for sending sig_ints etc), file descriptors for redirecting when piping, using many pipes at the same time without overwriting the old fd.

more specifications, perhaps hints on how to solve the issues. 
(you can find some info from the test file and strings when failing, but perhaps could be more in depth)
