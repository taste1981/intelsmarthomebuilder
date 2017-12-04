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

int main() {

    hb_result ret;

    ihb_data_bus *dbus = NULL;
    ihb_msg_bus * mbus = NULL;
    ihb_dev_manager *dman = NULL;
    ihb_rule_engine *reng = NULL;

    string cfg = "./config.xml";
    string rules = "./rules.xml";
    string repath = "./libhb_rule_engine.so";
    string ahpath = "./libhb_action_handler.so";

    ret = hb_create_data_bus(&dbus);
    if (ret != hb_ok) {
        HB_ERROR("Failed to create data bus\n");
        goto cleanup;
    }

    ret = hb_create_msg_bus(&mbus);
    if (ret != hb_ok) {
        HB_ERROR("Failed to create msg bus\n");
        goto cleanup;
    }

    ret = hb_create_dev_manager(dbus, mbus, &dman);
    if (ret != hb_ok) {
        HB_ERROR("Failed to create device manager\n");
        goto cleanup;
    }

    ret = dman->load_config(cfg);
    if (ret != hb_ok) {
        HB_ERROR("Failed to load configuration file\n");
        goto cleanup;
    }

    ret = hb_create_rule_engine(repath, ahpath, dbus, mbus, dman, &reng);
    if (ret != hb_ok) {
        HB_ERROR("Failed to create rule engine\n");
        goto cleanup;
    }

    mbus->register_listener(reng);

    ret = reng->load_rules(rules);
    if (ret != hb_ok) {
        HB_ERROR("Failed to load rules\n");
        goto cleanup;
    }

    while (true) {
        char c = getchar();
        if (c == 'q' || c == 'Q') {
            break;
        }
    }

    cleanup:

    mbus->unregister_listener(reng);
    HB_SAFE_DEREF(dman);
    HB_SAFE_DEREF(reng);
    HB_SAFE_DEREF(dbus);
    HB_SAFE_DEREF(mbus);

    return 0;
}

