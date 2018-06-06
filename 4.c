#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <libgen.h>
#include <unistd.h>
#include <malloc.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAX_SIGNAL_COUNT 101
#define TREE_NODE_COUNT 8
#define FILE_NAME_LEN 6


/*
TODO: implement the usage of async-safe functions;

*/
char *programName;
pid_t pid, ppid;
struct sigaction act;
int sigCountTotal = 0, 
    countUsr1 = 0,
    countUsr2 = 0;

int savePidToFile(char *fileName, int pid);
long long getTime();
int getPidFromFile(char *fileName);
void proc1Handler(int sig);
void proc2Handler(int sig);
void proc3Handler(int sig);
void proc4Handler(int sig);
void proc5Handler(int sig);
void proc6Handler(int sig);
void proc7Handler(int sig);
void proc8Handler(int sig);

int main (char argc, char *argv[]) {
    programName = basename(argv[0]);
    if ((pid = fork()) == 0) {
        //1st process//
        act.sa_handler = proc1Handler;
        sigaction(SIGUSR2, &act, 0);
        printf("1-й в группе с пидом %d\n", getpgid(getpid()));
        savePidToFile("1.txt", getpid());
        if ((pid = fork()) == 0) {
            //2nd proc//
            act.sa_handler = proc2Handler;
            sigaction(SIGUSR1, &act, 0);    
            sigaction(SIGTERM, &act, 0);  
            setpgid(getpid(), getpid());
            printf("2-й в группе с пидом %d\n", getpgid(getpid()));
            savePidToFile("2.txt", getpid());    
            if ((pid = fork()) == 0) {
                //7th proc//              
                act.sa_handler = proc7Handler;
                sigaction(SIGUSR1, &act, 0);    
                sigaction(SIGTERM, &act, 0);
                while(!getPidFromFile("6.txt"));
                setpgid(getpid(), getPidFromFile("6.txt"));
                printf("7-й в группе с пидом %d\n",  getpgid(getpid()));
                savePidToFile("7.txt", getpid());
                if ((pid = fork()) == 0){
                    //8th proc//
                    act.sa_handler = proc8Handler;
                    sigaction(SIGUSR1, &act, 0);    
                    sigaction(SIGTERM, &act, 0); 
                    setpgid(getpid(), 0);
                    printf("8-й в группе с пидом %d\n", getpgid(getpid()));
                    savePidToFile("8.txt", getpid());
                    while (1) 
                        pause();
                }
                while(1)
                    pause(); 
            }
            if ((pid = fork()) == 0) {
                //6th proc//
                act.sa_handler = proc6Handler;
                sigaction(SIGUSR1, &act, 0);    
                sigaction(SIGTERM, &act, 0);
                setpgid(getpid(), getpid());
                printf("6-й в группе с пидом %d\n", getpgid(getpid()));
                savePidToFile("6.txt", getpid());
                while(1)
                    pause();
            }
            while(1)
                pause();
        }
        if ((pid = fork()) == 0) {
            //3rd proc//           
            act.sa_handler = proc3Handler;
            sigaction(SIGUSR1, &act, 0);   
            sigaction(SIGTERM, &act, 0);
            while (!getPidFromFile("2.txt"));
            setpgid(getpid(), getPidFromFile("2.txt"));
            printf("3-й в группе с пидом %d\n", getpgid(getpid()));
            savePidToFile("3.txt", getpid());
            while(1)
                pause();            
        }
        
        if ((pid = fork()) == 0) {
            //4rd proc//
            act.sa_handler = proc4Handler;
            sigaction(SIGUSR2, &act, 0);    
            sigaction(SIGTERM, &act, 0);
            printf("4-й в группе с пидом %d\n", getpgid(getpid()));
            savePidToFile("4.txt", getpid());
            while(1)
                pause();            
        }

        if ((pid = fork()) == 0) {
            //5rd proc//
            act.sa_handler = proc5Handler;
            sigaction(SIGUSR1, &act, 0);    
            sigaction(SIGTERM, &act, 0);
            while (!getPidFromFile("6.txt"));
            setpgid(getpid(), getPidFromFile("6.txt"));
            printf("5-й в группе с пидом %d\n", getpgid(getpid()));
            savePidToFile("5.txt", getpid());
            while(1)
                pause();            
        }
        while (1)
            pause();
    } else if (pid > 0){
        char *fileName = (char *)malloc(sizeof(char) * FILE_NAME_LEN);
        int fileCount = 0;
        while (fileCount < 8) {
            fileCount = 0;
            for (int i = 1; i <= 8; i++) {
                sprintf(fileName, "%d.txt", i);
                if (getPidFromFile(fileName)) {
                    fileCount++;
                }
            }
        }
        printf ("All processes created and paused!\n");
        //char command[256];
        //sprintf(command, "pstree %d", getpid()); = 0;
        //system(command);
        system("ps --forest");
        int pid1 = getPidFromFile("1.txt"), wstat;
        kill(pid1, SIGUSR2);
        waitpid(pid1, &wstat, 0);
        /*for (int i = 1; i <= 8; i++) {
            sprintf(fileName, "%d.txt", i);
            remove(fileName);
        }*/
    }
}

