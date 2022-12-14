#include "headfile.h"
//#include "control_line.h"
#pragma section all "cpu0_dsram"
IfxCpu_mutexLock mutexCpu0InitIsOk = 1;   // CPU0 初始化完成标志位
volatile char mutexCpu0TFTIsOk=0;  // CPU1 0占用/1释放 TFT


uint16 duty,duty_goal;
int dv;
int vl,vr=1200;
int32 speed1=000,speed2=000;
volatile uint8 select;

volatile float duty_k;
volatile int threshold_ad;
PIDTypedef *Pidautocontrol;
uint8 button=0;
uint32 IN_length=0;


void variable_init()
{
    select=1;
    vl=vr=1600;
    duty_k=3.0;
    threshold=0;
    pre=1;
    back=58;
    C0=0;
    C1=18;
    Pidautocontrol=&PID;
}
/*
void motor(int motor1,int motor2)
{
    if(motor1>=0)
    {
        gpio_set(P21_2,0);
        pwm_duty(ATOM0_CH1_P21_3, motor1);
    }
    else
    {
        gpio_set(P21_2,1);
        pwm_duty(ATOM0_CH1_P21_3, -1*motor1);
    }

    if(motor2>=0)
    {
        gpio_set(P21_4,0);
        pwm_duty(ATOM0_CH3_P21_5, motor2);
    }
    else
    {
        gpio_set(P21_4,1);
        pwm_duty(ATOM0_CH3_P21_5, -1*motor2);
    }
}
*/
void motor(int motor1,int motor2)
{
    if(motor1>=0)
    {
        pwm_duty(ATOM0_CH0_P21_2, motor1);
        pwm_duty(ATOM0_CH2_P21_4, 0);
    }
    else
    {
        pwm_duty(ATOM0_CH0_P21_2, 0);
        pwm_duty(ATOM0_CH2_P21_4, -1*motor1);
    }

    if(motor2>=0)
    {
        pwm_duty(ATOM0_CH1_P21_3, motor2);
        pwm_duty(ATOM0_CH3_P21_5, 0);
    }
    else
    {
        pwm_duty(ATOM0_CH1_P21_3, 0);
        pwm_duty(ATOM0_CH3_P21_5, -1*motor2);
    }
}


