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


#include "dummy_analytic.h"
#include "cdummy_videoana.h"
#include "cdummy_audioana.h"

#define DUMMY_ANALYTIC_COUNT 2

int hb_get_analytic_count() {
    HB_LOG("begin\n");
    HB_LOG("end\n");
    return DUMMY_ANALYTIC_COUNT;
}

hb_result hb_create_analytic_instance(int index, ihb_data_bus *dbus,
        ihb_msg_bus *mbus, ihb_data_analytic **obj) {

    HB_LOG("begin\n");

    if (index >= DUMMY_ANALYTIC_COUNT || index < 0) {
        HB_ERROR("hb_invalid_parameter\n");
        return hb_invalid_parameter;
    }

    if (!obj) {
        HB_ERROR("hb_invalid_parameter\n");
        return hb_invalid_parameter;
    }

    if (index == 0) {

        cdummy_videoana *ana = new cdummy_videoana(dbus, mbus);
        if (!ana) {
            HB_ERROR("hb_insufficient_memory\n");
            return hb_insufficient_memory;
        }

        *obj = ana;
    }

    if (index == 1) {

        cdummy_audioana *ana = new cdummy_audioana(dbus, mbus);
        if (!ana) {
            HB_ERROR("hb_insufficient_memory\n");
            return hb_insufficient_memory;
        }

        *obj = ana;
    }

    HB_LOG("end\n");
    return hb_ok;
}
