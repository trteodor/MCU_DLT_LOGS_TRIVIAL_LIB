

#define DLT_LOG_CONTEX           "MAIN"
#define DLT_LOG_APPID            "NUM1"

#include "main.h"
#include "DLTuc.h"

#define UseSysTick

UART_DMA_Handle_Td *Uart2HandlerPointer;


/*CallBacks used by ucDltLibrary section start..*/

void UART2_TransmitDMAEndCallBack()
{

	DLTuc_MessageTransmitDone(); /*Inform ucDLTlib about message transmission end*/
		/*This way to make this example simple to read...*/
	/*
	*In case of STM32 HAL lib, you have to subsitute this function using:
	* void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart, uint16_t Size)
	*/
}


/*This CallBack was registered in main function using function: DLTuc_RegisterTransmitSerialDataFunction*/
void UART2_LowLevelDataTransmit(uint8_t *DltLogData, uint8_t Size)
{
	/*This function is called by ucDLTlib when is something to send*/

	TUART_DMA_Trasmit(Uart2HandlerPointer,DltLogData,Size); /*Call of this function may block contex!!! */
	/*
	* In case of STM32 HAL library you should use:
	* HAL_UART_Transmit_DMA(&huart2, DltLogData, Size);
	*/
		/*This way to make this example simple to read...*/
}
/*CallBacks used by ucDltLibrary section end..*/

void UART2_DataReceiveEndCallback(uint16_t Size)
{
	DLTuc_RawDataReceiveDone(Size);
	/*
	*In case of STM32 HAL lib, you have to subsitute this function using:
	*void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
	*/
}

void UART2_LowLevelReceiveDmaToIdle(uint8_t *rxBuf, uint16_t size)
{
	TUART_DMA_Receive(Uart2HandlerPointer, rxBuf, size);
	/*
	* In case of STM32 HAL library you should use:
	* HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rxBuf, size);
	*/
}

void DltInjectDataRcvd(uint32_t AppId, uint32_t ConId,uint32_t ServId,uint8_t *Data, uint16_t Size)
{
	LOG("RecInjectionData: %s, ServId: %d Size: %d", Data,ServId,Size)

	if(Data[0] == 'A')
	{
		tooglePIN(GPIOA, Psm5);
	}
}

int main(void) 
{
	/**********************************************************/
	/*Microcontoler Initialization start...*/
	LowLevelSystemInit(SleepMode);
	ConfigSysTick1ms();
	// ConfigRTC(40000, 1); /*1 Seconds*/
	GPIO_Pin_Cfg(GPIOA,Px5,gpio_mode_output_PP_2MHz);
	GPIO_PinSet(GPIOA, Psm5); /*Led is very exicited! :) */
	Uart2HandlerPointer = UART2_Init115200(UART2_TransmitDMAEndCallBack,UART2_DataReceiveEndCallback);
	/*Init UART with baud 115200 and pass pointer called after transmission end (transmit complet DMA)*/
	/*Microcontoler Initialization end...*/
	/**********************************************************/

	/*Register Low Level Transmit/Receive functions for DLTuc Library*/
	DLTuc_RegisterTransmitSerialDataFunction(UART2_LowLevelDataTransmit);
	DLTuc_RegisterReceiveSerialDataFunction(UART2_LowLevelReceiveDmaToIdle);

	DLTuc_RegisterGetTimeStampMsCallback(GetSysTime); 	/*Register GetSysTime function*/
	/*The function "GetSysTime" must return the time in ms*/

    DLTuc_RegisterInjectionDataReceivedCb(DltInjectDataRcvd);

	/*Now ucDLTlib is ready to work!*/
	LOGL(DL_INFO, "DLT TESTS START!!!");

	LOG("Compilation date: %s time: %s", __DATE__, __TIME__);

	for(int i=0; i<DLT_TRANSMIT_RING_BUFFER_SIZE; i++)
	{
		LOGL(DL_DEBUG, "Hello DLT with period 1ms");
		DelayMs(1);
	}
	DelayMs(500);
	/*LOG DROP TEST*/
	for(int i=0; i<DLT_TRANSMIT_RING_BUFFER_SIZE + 10 ; i++)
	{
		LOG("Log Drop testing :)  %d" , i);
	}
		LOGL(DL_INFO, "Dropped log...  %d" , 5);
		LOGL(DL_FATAL, "Dropped log...   %d" , 5);
	DelayMs(100);
	static uint32_t TimeStartLog = 0u;

	while(1)
	{
		TimeStartLog = GetSysTime();
		LOG("LogData TimeStart: %lu ",TimeStartLog )
		LOG("Compilation date: %s time: %s", __DATE__, __TIME__);
		// /*Send example Logs in loop...*/
		LOGL(DL_ERROR, "Hello DLT Again Arg1 %d Arg2 :%d" , 2565, 56);
		LOGFF(DL_FATAL, "FATAL LOG, ucTime: %d", GetSysTime());
		LOGF(DL_DEBUG, "AnotherTest DLT Again");
		LOG("AnotherTest2 DLT Again");
		LOG("LogData TimeEnd: %lu DELTA:%lu",GetSysTime(),GetSysTime() - TimeStartLog );
		DelayMs(1000);
		LOGL(DL_WARN, "Orange it's a sweet fruit");
		DelayMs(3000);
	}
}


