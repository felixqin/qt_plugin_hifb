#ifndef __HIFB_FRAMEBUFFER_H__
#define __HIFB_FRAMEBUFFER_H__


#ifdef __cplusplus
extern "C" {
#endif

int HIFB_Open(const char* param);
void HIFB_Close(int fd);


#ifdef __cplusplus
};
#endif

#endif // __HIFB_FRAMEBUFFER_H__
