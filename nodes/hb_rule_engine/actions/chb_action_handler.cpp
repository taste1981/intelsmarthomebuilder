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


#include "hb_rule_def.h"
#include "hb_action_handler.h"
#include "chb_action_handler.h"

hb_result hb_create_action_handler_instance(ihb_data_bus *dbus, ihb_msg_bus *mbus,
        ihb_dev_manager *dman, ihb_action_handler **obj) {

    HB_LOG("begin\n");

    if (!obj) {
        HB_ERROR("hb_invalid_parameter\n");
        return hb_invalid_parameter;
    }

    hb_result ret = hb_fail;

    chb_action_handler* ah = new chb_action_handler(dbus, mbus, dman, ret);
    if (!ah) {
        HB_ERROR("hb_insufficient_memory\n");
        return hb_insufficient_memory;
    }

    if (ret != hb_ok) {
        delete ah;
    }

    *obj = ah;

    HB_LOG("end\n");

    return ret;

}

chb_action_handler::chb_action_handler(ihb_data_bus *dbus, ihb_msg_bus *mbus,
        ihb_dev_manager *dman, hb_result &result) {

    HB_LOG("begin\n");

    m_dbus = dbus;
    m_mbus = mbus;
    m_dman = dman;

    HB_SAFE_REF(dbus);
    HB_SAFE_REF(mbus);
    HB_SAFE_REF(dman);

    result = hb_ok;

    HB_LOG("end\n");
}

chb_action_handler::~chb_action_handler() {
    HB_LOG("begin\n");

    HB_SAFE_DEREF(m_dbus);
    HB_SAFE_DEREF(m_mbus);
    HB_SAFE_DEREF(m_dman);

    HB_LOG("end\n");
}

void chb_action_handler::handle_actions(anomaly_actions &actions) {

    HB_LOG("begin\n");

    for (size_t i = 0; i < actions.actions.size(); i++) {
        if (actions.actions[i].play == "alarm") {
            cout << actions.actions[i].msg << endl;
        } else if (actions.actions[i].play == "stop-alarm") {
            cout << actions.actions[i].msg << endl;
        } else if (actions.actions[i].play == "client-message") {
            cout << actions.actions[i].msg << endl;
        } else if (actions.actions[i].play.find("email=") != string::npos) {
            cout << actions.actions[i].msg << endl;
        }
    }

    HB_LOG("end\n");
}
