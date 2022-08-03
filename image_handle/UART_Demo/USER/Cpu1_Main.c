#include "headfile.h"
#include "LOGO.h"
//#include "control_line.h"
#pragma section all "cpu1_dsram"

//#define IPS200
#define TFT18

uint8 LCC[6][16]={
        {0x00,0x00,0x00,0x40,0x60,0x80,0x37,0xFE,0x11,0x00,0x01,0x20,0x02,0x20,0x73,0xFC},
        {0x10,0x20,0x10,0x20,0x17,0xFE,0x10,0x20,0x10,0x20,0x38,0x20,0x47,0xFE,0x00,0x00},/*"连",0*/

        {0x00,0x00,0x18,0x20,0x18,0x20,0x18,0x20,0x18,0x20,0x7F,0x26,0x59,0x26,0x59,0x26},
        {0x51,0x26,0x11,0x26,0x19,0x26,0x1D,0x26,0x37,0x26,0x21,0x7E,0x61,0xC6,0x00,0x06},/*"灿",1*/

        {0x00,0x00,0x30,0x28,0x30,0x24,0x30,0x28,0x33,0xFE,0x7A,0x20,0x32,0x24,0x33,0xE4},
        {0x32,0xF4,0x32,0x94,0x32,0x98,0x3E,0x98,0x45,0x9A,0x0D,0x3E,0x18,0xE6,0x00,0x00},/*"城",2*/
};

#ifdef IPS200
void show_init()
{
    show_select=1;
    ips200_init();
    ips200_clear(0x0002);
    logo(120,70,20,35,35,BLUE);
    IPS200_BGCOLOR=BLACK;
    IPS200_PENCOLOR=BLUE;
    ips200_showstr(75,8,"made by LCC");
    systick_delay_ms(STM0,3000);
    //小logo
    logo(215,30,10,0,0,BLUE);
}
//int X=MT9V03X_W>IPS200_X_MAX?IPS200_X_MAX:MT9V03X_W,Y=MT9V03X_H>IPS200_Y_MAX?IPS200_Y_MAX:MT9V03X_H;
int MT9V03X_image(int x,int y)
{
    int coord_x = MT9V03X_W>IPS200_X_MAX?IPS200_X_MAX:MT9V03X_W;
    int coord_y = MT9V03X_H>IPS200_Y_MAX?IPS200_Y_MAX:MT9V03X_H;
    int point = y*MT9V03X_W+x*MT9V03X_W/coord_x;
    return *(mt9v03x_image[0]+point);
}
_Bool MT9V03X_01(int x,int y)
{
    int coord_x = MT9V03X_W>IPS200_X_MAX?IPS200_X_MAX:MT9V03X_W;
    int coord_y = MT9V03X_H>IPS200_Y_MAX?IPS200_Y_MAX:MT9V03X_H;
    int point = y*MT9V03X_W+x*MT9V03X_W/coord_x;
    return *(mt9v03x_image[0]+point)>=gray;
}
#endif

#ifdef TFT18
void show_init()
{
    show_select=0;
    lcd_init();
    lcd_clear(0);
    logo(64,30,10,28,35,BLUE);
    TFT_PENCOLOR=BLUE;
    TFT_BGCOLOR=BLACK;
    lcd_showstr(20,4,"made by LCC");
    //systick_delay_ms(STM0,3000);
    //小logo
    logo(110,15,4,0,0,BLUE);
    lcd_display_chinese(102,33,16,LCC[0],1,WHITE,0);
}

int MT9V03X_image(int x,int y)
{
    int coord_x = MT9V03X_W;
    int coord_y = MT9V03X_H>TFT_Y_MAX?TFT_Y_MAX:MT9V03X_H;
    int point = y*MT9V03X_W+x*MT9V03X_W/coord_x;
    return *(mt9v03x_image[0]+point);
}
/*
_Bool MT9V03X_01(int x,int y)
{
    int coord_x = MT9V03X_W;
    int coord_y = MT9V03X_H>TFT_Y_MAX?TFT_Y_MAX:MT9V03X_H;
    int point = y*MT9V03X_W+x*MT9V03X_W/coord_x;
    return *(mt9v03x_image[0]+point)>=threshold;
}
*/
#endif

long wait_time = 0;                //超时检测变量

uint32 distance_time = 0;          //测距的时间变量

uint32 distance = 0;               //转换出来的距离  单位为厘米


uint32 i=0,j=0,time=0;

float real_angle1=0;
float real_angle()
{
    float acc_x, acc_z, angle;
    float K1 = 0.170 , dt=0.005;
    get_icm20602_gyro_spi();
    get_icm20602_accdata_spi();
    //if(icm_acc_y > 1) icm_acc_y =  -1;
    angle = (icm_gyro_x / 16.4);                                //化单位，查手册得到灵敏度值16.4，消除零点漂移
    lcd_showint16(70,8,angle);
    acc_x = icm_acc_x * 9.8 / 4096.0;                                //化单位，查手册得到灵敏度值4096
    acc_z = icm_acc_z * 9.8 / 4096.0;                                //化单位
    real_angle1 = K1 * atan((acc_z + 0.1) / acc_x) * 57.3 + (1 - K1) * (real_angle1 + angle * dt);
    lcd_showint16(70,9,real_angle1);
    return real_angle1;
}

