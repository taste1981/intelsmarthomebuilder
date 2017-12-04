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

#ifndef HBBASEDEF_H_
#define HBBASEDEF_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <string>
#include <vector>

using namespace std;

#define HB_LOG_ERROR   0
#define HB_LOG_NORMAL  1
#define HB_LOG_VERBOSE 2

#define HB_LOG_LEVEL 2

#if HB_LOG_LEVEL >= HB_LOG_ERROR
#define HB_ERROR(format, ...) fprintf(stderr, "\x1B[31m"   "%s: %d: ---> " format  "\x1B[0m", __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__);
#else
#define HB_ERROR(format, ...)
}
#endif

#if IAVS_LOG_LEVEL >= IAVS_LOG_NORMAL
#define HB_LOG(format, ...) printf("%s: %d: ---> " format, __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__);
#else
#define HB_LOG(format, ...)
#endif

#if IAVS_LOG_LEVEL >= IAVS_LOG_VERBOSE
#define HB_VERBOSE(format, ...) printf("%s: %d: ---> " format, __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__);
#else
#define HB_VERBOSE(format, ...)
#endif

#define HB_SAFE_DELETE(p) if(p) { delete p; p = NULL; }
#define HB_SAFE_DEREF(p) if(p) { p->deref(); p = NULL; }
#define HB_SAFE_REF(p) if(p) { p->ref(); }
#define HB_MAX_IMAGE_PLANE 8

/// home builder API return codes
typedef enum _hb_result {

    hb_ok = 0,

    hb_fail,
    hb_busy,
    hb_eof,

    hb_no_audio,
    hb_no_video,

    hb_fail_access_file,
    hb_not_supported,
    hb_not_implemented,
    hb_not_initialized,
    hb_insufficient_memory,
    hb_invalid_parameter,
    hb_already_initialized,

} hb_result;

/// continuous data, different from attributes
typedef enum _hb_data_type {
    hb_data_type_none = -1,

    hb_data_type_video,    	// continuous data
    hb_data_type_audio,    	// continuous data
    hb_data_type_generic,

    hb_data_type_user = 0x1000,

} hb_data_type;

typedef enum _hb_video_type {
    hb_video_type_none = -1,

    // uncompressed
    hb_video_type_rgb24,
    hb_video_type_bgr24,
    hb_video_type_rgb32,
    hb_video_type_YUY2,
    hb_video_type_gray,

    // compressed
    hb_video_type_mjpeg,
    hb_video_type_h264,

} hb_video_type;

/// Audio codec types
typedef enum _hb_audio_type {
    hb_audio_type_none = -1,

    // uncompressed
    hb_audio_type_pcm,

    // compressed
    hb_audio_type_aac,

} hb_audio_type;

/// Video format
typedef struct _hb_video_fmt {
    hb_video_type type;
    int width;
    int height;
    int frame_rate_n;
    int frame_rate_d;

} hb_video_fmt;

/// Audio format
typedef struct _hb_audio_fmt {
    hb_audio_type type;
    int channels;
    int samples_per_sec;
    int bits_per_sample;
    int block_align;
    int avg_bytes_per_sec;

} hb_audio_fmt;

/// Video data description
typedef struct _hb_video_data_desc {
    hb_video_type type;

    int width;
    int height;

    // for compressed and uncompressed data
    uint8_t *data[HB_MAX_IMAGE_PLANE];

    // for uncompressed data
    int stride[HB_MAX_IMAGE_PLANE];
    int pixel_depth; // TODO: use it for future

    // for compressed video data
    int size;

} hb_video_data_desc;

/// Audio data description
typedef struct _hb_audio_data_desc {
    hb_audio_type type;
    int channels;
    int samples_per_sec;
    int bits_per_sample;
    int length;
    uint8_t *data;

} hb_audio_data_desc;

/// Home builder obj base class.
class ihb_obj {
public:
    virtual ~ihb_obj() {
    }
	/// Add ref to the object
    virtual void ref() = 0
	/// Decrease ref to the object
    virtual void deref() = 0;
};

