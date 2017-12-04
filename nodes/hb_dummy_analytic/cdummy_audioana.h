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


#ifndef CDUMMY_AUDIOANA_H_
#define CDUMMY_AUDIOANA_H_

#include "hbutils.h"

class cdummy_audioana: public ihb_data_analytic, public chb_obj {

    pthread_mutex_t m_mutex;
    ihb_data_bus *m_data_bus;
    ihb_msg_bus *m_msg_bus;
    ihb_msg_sender *m_msg_snd;

    string m_tid;
    pthread_cond_t m_cond_start;

    bool m_bstarted;

public:

    cdummy_audioana(ihb_data_bus * data_bus, ihb_msg_bus *msg_bus);
    virtual ~cdummy_audioana();

    void on_msg(string &sid, uint64_t timestamp, hb_result result, string &msg);

    void on_data(string &sid, uint64_t timestamp, hb_result result,
            hb_data_type type, ihb_obj *obj);

    const string & get_tid();

    hb_data_type get_data_type();
    hb_result start();
    hb_result stop();

};

#endif /* CDUMMY_AUDIOANA_H_ */
