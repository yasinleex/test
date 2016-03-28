LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS) 

LOCAL_MODULE := hdmi-ioctl

LOCAL_SHARED_LIBRARIES := libc

LOCAL_SRC_FILES += \
		hdmi-ioctl.c	

include $(BUILD_EXECUTABLE)		
