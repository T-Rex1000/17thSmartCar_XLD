/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,��ɿƼ�
 * All rights reserved.
 * ��������QQȺ����Ⱥ��824575535
 *
 * �����������ݰ�Ȩ������ɿƼ����У�δ����������������ҵ��;��
 * ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
 *
 * @file       		�����
 * @company	   		�ɶ���ɿƼ����޹�˾
 * @author     		��ɿƼ�(QQ3184284598)
 * @version    		�鿴doc��version�ļ� �汾˵��
 * @Software 		ADS v1.2.2
 * @Target core		TC264D
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2020-3-23
 * @note		
					���߶��壺
					------------------------------------ 
					ģ��ܽ�            			��Ƭ���ܽ�
					SDA(51��RX)         		�鿴SEEKFREE_MT9V03X.h�ļ��е�MT9V03X_COF_UART_TX�궨��
					SCL(51��TX)         		�鿴SEEKFREE_MT9V03X.h�ļ��е�MT9V03X_COF_UART_RX�궨��
					���ж�(VSY)         		�鿴SEEKFREE_MT9V03X.h�ļ��е�MT9V03X_VSYNC_PIN�궨��
					���ж�(HREF)				����û��ʹ�ã���˲�����
					�����ж�(PCLK)      		�鿴SEEKFREE_MT9V03X.h�ļ��е�MT9V03X_PCLK_PIN�궨��
					���ݿ�(D0-D7)			�鿴SEEKFREE_MT9V03X.h�ļ��е�MT9V03X_DATA_PIN�궨��
					------------------------------------ 
	
					Ĭ�Ϸֱ�����           			188*120
					Ĭ��FPS                 50֡
 ********************************************************************************************************************/


#include "IfxDma.h"
#include "IfxScuEru.h"
#include "zf_stm_systick.h"
#include "zf_gpio.h"
#include "zf_eru.h"
#include "zf_eru_dma.h"
#include "SEEKFREE_MT9V03X.h"


//����4�ֽڶ���
IFX_ALIGN(4) uint8 mt9v03x_image[MT9V03X_H][MT9V03X_W];

uint8   receive[3];
uint8   receive_num = 0;
vuint8  uart_receive_flag;

uint8   link_list_num;
int16 timeout = MT9V03X_INIT_TIMEOUT;

//��Ҫ���õ�����ͷ������
int16 MT9V03X_CFG[CONFIG_FINISH][2]=
{
    {AUTO_EXP,          0},   //�Զ��ع�����      ��Χ1-63 0Ϊ�ر� ����Զ��ع⿪��  EXP_TIME�������õ����ݽ����Ϊ����ع�ʱ�䣬Ҳ�����Զ��ع�ʱ�������
                              //һ������ǲ���Ҫ����������ܣ���Ϊ�������ع���һ�㶼�ȽϾ��ȣ�����������߷ǳ������ȵ�������Գ������ø�ֵ������ͼ���ȶ���
    {EXP_TIME,          250}, //�ع�ʱ��          ����ͷ�յ�����Զ����������ع�ʱ�䣬������ù���������Ϊ�������������ع�ֵ
    {FPS,               50},  //ͼ��֡��          ����ͷ�յ�����Զ���������FPS���������������Ϊ������������FPS
    {SET_COL,           MT9V03X_W}, //ͼ��������        ��Χ1-752     K60�ɼ�����������188
    {SET_ROW,           MT9V03X_H}, //ͼ��������        ��Χ1-480
    {LR_OFFSET,         0},   //ͼ������ƫ����    ��ֵ ��ƫ��   ��ֵ ��ƫ��  ��Ϊ188 376 752ʱ�޷�����ƫ��    ����ͷ��ƫ�����ݺ���Զ��������ƫ�ƣ�������������ü�����������ƫ��
    {UD_OFFSET,         0},   //ͼ������ƫ����    ��ֵ ��ƫ��   ��ֵ ��ƫ��  ��Ϊ120 240 480ʱ�޷�����ƫ��    ����ͷ��ƫ�����ݺ���Զ��������ƫ�ƣ�������������ü�����������ƫ��
    {GAIN,              32},  //ͼ������          ��Χ16-64     ����������ع�ʱ��̶�������¸ı�ͼ�������̶�
    {PCLK_MODE,         0},   //�������MT9V034 V2.0�Լ����ϰ汾֧�ָ����
                              //����ʱ��ģʽ���� PCLKģʽ     Ĭ�ϣ�0     ��ѡ����Ϊ��0 1��        0������������źţ�1����������źš�(ͨ��������Ϊ0�����ʹ��CH32V307��DVP�ӿڻ�STM32��DCMI�ӿڲɼ���Ҫ����Ϊ1)
    
    {INIT,              0}    //����ͷ��ʼ��ʼ��
};

