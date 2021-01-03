#ifndef _COMMON_RESULT_H_
#define _COMMON_RESULT_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    eResultSucces = 0,
    eResultFailed = 1,
    eResultNotInitialized = 2,
} result_t;

#ifdef __cplusplus
}
#endif

#endif /*_COMMON_RESULT_H_*/