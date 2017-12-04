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


#include "chb_msg_bus.h"

#define HB_MSG_QUEUE_SIZE 35

class chb_queue_msg_cbN {

    friend class chb_msg_sender;

    ihb_msg_cb* m_cb;
    chb_queue* m_que;

    bool m_bstopthread;
    pthread_t m_threadid;
    static void* _thread(void *arg);
    void thread();

public:
    chb_queue_msg_cbN(ihb_msg_cb* cb, hb_result &result);
    ~chb_queue_msg_cbN();

    hb_result start();
    hb_result stop();
};

chb_queue_msg_cbN::chb_queue_msg_cbN(ihb_msg_cb* cb, hb_result &result) {
    HB_LOG("begin\n");

    if (!cb) {
        result = hb_invalid_parameter;
        HB_LOG("hb_invalid_parameter\n");
        return;
    }

    HB_LOG("to create queue\n");
    m_que = new chb_queue(HB_MSG_QUEUE_SIZE);
    HB_LOG("queue created : 0x%x\n", m_que);
    if (!m_que) {
        result = hb_insufficient_memory;
        HB_ERROR("hb_insufficient_memory\n");
        return;
    }
    m_cb = cb;

    HB_LOG("to ref the cb\n");
    m_cb->ref();

    m_bstopthread = false;
    m_threadid = 0;
    HB_LOG("end\n");
}

chb_queue_msg_cbN::~chb_queue_msg_cbN() {
    stop();

    HB_SAFE_DEREF(m_que);
    HB_SAFE_DEREF(m_cb);
}

hb_result chb_queue_msg_cbN::start() {

    if (!m_cb || !m_que) {
        HB_ERROR("!m_cb || !m_que\n");
        return hb_fail;
    }

    m_bstopthread = false;
    if (pthread_create(&m_threadid, NULL, &_thread, this) != 0) {
        HB_ERROR("failed to create thread\n");
        return hb_fail;
    }
    return hb_ok;
}

hb_result chb_queue_msg_cbN::stop() {

    m_bstopthread = true;
    if (m_que) {
        string sid = "unblock_deque";
        hb_data data;
        data.type = hb_data_type_none;
        data.obj = NULL;
        data.msg = "unblock_deque";
        m_que->enque(sid, 0, hb_ok, data); // to unblock dequeue
    }

    pthread_join(m_threadid, NULL);
    return hb_ok;
}

void* chb_queue_msg_cbN::_thread(void *arg) {
    chb_queue_msg_cbN* pthis = (chb_queue_msg_cbN*) arg;
    pthis->thread();
    return NULL;
}

void chb_queue_msg_cbN::thread() {

    while (!m_bstopthread) {

        string sid;
        uint64_t timestamp;
        hb_result result;

        hb_data buf = m_que->deque(sid, timestamp, result, true);
        if (m_bstopthread) {
            HB_SAFE_DEREF(buf.obj);
            break;
        }

        m_cb->on_msg(sid, timestamp, result, buf.msg);
        if (buf.obj) {
            buf.obj->deref();
        }
    }
}

class chb_msg_sender: public chb_obj, public ihb_msg_sender {

    pthread_mutex_t m_mutex;

    string m_sid;
    chb_msg_bus *m_bus;
    vector<chb_queue_msg_cbN *> m_que_cb_list;

public:
    chb_msg_sender(string &sid, chb_msg_bus *msg_bus);
    ~chb_msg_sender();

    hb_result add_cb(ihb_msg_cb *cb);
    hb_result remove_cb(ihb_msg_cb *cb);

    hb_result send_msg(string &oid, string &tid, uint64_t timestamp,
            hb_result result, string &msg, bool post);

    hb_result send(string &oid, string &tid, uint64_t timestamp,
            hb_result result, hb_data &data, bool post);
};

chb_msg_sender::chb_msg_sender(string &sid, chb_msg_bus *msg_bus) :
        m_sid(sid) {

    m_bus = msg_bus;
    m_bus->ref();

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m_mutex, &attr);
}

chb_msg_sender::~chb_msg_sender() {

    {
        chb_auto_lock lock(&m_mutex);
        for (size_t i = 0; i < m_que_cb_list.size(); i++) {
            HB_SAFE_DELETE(m_que_cb_list[i]);
        }
        m_bus->deref();
    }

    pthread_mutex_destroy(&m_mutex);
}

