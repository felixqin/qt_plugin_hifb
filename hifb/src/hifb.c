
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


inline int HIFB_GetVoInfo(const char* interface,
        VO_INTF_TYPE_E* penVoIntfType, VO_INTF_SYNC_E* penIntfSync,
        HI_U32* pu32Width, HI_U32* pu32Height)
{
    if (strcmp(interface, "CVBS") == 0) {
        *penVoIntfType = VO_INTF_CVBS;
        *penIntfSync = VO_OUTPUT_PAL;
    } else if (strcmp(interface, "YPBPR") == 0) {
        *penVoIntfType = VO_INTF_YPBPR;
        *penIntfSync = VO_OUTPUT_1080P60;
    } else if (strcmp(interface, "VGA") == 0) {
        *penVoIntfType = VO_INTF_VGA;
        *penIntfSync = VO_OUTPUT_1080P60;
    } else if (strcmp(interface, "BT656") == 0) {
        *penVoIntfType = VO_INTF_BT656;
        *penIntfSync = VO_OUTPUT_1080P60;
    } else if (strcmp(interface, "BT1120") == 0) {
        *penVoIntfType = VO_INTF_BT1120;
        *penIntfSync = VO_OUTPUT_1080P60;
    } else if (strcmp(interface, "HDMI") == 0) {
        *penVoIntfType = VO_INTF_HDMI;
        *penIntfSync = VO_OUTPUT_1080P60;
    } else if (strcmp(interface, "LCD") == 0) {
        *penVoIntfType = VO_INTF_LCD;
        *penIntfSync = VO_OUTPUT_1080P60;
    } else {
        SAMPLE_PRT("unsupported video out interface(%s)\n", interface);
        return -1;
    }

    HI_U32 u32VoFrmRate;
    HI_S32 s32Ret = SAMPLE_COMM_VO_GetWH(*penIntfSync, pu32Width, pu32Height, &u32VoFrmRate);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get vo wh failed with %d!\n", s32Ret);
        return -1;
    }

    return 0;
}

static int HIFB_OpenVoLayer(VO_DEV VoDev, VO_LAYER VoLayer, VO_INTF_TYPE_E enVoIntfType, VO_INTF_SYNC_E enIntfSync, HI_U32 u32Width, HI_U32 u32Height)
{
    HI_S32 s32Ret = HI_FAILURE;

    VO_PUB_ATTR_S stPubAttr;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    VB_CONF_S       stVbConf;
    HI_U32 u32BlkSize;

    /******************************************
     step  1: mpp system init.
    ******************************************/
    memset(&stVbConf, 0, sizeof(VB_CONF_S));
    u32BlkSize = CEILING_2_POWER(u32Width, SAMPLE_SYS_ALIGN_WIDTH) * CEILING_2_POWER(u32Height, SAMPLE_SYS_ALIGN_WIDTH) * 2;
    stVbConf.u32MaxPoolCnt = 128;
    stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt =  6;
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        return s32Ret;
    }

    /******************************************
     step 2:  start vo dev.
    *****************************************/
    stPubAttr.enIntfSync = enIntfSync;
    stPubAttr.enIntfType = enVoIntfType;
    stPubAttr.u32BgColor = 0x0000FF;
    s32Ret = SAMPLE_COMM_VO_StartDev(VoDev, &stPubAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vo dev failed with %d!\n", s32Ret);
        SAMPLE_COMM_SYS_Exit();
        return s32Ret;
    }

    /******************************************
    step 3:  start graphics layer.
    *****************************************/
    stLayerAttr.bClusterMode = HI_FALSE;
    stLayerAttr.bDoubleFrame = HI_FALSE;
    stLayerAttr.enPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    stLayerAttr.stImageSize.u32Width = u32Width;
    stLayerAttr.stImageSize.u32Height = u32Height;
    stLayerAttr.u32DispFrmRt = 30 ;
    stLayerAttr.stDispRect.s32X = 0;
    stLayerAttr.stDispRect.s32Y = 0;
    stLayerAttr.stDispRect.u32Width = u32Width;
    stLayerAttr.stDispRect.u32Height = u32Height;
    s32Ret = SAMPLE_COMM_VO_StartLayer(VoLayer, &stLayerAttr, HI_TRUE);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vo layer failed with %d!\n", s32Ret);
        SAMPLE_COMM_VO_StopDev(VoDev);
        SAMPLE_COMM_SYS_Exit();
        return s32Ret;
    }

    return HI_SUCCESS;
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
            var.yres_virtual = u32Height;
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

    HIFB_ALPHA_S stAlpha = {};
    if (ioctl(fd, FBIOGET_ALPHA_HIFB, &stAlpha) != 0) {
        SAMPLE_PRT("Error reading alpha information");
        close(fd);
        return -1;
    }

    stAlpha.u8Alpha0 = 0x80;
    if (ioctl(fd, FBIOPUT_ALPHA_HIFB, &stAlpha) != 0) {
        SAMPLE_PRT("Error writing alpha information");
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


static VO_DEV gVoDev = -1;
static VO_LAYER gVoLayer = GRAPHICS_LAYER_G0;


int HIFB_Open(const char* param)
{
    HI_S32 s32Ret = HI_FAILURE;
    VO_INTF_SYNC_E enIntfSync;
    HI_U32 u32Width;
    HI_U32 u32Height;
    VO_INTF_TYPE_E enVoIntfType;

    const char* interface = param;  // TODO: parse from param
    gVoDev = -1;                    // TODO: parse from param
    SAMPLE_PRT("HIFB_Open gVoDev(%d) interface(%s)\n", gVoDev, interface);

    s32Ret = HIFB_GetVoInfo(interface, &enVoIntfType, &enIntfSync, &u32Width, &u32Height);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HIFB_GetVoInfo failed with %d!\n", s32Ret);
        return -1;
    }

    if (gVoDev >= 0) {
        s32Ret = HIFB_OpenVoLayer(gVoDev, gVoLayer, enVoIntfType, enIntfSync, u32Width, u32Height);
        if (HI_SUCCESS != s32Ret) {
            SAMPLE_PRT("HIFB_OpenVoLayer failed with %d!\n", s32Ret);
            return -1;
        }
    }

    int fd = HIFB_OpenFB(gVoLayer, u32Width, u32Height);
    if (fd < 0)
    {
        SAMPLE_PRT("HIFB_OpenFB failed with %d!\n", s32Ret);
        if (gVoDev >= 0) {
            HIFB_CloseVoLayer(gVoDev, gVoLayer);
        }

        return -1;
    }

    return fd;
}

void HIFB_Close(int fd)
{
    if (fd >= 0) {
        HIFB_CloseFB(fd);
    }

    if (gVoDev >= 0) {
        HIFB_CloseVoLayer(gVoDev, gVoLayer);
    }
}
