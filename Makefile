.PHONY: all install clean

all:
	cd /Users/aifei/workspace/ngx_iresty/build/LuaJIT-2.1-20150622 && $(MAKE) TARGET_STRIP=@: CCDEBUG=-g CC=cc PREFIX=/opt/iresty/luajit
	cd /Users/aifei/workspace/ngx_iresty/build/lua-cjson-2.1.0.2 && $(MAKE) DESTDIR=$(DESTDIR) LUA_INCLUDE_DIR=/Users/aifei/workspace/ngx_iresty/build/luajit-root/opt/iresty/luajit/include/luajit-2.1 LUA_CMODULE_DIR=/opt/iresty/lualib LUA_MODULE_DIR=/opt/iresty/lualib CJSON_CFLAGS="-g -fpic" CJSON_LDFLAGS='-bundle -undefined dynamic_lookup' CC=cc
	cd /Users/aifei/workspace/ngx_iresty/build/lua-redis-parser-0.10 && $(MAKE) DESTDIR=$(DESTDIR) LUA_INCLUDE_DIR=/Users/aifei/workspace/ngx_iresty/build/luajit-root/opt/iresty/luajit/include/luajit-2.1 LUA_LIB_DIR=/opt/iresty/lualib LDFLAGS='-bundle -undefined dynamic_lookup' CC=cc
	cd /Users/aifei/workspace/ngx_iresty/build/lua-rds-parser-0.05 && $(MAKE) DESTDIR=$(DESTDIR) LUA_INCLUDE_DIR=/Users/aifei/workspace/ngx_iresty/build/luajit-root/opt/iresty/luajit/include/luajit-2.1 LUA_LIB_DIR=/opt/iresty/lualib LDFLAGS='-bundle -undefined dynamic_lookup' CC=cc
	cd /Users/aifei/workspace/ngx_iresty/build/nginx-1.7.10 && $(MAKE)

install: all
	cd /Users/aifei/workspace/ngx_iresty/build/LuaJIT-2.1-20150622 && $(MAKE) install TARGET_STRIP=@: CCDEBUG=-g CC=cc PREFIX=/opt/iresty/luajit DESTDIR=$(DESTDIR)
	cd /Users/aifei/workspace/ngx_iresty/build/lua-cjson-2.1.0.2 && $(MAKE) install DESTDIR=$(DESTDIR) LUA_INCLUDE_DIR=/Users/aifei/workspace/ngx_iresty/build/luajit-root/opt/iresty/luajit/include/luajit-2.1 LUA_CMODULE_DIR=/opt/iresty/lualib LUA_MODULE_DIR=/opt/iresty/lualib CJSON_CFLAGS="-g -fpic" CJSON_LDFLAGS='-bundle -undefined dynamic_lookup' CC=cc
	cd /Users/aifei/workspace/ngx_iresty/build/lua-redis-parser-0.10 && $(MAKE) install DESTDIR=$(DESTDIR) LUA_INCLUDE_DIR=/Users/aifei/workspace/ngx_iresty/build/luajit-root/opt/iresty/luajit/include/luajit-2.1 LUA_LIB_DIR=/opt/iresty/lualib LDFLAGS='-bundle -undefined dynamic_lookup' CC=cc
	cd /Users/aifei/workspace/ngx_iresty/build/lua-rds-parser-0.05 && $(MAKE) install DESTDIR=$(DESTDIR) LUA_INCLUDE_DIR=/Users/aifei/workspace/ngx_iresty/build/luajit-root/opt/iresty/luajit/include/luajit-2.1 LUA_LIB_DIR=/opt/iresty/lualib LDFLAGS='-bundle -undefined dynamic_lookup' CC=cc
	cd /Users/aifei/workspace/ngx_iresty/build/lua-resty-dns-0.14 && $(MAKE) install DESTDIR=$(DESTDIR) LUA_LIB_DIR=/opt/iresty/lualib INSTALL=/Users/aifei/workspace/ngx_iresty/build/install
	cd /Users/aifei/workspace/ngx_iresty/build/lua-resty-memcached-0.13 && $(MAKE) install DESTDIR=$(DESTDIR) LUA_LIB_DIR=/opt/iresty/lualib INSTALL=/Users/aifei/workspace/ngx_iresty/build/install
	cd /Users/aifei/workspace/ngx_iresty/build/lua-resty-redis-0.20 && $(MAKE) install DESTDIR=$(DESTDIR) LUA_LIB_DIR=/opt/iresty/lualib INSTALL=/Users/aifei/workspace/ngx_iresty/build/install
	cd /Users/aifei/workspace/ngx_iresty/build/lua-resty-mysql-0.15 && $(MAKE) install DESTDIR=$(DESTDIR) LUA_LIB_DIR=/opt/iresty/lualib INSTALL=/Users/aifei/workspace/ngx_iresty/build/install
	cd /Users/aifei/workspace/ngx_iresty/build/lua-resty-string-0.09 && $(MAKE) install DESTDIR=$(DESTDIR) LUA_LIB_DIR=/opt/iresty/lualib INSTALL=/Users/aifei/workspace/ngx_iresty/build/install
	cd /Users/aifei/workspace/ngx_iresty/build/lua-resty-upload-0.09 && $(MAKE) install DESTDIR=$(DESTDIR) LUA_LIB_DIR=/opt/iresty/lualib INSTALL=/Users/aifei/workspace/ngx_iresty/build/install
	cd /Users/aifei/workspace/ngx_iresty/build/lua-resty-websocket-0.05 && $(MAKE) install DESTDIR=$(DESTDIR) LUA_LIB_DIR=/opt/iresty/lualib INSTALL=/Users/aifei/workspace/ngx_iresty/build/install
	cd /Users/aifei/workspace/ngx_iresty/build/lua-resty-lock-0.04 && $(MAKE) install DESTDIR=$(DESTDIR) LUA_LIB_DIR=/opt/iresty/lualib INSTALL=/Users/aifei/workspace/ngx_iresty/build/install
	cd /Users/aifei/workspace/ngx_iresty/build/lua-resty-lrucache-0.04 && $(MAKE) install DESTDIR=$(DESTDIR) LUA_LIB_DIR=/opt/iresty/lualib INSTALL=/Users/aifei/workspace/ngx_iresty/build/install
	cd /Users/aifei/workspace/ngx_iresty/build/lua-resty-core-0.1.1 && $(MAKE) install DESTDIR=$(DESTDIR) LUA_LIB_DIR=/opt/iresty/lualib INSTALL=/Users/aifei/workspace/ngx_iresty/build/install
	cd /Users/aifei/workspace/ngx_iresty/build/lua-resty-upstream-healthcheck-0.03 && $(MAKE) install DESTDIR=$(DESTDIR) LUA_LIB_DIR=/opt/iresty/lualib INSTALL=/Users/aifei/workspace/ngx_iresty/build/install
	cd /Users/aifei/workspace/ngx_iresty/build/resty-cli-0.03 && /Users/aifei/workspace/ngx_iresty/build/install resty $(DESTDIR)//opt/iresty/bin/
	cd /Users/aifei/workspace/ngx_iresty/build/nginx-1.7.10 && $(MAKE) install DESTDIR=$(DESTDIR)

clean:
	rm -rf build