int savePidToFile(char *fileName, int pid) {
    FILE *file;
    printf("Файл %s создан!\n", fileName);
    if (!(file = fopen(fileName,"w"))) {
        fprintf(stderr, "%d %s %s\n", getpid(), programName, fileName);
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%d\n", pid);
    fclose(file);
    return 1;
}

int getPidFromFile(char *fileName) {
    FILE *file;
    int pid;
    if (!(file = fopen(fileName, "r"))){
        return 0;
    }
    fscanf(file, "%d", &pid);
    if (pid) {
        fclose(file);
        return pid;
    }
    fclose(file);
    return 0;
}

long long getTime(){
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_usec;
}

void proc1Handler(int sig) {
    if (sig == SIGUSR2) {
        printf("%d %d %d %s %llu\n", 1, getpid(),
        getppid(), "получил SIGUSR2", getTime());
        countUsr2++;
        sigCountTotal++;
        if (sigCountTotal != MAX_SIGNAL_COUNT){
            kill(-getPidFromFile("2.txt"), SIGUSR1);
            printf("%d %d %d %s %llu\n", 1, getpid(),
            getppid(), "отправил SIGUSR2", getTime());
        } else {
            kill(getPidFromFile("4.txt"), SIGTERM);
            kill(-getPidFromFile("2.txt"), SIGTERM);
            kill(-getPidFromFile("6.txt"), SIGTERM);
            kill(getPidFromFile("8.txt"), SIGTERM);
            int wstatus;
            while (wait(&wstatus) != -1);
            remove("1.txt");
            exit(EXIT_SUCCESS);
        }
    }
}

void proc2Handler(int sig){
    if (sig == SIGUSR1) {
        printf("%d %d %d %s %llu\n", 2, getpid(),
        getppid(), "получил SIGUSR1", getTime());
        countUsr1++;
    } else {
        printf("2 %d %d завершил работу после %d сигнала SIGUSR1 и %d сигнала SIGUSR2\n",
        getpid(), getppid(), countUsr1, countUsr2);
        remove("2.txt");
        exit(EXIT_SUCCESS);
    } 
}

void proc3Handler(int sig){
    if (sig == SIGUSR1) {
        printf("%d %d %d %s %llu\n", 3, getpid(),
        getppid(), "получил SIGUSR1", getTime());
        countUsr1++;
        kill(getPidFromFile("4.txt"), SIGUSR2);
        printf("%d %d %d %s %llu\n", 3, getpid(),
        getppid(), "отправил SIGUSR2", getTime());
    } else {
        printf("3 %d %d завершил работу после %d сигнала SIGUSR1 и %d сигнала SIGUSR2\n",
        getpid(), getppid(), countUsr1, countUsr2);
        remove("3.txt");
        exit(EXIT_SUCCESS);
    } 
}

void proc4Handler(int sig){
    if (sig == SIGUSR2) {
        printf("%d %d %d %s %llu\n", 4, getpid(),
        getppid(), "получил SIGUSR2", getTime());
        countUsr2++;
        kill(-getPidFromFile("6.txt"), SIGUSR1);
        printf("%d %d %d %s %llu\n", 4, getpid(),
        getppid(), "отправил SIGUSR1", getTime());
    } else {
        printf("4 %d %d завершил работу после %d сигнала SIGUSR1 и %d сигнала SIGUSR2\n",
        getpid(), getppid(), countUsr1, countUsr2);
        remove("4.txt");
        exit(EXIT_SUCCESS);
    } 
}

void proc5Handler(int sig){
   if (sig == SIGUSR1) {
        printf("%d %d %d %s %llu\n", 5, getpid(),
        getppid(), "получил SIGUSR1", getTime());
        countUsr1++;
    } else {
        printf("5 %d %d завершил работу после %d сигнала SIGUSR1 и %d сигнала SIGUSR2\n",
        getpid(), getppid(), countUsr1, countUsr2);
        remove("5.txt");
        exit(EXIT_SUCCESS);
    } 
}

void proc6Handler(int sig){
    if (sig == SIGUSR1) {
        printf("%d %d %d %s %llu\n", 6, getpid(),
        getppid(), "получил SIGUSR1", getTime());
        countUsr1++;
    } else {
        printf("6 %d %d завершил работу после %d сигнала SIGUSR1 и %d сигнала SIGUSR2\n",
        getpid(), getppid(), countUsr1, countUsr2);
        remove("6.txt");
        exit(EXIT_SUCCESS);
    } 
}

void proc7Handler(int sig){
   if (sig == SIGUSR1) {
        printf("%d %d %d %s %llu\n", 7, getpid(),
        getppid(), "получил SIGUSR1", getTime());
        countUsr1++;
        kill(getPidFromFile("8.txt"), SIGUSR1);
        printf("%d %d %d %s %llu\n", 7, getpid(),
        getppid(), "отправил SIGUSR1", getTime());
   } else {
        printf("7 %d %d завершил работу после %d сигнала SIGUSR1 и %d сигнала SIGUSR2\n",
        getpid(), getppid(), countUsr1, countUsr2);
        remove("7.txt");
        exit(EXIT_SUCCESS);
    } 
}

void proc8Handler(int sig){
    if (sig == SIGUSR1) {
        printf("%d %d %d %s %llu\n", 8, getpid(),
        getppid(), "получил SIGUSR1", getTime());
        countUsr1++;
        kill(getPidFromFile("1.txt"), SIGUSR2);
        printf("%d %d %d %s %llu\n", 8, getpid(),
        getppid(), "отправил SIGUSR2", getTime());
    } else {
        printf("8 %d %d завершил работу после %d сигнала SIGUSR1 и %d сигнала SIGUSR2\n",
        getpid(), getppid(), countUsr1, countUsr2);
        remove("8.txt");
        exit(EXIT_SUCCESS);
    }
}