float angle()
{
    float angle=0,dt=0.005;
    real_angle1+=(icm_gyro_y / 16.4)*dt;
    return real_angle1;
}

void bc_dis()
{

    gpio_set(P14_0,1);                           //触发引脚输出高电平

    systick_delay_us(STM1, 10);                     //延时10us

    gpio_set(P14_0,0);



    while(!gpio_get(P14_1));                     //检测到接收引脚为高电平则开始计时


    systick_start(STM1);

    wait_time = 0;

    while(gpio_get(P14_1))                       //检测到接收引脚为低电平则结束计时
    {
        wait_time++;                                //超时检测


        if(wait_time>200000) break;                 //超过等待时间

    }

    if(wait_time<6666) distance_time = systick_getval_us(STM1) ; //获取定时的时间

    else                  distance_time = 6666;

    //pit_close(CCU6_0,PIT_CH0);                             //关闭定时器以清除计时



    distance = distance_time*340/2/10000;            //计算距离  单位毫米



    //通过在线调试时 Live Watch（IAR）或 Watch（MDK）功能，查看 distance 的数值即可。

    //使用方法，右键点击变量例如 distance ，Add to Live Watch(IAR) 或 add‘distance'to...->watch1(MDK),即可在watch窗口看到变量实时值。



    systick_delay_ms(STM0,10);

}
void TrigInit()//???????
{
    gpio_set(P02_8,0);
    gpio_set(P02_8,1);
    systick_delay_us(STM1,10);
    gpio_set(P02_8,0);
    gpio_toggle(P20_9);
}
void Bc_Dis()
{
    if(gpio_get(P00_9)==0);
        i=systick_getval_us(STM1);//??????,Echo?1,????while(Echo==0)??,???????
    if(gpio_get(P00_9)==1);
        j=systick_getval_us(STM1);//??????,Echo?0,????,???????
    time=(i*256+j)*1.085/1000000;//?S???
    i=time*17000;//?cm???,????


}
void show_variable_adjust()
{
    int con_select_x=5;
    int con_select_y=6;
    TFT_PENCOLOR=BLACK;
    TFT_BGCOLOR=BLUE;
    lcd_showstr(con_select_x+4,con_select_y,"        ");
    switch(select)
    {
        case 0:
        {
            lcd_showstr(con_select_x,con_select_y,"going ");
            break;
        }
        case 1:
        {
            lcd_showint32(con_select_x+13,con_select_y,vl,4);
            lcd_showstr(con_select_x,con_select_y,"Vl:");
            break;
        }
        case 2:
        {
            lcd_showint32(con_select_x+13,con_select_y,vr,4);
            lcd_showstr(con_select_x,con_select_y,"Vr:");
            break;
        }
        case 3:
        {
            lcd_showint32(con_select_x+17,con_select_y,C1,2);
            lcd_showstr(con_select_x,con_select_y,"C1:");
            break;
        }
    }
    TFT_PENCOLOR=BLUE;
    TFT_BGCOLOR=BLACK;

}


void core1_main(void)
{
	disableInterrupts();
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());

    IfxCpu_emitEvent(&g_cpuSyncEvent);

    IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);

    enableInterrupts();
    //TrigInit();
    int time_temp=0;

    while (TRUE)
    {
        if(mt9v03x_finish_flag)
        {
#ifdef IPS200
            ips200_displayimage032(imagezip[0],94,60);
            ips200_showuint16(60,10,duty);
            ips200_showint8(20,10,gray);
            ips200_showint16(10,12,speed1);
            ips200_showint16(80,12,speed2);
#endif
#ifdef TFT18
            show_variable_adjust();
            toral_control();
            //lcd_displayimage032(imagezip[0],94,60);
            //Image_process();
            //draw_line();
            //lcd_displayimage032(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
            //Bc_Dis();
            //lcd_showint16(0,7,speed1);
            //lcd_showint32(79,8,distance,5);
            lcd_showint16(0,5,1500-(uint32)PID.DirOut);

            lcd_showint32(70,9,type_Y,2);
            //real_angle();
            //time_temp=systick_getval_us(STM1);
            //lcd_showint32(70,6,)
            //lcd_showint32(60,7,distance,4);

            lcd_showint32(70,8,IN_length/10,5);
            lcd_showint32(0,7,speed1,5);
            lcd_showint32(50,7,speed2,5);
            //lcd_showint16(60,8,icm_gyro_y);
            //lcd_showint16(60,9,icm_gyro_z);
            //lcd_showint16(0,7,duty);
            //lcd_showuint16(80,6,v);
            //lcd_showint32(0,6,time,9);
#endif
            mt9v03x_finish_flag=0;
        }
    }
}





#pragma section all restore
