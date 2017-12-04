#include "chb_analytic_proxy.h"
#include "chb_devana_man.h"
#include "chb_device_proxy.h"
#include "ezxml.h"

chb_devana_manager::chb_devana_manager(ihb_data_bus *dbus, ihb_msg_bus *mbus) :
        m_dbus(dbus), m_mbus(mbus) {

    if (m_dbus) {
        m_dbus->ref();
    }

    if (m_mbus) {
        m_mbus->ref();
    }

    pthread_mutex_init(&m_mutex, NULL);
}

chb_devana_manager::~chb_devana_manager() {

    clean_devanas();

    HB_SAFE_DEREF(m_dbus);
    HB_SAFE_DEREF(m_mbus);

    pthread_mutex_destroy(&m_mutex);
}

void chb_devana_manager::clean_devanas() {
    map<string, hb_dev_anas>::iterator pos;
    for (pos = m_devlist.begin(); pos != m_devlist.end(); ++pos) {

        // stop and disconnect analytics from device
        pos->second.dev->stop();
        hb_dev_desc desc;
        pos->second.dev->get_dev_desc(&desc);
        m_dbus->unregister_sender(desc.id);

        for (size_t i = 0; i < pos->second.anas.size(); i++) {
            m_mbus->unregister_listener(pos->second.anas[i]);
            pos->second.anas[i]->deref();
        }
        pos->second.dev->deref();
    }
    m_devlist.clear();
}

hb_dev_type string_to_dev_type(string &type) {

    if (type == "video-source") {
        return hb_dev_type_video_source;
    } else if (type == "audio-source") {
        return hb_dev_type_audio_source;
    } else if (type == "video-sink") {
        return hb_dev_type_video_sink;
    } else if (type == "audio-sink") {
        return hb_dev_type_audio_sink;
    } else if (type == "generic") {
        return hb_dev_type_generic;
    }
    return hb_dev_type_none;
}

hb_result chb_devana_manager::load_config(string &cfg) {
    return load_config(cfg.c_str());
}

