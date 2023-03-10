#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// STEP 1:
// After our in-class discussion, write code here that will create two
// processes, a parent process and a child process. Each process should print
// out the following:
//    Hello from <parent|child> process with pid = <pid>.
//
// Make sure to add error checking code to account for failures to create the
// processes.

// STEP 2:
// In the child process, add the following line before the child prints its
// hello message:
//    sleep(1);
//
// Now compile and run the program, what do you see?

// STEP 3:
// Fix the problem in step 2 by adding the appropriate function call in the
// parent process.

int
main(int argc, char **argv)
{
  // TODO: Add your code here to follow the steps above.
  int rc;
  int s;

  rc = fork();
  if(rc < 0) {
    perror("Failed to fork a process");
    exit(EXIT_FAILURE);
  }

  if (rc == 0) {
    sleep(1);
    printf("Hello from the child process with pid = %d\n", getpid());
  }
  else if (rc > 0) {
    printf("Hello from the parent process with pid = %d\n", getpid());
    //block until child is done
    wait(&s);
  }

  exit(EXIT_SUCCESS);
}

