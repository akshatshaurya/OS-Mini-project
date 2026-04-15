#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>

#define SOCKET_PATH "/tmp/mini_runtime.sock"

static int run_supervisor(const char *rootfs)
{
    int server_fd, client_fd;
    struct sockaddr_un addr;
    char buffer[1024];

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    unlink(SOCKET_PATH);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 5);

    printf("Supervisor running...\n");

    while (1) {
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) continue;

        int n = read(client_fd, buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Received command: %s\n", buffer);

            pid_t pid = fork();

            if (pid == 0) {
                if (unshare(CLONE_NEWPID | CLONE_NEWNS) < 0) {
                    perror("unshare");
                    exit(1);
                }

                pid_t pid2 = fork();

                if (pid2 == 0) {
                    printf("Inside container\n");

                    FILE *f = fopen("/sys/fs/cgroup/mycontainer/cgroup.procs", "w");
                    if (f) {
                        fprintf(f, "%d\n", getpid());
                        fclose(f);
                    }

                    if (chroot(rootfs) < 0) {
                        perror("chroot");
                        exit(1);
                    }
                    chdir("/");

                    execl("/bin/sh", "/bin/sh", NULL);
                    perror("exec");
                    exit(1);
                } else {
                    wait(NULL);
                }

                exit(0);
            } else {
                printf("Container started with PID: %d\n", pid);
            }
        }

        close(client_fd);
    }

    return 0;
}

static int send_control_request()
{
    int sockfd;
    struct sockaddr_un addr;

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));

    char msg[] = "start request";
    write(sockfd, msg, strlen(msg));

    close(sockfd);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2) return 1;

    if (strcmp(argv[1], "supervisor") == 0)
        return run_supervisor(argv[2]);

    if (strcmp(argv[1], "start") == 0)
        return send_control_request();

    return 0;
}