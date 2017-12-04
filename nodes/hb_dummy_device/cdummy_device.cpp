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


#include "cdummy_device.h"

cdummy_device::cdummy_device(ihb_data_bus* data_bus, ihb_msg_bus * msg_bus,
        const char *uri, hb_result &result) {

    m_data_bus = data_bus;
    m_msg_bus = msg_bus;

    if (!data_bus || !msg_bus) {
        result = hb_invalid_parameter;
        return;
    }

    data_bus->ref();
    msg_bus->ref();

    pthread_mutex_init(&m_mutex, NULL);
    m_bstarted = false;

    m_devdesc.id = "C7F1E7BA-8267-41FA-BAFA-D38D6D70A77C-DUMMY-DEVICE";
    m_devdesc.uri = uri;

    m_devdesc.devtype = hb_dev_type_generic;
    m_devdesc.datatype = hb_data_type_generic;

    m_devdesc.name = "generic device";
    m_devdesc.group = "unknown";

    result = hb_ok;
}

cdummy_device::~cdummy_device() {

    stop();

    pthread_mutex_lock(&m_mutex);

    HB_SAFE_DEREF(m_data_bus);
    HB_SAFE_DEREF(m_msg_bus);

    pthread_mutex_unlock(&m_mutex);
    pthread_mutex_destroy(&m_mutex);
}

hb_result cdummy_device::get_attr_descs(string &attributes) {
    return hb_not_implemented;
}

hb_result cdummy_device::get_attr_value(string &name, string &value) {
    return hb_not_implemented;
}

hb_result cdummy_device::set_attr_value(string &name, string &value) {

    return hb_not_implemented;
}

hb_result cdummy_device::get_dev_desc(hb_dev_desc *desc) {
    if (!desc) {
        return hb_invalid_parameter;
    }

    *desc = m_devdesc;
    return hb_ok;
}

hb_result cdummy_device::get_dev_status(hb_dev_status &status) {
    return hb_not_implemented;
}

hb_result cdummy_device::set_dev_group(string &group) {
    m_devdesc.group = group;
    return hb_ok;
}

hb_result cdummy_device::set_dev_name(string &name) {
    m_devdesc.name = name;
    return hb_ok;
}

// for continuous streams
hb_result cdummy_device::start() {

    chb_auto_lock lock(&m_mutex);
    if (m_bstarted) {
        return hb_busy;
    }

    hb_result ret = hb_ok;

    m_bstarted = true;

    if (ret != hb_ok) {
    }

    return ret;

}

hb_result cdummy_device::stop() {

    {
        chb_auto_lock lock(&m_mutex);
        if (!m_bstarted) {
            return hb_ok;
        }

    }

    return hb_ok;
}
