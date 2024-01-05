#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

// You may assume that lines are no longer than 1024 bytes
#define LINELEN 1024
#define MAXLENGTH 100 // arbitrary

static void usage (void);

int
main (int argc, char *argv[]) // unfinished... I think it works
{
  if (argc <= 0 || argv == NULL)
  {
    usage();
    return EXIT_FAILURE;
  }

  char *num_lines = "5";
  int file_arg = 1; // index of argv that file is in
  // int fd;
  char path[MAXLENGTH];
  memset (path, 0, MAXLENGTH);
  
  if (strncmp (argv[1], "-n", 2) == 0)
    {
      if (argv[2] != NULL)
        {
          file_arg = 3;
          num_lines = argv[2];
        }
    }
  if (argv[file_arg] != NULL)
    {
      getcwd (path, sizeof(path)); // was MAXLENGTH
      strncat (path, "/", 2);
      strncat (path, argv[file_arg], MAXLENGTH - strlen(path) - 1); 
      // fd = open (path, O_RDONLY);
    }
  else
    {
      strncat (path, "/dev/stdin", 11);
    }
  if (file_arg == 3)
    {
      execlp ("head", "head", argv[1], argv[2], path, NULL); 
    }
  else
    {
      execlp ("head", "head", "-n", num_lines, path, NULL);
    }
  
  // close(fd);

  return true;
}

static void
usage (void)
{
  printf ("head, prints the first few lines of a file\n");
  printf ("usage: head [FLAG] FILE\n");
  printf ("FLAG can be:\n");
  printf ("  -n N     show the first N lines (default 5)\n");
  printf ("If no FILE specified, read from STDIN\n");
}
