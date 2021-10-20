#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void print_info(char* process_num) {
    char process_dir_name[256], process_fd_dir_name[256], process_cmd_dir_name[256];
    strcpy(process_dir_name, "/proc/");
    strcat(process_dir_name, process_num);
    
    strcpy(process_fd_dir_name, process_dir_name);
    strcat(process_fd_dir_name, "/fd/");

    strcpy(process_cmd_dir_name, process_dir_name);
    strcat(process_cmd_dir_name, "/comm");

    //printf("%s %s\n", process_dir_name, process_fd_dir_name);

    //get pts
    char * ptr;
    DIR* fd_dir = opendir (process_fd_dir_name); 
    struct dirent *pent = NULL;
    while (pent = readdir(fd_dir)) {
        char * name = pent->d_name;
        if (strcmp(name, ".")*strcmp(name, "..") != 0) {
            if (pent->d_type == DT_LNK) {
                char buf[256];
                char link_path[256];
                strcpy(link_path, process_fd_dir_name);
                strcat(link_path, name);
                //readlink(link_path, buf, 256);
                realpath(link_path, buf);
                if ((ptr = strstr(buf, "/dev/pts/")) != NULL) {
                    ptr += 5;
                }
            }
        }
        
    }


    //get_cmd
    FILE * cmd = fopen(process_cmd_dir_name, "r");
    char cmdname[256];
    fgets(cmdname, 256, cmd);
    



    printf("%7s %-7s%s\n", process_num, ptr ? ptr : "?", cmdname);
    cmdname[0] = 0;
}

void scan (const char* directory) {
    DIR* pdir = opendir (directory); 
    struct dirent *pent = NULL;

    printf("%7s %-7s%s\n", "PID", "TTY", "CMD");

    while (pent = readdir (pdir)) {
        struct stat path_stat;
        char next_dir_name[256];
        char * name = pent->d_name;
        //printf("%s/%s\n", directory, name);
        if (pent->d_type == DT_DIR && (strcmp(name, ".")*strcmp(name, "..") != 0)) {
            //strcpy(next_dir_name, directory);
            //strcat(next_dir_name, "/");
            //strcat(next_dir_name, name);
            //scan(next_dir_name);
            if (name[0] >= '0' && name[0] <= '9') {
                print_info(name);
            }
        }
    }
}
            
      


int main() {
    scan("/proc");
}