#include "chb_data_bus.h"
#include "chb_queue.h"
#include <string.h>

#define HBQUEUE_SIZE 10

class chb_queue_data_cb {

    friend class chb_data_sender;

    ihb_data_cb* m_cb;
    chb_queue* m_que;

    bool m_bstopthread;
    pthread_t m_threadid;
    static void* _thread(void *arg);
    void thread();

public:
    chb_queue_data_cb(ihb_data_cb* cb, hb_result &result);
    ~chb_queue_data_cb();

    hb_result start();
    hb_result stop();
};

chb_queue_data_cb::chb_queue_data_cb(ihb_data_cb* cb, hb_result &result) {
    HB_LOG("begin\n");

    if (!cb) {
        result = hb_invalid_parameter;
        HB_ERROR("hb_invalid_parameter\n");
        return;
    }

    HB_LOG("to create queue\n");
    m_que = new chb_queue(HBQUEUE_SIZE);
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

chb_queue_data_cb::~chb_queue_data_cb() {
    stop();

    HB_SAFE_DEREF(m_que);
    HB_SAFE_DEREF(m_cb);
}

hb_result chb_queue_data_cb::start() {

    if (!m_cb || !m_que) {
        return hb_fail;
    }

    m_bstopthread = false;
    if (pthread_create(&m_threadid, NULL, &_thread, this) != 0) {
        return hb_fail;
    }
    return hb_ok;
}

hb_result chb_queue_data_cb::stop() {

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

void* chb_queue_data_cb::_thread(void *arg) {
    chb_queue_data_cb* pthis = (chb_queue_data_cb*) arg;
    pthis->thread();
    return NULL;
}

void chb_queue_data_cb::thread() {

    while (!m_bstopthread) {

        string sid;
        uint64_t timestamp;
        hb_result result;

        hb_data buf = m_que->deque(sid, timestamp, result, true);
        if (m_bstopthread) {
            HB_SAFE_DEREF(buf.obj);
            break;
        }

        m_cb->on_data(sid, timestamp, result, buf.type, buf.obj);
        HB_SAFE_DEREF(buf.obj);
    }
}

class chb_data_sender: public chb_obj, public ihb_data_sender {

    pthread_mutex_t m_mutex;

    string m_sid;
    vector<chb_queue_data_cb *> m_que_cb_list;

public:
    chb_data_sender(string &sid);
    ~chb_data_sender();

    hb_result add_cb(ihb_data_cb *cb);
    hb_result remove_cb(ihb_data_cb *cb);

    hb_result send_data(uint64_t timestamp, hb_result result, hb_data_type type,
            ihb_obj *obj, bool post);

    hb_result send(uint64_t timestamp, hb_result result, hb_data &data,
            bool post);
};

chb_data_sender::chb_data_sender(string &sid) :
        m_sid(sid) {

    HB_LOG("begin\n");
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m_mutex, &attr);
    HB_LOG("end\n");
}

chb_data_sender::~chb_data_sender() {

    HB_LOG("begin\n");
    {
        chb_auto_lock lock(&m_mutex);
        for (size_t i = 0; i < m_que_cb_list.size(); i++) {
            HB_SAFE_DELETE(m_que_cb_list[i]);
        }
    }

    pthread_mutex_destroy(&m_mutex);
    HB_LOG("end\n");
}

hb_result chb_data_sender::add_cb(ihb_data_cb *cb) {

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
        chb_queue_data_cb *que_cb = new chb_queue_data_cb(cb, ret);
        if (!que_cb) {
            return hb_insufficient_memory;
        }

        m_que_cb_list.push_back(que_cb);
        que_cb->start();
    }

    HB_LOG("end\n");
    return hb_ok;
}

hb_result chb_data_sender::remove_cb(ihb_data_cb *cb) {

    HB_LOG("begin\n");
    chb_auto_lock lock(&m_mutex);

    vector<chb_queue_data_cb *>::iterator pos;
    for (pos = m_que_cb_list.begin(); pos != m_que_cb_list.end(); ++pos) {
        chb_queue_data_cb *que_cb = *pos;
        if (que_cb->m_cb == cb) {
            m_que_cb_list.erase(pos);
            delete que_cb;
            break;
        }
    }

    HB_LOG("end\n");
    return hb_ok;
}

hb_result chb_data_sender::send_data(uint64_t timestamp, hb_result result,
        hb_data_type type, ihb_obj *obj, bool post) {

    hb_data data;
    data.obj = obj;
    data.type = type;
    return send(timestamp, result, data, post);
}

