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

#ifndef CHB_QUEUE_H_
#define CHB_QUEUE_H_

#include "hbutils.h"
#include <queue>

typedef struct _hb_data {
    hb_data_type type;
    ihb_obj *obj;
    string msg;
} hb_data;

typedef struct _hb_data_for_queue {
    std::string sid;
    uint64_t timestamp;
    hb_result result;
    hb_data data;
} hb_data_for_queue;

class chb_queue: public chb_obj {

protected:

    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;

    std::queue<hb_data_for_queue> m_queue;
    size_t m_queue_size;

public:
    chb_queue(size_t queue_size);
    virtual ~chb_queue();

    hb_result enque(std::string& sid, uint64_t timestamp, hb_result result,
            hb_data &data);
    hb_data deque(std::string& sid, uint64_t &timestamp, hb_result &result,
            bool wait);
    void flush();

};

#endif /* CHB_QUEUE_H_ */
