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

#include "chb_queue.h"

chb_queue::chb_queue(size_t queue_size) :
        m_queue_size(queue_size) {

    HB_LOG("begin\n");

    pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_cond, NULL);

    HB_LOG("end\n");
}

chb_queue::~chb_queue() {
    flush();

    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_cond);
}

hb_result chb_queue::enque(string& sid, uint64_t timestamp, hb_result result,
        hb_data &buf) {

//	HB_LOG("begin, thread = 0x%x\n", pthread_self());

    pthread_mutex_lock(&m_mutex);

    hb_data_for_queue data4que;
    while (m_queue.size() > m_queue_size) {

//        HB_ERROR(" --- queue is full: size = %d --- \n", m_queue_size);

        data4que = m_queue.front();
        m_queue.pop();
        if (data4que.data.obj) {
            data4que.data.obj->deref();
        }
    }

    data4que.sid = sid;
    data4que.timestamp = timestamp;
    data4que.result = result;
    data4que.data = buf;
    m_queue.push(data4que);
    if (buf.obj) {
        buf.obj->ref();
    }

    pthread_cond_signal(&m_cond);

    pthread_mutex_unlock(&m_mutex);

//	HB_LOG("end\n");

    return hb_ok;
}

hb_data chb_queue::deque(string& sid, uint64_t &timestamp, hb_result &result,
        bool wait) {

//	HB_LOG("begin, thread = 0x%x\n", pthread_self());

    pthread_mutex_lock(&m_mutex);

    hb_data buf;
    if (m_queue.empty() && wait) {
        pthread_cond_wait(&m_cond, &m_mutex);
    }

    hb_data_for_queue data4que;
    if (!m_queue.empty()) {
        data4que = m_queue.front();
        m_queue.pop();

        sid = data4que.sid;
        timestamp = data4que.timestamp;
        result = data4que.result;
        buf = data4que.data;
    }

    pthread_mutex_unlock(&m_mutex);

//	HB_LOG("end\n");

    return buf;
}

void chb_queue::flush() {
    chb_auto_lock lock(&m_mutex);
    while (!m_queue.empty()) {
        hb_data_for_queue data4que;
        data4que = m_queue.front();
        if (data4que.data.obj) {
            data4que.data.obj->deref();
        }
        m_queue.pop();
    }
}

