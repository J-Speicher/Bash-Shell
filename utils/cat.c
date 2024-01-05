#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define MAXLENGTH 100 // arbitrary value

static void usage (void);

int
main (int argc, char *argv[])
{
  // Open a file and print out its contents.
  // FILE *fptr;
  // char c;

  if (argc <= 0 || argv == NULL || argv[0] == NULL)
  {
    usage();
    exit(0);
  } 
    
  // // Open file
  // fptr = fopen(argv[0], "r");
  // if (fptr == NULL)
  // {
  //   usage();
  //   exit(0);
  // }
  
  // // Read and print contents from file
  // c = fgetc(fptr);
  // while (c != EOF)
  // {
  //   printf ("%c", c);
  //   c = fgetc(fptr);
  // }
  
  // fclose(fptr);

  // char path[MAXLENGTH];
  // memset (path, 0, MAXLENGTH);
  // getcwd (path, MAXLENGTH);
  
  // strncat(path, "/", 2);
  // strncat(path, argv[0], strlen(argv[0]));

  execlp ("cat", "cat", argv[0], NULL);

  return EXIT_SUCCESS;
}

static void
usage (void)
{
  printf ("cat, print the contents of a file\n");
  printf ("usage: cat FILE\n");
}
