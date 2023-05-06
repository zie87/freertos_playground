#include <FreeRTOS.h>

#if (configSUPPORT_DYNAMIC_ALLOCATION == 1)

#ifdef __cplusplus
extern "C" {
#endif
void* malloc(size_t size) { return pvPortMalloc(size); }
void free(void* p) { vPortFree(p); }
#ifdef __cplusplus
}
#endif

#endif /* (configSUPPORT_DYNAMIC_ALLOCATION == 1) */
