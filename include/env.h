#ifndef ENV_H
#define ENV_H

#define MAX_ENV 64

struct envpair {
    char *name;
    char *value;
};

extern struct envpair env_table[MAX_ENV];
extern int env_count;

void env_set(const char *name, const char *value);
const char *env_get(const char *name);

// Exports every entry in env_table into the real process environment
// so that child processes spawned by satiscli inherit them.
void env_export(void);

#endif
