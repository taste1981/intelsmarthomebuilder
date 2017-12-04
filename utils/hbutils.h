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


#ifndef HBUTILS_H_
#define HBUTILS_H_

#include "hbdef.h"

class chb_auto_lock {
    pthread_mutex_t *m_mutex;

public:
    chb_auto_lock(pthread_mutex_t *mutex) {
        m_mutex = mutex;
        pthread_mutex_lock(m_mutex);
    }

    ~chb_auto_lock() {
        pthread_mutex_unlock(m_mutex);
    }

    void Lock() {
        pthread_mutex_lock(m_mutex);
    }

    void UnLock() {
        pthread_mutex_unlock(m_mutex);
    }
};

inline uint64_t hb_get_timestamp() {

    timespec tm;
    clock_gettime( CLOCK_REALTIME, &tm);
    return (tm.tv_nsec + tm.tv_sec * 1000000000LL) / 100;    // 100 nano unit
}

class chb_obj: public virtual ihb_obj {

protected:
    int m_refcnt;
    pthread_mutex_t m_refcnt_mutex;

public:
    chb_obj() :
            m_refcnt(1) {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&m_refcnt_mutex, &attr);
    }

    virtual ~chb_obj() {
        pthread_mutex_destroy(&m_refcnt_mutex);
    }

    void ref() {
        chb_auto_lock lock(&m_refcnt_mutex);
        m_refcnt++;
    }
    void deref() {
        chb_auto_lock lock(&m_refcnt_mutex);
        if (--m_refcnt == 0) {
            delete this;
        }
    }
};


#endif /* HBUTILS_H_ */
