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

#include "hb.h"

#include "chb_data_bus.h"
#include "chb_msg_bus.h"
#include "chb_device_proxy.h"
#include "chb_analytic_proxy.h"
#include "chb_rule_engine_proxy.h"
#include "chb_devana_man.h"

hb_result hb_create_data_bus(ihb_data_bus **dbus) {

    if (!dbus) {
        HB_ERROR("Invalid parameter\n");
        return hb_invalid_parameter;
    }

    ihb_data_bus * bus = new chb_data_bus();
    if (!bus) {
        HB_ERROR("Failed to create data bus\n");
        return hb_insufficient_memory;
    }

    *dbus = bus;
    return hb_ok;
}

hb_result hb_create_msg_bus(ihb_msg_bus **mbus) {

    if (!mbus) {
        HB_ERROR("Invalid parameter\n");
        return hb_invalid_parameter;
    }

    ihb_msg_bus * bus = new chb_msg_bus();
    if (!bus) {
        HB_ERROR("Failed to create message bus\n");
        return hb_insufficient_memory;
    }

    *mbus = bus;
    return hb_ok;
}

#if 0
hb_result hb_create_device(string &factory, hb_dev_type devtype,
        const char *uri, ihb_data_bus* dbus, ihb_msg_bus * mbus, void *option,
        ihb_dev **dev) {

    if (!dev) {
        HB_ERROR("Invalid parameter\n");
        return hb_invalid_parameter;
    }

    hb_result ret = hb_ok;
    chb_device_proxy *devpxy = new chb_device_proxy(factory, devtype, uri, dbus,
            mbus, option, ret);

    if (!devpxy || ret != hb_ok) {
        HB_ERROR("failed to new chb_device_proxy\n");
        HB_SAFE_DEREF(devpxy);
        return hb_fail;
    }

    *dev = devpxy;
    return ret;
}

hb_result hb_create_data_analytics(string &factory, ihb_data_bus * dbus,
        ihb_msg_bus * mbus, vector<ihb_data_analytic *> &alist) {

    hb_result ret = chb_data_analytic_proxy::create_data_analytics(dbus, mbus,
            factory, alist);

    if (ret != hb_ok) {
        HB_ERROR("failed to create_data_analytics, but, let's continue\n");
    }

    return ret;
}
#endif

hb_result hb_create_dev_manager(ihb_data_bus * dbus, ihb_msg_bus * mbus,
        ihb_dev_manager ** man) {

    if (!man) {
        HB_ERROR("Invalid parameter\n");
        return hb_invalid_parameter;
    }

    chb_devana_manager *devana_man = new chb_devana_manager(dbus, mbus);
    if (!devana_man) {
        HB_ERROR("Failed to create device manager\n");
        return hb_insufficient_memory;
    }

    *man = devana_man;
    return hb_ok;
}

#if 0
hb_result hb_create_rule_engine(ihb_data_bus *dbus, ihb_msg_bus *mbus,
        ihb_dev_manager *man, ihb_rule_engine **obj) {
    if (!obj) {
        HB_ERROR("Invalid parameter\n");
        return hb_invalid_parameter;
    }

    hb_result ret = hb_ok;

    chb_rule_engine *re = new chb_rule_engine(mbus, dbus, man);

    if (!re || ret != hb_ok) {
        HB_ERROR("failed to new chb_rule_engine_proxy\n");
        HB_SAFE_DEREF(re);
        return hb_fail;
    }

    *obj = re;
    return hb_ok;
}
#endif

hb_result hb_create_rule_engine(string& repath, string& ahpath,
        ihb_data_bus *dbus, ihb_msg_bus *mbus, ihb_dev_manager *man,
        ihb_rule_engine **obj) {
    if (!obj) {
        HB_ERROR("Invalid parameter\n");
        return hb_invalid_parameter;
    }

    hb_result ret = hb_ok;

    chb_rule_engine_proxy *re = new chb_rule_engine_proxy(repath, ahpath, dbus,
            mbus, man, ret);

    if (!re || ret != hb_ok) {
        HB_ERROR("failed to new chb_rule_engine_proxy\n");
        HB_SAFE_DEREF(re);
        return hb_fail;
    }

    *obj = re;
    return hb_ok;
}