//������ͷ�ڲ���ȡ������������
int16 GET_CFG[CONFIG_FINISH-1][2]=
{
    {AUTO_EXP,          0},   //�Զ��ع�����      
    {EXP_TIME,          0},   //�ع�ʱ��          
    {FPS,               0},   //ͼ��֡��          
    {SET_COL,           0},   //ͼ��������        
    {SET_ROW,           0},   //ͼ��������        
    {LR_OFFSET,         0},   //ͼ������ƫ����    
    {UD_OFFSET,         0},   //ͼ������ƫ����    
    {GAIN,              0},   //ͼ������
    {PCLK_MODE,         0},   //�����ź�
};


//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V03X����ͷ�����жϺ���
//  @param      NULL
//  @return     void					
//  @since      v1.0
//  Sample usage:	
//  @note       �˺�����isr.c�� �������жϺ�������
//-------------------------------------------------------------------------------------------------------------------
void mt9v03x_uart_callback(void)
{
	while(uart_query(MT9V03X_COF_UART, &receive[receive_num]))
	{
		receive_num++;

        if(1==receive_num && 0XA5!=receive[0])  receive_num = 0;
        if(3 == receive_num)
        {
            receive_num = 0;
            uart_receive_flag = 1;
        }

	}
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ��������ͷ�ڲ�������Ϣ
//  @param      uartn       ѡ��ʹ�õĴ���
//  @param      buff        ����������Ϣ�ĵ�ַ
//  @return     void
//  @since      v1.0
//  Sample usage:           ���øú���ǰ���ȳ�ʼ������
//-------------------------------------------------------------------------------------------------------------------
void set_config(UARTN_enum uartn, int16 buff[CONFIG_FINISH-1][2])
{
	uint16 temp, i;
    uint8  send_buffer[4];

    uart_receive_flag = 0;
    
    //���ò���  ������ο���������ֲ�
    //��ʼ��������ͷ�����³�ʼ��
    for(i=0; i<CONFIG_FINISH; i++)
    {
        send_buffer[0] = 0xA5;
        send_buffer[1] = (uint8)buff[i][0];
        temp = buff[i][1];
        send_buffer[2] = temp>>8;
        send_buffer[3] = (uint8)temp;
        
        uart_putbuff(uartn,send_buffer,4);
        systick_delay_ms(STM0, 2);
    }

    timeout = MT9V03X_INIT_TIMEOUT;
    //�ȴ�����ͷ��ʼ���ɹ�
    while(!uart_receive_flag && timeout-- > 0)
    {
        systick_delay_ms(STM0, 1);
    }
    uart_receive_flag = 0;
    timeout = MT9V03X_INIT_TIMEOUT;
    //�ȴ����ܻش�����
    while(((0xff != receive[1]) || (0xff != receive[2])) && timeout-- > 0)
    {
        systick_delay_ms(STM0, 1);
    }
    //���ϲ��ֶ�����ͷ���õ�����ȫ�����ᱣ��������ͷ��51��Ƭ����eeprom��
    //����set_exposure_time�����������õ��ع����ݲ��洢��eeprom��
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      ��ȡ����ͷ�ڲ�������Ϣ
//  @param      uartn       ѡ��ʹ�õĴ���
//  @param      buff        ����������Ϣ�ĵ�ַ
//  @return     void
//  @since      v1.0
//  Sample usage:           ���øú���ǰ���ȳ�ʼ������
//-------------------------------------------------------------------------------------------------------------------
void get_config(UARTN_enum uartn, int16 buff[CONFIG_FINISH-1][2])
{
	uint16 temp, i;
    uint8  send_buffer[4];
    
    for(i=0; i<CONFIG_FINISH-1; i++)
    {
        send_buffer[0] = 0xA5;
        send_buffer[1] = GET_STATUS;
        temp = buff[i][0];
        send_buffer[2] = temp>>8;
        send_buffer[3] = (uint8)temp;
        
        uart_putbuff(uartn,send_buffer,4);
        
        timeout = MT9V03X_INIT_TIMEOUT;
        //�ȴ����ܻش�����
        while(!uart_receive_flag && timeout-- > 0)
        {
            systick_delay_ms(STM0, 1);
        }
        uart_receive_flag = 0;
        
        buff[i][1] = receive[1]<<8 | receive[2];
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ��ȡ����ͷ�̼��汾
//  @param      uartn       ѡ��ʹ�õĴ���
//  @return     void
//  @since      v1.0
//  Sample usage:           ���øú���ǰ���ȳ�ʼ������
//-------------------------------------------------------------------------------------------------------------------
uint16 get_version(UARTN_enum uartn)
{
    uint16 temp;
    uint8  send_buffer[4];
    send_buffer[0] = 0xA5;
    send_buffer[1] = GET_STATUS;
    temp = GET_VERSION;
    send_buffer[2] = temp>>8;
    send_buffer[3] = (uint8)temp;
    
    uart_putbuff(uartn,send_buffer,4);
        
    timeout = MT9V03X_INIT_TIMEOUT;
    //�ȴ����ܻش�����
    while(!uart_receive_flag && timeout-- > 0)
    {
        systick_delay_ms(STM0, 1);
    }
    uart_receive_flag = 0;
    
    return ((uint16)(receive[1]<<8) | receive[2]);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ������������ͷ�ع�ʱ��
//  @param      uartn       ѡ��ʹ�õĴ���
//  @param      light       �����ع�ʱ��Խ��ͼ��Խ��������ͷ�յ������ݷֱ��ʼ�FPS��������ع�ʱ��������õ����ݹ�����ô����ͷ��������������ֵ
//  @return     uint16      ��ǰ�ع�ֵ������ȷ���Ƿ���ȷд��
//  @since      v1.0
//  Sample usage:           ���øú���ǰ���ȳ�ʼ������
//-------------------------------------------------------------------------------------------------------------------
uint16 set_exposure_time(UARTN_enum uartn, uint16 light)
{
	uint16 temp;
    uint8  send_buffer[4];

    send_buffer[0] = 0xA5;
    send_buffer[1] = SET_EXP_TIME;
    temp = light-30;
    send_buffer[2] = temp>>8;
    send_buffer[3] = (uint8)temp;
    
    uart_putbuff(uartn,send_buffer,4);
    
    timeout = MT9V03X_INIT_TIMEOUT;
    //�ȴ����ܻش�����
    while(!uart_receive_flag && timeout-- > 0)
    {
        systick_delay_ms(STM0, 1);
    }
    uart_receive_flag = 0;
    
    temp = receive[1]<<8 | receive[2];
    return temp;

}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      ������ͷ�ڲ��Ĵ�������д����
//  @param      uartn       ѡ��ʹ�õĴ���
//  @param      addr        ����ͷ�ڲ��Ĵ�����ַ
//  @param      data        ��Ҫд�������
//  @return     uint16      �Ĵ�����ǰ���ݣ�����ȷ���Ƿ�д��ɹ�
//  @since      v1.0
//  Sample usage:			���øú���ǰ���ȳ�ʼ������
//-------------------------------------------------------------------------------------------------------------------
uint16 set_mt9v03x_reg(UARTN_enum uartn, uint8 addr, uint16 data)
{
	uint16 temp;
    uint8  send_buffer[4];
    
    send_buffer[0] = 0xA5;
    send_buffer[1] = SET_ADDR;
    temp = addr;
    send_buffer[2] = temp>>8;
    send_buffer[3] = (uint8)temp;
    
    uart_putbuff(uartn,send_buffer,4);
    systick_delay_ms(STM0, 10);
    
    send_buffer[0] = 0xA5;
    send_buffer[1] = SET_DATA;
    temp = data;
    send_buffer[2] = temp>>8;
    send_buffer[3] = (uint8)temp;
    
    uart_putbuff(uartn,send_buffer,4);
    
    timeout = MT9V03X_INIT_TIMEOUT;
    //�ȴ����ܻش�����
    while(!uart_receive_flag && timeout-- > 0)
    {
        systick_delay_ms(STM0, 1);
    }
    uart_receive_flag = 0;
    
    temp = receive[1]<<8 | receive[2];
    return temp;

}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V03X����ͷ��ʼ��
//  @param      NULL
//  @return     void					
//  @since      v1.0
//  Sample usage:	ʹ��FLEXIO�ӿڲɼ�����ͷ	
//-------------------------------------------------------------------------------------------------------------------
void mt9v03x_init(void)
{
	uint8 i;
    camera_type = CAMERA_GRAYSCALE;//������������ͷ����
    camera_buffer_addr = mt9v03x_image[0];

    boolean interrupt_state = disableInterrupts();

    uart_init (MT9V03X_COF_UART, 9600, MT9V03X_COF_UART_TX, MT9V03X_COF_UART_RX);	//��ʼ������ ��������ͷ
    enableInterrupts();//�����ж�

    //�ȴ�����ͷ�ϵ��ʼ���ɹ� ��ʽ�����֣���ʱ����ͨ����ȡ���õķ�ʽ ��ѡһ
    //systick_delay_ms(STM0, 1000);//��ʱ��ʽ
    get_config(MT9V03X_COF_UART, GET_CFG);//��ȡ���õķ�ʽ

    uart_receive_flag = 0;
    set_config(MT9V03X_COF_UART, MT9V03X_CFG);

    //��ȡ���ñ��ڲ鿴�����Ƿ���ȷ
    get_config(MT9V03X_COF_UART, GET_CFG);

    disableInterrupts();

	//����ͷ�ɼ���ʼ��
	//��ʼ�� ��������
	for(i=0; i<8; i++)
	{
		gpio_init((PIN_enum)(MT9V03X_DATA_PIN+i), GPI, 0, PULLUP);
	}

    link_list_num = eru_dma_init(MT9V03X_DMA_CH, GET_PORT_IN_ADDR(MT9V03X_DATA_PIN), camera_buffer_addr, MT9V03X_PCLK_PIN, RISING, MT9V03X_W*MT9V03X_H);//�����Ƶ��300M �����ڶ�������������ΪFALLING

    eru_init(MT9V03X_VSYNC_PIN, FALLING);	//��ʼ�����жϣ�������Ϊ�½��ش����ж�
    restoreInterrupts(interrupt_state);
}


uint8   mt9v03x_finish_flag = 0;//һ��ͼ��ɼ���ɱ�־λ
uint8 	mt9v03x_lost_flag = 1;	//ͼ��ʧ��־λ
uint8	mt9v03x_dma_int_num;	//��ǰDMA�жϴ���
uint8 	mt9v03x_dma_init_flag;	//�Ƿ���Ҫ���³�ʼ��
//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V03X����ͷ���ж�
//  @param      NULL
//  @return     void			
//  @since      v1.0
//  Sample usage:				�˺�����isr.c�б�eru��GPIO�жϣ��жϵ���
//-------------------------------------------------------------------------------------------------------------------
void mt9v03x_vsync(void)
{
	CLEAR_GPIO_FLAG(MT9V03X_VSYNC_PIN);
	mt9v03x_dma_int_num = 0;
	if(mt9v03x_dma_init_flag || mt9v03x_lost_flag)
    {
        mt9v03x_dma_init_flag = 0;
        IfxDma_resetChannel(&MODULE_DMA, MT9V03X_DMA_CH);
        link_list_num = eru_dma_init(MT9V03X_DMA_CH, GET_PORT_IN_ADDR(MT9V03X_DATA_PIN), camera_buffer_addr, MT9V03X_PCLK_PIN, RISING, MT9V03X_W*MT9V03X_H);
        dma_start(MT9V03X_DMA_CH);
    }
    else
	{
		if(1 == link_list_num)
		{
			//û�в������Ӵ���ģʽ ��������Ŀ�ĵ�ַ
			DMA_SET_DESTINATION(MT9V03X_DMA_CH, camera_buffer_addr);
		}
		dma_start(MT9V03X_DMA_CH);
	}
	
	mt9v03x_lost_flag = 1;
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V03X����ͷDMA����ж�
//  @param      NULL
//  @return     void			
//  @since      v1.0
//  Sample usage:				�˺�����isr.c�б�dma�жϵ���
//-------------------------------------------------------------------------------------------------------------------
void mt9v03x_dma(void)
{
	CLEAR_DMA_FLAG(MT9V03X_DMA_CH);

	if(IfxDma_getChannelTransactionRequestLost(&MODULE_DMA, MT9V03X_DMA_CH))
    {	//ͼ���λ
        mt9v03x_finish_flag = 0;
        dma_stop(MT9V03X_DMA_CH);
        IfxDma_clearChannelTransactionRequestLost(&MODULE_DMA, MT9V03X_DMA_CH);
        mt9v03x_dma_init_flag = 1;
    }
    else
	{
		mt9v03x_dma_int_num++;
        if(mt9v03x_dma_int_num >= link_list_num)
        {
            //�ɼ����
            mt9v03x_dma_int_num = 0;
			mt9v03x_lost_flag = 0;
            mt9v03x_finish_flag = 1;//һ��ͼ��Ӳɼ���ʼ���ɼ�������ʱ3.8MS����(50FPS��188*120�ֱ���)
            dma_stop(MT9V03X_DMA_CH);
        }
	}
}



//-------------------------------------------------------------------------------------------------------------------
//  @brief      ���������ͷͼ��������λ���鿴ͼ��
//  @param      uartn			ʹ�õĴ��ں�
//  @param      image			��Ҫ���͵�ͼ���ַ
//  @param      width			ͼ�����
//  @param      height			ͼ�����
//  @return     void			
//  @since      v1.0
//  Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
void seekfree_sendimg_03x(UARTN_enum uartn, uint8 *image, uint16 width, uint16 height)
{
	uart_putchar(uartn,0x00);uart_putchar(uartn,0xff);uart_putchar(uartn,0x01);uart_putchar(uartn,0x01);//��������
    uart_putbuff(uartn, image, width*height);  //����ͼ��
}

