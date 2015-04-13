#ifndef LIST_H
#define LIST_H
struct list_head{
struct list_head* next,*pre;
};

#define list_ini(lst) (lst).next=(lst).pre=&(lst);
#define list_load(loadtype,name,nameptr)\
    (loadtype*)((unsigned char*)nameptr-(unsigned char*)(&(((loadtype*)0)->name)))
#define member_load list_load

/*添加至链表头之后*/
static inline struct list_head* list_insert(struct list_head *entry,struct list_head *new_){
    struct list_head *next=entry->next;
    entry->next=new_->pre;
    next->pre = new_->next;
    new_->pre = entry;
    new_->next=next;
    return new_;
}

/*添加至链表末尾*/
static inline struct list_head* list_inserttail(struct list_head *entry,struct list_head *new_){
    struct list_head *pre=entry->pre;
    pre->next=new_;
    new_->pre=pre;
    new_->next=entry;
    entry->pre=new_;
    return new_;
}

/*删除入口*/
static inline struct list_head* list_remove(struct list_head *entry){
    struct list_head *p = entry->next;
    if (p==entry)
        return entry;
    else{
        p->pre = entry->pre;
        entry->pre->next = p;
        list_ini(*entry);
        return entry;
    }
}
#endif // LIST_H
