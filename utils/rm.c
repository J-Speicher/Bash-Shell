#include <stdio.h>
#include <stdlib.h>

static void usage (void);

int
main (int argc, char *argv[])
{
  if (argc <= 0 || argv == NULL)
  {
    usage();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

static void
usage (void)
{
  printf ("rm, remove a file\n");
  printf ("usage: rm FILE\n");
}
