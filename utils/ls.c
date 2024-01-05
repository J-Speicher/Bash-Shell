#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../src/hash.c"

#define MAXLENGTH 100 // arbitrary value

static void usage (void);

int
main (int argc, char *argv[]) // unfinished
{
  if (argc <= 0 || argv == NULL) 
	{
        usage();
        return EXIT_FAILURE;
    }

    int file_arg = 1;
    if (file_arg < argc && ((strstr(argv[1], "-a") != NULL || strstr(argv[1], "-s") != NULL))) 
        file_arg = 2;

    char path[MAXLENGTH];
    if (getcwd(path, sizeof(path)) == NULL) 
    {
        perror("getcwd");
        return EXIT_FAILURE;
    }

    // if (file_arg < argc) 
    // {
        char *direct = argv[file_arg];
        // if (dir[strlen(dir) - 1] == '\n')
        //     dir[strlen(dir) - 1] = '\0'; // Remove newline character if present
        strncat(path, "/", 2);
        strncat(path, direct, sizeof(path) - strlen(path) - 1);
    // }

    char *flags = "-1tr"; // Default 

    if (file_arg == 2) {
        if (strstr(argv[1], "-a") != NULL || strstr(argv[1], "-s") != NULL) {
            flags = argv[1];
        }
    }

    // if (strncmp(flags, "-1tr", 5) == 0)
    //     {
    //         execlp("ls", "ls", flags, path, NULL);
    //     }
    // else // reconstruct the ls command for accurate file sizes
    //     {
            DIR *dir;
            struct dirent *d;
            dir = opendir(path);
            if (dir == NULL)
                return (1);
            // assert (dir != NULL);
            d = readdir(dir);
            while (d != NULL)
                {
                    if (d->d_name[0] != '.' || (d->d_name[0] == '.' && strstr(flags, "a")))
                        {
                            struct stat info;
                            char name[sizeof(path) + strlen(d->d_name) + 1];
                            strncpy(name, path, sizeof(name));
                            strncat(name, "/", 2);
                            strncat(name, d->d_name, sizeof(name));
                            int fd = open (name, O_RDONLY);
                            assert (fstat (fd, &info) >= 0);
                            if (strstr(flags, "s"))
                                printf("%ld ", info.st_size);
                            printf("%s\n", d->d_name);
                        }
                    d = readdir(dir);
                }
            closedir(dir);
            return(0);
        // }

    // pipe output of ls to grep to remove "total" line
    // int pipefd[2];
    // assert (pipe (pipefd) >= 0);
    // pid_t child_pid = fork ();
    // assert (child_pid >= 0);
    // if (child_pid == 0)
    //     {
    //         close (pipefd[0]); // close read end
    //         dup2 (pipefd[1], STDOUT_FILENO); // change stdout to pipe write end
    //         execlp ("ls", "ls", flags, path, NULL);
    //     }
    // else
    //     {
    //         close (pipefd[1]); // close write end
    //         dup2 (pipefd[0], STDIN_FILENO); // read from pipe instead of stdin
    //         execlp ("grep", "grep", "-v", "total", NULL); // remove "total" line
    //     }

    perror("execlp");
    return EXIT_FAILURE;
}

static void
usage (void)
{
  printf ("ls, list directory contents\n");
  printf ("usage: ls [FLAG ...] [DIR]\n");
  printf ("FLAG is one or more of:\n");
  printf ("  -a       list all files (even hidden ones)\n");
  printf ("  -s       list file sizes\n");
  printf ("If no DIR specified, list current directory contents\n");
}
