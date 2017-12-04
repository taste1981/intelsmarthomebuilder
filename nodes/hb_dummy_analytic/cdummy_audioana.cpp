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


#include "cdummy_audioana.h"
cdummy_audioana::cdummy_audioana(ihb_data_bus * data_bus, ihb_msg_bus *msg_bus) :
        m_data_bus(data_bus), m_msg_bus(msg_bus), m_msg_snd(NULL), m_bstarted(false) {

    HB_LOG("begin\n");

    if (m_msg_bus) {
        m_msg_bus->ref();
        m_msg_bus->register_sender(&m_msg_snd);
    }

    if (m_data_bus) {
        m_data_bus->ref();
    }

    // TODO: generate your own GUID
    m_tid = "011C8834-0772-4D50-A7EB-D33FAD85A798";

    pthread_mutex_init(&m_mutex, NULL);

    HB_LOG("end\n");
}

cdummy_audioana::~cdummy_audioana() {

    HB_LOG("begin\n");

    HB_SAFE_DEREF(m_msg_snd);
    HB_SAFE_DEREF(m_data_bus);
    HB_SAFE_DEREF(m_msg_bus);

    pthread_mutex_destroy(&m_mutex);
    HB_LOG("end\n");
}

const string & cdummy_audioana::get_tid() {
    return m_tid;
}

hb_data_type cdummy_audioana::get_data_type() {
    return hb_data_type_audio;
}

hb_result cdummy_audioana::stop() {
    HB_LOG("begin\n");

    chb_auto_lock lock(&m_mutex);
    if (!m_bstarted) {
        HB_LOG("Not yet started\n");
        return hb_ok;
    }

    HB_LOG("end\n");
    return hb_ok;
}

hb_result cdummy_audioana::start() {

    HB_LOG("begin\n");

    chb_auto_lock lock(&m_mutex);

    if (m_bstarted) {
        HB_LOG("Already started\n");
        return hb_ok;
    }

    m_bstarted = true;

    HB_LOG("end\n");
    return hb_ok;
}

void cdummy_audioana::on_msg(string &sid, uint64_t timestamp,
        hb_result result, string &msg) {

}

void cdummy_audioana::on_data(string &sid, uint64_t timestamp, hb_result result,
        hb_data_type type, ihb_obj *obj) {

    static int a = 0;

    a++;
    //printf("*** ana_a = %d, sid = %s\n", a, sid.c_str());


    string msg = "<?xml version=\"1.0\" ?>"
    "<home-builder-msg type=\"message\">"
    "<msg-data state=\"glass-breaking-detected\"/>"
    "</home-builder-msg>";

    chb_auto_lock lock(&m_mutex);
    if (m_msg_snd) {
        if (a % 100 == 0) {
            string tid = HB_RULE_ENGINE;
//            string msg = "hello from audio analytic";
            m_msg_snd->send_msg(sid, tid, 0, hb_ok, msg, true);
            printf("*** ana_a = %d, send msg, sid = %s\n", a, sid.c_str());
        }

    }

    if (!obj) {
        return;
    }

    /*
     * TODO:
     * ====================================================================================
     */

    /*
     * ====================================================================================
     */

}

