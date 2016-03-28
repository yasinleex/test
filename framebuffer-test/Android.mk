LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS) 

LOCAL_MODULE := fb-test

LOCAL_SHARED_LIBRARIES := libc

LOCAL_SRC_FILES += \
		fb0.c	

include $(BUILD_EXECUTABLE)		
