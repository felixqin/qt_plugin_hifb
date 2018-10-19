
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>

#include "common/sample_comm.h"

#include <linux/fb.h>
#include "hifb.h"



static struct fb_bitfield s_r16 = {10, 5, 0};
static struct fb_bitfield s_g16 = {5, 5, 0};
static struct fb_bitfield s_b16 = {0, 5, 0};
static struct fb_bitfield s_a16 = {15, 1, 0};



#define GRAPHICS_LAYER_G0  0


#define WIDTH_1920             1920
#define HEIGHT_1080            1080
#define WIDTH_720              720
#define HEIGHT_576             576



static int HIFB_OpenVoLayer(VO_DEV VoDev, VO_INTF_TYPE_E enVoIntfType, VO_LAYER VoLayer, HI_U32 u32PicWidth, HI_U32 u32PicHeight)
{
    HI_S32 s32Ret = HI_FAILURE;
    SIZE_S  stSize;

    VO_PUB_ATTR_S stPubAttr;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    HI_U32 u32VoFrmRate;

    VB_CONF_S       stVbConf;
    HI_U32 u32BlkSize;

    /******************************************
     step  1: init variable
    ******************************************/
    memset(&stVbConf, 0, sizeof(VB_CONF_S));
    u32BlkSize = CEILING_2_POWER(u32PicWidth, SAMPLE_SYS_ALIGN_WIDTH)\
                 * CEILING_2_POWER(u32PicHeight, SAMPLE_SYS_ALIGN_WIDTH) * 2;
    stVbConf.u32MaxPoolCnt = 128;
    stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt =  6;

    /******************************************
     step 2: mpp system init.
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        goto SAMPLE_HIFB_StandarMode_0;
    }

    /******************************************
     step 3:  start vo dev.
    *****************************************/
    if (VO_INTF_CVBS == enVoIntfType)
    {
        stPubAttr.enIntfSync = VO_OUTPUT_PAL;
        stPubAttr.enIntfType = enVoIntfType;
        stPubAttr.u32BgColor = 0x0000FF;
    }
    else
    {
        stPubAttr.enIntfSync = VO_OUTPUT_1080P60;
        stPubAttr.enIntfType = enVoIntfType;
        stPubAttr.u32BgColor = 0x0000FF;
    }

    stLayerAttr.bClusterMode = HI_FALSE;
    stLayerAttr.bDoubleFrame = HI_FALSE;
    stLayerAttr.enPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;

    s32Ret = SAMPLE_COMM_VO_GetWH(stPubAttr.enIntfSync, &stSize.u32Width, \
                                  &stSize.u32Height, &u32VoFrmRate);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get vo wh failed with %d!\n", s32Ret);
        goto SAMPLE_HIFB_StandarMode_0;
    }
    memcpy(&stLayerAttr.stImageSize, &stSize, sizeof(stSize));

    stLayerAttr.u32DispFrmRt = 30 ;
    stLayerAttr.stDispRect.s32X = 0;
    stLayerAttr.stDispRect.s32Y = 0;
    stLayerAttr.stDispRect.u32Width = stSize.u32Width;
    stLayerAttr.stDispRect.u32Height = stSize.u32Height;

    s32Ret = SAMPLE_COMM_VO_StartDev(VoDev, &stPubAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vo dev failed with %d!\n", s32Ret);
        goto SAMPLE_HIFB_StandarMode_0;
    }

    s32Ret = SAMPLE_COMM_VO_StartLayer(VoLayer, &stLayerAttr, HI_TRUE);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vo layer failed with %d!\n", s32Ret);
        goto SAMPLE_HIFB_StandarMode_1;
    }

    /******************************************
     step 4:  start hifb.
    *****************************************/
    return s32Ret;

SAMPLE_HIFB_StandarMode_1:
    SAMPLE_COMM_VO_StopDev(VoDev);
SAMPLE_HIFB_StandarMode_0:
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

static void HIFB_CloseVoLayer(VO_DEV VoDev, VO_LAYER VoLayer)
{
    SAMPLE_COMM_VO_StopLayer(VoLayer);
    SAMPLE_COMM_VO_StopDev(VoDev);
    SAMPLE_COMM_SYS_Exit();
}

