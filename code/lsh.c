/*
 * Main source code file for lsh shell program
 *
 * You are free to add functions to this file.
 * If you want to add functions in a separate file(s)
 * you will need to modify the CMakeLists.txt to compile
 * your additional file(s).
 *
 * Add appropriate comments in your code to make it
 * easier for us while grading your assignment.
 *
 * Using assert statements in your code is a great way to catch errors early and make debugging easier.
 * Think of them as mini self-checks that ensure your program behaves as expected.
 * By setting up these guardrails, you're creating a more robust and maintainable solution.
 * So go ahead, sprinkle some asserts in your code; they're your friends in disguise!
 *
 * All the best!
 */
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <sys/wait.h>

// The <unistd.h> header is your gateway to the OS's process management facilities.
#include <unistd.h>

#include "parse.h"

#include <fcntl.h>

static void print_cmd(Command *cmd);
static void print_pgm(Pgm *p);
void stripwhite(char *);

int main(void)
{
  for (;;)
  {
    char *line;
    line = readline("> ");
    if (line == NULL) {
	    return 0;
    }

    // Remove leading and trailing whitespace from the line
    stripwhite(line);

    // If stripped line not blank
    if (*line)
    {
      add_history(line);

      Command cmd;
      if (parse(line, &cmd) == 1)
      {
        // Just prints cmd
        print_cmd(&cmd);
	int pid;
	int status;
	Pgm * p = cmd.pgm;

	
		
	while(p != NULL) {
		char ** strs = p->pgmlist;	

		if (strcmp(*strs, "exit") == 0) {
			return 0;
		} else if (strcmp(*strs, "cd") == 0 && ++strs != NULL) {

			if(chdir(*strs)) {
				printf("directory: %s invalid\n", *strs);
			}
			//printf("changing dir to: %s\n", *strs);
			goto next;
		}


		int i = 0;

		while (*(strs+i)) {	
			printf("%s ", *(strs+i));
			i++;
		}
		printf("\n");


		if( (pid = fork()) == 0) {

			// redirect stdout
			if (cmd.rstdout != NULL) {
				int fd = open(cmd.rstdout, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
				dup2(fd, 1);
				close(fd);
			}
			
			// redirect stderr
			if (cmd.rstderr !=NULL) {
				int fd = open(cmd.rstdin, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
				dup2(fd, 2);
				close(fd);
			}

			//redirect stdin
			if (cmd.rstdin !=NULL) {
				int fd = open(cmd.rstdin, O_RDONLY);
				dup2(fd, 0);
				close(fd);
			}

			//execute command
			if (*strs) {
				execvp(*strs, strs);
			}
		} else {
			wait(&status);
			printf("status: %d\n", status);
		}
next:
		p = p->next;
	}
		
      }
      else
      {
        printf("Parse ERROR\n");
      }
    }

    // Clear memory
    free(line);
  }

  return 0;
}

/*
 * Print a Command structure as returned by parse on stdout.
 *
 * Helper function, no need to change. Might be useful to study as inspiration.
 */
static void print_cmd(Command *cmd_list)
{
  printf("------------------------------\n");
  printf("Parse OK\n");
  printf("stdin:      %s\n", cmd_list->rstdin ? cmd_list->rstdin : "<none>");
  printf("stdout:     %s\n", cmd_list->rstdout ? cmd_list->rstdout : "<none>");
  printf("background: %s\n", cmd_list->background ? "true" : "false");
  printf("Pgms:\n");
  print_pgm(cmd_list->pgm);
  printf("------------------------------\n");
}

/* Print a (linked) list of Pgm:s.
 *
 * Helper function, no need to change. Might be useful to study as inpsiration.
 */
static void print_pgm(Pgm *p)
{
  if (p == NULL)
  {
    return;
  }
  else
  {
    char **pl = p->pgmlist;

    /* The list is in reversed order so print
     * it reversed to get right
     */
    print_pgm(p->next);
    printf("            * [ ");
    while (*pl)
    {
      printf("%s ", *pl++);
    }
    printf("]\n");
  }
}


/* Strip whitespace from the start and end of a string.
 *
 * Helper function, no need to change.
 */
void stripwhite(char *string)
{
  size_t i = 0;

  while (isspace(string[i]))
  {
    i++;
  }

  if (i)
  {
    memmove(string, string + i, strlen(string + i) + 1);
  }

  i = strlen(string) - 1;
  while (i > 0 && isspace(string[i]))
  {
    i--;
  }

  string[++i] = '\0';
}
