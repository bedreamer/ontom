#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include "exso.h"

/*
 * 加载.so扩展文件
 */
struct exso_struct *exso_load(struct exso_struct **head, const char *modulename,
                              const char *path, void *param)
{
    struct exso_struct *exso = NULL;
    struct exso_struct copy;
    char entry[128] = {0};

    if ( head == NULL ) return NULL;
    if ( path == NULL ) return NULL;
    if ( modulename == NULL ) return NULL;

    void *so_handle = dlopen(path, RTLD_LAZY);
    if ( so_handle == NULL ) return NULL;

    sprintf(entry, "exso_%s_init", modulename);
    copy.exso_init_entry = (int (*)(void *))dlsym(so_handle, entry);
    if ( dlerror() ) {
        dlclose(so_handle);
        return NULL;
    }

    sprintf(entry, "exso_%s_main_loop", modulename);
    copy.exso_init_entry = (int (*)(void *))dlsym(so_handle, entry);
    if ( dlerror() ) {
        copy.exso_init_entry = NULL;
    }

    sprintf(entry, "exso_%s_exit", modulename);
    copy.exso_init_entry = (int (*)(void *))dlsym(so_handle, entry);
    if ( dlerror() ) {
        dlclose(so_handle);
        return NULL;
    }

    strncpy(copy.exso_path, path, 256);
    strncpy(copy.exso_name, modulename, 32);
    copy.exso_handle = so_handle;
    copy.exso_next = NULL;
    copy.exso_status = EXSO_OK;
    copy.exso_param = param;

    exso = (struct exso_struct *)malloc(sizeof(copy));
    if ( exso == NULL ) {
        // 内存分配失败
        dlclose(so_handle);
        return NULL;
    }
    memcpy(exso, &copy, sizeof(copy));

    if ( *head != NULL ) {
        exso->exso_next = *head;
    }
    *head = exso;

    return exso;
}

/*
 * 退出.so扩展文件
 */
int exso_unload(struct exso_struct **head, struct exso_struct *exso)
{
    return 0;
}

/*
 * 执行扩展的主函数
 */
void exso_do_mainloop(struct exso_struct **head)
{
    int ret;
    struct exso_struct *thiz;

    if ( head == NULL ) return;
    if ( *head == NULL ) return;

    thiz = *head;

    while ( thiz ) {
        if ( thiz->exso_init_entry ) {
            ret = thiz->exso_init_entry( thiz->exso_param );
            if ( ret != 0 ) {
                // 初始化失败
                thiz->exso_init_entry_copy = thiz->exso_init_entry;
                thiz->exso_init_entry = NULL;
                thiz->exso_status = EXSO_DEL;
            } else {
                thiz->exso_status = EXSO_OK;
            }
        } else if ( thiz->exso_main_loop &&
                    thiz->exso_status == EXSO_OK ) {
            thiz->exso_main_loop( thiz->exso_param );
        } else {};

        if ( thiz->exso_status == EXSO_DEL ) {
            if ( thiz->exso_exit_entry ) {
                // 模块将被卸载
                thiz->exso_exit_entry( thiz->exso_param );
                thiz->exso_exit_entry = NULL;
                thiz->exso_main_loop = NULL;
                thiz->exso_init_entry = NULL;

                if ( thiz == * head ) {
                    *head = thiz->exso_next;
                }
                // 删除没有完全实现
            }
        }

        thiz = thiz->exso_next;
    }
}
