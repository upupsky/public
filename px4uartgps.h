#ifndef PX4UARTGPS_H
#define PX4UARTGPS_H


#include "stm32l1xx_hal.h"

//-----------macro define
#define   CIRCLE_BUFFER_LOCK  	  0
#define   CIRCLE_BUFFER_UNLOCK	  1
#define   CIRCLE_BUFFER_LENGTH    512
#define   FRAME_BUFFER_LENGTH     128


//-----------vars define
//circle buffer, called by uart interrupt function
extern uint8_t	  circle_buffer_array[CIRCLE_BUFFER_LENGTH];
//read circle buffer ptr, eg:process function
extern uint32_t  circle_buffer_read_ptr;
//write circle buffer ptr, eg:uart interrupt function
extern uint32_t  circle_buffer_write_ptr;
//the lock status @ref CIRCLE_BUFFER_LOCK/CIRCLE_BUFFER_UNLOCK
extern uint8_t   circle_buffer_lock;
//protocol deal buffer, called by process function
extern uint8_t   frame_data_buffer_array[FRAME_BUFFER_LENGTH];
//protocol deal byte counter
extern uint32_t  frame_data_counter_ptr;
//protocol deal status flag
extern uint8_t   frame_data_head_flag;

/*********************************************************************
 * @fn      uartInterrupt(uint8_t *p_data, uint32_t p_data_len)
 *
 * @brief   called by hardware interrupt function
 *
 *
 * @return  none
 */
extern void uartInterrupt(uint8_t *p_data, uint32_t p_data_len);


/*********************************************************************
 * @fn      uartDataProcess()
 *
 * @brief   called by timer period, eg:1s interval
 *
 *
 * @return  none
 */
extern void uartDataProcess();


#endif

