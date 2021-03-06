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


#ifndef CHB_RULE_ENGINE_H_
#define CHB_RULE_ENGINE_H_

#include "hbutils.h"
#include "chb_rule_xml_parser.h"
#include "chb_action_handler_proxy.h"

class chb_rule_engine: public ihb_rule_engine, public chb_obj {

    pthread_mutex_t m_mutex;

    ihb_msg_bus *m_mbus;
    ihb_data_bus *m_dbus;
    ihb_dev_manager *m_dman;
    chb_action_handler_proxy *m_ah;

    string m_id;

    anomaly_conditions m_device_state;
    chb_rule_xml_parser m_rule_xml_parser;

    void update_device_state(anomaly_device devstate);
    void handle_actions(anomaly_actions &actions);

public:
    chb_rule_engine(string &ahpath, ihb_data_bus *dbus, ihb_msg_bus *mbus,
            ihb_dev_manager *dman, hb_result &result);

    virtual ~chb_rule_engine();

    const string & get_tid();
    void on_msg(string &sid, uint64_t timestamp, hb_result result, string &msg);

    hb_result load_rules(string &rules);

};


#endif /* CHB_RULE_ENGINE_H_ */