hb_result chb_devana_manager::load_config(const char * cfgxml) {

    ezxml_t root = ezxml_parse_file(cfgxml);
    if (!root) {
        HB_ERROR("failed to load configuration file\n");
        return hb_fail;
    }

    hb_result ret = hb_ok;
    const char *vattr = NULL;
    ezxml_t device = NULL;

    for (device = ezxml_child(root, "device"); device; device = device->next) {

        hb_dev_anas devanas;
        devanas.dev = NULL;

        string group, name, devtype, factory, uri;

        vattr = ezxml_attr(device, "group");
        if (!vattr) {
            HB_ERROR("failed to get group\n");
            ret = hb_fail;
            goto cleanup;
        }
        group = vattr;

        vattr = ezxml_attr(device, "name");
        if (!vattr) {
            HB_ERROR("failed to get name\n");
            ret = hb_fail;
            goto cleanup;
        }
        name = vattr;

        vattr = ezxml_attr(device, "devtype");
        if (!vattr) {
            HB_ERROR("failed to get devtype\n");
            ret = hb_fail;
            goto cleanup;
        }
        devtype = vattr;

        vattr = ezxml_attr(device, "factory");
        if (!vattr) {
            HB_ERROR("failed to get factory\n");
            ret = hb_fail;
            goto cleanup;
        }
        factory = vattr;

        vattr = ezxml_attr(device, "uri");
        if (!vattr) {
            HB_ERROR("failed to get uri\n");
            ret = hb_fail;
            goto cleanup;
        }
        uri = vattr;

        hb_dev_type devt = string_to_dev_type(devtype);
        if (devt == hb_dev_type_none) {
            HB_ERROR("failed to call string_to_dev_type\n");
            ret = hb_fail;
            goto cleanup;
        }

        HB_LOG("about to create device %s : %s\n", factory.c_str(), uri.c_str());

        chb_device_proxy *dev = new chb_device_proxy(factory, devt, uri.c_str(),
                m_dbus, m_mbus,
                NULL, ret);
        if (!dev || ret != hb_ok) {
            HB_ERROR("failed to new chb_device_proxy\n");
            HB_SAFE_DEREF(dev);
            goto cleanup;
        }

        HB_LOG("created device %s : %s\n", factory.c_str(), uri.c_str());

        dev->set_dev_name(name);
        dev->set_dev_group(group);

        hb_dev_desc devdesc;
        dev->get_dev_desc(&devdesc);
        devanas.dev = dev;

        HB_LOG("about to create analytics\n");

        ezxml_t analytic = ezxml_child(device, "analytic");
        if (analytic) {

            string anafactory;
            vattr = ezxml_attr(analytic, "factory");
            if (!vattr) {
                HB_ERROR("failed to get factory\n");
                ret = hb_fail;
                goto cleanup;
            }

            anafactory = vattr;
            HB_LOG("analytic is specified %s\n", anafactory.c_str());

            vector<ihb_data_analytic *> analist;
            hb_result ret = chb_data_analytic_proxy::create_data_analytics(
                    m_dbus, m_mbus, anafactory, analist);

            if (ret != hb_ok) {
                HB_ERROR(
                        "failed to create_data_analytics, but, let's continue\n");
            }

            HB_LOG("## ana number = %d\n", analist.size());

            for (size_t i = 0; i < analist.size(); i++) {

                HB_LOG("## dev datatype = %x, ana datatype = %x\n",
                        devdesc.datatype, analist[i]->get_data_type());
                if (analist[i]->get_data_type() == devdesc.datatype) {
                    HB_LOG("## register to data_bus\n");
                    m_dbus->register_listener(devdesc.id, analist[i]);

                    HB_LOG("## register to msg_bus\n");
                    m_mbus->register_listener(analist[i]);

                    HB_LOG("## push to list\n");
                    devanas.anas.push_back(analist[i]);
                    HB_LOG("## to start ana\n");

                    analist[i]->start();
                } else {
                    HB_LOG("## datatype does not match, deref this ana\n");
                    analist[i]->deref();
                }
            }
        }

        pair<string, hb_dev_anas> pair;
        pair.first = devdesc.id;
        pair.second = devanas;
        m_devlist.insert(pair);

        devanas.dev->start();
    }

    cleanup: ezxml_free(root);

    return ret;

}

hb_result chb_devana_manager::get_dev_descs(vector<hb_dev_desc> &descs) {

    chb_auto_lock lock(&m_mutex);
    descs.clear();
    map<string, hb_dev_anas>::iterator pos;
    for (pos = m_devlist.begin(); pos != m_devlist.end(); ++pos) {
        hb_dev_desc desc;
        pos->second.dev->get_dev_desc(&desc);
        descs.push_back(desc);
    }

    return hb_ok;
}

hb_result chb_devana_manager::get_dev_desc_byid(string &devid,
        hb_dev_desc &desc) {
    chb_auto_lock lock(&m_mutex);
    map<string, hb_dev_anas>::iterator pos;
    pos = m_devlist.find(devid);
    if (pos != m_devlist.end()) {
        pos->second.dev->get_dev_desc(&desc);
        return hb_ok;
    }

    HB_ERROR("failed to get_devdesc_byid\n");
    return hb_fail;
}

hb_result chb_devana_manager::get_dev_byid(string &devid, ihb_dev** dev) {

    if (!dev) {
        HB_ERROR("Invalid parameter\n");
        return hb_invalid_parameter;
    }

    chb_auto_lock lock(&m_mutex);
    map<string, hb_dev_anas>::iterator pos;
    pos = m_devlist.find(devid);
    if (pos != m_devlist.end()) {
        *dev = pos->second.dev;
        pos->second.dev->ref();
        return hb_ok;
    }

    HB_ERROR("failed to dev\n");
    return hb_fail;
}

