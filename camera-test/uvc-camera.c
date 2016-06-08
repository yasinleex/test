#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <linux/videodev2.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

/**********************************/
static int debug_level = 1;

#define ENTER \
do{ if(debug_level >= 1) printf("[%s_DBG ][%04d] [%s]\n", __FILE__, __LINE__, __func__); }while(0)

#define PRINT_DBG(format,x...) \
do{ if(debug_level >= 1) printf("[%s_DBG ][%04d] " format, __FILE__, __LINE__, ## x); }while(0)

#define PRINT_INFO(format,x...) \
do{ printf("[%s_INFO][%04d] " format, __FILE__, __LINE__, ## x); }while(0)

#define PRINT_WARN(format,x...) \
do{ printf("[%s_WARN][%04d] " format, __FILE__, __LINE__, ## x); }while(0)

#define PRINT_ERR(format,x...) \
do{ printf("[%s_ERR ][%04d] [%s] " format, __FILE__, __LINE__, __func__, ## x); }while(0)
/**********************************/

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define CLEAR(x) memset(&(x), 0, sizeof (x))

#define NUM_OF_FRAME_TO_GET 20
#define MAX_PREVIEW_BUF_COUNT 6
#define UVC_CAM_NAME "/dev/video0"
#define FILE_PATH "./picture_got/"

char *ISP_buf[MAX_PREVIEW_BUF_COUNT] = {NULL};

enum {
    V4L2_POLL_ERROR = -1,
    V4L2_POLL_TIMEOUT = -2,
};

struct v4l2_buffer v4l2_buf;
struct pollfd m_events_c;
pthread_t preview_thd;

char *mISPBufferBase = 0;
int uvc_cam_fd = -1;
int m_cam_index = 0;
int m_preview_width = 320;
int m_preview_height = 240;
int m_preview_buffer_cnt = MAX_PREVIEW_BUF_COUNT;
int m_preview_fmt = V4L2_PIX_FMT_YUYV;
int m_preview_fps = 30;

static int get_pixel_depth(unsigned int fmt)
{
    int depth = 0;

    switch (fmt) {
    case V4L2_PIX_FMT_NV12:
        depth = 12;
        break;
    case V4L2_PIX_FMT_NV21:
        depth = 12;
        break;
    case V4L2_PIX_FMT_YUV420:
        depth = 12;
        break;

    case V4L2_PIX_FMT_RGB565:
    case V4L2_PIX_FMT_YUYV:
    case V4L2_PIX_FMT_YVYU:
    case V4L2_PIX_FMT_UYVY:
    case V4L2_PIX_FMT_VYUY:
    case V4L2_PIX_FMT_NV16:
    case V4L2_PIX_FMT_NV61:
    case V4L2_PIX_FMT_YUV422P:
        depth = 16;
        break;

    case V4L2_PIX_FMT_RGB32:
        depth = 32;
        break;
    }
    //printf("@@@depth=%d\n",depth);
    return depth;
}

static int getPreviewFrameSize()
{
    int frame_size = (m_preview_width * m_preview_height * get_pixel_depth(m_preview_fmt)) / 8;
    return frame_size;
}

static int v4l2_querycap(int fd)
{
    struct v4l2_capability capability;
    int ret = -1;

    memset(&capability, 0, sizeof(struct v4l2_capability));

    ret = ioctl(fd, VIDIOC_QUERYCAP, &capability);
    if (ret < 0) {
        PRINT_ERR("VIDIOC_QUERYCAP failed, ret=%d\n", ret);
        return ret;
    } else {
        PRINT_INFO("VIDIOC_QUERYCAP success\n");
    }

    PRINT_INFO("capability.driver=\"%s\"\n", capability.driver);
    PRINT_INFO("capability.card=\"%s\"\n", capability.card);
    PRINT_INFO("capability.bus_info=\"%s\"\n", capability.bus_info);
    PRINT_INFO("capability.version=\"%d.%d.%d\"\n", 
        (unsigned char)(capability.version>>16),
        (unsigned char)(capability.version>>8),
        (unsigned char)(capability.version));
    PRINT_INFO("capability.capabilities=0x%08X\n", capability.capabilities);
    PRINT_INFO("capability.device_caps=0x%08X\n", capability.device_caps);

    if (!(capability.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        PRINT_ERR("this is Not a video capture device\n");
        return -1;
    } else {
        PRINT_INFO("this is a video capture device\n");
        return ret;
    }
}

/*
应用程序可以通过 VIDIOC_ENUMINPUT 和 VIDIOC_ENUMOUTPUT 来查看有效输入和输出
的数量和属性。VIDIOC_ENUMINPUT 会返回v4l2_input结构体，也会返回被查询的视频输
入设备的状态信息。
*/
static const __u8* v4l2_enuminput(int fd, int index)
{
    static struct v4l2_input input;
    input.index = index;

    if (ioctl(fd, VIDIOC_ENUMINPUT, &input) != 0) {
        PRINT_ERR("VIDIOC_ENUMINPUT failed, no matching input index found\n");
        return NULL;
    } else {
        PRINT_INFO("VIDIOC_ENUMINPUT success, input.index=%d, input.name=\"%s\"\n",
                   input.index, input.name);
        return input.name;
    }
}

static int v4l2_s_input(int fd, int index)
{
    struct v4l2_input input;
    int ret = -1;
    input.index = index;

    ret = ioctl(fd, VIDIOC_S_INPUT, &input);
    if (ret < 0) {
        PRINT_ERR("VIDIOC_S_INPUT failed, ret=%d\n", ret);
        return ret;
    } else {
        PRINT_INFO("VIDIOC_S_INPUT success\n");
        return ret;
    }
}

static int v4l2_cropcaps(int fd, struct v4l2_cropcap *cap)
{
    int ret;

    ret = ioctl(fd, VIDIOC_CROPCAP, cap);
    if (ret < 0) {
        printf("VIDIOC_CROPCAP failed %d.\n", ret);
        return -1;
    }

    return 0;
}

static int v4l2_enum_fmt(int fd, enum v4l2_buf_type type, unsigned int fmt)
{
    struct v4l2_fmtdesc fmtdesc;
    int found = 0;

    fmtdesc.type = type;
    fmtdesc.index = 0;

    while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) == 0) {
        if (fmtdesc.pixelformat == fmt) {
            printf("passed fmt = %#x found pixel format[%d]: %s\n",
                   fmt, fmtdesc.index, fmtdesc.description);
            found = 1;
            break;
        }

        fmtdesc.index++;
    }

    if (!found) {
        printf("unsupported pixel format\n");
        return -1;
    }

    return 0;
}

static int v4l2_s_fmt(int fd, enum v4l2_buf_type type,
                      int width, int height, unsigned int fmt)
{
    struct v4l2_format v4l2_fmt;
    int ret;

    memset(&v4l2_fmt, 0, sizeof(v4l2_fmt));

    v4l2_fmt.type = type;
    v4l2_fmt.fmt.pix.width = width;
    v4l2_fmt.fmt.pix.height = height;
    v4l2_fmt.fmt.pix.pixelformat = fmt;
    v4l2_fmt.fmt.pix.sizeimage = (width * height * get_pixel_depth(fmt)) / 8;
    v4l2_fmt.fmt.pix.field = V4L2_FIELD_ANY;
    if (fmt == V4L2_PIX_FMT_JPEG)
        v4l2_fmt.fmt.pix.colorspace = V4L2_COLORSPACE_JPEG;

    /* Set up for capture */
    ret = ioctl(fd, VIDIOC_S_FMT, &v4l2_fmt);
    if (ret < 0) {
        printf("VIDIOC_S_FMT failed %d\n", ret);
        return -1;
    }

    return 0;
}

static int v4l2_s_parm(int fd, enum v4l2_buf_type type,
                       struct v4l2_streamparm *streamparm)
{
    int ret;

    streamparm->type = type;

    ret = ioctl(fd, VIDIOC_S_PARM, streamparm);
    if (ret < 0) {
        printf("VIDIOC_S_PARM failed %d\n", ret);
        return ret;
    }

    return 0;
}

static int v4l2_s_ctrl(int fd, unsigned int id, unsigned int value)
{
    struct v4l2_control ctrl;
    int ret;

    ctrl.id = id;
    ctrl.value = value;

    ret = ioctl(fd, VIDIOC_S_CTRL, &ctrl);
    if (ret < 0) {
        printf("VIDIOC_S_CTRL(id = %#x (%d), value = %d) failed %d\n",
               id, id - V4L2_CID_PRIVATE_BASE, value, ret);
        return ret;
    }

    return ctrl.value;
}

static int v4l2_reqbufs(int fd, enum v4l2_buf_type type, int nr_bufs)
{
    struct v4l2_requestbuffers req;
    int ret;

    req.count = nr_bufs;
    req.type = type;
    req.memory = V4L2_MEMORY_MMAP;

    ret = ioctl(fd, VIDIOC_REQBUFS, &req);
    if (ret < 0) {
        printf("VIDIOC_REQBUFS failed %d\n", ret);
        return -1;
    }

    return req.count;
}

static int v4l2_qbuf(int fd, enum v4l2_buf_type type, int index)
{
    struct v4l2_buffer v4l2_buf;
    int ret;

    v4l2_buf.type = type;
    v4l2_buf.memory = V4L2_MEMORY_MMAP;
    v4l2_buf.index = index;

    ret = ioctl(fd, VIDIOC_QBUF, &v4l2_buf);
    if (ret < 0) {
        printf("VIDIOC_QBUF failed %d\n", ret);
        return ret;
    }

    return 0;
}

static int v4l2_dqbuf(int fd, enum v4l2_buf_type type,
                      unsigned int *paddr)
{
    struct v4l2_buffer v4l2_buf;
    int ret;

    v4l2_buf.type = type;
    v4l2_buf.memory = V4L2_MEMORY_MMAP;

    ret = ioctl(fd, VIDIOC_DQBUF, &v4l2_buf);
    if (ret < 0) {
        printf("VIDIOC_DQBUF failed %d\n", ret);
        return ret;
    }

    if (paddr)
        *paddr = v4l2_buf.reserved;

    return v4l2_buf.index;
}

static int v4l2_streamon(int fd, enum v4l2_buf_type type)
{
    int ret;

    ret = ioctl(fd, VIDIOC_STREAMON, &type);
    if (ret < 0) {
        printf("VIDIOC_STREAMON failed %d\n", ret);
        return ret;
    }

    return ret;
}

static int v4l2_streamoff(int fd, enum v4l2_buf_type type)
{
    int ret;

    ret = ioctl(fd, VIDIOC_STREAMOFF, &type);
    if (ret < 0) {
        printf("VIDIOC_STREAMOFF failed %d\n", ret);
        return ret;
    }

    return ret;
}

static int v4l2_poll(int fd, struct pollfd *events, int timeout)
{
    int ret;

    ret = poll(events, 1, timeout);
    if (ret < 0) {
        printf("poll: error %d (%s)\n", ret, strerror(errno));
        return ret;
    } else if (ret == 0) {
        printf("no data in %d secs..\n", timeout);
        return ret;
    } else {
        if (!(events->revents & POLLIN))  {
            printf("poll: error event (%s)\n", strerror(errno));
            ret = -1;
        }
    }

    return ret;
}

static int v4l2_querybuf(int fd, enum v4l2_buf_type type, int buffer_cnt)
{
    struct v4l2_buffer v4l2_buf;
    int ret;
    int frame_size = getPreviewFrameSize();

    mISPBufferBase = (char*)MAP_FAILED;

    v4l2_buf.type = type;
    v4l2_buf.memory = V4L2_MEMORY_MMAP;
    v4l2_buf.index = 0;

    ret = ioctl(fd, VIDIOC_QUERYBUF, &v4l2_buf);
    if (ret < 0) {
        printf("VIDIOC_QUERYBUF failed %d\n", ret);
        return -1;
    }


    if ((mISPBufferBase = (char *)mmap(NULL,
                                       frame_size * buffer_cnt,
                                       PROT_READ | PROT_WRITE, MAP_SHARED,
                                       fd, /*v4l2_buf.m.offset*/0)) == MAP_FAILED) {
        printf("mmap failed\n");
        return -1;
    }
    while(1)
        ;
    return 0;
}

int enum_frame_intervals(int fd, __u32 pixfmt, __u32 width, __u32 height)
{
    int ret = -1;
    struct v4l2_frmivalenum fival;

    memset(&fival, 0, sizeof(fival));
    fival.index = 0;
    fival.pixel_format = pixfmt;
    fival.width = width;
    fival.height = height;
    PRINT_INFO("    frame interval: ");
    while ((ret = ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &fival)) == 0) {
        if (fival.type == V4L2_FRMIVAL_TYPE_DISCRETE) {
            printf("%u/%u, ",
                   fival.discrete.numerator, fival.discrete.denominator);
        } else if (fival.type == V4L2_FRMIVAL_TYPE_CONTINUOUS) {
            printf("{min { %u/%u } .. max { %u/%u } }, ",
                   fival.stepwise.min.numerator, fival.stepwise.min.numerator,
                   fival.stepwise.max.denominator, fival.stepwise.max.denominator);
            break;
        } else if (fival.type == V4L2_FRMIVAL_TYPE_STEPWISE) {
            printf("{min { %u/%u } .. max { %u/%u } / "
                   "stepsize { %u/%u } }, ",
                   fival.stepwise.min.numerator, fival.stepwise.min.denominator,
                   fival.stepwise.max.numerator, fival.stepwise.max.denominator,
                   fival.stepwise.step.numerator, fival.stepwise.step.denominator);
            break;
        }
        fival.index++;
    }
    printf("\n\n");
    if (ret != 0 && errno != EINVAL) {
        perror("ERROR enumerating frame intervals");
        return errno;
    }

    return 0;
}


int enum_frame_sizes(int fd, __u32 pixfmt)
{
    int ret = -1;
    struct v4l2_frmsizeenum fsize;

    memset(&fsize, 0, sizeof(fsize));
    fsize.index = 0;
    fsize.pixel_format = pixfmt;
    while ((ret = ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &fsize)) == 0) {
        if (fsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
            PRINT_INFO("    fsize.discrete.width=%u, fsize.discrete.height=%u\n",
                       fsize.discrete.width, fsize.discrete.height);
            ret = enum_frame_intervals(fd, pixfmt,
                                       fsize.discrete.width, fsize.discrete.height);
            if (ret != 0)
                printf("  Unable to enumerate frame sizes.\n");
        } else if (fsize.type == V4L2_FRMSIZE_TYPE_CONTINUOUS) {
            printf("{ continuous: min { width = %u, height = %u } .. "
                   "max { width = %u, height = %u } }\n",
                   fsize.stepwise.min_width, fsize.stepwise.min_height,
                   fsize.stepwise.max_width, fsize.stepwise.max_height);
            printf("  Refusing to enumerate frame intervals.\n");
            break;
        } else if (fsize.type == V4L2_FRMSIZE_TYPE_STEPWISE) {
            printf("{ stepwise: min { width = %u, height = %u } .. "
                   "max { width = %u, height = %u } / "
                   "stepsize { width = %u, height = %u } }\n",
                   fsize.stepwise.min_width, fsize.stepwise.min_height,
                   fsize.stepwise.max_width, fsize.stepwise.max_height,
                   fsize.stepwise.step_width, fsize.stepwise.step_height);
            printf("  Refusing to enumerate frame intervals.\n");
            break;
        }
        fsize.index++;
    }
    if (ret != 0 && errno != EINVAL) {
        perror("ERROR enumerating frame sizes");
        return errno;
    }

    return 0;
}

int enum_frame_formats(int fd)
{
    int ret = -1;
    struct v4l2_fmtdesc fmt;

    memset(&fmt, 0, sizeof(fmt));
    fmt.index = 0;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    while ((ret = ioctl(fd, VIDIOC_ENUM_FMT, &fmt)) == 0) {
        PRINT_INFO("VIDIOC_ENUM_FMT success\n");
        fmt.index++;
        PRINT_INFO("==================================================================\n");
        PRINT_INFO("fmt.pixelformat=\'%c%c%c%c\', fmt.description=\'%s\', fmt.index=%d\n",
                   fmt.pixelformat & 0xFF, (fmt.pixelformat >> 8 ) & 0xFF,
                   (fmt.pixelformat >> 16) & 0xFF, (fmt.pixelformat >> 24) & 0xFF,
                   fmt.description, fmt.index);
        ret = enum_frame_sizes(fd, fmt.pixelformat);
        if (ret != 0)
            PRINT_ERR("enum_frame_sizes failed, Unable to enumerate frame sizes, ret=%d\n", ret);
    }

    return 0;
}


int uvc_cam_init()
{
    char *sensorName = NULL;
    int ret = 0;

    struct v4l2_format fmt;

    ENTER;

    uvc_cam_fd = open(UVC_CAM_NAME, O_RDWR | O_NONBLOCK, 0);
    if (uvc_cam_fd < 0) {
        PRINT_ERR("open \"%s\" failed, ret=%d\n", UVC_CAM_NAME, uvc_cam_fd);
        return -1;
    } else {
        PRINT_INFO("open \"%s\" success\n", UVC_CAM_NAME);
    }

    //检查设备的支持能力
    ret = v4l2_querycap(uvc_cam_fd);
    if (ret < 0) {
        PRINT_ERR("v4l2_querycap failed, ret=%d\n", ret);
        return ret;
    }

    //枚举
    sensorName = (char*)v4l2_enuminput(uvc_cam_fd, m_cam_index);
    if (sensorName == NULL) {
        PRINT_ERR("v4l2_enuminput failed\n");
        return -1;
    } else {
        PRINT_INFO("camera sensor name is \"%s\"\n", sensorName);
    }

    ret = v4l2_s_input(uvc_cam_fd, m_cam_index);
    if (ret < 0) {
        PRINT_ERR("v4l2_s_input failed, ret=%d\n", ret);
        return ret;
    }

    // Enumerate the supported formats to check whether the requested one
    if(enum_frame_formats(uvc_cam_fd)) {
        PRINT_ERR("enum_frame_formats failed, Unable to enumerate frame formats\n");
        return -1;
    }
    int i = 0 ;

    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.width = m_preview_width;
    fmt.fmt.pix.height = m_preview_height;
    fmt.fmt.pix.field = V4L2_FIELD_ANY;

    if (ioctl(uvc_cam_fd, VIDIOC_S_FMT, &fmt) < 0) {
        PRINT_ERR("VIDIOC_S_FMT failed\n");
        return -1;
    }

    if ((fmt.fmt.pix.width != m_preview_width) ||
            (fmt.fmt.pix.height != m_preview_height)) {
        printf("  Frame size:   %ux%u (requested size %ux%u is not supported by device)\n",
               fmt.fmt.pix.width, fmt.fmt.pix.height, m_preview_width, m_preview_height);
        /* look the format is not part of the deal ??? */
        //vd->formatIn = vd->fmt.fmt.pix.pixelformat;
    } else {
        printf("  Frame size:   %dx%d\n", m_preview_width, m_preview_height);
    }

    /* set framerate */
    struct v4l2_streamparm* setfps;
    setfps=(struct v4l2_streamparm *) calloc(1, sizeof(struct v4l2_streamparm));
    memset(setfps, 0, sizeof(struct v4l2_streamparm));
    setfps->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    setfps->parm.capture.capturemode = V4L2_CAP_TIMEPERFRAME;
    setfps->parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
    setfps->parm.capture.timeperframe.numerator=1;
    setfps->parm.capture.timeperframe.denominator=m_preview_fps;
    ret = ioctl(uvc_cam_fd, VIDIOC_S_PARM, setfps);
    if(ret == -1) {
        PRINT_ERR("VIDIOC_S_PARM failed, Unable to set frame rate\n");
        return -1;
    }
    ret = ioctl(uvc_cam_fd, VIDIOC_G_PARM, setfps);
    if(ret == 0) {
        if (setfps->parm.capture.timeperframe.numerator != 1 ||
                setfps->parm.capture.timeperframe.denominator != m_preview_fps) {
            printf("  Frame rate:   %u/%u fps (requested frame rate %u fps is "
                   "not supported by device)\n",
                   setfps->parm.capture.timeperframe.denominator,
                   setfps->parm.capture.timeperframe.numerator,
                   m_preview_fps);
        } else {
            printf("  Frame rate:   %d fps\n", m_preview_fps);
        }
    } else {
        printf("Unable to read out current frame rate\n");
        return -1;
    }

    //set exposure
    struct v4l2_control ctrl;
    ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
    ctrl.value = 10;
    ret = ioctl(uvc_cam_fd, VIDIOC_S_CTRL, &ctrl);
    if (ret < 0) {
        printf("s_ctrl exposure failed:%d.\n", ret);
    }

    // reqbuf the buffers
    struct v4l2_requestbuffers req;
    req.count = m_preview_buffer_cnt;//numbuf;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    ret = ioctl(uvc_cam_fd, VIDIOC_REQBUFS, &req);
    if (ret < 0) {
        printf("VIDIOC_REQBUFS failed\n");
        return -1;
    }

    // Queue the buffers.
    for (i = 0; i < req.count; i++) {
        CLEAR(v4l2_buf);
        v4l2_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        v4l2_buf.memory = V4L2_MEMORY_MMAP;
        v4l2_buf.index = i;

        if (-1 == ioctl (uvc_cam_fd, VIDIOC_QBUF, &v4l2_buf)) {
            printf("[mm_app]:VIDIOC_QBUF error\n");
            return -1;
        }
    }

    //mmap the buffer
    for(i = 0; i < req.count; i++) {
        memset(&v4l2_buf, 0, sizeof(v4l2_buf));

        v4l2_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        v4l2_buf.memory = V4L2_MEMORY_MMAP;
        v4l2_buf.index = i;
        ret = ioctl(uvc_cam_fd , VIDIOC_QUERYBUF, &v4l2_buf);
        if(ret < 0) {
            printf(" VIDIOC_QUERYBUF failed\n");
            return -1;
        }

        printf("v4l2_buf.length=%d\n",v4l2_buf.length);
        if ((ISP_buf[i] = (char*)mmap(0, v4l2_buf.length,
                                      PROT_READ | PROT_WRITE, MAP_SHARED,
                                      uvc_cam_fd, v4l2_buf.m.offset)) < 0) {
            printf("mmap() failed\n");
            return -1;
        }
    }

    return 0;
}

int uvc_cam_deinit()
{
    ENTER;
    if (uvc_cam_fd > -1) {
        int i = 0;
        for (i = 0; i < m_preview_buffer_cnt; i++) {
            munmap(ISP_buf[i], getPreviewFrameSize());
        }

        close(uvc_cam_fd);
        uvc_cam_fd = -1;
    }

    return 0;
}

int stream_on()
{
    int i;
    int ret;

    memset(&m_events_c, 0, sizeof(m_events_c));
    m_events_c.fd = uvc_cam_fd;
    m_events_c.events = POLLIN | POLLERR;

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    ret = ioctl(uvc_cam_fd, VIDIOC_STREAMON, &type);
    if (ret < 0) {
        printf("VIDIOC_STREAMON failed\n");
        return ret;
    }
    return 0;
}

int startPreview()
{
    printf("startPreview\n");
    int ret = 0;
    struct v4l2_streamparm streamparm;
    /*
        memset(&m_events_c, 0, sizeof(m_events_c));
        m_events_c.fd = uvc_cam_fd;
        m_events_c.events = POLLIN | POLLERR;

        ret = v4l2_enum_fmt(uvc_cam_fd, V4L2_BUF_TYPE_VIDEO_CAPTURE, m_preview_fmt);
        if (ret < 0) return -1;

        ret = v4l2_s_fmt(uvc_cam_fd, V4L2_BUF_TYPE_VIDEO_CAPTURE, m_preview_width, m_preview_height, m_preview_fmt);
        if (ret < 0) return -1;

        ret = v4l2_s_parm(uvc_cam_fd, V4L2_BUF_TYPE_VIDEO_CAPTURE, &streamparm);
        if (ret < 0) return -1;

       //printf("@@@@yuanlan>>>>>\n");
        ret = v4l2_s_ctrl(uvc_cam_fd, V4L2_CID_SNAPSHOT, 0);
        if (ret < 0) return -1;

        ret = v4l2_reqbufs(uvc_cam_fd, V4L2_BUF_TYPE_VIDEO_CAPTURE, m_preview_buffer_cnt);
        if (ret < 0) return -1;

        // get the real buffer count.
        m_preview_buffer_cnt = ret;
        printf("@@@@@@@@m_preview_buffer_cnt=%d\n",m_preview_buffer_cnt);
        // start with all buffers in queue.
        int i = 0;
        for (i = 0; i < m_preview_buffer_cnt; i++) {
            ret = v4l2_qbuf(uvc_cam_fd, V4L2_BUF_TYPE_VIDEO_CAPTURE, i);
            if (ret < 0) return -1;
        }

        // map vaddr from camera paddr buffer
        ret = v4l2_querybuf(uvc_cam_fd, V4L2_BUF_TYPE_VIDEO_CAPTURE, m_preview_buffer_cnt);
        if (ret < 0){
            printf("startPreview: v4l2_querybuf fail!!");
            return -1;
        }
    */
    ret = v4l2_streamon(uvc_cam_fd, V4L2_BUF_TYPE_VIDEO_CAPTURE);

    return ret;
}

int stopPreview()
{
    ENTER;
    int ret = 0;

    ret = v4l2_streamoff(uvc_cam_fd, V4L2_BUF_TYPE_VIDEO_CAPTURE);

    return ret;
}

int getPreview(unsigned int *paddr)
{
    int index;
    int ret;

    ret = v4l2_poll(uvc_cam_fd, &m_events_c, 2000);
    if (ret < 0) {
        PRINT_ERR("getPreview: v4l2_poll error\n");
        return V4L2_POLL_ERROR;
    } else if (ret == 0) {
        return V4L2_POLL_TIMEOUT;
    }

    index = v4l2_dqbuf(uvc_cam_fd, V4L2_BUF_TYPE_VIDEO_CAPTURE, paddr);
    if (!(index >= 0 && index < m_preview_buffer_cnt)) {
        PRINT_ERR("getPreview: invalid index=%d\n", index);
        return -1;
    } else {
        PRINT_INFO("getPreview: index=%d\n", index);
    }

    return index;
}

int releasePreview(int index)
{
    int ret;

    if (index >= m_preview_buffer_cnt) {
        printf("releasePreview: invalid index = %d\n", index);
        return -1;
    }

    ret = v4l2_qbuf(uvc_cam_fd, V4L2_BUF_TYPE_VIDEO_CAPTURE, index);
    if (ret < 0)
        return -1;

    return 0;
}

int save_this_frame(char *vaddr, unsigned int paddr, int frame_num)
{
    PRINT_INFO("frame_num=%3d, vaddr=0x%016lX, paddr=0x%08X\n",
        frame_num, (unsigned long)vaddr, paddr);

    int ret = -1;
    char file_path[100] = {FILE_PATH};
    char file_name[30] = {'\0'};
    int frame_size = getPreviewFrameSize();

    ret = access(file_path, 6);
    if (0 != ret) {
        PRINT_INFO("path: \"%s\" is not exist, now mkdir of it\n", file_path);
        ret = mkdir(file_path, S_IRWXU|S_IRWXG|S_IRWXO);
        if (0 != ret) {
            PRINT_ERR("mkdir of \"%s\" failed\n", file_path);
            return ret;
        }
    }

    sprintf(file_name, "YUYV(%dX%d)_%03d",
        m_preview_width, m_preview_height, frame_num);
    strcat(file_path, file_name);

    FILE *file = fopen(file_path, "wb");
    if (file != NULL) {
        PRINT_INFO("saving frame_%03d to file: \"%s\"\n", frame_num, file_path);
        fwrite(vaddr, frame_size, 1, file);
        fclose(file);
        return 0;
    } else {
        PRINT_ERR("open \"%s\" failed\n", file_path);
        return -1;
    }
}

void* thread_preview(void* arg)
{
    unsigned int paddr = 0;
    char *vaddr = NULL;
    int frame_num = 0;
    int index = -1;
    unsigned long time_elapsed = 0;
    struct timeval tv1, tv2;

    while(frame_num <= NUM_OF_FRAME_TO_GET) {
        gettimeofday(&tv1, NULL);
        index = getPreview(&paddr);
        if (index < 0) {
            PRINT_ERR("failed to get preview\n");
        }
        gettimeofday(&tv2, NULL);

        if (0 == frame_num) {
            PRINT_INFO("ignore the first frame\n");
        } else {
            time_elapsed = ((unsigned long)tv2.tv_sec * 1000 + (unsigned long)tv2.tv_usec/1000)
                   - ((unsigned long)tv1.tv_sec * 1000 + (unsigned long)tv1.tv_usec/1000);
            PRINT_INFO("getPreview time_elapsed=%4ldms, frame_num=%3d\n",
                time_elapsed, frame_num);

            vaddr = ISP_buf[index];
            save_this_frame(vaddr, paddr, frame_num);
        }
        frame_num++;
        releasePreview(index);
    }

    pthread_exit(NULL);
    return;
}

int main()
{
    int ret = -1;

    ret = uvc_cam_init();
    if (ret < 0)
        return ret;

    ret = stream_on();
    if (ret < 0) {
        printf("startPreview stream on failed.\n");
    } else {
        pthread_create(&preview_thd, NULL, thread_preview, NULL);
        pthread_join(preview_thd, NULL);
    }

    ret = stopPreview();
    if (ret < 0) printf("stopPreview failed.\n");

    uvc_cam_deinit();

    return ret;
}
