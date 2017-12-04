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


#ifndef HBBUFPOOL_H_
#define HBBUFPOOL_H_

#include "hbutils.h"
#include <vector>

class chb_data_buffer;
class chb_data_buffer_pool: public chb_obj {
    friend class chb_data_buffer;

    pthread_mutex_t m_mutex;
    std::vector<chb_data_buffer *> m_buffer_list;
    pthread_cond_t m_cond;

public:
    chb_data_buffer_pool();
    ~chb_data_buffer_pool();

    hb_result add_buffer(chb_data_buffer *buffer);

    chb_data_buffer *get_free_buffer();

    size_t get_buffer_count();
    chb_data_buffer *get_buffer(size_t index);
};

class chb_data_buffer: public virtual ihb_obj {
    friend class chb_data_buffer_pool;

    pthread_mutex_t m_mutex;

    chb_data_buffer_pool *m_pool;

    void set_pool(chb_data_buffer_pool *pool);

protected:
    int m_refcnt;

    bool is_locked();

public:

    chb_data_buffer();
    ~chb_data_buffer();

    void ref();
    void deref();
};


#endif /* HBBUFPOOL_H_ */