static int HIFB_OpenFB(VO_LAYER VoLayer, HI_U32 u32Width, HI_U32 u32Height)
{
    int fd;
    struct fb_fix_screeninfo fix;
    struct fb_var_screeninfo var;
    HIFB_POINT_S stPoint = {0, 0};
    HI_CHAR file[12] = "/dev/fb0";
    HI_BOOL bShow;

    switch (VoLayer)
    {
        case GRAPHICS_LAYER_G0 :
            strncpy(file, "/dev/fb0", 12);
            break;
        default:
            strncpy(file, "/dev/fb0", 12);
            break;
    }

    /* 1. open framebuffer device overlay 0 */
    fd = open(file, O_RDWR, 0);
    if (fd < 0)
    {
        SAMPLE_PRT("open %s failed!\n", file);
        return fd;
    }

    bShow = HI_FALSE;
    if (ioctl(fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
    {
        SAMPLE_PRT("FBIOPUT_SHOW_HIFB failed!\n");
        return -1;
    }
    /* 2. set the screen original position */
    switch (VoLayer)
    {
        case GRAPHICS_LAYER_G0:
        {
            stPoint.s32XPos = 0;
            stPoint.s32YPos = 0;
        }
            break;
        default:
            break;
    }

    if (ioctl(fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoint) < 0)
    {
        SAMPLE_PRT("set screen original show position failed!\n");
        close(fd);
        return -1;
    }

    /* 3. get the variable screen info */
    if (ioctl(fd, FBIOGET_VSCREENINFO, &var) < 0)
    {
        SAMPLE_PRT("Get variable screen info failed!\n");
        close(fd);
        return -1;
    }

    /* 4. modify the variable screen info
          the screen size: IMAGE_WIDTH*IMAGE_HEIGHT
          the virtual screen size: VIR_SCREEN_WIDTH*VIR_SCREEN_HEIGHT
          (which equals to VIR_SCREEN_WIDTH*(IMAGE_HEIGHT*2))
          the pixel format: ARGB1555
    */
    usleep(4 * 1000 * 1000);
    switch (VoLayer)
    {
        case GRAPHICS_LAYER_G0:
        {
            var.xres_virtual = u32Width;
            var.yres_virtual = u32Height * 2;
            var.xres = u32Width;
            var.yres = u32Height;
        }
            break;
        default:
            break;
    }

    var.transp = s_a16;
    var.red = s_r16;
    var.green = s_g16;
    var.blue = s_b16;
    var.bits_per_pixel = 16;
    var.activate = FB_ACTIVATE_NOW;

    /* 5. set the variable screeninfo */
    if (ioctl(fd, FBIOPUT_VSCREENINFO, &var) < 0)
    {
        SAMPLE_PRT("Put variable screen info failed!\n");
        close(fd);
        return -1;
    }

    /* 6. get the fix screen info */
    if (ioctl(fd, FBIOGET_FSCREENINFO, &fix) < 0)
    {
        SAMPLE_PRT("Get fix screen info failed!\n");
        close(fd);
        return -1;
    }

    /* 7. map the physical video memory for user use */
    bShow = HI_TRUE;
    if (ioctl(fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
    {
        SAMPLE_PRT("FBIOPUT_SHOW_HIFB failed!\n");
        close(fd);
        return -1;
    }

    return fd;
}

static void HIFB_CloseFB(int fd)
{
    close(fd);
}

int HIFB_Open(int vout, const char* interface)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_U32 u32Width;
    HI_U32 u32Height;
    VO_DEV VoDev = vout;
    VO_LAYER VoLayer = GRAPHICS_LAYER_G0;
    VO_INTF_TYPE_E enVoIntfType = VO_INTF_HDMI;

    if (strcmp(interface, "CVBS") == 0) {
        enVoIntfType = VO_INTF_CVBS;
    } else if (strcmp(interface, "YPBPR") == 0) {
        enVoIntfType = VO_INTF_YPBPR;
    } else if (strcmp(interface, "VGA") == 0) {
        enVoIntfType = VO_INTF_VGA;
    } else if (strcmp(interface, "BT656") == 0) {
        enVoIntfType = VO_INTF_BT656;
    } else if (strcmp(interface, "BT1120") == 0) {
        enVoIntfType = VO_INTF_BT1120;
    } else if (strcmp(interface, "HDMI") == 0) {
        enVoIntfType = VO_INTF_HDMI;
    } else if (strcmp(interface, "LCD") == 0) {
        enVoIntfType = VO_INTF_LCD;
    } else {
        SAMPLE_PRT("unsupported video out interface(%s)\n", interface);
        return -1;
    }

    if (VO_INTF_CVBS == enVoIntfType)
    {
        u32Width = WIDTH_720;
        u32Height = HEIGHT_576;
    }
    else
    {
        u32Width = WIDTH_1920;
        u32Height = HEIGHT_1080;
    }

    s32Ret = HIFB_OpenVoLayer(VoDev, enVoIntfType, VoLayer, u32Width, u32Height);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HIFB_OpenVoLayer failed with %d!\n", s32Ret);
        HIFB_CloseVoLayer(VoDev, VoLayer);
        return -1;
    }

    int fd = HIFB_OpenFB(VoLayer, u32Width, u32Height);
    return fd;
}

void HIFB_Close(int vout, int fd)
{
    VO_DEV VoDev = vout;
    VO_LAYER VoLayer = GRAPHICS_LAYER_G0;

    HIFB_CloseFB(fd);
    HIFB_CloseVoLayer(VoDev, VoLayer);
}
