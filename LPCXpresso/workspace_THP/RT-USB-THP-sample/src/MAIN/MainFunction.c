/**
 * @file   MainFunction.c
 * @brief  データの送信を行う<br>
 *         10msec毎に割り込み関数で処理を実行
 *
 * @author RTCorp. Ryota Takahashi
 */


#include "LPC13xx.h"
#include "core_cm3.h"
#include "type.h"

#include "timer.h"
#include "MainFunction.h"
#include "UserInterface.h"
#include "bme280.h"

#include "debug.h"
#include "uart.h"


#include "usbTransmission.h"


///////uart.cに宣言があり/////////////////////////
extern volatile uint32_t UARTCount;
extern volatile uint8_t  UARTBuffer[BUFSIZE];
//////////////////////////////////////////////

volatile static uint8_t flag_send_data_enable = 0;

/**
 * MainFunction.c内の関数を使用できるようにする
 *
 * @param void
 * @return void
 */
void initMainFunction(void)
{
	startTimer32_1();
}

/**
 * データの送信をするかどうかを選択
 *
 * @param en 1:送信 0:送信じ禁止
 * @return void
 */
void setSendDataEnable(uint8_t en)
{
	flag_send_data_enable = en;
}

/**
 * 1000msec毎に行われる処理 <br>
 *          <li>各種情報をUART,USBで送信 </li>
 *         </ul>
 * @param  void
 * @return void
 */
void TIMER32_1_IRQHandler(void)
{

	static uint32_t timestamp;
	int32_t t_raw,p_raw, h_raw;



	LPC_TMR32B1->IR=0x08; //clear interrupt flag

	get_rawdata(&t_raw,&p_raw,&h_raw);  //生データ取得

	if(flag_send_data_enable == 1)
	{
		if(isUSBConfig() == 1)
		{

			myPrintfUSB("%d, %3.2f, %4.2f, %3.2f.\n",
					timestamp,
					BME280_compensate_T_double(t_raw)-4.0,
					BME280_compensate_P_double(p_raw)/100.0,
					bme280_compensate_H_double(h_raw)
			);

		}

		myPrintfUART("%d, %3.2f, %4.2f, %3.2f.\n",
				timestamp,
				BME280_compensate_T_double(t_raw)-4.0,
				BME280_compensate_P_double(p_raw)/100.0,
				bme280_compensate_H_double(h_raw)
		);
	}


	timestamp ++;

}



/******************************************************************************
**                            End Of File
******************************************************************************/
