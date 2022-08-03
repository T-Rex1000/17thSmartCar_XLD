#include "headfile.h"
#include "stdio.h"
//#include "car_control.h"
PIDTypedef PID;
extern float DirCenter;

/*舵机PID参数初始化*/
void ServoPIDParameterInit(PIDTypedef *PidParameter)
{ 
        PidParameter->Kp =  5;
        //PidParameter->Kp2 =  3;
        PidParameter->Kd =  7;
        PidParameter->CurrentError =  0;
        PidParameter->LastError =  0;
        PidParameter->Dir =  750;
        PidParameter->DirOut =  750;
        PidParameter->CenterValue =  750;
}

/*pid控制器控制舵机输出*/
void DirControl(void) 
{ 
         PID.CurrentError =  cenline[C1]- 46;
         PID.CenterValue = 750;
         /*
         PID.Dir = PID.CenterValue+PID.Kp1*PID.CurrentError/10
                 +PID.Kd*(PID.CurrentError-PID.LastError);
         */
         PID.Dir = (PID.CenterValue 
                 + ( PID.Kp *  PID.CurrentError *  PID.CurrentError / 100 )*  PID.CurrentError
                 + ( PID.Kd * ( PID.CurrentError -  PID.LastError))
               );

         if(PID.Dir < 0)  PID.Dir = 0;

         PID.DirOut =  PID.Dir;

         if(PID.DirOut >  PID.CenterValue + 70)  PID.DirOut = (PID.CenterValue + 70);
         if(PID.DirOut <  PID.CenterValue - 70)  PID.DirOut = (PID.CenterValue - 70);

         PID.LastError =  PID.CurrentError; 
         pwm_duty(ATOM1_CH1_P33_9, 1500-(uint32)PID.DirOut);
}


