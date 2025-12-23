/*


************************************************


* File: mpu6050.c


* Function: Motion tracking sensor source file


* Specs: 3-axis accel + 3-axis gyro


*       3-axis accelerometer, 3-axis gyroscope


* Author: Lijh


************************************************


*/





// Include header files


#include "MPU6050.h"





// I2Cbase operation macros


#define I2C2_SendSTART()  I2C_GenerateSTART(I2C2, ENABLE)  // Start


#define I2C2_SendSTOP()   I2C_GenerateSTOP(I2C2, ENABLE)   // Stop





#define I2C2_OpenAck()    I2C_AcknowledgeConfig(I2C2, ENABLE)  // Enable Ack


#define I2C2_ColseAck()   I2C_AcknowledgeConfig(I2C2, DISABLE) // Disable Ack





/****I2C操作event wait macros************************


*EV5 - Event 5, 事件5: I2CStart位Send Complete  


*EV6 - Event 6, 事件6: I2C’Write’slaveaddressSend Complete  


*EV6 - Event 6, 事件6: I2C'Read‘slaveaddressSend Complete  


*EV5 - Event 7, 事件7: I2CReceive bytedataComplete  


*EV5 - Event 8, 事件8: I2CSend bytedataComplete  


******************************************************/


#define I2C2_START_Wait()        while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))     // EV5


#define I2C2_SendWrAddr_Wait()   while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) // EV6 Writeslave


#define I2C2_SendRdAddr_Wait()   while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ))   // EV6 Readslave


#define I2C2_ReceiveData_Wait()  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED))	  	// EV7


#define I2C2_SendData_Wait()     while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))  // EV8





/*function*/


// I2Cinitialization


void MPU6050_I2C2_Init(void);


// MPU6050registerSend bytedataregisteraddressdata


void MPU6050_WriteReg(uint8_t regaddr, int8_t data);


// MPU6050registerReadbytedataaddress


uint8_t MPU6050_ReadReg (uint8_t regaddr);





// MPU6050Configure


void MPU6050Init(void)


{


	// I2Cinitialization


	MPU6050_I2C2_Init();


	// Configure 5register-Refer to manual


	MPU6050_WriteReg(PWR_MGMT_1,   0x00);	// Wake up sleep


	MPU6050_WriteReg(ACCEL_CONFIG, 0x01); // Accelerometerself-testmeasurementrangefilterfrequencytypical0x01(self-test2G5Hz))[Reg_No.28,RegP13]


	MPU6050_WriteReg(SMPLRT_DIV,   0x07); // Gyroscopesample ratetypical0x07(1KHz) [Reg_No.25,RegP10]


	MPU6050_WriteReg(CONFIG,       0x06); // External sync and LPF filtersettings5Hz[Reg_No.26,RegP11]


	MPU6050_WriteReg(GYRO_CONFIG,  0x18); // Gyroscopeself-testmeasurementrange0x18=000 11 000(self-test,2000deg/s)[Reg_No.27,RegP12]


}





// I2Cinitialization


void MPU6050_I2C2_Init(void)


{


   GPIO_InitTypeDef GPIO_InitStructure;


	 I2C_InitTypeDef  I2C_InitStructure;





	// Enable device clock


	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE); /*GPIOH*/


	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);  /*I2C2*/


	


	 // Enable PH3-SCK, PH5-SDAI2CAF


	 GPIO_PinAFConfig(GPIOH, GPIO_PinSource4, GPIO_AF_I2C2);


	 GPIO_PinAFConfig(GPIOH, GPIO_PinSource5, GPIO_AF_I2C2);


	


	 // initializationGPIO PH4, PH5, AF mode


	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    /*AF mode*/


	 GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;  /*OpenDrain-OpenDrain output*/


	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;


	 GPIO_Init(GPIOH, &GPIO_InitStructure);





	 // I2C2 Configure


	 I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;          /* Work mode*/


	 I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;  /* Duty cycle 2/(1+2) */


	 I2C_InitStructure.I2C_ClockSpeed = 100*10^3;        /*clockspeed max. 400kHz*/								


	 I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;         /* Enable Ack*/	


// I2C_InitStructure.I2C_OwnAddress1 = 0x80;  /*MCUI2Caddress*/


// I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; /*MCUslaveaddr bits*/


   I2C_Init(I2C2, &I2C_InitStructure);  /*Complete Configure */


	 I2C_Cmd(I2C2, ENABLE);               /*Enable I2C2*/


}





// MPU6050registerSend bytedataregisteraddressdata


void MPU6050_WriteReg(uint8_t regaddr, int8_t data)


{


	I2C2_SendSTART();   /*Start*/


	I2C2_START_Wait();  /*EV5-Wait StartComplete */





	I2C_SendData(I2C2, MPU6050_Address & 0xFE); /*Send MPU6050addressendWrite bit 0)*/


	I2C2_SendWrAddr_Wait() ;                    /*EV6 - Wait WriteaddressSend Complete */


	


	I2C_SendData(I2C2, regaddr); /*Send MPU6050registeraddress*/


	I2C2_SendData_Wait();        /*EV8 - Wait Send Complete */


	


	I2C_SendData(I2C2, data);  /*Send bytedata*/


	I2C2_SendData_Wait();      /*EV8 - Wait Send Complete */


	


	I2C2_SendSTOP(); /*Stop*/


}	





// MPU6050registerReadbytedataaddress


uint8_t MPU6050_ReadReg (uint8_t regaddr)


{


	uint8_t data;


	


	I2C2_SendSTART();    /*Start*/


	I2C2_START_Wait();   /*EV5-Wait StartComplete */


	


	I2C_SendData(I2C2, MPU6050_Address & 0xFE); /*Send MPU6050addressendWrite bit 0)*/


  I2C2_SendWrAddr_Wait();                     /*EV6 - Wait WriteaddressSend Complete */


	


	I2C_SendData(I2C2, regaddr); /*dataSend MPU6050registeraddress*/	


  I2C2_SendData_Wait();        /*EV8 - Wait Send Complete */


	


	// StartReaddata


	I2C2_SendSTART();   /*Start*/


	I2C2_START_Wait();  /*EV5-Wait StartComplete */


	


	I2C_SendData(I2C2, MPU6050_Address | 0x01); /*Send MPU6050addressendRead bit 1)*/


	I2C2_SendRdAddr_Wait();                     /*EV6 - Wait ReadaddressSend Complete */


	


	I2C2_ColseAck();  /*masterAck*/


	


  I2C2_ReceiveData_Wait();      /*EV7-Wait dataReceive */


  data = I2C_ReceiveData(I2C2); /*Readdata*/


	


	I2C2_SendSTOP();  /*Stop*/


	I2C2_OpenAck();   /*masterAck*/


	


  return data;      /*data*/


}	





// MPU6050Readdata - 6bytespeed2byte6byteGyroscope14byte


void MPU6050ReadData (MPU6050_DataTypeDef *pM6050)


{


	uint8_t i;


	uint8_t readbuf[14];


	// Provide speedbyteaddressauto-increment via loopaddressReaddataregister


	for (i = 0 ; i < 6; i++)


	{


		readbuf[i] = MPU6050_ReadReg(Accel_Reg_XH + i);


	}


		


	/* speeddataOrganizeSavestructure*/


	pM6050 -> AccelDataX = (readbuf[0] << 8) + readbuf[1]; // XbyteCombine


	pM6050 -> AccelDataY = (readbuf[2] << 8) + readbuf[3]; // Y


	pM6050 -> AccelDataZ = (readbuf[4] << 8) + readbuf[5]; // Z


}




