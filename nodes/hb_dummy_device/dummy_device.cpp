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

#include "dummy_device.h"
#include "cdummy_device.h"

hb_result hb_create_device_instance(ihb_data_bus *data_bus,
        ihb_msg_bus *msg_bus, hb_dev_type devtype, const char *uri,
        void *option, ihb_dev **obj) {

    HB_LOG("begin\n");

    if (devtype != hb_dev_type_generic || !obj) {
        HB_ERROR("hb_invalid_parameter\n");
        return hb_invalid_parameter;
    }

    hb_result ret = hb_fail;

    cdummy_device* dev = new cdummy_device(data_bus, msg_bus, uri, ret);
    if (!dev) {
        HB_ERROR("hb_insufficient_memory\n");
        return hb_insufficient_memory;
    }

    if (ret != hb_ok) {
        delete dev;
    }

    *obj = dev;

    HB_LOG("end\n");

    return ret;
}

