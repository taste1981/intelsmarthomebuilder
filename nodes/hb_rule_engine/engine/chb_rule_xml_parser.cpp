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


#include "hbutils.h"
#include "chb_rule_xml_parser.h"
#include "ezxml.h"

/*
<?xml version="1.0" encoding="utf-8" standalone="yes"?>
<rule-set name="armed">
    <rule>
        <conditions>
            <device group="living-room" name="indoor-microphone" state="glass-breaking-detected" />
        </conditions>
        <actions level="red-alert" wait="10000">
            <action play="alarm" message="alert.wav" repeat="10"/>
            <action play="client-message" message="alert! glass breaking sound detected"/>
            <action play="email=tao.q.tao@intel.com;nusrat.habib@intel.com;rowena.almeida@intel.com;anitha.suryanarayan@intel.com;" message="alert! glass breaking sound detected" />
        </actions>
    </rule>
    <rule>
        <conditions>
            <device group="front-door" name="front-door-sensor" state="opened" />
            <device group="front-door" name="front-door-camera" state="person-detected" status="known"/>
        </conditions>
        <actions level="none">
            <action play="stop-alarm" message="nothing"/>
        </actions>
    </rule>
    <rule>
        <conditions>
            <device group="front-door" name="front-door-sensor"  state="opened" />
            <device group="front-door" name="front-door-camera" state="person-detected" status="unknown" />
        </conditions>
        <actions level="red-alert">
            <action play="alarm" message="alert.wav" repeat="10"/>
            <action play="client-message" message="intruder! your door is opened, stranger detected!"/>
            <action play="email=tao.q.tao@intel.com;nusrat.habib@intel.com;rowena.almeida@intel.com;anitha.suryanarayan@intel.com;" message="intruder! your door is opened, stranger detected!" />
        </actions>
    </rule>
    <rule>
        <conditions>
            <device group="front-door" name="front-door-sensor" state="opened" />
            <device group="front-door" name="front-door-camera" state="person-not-detected" />
        </conditions>
        <actions level="notification">
            <action play="alarm" message="door-open.wav" repeat="10"/>
            <action play="client-message" message="attention, door is opened, but there's no person detected"/>
            <action play="email=tao.q.tao@intel.com;nusrat.habib@intel.com;rowena.almeida@intel.com;anitha.suryanarayan@intel.com;" message="attention, door is opened, but there's no person detected" />
        </actions>
    </rule>
    <rule>
        <conditions>
            <device group="front-door" name="front-door-sensor" state="not-opened" />
            <device group="front-door" name="front-door-camera" state="person-detected" status="known"/>
        </conditions>
        <actions level="notification">
            <action play="alarm" message="how-did-you-come-in.wav" repeat="10"/>
            <action play="client-message" message="it is a magic!"/>
            <action play="email=tao.q.tao@intel.com;nusrat.habib@intel.com;rowena.almeida@intel.com;anitha.suryanarayan@intel.com;" message="it is a magic!" />
        </actions>
    </rule>
    <rule>
        <conditions>
            <device group="front-door" name="front-door-sensor" state="not-opened" />
            <device group="front-door" name="front-door-camera" state="person-detected" status="unknown" />
        </conditions>
        <actions level="alert">
            <action play="alarm" message="alert.wav" repeat="10"/>
            <action play="client-message" message="intruder! satan claus is coming to home, from chimney?!"/>
            <action play="email=tao.q.tao@intel.com;nusrat.habib@intel.com;rowena.almeida@intel.com;anitha.suryanarayan@intel.com;" message="intruder! satan claus is coming to home, from chimney?!" />
        </actions>
    </rule>
</rule-set>

 */

void clear_rules(vector<anomaly_rule> &rules) {
    for (size_t i = 0; i < rules.size(); i++) {
        rules[i].conditions.devices.clear();
        rules[i].actions.actions.clear();
    }
    rules.clear();
}

void clear_rule_set(vector<anomaly_rule_set> &rule_sets) {
    for (size_t i = 0; i < rule_sets.size(); i++) {
        clear_rules(rule_sets[i].rules);
        rule_sets[i].rules.clear();
    }
    rule_sets.clear();
}

