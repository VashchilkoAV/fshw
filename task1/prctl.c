#include <sys/prctl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


int main(int argc, char** argv, char** envp) {
    int pid = getpid();
    printf("Pid = %d\n", pid);
    int wait = 0;
    scanf("%d", &wait);
    int status = prctl(PR_SET_MM, PR_SET_MM_ARG_START, argv[1], NULL, NULL);
    if (status == -1) {
        int errnum = errno;
        printf("Errornum: %d, %s\n", errnum, strerror(errnum));
    } else {
        printf("Success!\n");
    }
    printf("List of cmd args:\n");
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }
    printf("List of environment variables:\n");
    for (i = 0; envp[i]; i++) {
        printf("%s\n", envp[i]);
    }

    scanf("%d", &wait);


    return 0;
}