
#ifndef _NGX_HTTP_LUA_SHLIST_H_INCLUDE_
#define _NGX_HTTP_LUA_SHLIST_H_INCLUDE_

#include "ngx_http_lua_common.h"

typedef struct {
    // u_char                       color;
    // u_char                       dummy;
    // u_short                      key_len;
    ngx_queue_t                  queue;  // ngx对链表的实现是在需要链表串起来的结点结构体中定义一个ngx_queue_t类型的变量 真TM聪明
    // uint64_t                  expires;
    uint8_t                      value_type;
    uint32_t                     value_len;
    // uint32_t                     user_flags;
    u_char                       data[1];
} ngx_http_lua_shlist_node_t;


typedef struct {
    ngx_queue_t                   queue;
} ngx_http_lua_shlist_shctx_t;


typedef struct {
    ngx_http_lua_shlist_shctx_t  *sh;
    ngx_slab_pool_t              *shpool;
    ngx_str_t                     name;   // 共享内存的名字 定义 lua_shared_list test 10M 那么 name = ‘test’
    ngx_http_lua_main_conf_t     *main_conf;  // 保存配置文件中main部分的配置
    ngx_log_t                    *log;
} ngx_http_lua_shlist_ctx_t;


ngx_int_t ngx_http_lua_shlist_init_zone(ngx_shm_zone_t *shm_zone, void *data);
void ngx_http_lua_shdict_rbtree_insert_value(ngx_rbtree_node_t *temp,
    ngx_rbtree_node_t *node, ngx_rbtree_node_t *sentinel);
void ngx_http_lua_inject_shlist_api(ngx_http_lua_main_conf_t *lmcf,
    lua_State *L);


#endif