/*
 * Copyright © 2017 Intel Corporation. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "chb_device_proxy.h"
#include <dlfcn.h>

chb_device_proxy::chb_device_proxy(string &lib, hb_dev_type devtype,
        const char *uri, ihb_data_bus* dbus, ihb_msg_bus * mbus, void *option,
        hb_result &result) :
        m_hlib(NULL), m_dev(NULL) {

    HB_LOG("begin: %s\n", lib.c_str());

    hb_create_device_instance_proc create_device = NULL;
    ihb_dev *dev = NULL;

    void *hlib = dlopen(lib.c_str(), RTLD_LAZY);
    if (!hlib) {
        HB_ERROR("%s\n", dlerror());
        result = hb_fail;
        return;
    }

    create_device = (hb_create_device_instance_proc) dlsym(hlib,
            "hb_create_device_instance");
    if (!create_device) {
        HB_ERROR("lib = %s, uri = %s\n", lib.c_str(), uri);
        HB_ERROR("failed to get hb_create_device_instance_proc\n");
        result = hb_fail;
        goto cleanup;
    }

    result = create_device(dbus, mbus, devtype, uri, option, &dev);
    if (result != hb_ok || !dev) {
        HB_ERROR("failed to create device\n");
        goto cleanup;
    }

    m_dev = dev;
    m_hlib = hlib;
    result = hb_ok;

    cleanup:

    if (result != hb_ok) {
        dlclose(hlib);
    }

    HB_LOG("end\n");
}

chb_device_proxy::~chb_device_proxy() {

    HB_LOG("begin\n");

    HB_SAFE_DEREF(m_dev);
    if (m_hlib) {
        dlclose(m_hlib);
    }
    HB_LOG("end\n");
}

void chb_device_proxy::on_data(string &sid, uint64_t timestamp,
        hb_result result, hb_data_type type, ihb_obj *obj) {

    if (!m_dev) {
        return;
    }

    ihb_data_cb *cb = dynamic_cast<ihb_data_cb *>(m_dev);
    if (cb) {
        cb->on_data(sid, timestamp, result, type, obj);
    }
}

const string & chb_device_proxy::get_tid() {

    static string ret("not_supported");
    ihb_data_cb *cb = dynamic_cast<ihb_data_cb *>(m_dev);
    if (cb) {
        return cb->get_tid();
    }

    return ret;
}

hb_result chb_device_proxy::get_attr_descs(string &attributes) {

    if (!m_dev) {
        HB_ERROR("!m_dev\n");
        return hb_fail;
    }

    return m_dev->get_attr_descs(attributes);
}

hb_result chb_device_proxy::get_attr_value(string &name, string &value) {

    if (!m_dev) {
        HB_ERROR("!m_dev\n");
        return hb_fail;
    }

    return m_dev->get_attr_value(name, value);
}

hb_result chb_device_proxy::set_attr_value(string &name, string &value) {

    if (!m_dev) {
        HB_ERROR("!m_dev\n");
        return hb_fail;
    }

    return m_dev->set_attr_value(name, value);
}

hb_result chb_device_proxy::get_dev_desc(hb_dev_desc *desc) {

    if (!m_dev) {
        HB_ERROR("!m_dev\n");
        return hb_fail;
    }

    return m_dev->get_dev_desc(desc);

}

hb_result chb_device_proxy::get_dev_status(hb_dev_status &status) {

    if (!m_dev) {
        HB_ERROR("!m_dev\n");
        return hb_fail;
    }

    return m_dev->get_dev_status(status);
}

hb_result chb_device_proxy::set_dev_group(string &group) {
    if (!m_dev) {
        HB_ERROR("!m_dev\n");
        return hb_fail;
    }
    return m_dev->set_dev_group(group);
}

hb_result chb_device_proxy::set_dev_name(string &name) {
    if (!m_dev) {
        HB_ERROR("!m_dev\n");
        return hb_fail;
    }
    return m_dev->set_dev_name(name);
}

hb_result chb_device_proxy::start() {
    if (!m_dev) {
        HB_ERROR("!m_dev\n");
        return hb_fail;
    }

    return m_dev->start();
}

hb_result chb_device_proxy::stop() {
    if (!m_dev) {
        HB_ERROR("!m_dev\n");
        return hb_fail;
    }

    return m_dev->stop();
}

