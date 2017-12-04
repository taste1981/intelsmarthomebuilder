/*
 * chb_data_analytic_proxy.h
 *
 *  Created on: Oct 18, 2016
 *      Author: taotao
 */

#ifndef CHB_ANALYTIC_PROXY_H_
#define CHB_ANALYTIC_PROXY_H_

#include "hbutils.h"
#include <vector>

class chb_slib_handle: public virtual ihb_obj {

    void *m_hlib;
    int m_refcnt;
    pthread_mutex_t m_refcnt_mutex;

public:
    chb_slib_handle(void *hlib);
    ~chb_slib_handle();

    void ref();
    void deref();
};

class chb_data_analytic_proxy: public ihb_data_analytic {

    int m_refcnt;
    pthread_mutex_t m_refcnt_mutex;

    chb_slib_handle *m_hlib;
    ihb_data_analytic *m_ana;

public:
    chb_data_analytic_proxy(ihb_data_analytic *ana, chb_slib_handle *hlib);
    virtual ~chb_data_analytic_proxy();

    void ref();
    void deref();

    void on_data(string &sid, uint64_t timestamp, hb_result result,
            hb_data_type type, ihb_obj *obj);

    void on_msg(string &sid, uint64_t timestamp, hb_result result, string &msg);

    const string & get_tid();

    hb_data_type get_data_type();
    hb_result start();
    hb_result stop();

    static hb_result create_data_analytics(ihb_data_bus * data_bus,
            ihb_msg_bus * msg_bus, string &lib,
            vector<ihb_data_analytic *> &ana_list);
};

#endif /* CHB_ANALYTIC_PROXY_H_ */
