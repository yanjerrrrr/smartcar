#include <rtdevice.h>
#include <rtthread.h>
#include <stdio.h>
#include "drv_gpio.h"

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

ALIGN(RT_ALIGN_SIZE)
static char thread1_stack[1024];
static struct rt_thread thread1; 

struct rt_semaphore sem1;

static void thread1_entry(void *param)
{
    rt_sem_take(&sem1, RT_WAITING_FOREVER);
}

rt_uint16_t width = 120;
rt_uint16_t height = 184;

int main(void)
{
    rt_thread_init(&thread1,
                   "thread1",
                   thread1_entry,
                   RT_NULL,
                   &thread1_stack[0],
                   sizeof(thread1_stack),
                   THREAD_PRIORITY - 1,
                   THREAD_TIMESLICE);
    
    rt_sem_init(&sem1, "sem1", 0, RT_IPC_FLAG_FIFO); // change the value to 1 when needed
    
    rt_thread_startup(&thread1);     

    rt_device_t cam = rt_device_find("MT9V034");
    rt_device_open(cam, 0);

    rt_uint8_t* buffer;
    rt_size_t buffer_size;

    while (1)
    {
        /* request a frame buffer */
        buffer_size = rt_device_read(cam, 0, &buffer, 0);
        if (buffer_size != 0)
        {
            rt_uint16_t window[9];
            /* Loop through the camera image */
            for (rt_uint16_t i = 0; i < buffer_size; ++i)
            {
                /* Access one pixel in the frame */
                buffer[i];
            
                /* implement median filter */
                for (rt_uint16_t w = 0; w < width-1; w++){ // width-1 or width
                    for (rt_uint16_t h = 0; h < height-1; h++){ // height-1 or height
                        rt_uint16_t k = 0;
                        for (rt_uint16_t u = w-1; u<= w+1; w++){
                            for (rt_uint16_t v = h-1; v <= h+1; v++){
                                window[k++] = buffer[width*v + u][]
                            }
                        }

                        for (rt_uint16_t a = 0; a < 5; a++){
                            for (rt_uint16_t b = a+1; b < 9; b++){
                                if (window[b] < window[a]){
                                    rt_uint16_t temp = window[a];
                                    window[a] = window[b];
                                    window[b] = temp;
                                }
                            }
                        }

                        buffer[i] = window[4];
                    }
                }

            }

            /* return the frame buffer back to queue */
            rt_device_write(cam, 0, &buffer, 0);
        }
        else
        {
            rt_thread_mdelay(1);
            rt_kprintf("%s", "Fail to create filter")
        }
    }

}

