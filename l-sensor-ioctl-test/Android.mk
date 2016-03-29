LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	l-sensor-ioctl.c

LOCAL_SHARED_LIBRARIES := \
    libutils

LOCAL_MODULE := l-sensor-ioctl
LOCAL_MODULE_TAGS := debug

include $(BUILD_EXECUTABLE)