hb_result chb_data_sender::send(uint64_t timestamp, hb_result result,
        hb_data &data, bool post) {

    chb_auto_lock lock(&m_mutex);

    for (size_t i = 0; i < m_que_cb_list.size(); i++) {
        if (post) {
            m_que_cb_list[i]->m_que->enque(m_sid, timestamp, result, data);
        } else {
            if (data.obj) {
                data.obj->ref();
            }
            m_que_cb_list[i]->m_cb->on_data(m_sid, timestamp, result, data.type,
                    data.obj);
            if (data.obj) {
                data.obj->deref();
            }
        }
    }

    return hb_ok;
}

chb_data_bus::chb_data_bus() {

    pthread_mutex_init(&m_mutex, NULL);
    m_sender_list.clear();
}

chb_data_bus::~chb_data_bus() {
    HB_LOG("begin\n");

    {
        chb_auto_lock lock(&m_mutex);
        map<string, ihb_data_sender *>::iterator pos;
        for (pos = m_sender_list.begin(); pos != m_sender_list.end(); ++pos) {
            pos->second->deref();
        }
        m_sender_list.clear();
    }

    pthread_mutex_destroy(&m_mutex);
    HB_LOG("end\n");
}

hb_result chb_data_bus::register_sender(string &sid, ihb_data_sender** snd) {

    HB_LOG("begin\n");

    if (!snd || sid.empty()) {
        HB_ERROR("hb_invalid_parameter\n");
        return hb_invalid_parameter;
    }

    chb_auto_lock lock(&m_mutex);
    map<string, ihb_data_sender *>::iterator pos;

    pos = m_sender_list.find(sid);
    if (pos != m_sender_list.end()) {
        *snd = pos->second;
        pos->second->ref();
        HB_ERROR("found\n");
        return hb_ok;
    }

    chb_data_sender *_snd = new chb_data_sender(sid);
    if (!_snd) {
        HB_ERROR("hb_insufficient_memory\n");
        return hb_insufficient_memory;
    }

    m_sender_list.insert(pair<string, chb_data_sender *>(sid, _snd));
    _snd->ref();

    *snd = _snd;

    HB_LOG("end\n");
    return hb_ok;
}

hb_result chb_data_bus::register_listener(string &sid, ihb_data_cb* cb) {

    HB_LOG("begin\n");

    if (!cb || sid.empty()) {
        HB_ERROR("hb_invalid_parameter\n");
        return hb_invalid_parameter;
    }

    chb_auto_lock lock(&m_mutex);
    map<string, ihb_data_sender *>::iterator pos;

    chb_data_sender *snd = NULL;
    pos = m_sender_list.find(sid);
    if (pos != m_sender_list.end()) {
        snd = dynamic_cast<chb_data_sender *>(pos->second);
        HB_LOG("found the sid, add cb to it\n");
        return snd->add_cb(cb);
    }

    HB_LOG("sid is not found, create it\n");
    snd = new chb_data_sender(sid);
    if (!snd) {
        HB_ERROR("hb_insufficient_memory\n");
        return hb_insufficient_memory;
    }
    m_sender_list.insert(pair<string, chb_data_sender *>(sid, snd));

    hb_result ret = snd->add_cb(cb);

    HB_LOG("end\n");
    return ret;
}

hb_result chb_data_bus::unregister_sender(string &sid) {

    if (sid.empty()) {
        HB_ERROR("hb_invalid_parameter\n");
        return hb_invalid_parameter;
    }

    chb_auto_lock lock(&m_mutex);
    map<string, ihb_data_sender *>::iterator pos;
    pos = m_sender_list.find(sid);
    if (pos != m_sender_list.end()) {
        pos->second->deref();
        m_sender_list.erase(pos);
    }
    return hb_ok;
}

hb_result chb_data_bus::unregister_listener(string &sid, ihb_data_cb* cb) {

    if (sid.empty()) {
        HB_ERROR("hb_invalid_parameter\n");
        return hb_invalid_parameter;
    }

    chb_auto_lock lock(&m_mutex);
    map<string, ihb_data_sender *>::iterator pos;
    pos = m_sender_list.find(sid);
    if (pos != m_sender_list.end()) {
        chb_data_sender *snd = dynamic_cast<chb_data_sender *>(pos->second);
        return snd->remove_cb(cb);
    }

    return hb_ok;
}
