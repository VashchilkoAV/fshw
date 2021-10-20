#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>

#define MINORBITS        20
#define MINORMASK        ((1U << MINORBITS) - 1)

#define MAJOR(dev)        ((unsigned int) ((dev) >> MINORBITS))
#define MINOR(dev)        ((unsigned int) ((dev) & MINORMASK))
#define MKDEV(ma,mi)      (((ma) << MINORBITS) | (mi))



int SearchProcUnix(char * inode, char * buf) {
    int a = 0;
    char tmp[256];
    FILE * f = fopen("/proc/net/unix", "r");
    char * ptra = NULL, *ptrb = NULL;
    //printf("haha\n");
    while(fgets(tmp, 256, f)) {
        ptra = ptrb = NULL;
        ptra = strstr(tmp, inode);
        if (ptra != NULL) {
            a = 1;
            break;
        }
    }
    fclose(f);
    //printf("haha\n");
    if (a == 1) {
        ptra = strstr(tmp, "/");
        if (ptra != NULL) {
            int j = 0;
            for (j = 0; ptra[j] != '\n'; j++);
            ptra[j] = '\0';
            strcpy(buf, ptra);
        } else {
             strcpy(buf, "");
        }
        return 1;
    }
    return 0;
}

char * getUser(uid_t uid) {
    struct passwd *pws;
    pws = getpwuid(uid);
    return pws->pw_name;
}

struct type {
    int islnk;
    int isreg;
    int isdir;
    int ischr;
    int isblk;
    int isfifo;
    int issock;
};

struct type ParseMode(mode_t mode) {
    struct type result = {S_ISBLK(mode), S_ISREG(mode), S_ISDIR(mode), S_ISCHR(mode), S_ISBLK(mode), S_ISFIFO(mode), S_ISSOCK(mode)};
    return result;
}


void print_info(char* process_num) {
    char process_dir_name[256], process_fd_dir_name[256], process_cmd_dir_name[256];
    strcpy(process_dir_name, "/proc/");
    strcat(process_dir_name, process_num);
    
    strcpy(process_fd_dir_name, process_dir_name);
    strcat(process_fd_dir_name, "/fd/");

    strcpy(process_cmd_dir_name, process_dir_name);
    strcat(process_cmd_dir_name, "/comm");

    //get cmdname
    FILE * cmd = fopen(process_cmd_dir_name, "r");
    char cmdname[256];
    fgets(cmdname, 256, cmd);
    fclose(cmd);
    int i;
    for (i = 0; cmdname[i]; i++);
    cmdname[i-1] = 0;


    //get pts
    DIR* fd_dir = opendir (process_fd_dir_name); 
    struct dirent *pent = NULL;
    while (pent = readdir(fd_dir)) {
        char * fdescname = pent->d_name;
        if (strcmp(fdescname, ".")*strcmp(fdescname, "..") != 0) {
            if (pent->d_type == DT_LNK) {
                char smode[256];
                char buf[256];
                char fdesc_path[256];
                char node[256];
                strcpy(fdesc_path, process_fd_dir_name);
                strcat(fdesc_path, fdescname);

                //getting stat
                struct stat st;
                stat(fdesc_path, &st);
                //printf("%d %d\n", st.st_mode, st.st_uid);

                //getting user
                char * user = getUser(st.st_uid);

                //getting mode
                struct type mode = ParseMode(st.st_mode);


                //getting real path of the link link
                sprintf(node, "%lu", st.st_ino);
                //printf("%u, %u", MAJOR(st.st_dev), MINOR(st.st_dev));
                realpath(fdesc_path, buf);
                


                int res = 0;

                if (mode.islnk)
                    strcpy(smode, "LINK");
                if (mode.isfifo)
                    strcpy(smode, "FIFO");
                if (mode.issock) {
                    strcpy(smode, "sock");
                    char * p = strstr(buf, ":");
                    p++;
                    p[0] = ' ';
                    int i;
                    for (i = 0; p[i] != ']'; i++);
                    p[i] = ' ';
                    //puts(p);
                    res = SearchProcUnix(p, buf);
                    if (!res) {
                        //search web sock
                    }
                }
                if (mode.isreg) 
                    strcpy(smode, "REG");
                if (mode.isdir)
                    strcpy(smode, "DIR");
                if (mode.ischr)
                    strcpy(smode, "CHR");
                //a_inode
                char* filename = NULL;
                filename = buf;
                char * delim = NULL;
                if ((delim = strstr(buf, "anon_inode")) != NULL) {
                    strcpy(smode, "a_inode");
                    filename = strstr(buf, ":");
                    filename++;
                }

                //filename = buf;
                


                


                printf("%-9.9s %-7s %7s %7s %7s %7s %s\n", cmdname, process_num, user, fdescname, smode, node, filename);
            }
        }   
    }
    closedir(fd_dir);
    cmdname[0] = 0;
}

void scan (const char* directory) {
    DIR* pdir = opendir (directory); 
    struct dirent *pent = NULL;

    printf("%-9.9s %-7s %7s %7s %7s %7s %s\n", "COMMAND", "PID", "USER", "FD", "TYPE", "NODE", "NAME");

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