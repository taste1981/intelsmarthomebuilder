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


#include "chb_rule_engine.h"

#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <iostream>
#include "ezxml.h"

chb_rule_engine::chb_rule_engine(string &ahpath, ihb_data_bus *dbus,
        ihb_msg_bus *mbus, ihb_dev_manager *dman, hb_result &result) {

    HB_LOG("begin\n");

    m_dbus = dbus;
    m_mbus = mbus;
    m_dman = dman;

    HB_SAFE_REF(dbus);
    HB_SAFE_REF(mbus);
    HB_SAFE_REF(dman);

    m_ah = new chb_action_handler_proxy(ahpath, dbus, mbus, dman, result);
    if (!m_ah) {
        result = hb_insufficient_memory;
        HB_ERROR("hb_insufficient_memory\n");
        return;
    }

    if (result != hb_ok) {
        HB_SAFE_DEREF(m_ah);
        HB_ERROR("failed to create chb_action_handler_proxy\n");
        return;
    }

    pthread_mutex_init(&m_mutex, NULL);
    m_id = HB_RULE_ENGINE;

    // TODO: initialize this value depends on status
    m_device_state = 0;
    vector<hb_dev_desc> descs;
    m_dman->get_dev_descs(descs);
    for (size_t i = 0; i < descs.size(); i++) {
        anomaly_device device;
        device.group = descs[i].group;
        device.name = descs[i].name;
        m_device_state.devices.push_back(device);
    }

    HB_LOG("end\n");
}

chb_rule_engine::~chb_rule_engine() {

    HB_LOG("begin\n");

    pthread_mutex_lock(&m_mutex);

    HB_SAFE_DEREF(m_dbus);
    HB_SAFE_DEREF(m_mbus);
    HB_SAFE_DEREF(m_dman);
    HB_SAFE_DEREF(m_ah);

    pthread_mutex_unlock(&m_mutex);
    pthread_mutex_destroy(&m_mutex);

    HB_LOG("end\n");
}

const string & chb_rule_engine::get_tid() {
    return m_id;
}

/*
 * video analytic message
 *
 *  msg attributes
 *  state = "person-detected" or "person-not-detected"
 *  status = "known" or "unknown"
 *
 *   <?xml version="1.0" ?>
 *   <home-builder-msg type="message">
 *       <msg-data state="person-detected" status="known" />
 *   </home-builder-msg>;
 *
 * audio analytic message
 *
 *   <?xml version="1.0" ?>
 *   <home-builder-msg type="message" >
 *       <msg-data state="glass-breaking-detected" />
 *   </home-builder-msg>;
 *
 * door sensor message
 *
 *  <?xml version="1.0" ?>
 *   <home-builder-msg type="message" >
 *       <msg-data state="opened" />
 *  </home-builder-msg>;
 *
 *
 */

/*
 * <?xml version=\"1.0\" ?>"
 " <home-builder-msg type=\"message\">"
 " <msg-data state=\"glass-breaking-detected\"/>"
 " </home-builder-msg>";
 *
 */

void chb_rule_engine::update_device_state(anomaly_device devstate) {

    for (size_t i = 0; i < m_device_state.devices.size(); i++) {
        if (m_device_state.devices[i].group == devstate.group
                && m_device_state.devices[i].name == devstate.name) {
            m_device_state.devices[i].state = devstate.state;
            m_device_state.devices[i].status = devstate.status;
            break;
        }
    }
}

void chb_rule_engine::on_msg(string &sid, uint64_t timestamp, hb_result result,
        string &msg) {

    HB_LOG("rule-engine got msg: %s\n", msg.c_str());

    char *_cmsg = new char[msg.length() + 1];
    if (!_cmsg) {
        return;
    }

    strcpy(_cmsg, msg.c_str());

    ezxml_t root = ezxml_parse_str(_cmsg, msg.length());
    if (!root) {
        HB_ERROR("!root\n");
        delete[] _cmsg;
        return;
    }

    ezxml_t home_builder_msg = root;
    if (strstr(home_builder_msg->name, "home-builder-msg") == 0) {
        HB_ERROR("not malan_msg, %s\n", home_builder_msg->name);
        ezxml_free(root);
        delete[] _cmsg;
        return;
    }

    const char *vattr = NULL;
    string mtype, stype;

    vattr = ezxml_attr(home_builder_msg, "type");
    if (!vattr) {
        mtype = "*";
    } else {
        mtype = vattr;
    }

    ezxml_t msg_data = ezxml_child(home_builder_msg, "msg-data");
    if (!msg_data) {
        HB_ERROR("msg-data not found\n");
        ezxml_free(root);
        delete[] _cmsg;
        return;
    }

    string state, status;
    vattr = ezxml_attr(msg_data, "state");
    if (!vattr) {
        state = "*";
    } else {
        state = vattr;
    }

    vattr = ezxml_attr(msg_data, "status");
    if (!vattr) {
        status = "*";
    } else {
        status = vattr;
    }
    ezxml_free(root);
    delete[] _cmsg;

    anomaly_device devstate;
    bool toupdate = false;

    if (mtype == "message") {

        cout << "state: " << state << " status: " << status << endl;
        toupdate = true;

    } else {

        cout << "unknown msg type type\n" << endl;
    }

    if (toupdate) {

        chb_auto_lock lock(&m_mutex);

        hb_dev_desc desc;

        cout << "to get_devdesc_byid" << endl;
        if (m_dman->get_dev_desc_byid(sid, desc) == hb_ok) {

            cout << "get_devdesc_byid called: group = " << desc.group
                    << " name = " << desc.name << endl;

            devstate.group = desc.group;
            devstate.name = desc.name;
            devstate.state = state;
            devstate.status = status;

            update_device_state(devstate);
        }
    }

    {
        chb_auto_lock lock(&m_mutex);
        anomaly_actions actions;
        if (m_rule_xml_parser.is_anomaly(m_device_state, actions)) {
            cout << "!!! anomaly detected !!!\n";

            handle_actions(actions);
        }
    }
}

/*
 <action play="alarm" message="door-open.wav"/>
 <action play="client-message" message="attention, door is opened, but there's no person detected"/>
 */

void chb_rule_engine::handle_actions(anomaly_actions &actions) {

    if (m_ah) {

        m_ah->handle_actions(actions);
    }
}

hb_result chb_rule_engine::load_rules(string &rules) {
    if (!m_rule_xml_parser.load_rule_xml(rules.c_str())) {
        HB_ERROR("failed to load rule xml file: %s", rules.c_str());
        return hb_fail;
    }

    return hb_ok;
}