/// Video data buffer
class ihb_videodata_buffer: public virtual ihb_obj {
public:
    ~ihb_videodata_buffer() {
    }
	/**
	 @brief Get the desciption of the video data buffer.
	 @details This API returns the desription of the data within the
	 buffer.
	 @param desc the output description.
	 @return hb_ok if successfully get the descripiton; other if failed.
	 */
    virtual hb_result get_data(hb_video_data_desc* desc) = 0;
};

/// Reference counted audio buffer
class ihb_audiodata_buffer: public virtual ihb_obj {
public:
    ~ihb_audiodata_buffer() {
    }
    virtual hb_result get_data(hb_audio_data_desc* desc) = 0;
};

/// callback for data 
class ihb_data_cb: public virtual ihb_obj {
public:
    virtual ~ihb_data_cb() {
    }
    /// Triggered when there is data incoming.
    virtual void on_data(string &sid, uint64_t timestamp, hb_result result,
            hb_data_type type, ihb_obj *obj) = 0;
    /// Get the thread id the callback is invoked.
    virtual const string & get_tid() = 0;
};

/// callback for message
class ihb_msg_cb: public virtual ihb_obj {
public:
    virtual ~ihb_msg_cb() {
    }
	/// Triggered when there is message incoming
    virtual void on_msg(string &sid, uint64_t timestamp, hb_result result,
            string &msg) = 0;
    /// Get the thread id the callback is invoked.
    virtual const string & get_tid() = 0;
};

/// Data sender object
class ihb_data_sender: public virtual ihb_obj {
public:
    virtual ~ihb_data_sender() {
    }
	/// send data to provided object.
    virtual hb_result send_data(uint64_t timestamp, hb_result result,
            hb_data_type type, ihb_obj *obj, bool post) = 0;
};

/// home builder data bus class
class ihb_data_bus: public virtual ihb_obj {
public:
    virtual ~ihb_data_bus() {
    }
	/**
	 @brief register a sender to the data bus.
	 @details This API registers a data sender on the data bus, associated with 
	 a unique sender id.
	 @param sid the unique sender ID.
	 @param snd the handle of sender object.
	 @return hb_ok if successfully registered. Other if failed.
	 */
    virtual hb_result register_sender(string &sid, ihb_data_sender** snd) = 0;
    /**
	 @brief unregister a sender to the data bus.
	 @details This API unregisteres a data sender on the data bus.
	 @param sid the id of the sender to be unregistered.
	 @return hb_ok if successfully unregistered. Other if failed.
	 */
    virtual hb_result unregister_sender(string &sid) = 0;
    /**
	 @brief register a data listener on the data bus.
	 @details This API registers a data listener on the data bus.
	 @param sid The id of the sender to be listend.
	 @param cb the data callback object.
	 @return hb_ok if successfully registered. Other if failed.
	 */
    virtual hb_result register_listener(string &sid, ihb_data_cb* cb) = 0;
	/**
	 @brief unreigster a data listener on the data bus.
	 @details. This API detaches a data listener on the data bus.
	 @param sid the id of the sender.
	 @param cb the handle of the data callback object.
	 @return hb_ok if successfully detached. Other if failed.
	 */
    virtual hb_result unregister_listener(string &sid, ihb_data_cb* cb) = 0;
};

/// Message sender object
class ihb_msg_sender: public virtual ihb_obj {
public:
    virtual ~ihb_msg_sender() {
    }
	/// sender a message to an object with provided id.
    virtual hb_result send_msg(string &oid, string &tid, uint64_t timestamp,
            hb_result result, string &msg, bool post) = 0;
};

