
#ifndef DDEBUG
#define DDEBUG 0
#endif
#include "ddebug.h"

#include "ngx_http_lua_shlist.h"
#include "ngx_http_lua_util.h"
#include "ngx_http_lua_api.h"


static int ngx_http_lua_shlist_pop(lua_State *L);
// static int ngx_http_lua_shlist_front(lua_State *L);
static int ngx_http_lua_shlist_push(lua_State *L);
// static int ngx_http_lua_shlist_size(lua_State *L);
// static int ngx_http_lua_shlist_clear(lua_State *L);

ngx_int_t
ngx_http_lua_shlist_init_zone(ngx_shm_zone_t *shm_zone, void *data)
{
    ngx_http_lua_shlist_ctx_t     *octx = data;

    size_t                         len;
    ngx_http_lua_shlist_ctx_t     *ctx;
    ngx_http_lua_main_conf_t      *lmcf;

    ctx = shm_zone->data;

    if (octx) {
        ctx->sh     = octx->sh;
        ctx->shpool = octx->shpool;
        
        goto done;
    }

    ctx->shpool = (ngx_slab_pool_t *) shm_zone->shm.addr;

    if (shm_zone->shm.exists) {
        ctx->sh = ctx->shpool->data;

        goto done;
    }

    ctx->sh = ngx_slab_alloc(ctx->shpool, sizeof(ngx_http_lua_shlist_ctx_t));
    if (ctx->sh == NULL) {
        return NGX_ERROR;
    }

    ctx->shpool->data = ctx->sh;
    
    ngx_queue_init(&ctx->sh->queue);

    len = sizeof("in lua_shared_list zone \"\" ") + shm_zone->shm.name.len;

    ctx->shpool->log_ctx = ngx_slab_alloc(ctx->shpool, len);
    if (ctx->shpool->log_ctx == NULL) {
        return NGX_ERROR;
    }

    ngx_sprintf(ctx->shpool->log_ctx, " in lua_shared_dict zone \"%V\"%Z",
                &shm_zone->shm.name);

#if defined(nginx_version) && nginx_version >= 1005013
    ctx->shpool->log_nomem = 0;
#endif

done:

    dd("get lmcf");

    lmcf = ctx->main_conf;

    dd("lmcf->lua: %p", lmcf->lua);

    lmcf->shm_zones_of_list_inited++;

    if (lmcf->shm_zones_of_list_inited == lmcf->shm_zones_of_list->nelts
        && lmcf->init_handler)
    {
        if (lmcf->init_handler(ctx->log, lmcf, lmcf->lua) != NGX_OK) {
            /* an error happened */
            return NGX_ERROR;
        }
    }

    return NGX_OK;

}


void
ngx_http_lua_inject_shlist_api(ngx_http_lua_main_conf_t *lmcf, lua_State *L)
{
    ngx_http_lua_shlist_ctx_t    *ctx;
    ngx_uint_t                    i;
    ngx_shm_zone_t              **zone;

    if (lmcf->shm_zones_of_list != NULL) {
        lua_createtable(L, 0, lmcf->shm_zones_of_list->nelts);

        lua_createtable(L, 0, 3);

        lua_pushcfunction(L, ngx_http_lua_shlist_push);
        lua_setfield(L, -2, "push");

        // lua_pushcfunction(L, ngx_http_lua_shlist_size);
        // lua_setfield(L, -2, "size");

        // lua_pushcfunction(L, ngx_http_lua_shlist_clear);
        // lua_setfield(L, -2, "clear");

        lua_pushcfunction(L, ngx_http_lua_shlist_pop);
        lua_setfield(L, -2, "pop");

        lua_pushvalue(L, -1); /* shared mt */
        lua_setfield(L, -2, "__index"); /* __index = mt */

        zone = lmcf->shm_zones_of_list->elts;

        for (i = 0; i < lmcf->shm_zones_of_list->nelts; i++) {
            ctx = zone[i]->data;

            lua_pushlstring(L, (char* )ctx->name.data, ctx->name.len);

            lua_pushlightuserdata(L, zone[i]);
            lua_pushvalue(L, -3);
            lua_setmetatable(L, -2);
            lua_rawset(L, -4);
        }

        lua_pop(L, 1);
    } else {
        lua_newtable(L);
    }

    lua_setfield(L, -2, "list");
}

