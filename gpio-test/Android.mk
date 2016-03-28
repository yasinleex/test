LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS) 

LOCAL_MODULE := gpio-test

LOCAL_SHARED_LIBRARIES := libc

LOCAL_SRC_FILES += \
		gpio-test.c

include $(BUILD_EXECUTABLE)		
