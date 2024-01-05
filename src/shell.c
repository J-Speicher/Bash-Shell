#include "builtins.h"
#include "hash.h"
#include "process.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// No command line can be more than 100 characters
#define MAXLENGTH 100

void run_command (char *);
void run_command_pipes (char *);
void path_helper (char *, char *);
void args_helper (char **, char *);
void envp_helper (char **);

void
shell (FILE *input)
{

  hash_init (100);
  hash_insert ("?", "0");
  char buffer[MAXLENGTH];
  // bool isPipe = false;
  char *cmd1 = NULL;
  char *cmd2 = NULL;
  int pipefd[2];
  while (1)
    {
      // Print the cursor and get the next command entered
      printf ("$ ");
      memset (buffer, 0, sizeof (buffer));
      if (fgets (buffer, MAXLENGTH, input) == NULL)
        break;

      if (input != stdin)
        printf ("%s", buffer);

      // start of student work

      // only checks if buffer contains a command
      // may have to change to check for proper formatting,
      // ie the command is the first word given
      // and only has the proper arguments following it.
      // might be improved with switch statement?

      // Check for pipe and take each argument
      /*if (strstr (buffer, "|"))
        {
          isPipe = true;
          cmd1 = strtok (buffer, "|");
          cmd2 = strtok (NULL, "|");

          if (pipe (pipefd) == -1)
            {
              perror ("pipe");
              exit (1);
            }

        } // end pipe if*/

      // builtins
      if (strstr (buffer, "which"))
        {
          which (buffer);
        }
      else if (strstr (buffer, "echo"))
        {
          echo (buffer);
        }
      else if (strstr (buffer, "quit"))
        {
          printf ("\n");
          exit (0);
        }
      else if (strstr (buffer, "cd"))
        {
          cd (buffer);
        }
      else if (strstr (buffer, "pwd"))
        {
          pwd ();
        }
      else if (strstr (buffer, "export"))
        {
          // skip export
          export(buffer + 7);
        }

      // utils
      else
        {

          // no pipe
          if (!strstr (buffer, "|"))
            {
              run_command (buffer);
            }

          // pipe
          else
            {
              // fflush(stdout);
              cmd1 = strtok (buffer, "|");
              cmd2 = strtok (NULL, "|");
              assert (pipe (pipefd) >= 0);

              pid_t child_pid1 = fork ();
              assert (child_pid1 >= 0);
              if (child_pid1 == 0) // run cmd1
                {
                  close (pipefd[0]);               // close read end
                  dup2 (pipefd[1], STDOUT_FILENO); // redirect stdout
                  run_command_pipes (cmd1);
                }

              pid_t child_pid2 = fork ();
              assert (child_pid2 >= 0);
              if (child_pid2 == 0) // run cmd2
                {
                  close (pipefd[1]);              // close write end
                  dup2 (pipefd[0], STDIN_FILENO); // redirect stdin
                  run_command_pipes (cmd2);
                }
              wait (NULL);
              fflush (stdout);
            }
        }
    }
  printf ("\n");
  hash_destroy ();
}

void
run_command (char *buffer)
{
  char *command;
  if (strstr (buffer, "/cat"))
    command = "cat";
  else if (strstr (buffer, "/cut"))
    command = "cut";
  else if (strstr (buffer, "/env"))
    command = "env";
  else if (strstr (buffer, "/head"))
    command = "head";
  else if (strstr (buffer, "/ls"))
    command = "ls";

  char path[MAXLENGTH];
  memset (path, 0, MAXLENGTH);
  path_helper (path, command);

  char *argv[MAXLENGTH];
  memset (argv, 0, sizeof (argv));
  args_helper (argv, buffer);

  char *envp[MAXLENGTH];
  memset (envp, 0, sizeof (envp));
  envp_helper (envp);

  fflush (stdout);

  pid_t child_pid = fork ();
  if (child_pid == 0)
    {
      execve (path, argv, envp);
    }

  int status = 0;
  waitpid (child_pid, &status, WUNTRACED);
  child_pid = 0;
  char str_status[2];
  snprintf (str_status, 2, "%d", WEXITSTATUS (status));
  hash_insert ("?", str_status);

  for (int i = 0; i < sizeof (envp) / sizeof (envp[0]); i++)
    {
      free (envp[i]);
    }
}

void
run_command_pipes (char *buffer)
{
  char *command;
  if (strstr (buffer, "cat"))
    command = "cat";
  if (strstr (buffer, "cut"))
    command = "cut";
  if (strstr (buffer, "env"))
    command = "env";
  if (strstr (buffer, "head"))
    command = "head";
  if (strstr (buffer, "ls"))
    command = "ls";

  char path[MAXLENGTH];
  memset (path, 0, MAXLENGTH);
  path_helper (path, command);

  char *argv[MAXLENGTH];
  memset (argv, 0, sizeof (argv));
  args_helper (argv, buffer);

  char *envp[MAXLENGTH];
  memset (envp, 0, sizeof (envp));
  envp_helper (envp);

  execve (path, argv, envp);
}

void
path_helper (char *path, char *name)
{
  getcwd (path, MAXLENGTH);
  strncat (path, "/bin/", 6);
  strncat (path, name, strlen (name));
}

void
args_helper (char **argv, char *buf)
{
  buf[strlen (buf) - 1] = ' '; // working around a bug in cut
  int i = 0;
  char *ptr = strtok (buf, " ");
  while (ptr != NULL)
    {
      argv[i++] = ptr;
      ptr = strtok (NULL, " ");
    }
}

void
envp_helper (char **envp)
{
  char **keys = hash_keys ();
  for (int i = 0; keys[i] != NULL; i++)
    {
      char str[MAXLENGTH]; // arbitrary
      memset (str, 0, sizeof (str));
      if (hash_find (keys[i]) != NULL)
        {
          char *str = (char *)calloc (MAXLENGTH, sizeof (char));
          strncat (str, keys[i], strlen (keys[i]));
          strncat (str, "=", 2);
          strncat (str, hash_find (keys[i]), strlen (hash_find (keys[i])));
          envp[i] = str;
        }
    }
}
