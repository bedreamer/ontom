#ifndef _UPLOAD_H_
#define _UPLOAD_H_

int upload_charge_status(struct pandoras_box *pb, struct pandora_client *pc, struct socket_session *ss);
int upload_charge_status_reset(struct pandoras_box *pb, struct pandora_client *pc, struct socket_session *ss);

#endif
