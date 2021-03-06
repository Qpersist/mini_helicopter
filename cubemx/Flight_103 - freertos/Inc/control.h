#ifndef _CONTROL_H_
#define _CONTROL_H_
#include "stm32f1xx_hal.h"

struct _pid{
          float Exp;
        float error;

        float differ;
        float PreErr;
  
        float kp;
			  float ki;
	      float kd;
	      float increment;
	      float increment_max;
	      float kp_out;
			  float ki_out;
	      float kd_out;
	      float pid_out;
          };

struct _tache{
    struct _pid shell;
    struct _pid core;	
		struct _pid temp;
          };
	

struct _ctrl{
		      uint8_t  ctrlRate;
      struct _tache pitch;    
	    struct _tache roll;  
	    struct _tache yaw;
			struct _tache height;
            };

extern struct _ctrl ctrl;						
						


void PID_Param_init(void);
void CONTROL(float rol, float pit, float yaw);

						
						
#endif
