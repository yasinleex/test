LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := uvc-camera

LOCAL_MODULE_TAGS := tests

LOCAL_SRC_FILES := \
    uvc-camera.c \
    
LOCAL_SHARED_LIBRARIES := \

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/ \
               
include $(BUILD_EXECUTABLE)
