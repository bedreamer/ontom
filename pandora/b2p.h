#ifnde __Z2B_H_
#define __Z2B_H_

// z2b 消息头
struct zb_message_head {
    // time stamp
    time_t tstp;
    // 消息序号, 发送和接收对应两个序号
};

// 对外连接服务器对象
struct pserver {
};


#endif /*__Z2B_H_*/
