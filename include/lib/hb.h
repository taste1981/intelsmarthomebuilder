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

#ifndef HB_H_
#define HB_H_

#include <vector>
#include "hbdef.h"

/**
 @brief Create the data bus instance
 @details Applications that uses data bus invoke this
 method to create a data bus that is shared by modules.
 @param dbus the data bus handle that is created. 
 @return hb_ok if successfully created. other code if failed
 to create the data bus.
 */
hb_result hb_create_data_bus(
        ihb_data_bus **dbus
        );
		
/**
 @brief Create the message bus instance
 @details Applications that uses message bus invoke this
 method to create a message bus that is shared by modules.
 @param mbus the message bus handle that is created. 
 @return hb_ok if successfully created. other code if failed
 to create the message bus.
 */
hb_result hb_create_msg_bus(
        ihb_msg_bus **mbus
        );

/**
 @brief Create the device manager instance.
 @details Device manager manages the devices that's registered.
 @param dbus the data bus used by the device manager.
 @param mbus the message bus used by the device manager.
 @param man the manager instance created. 
 @return hb_ok if successfully created. other code if failed
 to create the message bus.
 */
hb_result hb_create_dev_manager(
        ihb_data_bus * dbus,
        ihb_msg_bus * mbus,
        ihb_dev_manager **man
        );

/**
 @brief Create the rule engine instance.
 @details Rule engine provides the mapping of events to actions.
 @param repath the rule engine lib path.
 @param ahpath the action handler lib path.
 @param dbus the data bus used by the device manager.
 @param mbus the message bus used by the device manager.
 @param man the manager instance. 
 @param obj the rule engine instance created.
 @return hb_ok if successfully created. other code if failed
 to create the message bus.
 */
hb_result hb_create_rule_engine(
        string &repath,
        string &ahpath,
        ihb_data_bus *dbus,
        ihb_msg_bus *mbus,
        ihb_dev_manager *man,
        ihb_rule_engine **obj
        );

#endif /* HB_H_ */
