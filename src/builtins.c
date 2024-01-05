#include "hash.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void trim_spaces (char *);

pid_t child_pid = 0;
char *wd = "/";

// Given a file path, give PATH a new value
int
cd (char *path)
{

  wd = malloc (strlen (path) + 1);
  strcpy (wd, path + 3);
  wd[strlen (wd)] = '\0';

  return setenv ("PATH", wd, 1);
}

// Given a message as input, print it to the screen followed by a
// newline ('\n'). If the message contains the two-byte escape sequence
// "\\n", print a newline '\n' instead. No other escape sequence is
// allowed. If the sequence contains a '$', it must be an environment
// variable or the return code variable ("$?"). Environment variable
// names must be wrapped in curly braces (e.g., ${PATH}).
//
// Returns 0 for success, 1 for errors (invalid escape sequence or no
// curly braces around environment variables).
int
echo (char *message)
{
  // char *replace = strstr(message, "\\n");

  char *copy = malloc (strlen (message));
  strcpy (copy, message + 5);

  int i = 0;
  for (i = 0; copy[i]; i++)
    {
      if (copy[i] == '\\' && copy[i + 1] == 'n')
        {
          copy[i] = '\n';
          copy[i + 1] = ' ';
        }
      else if (copy[i] == '$')
        {
          if (copy[i + 1] != '\0' && copy[i + 1] == '{')
            {
              int j = i + 1;
              int k = i + 1;
              while (copy[k] != '}' && copy[k] != '\0')
                {
                  k++;
                }
              j++;
              int size = k - j;
              char key[size];
              memset (key, 0, size);
              strncat (key, &copy[j], size);
              char *value = hash_find (key);
              char str[3];
              memset (str, 0, sizeof (str));
              strncat (str, &copy[i - 2], 2);
              // printf("%s\n", key);
              if (value != NULL)
                printf ("%s%s", str, value);
              else
                {
                  char str[3];
                  memset (str, 0, sizeof (str));
                  strncat (str, &copy[i - 2], 2);
                  printf ("%s\n", str);
                }
              return 0;
            }
          else if (copy[i + 1] != '\0' && copy[i + 1] == '?')
            {
              char *str = hash_find ("?");
              printf ("%s\n", str);
              return 0;
            }
        }
    }

  trim_spaces (copy);

  printf ("%s", copy); // skips "echo" and only prints what's after it
  free (copy);
  return 0;
}

// Given a key-value pair string (e.g., "alpha=beta"), insert the mapping
// into the global hash table (hash_insert ("alpha", "beta")).
//
// Returns 0 on success, 1 for an invalid pair string (kvpair is NULL or
// there is no '=' in the string).
int export(char *kvpair)
{
  // better way to check if string is null?
  if (kvpair == NULL || strstr (kvpair, "=") == NULL)
    return 1;
  char *kv = strdup (kvpair);
  char *key = strtok (kv, "=");
  char *value = strtok (NULL, "=");
  hash_insert (key, value);
  return 0;
}

// traverse $PATH for executable command
char *
find_executable (const char *command)
{
  char *path = getenv ("PATH");
  if (path == NULL)
    {
      fprintf (stderr, "PATH environment variable not found.\n");
      return NULL;
    }

  // Create a copy of the PATH variable so we can modify it
  char *path_copy = strdup (path);
  if (path_copy == NULL)
    {
      perror ("Memory allocation error");
      return NULL;
    }

  char *token = strtok (path_copy, "/");
  while (token != NULL)
    {
      // Construct the full path by combining the directory and the command
      char *full_path = malloc (strlen (token) + strlen (command) + 2);

      if (full_path == NULL)
        {
          perror ("Memory allocation error");
          free (path_copy);
          return NULL;
        }

      // Create the full path
      sprintf (full_path, "%s/%s", token, command);
      printf ("PATH %s\n", full_path);

      // Check if the file exists and is executable
      if (access (full_path, X_OK) == 0)
        {
          // Clean up memory and return the full path
          free (path_copy);
          return full_path;
        }

      free (full_path);
      token = strtok (NULL, "/");
    }

  // Clean up memory and return NULL if the executable was not found
  free (path_copy);
  return NULL;
}

// check if file path is executable
int
is_executable (const char *path)
{
  return access (path, X_OK) == 0;
}

// Prints the current working directory (see getcwd()). Returns 0.
int
pwd (void)
{
  // char *getcwd(char *buf, size_t size);
  const char *s = getenv ("PATH");
  printf ("%s", s);

  return 0;
}

// Removes a key-value pair from the global hash table.
// Returns 0 on success, 1 if the key does not exist.
int
unset (char *key)
{
  if (key == NULL)
    return 1;
  hash_remove (key);
  return 0;
}

// adjust string to have at most one space at a time
void
trim_spaces (char *str)
{
  int i, x = 0;
  for (i = 0; str[i]; i++)
    {
      if (!isspace (str[i]) || (i > 0 && !isspace (str[i - 1])))
        str[x++] = str[i];
    }
  // end shortened string
  str[x] = '\0';
}

// Given a string of commands, find their location(s) in the $PATH global
// variable. If the string begins with "-a", print all locations, not just
// the first one.
//
// Returns 0 if at least one location is found, 1 if no commands were
// passed or no locations found.
int
which (char *cmdline)
{
  char *token = strtok (cmdline, " ");
  token = strtok (NULL, " ");
  if (token == NULL)
    return 1;

  if (!strncmp (token, "./", 2))
    {
      // token = token + 2;

      /* ls command is supported */
      if (strstr (token, "ls"))
        {
          printf ("%s", token);
        }
      else if (strstr (token, "cut"))
        {
          // Not sure if is necessary to check each if
          // if (is_executable(token))
          // {
          printf ("%s", token);
          // }
        }
      else if (strstr (token, "head"))
        {
          printf ("%s", token);
        }
      else if (strstr (token, "rm"))
        {
          printf ("%s", token);
        }
      else if (strstr (token, "cat"))
        {
          printf ("%s", token);
        }
      else
        {
          printf ("Invalid command: %s\n", token);
          return 1;
        }
    }
  else
    {
      if (!strncmp (token, "ls", 2))
        {
          size_t len = strlen (wd);
          char *str2 = malloc (len + 4);
          strncpy (str2, wd, strlen (wd) - 1);
          strcat (str2, "/ls");
          // printf("%s\n", str2);
          // printf("%s\n", wd);
          // // setenv ("PATH", wd, 1);
          // // printf("PATH %s\n", getenv("PATH"));
          // const char* s = find_executable("ls");
          if (is_executable (str2))
            {
              printf ("%s\n", str2);
              setenv ("PATH", str2, 1);
              wd = str2;
            }
        }
      else if (!strncmp (token, "cd", 2))
        {
          printf ("cd: dukesh built-in command\n");
        }
      else if (!strncmp (token, "echo", 4))
        {
          printf ("echo: dukesh built-in command\n");
        }
      else if (!strncmp (token, "pwd", 3))
        {
          printf ("pwd: dukesh built-in command\n");
        }
      else if (!strncmp (token, "which", 5))
        {
          printf ("which: dukesh built-in command\n");
        }
      else if (!strncmp (token, "export", 5))
        {
          printf ("export: dukesh built-in command\n");
        }
      else
        {
          printf ("error\n"); // placeholder
        }
    }

  return 0;
}