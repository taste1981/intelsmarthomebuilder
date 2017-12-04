#ifndef CHB_DATA_BUS_H_
#define CHB_DATA_BUS_H_

#include "hbutils.h"
#include "chb_queue.h"

#include <vector>
#include <map>

class chb_data_bus: public ihb_data_bus, public chb_obj {
protected:

    pthread_mutex_t m_mutex;
    map<string, ihb_data_sender *> m_sender_list;

public:
    chb_data_bus();
    virtual ~chb_data_bus();

    hb_result register_sender(string &sid, ihb_data_sender** snd);
    hb_result unregister_sender(string &sid);

    hb_result register_listener(string &sid, ihb_data_cb* cb);
    hb_result unregister_listener(string &sid, ihb_data_cb* cb);
};

#endif /* CHB_DATA_BUS_H_ */
