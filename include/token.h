#ifndef TOKEN_H
#define TOKEN_H

#define MAX_TOKENS 64

struct token {
    char *text;
    int type;   // 0=word, 1=pipe, 2=redir_out, 3=redir_in, 4=background
};

struct token_list {
    struct token t[MAX_TOKENS];
    int count;
};

struct token_list tokenize(const char *line);

#endif
