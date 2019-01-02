LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	show.c

LOCAL_SHARED_LIBRARIES := \
    libutils

LOCAL_MODULE := show
LOCAL_MODULE_TAGS := debug

include $(BUILD_EXECUTABLE)
