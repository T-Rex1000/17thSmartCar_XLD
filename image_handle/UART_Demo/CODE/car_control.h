#ifndef __CAR_CONCTROL_H__
#define __CAR_CONCTROL_H__

//#include "include.h"

typedef struct{ 
    float Kp;
    //float Kp2;
    float Kd;
    float CurrentError;
    float LastError;
    float Dir;
    float DirOut;
    float CenterValue;
}PIDTypedef;

extern PIDTypedef PID;

extern void ServoPIDParameterInit(PIDTypedef *PidParameter);
extern void DirControl(void);

#endif
