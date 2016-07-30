#ifndef CONFIG_H
#define CONFIG_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <chibi/eval.h>

#include "macros.h"

struct beaugeste_config {
    char* path;
    sexp ctx;
};

char* beaugeste_get_config_path(void);

int beaugeste_open_config_file(struct beaugeste_config* cfg, const char* path);
void beaugeste_close_config_file(struct beaugeste_config* cfg);

char* beaugeste_config_read_string(struct beaugeste_config* cfg, const char* key, const char* deflt);
void beaugeste_config_act(struct beaugeste_config* cfg, const char* key);

#endif // CONFIG_H
