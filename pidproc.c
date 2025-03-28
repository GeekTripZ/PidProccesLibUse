#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROC_DIR "/proc"

void list_processes() {
    DIR *dir = opendir(PROC_DIR);
    if (!dir) {
        perror("неудалось открыть папку /proc");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Проверяем, является ли запись числом (PID)
        if (atoi(entry->d_name) > 0) {
            printf("PID: %s\n", entry->d_name);
        }
    }
    closedir(dir);
}

void show_libraries_for_process(const char* pid) {
    char maps_file[256];
    snprintf(maps_file, sizeof(maps_file), "%s/%s/maps", PROC_DIR, pid);

    FILE *file = fopen(maps_file, "r");
    if (!file) {
        perror("ошибка карты процессов");
        return;
    }

    char line[1024];
    printf("Билиотеки которые использует PID %s:\n", pid);
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, ".so")) { // Если строка содержит ".so", это библиотека
            printf("%s", line);
        }
    }
    fclose(file);
}

int main() {
    printf("Все процессы в системе:\n");
    list_processes();

    printf("Введи номер процесса, что бы узнать какие бибилотеки юзает: ");
    char pid[16];
    scanf("%s", pid);

    show_libraries_for_process(pid);

    return 0;
}
