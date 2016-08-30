#include "OSQMem.h"
#include "USART1DMATX.h"

#define USART1_SEND_MAX_Q	  	(OS_MEM_USART1_BLK-4)	//�����ڴ���ڵ����ռ�
#define USART1_SEND_MAX_BOX		20	   					//�����ڴ����������

unsigned char USART1SendQBuffer[USART1_SEND_MAX_BOX][USART1_SEND_MAX_Q];//�����ڴ��	
unsigned char USART1SendQBoxHost=0;			//�ڴ��ͷָ��							
unsigned char USART1SendQBoxTail=0;			//�ڴ��βָ��
unsigned int  USART1SendQFree=USART1_SEND_MAX_BOX;   
unsigned char USART1SendOVF=0; 				//USART1��������������־
unsigned char USART1RunningFlag=0;
typedef struct{
unsigned char Num;
unsigned char *Index;
}USART1SendTcb;
USART1SendTcb USART1SendTCB[USART1_SEND_MAX_BOX];

#define USART1_RECV_MAX_Q	  	32		//�ڴ���ڵ����ռ�
unsigned char USART1QRecvBuffer[USART1_RECV_MAX_Q];//�����ڴ��	
unsigned char USART1RecvOVF=0; 				//USART2��������������־  
unsigned int Recv1Index=0x00;
unsigned int Recv1Count=0x00;
unsigned char USART1RecvFlag=0;

extern u8 OSUSART1MemQ[OS_MEM_USART1_MAX];  			//�հ��ڴ��
extern OSMEMTcb* OSQUSART1Index;

void USART1DMAUpdate(void);
//������
#define ERR_NO_SPACE	0xff


void USART1DMAConfig(u8* TxBuffer1,u16 num)
{
    DMA_InitTypeDef     DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
    /* DMA1 Channel4 (triggered by USART1 Tx event) Config */
    DMA_DeInit(DMA2_Stream7);
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART1->DR));
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)TxBuffer1;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = num;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//������byte
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream7, &DMA_InitStructure);
	
	DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE);
	
	/* Enable the DMA2_Stream7 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	DMA_Cmd(DMA2_Stream7, ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);	 // ʹ�ܷ���DMA
	USART_Cmd(USART1, ENABLE);
}

/*******************************************************************************
* �ļ���	  	 : USART1WriteDataToBuffer
* ����	         : ��鷢�ͻ������Ĵ�С�����ռ��㹻���������͵����ݷ��뵽���ͻ���
				   ����ȥ,������������
* ����           : buffer�����͵����ݵ�ָ�룬count�����͵����ݵ�����
* ���           : ��
* ����           : ����ȷ���뵽���ͻ�������ȥ�ˣ��ͷ���0x00	 �����򷵻�0x01
*******************************************************************************/
unsigned char USART1WriteDataToBuffer(unsigned char *buffer,unsigned char count)
{
	unsigned char i=count;
	u8 err;
	/*�˴����Լ����źŵƻ��߹ر��ж�*/
	if(count==0) return 0;
	/*�������count��������Ҫ���ٸ��ڴ��*/
	if(count%USART1_SEND_MAX_Q)count=count/USART1_SEND_MAX_Q+1;
	else count=count/USART1_SEND_MAX_Q;
	/*��Ҫcount�����ݿ�*/
	/*����ڴ治�㣬ֱ�ӷ���*/		 
	if(USART1SendQFree<count){return ERR_NO_SPACE;}
	//���������ڴ�飬USART1SendQBoxHost����һ���ڴ������ż�һ
	USART1SendTCB[USART1SendQBoxHost].Index=(u8 *)OSMemGet(OSQUSART1Index,&err);
	if(USART1SendQBoxHost>=USART1_SEND_MAX_BOX)USART1SendQBoxHost=0;	
	count=0;
	while(i!='\0')										 
	{
		*(USART1SendTCB[USART1SendQBoxHost].Index+count)=*buffer;
		count++;
		if(count>=USART1_SEND_MAX_Q)
		{
			USART1SendTCB[USART1SendQBoxHost].Num=USART1_SEND_MAX_Q;
			//��Ҫһ���µ��ڴ���Ž����������ݣ����Ը���USART1SendQBoxHost
			if(++USART1SendQBoxHost>=USART1_SEND_MAX_BOX)USART1SendQBoxHost=0;
			//��Ҫһ���µ��ڴ���Ž�����������	
			USART1SendTCB[USART1SendQBoxHost].Index=(u8 *)OSMemGet(OSQUSART1Index,&err);
			//�յķ���������һ 			
			USART1SendQFree--;
			count=0;
		}
		buffer++;
		i--;
	}
	//�˴�����δ�����������ݣ�����ҲҪ�����һ���µ��ڴ����
	if(count!=0)
	{
		USART1SendTCB[USART1SendQBoxHost].Num=count; 
		USART1SendQFree--;
		if(++USART1SendQBoxHost>=USART1_SEND_MAX_BOX)USART1SendQBoxHost=0;
	}
	//����ǵ�һ�Σ����������ͣ�������Ѿ�������û�������Ҫ��
	if(USART1RunningFlag==0)
	{
#if	  	DMA_MODE
		USART1DMAConfig(USART1SendTCB[USART1SendQBoxTail].Index,USART1SendTCB[USART1SendQBoxTail].Num);
#else	
		USART1SendUpdate();
#endif		
		USART1RunningFlag=1;
	}

	return 0x00;
}

