#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROC_DIR "/proc"

// Получаем имя процесса по PID
int get_process_name(const char* pid, char* name, size_t len) {
    char comm_file[256];
    snprintf(comm_file, sizeof(comm_file), "%s/%s/comm", PROC_DIR, pid);

    FILE *file = fopen(comm_file, "r");
    if (!file) {
        return -1;
    }

    if (fgets(name, len, file)) {
        // Убираем завершающий символ новой строки, если есть
        size_t length = strlen(name);
        if (length > 0 && name[length - 1] == '\n') {
            name[length - 1] = '\0';
        }
        fclose(file);
        return 0;
    }

    fclose(file);
    return -1;
}

void list_processes() {
    DIR *dir = opendir(PROC_DIR);
    if (!dir) {
        perror("не смог открыть /proc");
        return;
    }

    struct dirent *entry;
    printf("Список всех процессов:\n");
    while ((entry = readdir(dir)) != NULL) {
        int pid = atoi(entry->d_name);
        if (pid > 0) {
            char name[256];
            if (get_process_name(entry->d_name, name, sizeof(name)) == 0) {
                printf("PID: %s — Имя: %s\n", entry->d_name, name);
            } else {
                printf("PID: %s — Имя: [Неизвестно]\n", entry->d_name);
            }
        }
    }
    closedir(dir);
}

void show_libraries_for_process(const char* pid) {
    char maps_file[256];
    snprintf(maps_file, sizeof(maps_file), "%s/%s/maps", PROC_DIR, pid);

    FILE *file = fopen(maps_file, "r");
    if (!file) {
        perror("ошибка при чтении карты процесса");
        return;
    }

    char line[1024];
    char name[256];
    get_process_name(pid, name, sizeof(name));

    printf("Библиотеки для процесса '%s' (PID: %s):\n", name, pid);
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, ".so")) { // Ищем строки с .so (библиотеки)
            printf("%s", line);
        }
    }
    fclose(file);
}

int main() {
    printf("Все процессы в системе:\n");
    list_processes();

    printf("Введите PID процесса, чтобы узнать используемые библиотеки: ");
    char pid[16];
    scanf("%s", pid);

    show_libraries_for_process(pid);

    return 0;
}