/*
 * chb_data_analytic_proxy.cpp
 *
 *  Created on: Oct 18, 2016
 *      Author: taotao
 */

#include <dlfcn.h>
#include "chb_analytic_proxy.h"

chb_slib_handle::chb_slib_handle(void *hlib) :
        m_hlib(hlib), m_refcnt(1) {

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m_refcnt_mutex, &attr);
}

chb_slib_handle::~chb_slib_handle() {
    HB_LOG("begin\n");
    if (m_hlib) {
        dlclose(m_hlib);
    }
    pthread_mutex_destroy(&m_refcnt_mutex);
    HB_LOG("end\n");
}

void chb_slib_handle::ref() {
    chb_auto_lock lock(&m_refcnt_mutex);
    m_refcnt++;
}

void chb_slib_handle::deref() {
    chb_auto_lock lock(&m_refcnt_mutex);
    if (--m_refcnt == 0) {
        delete this;
    }
}

chb_data_analytic_proxy::chb_data_analytic_proxy(ihb_data_analytic *ana,
        chb_slib_handle *hlib) :
        m_refcnt(1), m_ana(ana), m_hlib(hlib) {
    ana->ref();
    hlib->ref();

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m_refcnt_mutex, &attr);
}

chb_data_analytic_proxy::~chb_data_analytic_proxy() {
}

void chb_data_analytic_proxy::ref() {
    chb_auto_lock lock(&m_refcnt_mutex);

    HB_LOG("begin\n");
    m_ana->ref();
    HB_LOG("ana is refed\n");
    m_hlib->ref();
    HB_LOG("lib is refed\n");
    m_refcnt++;
    HB_LOG("end\n");
}

void chb_data_analytic_proxy::deref() {
    chb_auto_lock lock(&m_refcnt_mutex);
    m_ana->deref();
    m_hlib->deref();
    if (--m_refcnt == 0) {
        delete this;
    }
}

const string & chb_data_analytic_proxy::get_tid() {
    return ((ihb_msg_cb *) m_ana)->get_tid();
}

hb_data_type chb_data_analytic_proxy::get_data_type() {
    return m_ana->get_data_type();
}

hb_result chb_data_analytic_proxy::stop() {
    return m_ana->stop();
}

hb_result chb_data_analytic_proxy::start() {

    return m_ana->start();
}

void chb_data_analytic_proxy::on_data(string &sid, uint64_t timestamp,
        hb_result result, hb_data_type type, ihb_obj *obj) {
    m_ana->on_data(sid, timestamp, result, type, obj);
}

void chb_data_analytic_proxy::on_msg(string &sid, uint64_t timestamp,
        hb_result result, string &msg) {
    m_ana->on_msg(sid, timestamp, result, msg);
}

hb_result chb_data_analytic_proxy::create_data_analytics(
        ihb_data_bus * data_bus, ihb_msg_bus * msg_bus, string &lib,
        vector<ihb_data_analytic *> &ana_list) {

    HB_LOG("begin: %s\n", lib.c_str());

    int count = 0;
    hb_result ret = hb_ok;
    hb_get_analytic_count_proc get_count_proc = NULL;
    hb_create_analytic_instance_proc create_analytic_proc = NULL;

    void *hlib = dlopen(lib.c_str(), RTLD_LAZY);
    if (!hlib) {
        HB_ERROR("%s\n", dlerror());
        return hb_fail;
    }

    HB_LOG("hlib = 0x%x\n", hlib);

    chb_slib_handle *iavshlib = new chb_slib_handle(hlib);
    if (!iavshlib) {
        HB_ERROR("hb_insufficient_memory\n");
        dlclose(hlib);
        return hb_insufficient_memory;
    }

    get_count_proc = (hb_get_analytic_count_proc) dlsym(hlib,
            "hb_get_analytic_count");
    if (!get_count_proc) {
        HB_ERROR("failed to get hb_get_analytic_count_proc\n");
        ret = hb_fail;
        goto cleanup;
    }

    create_analytic_proc = (hb_create_analytic_instance_proc) dlsym(hlib,
            "hb_create_analytic_instance");
    if (!create_analytic_proc) {
        HB_ERROR("failed to get hb_create_analytic_instance_proc\n");
        ret = hb_fail;
        goto cleanup;
    }

    ana_list.clear();
    count = get_count_proc();
    for (int i = 0; i < count; i++) {
        ihb_data_analytic* ana = NULL;
        ret = create_analytic_proc(i, data_bus, msg_bus, &ana);
        if (ret == hb_ok) {
            ihb_data_analytic *proxy = new chb_data_analytic_proxy(ana,
                    iavshlib);
            if (!proxy) {
                HB_SAFE_DEREF(ana);
                ret = hb_insufficient_memory;
                HB_ERROR("hb_insufficient_memory\n");
                goto cleanup;
            }
            ana_list.push_back(proxy);
        }
    }

    ret = hb_ok;

    cleanup:

    if (ret != hb_ok) {
        for (size_t i = 0; i < ana_list.size(); i++) {
            HB_SAFE_DEREF(ana_list[i]);
        }
        ana_list.clear();
    }
    HB_SAFE_DEREF(iavshlib);
    HB_LOG("end\n");
    return ret;
}
