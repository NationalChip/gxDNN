/* GXDNN
 * Copyright (C) 1991-2017 NationalChip Co., Ltd
 *
 * gxdnn.h  NPU Task loader and executor
 *
 */

#ifndef __GXDNN_H__
#define __GXDNN_H__

#ifdef __cplusplus
extern "C" {
#endif

/*===============================================================================================*/

typedef void* GxDnnDevice;
typedef void* GxDnnTask;

typedef enum {
    GXDNN_RESULT_SUCCESS = 0,
    GXDNN_RESULT_WRONG_PARAMETER,
    GXDNN_RESULT_MEMORY_NOT_ENOUGH,
    GXDNN_RESULT_DEVICE_ERROR,
    GXDNN_RESULT_FILE_NOT_FOUND,
    GXDNN_RESULT_UNSUPPORT,
    GXDNN_RESULT_UNKNOWN_ERROR
} GxDnnResult;

/*===============================================================================================*/

/**
 * @brief   Open NPU device
 * @param   [in]    devicePath      the path to device
 *          [out]   device          a handle to the openned device
 * @return  GxDnnResult GXDNN_RESULT_SUCCESS    succeed without error
 *                      GXDNN_RESULT_WRONG_PARAMETER    wrong parameter
 *                      GXDNN_RESULT_DEVICE_ERROR       device error
 * @remark  if devicePath is "/dev/gxnpu", open npu device
 *             devicePath is "/dev/gxsnpu", open snpu device
 */
GxDnnResult GxDnnOpenDevice(const char *devicePath,
                            GxDnnDevice *device);

/*===============================================================================================*/

/**
 * @brief   Close NPU device
 * @param   [in]    device          the handle to openned device
 * @return  GxDnnResult GXDNN_RESULT_SUCCESS    succeed without error
 *                      GXDNN_RESULT_WRONG_PARAMETER    wrong parameter
 *                      GXDNN_RESULT_DEVICE_ERROR       device error
 * @remark
 */
GxDnnResult GxDnnCloseDevice(GxDnnDevice device);

/*===============================================================================================*/

/**
 * @brief   Load NPU Task from file (in Linux/MacOSX)
 * @param   [in]    device          the device handle
 *          [in]    taskPath        the path to NPU task file
 *          [out]   task            a handle to the loaded task
 * @return  GxDnnResult GXDNN_RESULT_SUCCESS    succeed without error
 *                      GXDNN_RESULT_WRONG_PARAMETER    wrong parameter
 *                      GXDNN_RESULT_MEMORY_NOT_ENOUGH  no enough memory
 *                      GXDNN_RESULT_DEVICE_ERROR       device error
 *                      GXDNN_RESULT_FILE_NOT_FOUND     file not found
 *                      GXDNN_RESULT_UNSUPPORT          unsupport NPU type or npu task version
 * @remark
 */
GxDnnResult GxDnnCreateTaskFromFile(GxDnnDevice device,
                                    const char *taskPath,
                                    GxDnnTask *task);

/*===============================================================================================*/

/**
 * @brief   Load NPU task from memory
 * @param   [in]    device          the device handle
 *          [in]    taskBuffer      the pointer to NPU task buffer
 *          [in]    bufferSize      the buffer size of the NPU task buffer
 *          [out]   task            a handle to the loaded task
 * @return  GxDnnResult GXDNN_RESULT_SUCCESS    succeed without error
 *                      GXDNN_RESULT_WRONG_PARAMETER    wrong parameter
 *                      GXDNN_RESULT_MEMORY_NOT_ENOUGH  no enough memory
 *                      GXDNN_RESULT_DEVICE_ERROR       device error
 *                      GXDNN_RESULT_UNSUPPORT          unsupport NPU type or npu task version
 * @remark
 */
GxDnnResult GxDnnCreateTaskFromBuffer(GxDnnDevice device,
                                      const unsigned char *taskBuffer,
                                      const int bufferSize,
                                      GxDnnTask *task);

/*===============================================================================================*/

/**
 * @brief   Release NPU task
 * @param   [in]    task            the loaded task handle
 * @return  GxDnnResult GXDNN_RESULT_SUCCESS    succeed without error
 *                      GXDNN_RESULT_WRONG_PARAMETER    wrong parameter
 * @remark
 */
GxDnnResult GxDnnReleaseTask(GxDnnTask task);

/*===============================================================================================*/

#define MAX_NAME_SIZE 30
#define MAX_SHAPE_SIZE 10

typedef struct NpuIOInfo {
    int direction;              /* 0: Input; 1: Output */
    char name[MAX_NAME_SIZE];   /* name of the IO */
    int shape[MAX_SHAPE_SIZE];  /* the shape of the IO */
    unsigned int dimension;     /* the dimension of the IO */
    void *dataBuffer;           /* the data buffer */
    int bufferSize;             /* the data buffer size */
} GxDnnIOInfo;

/**
 * @brief   Get the IO Num of the loaded task
 * @param   [in]    task            the loaded task
 *          [out]   inputNum        Input number
 *          [out]   outputNum       Output Number
 * @return  GxDnnResult GXDNN_RESULT_SUCCESS    succeed without error
 *                      GXDNN_RESULT_WRONG_PARAMETER    wrong parameter
 * @remark
 */
GxDnnResult GxDnnGetTaskIONum(GxDnnTask task,
                              int *inputNum,
                              int *outputNum);

/*===============================================================================================*/

/**
 * @brief   Get the IO Info of the loaded task
 * @param [in]      task            the loaded task
 *        [out]     inputInfo       the input information List
 *        [in]      inputInfoSize   the size of the output info list buffer
 *        [out]     outputInfo      the output information list
 *        [in]      outputInfoSize  the size of the output info list buffer
 * @return  GxDnnResult GXDNN_RESULT_SUCCESS    succeed without error
 *                      GXDNN_ERR_BAD_PARAMETER wrong parameter
 * @remark
 */
GxDnnResult GxDnnGetTaskIOInfo(GxDnnTask task,
                               GxDnnIOInfo *inputInfo,
                               int inputInfoSize,
                               GxDnnIOInfo *outputInfo,
                               int outputInfoSize);

/*===============================================================================================*/

typedef enum {
    GXDNN_EVENT_FINISH,
    GXDNN_EVENT_ABORT,
    GXDNN_EVENT_FAILURE
} GxDnnEvent;

/**
 * @brief   The event handler
 * @param   [in]    task            the running task
 *          [in]    event           the event type
 *          [in]    userData        the userData passed by GxDnnRunTask
 * @return  int     0               break the task
 *                  not 0           continue the task
 */
typedef int (*GxDnnEventHandler)(GxDnnTask task, GxDnnEvent event, void *userData);

/*===============================================================================================*/

/**
 * @brief   Run task
 * @param   [in]    task            the loaded task
 *          [in]    priority        the task priority
 *          [in]    eventHandler    the event callback (see remark)
 *          [in]    userData        a void data will be passed to event handler
 * @return  GxDnnResult GXDNN_RESULT_SUCCESS    succeed without error
 *                      GXDNN_RESULT_WRONG_PARAMETER    wrong parameter
 * @remark  if eventHandler == NULL, the function will not return until finish or error happens;
 *          if the task is running, the task will stop first;
 */
GxDnnResult GxDnnRunTask(GxDnnTask task,
                         int priority,
                         GxDnnEventHandler eventHandler,
                         void *userData);

/*===============================================================================================*/

/**
 * @brief   Stop task
 * @param   [in]    task            the loaded task
 * @return  GxDnnResult GXDNN_RESULT_SUCCESS    succeed without error
 *                      GXDNN_RESULT_WRONG_PARAMETER    wrong parameter
 * @remark  if the task is running, the event handler will be invoked
 */
GxDnnResult GxDnnStopTask(GxDnnTask task);

/*===============================================================================================*/

typedef struct NpuDevUtilInfo {
    float ratio;
} GxDnnDevUtilInfo;

/**
 * @brief   Get device utilization information
 * @param   [in]   GxDnnDevice device
 *          [out]  GxDnnDevUtilInfo info
 * @return  GxDnnResult GXDNN_RESULT_SUCCESS    succeed without error
 *                      GXDNN_RESULT_WRONG_PARAMETER    wrong parameter
 *                      GXDNN_RESULT_DEVICE_ERROR    device error
 */
GxDnnResult GxDnnGetDeviceUtil(GxDnnDevice device, GxDnnDevUtilInfo *info);

/*===============================================================================================*/

#ifdef __cplusplus
} /* extern C */
#endif

#endif

