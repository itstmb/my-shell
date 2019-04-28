#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFFER_SIZE 100

int main(void) {
    close(2);
    dup(1);
    char command[BUFFER_SIZE];
    char **args;
    bool background;
    int result;

    while (1)
    {
        fprintf(stdout, "my-shell> ");
        memset(command, 0, BUFFER_SIZE);
        fgets(command, BUFFER_SIZE, stdin);
        if(strncmp(command, "exit", 4) == 0)
        {
            break;
        }

        /* parse input */
        args = parse_input(command);

        /* check for & (background vs foreground) */
        background = check_background(args);

        /* execute command */
        result = exec_command(args, background);

        /* free memory */
        free(args);
    }
    return 0;
}

char **parse_input(char *input)
{
  char **args = malloc(BUFFER_SIZE * sizeof(char*));
  char *token;
  int i = 0;

  token = strtok(input, " \n");
  while (token != NULL)
  {
    args[i++] = token;
    token = strtok(NULL, " \n");
  }

  args[i] = NULL;
  return args;
}

bool check_background(char **args) {
  int i = 0;
  while (args[i] != NULL) {
    i++;
  }
  if (strcmp(args[i-1],"&") == 0) {
    args[i-1] = NULL;
    return (true);
  }
  return (false);
}

int exec_command(char **args, bool background)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid < 0 ) { // fork failure
    perror("Failure during fork()");
  }
  else if (pid == 0) { // child process
    if (execvp(args[0],args) == -1) {
      perror("Error during exec ");
    }
  } else { // parent process (pid > 0)
    if (!background) { // only wait when running in foreground
      waitpid(pid, &status, 0); // wait for child process
    }
  }

  return 1;
}