/// message bus object.
class ihb_msg_busN: public virtual ihb_obj {
public:
    virtual ~ihb_msg_busN() {
    }
	/**
	 @brief register a sender to the messsage bus.
	 @details This API registers a message sender on the message bus, associated with 
	 a unique sender id.
	 @param sid the unique sender ID.
	 @param snd the handle of sender object.
	 @return hb_ok if successfully registered. Other if failed.
	*/
    virtual hb_result register_sender(string &sid, ihb_msg_sender** snd) = 0;
    /**
	 @brief unregister a sender to the message bus.
	 @details This API unregisteres a message sender on the message bus.
	 @param sid the id of the sender to be unregistered.
	 @return hb_ok if successfully unregistered. Other if failed.
	 */
    virtual hb_result unregister_sender(string &sid) = 0;
    /**
	 @brief register a message listener on the message bus.
	 @details This API registers a message listener on the message bus.
	 @param sid The id of the sender to be listend.
	 @param cb the message callback object.
	 @return hb_ok if successfully registered. Other if failed.
	 */
    virtual hb_result register_listener(string &sid, ihb_msg_cb* cb) = 0;
	/**
	 @brief unreigster a data listener on the message bus.
	 @details. This API detaches a message listener on the message bus.
	 @param sid the id of the sender.
	 @param cb the handle of the message callback object.
	 @return hb_ok if successfully detached. Other if failed.
	 */
    virtual hb_result unregister_listener(string &sid, ihb_msg_cb* cb) = 0;
};

/// Deprecated. Please use the message bus class above.
class ihb_msg_bus: public virtual ihb_obj {
public:
    virtual ~ihb_msg_bus() {
    }
    virtual hb_result register_sender(ihb_msg_sender** snd) = 0;
    virtual hb_result register_listener(ihb_msg_cb* cb) = 0;
    virtual hb_result unregister_listener(ihb_msg_cb* cb) = 0;
};

/// Video/audio analytics class. It takes video/audio for analytics,
/// and outputs message with registered message callback.
class ihb_data_analytic: public ihb_data_cb, public ihb_msg_cb {
public:
    virtual ~ihb_data_analytic() {
    }
	/**
	 @breif get the data type the analytics is processing.
	 @details Get the data type the analytics object is processing.
	 it can be video/audio.
	 @return the data type current object supports.
	 */
    virtual hb_data_type get_data_type() = 0;
	/// starts the analytics engine.
    virtual hb_result start() = 0;
	/// stops the analytics engine.
    virtual hb_result stop() = 0;
};

typedef int (*hb_get_analytic_count_proc)();

typedef hb_result (*hb_create_analytic_instance_proc)(int index,
        ihb_data_bus *dbus, ihb_msg_bus *mbus, ihb_data_analytic **obj);

/// Home builder device types.
typedef enum _hb_dev_type {

    hb_dev_type_none = -1,

    hb_dev_type_video_source, // video file, camera
    hb_dev_type_audio_source, // audio file, microphone
    hb_dev_type_video_sink,   //
    hb_dev_type_audio_sink,   // such as speaker
    hb_dev_type_generic,      // no data A/V input/output

} hb_dev_type;

/// Device descritpion.
typedef struct _hb_dev_desc {

    hb_dev_type devtype;
    hb_data_type datatype;

    string id;
    string uri;

    string name;
    string group;

} hb_dev_desc;

/// Device status
typedef enum _hb_dev_status {

    hb_dev_status_running,
    hb_dev_status_stopped,
    hb_dev_status_lost,
    hb_dev_status_unknown,

} hb_dev_status;

/// Home bulder device definition
class ihb_dev: public virtual ihb_obj {
public:
    virtual ~ihb_dev() {
    }

