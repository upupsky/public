#include "px4uartgps.h"
#include <string.h>

//-----------vars define
//circle buffer, called by uart interrupt function
uint8_t	  circle_buffer_array[CIRCLE_BUFFER_LENGTH] = {0};
//read circle buffer ptr, eg:process function
uint32_t  circle_buffer_read_ptr  = 0;
//write circle buffer ptr, eg:uart interrupt function
uint32_t  circle_buffer_write_ptr = 0;
//the lock status @ref CIRCLE_BUFFER_LOCK/CIRCLE_BUFFER_UNLOCK
uint8_t   circle_buffer_lock      = 0;
//protocol deal buffer, called by process function
uint8_t   frame_data_buffer_array[FRAME_BUFFER_LENGTH] = {0};
//protocol deal byte counter
uint32_t  frame_data_counter_ptr  = 0;
//protocol deal status flag
uint8_t   frame_data_head_flag    = 0;

/*********************************************************************
 * @fn      uartInterrupt(uint8_t *p_data, uint32_t p_data_len)
 *
 * @brief   called by hardware interrupt function
 *
 *
 * @return  none
 */
void uartInterrupt(uint8_t *p_data, uint32_t p_data_len)
{
	//lock
	circle_buffer_lock = CIRCLE_BUFFER_LOCK;
	//get the data length of notify
	uint32_t receiver_length = p_data_len;

	//move the pointer check the pointer to the end
	if (circle_buffer_write_ptr + receiver_length > CIRCLE_BUFFER_LENGTH)
	{
		//get the remain buffer size
		uint32_t tmp_pointer = CIRCLE_BUFFER_LENGTH - circle_buffer_write_ptr;
		//full the remain buffer
		memcpy(circle_buffer_array + circle_buffer_write_ptr, p_data, tmp_pointer);

		//full the remain data from  circle_buffer_array's head
		memcpy(circle_buffer_array, p_data + tmp_pointer, (receiver_length - tmp_pointer));

		//move the write pointer
		circle_buffer_write_ptr = receiver_length - tmp_pointer;
	}
	else
	{
		//copy the datas to queue
		memcpy(circle_buffer_array + circle_buffer_write_ptr, p_data, receiver_length);
		//move the write pointer
		circle_buffer_write_ptr += receiver_length;
	}
	//unlock
	circle_buffer_lock = CIRCLE_BUFFER_UNLOCK;
}

/*********************************************************************
 * @fn      uartDataProcess()
 *
 * @brief   called by timer period, eg:1s interval
 *
 *
 * @return  none
 */
void uartDataProcess()
{
	if(circle_buffer_lock == CIRCLE_BUFFER_LOCK)
	{
		printf("process return\r\n");
		return ;
	}
	if(circle_buffer_read_ptr != circle_buffer_write_ptr)
	{
		frame_data_buffer_array[frame_data_counter_ptr] = circle_buffer_array[circle_buffer_read_ptr];

		//
		frame_data_counter_ptr += 1;

		//
		circle_buffer_read_ptr += 1;

		//
		if (circle_buffer_read_ptr >= CIRCLE_BUFFER_LENGTH)
			circle_buffer_read_ptr = 0;

		//******************protocol start*************************************
		//1.head=0
		if (frame_data_head_flag == 0)
		{
			//
			if (frame_data_buffer_array[frame_data_counter_ptr - 1] == 0x68)
			{
				//
				frame_data_head_flag = 1;
				//continue;
			}
			else
			{
				frame_data_head_flag = 0;
				frame_data_counter_ptr = 0;
			}
		}
		//2.head=1
		if (frame_data_head_flag == 1)
		{
			//
			if (frame_data_counter_ptr > 4)
			{


				if(frame_data_buffer_array[frame_data_counter_ptr - 1] == 0x0D && frame_data_buffer_array[frame_data_counter_ptr] == 0x0A)
				{
					printf("GPS Ready!\r\n");

					frame_data_head_flag = 0;
					frame_data_counter_ptr = 0;
				}
				if(frame_data_counter_ptr > 250)
				{
					frame_data_head_flag = 0;
					frame_data_counter_ptr = 0;
				}
			}
		}
	}

}