bool parse_rule(ezxml_t rule, anomaly_rule &arule) {

    const char *vattr = NULL;
    ezxml_t conditions = ezxml_child(rule, "conditions");
    if (conditions) {

        ezxml_t device = NULL;
        for (device = ezxml_child(conditions, "device"); device;
                device = device->next) {

            anomaly_device adevice;

            vattr = ezxml_attr(device, "group");
            if (!vattr) {
                return false;
            }
            adevice.group = vattr;

            vattr = ezxml_attr(device, "name");
            if (!vattr) {
                return false;
            }
            adevice.name = vattr;

            vattr = ezxml_attr(device, "state");
            if (!vattr) {
                return false;
            }
            adevice.state = vattr;

            adevice.status = "*";
            vattr = ezxml_attr(device, "status");
            if (vattr) {
                adevice.status = vattr;
            }

            arule.conditions.devices.push_back(adevice);
        }
    }

    ezxml_t actions = ezxml_child(rule, "actions");
    if (actions) {

        vattr = ezxml_attr(actions, "level");
        if (!vattr) {
            return false;
        }

        arule.actions.level = vattr;

        vattr = ezxml_attr(actions, "wait");
        if (!vattr) {
            arule.actions.wait = "*";
        } else {
            arule.actions.wait = vattr;
        }

        ezxml_t action = NULL;
        for (action = ezxml_child(actions, "action"); action;
                action = action->next) {
            anomaly_action aaction;

            vattr = ezxml_attr(action, "play");
            if (!vattr) {
                return false;
            }
            aaction.play = vattr;

            vattr = ezxml_attr(action, "message");
            if (!vattr) {
                return false;
            }
            aaction.msg = vattr;

            vattr = ezxml_attr(action, "repeat");
            if (!vattr) {
                aaction.repeat = "*";
            } else {
                aaction.repeat = vattr;
            }

            arule.actions.actions.push_back(aaction);
        }
    }

    return true;
}

bool chb_rule_xml_parser::load_rule_xml(const char * rule_xml_file) {

    ezxml_t rule_set_0 = ezxml_parse_file(rule_xml_file);
    if (!rule_set_0) {
        ezxml_free(rule_set_0);
        return false;
    }

    clear_rule_set(m_rule_sets);
    for (ezxml_t rule_set = rule_set_0; rule_set; rule_set = rule_set->next) {

        anomaly_rule_set arule_set;
        arule_set.name = ezxml_attr(rule_set, "name");

        ezxml_t rule = NULL;
        for (rule = ezxml_child(rule_set, "rule"); rule; rule = rule->next) {
            anomaly_rule arule;
            bool ret = parse_rule(rule, arule);
            if (!ret) {
                cout << "failed to parse_rule" << endl;
                return false;
            }

            arule_set.rules.push_back(arule);
        }
        m_rule_sets.push_back(arule_set);
    }

    return true;
}

bool chb_rule_xml_parser::load_rule_xml(string &rule_xml_file) {

    return load_rule_xml(rule_xml_file.c_str());
}

void print_conditions(anomaly_conditions &conditions) {
    cout << "      conditions" << endl;
    for (size_t i = 0; i < conditions.devices.size(); i++) {
        cout << "        device : " << endl;
        cout << "            group = " << conditions.devices[i].group << endl;
        cout << "            name = " << conditions.devices[i].name << endl;
        cout << "            state = " << conditions.devices[i].state << endl;
        cout << "            status = " << conditions.devices[i].status << endl;
    }
}

void print_actions(anomaly_actions &actions) {

    cout << "      actions : level = " << actions.level << " wait = "
            << actions.wait << endl;

    for (size_t i = 0; i < actions.actions.size(); i++) {
        cout << "        action : " << endl;
        cout << "            play = " << actions.actions[i].play << endl;
        cout << "            msg = " << actions.actions[i].msg << endl;
        cout << "            repeat = " << actions.actions[i].repeat << endl;
    }
}

void print_rule(anomaly_rule &rule) {
    cout << "   rule" << endl;
    print_conditions(rule.conditions);
    print_actions(rule.actions);
}

void print_rule_set(anomaly_rule_set &rule_set) {
    cout << "rule_set: name = " << rule_set.name << endl;
    for (size_t i = 0; i < rule_set.rules.size(); i++) {
        print_rule(rule_set.rules[i]);
    }
}

void chb_rule_xml_parser::print_rule_sets() {

    cout << "rule_sets" << endl;
    for (size_t i = 0; i < m_rule_sets.size(); i++) {
        print_rule_set(m_rule_sets[i]);
    }
}

bool chb_rule_xml_parser::is_anomaly(anomaly_conditions &devices,
        anomaly_actions &actions) {

    /*
     printf("\n----------------------------------------------------\n");
     print_conditions(devices);
     printf("\n----------------------------------------------------\n");
     */

    for (size_t i = 0; i < m_rule_sets.size(); i++) {
        for (size_t j = 0; j < m_rule_sets[i].rules.size(); j++) {

            /*
             printf("\n++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
             print_conditions(m_rule_sets[i].rules[j].conditions);
             printf("\n++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
             */

            if (m_rule_sets[i].rules[j].conditions == devices) {
                actions = m_rule_sets[i].rules[j].actions;
                return true;
            }
        }
    }

    return false;
}