hb_result chb_msg_sender::add_cb(ihb_msg_cb *cb) {

    chb_auto_lock lock(&m_mutex);

    HB_LOG("begin\n");

    bool found = false;
    for (size_t i = 0; i < m_que_cb_list.size(); i++) {
        if (m_que_cb_list[i]->m_cb == cb) {
            found = true;
            break;
        }
    }

    if (!found) {
        hb_result ret;
        HB_LOG("to create chb_queue_data_cb\n");
        chb_queue_msg_cbN *que_cb = new chb_queue_msg_cbN(cb, ret);
        if (!que_cb) {
            return hb_insufficient_memory;
        }

        m_que_cb_list.push_back(que_cb);
        que_cb->start();
    }

    HB_LOG("end\n");
    return hb_ok;
}

hb_result chb_msg_sender::remove_cb(ihb_msg_cb *cb) {
    chb_auto_lock lock(&m_mutex);

    vector<chb_queue_msg_cbN *>::iterator pos;
    for (pos = m_que_cb_list.begin(); pos != m_que_cb_list.end(); ++pos) {
        chb_queue_msg_cbN *que_cb = *pos;
        if (que_cb->m_cb == cb) {
            m_que_cb_list.erase(pos);
            delete que_cb;
            break;
        }
    }

    return hb_ok;
}

hb_result chb_msg_sender::send_msg(string &oid, string &tid, uint64_t timestamp,
        hb_result result, string &msg, bool post) {

    hb_data data;
    data.type = hb_data_type_none;
    data.obj = NULL;
    data.msg = msg;
    return send(oid, tid, timestamp, result, data, post);
}

hb_result chb_msg_sender::send(string &oid, string &tid, uint64_t timestamp,
        hb_result result, hb_data &data, bool post) {

    chb_auto_lock lock(&m_mutex);

    // send data to m_sid registered cbs
    if (tid == HB_MSG_MULTICAST || tid == HB_MSG_BROADCAST) {
        for (size_t i = 0; i < m_que_cb_list.size(); i++) {
            if (post) {
                m_que_cb_list[i]->m_que->enque(oid, timestamp, result, data);
            } else {
                if (data.obj) {
                    data.obj->ref();
                }
                m_que_cb_list[i]->m_cb->on_msg(oid, timestamp, result,
                        data.msg);
                if (data.obj) {
                    data.obj->deref();
                }

            }
        }

        if (tid == HB_MSG_BROADCAST) {
            string _tid = HB_MSG_MULTICAST;
            m_bus->send(m_sid, oid, _tid, timestamp, result, data, post);
        }
        return hb_ok;
    }

    bool found = false;
    for (size_t i = 0; i < m_que_cb_list.size(); i++) {

        if (m_que_cb_list[i]->m_cb->get_tid() == tid) {
            if (post) {
                m_que_cb_list[i]->m_que->enque(oid, timestamp, result, data);
            } else {
                if (data.obj) {
                    data.obj->ref();
                }
                m_que_cb_list[i]->m_cb->on_msg(oid, timestamp, result,
                        data.msg);
                if (data.obj) {
                    data.obj->deref();
                }
            }
            found = true;
            break;
        }
    }

    if (!found) {
        m_bus->send(m_sid, oid, tid, timestamp, result, data, post);
    }

    return hb_ok;
}

chb_msg_bus::chb_msg_bus() {

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m_mutex, &attr);
    m_sndlist.clear();
}

chb_msg_bus::~chb_msg_bus() {
    HB_LOG("begin\n");

    {
        chb_auto_lock lock(&m_mutex);
        map<string, ihb_msg_sender *>::iterator pos;
        for (pos = m_sndlist.begin(); pos != m_sndlist.end(); ++pos) {
            pos->second->deref();
        }
        m_sndlist.clear();
    }

    pthread_mutex_destroy(&m_mutex);
    HB_LOG("end\n");
}

