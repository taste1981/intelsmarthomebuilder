#ifndef CHB_DEVANA_MAN_H_
#define CHB_DEVANA_MAN_H_

#include "hbutils.h"
#include <map>

struct hb_dev_anas {
    ihb_dev* dev;
    vector<ihb_data_analytic *> anas;
};

class chb_devana_manager: public ihb_dev_manager, public chb_obj {

    pthread_mutex_t m_mutex;

    map<string, hb_dev_anas> m_devlist;

    ihb_data_bus *m_dbus;
    ihb_msg_bus *m_mbus;

    void clean_devanas();

public:
    chb_devana_manager(ihb_data_bus *dbus, ihb_msg_bus *mbus);
    virtual ~chb_devana_manager();

    hb_result load_config(const char* cfgxml);

    hb_result load_config(string &cfg);
    hb_result get_dev_descs(vector<hb_dev_desc> &descs);
    hb_result get_dev_byid(string &devid, ihb_dev** dev);
    hb_result get_dev_desc_byid(string &devid, hb_dev_desc &desc);

};

#endif /* CHB_DEVANA_MAN_H_ */
