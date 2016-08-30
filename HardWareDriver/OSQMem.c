#include "stdio.h"
#include "OSQMem.h"				
OSMEMTcb OSMemTcb[OS_MEM_MAX];						//�ڴ����������壬�������OS_MEM_MAX���ڴ���

u8 OSUSART1MemQ[OS_MEM_USART1_MAX];  				//�հ��ڴ��,OS_MEM_USART1_MAX��ʾ�ڴ�����е��������



/*******************************************************************************
* �ļ���	  	 : OSMemInit
* ����	         : �հ��ڴ��ĳ�ʼ��������
* ��ֲ����		 : ��
* ����           : ptr�հ������ڴ�����ָ�룬count�հ������ڴ����ڵ�������
* ���           : ��
* ����           : �ڴ����׵�ַ
*******************************************************************************/
OSMEMTcb *OSMemInit(u8 *ptr,u16 count)
{
	u16 i;
	for(i=0;i<count;i++)
	{
		ptr[i]=0;
	}
	return 	(OSMEMTcb*)*ptr;
}

/*******************************************************************************
* �ļ���	  	 : OSMemCreate
* ����	         : ����һ���ڴ��������������ο�UCOS���ڴ�����ṹ��
* ��ֲ����		 : ��
* ����           : ptr�հ������ڴ�����ָ�룬 blksizeÿ���ڴ���ж��ٸ��ֽڣ�
				   nblks���ٸ��ڴ�飬
* ���           : ��
* ����           : �ڴ����׵�ַ
*******************************************************************************/		
OSMEMTcb *OSMemCreate(u8 *ptr,u8 blksize,u8 nblks,u8 *err)
{
	u8 *link=(ptr+blksize);
	void **plink=(void **)ptr;
	u16 i=0;
	u16 j=0;
	/*���Ȳ��ҿհ׵��ڴ������*/
	while((OSMemTcb[i].OSMemFreeList!=(u8*)0)&&(i<=OS_MEM_MAX))		
	{														
		i++;
	}
	if(i>=OS_MEM_MAX){*err=0xff;return (OSMEMTcb *)0;}		//�ڴ����������䲻�������ؿ�ָ��						
	OSMemTcb[i].OSMemFreeList=ptr;							//�ڴ����������ڴ��ָ��
	OSMemTcb[i].OSMemBlkSize=blksize;						//�ڴ���������ÿ���ڴ����ֽ�
	OSMemTcb[i].OSMemNBlks=nblks;							//�ڴ����������ڴ���������
	OSMemTcb[i].OSMemFreeNBlks=nblks;						//�ڴ��������Ŀհ��ڴ�������
	j=i;
	OSMemInit(ptr,blksize*nblks);							//���ڴ���ڵ����ݳ�ʼ��
	for(i=0;i<nblks-1;i++)									//
	{
		*plink=(void *)(link);								//���ڴ��ĵ�ַ��ŵ��ǵڶ�Ƭ�ڴ������׵�ַ
		plink=(void **)(link);								//����άָ�붨λ�������λ		
		link+=blksize;										//һάָ�����¶�λ
	}
//	*plink=(void *)0;										//���һ���ڴ�������һ����ַ�������Ϊ0
	return (OSMEMTcb *)(OSMemTcb+j); 										//�����ڴ����������׵�ַ
}	
/*******************************************************************************
* �ļ���	  	 : OSMemGet
* ����	         : ��һ���ڴ��������ȡһ���ڴ��
* ��ֲ����		 : ��
* ����           : ptr�ڴ��������ָ��
* ���           : ��
* ����           : ��ȡ�Ŀհ��ڴ����׵�ַ
*******************************************************************************/
u8 *OSMemGet(OSMEMTcb *ptr,u8 *err)
{
	void *tcb;
	u8 *index;
	tcb=(*ptr).OSMemFreeList;
	if((*ptr).OSMemFreeNBlks==0){*err=0xff;return (void *)0;}//����հ��ڴ�������Ϊ����,����ȷ���أ��յ�������Ӧ����0
	(*ptr).OSMemFreeNBlks--;								//�հ��ڴ���������һ
	if((*ptr).OSMemFreeNBlks!=0)(*ptr).OSMemFreeList=*(void **)tcb;	//ǿ������ת��
	index=(u8 *)tcb;
	index+=4;
	*err=0;
	return index;	 										//���ػ�ȡ�Ŀհ��ڴ����׵�ַ
}
/*******************************************************************************
* �ļ���	  	 : SMemDelete
* ����	         : ��һ���ڴ������ɾ��һ���ڴ��
* ��ֲ����		 : ��
* ����           : ptr�ڴ��������ָ��
* ���           : ��
* ����           : ���Ҫɾ�����ڴ����һ����ָ�룬�򷵻�0xff�����ܹ�ɾ��������0
*******************************************************************************/
u8 OSMemDelete(OSMEMTcb *ptr,u8 *index)
{
	void *tcb;
	if(index==(void*)0)return 0xff;					   		//���Ҫɾ�����ڴ����һ����ָ�룬�򷵻�0xff
	index-=4;
	tcb=(void *)index;	
	*(void **)tcb=(*ptr).OSMemFreeList;						//��OSMemFreeList����ָ������Ѿ���ɿհ��˵�ָ��
	(*ptr).OSMemFreeList=tcb;								//������հ׵�ָ�����һ��ָ��ָ��һ��ԭ�ȵ�OSMemFreeList												//
	(*ptr).OSMemFreeNBlks++;								//�հ��ڴ��������1
	return 0;	 		
}

//------------------End of File----------------------------