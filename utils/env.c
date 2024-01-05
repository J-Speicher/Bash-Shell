#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../src/builtins.c"
#include "../src/hash.c"

static void usage (void);
void hash_helper (char**);

int
main (int argc, char *argv[], char *envp[])
{
  if (argc <= 0 || argv == NULL || envp == NULL)
  {
    usage();
    return EXIT_FAILURE;
  }

  hash_init(MINSIZE);

  for (int i = 1; argv[i] != NULL; i++)
    {
      // reading something like ALPHA=beta
      if (strstr(argv[i], "="))
        {
          export(argv[i]);
        }
      // reading ./bin/env
      else if (strstr(argv[i], "env") && argv[i+1] == NULL)
      {
        char **env = hash_keys();
        for (int i = 0; env[i] != NULL; i++)
          {
            printf("%s=%s\n", env[i], hash_find(env[i]));
          }
      }
    }
  return EXIT_SUCCESS;
}

static void
usage (void)
{
  printf ("env, set environment variables and execute program\n");
  printf ("usage: env [name=value ...] PROG ARGS\n");
}

