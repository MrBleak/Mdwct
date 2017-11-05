##
# Build for demo
#

include $(CLEAR_VAR)

MODULE_PATH := $(BASE_DIR)/examples/linux

LOCAL_MODULE := dcs3-linux-demo

LOCAL_STATIC_LIBRARIES :=  connagent coap \
                          voice_engine ntp port-linux \
                          framework nsdl speex mbedtls cjson

LOCAL_SRC_FILES := \
    $(MODULE_PATH)/duerapp_args.c \
    $(MODULE_PATH)/duerapp.c \
    $(MODULE_PATH)/duerapp_recorder.c \
    $(MODULE_PATH)/duerapp_profile_config.c

#    $(MODULE_PATH)/duerapp_aes_test.c \
#    $(MODULE_PATH)/duerapp.c \

LOCAL_INCLUDES += $(BASE_DIR)/platform/include

LOCAL_LDFLAGS := -lm \
								 -lrt \
								 -lpthread \
								 -lasound \
								 -pthread \
 								 -I/usr/include/gstreamer-1.0 \
								 -I/usr/lib/i386-linux-gnu/gstreamer-1.0/include \
								 -I/usr/include/glib-2.0 \
								 -I/usr/lib/i386-linux-gnu/glib-2.0/include \
								 -lgstreamer-1.0 \
								 -lgobject-2.0 \
								 -lglib-2.0

include $(BUILD_EXECUTABLE)
