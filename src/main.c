#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <direct.h>
#include "../include/token.h"
#include "../include/env.h"
#include "../include/config.h"

void run_cmd(char *line) {
    struct token_list tl = tokenize(line);

    if (tl.count == 0)
        return;

    // built-in: exit
    if (!strcmp(tl.t[0].text, "exit")) {
        ExitProcess(0);
    }

    // built-in: cd
    if (!strcmp(tl.t[0].text, "cd")) {
        if (tl.count < 2) {
            printf("cd: missing path\n");
        } else if (_chdir(tl.t[1].text) != 0) {
            printf("cd: cannot enter '%s'\n", tl.t[1].text);
        }
        return;
    }

    // built-in: echo
    if (!strcmp(tl.t[0].text, "echo")) {
        for (int i = 1; i < tl.count; i++)
            printf("%s ", tl.t[i].text);
        printf("\n");
        return;
    }

    if (!strcmp(tl.t[0].text, "set")) {
    if (tl.count < 2) {
        printf("set: usage: set NAME=value\n");
        return;
    }

    char *eq = strchr(tl.t[1].text, '=');
    if (!eq) {
        printf("set: invalid format, expected NAME=value\n");
        return;
    }

    *eq = 0; // split into name + value
    char *name = tl.t[1].text;
    char *value = eq + 1;

    env_set(name, value);
    return;
    }

    // built-in: pwd
    if (strcmp(tl.t[0].text, "pwd") == 0) {
        DWORD size = GetCurrentDirectoryA(0, NULL);
        char *buf = malloc(size);

        if (!buf) {
            fprintf(stderr, "pwd: out of memory\n");
            return;
        }

        if (GetCurrentDirectoryA(size, buf) == 0) {
            fprintf(stderr, "pwd: error %lu\n", GetLastError());
            free(buf);
            return;
        }

        printf("%s\n", buf);
        free(buf);
        return;
    }

    // built-in: ls
    if (strcmp(tl.t[0].text, "ls") == 0) {
        WIN32_FIND_DATAA fd;
        HANDLE h = FindFirstFileA("*", &fd);
        if (h == INVALID_HANDLE_VALUE) {
            printf("ls: cannot read directory\n");
            return;
        }
        do {
        // skip . and ..
        if (!strcmp(fd.cFileName, ".") || !strcmp(fd.cFileName, ".."))
            continue;

        // directories in blue, files normal
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            printf("\x1b[34m%s/\x1b[0m\n", fd.cFileName);
        else
            printf("%s\n", fd.cFileName);

    } while (FindNextFileA(h, &fd));

    FindClose(h);
    return;
    }

    if (strcmp(tl.t[0].text, "clear") == 0) {
        // ANSI clear screen + move cursor to top-left
        printf("\x1b[2J\x1b[H");
        return;
    }


    // external command
    char cmdline[512] = {0};
    for (int i = 0; i < tl.count; i++) {
        strcat(cmdline, tl.t[i].text);
        strcat(cmdline, " ");
    }

    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);

    if (!CreateProcessA(NULL, cmdline, NULL, NULL, TRUE,
                        0, NULL, NULL, &si, &pi)) {
        printf("error: cannot run '%s'\n", tl.t[0].text);
        return;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

static void format_prompt(char *out, DWORD out_size) {
    char cwd[MAX_PATH];
    if (GetCurrentDirectoryA(MAX_PATH, cwd) == 0) {
        strcpy(out, "<?>");
        return;
    }

    // last folder name
    char *last = strrchr(cwd, '\\');
    last = last ? last + 1 : cwd;

    // shorten long paths: C:\Users\Tamsin\... → C:\...\Tamsin
    char short_path[MAX_PATH];
    char *first = strchr(cwd, '\\');
    char *second = first ? strchr(first + 1, '\\') : NULL;

    if (second) {
        // build: C:\...\LastFolder
        snprintf(short_path, sizeof(short_path), "%.*s...\\%s",
                 (int)(second - cwd), cwd, last);
    } else {
        // path is already short
        strcpy(short_path, cwd);
    }

    // resolve name from satiscli env (set via .satisconf)
    const char *name = env_get("NAME");
    if (!name || name[0] == '\0') name = "user";

    // final colored prompt: SATISCLI > name > path >
    snprintf(out, out_size,
        "\x1b[36mSATISCLI\x1b[0m > \x1b[32m%s\x1b[0m > \x1b[33m%s\x1b[0m > ",
        name, short_path
    );
}


int main(void) {
    load_satisconf();
    env_export();
    char line[512];

    while (1) {
        char prompt[256];
        format_prompt(prompt, sizeof(prompt));
        printf("%s", prompt);

        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin))
            break;

        // strip newline
        line[strcspn(line, "\n")] = 0;

        if (line[0] == 0)
            continue;

        run_cmd(line);
    }
}
