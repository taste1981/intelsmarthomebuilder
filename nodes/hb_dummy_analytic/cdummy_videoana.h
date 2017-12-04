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


#ifndef CDUMMY_VIDEOANA_H_
#define CDUMMY_VIDEOANA_H_

#include "hbutils.h"

class cdummy_videoana: public ihb_data_analytic, public chb_obj {

    pthread_mutex_t m_mutex;
    ihb_data_bus *m_dbus;
    ihb_msg_bus *m_mbus;
    ihb_msg_sender *m_msnd;

    string m_tid;
    pthread_cond_t m_cond_start;

    bool m_bstarted;

    // for framerate logging
    uint64_t m_vtimestamp;
    int m_vframecount;
    int m_count;


public:

    cdummy_videoana(ihb_data_bus * dbus, ihb_msg_bus *mbus);
    virtual ~cdummy_videoana();

    void on_data(string &sid, uint64_t timestamp, hb_result result,
            hb_data_type type, ihb_obj *obj);

    void on_msg(string &sid, uint64_t timestamp, hb_result result, string &msg);

    const string & get_tid();

    hb_data_type get_data_type();
    hb_result start();
    hb_result stop();

};

#endif /* CDUMMY_VIDEOANA_H_ */
