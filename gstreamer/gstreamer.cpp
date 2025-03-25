/*
 * This file is part of CasparCG (www.casparcg.com).
 *
 * CasparCG is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CasparCG is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CasparCG. If not, see <http://www.gnu.org/licenses/>.
 */

#include "StdAfx.h"

#include "gstreamer.h"

#include "consumer/gstreamer_consumer.h"
#include "producer/gstreamer_producer.h"

#include <common/log.h>

#include <core/module_dependencies.h>
#include <core/consumer/frame_consumer.h>

#include <mutex>

namespace caspar { namespace gstreamer {

static void gst_debug_log_callback(GstDebugCategory* category, GstDebugLevel level,
                                  const gchar* file, const gchar* function,
                                  gint line, GObject* object, GstDebugMessage* message,
                                  gpointer user_data)
{
    // Filter out too verbose messages
    if (object && GST_IS_MESSAGE(object)) {
        GstObject* source = GST_MESSAGE_SRC(GST_MESSAGE_CAST(object));
        if (source && GST_OBJECT_NAME(source) && 
            g_strcmp0(GST_OBJECT_NAME(source), "fakesink") == 0) {
            if (level < GST_LEVEL_WARNING) {
                return;
            }
        }
    }
    
    // Map GStreamer debug levels to CasparCG log levels
    const char* message_str = gst_debug_message_get(message);
    const char* category_name = category ? gst_debug_category_get_name(category) : "unknown";
    
    // Construct a more detailed message including category and location
    std::string detailed_msg = std::string(category_name) + " - ";
    
    // Add file and line information for higher debug levels
    if (level <= GST_LEVEL_DEBUG) {
        std::string file_str = file ? file : "unknown";
        size_t pos = file_str.find_last_of("/\\");
        if (pos != std::string::npos) {
            file_str = file_str.substr(pos + 1);
        }
        detailed_msg += file_str + ":" + std::to_string(line) + " - ";
    }
    
    detailed_msg += message_str;
    
    switch (level) {
        case GST_LEVEL_ERROR:
            CASPAR_LOG(error) << L"[gstreamer] " << detailed_msg;
            break;
        case GST_LEVEL_WARNING:
            CASPAR_LOG(warning) << L"[gstreamer] " << detailed_msg;
            break;
        case GST_LEVEL_INFO:
            CASPAR_LOG(info) << L"[gstreamer] " << detailed_msg;
            break;
        case GST_LEVEL_DEBUG:
            CASPAR_LOG(debug) << L"[gstreamer] " << detailed_msg;
            break;
        case GST_LEVEL_LOG:
        case GST_LEVEL_TRACE:
        default:
            CASPAR_LOG(trace) << L"[gstreamer] " << detailed_msg;
            break;
    }
}

void init(const core::module_dependencies& dependencies)
{
    CASPAR_LOG(info) << L"Initializing GStreamer module...";
    
    // Initialize GStreamer
    GError* error = nullptr;
    if (!gst_init_check(nullptr, nullptr, &error)) {
        CASPAR_LOG(error) << L"Failed to initialize GStreamer: " << error->message;
        g_error_free(error);
        return;
    }
    
    // Set up debug logging
    gst_debug_remove_log_function(gst_debug_log_default);
    gst_debug_add_log_function(gst_debug_log_callback, nullptr, nullptr);
    
    // Set default debug level (can be overridden by GST_DEBUG env var)
    int debug_level = 2;  // Default debug level
    
    // Get configuration from environment directly to avoid property_tree issues
    const wchar_t* gst_debug_level = _wgetenv(L"CASPARCG_GST_DEBUG_LEVEL");
    if (gst_debug_level) {
        try {
            debug_level = std::stoi(gst_debug_level);
        } catch (...) {
            // Ignore conversion errors and use default
        }
    }
    
    gst_debug_set_default_threshold(static_cast<GstDebugLevel>(debug_level));
    
    // Log GStreamer version and plugins
    CASPAR_LOG(info) << L"GStreamer initialized, version: " << GST_VERSION_MAJOR << "." 
                     << GST_VERSION_MINOR << "." << GST_VERSION_MICRO;
    
    // Check for critical plugins
    std::vector<const char*> required_plugins = {"playbin", "decodebin", "appsink"};
    bool missing_plugins = false;
    
    for (const auto& plugin_name : required_plugins) {
        GstPlugin* plugin = gst_registry_find_plugin(gst_registry_get(), plugin_name);
        if (!plugin) {
            CASPAR_LOG(error) << L"Required GStreamer plugin not found: " << plugin_name;
            missing_plugins = true;
        } else {
            gst_object_unref(plugin);
        }
    }
    
    if (missing_plugins) {
        CASPAR_LOG(warning) << L"Some required GStreamer plugins are missing. The GStreamer module may not function correctly.";
    }

    // Register regular consumers
    dependencies.consumer_registry->register_consumer_factory(L"GStreamer Consumer", create_consumer);
    dependencies.consumer_registry->register_preconfigured_consumer_factory(L"gstreamer", create_preconfigured_consumer);
    
    // Register GStreamer-specific command consumers
    dependencies.consumer_registry->register_consumer_factory(L"GSADD", create_consumer);
    dependencies.consumer_registry->register_consumer_factory(L"GSFILE", create_consumer);
    
    // Register producer
    dependencies.producer_registry->register_producer_factory(L"GStreamer Producer", create_producer);
    dependencies.producer_registry->register_producer_factory(L"GSTREAMER_PRODUCER", create_producer);
    
    CASPAR_LOG(info) << L"GStreamer module initialized successfully";
}

void uninit()
{
    CASPAR_LOG(info) << L"Uninitializing GStreamer module";
    gst_debug_remove_log_function(gst_debug_log_callback);
    gst_deinit();
    CASPAR_LOG(info) << L"GStreamer module uninitialized";
}

}} // namespace caspar::gstreamer