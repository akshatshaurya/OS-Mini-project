#include <stdio.h>
#include <unistd.h>

int main() {
    while (1) {
        FILE *f = fopen("temp.txt", "w");
        fprintf(f, "IO pulse\n");
        fclose(f);
        sleep(1);
    }
    return 0;
}