#ifndef __GX_SNPU_H__
#define __GX_SNPU_H__

//#include "autoconf.h"
#define CONFIG_ARCH_GRUS

/**
 * @brief 初始化snpu
 *
 *
 * @return snpu 初始化成功与否
 * @retval 0 正常
 * @retval -1 失败
 */
int gx_snpu_init(void);

/**
 * @brief 关闭snpu
 *
 *
 * @return snpu 关闭成功与否
 * @retval 0 正常
 * @retval -1 失败
 */
int gx_snpu_exit(void);

/**
 * @brief snpu 输入和输出的数据格式，注：GRUS的输出格式可以在编译模型时选择使用float16还是float32
*/
typedef short GX_SNPU_FLOAT;
/**
 * @brief snpu工作状态
*/
typedef enum {
    GX_SNPU_IDLE,    /**< 空闲状态*/
    GX_SNPU_BUSY,    /**< 工作忙状态*/
    GX_SNPU_STALL,   /**< 停滞状态*/
} GX_SNPU_STATE;

#if defined(CONFIG_ARCH_LEO)


/**
 * @brief snpu 异步方式运行任务时需要的回调函数
*/
typedef int (*GX_SNPU_CALLBACK)(GX_SNPU_STATE state, void *private_data);

/**
 * @brief snpu 工作任务
*/
typedef struct {
    void *ops;              /**< mcu算子信息内容区域 ops_content in model.h*/
    void *data;             /**< 权重和数据内容区域  cpu_content in model.h*/
    void *input;            /**< 模型输入数据        input in model.h*/
    void *output;           /**< 模型输出数据        output in model.h*/
    void *cmd;              /**< 指令内容区域        npu_content in model.h*/
    void *tmp_mem;          /**< 临时数据内容区域    tmp_content in model.h*/
} GX_SNPU_TASK;

/**
 * @brief 同步方式运行一个工作任务
 *
 * @param task 工作任务
 *
 * @return snpu 任务工作状态
 * @retval 0  正常
 * @retval -1 失败
 */
int gx_snpu_run_task(GX_SNPU_TASK *task);

#elif defined(CONFIG_ARCH_GRUS)

/**
 * @brief snpu 回调函数，任务处理完后，中断里面执行该函数
*/
typedef int (*GX_SNPU_CALLBACK)(int module_id, GX_SNPU_STATE state, void *private_data);

/**
 * @brief snpu 工作任务
*/
typedef struct {
    int module_id;          /**< 模块标识号          module id defined by programmer*/
    void *ops;              /**< mcu算子信息内容区域 ops_content in model.h*/
    void *data;             /**< 临时数据内容区域    data_content in model.h*/
    void *input;            /**< 模型输入数据        input in model.h*/
    void *output;           /**< 模型输出数据        output in model.h*/
    void *cmd;              /**< 指令内容区域        cmd_content in model.h*/
    void *tmp_mem;          /**< 临时数据内容区域    tmp_content in model.h*/
    void *weight;           /**< 权重内容区域        weight_content in model.h*/
} GX_SNPU_TASK;

/**
 * @brief 异步方式运行一个工作任务
 *
 * @param task 工作任务
 * @param callback 回调函数，任务处理完后，中断里面执行该函数
 * @param private_data 回调函数的参数
 *
 * @return snpu 任务工作状态
 * @retval 0  正常
 * @retval -1 失败
 */
int gx_snpu_run_task(GX_SNPU_TASK *task, GX_SNPU_CALLBACK callback, void *private_data);

/**
 * @brief 同步方式运行一个工作任务
 *
 * @param task 工作任务
 *
 * @return snpu 任务工作状态
 * @retval 0  正常
 * @retval -1 失败
 */
int gx_snpu_run_task_sync(GX_SNPU_TASK *task);

#endif

/**
 * @brief snpu 获得工作状态
 *
 *
 * @return snpu 任务工作状态
 * @retval GX_SNPU_IDLE    空闲状态
 * @retval GX_SNPU_BUSY    工作忙状态
 * @retval GX_SNPU_STALL   停滞状态
*/
GX_SNPU_STATE gx_snpu_get_state(void);

#endif
