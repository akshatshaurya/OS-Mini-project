#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    int mb = 0;
    while (1) {
        void *p = malloc(1024 * 1024);
        if (!p) {
            printf("Memory allocation failed\n");
            break;
        }
        memset(p, 0, 1024 * 1024);
        mb++;
        printf("allocation-%d chunk=1MB total=%dMB\n", mb, mb);
        sleep(1);
    }
    return 0;
}