hb_result chb_msg_bus::register_sender(string &sid, ihb_msg_sender** snd) {

    HB_LOG("begin\n");

    if (!snd || sid.empty()) {
        HB_ERROR("hb_invalid_parameter\n");
        return hb_invalid_parameter;
    }

    chb_auto_lock lock(&m_mutex);
    map<string, ihb_msg_sender *>::iterator pos;

    pos = m_sndlist.find(sid);
    if (pos != m_sndlist.end()) {
        *snd = pos->second;
        pos->second->ref();
        HB_LOG("found\n");
        return hb_ok;
    }

    chb_msg_sender *_snd = new chb_msg_sender(sid, this);
    if (!_snd) {
        HB_ERROR("hb_insufficient_memory\n");
        return hb_insufficient_memory;
    }

    m_sndlist.insert(pair<string, chb_msg_sender *>(sid, _snd));
    _snd->ref();

    *snd = _snd;

    HB_LOG("end\n");
    return hb_ok;
}

hb_result chb_msg_bus::unregister_sender(string &sid) {
    if (sid.empty()) {
        HB_ERROR("hb_invalid_parameter\n");
        return hb_invalid_parameter;
    }

    chb_auto_lock lock(&m_mutex);
    map<string, ihb_msg_sender *>::iterator pos;
    pos = m_sndlist.find(sid);
    if (pos != m_sndlist.end()) {
        pos->second->deref();
        m_sndlist.erase(pos);
    }
    return hb_ok;
}

hb_result chb_msg_bus::register_listener(string &sid, ihb_msg_cb* cb) {

    HB_LOG("begin\n");

    if (!cb || sid.empty()) {
        HB_ERROR("hb_invalid_parameter\n");
        return hb_invalid_parameter;
    }

    chb_auto_lock lock(&m_mutex);
    map<string, ihb_msg_sender *>::iterator pos;

    chb_msg_sender *snd = NULL;
    pos = m_sndlist.find(sid);
    if (pos != m_sndlist.end()) {
        snd = dynamic_cast<chb_msg_sender *>(pos->second);
        HB_LOG("found the sid, add cb to it\n");
        return snd->add_cb(cb);
    }

    HB_LOG("sid is not found, create it\n");
    snd = new chb_msg_sender(sid, this);
    if (!snd) {
        HB_ERROR("hb_insufficient_memory\n");
        return hb_insufficient_memory;
    }
    m_sndlist.insert(pair<string, chb_msg_sender *>(sid, snd));

    hb_result ret = snd->add_cb(cb);

    HB_LOG("end\n");
    return ret;
}

hb_result chb_msg_bus::unregister_listener(string &sid, ihb_msg_cb* cb) {

    if (sid.empty()) {
        HB_ERROR("hb_invalid_parameter\n");
        return hb_invalid_parameter;
    }

    HB_LOG("begin\n");
    chb_auto_lock lock(&m_mutex);
    map<string, ihb_msg_sender *>::iterator pos;
    pos = m_sndlist.find(sid);
    if (pos != m_sndlist.end()) {
        chb_msg_sender *snd = dynamic_cast<chb_msg_sender *>(pos->second);
        return snd->remove_cb(cb);
    }
    HB_LOG("end\n");
    return hb_ok;
}

hb_result chb_msg_bus::send(string &sid_to_exclude, string &oid, string &tid,
        uint64_t timestamp, hb_result result, hb_data &data, bool post) {

    chb_auto_lock lock(&m_mutex);
    map<string, ihb_msg_sender *>::iterator pos;
    for (pos = m_sndlist.begin(); pos != m_sndlist.end(); ++pos) {
        if (pos->first != sid_to_exclude) {
            chb_msg_sender *snd = dynamic_cast<chb_msg_sender *>(pos->second);
            snd->send(oid, tid, timestamp, result, data, post);
        }
    }

    return hb_ok;
}

hb_result chb_msg_bus::register_sender(ihb_msg_sender** snd) {
    string sid = HB_MESSAGE_BUS;
    return register_sender(sid, snd);
}

hb_result chb_msg_bus::register_listener(ihb_msg_cb* cb) {
    string sid = HB_MESSAGE_BUS;
    return register_listener(sid, cb);
}

hb_result chb_msg_bus::unregister_listener(ihb_msg_cb* cb) {
    string sid = HB_MESSAGE_BUS;
    return unregister_listener(sid, cb);
}
