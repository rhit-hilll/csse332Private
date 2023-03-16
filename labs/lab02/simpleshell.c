/* Copyright 2016 Rose-Hulman
   But based on idea from http://cnds.eecs.jacobs-university.de/courses/caoslab-2007/
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void handle_sigchld(int ignored) {
  //ignore what the child signal is, but wait for it to be done.
  int status;
  wait(&status);
}


int main() {

  signal(SIGCHLD, handle_sigchld);

  char command[82];
  char *parsed_command[2];
  //takes at most two input arguments
  // infinite loop but ^C quits
  while (1) {
    printf("SHELL%% ");
    fgets(command, 82, stdin);
    command[strlen(command) - 1] = '\0';//remove the \n
    int len_1;
    for(len_1 = 0;command[len_1] != '\0';len_1++){
      if(command[len_1] == ' ')
        break;
    }
    parsed_command[0] = command;
    if(len_1 == strlen(command)){
      printf("Command is '%s' with no arguments\n", parsed_command[0]); 
      parsed_command[1] = NULL;

      // leave this here, do not change it
      if(!strcmp(parsed_command[0], "quit") ||
         !strcmp(parsed_command[0], "exit")) {
        exit(EXIT_SUCCESS);
      }

      else {
        int rc;
        int s;

        if (parsed_command[0][0] == 'B' && parsed_command[0][1] == 'G') { //Background Process
          int newlen = strlen(parsed_command[0]);
          char *new_str = malloc(newlen - 1);
          for (int i = 0; i < newlen - 1; i++) {
            new_str[i] = parsed_command[0][i+2];
          }
          rc = fork();

          if(rc < 0) { //Cannot fork, terminate.
            perror("Failed to fork a process");
            exit(EXIT_FAILURE);
          }

          if (rc == 0) { //Child
            int rc2;
            rc2 = fork();

            if (rc2 < 0) { //Cannot fork, terminate.
              perror("Failed to fork a process");
              exit(EXIT_FAILURE);
            }

            if (rc2 == 0) { //Inner Child
              execlp(new_str, new_str, NULL);

              perror("Return from exec() not expected");
              exit(EXIT_SUCCESS);
            }

            else { //Inner Parent, patiently waiting.
              int s2;
              wait(&s2);

              printf("Background %s finished\n", new_str);
              exit(EXIT_SUCCESS);
            }
          }

          free(new_str);
        }

        else { //Foreground Process
          rc = fork();
          if(rc < 0) { //Cannot fork, terminate.
            perror("Failed to fork a process");
            exit(EXIT_FAILURE);
          }

          if (rc == 0) { //Child
            execlp(parsed_command[0], parsed_command[0], NULL);

            perror("Return from exec() not expected");
            exit(EXIT_SUCCESS);
          }
          else {
            wait(&s);
          }
        }
        
      }

    } else { //Argument Given
      command[len_1] = '\0';
      parsed_command[1] = command + len_1 + 1;
      printf("Command is '%s' with argument '%s'\n", parsed_command[0], parsed_command[1]); 

      // leave this here, do not change it
      if(!strcmp(parsed_command[0], "quit") ||
         !strcmp(parsed_command[0], "exit")) {
        exit(EXIT_FAILURE);
      }

      else {
        int rc;
        int s;

        if (parsed_command[0][0] == 'B' && parsed_command[0][1] == 'G') { //Background Process
          int newlen = strlen(parsed_command[0]);
          char *new_str = malloc(newlen - 1);
          for (int i = 0; i < newlen - 1; i++) {
            new_str[i] = parsed_command[0][i+2];
          }
          rc = fork();

          if(rc < 0) { //Cannot fork, terminate.
            perror("Failed to fork a process");
            exit(EXIT_FAILURE);
          }

          if (rc == 0) { //Child
            int rc2;
            rc2 = fork();

            if (rc2 < 0) { //Cannot fork, terminate.
              perror("Failed to fork a process");
              exit(EXIT_FAILURE);
            }

            if (rc2 == 0) { //Inner Child
              execlp(new_str, new_str, parsed_command[1], NULL);

              perror("Return from exec() not expected");
              exit(EXIT_SUCCESS);
            }

            else { //Inner Parent, patiently waiting.
              int s2;
              wait(&s2);

              printf("Background %s finished\n", new_str);
              exit(EXIT_SUCCESS);
            }
          }

          free(new_str);
        }

        else { //Foreground Process
          printf("Foreground\n");
          rc = fork();
          if(rc < 0) { //Cannot fork, terminate.
            perror("Failed to fork a process");
            exit(EXIT_FAILURE);
          }

          if (rc == 0) { //Child
            execlp(parsed_command[0], parsed_command[0], parsed_command[1], NULL);
          
            perror("Return from exec() not expected");
            exit(EXIT_FAILURE);
          }
          else {
            wait(&s);
          }
        }
      }
    }
  }
}
