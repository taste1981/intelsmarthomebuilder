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


#ifndef CHB_MSG_BUSN_H_
#define CHB_MSG_BUSN_H_

#include "hbutils.h"
#include "chb_queue.h"
#include <map>

class chb_msg_bus: public ihb_msg_busN, public ihb_msg_bus, public chb_obj {

    pthread_mutex_t m_mutex;
    map<string, ihb_msg_sender *> m_sndlist;

public:
    chb_msg_bus();
    virtual ~chb_msg_bus();

    hb_result send(string &sid_to_exclude, string &oid, string &tid,
            uint64_t timestamp, hb_result result, hb_data &data, bool post);

    // ihb_msg_busN
    hb_result register_sender(string &sid, ihb_msg_sender** snd);
    hb_result unregister_sender(string &sid);
    hb_result register_listener(string &sid, ihb_msg_cb* cb);
    hb_result unregister_listener(string &sid, ihb_msg_cb* cb);

    // ihb_msg_bus
    hb_result register_sender(ihb_msg_sender** snd);
    hb_result register_listener(ihb_msg_cb* cb);
    hb_result unregister_listener(ihb_msg_cb* cb);
};

#endif /* CHB_MSG_BUSN_H_ */
