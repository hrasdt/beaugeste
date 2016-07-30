#include "config.h"

char* beaugeste_get_config_path(void){
    char* result = NULL;
    int stat = -1;

    const char* xdg_conf = getenv("XDG_CONFIG_HOME");
    if (xdg_conf != NULL){
        stat = asprintf(&result, "%s/BeauGeste.conf", xdg_conf);
    }
    else {
        const char* home_path = getenv("HOME");
        stat = asprintf(&result, "%s/.config/BeauGeste.conf", home_path);
    }

    if (stat == -1){
        ERR("Could not find the config path!");
        return NULL;
    }
    else {
        return result; // Don't forget to free this!
    }
}

int beaugeste_open_config_file(struct beaugeste_config* cfg, const char* path){
    assert(cfg != NULL);
    assert(strlen(path) >= 1);
    sexp result;

    cfg->ctx = sexp_make_eval_context(NULL, NULL, NULL, 0, 0);
    sexp_load_standard_env(cfg->ctx, NULL, SEXP_SEVEN);
    sexp_load_standard_ports(cfg->ctx, NULL, stdin, stdout, stderr, 0);

    sexp out = sexp_current_output_port(cfg->ctx);

    // First load the preamble.
    // Load the file at the config path.
    // We need to add some sort of "if this doesn't exist, create it", but that's a problem for another day.
    sexp_gc_var2(spath, sdefs);
    sexp_gc_preserve2(cfg->ctx, spath, sdefs);

    const char* home_path = getenv("HOME");
    char* defs_path;
    asprintf(&defs_path, "%s/.local/share/beaugeste_definitions.scm", home_path);
    sdefs = sexp_c_string(cfg->ctx, defs_path, -1);
    result = sexp_load(cfg->ctx, sdefs, NULL);
    if (sexp_exceptionp(result)){
        sexp_print_exception(cfg->ctx, result, out);
    }
    free(defs_path);

    spath = sexp_c_string(cfg->ctx, path, -1);
    result = sexp_load(cfg->ctx, spath, NULL);
    if (sexp_exceptionp(result)){
        sexp_print_exception(cfg->ctx, result, out);
    }

    sexp_gc_release2(cfg->ctx);

    // Copy where we got this from.
    cfg->path = malloc(strlen(path) + 1);
    strcpy(cfg->path, path);

    // do stuff
    return 0; // All good.
}

void beaugeste_close_config_file(struct beaugeste_config* cfg){
    free(cfg->path);
    sexp_destroy_context(cfg->ctx);
}

char* beaugeste_config_read_string(struct beaugeste_config* cfg, const char* key, const char* deflt){
    sexp_gc_var1(sres);
    sexp_gc_preserve1(cfg->ctx, sres);
    char* res = NULL;

    sres = sexp_eval_string(cfg->ctx, key, -1, NULL);
    res = sexp_string_data(sres);
    if (strlen(res) == 0){
        if (deflt == NULL){
            res = NULL;
        }
        else {
            res = malloc(strlen(deflt) + 1);
            strcpy(res, deflt);
        }
    }

    sexp_gc_release1(cfg->ctx);
    return res;
}

void beaugeste_config_act(struct beaugeste_config* cfg, const char* key){
    sexp_gc_var1(sresult);
    sexp_gc_preserve1(cfg->ctx, sresult);
    char* cmd = NULL;

    sresult = sexp_eval_string(cfg->ctx, key, -1, NULL);
    if (sexp_stringp(sresult)){
        // Execute a command.
        cmd = sexp_string_data(sresult);
        system(cmd);
    }
    else if (sexp_procedurep(sresult)){
        // Run a Scheme thunk.
        sexp_eval(cfg->ctx, sresult, NULL);
    }

    sexp_gc_release1(cfg->ctx);
}