int core0_main(void)
{
    uint8 type_nums=3;
	disableInterrupts();
	get_clk();//获取时钟频率  务必保留
	duty=750;
	contemp="|     ";
	icm20602_init_spi();
	//icm20602_init();
	//seekfree_wireless_init();
    //显示屏
    show_init();
    //摄像头
    mt9v03x_init();
    //超声波
    gpio_init(P14_0,GPO,0,PULLUP);
    gpio_init(P14_1,GPI,0,PULLUP);
	//电机
	gtm_pwm_init(ATOM0_CH1_P21_3,7000,0);
	gtm_pwm_init(ATOM0_CH3_P21_5,7000,0);
	gtm_pwm_init(ATOM0_CH0_P21_2,7000,0);
    gtm_pwm_init(ATOM0_CH2_P21_4,7000,0);
	//LED
	/*
	gpio_init(P21_4, GPO, 1, PUSHPULL);
	gpio_init(P21_5, GPO, 0, PUSHPULL);
	*/
	gpio_init(P20_9, GPO, 0, PUSHPULL);
    gpio_init(P20_8, GPO, 0, PUSHPULL);
    //按键
    gpio_init(P22_0,GPI,1,PUSHPULL);
    gpio_init(P22_1,GPI,1,PUSHPULL);
    gpio_init(P22_2,GPI,1,PUSHPULL);
    gpio_init(P22_3,GPI,1,PUSHPULL);
    //舵机
    gtm_pwm_init(ATOM1_CH1_P33_9,50,duty);
    //蜂鸣器
    gpio_init(P33_10,GPO,0,PUSHPULL);
    //编码器
    gpt12_init(GPT12_T2,GPT12_T2INB_P33_7,GPT12_T2EUDB_P33_6);
    gpt12_init(GPT12_T6,GPT12_T6INA_P20_3,GPT12_T6EUDA_P20_0);

    //uart_init(UART_2,9600,UART2_TX_P10_5,UART2_RX_P02_1);

    pit_init(CCU6_0,PIT_CH0,5000);
    pit_enable_interrupt(CCU6_0,PIT_CH0);

    variable_init();
    ServoPIDParameterInit(&PID);
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);

	enableInterrupts();
	int count_speed_up=0,count_speed_down=0,count_all=1,speed_count_time=100;
	uint32 prespeed1=1000,prespeed2=1000,pre_D_speed1=0,pre_D_speed2=0,acc=0,acc_time=0,count_time=0,IN_count=0;
	int parking=0;
	while(1)
	{
        switch(select)
        {
            case 0://going
            {
                while(gpio_get(P22_1)==1&&gpio_get(P22_0)==1)
                {
                    if(type_F==0)
                    {
                        IN_count=1;
                    }
                    if(IN_count)
                    {
                        IN_length+=gpt12_get(GPT12_T6);
                    }
                    prespeed1=speed1;
                    prespeed2=speed2;
                    speed1=-1*gpt12_get(GPT12_T2);
                    speed2=gpt12_get(GPT12_T6);
                    gpt12_clear(GPT12_T2);
                    gpt12_clear(GPT12_T6);

                    DirControl();
                    systick_delay_ms(STM0,1);
                    /*
                    if(acc==0&&speed1<60&&speed2<60)
                    {
                        acc=2;
                        acc_time=systick_getval_ms(STM1);
                        button=1;
                    }
                    else if(acc==2&&systick_getval_ms(STM1)-acc_time>1000)
                    {
                        acc=1;
                        acc_time=systick_getval_ms(STM1);
                    }
                    else if(acc==1&&systick_getval_ms(STM1)-acc_time>500)
                    {
                        acc=0;
                        acc_time=systick_getval_ms(STM1);
                        button=0;
                    }
                    if(acc_time>=3000)
                        acc_time=0;
                    if(acc==2)
                    {
                        pre=back-C1;
                        motor(7000,7000);
                        gpio_toggle(P20_8);
                    }
                    else if(acc==1)
                    {
                        pre=1;
                        motor(0,0);
                    }
                    else
                    */

                    if(contemp[0]=='G'||IN_length>13000)
                    {
                        motor(0,0);

                        while(1)
                        {
                            parking++;
                            if(parking>10) pwm_duty(ATOM1_CH1_P33_9,750);
                            gpio_toggle(P20_8);
                            gpio_set(P33_10,button);
                            systick_delay_ms(STM0,100);
                        }
                    }
                    else if((contemp[0]=='T'||contemp[0]=='t')&&T_count==1)
                    {
                        motor(vl-100,vr-100);
                    }
                    else if(type_Y==5)
                    {
                        motor(vl-200,vr-200);
                    }
                    else if(PID.DirOut>=770||PID.DirOut<=730)
                    {
                        if(PID.DirOut>=800)
                            motor(vl-100,vr-250);
                        else if(PID.DirOut<=700)
                            motor(vl-250,vr-100);
                        else
                            motor(vl-100,vr-100);
                    }
                    else
                        motor(vl,vr);
                    gpio_set(P33_10,button);
                }
                break;
            }
            case 1://vl
            {
                while(gpio_get(P22_1)==1&&gpio_get(P22_0)==1)
                {
                    if(gpio_get(P22_2)==0)
                        if(vl<7000) vl+=100;
                    if(gpio_get(P22_3)==0)
                        if(vl>=100) vl-=100;
                    systick_delay_ms(STM0,120);
                }
                break;
            }
            case 2://vr
            {
                while(gpio_get(P22_1)==1&&gpio_get(P22_0)==1)
                {
                    if(gpio_get(P22_2)==0)
                        if(vr<7000) vr+=100;
                    if(gpio_get(P22_3)==0)
                        if(vr>=100) vr-=100;
                    systick_delay_ms(STM0,120);
                }
                break;
            }
            case 3://cenline[C1]
            {
                while(gpio_get(P22_1)==1&&gpio_get(P22_0)==1)
                {
                    if(gpio_get(P22_2)==0)
                        if(C1<back-pre) C1+=1;
                    if(gpio_get(P22_3)==0)
                        if(C0<C1) C1-=1;
                    systick_delay_ms(STM0,120);
                }
                break;
            }

        }
        if(gpio_get(P22_1)==0)
        {
            if(select<type_nums)select++;
            else select=0;
        }
        else if(gpio_get(P22_0)==0)
        {
            if(select>0)select--;
            else select=type_nums;
        }
        pwm_duty(ATOM1_CH1_P33_9,750);
        motor(0,0);
        systick_delay_ms(STM0,200);
	}
	return 0;
}

#pragma section all restore
