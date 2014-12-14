#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct {
	ngx_flag_t    enable;
} ngx_http_requestid_conf_t;


static ngx_int_t ngx_http_requestid_add_variables(ngx_conf_t *cf);
static void *ngx_http_requestid_create_conf(ngx_conf_t *cf);
static char *ngx_http_requestid_merge_conf(ngx_conf_t *cf, void *parent,
    void *child);

static ngx_command_t  ngx_http_requestid_commands[] = {
	 { ngx_string("requestid"),
	  NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
	  ngx_conf_set_flag_slot,
	  NGX_HTTP_LOC_CONF_OFFSET,
	  offsetof(ngx_http_requestid_conf_t, enable),
	  NULL },

      ngx_null_command
};


static ngx_http_module_t  ngx_http_requestid_module_ctx = {
	ngx_http_requestid_add_variables,      /* preconfiguration */
    NULL,                  				   /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    ngx_http_requestid_create_conf,           /* create location configration */
    ngx_http_requestid_merge_conf             /* merge location configration */
};


ngx_module_t  ngx_http_requestid_module = {
    NGX_MODULE_V1,
    &ngx_http_requestid_module_ctx, /* module context */
    ngx_http_requestid_commands,           /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,           					   /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};

static ngx_str_t  ngx_http_requestid = ngx_string("request_id");


static ngx_int_t
ngx_http_requestid_set_variable(ngx_http_request_t *r,
     ngx_http_variable_value_t *v, uintptr_t data)
{
    ngx_http_requestid_conf_t      *conf;
    u_char       				   *end;
	u_char 						   val[NGX_INT32_LEN + 1];

    conf = ngx_http_get_module_loc_conf(r, ngx_http_requestid_module);

    if (conf->enable == 0) {
    	v->not_found = 1;
    	return NGX_OK;
    }

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

static void *
ngx_http_requestid_create_conf(ngx_conf_t *cf)
{
    ngx_http_requestid_conf_t  *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_requestid_conf_t));
    if (conf == NULL) {
        return NGX_CONF_ERROR;
    }

    conf->enable = NGX_CONF_UNSET;
    return conf;
}


static char *
ngx_http_requestid_merge_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_requestid_conf_t *prev = parent;
    ngx_http_requestid_conf_t *conf = child;

    ngx_conf_merge_value(conf->enable, prev->enable, 0);

    return NGX_CONF_OK;
}

static ngx_int_t
ngx_http_requestid_add_variables(ngx_conf_t *cf)
{
    ngx_http_variable_t  *var;

    var = ngx_http_add_variable(cf, &ngx_http_requestid, NGX_HTTP_VAR_CHANGEABLE);
    if (var == NULL) {
        return NGX_ERROR;
    }

    var->get_handler = ngx_http_requestid_set_variable;

    return NGX_OK;
}