    /**
	 @brief this get the device attributes 
	 @details. By calling this API, the xml format device attribute string
	 will be returned.
	 @param attributes the output device attribute XML string.
	 @return hb_ok if successfully the the attribute. Other if failed.
	*/
    virtual hb_result get_attr_descs(string &attributes) = 0;
    /**
	 @breif get one attribute of the device.
	 @details Providing an attribute name and get the attribute value.
	 @param name attribute name
	 @param value returned value.
	 @return hb_ok if successfully get the attribute value. Other if failed.
	*/
    virtual hb_result get_attr_value(string &name, string &value) = 0;
	/**
	 @brief set one attribute of the device.
	 @details Setting an attribute on the device.
	 @param name attribute name
	 @param value attribute value to be set.
	 @return hb_ok if successfully set the attribute value. Other if failed.
	*/
    virtual hb_result set_attr_value(string &name, string &value) = 0;
    /**
	 @brief get the device description.
	 @details Getting the device's description.
	 @param desc the output description of the device.
	 @return hb_ok if successfully get the description. Other if failed.
	*/
    virtual hb_result get_dev_desc(hb_dev_desc *desc) = 0;
	/**
	 @brief get the device status
	 @details Getting the devices status.
	 @param status the returned status
	 @return hb_ok if successfully get the status. Other if failed.
	*/
    virtual hb_result get_dev_status(hb_dev_status &status) = 0;
    /**
	 @brief Get the device group.
	 @details Getting the group current device belongs to.
	 @param group the output group name.
	 @return hb_ok if successfully get the group name. Other if failed.
	*/
    virtual hb_result set_dev_group(string &group) = 0;
	/**
	 @brief Set the device name.
	 @details Setting the device's name
	 @param the name of the device to be set.
	 @return hb_ok if successfully set the name. Other if failed.
	*/
    virtual hb_result set_dev_name(string &name) = 0;

    /// Start the deivce for continuous streams
    virtual hb_result start() = 0;
	/// Stop the device for continous streams.
    virtual hb_result stop() = 0;
};

/// options for source device.
typedef struct _hb_source_device_option {
    bool post;
    uint16_t repeat;
} hb_source_device_option;

typedef hb_result (*hb_create_device_instance_proc)(ihb_data_bus *dbus,
        ihb_msg_bus *mbus, hb_dev_type devtype, const char *uri, void *option,
        ihb_dev **obj);

/// Device manager
class ihb_dev_manager: public virtual ihb_obj {
public:
    virtual ~ihb_dev_manager() {
    }
    /**
	 @brief load configure for the device manager.
	 @details This loads the config file and configures the devices.
	 @param cfg_path the path of the config file.
	 @return hb_ok if successfully loaded. Other if failed.
	*/
    virtual hb_result load_config(string &cfg_path) = 0;
	/**
	 @brief get the description of devices.
	 @details Returns a vector of devices' description.
	 @param descs the vector for outputting devices' descriptions.
	 @return hb_ok if successfully get the description. Other if failed.
	*/
    virtual hb_result get_dev_descs(vector<hb_dev_desc> &descs) = 0;
	/**
	 @brief get the device object by its id.
	 @details Get the device's handle by its id.
	 @param devid the device id.
	 @param dev the returned device handle.
	 @return hb_ok if successfully get the device handle. Other if failed.
	*/
    virtual hb_result get_dev_byid(string &devid, ihb_dev **dev) = 0;
	/**
	 @brief get the device's description by its id.
	 @details Get the device descrition by is id.
	 @param devid the device id.
	 @param desc the device's description.
	 @return hb_ok if successfully get the device description. Other if failed.
	 */
    virtual hb_result get_dev_desc_byid(string &devid, hb_dev_desc &desc) = 0;

};

/// Rule engine.
class ihb_rule_engine: public virtual ihb_msg_cb {
public:
    virtual ~ihb_rule_engine() {
    }
    /// load rules from a file specified by the path.
    virtual hb_result load_rules(string &rules_path) = 0;
};

typedef hb_result (*hb_create_rule_engine_instance_proc)(string &ahpath,
        ihb_data_bus *dbus, ihb_msg_bus *mbus, ihb_dev_manager *man,
        ihb_rule_engine **obj);

#define HB_MSG_BROADCAST   "4ED71700-EFE3-416D-BC29-B7519A65B534"
#define HB_MSG_MULTICAST   "ED361FEC-79C8-4F18-823B-A099BD674767"
#define HB_MESSAGE_BUS     "A9C5E14B-1745-4500-97A5-42A2E3C51FA4"
#define HB_RULE_ENGINE     "9B1CEC59-0A67-4F6B-A9C0-5B7DC694B751"

#endif /* HBBASEDEF_H_ */
