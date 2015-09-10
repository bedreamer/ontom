#include "stdafx.h"


int BOSS_initialize(struct pandoras_box *pb, struct pandora_client *pc);
int BOSS_working(struct pandoras_box *pb, struct pandora_client *pc);
int BOSS_pause(struct pandoras_box *pb, struct pandora_client *pc);
int BOSS_exit(struct pandoras_box *pb, struct pandora_client *pc);
int BOSS_destroy(struct pandoras_box *pb, struct pandora_client *pc);

int do_connect(void *);
int do_disconnect(void *);

int register_eventproc(int (*callback)(int), void *);
int do_read(struct pandoras_msg *, size_t, void *);
int do_write(const struct pandoras_msg *, void *);

int status(void *);


int BOSS_initialize(struct pandoras_box *pb, struct pandora_client *pc)
{
    return 0;
}

int BOSS_working(struct pandoras_box *pb, struct pandora_client *pc)
{
    return 0;
}

int BOSS_pause(struct pandoras_box *pb, struct pandora_client *pc)
{
    return 0;
}

int BOSS_exit(struct pandoras_box *pb, struct pandora_client *pc)
{
    return 0;
}

int BOSS_destroy(struct pandoras_box *pb, struct pandora_client *pc)
{
    return 0;
}
