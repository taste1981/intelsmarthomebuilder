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

#include "hbpool.h"

chb_data_buffer_pool::chb_data_buffer_pool() {
    pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_cond, NULL);

    m_buffer_list.clear();
}

chb_data_buffer_pool::~chb_data_buffer_pool() {

    pthread_mutex_lock(&m_mutex);
    chb_data_buffer *buffer = NULL;
    for (size_t i = 0; i < m_buffer_list.size(); i++) {
        buffer = m_buffer_list[i];
        HB_SAFE_DEREF(buffer);
    }
    pthread_mutex_unlock(&m_mutex);

    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_cond);
}

hb_result chb_data_buffer_pool::add_buffer(chb_data_buffer *buffer) {

//	HB_LOG("begin\n");

    if (!buffer) {
        HB_ERROR("hb_invalid_parameter\n");
        return hb_invalid_parameter;
    }

    {
        chb_auto_lock lock(&m_mutex);
        m_buffer_list.push_back(buffer);
    }

    buffer->set_pool(this);

//	HB_LOG("end\n");

    return hb_ok;
}

chb_data_buffer *chb_data_buffer_pool::get_free_buffer() {
    chb_data_buffer *buffer = NULL;
    chb_data_buffer *free_buffer = NULL;
//	HB_LOG("begin\n");

    while (true) {
        pthread_mutex_lock(&m_mutex);
        for (size_t i = 0; i < m_buffer_list.size(); i++) {
            buffer = m_buffer_list[i];
            if (!buffer->is_locked()) {
                buffer->ref();
                free_buffer = buffer;
                break;
            }
        }

        if (free_buffer) {
            pthread_mutex_unlock(&m_mutex);
            break;
        }
//        HB_LOG(" NO FREE BUFFER 1\n");
        pthread_cond_wait(&m_cond, &m_mutex);
//        HB_LOG(" NO FREE BUFFER 2\n");
        pthread_mutex_unlock(&m_mutex);
    }

//	HB_LOG("end\n");

//	HB_LOG("free buffer = 0x%x cnt = %d\n", free_buffer, free_buffer->m_refcnt);

    return free_buffer;
}

size_t chb_data_buffer_pool::get_buffer_count() {
    HB_LOG("begin\n");
    chb_auto_lock lock(&m_mutex);
    HB_LOG("end\n");
    return m_buffer_list.size();
}

chb_data_buffer* chb_data_buffer_pool::get_buffer(size_t index) {
//	HB_LOG("begin\n");
    chb_auto_lock lock(&m_mutex);
    if (index >= m_buffer_list.size()) {
        return NULL;
    }

//	HB_LOG("end\n");
    return m_buffer_list[index];
}

chb_data_buffer::chb_data_buffer() :
        m_refcnt(1), m_pool(NULL) {
    pthread_mutex_init(&m_mutex, NULL);
}

chb_data_buffer::~chb_data_buffer() {
    HB_SAFE_DEREF(m_pool);
    pthread_mutex_destroy(&m_mutex);
}

void chb_data_buffer::ref() {
    chb_auto_lock lock(&m_mutex);
    m_refcnt++;

//	HB_LOG("this = 0x%x, cnt = %d\n", this, m_refcnt);
}

void chb_data_buffer::deref() {
    chb_auto_lock lock(&m_mutex);
    if (--m_refcnt == 0) {
        delete this;
    }

//	HB_LOG("this = 0x%x, cnt = %d\n", this, m_refcnt);

    if (m_refcnt == 1 && m_pool) {
        pthread_cond_signal(&m_pool->m_cond);
    }
}

bool chb_data_buffer::is_locked() {
    chb_auto_lock lock(&m_mutex);
    return m_refcnt > 1;
}

void chb_data_buffer::set_pool(chb_data_buffer_pool *pool) {
    m_pool = pool;
    if (m_pool) {
        m_pool->ref();
    }
}
