#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/token.h"
#include "../include/env.h"

static char *dup_range(const char *start, const char *end) {
    int len = end - start;
    char *s = malloc(len + 1);
    memcpy(s, start, len);
    s[len] = 0;
    return s;
}

struct token_list tokenize(const char *line) {
    struct token_list out = {0};

    const char *p = line;
    while (*p) {
        // skip spaces
        while (*p == ' ' || *p == '\t')
            p++;

        if (!*p) break;

        // operators
        if (*p == '|') {
            out.t[out.count++] = (struct token){ strdup("|"), 1 };
            p++;
            continue;
        }
        if (*p == '>') {
            out.t[out.count++] = (struct token){ strdup(">"), 2 };
            p++;
            continue;
        }
        if (*p == '<') {
            out.t[out.count++] = (struct token){ strdup("<"), 3 };
            p++;
            continue;
        }
        if (*p == '&') {
            out.t[out.count++] = (struct token){ strdup("&"), 4 };
            p++;
            continue;
        }

        // quoted string
        if (*p == '"') {
            p++;
            const char *start = p;
            while (*p && *p != '"')
                p++;
            out.t[out.count++] = (struct token){ dup_range(start, p), 0 };
            if (*p == '"') p++;
            continue;
        }

        // normal word
        const char *start = p;
        while (*p &&
               *p!=' ' && *p!='\t' &&
               *p!='|' && *p!='>' &&
               *p!='<' && *p!='&')
        {
            p++;
        }

        char *word = dup_range(start, p);

        // ENV EXPANSION
        if (word[0] == '$') {
            const char *val = env_get(word + 1);
            free(word);
            word = strdup(val ? val : "");
        }

        out.t[out.count++] = (struct token){ word, 0 };
    }

    return out;
}
