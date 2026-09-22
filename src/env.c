#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include "../include/env.h"

struct envpair env_table[MAX_ENV];
int env_count = 0;

void env_set(const char *name, const char *value) {
    // overwrite existing
    for (int i = 0; i < env_count; i++) {
        if (!strcmp(env_table[i].name, name)) {
            free(env_table[i].value);
            env_table[i].value = strdup(value);
            return;
        }
    }

    // add new
    if (env_count < MAX_ENV) {
        env_table[env_count].name = strdup(name);
        env_table[env_count].value = strdup(value);
        env_count++;
    }
}

const char *env_get(const char *name) {
    // check custom envs first
    for (int i = 0; i < env_count; i++) {
        if (!strcmp(env_table[i].name, name))
            return env_table[i].value;
    }

    // fallback to Windows env
    return getenv(name);
}

void env_export(void) {
    for (int i = 0; i < env_count; i++) {
        SetEnvironmentVariableA(env_table[i].name, env_table[i].value);
    }
}
