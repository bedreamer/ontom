#ifndef _EX_SO_INCLUDED_H_
#define _EX_SO_INCLUDED_H_

typedef enum {
    EXSO_OK,  //  正常
    EXSO_DEL, //  删除
    EXSO_PAUSE//  暂停
}EXSO_STATUS;

/*
 * 扩展SO描述结构
 */
struct exso_struct {
    // 文件路径
    char exso_path[256];
    // 模块名称
    char exso_name[32];

    // 模块初始化入口, 必须有
    // 入口名称为  exso_$(exso_name)_init
    int (*exso_init_entry)(void *);
    // 主循环调用函数入口, 可选
    // 入口名称为  exso_$(exso_name)_main_loop
    int (*exso_main_loop)(void *);
    // 模块退出函数入口, 必须有
    // 入口名称为 exso_$(exso_name)_exit
    int (*exso_exit_entry)(void *);
    // 临时调用接口，用户程序功能扩展
    int (*exso_ioctrl)(unsigned int cmd, void *iparam, void *oparam, void *_private);

    // {{
    void *exso_param;
    int (*exso_init_entry_copy)(void *);
    void *exso_handle;
    EXSO_STATUS exso_status;
    struct exso_struct *exso_next;
    // }}
};

/*
 * 加载.so扩展文件，并执行初始化操作
 */
struct exso_struct *exso_load(struct exso_struct **head, const char *modulename, const char *path, void *param);

/*
 * 退出.so扩展文件
 */
int exso_unload(struct exso_struct **head, struct exso_struct *exso);

/*
 * 执行扩展的主函数
 */
void exso_do_mainloop(struct exso_struct **head);

/*
 *  搜索指定模块
 */
struct exso_struct *exso_search(struct exso_struct **head, const char name);
#endif
