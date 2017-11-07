##
# Build for demo
#

include $(CLEAR_VAR)

MODULE_PATH := $(BASE_DIR)/examples/dcs3.0-demo

LOCAL_MODULE := dcs3-linux-demo

LOCAL_STATIC_LIBRARIES :=  connagent coap dcs \
                          voice_engine ntp port-linux \
                          framework nsdl speex mbedtls cjson

LOCAL_SRC_FILES := \
    $(MODULE_PATH)/duerapp_kbd_listener.c \
    $(MODULE_PATH)/duerapp.c \
    $(MODULE_PATH)/event.c \
    $(MODULE_PATH)/queue.c \
    $(MODULE_PATH)/duerapp_profile_config.c \
		$(MODULE_PATH)/duerapp_recorder.c \
		$(MODULE_PATH)/duerapp_media.c


#    $(MODULE_PATH)/duerapp_aes_test.c \
#    $(MODULE_PATH)/duerapp.c \


LOCAL_INCLUDES += $(BASE_DIR)/platform/include

LOCAL_INCLUDES += /usr/include/gstreamer-1.0 \
				/usr/include/glib-2.0/ \
				/usr/lib/i386-linux-gnu/glib-2.0/include \
				/usr/lib/i386-linux-gnu/gstreamer-1.0/include

LOCAL_LDFLAGS := -lm \
				-lrt \
				-lasound \
				-pthread \
				-lgstreamer-1.0 \
				-lgobject-2.0 \
				-lglib-2.0
#								 $(pkg-config --cflags --libs gstreamer-1.0)
#$(error $(LOCAL_INCLUDES))
include $(BUILD_EXECUTABLE)
