#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// You may assume that lines are no longer than 1024 bytes
#define LINELEN 1024
#define MAXLENGTH 100

static void usage (void);

int
main (int argc, char *argv[])
{
  char *delim = " ";
  char *field = "1";
  int file_arg = 1;

  // definitely a better way to format these if statements
  if (argv[1] != NULL && strstr (argv[1], "-d") != NULL)
  {
    file_arg = 3;
    if (strstr (argv[2], "-") != NULL)
      {
        usage();
        return 1;
      }
    else
      {
        delim = argv[2];
      }
    if (argv[3] != NULL && strstr (argv[3], "-f") != NULL)
    {
      field = argv[4];
      file_arg = 5;
    }
  }
  else if (argv[1] != NULL && strstr (argv[1], "-f") != NULL)
  {
    file_arg = 3;
    if (strstr (argv[2], "-") != NULL)
      {
        usage();
        return 1;
      }
    else
      {
        field = argv[2];
      }
    if (argv[3] != NULL && strstr (argv[3], "-d") != NULL)
    {
      delim = argv[4];
      file_arg = 5;
    }
  }
  char final_field[4];
  memset (final_field, 0, sizeof(final_field));
  strncat (final_field, "-f", 3);
  strncat (final_field, field, 2);

  char final_delim[5];
  memset (final_delim, 0, sizeof(final_delim));
  strncat(final_delim, "-d", 3);
  strncat(final_delim, delim, 2);

  char path[MAXLENGTH];
  memset (path, 0, MAXLENGTH);

  if (argv[file_arg] != NULL)
    {
      getcwd (path, MAXLENGTH);
      strncat(path, "/", 2);
      strncat(path, argv[file_arg], strlen(argv[file_arg]));
    }
  else
    {
      strncat (path, "/dev/stdin", 11);
    }
  
  
  execlp ("cut", "cut", final_delim, final_field, path, NULL);
    
  return EXIT_SUCCESS;
}

static void
usage (void)
{
  printf ("cut, splits each line based on a delimiter\n");
  printf ("usage: cut [FLAG] FILE\n");
  printf ("FLAG can be:\n");
  printf ("  -d C     split each line based on the character C (default ' ')\n");
  printf ("  -f N     print the Nth field (1 is first, default 1)\n");
  printf ("If no FILE specified, read from STDIN\n");
}
