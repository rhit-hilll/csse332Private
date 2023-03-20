#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h> 

#define simple_assert(message, test)                                           \
  do {                                                                         \
    if(!(test))                                                                \
      return message;                                                          \
  } while(0)
#define TEST_PASSED NULL
#define DATA_SIZE 100
#define INITIAL_VALUE 77
#define MAX_TESTS 10

/* test_funcs is an array of function pointers that store all of the test we
 * would like to run. */
char *(*test_funcs[MAX_TESTS])();

/* total number of tests registered */
static int num_tests = 0;

/* shared data that all the tests will use. */
static int data[DATA_SIZE][DATA_SIZE];

/**
 * Add a test to our list of test functions that we would like to run.
 *
 * @param test_func A pointer to the function that we would like to run.
 */
void add_test(char *(*test_func)()) {
  if(num_tests == MAX_TESTS) {
    printf("exceeded max possible tests");
    exit(1);
  }
  test_funcs[num_tests] = test_func;
  num_tests++;
}

/**
 * This setup function should run before each test
 */
void setup(void) {
  printf("starting setup\n");
  fflush(stdout);
  for(int i = 0; i < DATA_SIZE; i++) {
    for(int j = 0; j < DATA_SIZE; j++) {
      data[i][j] = INITIAL_VALUE;
    }
  }

  /* imagine this function does a lot of other complicated setup that takes a
   * long time. */
  usleep(3000000);
}

void setsighandler(int signum, void (*handler)(int))
{
	struct sigaction act;

	act.sa_handler = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	sigaction(signum, &act, NULL);
}

void alarm_handler(int handler) {
  printf("Test Timed Out\n");
  exit(1);
}

/**
 * Run all the test in the test suite.
 */
void run_all_tests() { /* TODO: Add your code here. */
  int rc;
  int s;
  
  int childPID[num_tests];

  setup();
  int fd[2];
  pipe(fd);

  for (int i = 0; i < num_tests; i++) {
    
    rc = fork();

    if (rc < 0) { //Cannot fork, terminate.
      perror("Failed to fork a process");
      exit(EXIT_FAILURE);
    }

    if (rc == 0) { //Child
      

      rc = fork();
      childPID[i] = rc; //store child PID for later.

      if (rc < 0) {
        perror("Failed to fork a process");
        exit(EXIT_FAILURE);
      }

      if (rc == 0) {
        char *tresult = test_funcs[i]();
        if(tresult == TEST_PASSED) {
          close(fd[0]);
	        close(fd[1]);
	        exit(0);
        } else {
          close(fd[0]);
	        write(fd[1], tresult, strlen(tresult));
	        close(fd[1]);
	        exit(1);
        }

      }
      else {
        setsighandler(SIGALRM, alarm_handler);
        alarm(3);

        waitpid(childPID[i], &s, 0);

        if (WIFEXITED(s)) {
          if (WEXITSTATUS(s) == 0) {
            printf("Test Passed\n");
          }
          else {
            close(fd[1]);

            char buff[512];
            if (read(fd[0], buff, 512) > 0) {
              printf("Test Failed: %s\n", buff);
            }
            close(fd[0]);
          }
        }
        else {
          printf("Test Crashed\n");
        }
        exit(EXIT_SUCCESS);
      }
    }
  }
}

char *test1() {
  printf("starting test 1\n");
  for(int i = 0; i < DATA_SIZE; i++) {
    for(int j = 0; j < DATA_SIZE; j++) {
      simple_assert("test 1 data not initialized properly",
                    data[i][j] == INITIAL_VALUE);
    }
  }

  for(int i = 0; i < DATA_SIZE; i++) {
    for(int j = 0; j < DATA_SIZE; j++) {
      data[i][j] = 1;
    }
  }

  for(int i = 0; i < DATA_SIZE; i++) {
    for(int j = 0; j < DATA_SIZE; j++) {
      simple_assert("test 1 data not set properly", data[i][j] == 1);
    }
  }
  printf("ending test 1\n");
  return TEST_PASSED;
}

char *test2() {
  printf("starting test 2\n");
  for(int i = 0; i < DATA_SIZE; i++) {
    for(int j = 0; j < DATA_SIZE; j++) {
      simple_assert("test 2 data not initialized properly",
                    data[i][j] == INITIAL_VALUE);
    }
  }

  for(int i = 0; i < DATA_SIZE; i++) {
    for(int j = 0; j < DATA_SIZE; j++) {
      data[i][j] = 2;
    }
  }

  for(int i = 0; i < DATA_SIZE; i++) {
    for(int j = 0; j < DATA_SIZE; j++) {
      simple_assert("test 2 data not set properly", data[i][j] == 2);
    }
  }

  printf("ending test 2\n");
  return TEST_PASSED;
}

char *test3() {
  printf("starting test 3\n");

  simple_assert("test 3 always fails", 1 == 2);

  printf("ending test 3\n");
  return TEST_PASSED;
}

char *test4() {
  printf("starting test 4\n");

  int *val = NULL;
  printf("data at val is %d", *val);

  printf("ending test 4\n");
  return TEST_PASSED;
}

char *test5() {
  printf("starting test 5\n");

  while(1)
    ;

  printf("ending test 5\n");
  return TEST_PASSED;
}

int main(int argc, char **argv) {
  add_test(test1);
  add_test(test2);
  add_test(test3);
  add_test(test4);
  add_test(test5);
  run_all_tests();
}
