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


#include "chb_rule_engine_proxy.h"
#include <dlfcn.h>

chb_rule_engine_proxy::chb_rule_engine_proxy(string &lib, string &ahlib, ihb_data_bus* dbus,
        ihb_msg_bus * mbus, ihb_dev_manager *devman, hb_result &result) :
        m_hlib(NULL), m_rule_engine(NULL) {

    HB_LOG("begin: %s\n", lib.c_str());

    hb_create_rule_engine_instance_proc create_rule_engine = NULL;
    ihb_rule_engine *rule_engine = NULL;

    void *hlib = dlopen(lib.c_str(), RTLD_LAZY);
    if (!hlib) {
        HB_ERROR("%s\n", dlerror());
        result = hb_fail;
        return;
    }

    create_rule_engine = (hb_create_rule_engine_instance_proc) dlsym(hlib,
            "hb_create_rule_engine_instance");
    if (!create_rule_engine) {
        HB_ERROR("lib = %s\n", lib.c_str());
        HB_ERROR("failed to get hb_create_rule_engine_instance\n");
        result = hb_fail;
        goto cleanup;
    }

    result = create_rule_engine(ahlib, dbus, mbus, devman, &rule_engine);
    if (result != hb_ok || !rule_engine) {
        HB_ERROR("failed to create rule engine\n");
        goto cleanup;
    }

    m_rule_engine = rule_engine;
    m_hlib = hlib;
    result = hb_ok;

    cleanup:

    if (result != hb_ok) {
        dlclose(hlib);
    }

    HB_LOG("end\n");

}

chb_rule_engine_proxy::~chb_rule_engine_proxy() {
    HB_LOG("begin\n");

    HB_SAFE_DEREF(m_rule_engine);
    if (m_hlib) {
        dlclose(m_hlib);
    }
    HB_LOG("end\n");
}

const string & chb_rule_engine_proxy::get_tid() {
    static string ret("not_supported");

    ihb_msg_cb *cb = dynamic_cast<ihb_msg_cb *>(m_rule_engine);
    if (cb) {
        return cb->get_tid();
    }

    return ret;
}

void chb_rule_engine_proxy::on_msg(string &sid, uint64_t timestamp,
        hb_result result, string &msg) {

    if (!m_rule_engine) {
        return;
    }

    ihb_msg_cb *cb = dynamic_cast<ihb_msg_cb *>(m_rule_engine);
    if (cb) {
        cb->on_msg(sid, timestamp, result, msg);
    }
}

hb_result chb_rule_engine_proxy::load_rules(string &rules) {

    if (!m_rule_engine) {
        HB_ERROR("!m_rule_engine\n");
        return hb_fail;
    }

    return m_rule_engine->load_rules(rules);

}

