#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

static char *ngx_http_requestid(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_command_t  ngx_http_requestid_commands[] = {
    { ngx_string("requestid"),
      NGX_HTTP_MAIN_CONF|NGX_CONF_TAKE1,
      ngx_http_requestid,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },

    ngx_null_command
};


static ngx_http_module_t  ngx_http_requestid_module_ctx = {
    NULL,                                  /* preconfiguration */
    NULL,                                  /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    NULL,                                  /* create location configration */
    NULL                                   /* merge location configration */
};


ngx_module_t  ngx_http_requestid_module = {
    NGX_MODULE_V1,
    &ngx_http_requestid_module_ctx,        /* module context */
    ngx_http_requestid_commands,           /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};

static ngx_int_t
ngx_http_requestid_set_variable(ngx_http_request_t *r,
     ngx_http_variable_value_t *v, uintptr_t data)
{
    u_char *end;
    u_char val[NGX_INT32_LEN + 1];

    end = ngx_sprintf(val, "%uD", ngx_random());
    *end = 0;

    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    v->len = end - val;

    v->data = ngx_pnalloc(r->pool, v->len + 1);
    if (v->data == NULL) {
        return NGX_ERROR;
    }

    ngx_memcpy(v->data, val, v->len + 1);

    return NGX_OK;
}

static char *
ngx_http_requestid(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_variable_t *var;
    ngx_str_t *value, name;

    value = cf->args->elts;

    name = value[1];

    if (name.data[0] != '$') {
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                           "invalid variable name \"%V\"", &name);
        return NGX_CONF_ERROR;
    }

    name.len--;
    name.data++;

    var = ngx_http_add_variable(cf, &name, NGX_HTTP_VAR_CHANGEABLE);
    if (var == NULL) {
        return NGX_CONF_ERROR;
    }

    var->get_handler = ngx_http_requestid_set_variable;

    return NGX_OK;
}
