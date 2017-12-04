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


#include "cdummy_videoana.h"
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

cdummy_videoana::cdummy_videoana(ihb_data_bus * dbus, ihb_msg_bus *mbus) :
        m_dbus(dbus), m_mbus(mbus), m_msnd(NULL), m_bstarted(
                false) {

    m_vtimestamp = 0;
    m_vframecount = 0;

    HB_LOG("begin\n");

    if (m_mbus) {
        m_mbus->register_sender(&m_msnd);
        m_mbus->ref();
    }

    if (m_dbus) {
        m_dbus->ref();
    }

    // TODO: generate your own GUID
    m_tid = "83650CA5-83E1-4A6B-BD85-505140176164";

    pthread_mutex_init(&m_mutex, NULL);

    HB_LOG("end\n");
}

cdummy_videoana::~cdummy_videoana() {

    HB_LOG("begin\n");

    HB_SAFE_DEREF(m_msnd);
    HB_SAFE_DEREF(m_dbus);
    HB_SAFE_DEREF(m_mbus);

    pthread_mutex_destroy(&m_mutex);
    HB_LOG("end\n");
}

const string & cdummy_videoana::get_tid() {
    return m_tid;
}

hb_data_type cdummy_videoana::get_data_type() {
    return hb_data_type_video;
}

hb_result cdummy_videoana::stop() {
    HB_LOG("begin\n");

    chb_auto_lock lock(&m_mutex);
    if (!m_bstarted) {
        HB_LOG("Not yet started\n");
        return hb_ok;
    }

    HB_LOG("end\n");
    return hb_ok;
}

hb_result cdummy_videoana::start() {

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

void cdummy_videoana::on_msg(string &sid, uint64_t timestamp, hb_result result,
        string &msg) {

}

void cdummy_videoana::on_data(string &sid, uint64_t timestamp, hb_result result,
        hb_data_type type, ihb_obj *obj) {

    string msg = "<?xml version=\"1.0\" ?>"
            "<home-builder-msg type=\"message\" >"
            "<msg-data state=\"person-detected\" status=\"unknown\" />"
            "</home-builder-msg>";

    m_count ++;
    if (m_msnd) {
        if (m_count % 50 == 0) {
            string tid = HB_RULE_ENGINE;
            m_msnd->send_msg(sid, tid, 0, hb_ok, msg, true);
            // printf("*** ana_v = %d, send msg, sid = %s\n", v, sid.c_str());
        }
    }

    if (!obj) {
        HB_LOG("!obj\n");
        return;
    }
    if (type != hb_data_type_video) {
        HB_LOG("type != hb_data_type_video\n");
        return;
    }

    chb_auto_lock lock(&m_mutex);

    // frame rate logging
    // -----------------------------------------------------------
    // HB_LOG("m_vframecount = %d threadid = 0x%x\n", m_vframecount);

    if (m_vframecount >= 30) {
        uint64_t vtimestamp = hb_get_timestamp();
        double delta = (vtimestamp - m_vtimestamp) / 10000000.0;

        double fps = (double) m_vframecount / delta;
        m_vframecount = 0;

        printf("\x1B[35m" "video-analytic fps = %lf, name = 0x%x\n" "\x1B[0m",
                fps, this);

    }
    if (m_vframecount == 0) {
        m_vtimestamp = hb_get_timestamp();
    }

    m_vframecount++;

    usleep(1000000 / 10); // 1/10 second
//    sleep(1);
            // -----------------------------------------------------------

    /*
     * TODO:
     * ====================================================================================
     */

    /*
     * ====================================================================================
     */
}