/*******************************************************************************
* �ļ���	  	 : USART1DispFun
* ����	         : ��鷢�ͻ������Ĵ�С�����ռ��㹻���������͵����ݷ��뵽���ͻ���
				   ����ȥ,������������,��USART1WriteDataToBuffer��ͬ���ǣ���������
				   ����������Ҫָ���ļ���С�ģ���͸������ṩ�˷���.
* ����           : buffer�����͵����ݵ�ָ��
* ���           : ��
* ����           : ����ȷ���뵽���ͻ�������ȥ�ˣ��ͷ���0x00	 �����򷵻�0x01
*******************************************************************************/
unsigned char USART1DispFun(unsigned char *buffer)
{
	unsigned long count=0;
	while(buffer[count]!='\0')count++;
	return(USART1WriteDataToBuffer(buffer,count));
}

/*******************************************************************************
* �ļ���	  	 : USART1DMAUpdate.c
* ����	         : USART_DMA����������
* ��ֲ����		 : �м�㺯��
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void USART1DMAUpdate(void)
{
	if(USART1SendQBoxTail!=USART1SendQBoxHost)
	{
		OSMemDelete(OSQUSART1Index,USART1SendTCB[USART1SendQBoxTail].Index);
		if(++USART1SendQBoxTail>=USART1_SEND_MAX_BOX)USART1SendQBoxTail=0;
		if(++USART1SendQFree>=USART1_SEND_MAX_BOX)USART1SendQFree=USART1_SEND_MAX_BOX;
		if(USART1SendQBoxTail!=USART1SendQBoxHost)
		{
			USART1DMAConfig(USART1SendTCB[USART1SendQBoxTail].Index,USART1SendTCB[USART1SendQBoxTail].Num);	
		}
		else USART1RunningFlag=0;	
	}
	else 
	{		
		OSMemDelete(OSQUSART1Index,USART1SendTCB[USART1SendQBoxTail].Index);
		if(++USART1SendQBoxTail>=USART1_SEND_MAX_BOX)USART1SendQBoxTail=0;
		if(++USART1SendQFree>=USART1_SEND_MAX_BOX)USART1SendQFree=USART1_SEND_MAX_BOX;
		USART1RunningFlag=0;
	}	
}

//------------------End of File----------------------------
