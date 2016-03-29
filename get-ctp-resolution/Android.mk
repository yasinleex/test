LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	get-ctp-resolution.c

LOCAL_SHARED_LIBRARIES := \
    libutils

LOCAL_MODULE := get-ctp-resolution
LOCAL_MODULE_TAGS := debug

include $(BUILD_EXECUTABLE)
