#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "../include/env.h"
#include "../include/config.h"

static void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;

    char line[512];
    while (fgets(line, sizeof(line), f)) {
        // trim leading/trailing whitespace
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '#' || *p == '\n' || *p == '\0') continue;

        char *eq = strchr(p, '=');
        if (!eq) continue;

        *eq = '\0';
        char *name = p;
        char *value = eq + 1;

        // trim newline
        value[strcspn(value, "\r\n")] = '\0';

        env_set(name, value);
    }

    fclose(f);
}

void load_satisconf(void) {
    char path[MAX_PATH];

    // 1. Try local .satisconf
    strcpy(path, ".satisconf");
    load_file(path);

    // 2. Try user directory
    char *home = getenv("USERPROFILE");
    if (home) {
        snprintf(path, sizeof(path), "%s\\.satisconf", home);
        load_file(path);
    }
}
