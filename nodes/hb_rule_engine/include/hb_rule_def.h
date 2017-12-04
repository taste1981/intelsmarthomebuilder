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


#ifndef HB_RULE_DEF_H_
#define HB_RULE_DEF_H_

#include "hbutils.h"
#include <iostream>

struct anomaly_device {
    string group;
    string name;
    string state;
    string status;

    anomaly_device() {
        group = "*";
        name = "*";
        state = "*";
        status = "*";
    }
    ~anomaly_device() {
    }

    inline bool operator==(const anomaly_device& r) {
        if (group == "*" || group == r.group) {
            if (name == "*" || name == r.name) {
                if (state == "*" || state == r.state) {
                    if (status == "*" || status == r.status) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    anomaly_device& operator=(int r) {
        group = "*";
        name = "*";
        state = "*";
        status = "*";
        return *this;
    }
};

struct anomaly_conditions {
    vector<anomaly_device> devices;
    inline bool operator==(const anomaly_conditions& r) {

        size_t true_count = 0;
        for (size_t i = 0; i < devices.size(); i++) {
            for (size_t j = 0; j < r.devices.size(); j++) {
                if (devices[i] == r.devices[j]) {
                    true_count++;
                    break;
                }
            }
        }

        if (true_count != devices.size()) {
            return false;
        }
        return true;
    }

    anomaly_conditions& operator=(int r) {
        for (size_t i = 0; i < devices.size(); i++) {
            devices[i] = 0;
        }
        return *this;
    }
};

struct anomaly_action {
    string play;
    string msg;
    string repeat;
};

struct anomaly_actions {
    string level;
    string wait;
    vector<anomaly_action> actions;

    anomaly_actions& operator=(anomaly_actions r) {
        level = r.level;
        wait = r.wait;
        actions = r.actions;
        return *this;
    }
};

struct anomaly_rule {
    anomaly_conditions conditions;
    anomaly_actions actions;
};

struct anomaly_rule_set {
    string name;
    vector<anomaly_rule> rules;
};

class ihb_action_handler: public virtual ihb_obj {
public:
    virtual ~ihb_action_handler() {
    }

    virtual void handle_actions(anomaly_actions &actions) = 0;
};

typedef hb_result (*hb_create_action_handler_instance_proc)(ihb_data_bus *dbus,
        ihb_msg_bus *mbus, ihb_dev_manager *dman, ihb_action_handler **obj);


#endif /* HB_RULE_DEF_H_ */