static
int ngx_http_lua_shlist_push(lua_State *L)
{
    ngx_http_lua_shlist_node_t  *node;
    ngx_shm_zone_t              *zone;
    ngx_str_t                    value;
    ngx_http_lua_shlist_ctx_t   *ctx;
    double                       num;
    int                          n;
    int                          value_type;
   
    n = lua_gettop(L);

    if (n != 2) {
        return luaL_error(L, "expecting 2 arguments, but seen %d", n);
    }

    zone = lua_touserdata(L, 1);
    if (zone == NULL) {
        return luaL_error(L, "bad \"zone\" argument");
    }

    ctx = zone->data;

    if (lua_isnil(L, 2)) {    // 不允许插入空值 
        lua_pushnil(L);
        lua_pushliteral(L, "nil value");
    }

    value_type = lua_type(L, 2);  // 第二个参数就是要入栈的

    switch (value_type) {
        case LUA_TSTRING:
            value.data = (u_char *) lua_tolstring(L, 2, &value.len);
            break;
        case LUA_TNUMBER:
            value.len = sizeof(double);
            num = lua_tonumber(L, 2);
            value.data = (u_char *) &num;
            break;
        case LUA_TNIL:
            if (1) {
                lua_pushnil(L);
                lua_pushliteral(L, "attempt to add nil to list");
                return 2;
            }
        default:
            lua_pushnil(L);
            lua_pushliteral(L, "bad value type");
            return 2;
    }

    ngx_shmtx_lock(&ctx->shpool->mutex);

    n = offsetof(ngx_http_lua_shlist_node_t, data)
        + value.len;

    node = ngx_slab_alloc_locked(ctx->shpool, n);

    if (node == NULL) {
        ngx_shmtx_unlock(&ctx->shpool->mutex);

        lua_pushboolean(L, 0);
        lua_pushliteral(L, "no memory");
        return 2;
    }
    
    node->value_type = (uint8_t) value_type;

    ngx_queue_insert_tail(&ctx->sh->queue, &node->queue);

    ngx_shmtx_unlock(&ctx->shpool->mutex);

    lua_pushboolean(L, 1); // push success flag
    lua_pushnil(L);  // no error
    return 2;
}

static
int ngx_http_lua_shlist_pop(lua_State *L)
{
    ngx_http_lua_shlist_node_t  *node;
    ngx_http_lua_shlist_ctx_t   *ctx;
    ngx_shm_zone_t              *zone;
    ngx_queue_t                 *head;
    ngx_str_t                    value;
    int                          value_type;
    double                       num;
    int                          n;

    n = lua_gettop(L);
    
    if (n != 1) {
        return luaL_error(L, "expecting only but one argument");
    }

    zone = lua_touserdata(L, 1);

    if (zone == NULL) {
        return luaL_error(L, "bad zone argument");
    }

    ctx = zone->data;

    ngx_shmtx_lock(&ctx->shpool->mutex);

    if (ngx_queue_empty(&ctx->sh->queue)) {
        ngx_shmtx_unlock(&ctx->shpool->mutex);
        lua_pushnil(L);
        return 1;
    }

    head = ngx_queue_head(&ctx->sh->queue);

    node = ngx_queue_data(head, ngx_http_lua_shlist_node_t, queue);

    value_type = node->value_type;

    switch(value_type) {
        case LUA_TSTRING:
            lua_pushlstring(L, (char *)value.data, value.len);
            break;
        case LUA_TNUMBER:
            if (value.len != sizeof(double)) {
                ngx_shmtx_unlock(&ctx->shpool->mutex);

                return luaL_error(L, "bad lua number value size found");
            }
            ngx_memcpy(&num, value.data, sizeof(double));
            lua_pushnumber(L, num);
            break;
        default:
            ngx_shmtx_unlock(&ctx->shpool->mutex);
            return luaL_error(L, "bad value type found");
    }
    ngx_shmtx_unlock(&ctx->shpool->mutex);

    return 1;
}

// static
// int ngx_http_lua_shlist_size(lua_State *L)
// {

// }

// static
// int ngx_http_lua_shlist_clear(lua_State *L)
// {

// }















