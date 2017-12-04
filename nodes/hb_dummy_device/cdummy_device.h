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


#ifndef CDUMMY_DEVICE_H_
#define CDUMMY_DEVICE_H_

#include "hbutils.h"

class cdummy_device: public ihb_dev, public chb_obj {

    ihb_data_bus* m_data_bus;
    ihb_msg_bus * m_msg_bus;

    bool m_bstarted;

    hb_dev_desc m_devdesc;

protected:

    pthread_mutex_t m_mutex;

public:
    cdummy_device(ihb_data_bus* data_bus, ihb_msg_bus * msg_bus,
            const char *uri, hb_result &result);
    virtual ~cdummy_device();

    hb_result get_attr_descs(string &attributes);

    hb_result get_attr_value(string &name, string &value);
    hb_result set_attr_value(string &name, string &value);

    hb_result get_dev_desc(hb_dev_desc *desc);
    hb_result get_dev_status(hb_dev_status &status);

    hb_result set_dev_group(string &group);
    hb_result set_dev_name(string &name);

    // for continuous streams
    hb_result start();
    hb_result stop();
};

#endif /* CDUMMY_DEVICE_H_ */
