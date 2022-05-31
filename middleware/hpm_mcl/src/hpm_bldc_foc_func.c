/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_common.h"
#include "stdio.h"
#include "hpm_bldc_define.h"
#include "hpm_bldc_foc_func.h"
#include "hpm_smc.h"

/*函数函数指针没有初始化的情况下报错*/
void bldc_nullcallback_func(void){
    while(1){
        ;
    }
}
/*
使用任意c编译器运行如下代码 可以生成 bldc_foc_sintable[]数组
#include <stdio.h>
#include <math.h>
#define NUM 501
#define PRECISION 0.18
#define PI 3.14159265
int main()
{
	short i;
	printf("staitc const float bldc_foc_sintable[%d] =\r",NUM);
	printf("{\r");
	for(i=0;i<NUM;i++)
	{
   		printf(" %f",sin(i*PRECISION*PI/180));
   		if(i != NUM-1)
   		{
   		   printf(",");
   		}
   	
   		if((i%10) == 0)
   		{
   		    printf("\r");
   		}
	}
	printf("};\r");
   
   return 0;
}
*/
#define PRECISION       (0.18)
const float bldc_foc_sintable[501] =
{
 0.000000,
 0.003142, 0.006283, 0.009425, 0.012566, 0.015707, 0.018848, 0.021989, 0.025130, 0.028271, 0.031411,
 0.034551, 0.037690, 0.040829, 0.043968, 0.047106, 0.050244, 0.053382, 0.056519, 0.059655, 0.062791,
 0.065926, 0.069060, 0.072194, 0.075327, 0.078459, 0.081591, 0.084721, 0.087851, 0.090980, 0.094108,
 0.097235, 0.100362, 0.103487, 0.106611, 0.109734, 0.112856, 0.115977, 0.119097, 0.122216, 0.125333,
 0.128449, 0.131564, 0.134678, 0.137790, 0.140901, 0.144011, 0.147119, 0.150226, 0.153331, 0.156434,
 0.159537, 0.162637, 0.165736, 0.168833, 0.171929, 0.175023, 0.178115, 0.181206, 0.184294, 0.187381,
 0.190466, 0.193549, 0.196631, 0.199710, 0.202787, 0.205863, 0.208936, 0.212007, 0.215076, 0.218143,
 0.221208, 0.224271, 0.227331, 0.230389, 0.233445, 0.236499, 0.239550, 0.242599, 0.245646, 0.248690,
 0.251732, 0.254771, 0.257807, 0.260842, 0.263873, 0.266902, 0.269928, 0.272952, 0.275973, 0.278991,
 0.282007, 0.285019, 0.288029, 0.291036, 0.294040, 0.297042, 0.300040, 0.303035, 0.306028, 0.309017,
 0.312003, 0.314987, 0.317967, 0.320944, 0.323917, 0.326888, 0.329855, 0.332820, 0.335780, 0.338738,
 0.341692, 0.344643, 0.347590, 0.350534, 0.353475, 0.356412, 0.359345, 0.362275, 0.365202, 0.368125,
 0.371044, 0.373959, 0.376871, 0.379779, 0.382683, 0.385584, 0.388481, 0.391374, 0.394263, 0.397148,
 0.400029, 0.402906, 0.405780, 0.408649, 0.411514, 0.414376, 0.417233, 0.420086, 0.422935, 0.425779,
 0.428620, 0.431456, 0.434288, 0.437116, 0.439939, 0.442758, 0.445573, 0.448383, 0.451189, 0.453990,
 0.456787, 0.459580, 0.462368, 0.465151, 0.467930, 0.470704, 0.473473, 0.476238, 0.478998, 0.481754,
 0.484504, 0.487250, 0.489991, 0.492727, 0.495459, 0.498185, 0.500907, 0.503623, 0.506335, 0.509041,
 0.511743, 0.514440, 0.517131, 0.519817, 0.522499, 0.525175, 0.527846, 0.530511, 0.533172, 0.535827,
 0.538477, 0.541121, 0.543760, 0.546394, 0.549023, 0.551646, 0.554263, 0.556876, 0.559482, 0.562083,
 0.564679, 0.567269, 0.569853, 0.572432, 0.575005, 0.577573, 0.580134, 0.582690, 0.585241, 0.587785,
 0.590324, 0.592857, 0.595384, 0.597905, 0.600420, 0.602930, 0.605433, 0.607930, 0.610422, 0.612907,
 0.615386, 0.617860, 0.620327, 0.622788, 0.625243, 0.627691, 0.630134, 0.632570, 0.635000, 0.637424,
 0.639841, 0.642253, 0.644657, 0.647056, 0.649448, 0.651834, 0.654213, 0.656586, 0.658952, 0.661312,
 0.663665, 0.666012, 0.668352, 0.670686, 0.673013, 0.675333, 0.677646, 0.679953, 0.682254, 0.684547,
 0.686834, 0.689114, 0.691387, 0.693653, 0.695913, 0.698165, 0.700411, 0.702650, 0.704882, 0.707107,
 0.709325, 0.711536, 0.713740, 0.715936, 0.718126, 0.720309, 0.722485, 0.724653, 0.726814, 0.728969,
 0.731116, 0.733255, 0.735388, 0.737513, 0.739631, 0.741742, 0.743845, 0.745941, 0.748030, 0.750111,
 0.752185, 0.754251, 0.756310, 0.758362, 0.760406, 0.762443, 0.764472, 0.766493, 0.768507, 0.770513,
 0.772512, 0.774503, 0.776487, 0.778462, 0.780430, 0.782391, 0.784343, 0.786288, 0.788226, 0.790155,
 0.792077, 0.793990, 0.795896, 0.797794, 0.799685, 0.801567, 0.803441, 0.805308, 0.807166, 0.809017,
 0.810860, 0.812694, 0.814521, 0.816339, 0.818150, 0.819952, 0.821746, 0.823533, 0.825311, 0.827081,
 0.828842, 0.830596, 0.832341, 0.834078, 0.835807, 0.837528, 0.839240, 0.840945, 0.842640, 0.844328,
 0.846007, 0.847678, 0.849340, 0.850994, 0.852640, 0.854277, 0.855906, 0.857527, 0.859139, 0.860742,
 0.862337, 0.863923, 0.865501, 0.867071, 0.868632, 0.870184, 0.871727, 0.873262, 0.874789, 0.876307,
 0.877816, 0.879316, 0.880808, 0.882291, 0.883766, 0.885231, 0.886688, 0.888136, 0.889576, 0.891007,
 0.892428, 0.893841, 0.895246, 0.896641, 0.898028, 0.899405, 0.900774, 0.902134, 0.903485, 0.904827,
 0.906160, 0.907484, 0.908800, 0.910106, 0.911403, 0.912692, 0.913971, 0.915241, 0.916502, 0.917755,
 0.918998, 0.920232, 0.921457, 0.922673, 0.923880, 0.925077, 0.926266, 0.927445, 0.928615, 0.929776,
 0.930928, 0.932071, 0.933205, 0.934329, 0.935444, 0.936550, 0.937646, 0.938734, 0.939812, 0.940881,
 0.941940, 0.942991, 0.944031, 0.945063, 0.946085, 0.947098, 0.948102, 0.949096, 0.950081, 0.951057,
 0.952023, 0.952979, 0.953927, 0.954865, 0.955793, 0.956712, 0.957622, 0.958522, 0.959412, 0.960294,
 0.961165, 0.962028, 0.962880, 0.963724, 0.964557, 0.965382, 0.966196, 0.967001, 0.967797, 0.968583,
 0.969360, 0.970127, 0.970884, 0.971632, 0.972370, 0.973099, 0.973817, 0.974527, 0.975227, 0.975917,
 0.976597, 0.977268, 0.977929, 0.978581, 0.979223, 0.979855, 0.980478, 0.981091, 0.981694, 0.982287,
 0.982871, 0.983445, 0.984010, 0.984564, 0.985109, 0.985645, 0.986170, 0.986686, 0.987192, 0.987688,
 0.988175, 0.988652, 0.989119, 0.989576, 0.990024, 0.990461, 0.990889, 0.991308, 0.991716, 0.992115,
 0.992504, 0.992883, 0.993252, 0.993611, 0.993961, 0.994301, 0.994631, 0.994951, 0.995261, 0.995562,
 0.995853, 0.996134, 0.996405, 0.996666, 0.996917, 0.997159, 0.997391, 0.997613, 0.997825, 0.998027,
 0.998219, 0.998402, 0.998574, 0.998737, 0.998890, 0.999033, 0.999166, 0.999289, 0.999403, 0.999507,
 0.999600, 0.999684, 0.999758, 0.999822, 0.999877, 0.999921, 0.999956, 0.999980, 0.999995, 1.000000
};
/*
*  
* 描述：速度计算器，通过角度差值计算速度
* 返回值：速度
*/
void bldc_foc_al_speed(BLDC_CONTRL_SPD_PARA  *par)
{
    HPM_MOTOR_MATH_TYPE deta;
    deta = par->speedtheta - par->speedlasttheta;
    if(deta > HPM_MOTOR_MATH_FL_MDF(180)){/*-speed*/
        deta = -par->speedlasttheta - (HPM_MOTOR_MATH_FL_MDF(360) - par->speedtheta);
    }
    else if(deta < HPM_MOTOR_MATH_FL_MDF(-180)){/*+speed*/
        deta = HPM_MOTOR_MATH_FL_MDF(360) + par->speedtheta - par->speedlasttheta;
    }
    par->speedthetaLastN += deta;
    par->speedlasttheta = par->speedtheta;
    par->num++;
    if(par->I_speedacq == par->num){
        par->num = 0;
        par->O_speedout = HPM_MOTOR_MATH_DIV(par->speedthetaLastN,
            HPM_MOTOR_MATH_MUL(HPM_MOTOR_MATH_MUL( par->I_speedLooptime_s,HPM_MOTOR_MATH_FL_MDF(par->I_motorpar->I_Poles_n)),HPM_MOTOR_MATH_FL_MDF(360)));
        par->O_speedout_filter = par->O_speedout_filter + HPM_MOTOR_MATH_MUL(par->I_speedfilter,
            (par->O_speedout - par->O_speedout_filter));
        par->speedthetaLastN = 0;
    }
}

/*
** ===================================================================
**  bldc_foc_sin(float angle , float angle_precision)
**  功能描述：计算电机角度的正弦值
**  输入参数：angle 电机转子角度值
**           angle_precision 角度精度 输入角度是每0.18度进行跳变
**  返回值：角度的正弦值
** ===================================================================
*/
static HPM_MOTOR_MATH_TYPE bldc_foc_sin(HPM_MOTOR_MATH_TYPE angle , HPM_MOTOR_MATH_TYPE angle_precision)
{
     HPM_MOTOR_MATH_TYPE result=0;
     int16_t  transfer=0;
     if((angle >= 0) && (angle <= HPM_MOTOR_MATH_FL_MDF(90)))
     {
        transfer = HPM_MOTOR_MATH_MDF_FL(HPM_MOTOR_MATH_DIV(angle , angle_precision));
        result = HPM_MOTOR_MATH_FL_MDF(bldc_foc_sintable[transfer]);
     }
     else if((angle > HPM_MOTOR_MATH_FL_MDF(90)) && (angle <= HPM_MOTOR_MATH_FL_MDF(180)))
     {
        transfer = HPM_MOTOR_MATH_MDF_FL(HPM_MOTOR_MATH_DIV((HPM_MOTOR_MATH_FL_MDF(180) - angle),angle_precision));
        result = HPM_MOTOR_MATH_FL_MDF(bldc_foc_sintable[transfer]);
     }
     else if((angle > HPM_MOTOR_MATH_FL_MDF(180)) && (angle <= HPM_MOTOR_MATH_FL_MDF(270)))
     {
        transfer = HPM_MOTOR_MATH_MDF_FL(HPM_MOTOR_MATH_DIV((angle - HPM_MOTOR_MATH_FL_MDF(180)),angle_precision));
        result = HPM_MOTOR_MATH_FL_MDF(-bldc_foc_sintable[transfer]);
     }
     else if((angle > HPM_MOTOR_MATH_FL_MDF(270)) && (angle <= HPM_MOTOR_MATH_FL_MDF(360)))
     {
        transfer = HPM_MOTOR_MATH_MDF_FL(HPM_MOTOR_MATH_DIV((HPM_MOTOR_MATH_FL_MDF(360)-angle),angle_precision));
        result = HPM_MOTOR_MATH_FL_MDF(-bldc_foc_sintable[transfer]);
     }
     return(result);
}

/*
** ===================================================================
**  函数名称：bldc_foc_cos(float angle , float angle_precision)
**  功能描述：计算电机角度的余弦值
**  输入参数：angle 电机转子角度值
**           angle_precision 角度精度 输入角度是每0.18度进行跳变
**  返回值：角度的余弦值
** ===================================================================
*/
static HPM_MOTOR_MATH_TYPE bldc_foc_cos(HPM_MOTOR_MATH_TYPE angle , HPM_MOTOR_MATH_TYPE angle_precision)
{
     HPM_MOTOR_MATH_TYPE result=0;
     int16_t  transfer=0;
     if((angle >= 0) && (angle <= HPM_MOTOR_MATH_FL_MDF(90)))
     {
        transfer = HPM_MOTOR_MATH_MDF_FL(HPM_MOTOR_MATH_DIV((HPM_MOTOR_MATH_FL_MDF(90) - angle),angle_precision));
        result = HPM_MOTOR_MATH_FL_MDF(bldc_foc_sintable[transfer]);
     }
     else if((angle > HPM_MOTOR_MATH_FL_MDF(90)) && (angle <= HPM_MOTOR_MATH_FL_MDF(180)))
     {
        transfer = HPM_MOTOR_MATH_MDF_FL(HPM_MOTOR_MATH_DIV((angle - HPM_MOTOR_MATH_FL_MDF(90)),angle_precision));
        result = HPM_MOTOR_MATH_FL_MDF(-bldc_foc_sintable[transfer]);
     }
     else if((angle > HPM_MOTOR_MATH_FL_MDF(180)) && (angle <= HPM_MOTOR_MATH_FL_MDF(270)))
     {
        transfer = HPM_MOTOR_MATH_MDF_FL(HPM_MOTOR_MATH_DIV((HPM_MOTOR_MATH_FL_MDF(270) - angle),angle_precision));
        result = HPM_MOTOR_MATH_FL_MDF(-bldc_foc_sintable[transfer]);
     }
     else if((angle > HPM_MOTOR_MATH_FL_MDF(270)) && (angle <= HPM_MOTOR_MATH_FL_MDF(360)))
     {
        transfer =HPM_MOTOR_MATH_MDF_FL(HPM_MOTOR_MATH_DIV((angle - HPM_MOTOR_MATH_FL_MDF(270)),angle_precision));
        result = HPM_MOTOR_MATH_FL_MDF(bldc_foc_sintable[transfer]);
     }

     return(result);
}
/** ===================================================================
**  函数名称：bldc_foc_inv_park
**  功能描述：反park变换
**  输入参数：
**  返回值：
** ===================================================================*/
void bldc_foc_inv_park(HPM_MOTOR_MATH_TYPE ud,HPM_MOTOR_MATH_TYPE uq,HPM_MOTOR_MATH_TYPE *ualpha, HPM_MOTOR_MATH_TYPE *ubeta, HPM_MOTOR_MATH_TYPE angle)
{
    HPM_MOTOR_MATH_TYPE sin_angle,cos_angle;
    sin_angle = bldc_foc_sin(angle,HPM_MOTOR_MATH_FL_MDF(PRECISION));
    cos_angle = bldc_foc_cos(angle,HPM_MOTOR_MATH_FL_MDF(PRECISION));
    *ualpha = HPM_MOTOR_MATH_MUL(cos_angle , ud) + HPM_MOTOR_MATH_MUL(-sin_angle , uq); // PARK逆变换
    *ubeta = HPM_MOTOR_MATH_MUL(sin_angle , ud) + HPM_MOTOR_MATH_MUL(cos_angle , uq);
}


/** ===================================================================
**  函数名称：bldc_foc_svpwm
**  功能描述：脉宽调制函数
**  输入参数：
**  返回值：
** ===================================================================*/

void bldc_foc_svpwm(BLDC_CONTROL_PWM_PARA *par)
{
    HPM_MOTOR_MATH_TYPE ualpha_60, ubeta_30;
    uint32_t pwm_reload;
    int32_t uref1, uref2, uref3;
    int32_t tx, ty, t0;  
    int32_t tuon, tvon, twon = 0;
    int8_t sector = 0;

    /*求出参考电压*/
    uref1 = HPM_MOTOR_MATH_MDF_FL(par->target_beta);
    ualpha_60 = HPM_MOTOR_MATH_MUL(HPM_MOTOR_MATH_FL_MDF(0.866025 ) , par->target_alpha);
    ubeta_30 = HPM_MOTOR_MATH_MUL(par->target_beta , HPM_MOTOR_MATH_FL_MDF(0.5));
    uref2 = HPM_MOTOR_MATH_MDF_FL(ualpha_60 - ubeta_30);   
    uref3 = HPM_MOTOR_MATH_MDF_FL(-ualpha_60 - ubeta_30);
    pwm_reload = par->pwmout.I_pwm_reload;


    if (uref1 >= 0)
        sector = 1;
    if (uref2 >= 0)
        sector = sector + 2;
    if (uref3 >= 0)
        sector = sector + 4;

    switch (sector)
    {
    case 1: // 第二扇区  000 010 110 111 110 010 000
        tx = -uref2;   
        ty = -uref3;
        t0 = pwm_reload - tx - ty;
        if (t0 < 0)  // tx + ty > T
        {
            tx = ((float)tx / (tx + ty)) * pwm_reload;
            ty = pwm_reload - tx;
            t0 = 0;
        }
        twon = ((int) (t0)) >> 1;
        tuon = (int) (ty + twon);
        tvon = (int) (tx + tuon);
        break;
    case 2: // 第六扇区  000   100 101 111 101 100 000
        tx = -uref3;
        ty = -uref1;
        t0 = pwm_reload - tx - ty;
        if (t0 < 0)
        {
            tx = ((float)tx / (tx + ty)) * pwm_reload;
            ty = pwm_reload - tx;
            t0 = 0;
        }
        tvon = ((int) (t0)) >> 1;
        twon = (int) (ty + tvon);
        tuon = (int) (tx + twon);
        break;
    case 3: // 第一扇区  000 100 110 111 110 100 000
        tx = uref2;
        ty = uref1;
        t0 = pwm_reload - tx - ty;
        if (t0 < 0)
        {
            tx = ((float)tx / (tx + ty)) * pwm_reload;
            ty = pwm_reload - tx;
            t0 = 0;
        }
        twon = ((int) (t0)) >> 1;
        tvon = (int) (ty + twon);
        tuon = (int) (tx + tvon);
        break;
    case 4: // 第四扇区  000 001 011 111 011 001 000
        tx = -uref1;
        ty = -uref2;
        t0 = pwm_reload - tx - ty;
        if (t0 < 0)
        {
            tx = ((float)tx / (tx + ty)) * pwm_reload;
            ty = pwm_reload - tx;
            t0 = 0;
        }
        tuon = ((int) (t0)) >> 1;
        tvon = (int) (ty + tuon);
        twon = (int) (tx + tvon);
        break;
    case 5: // 第三扇区 000 010 011 111 011 010 000
        tx = uref1;
        ty = uref3;
        t0 = pwm_reload - tx - ty;
        if (t0 < 0)
        {
            tx = ((float)tx / (tx + ty)) * pwm_reload;
            ty = pwm_reload - tx;
            t0 = 0;
        }
        tuon = ((int) (t0)) >> 1;
        twon = (int) (ty + tuon);
        tvon = (int) (tx + twon);
        break;
    case 6: // 第五扇区 000 001 101 111 101 001 000
        tx = uref3;
        ty = uref2;
        t0 = pwm_reload - tx - ty;
        if (t0 < 0)
        {
            tx = ((float)tx / (tx + ty)) * pwm_reload;
            ty = pwm_reload - tx;
            t0 = 0;
        }
        tvon = ((int) (t0)) >> 1;
        tuon = (int) (ty + tvon);
        twon = (int) (tx + tuon);
        break;

    default:
        tuon = (int)(pwm_reload/2);
        tvon = (int)(pwm_reload/2);
        twon = (int)(pwm_reload/2);
    }
    par->sector = sector;


    if (tuon > par->I_pwm_reload_max)
        tuon = par->I_pwm_reload_max;
    if (tvon > par->I_pwm_reload_max)
        tvon = par->I_pwm_reload_max;
    if (twon > par->I_pwm_reload_max)
        twon = par->I_pwm_reload_max;

    if (tuon < 0)
        tuon = 0;
    if (tuon < 0)
        tvon = 0;
    if (tuon < 0)
        twon = 0;

    par->pwmout.pwm_u = tuon;
    par->pwmout.pwm_v = tvon;
    par->pwmout.pwm_w = twon;
}

/*
** ===================================================================
**  函数名称：bldc_foc_clarke
**  功能描述：Clark 坐标变换
**  输入参数：
**         
**  返回值：无
** ===================================================================
*/
void bldc_foc_clarke(HPM_MOTOR_MATH_TYPE currentU,HPM_MOTOR_MATH_TYPE currentV,HPM_MOTOR_MATH_TYPE currentW,
             HPM_MOTOR_MATH_TYPE * currentAlpha, HPM_MOTOR_MATH_TYPE * currentBeta)
{
    HPM_MOTOR_MATH_TYPE curbeta;
    *currentAlpha = currentU;
    curbeta = HPM_MOTOR_MATH_MUL(HPM_MOTOR_MATH_FL_MDF(0.5773502) , currentU)  + HPM_MOTOR_MATH_MUL(HPM_MOTOR_MATH_FL_MDF(1.1547004), currentV);
    *currentBeta = curbeta;
   
}
/*
** ===================================================================
**  函数名称：bldc_foc_park
**  功能描述：Clark 坐标变换
**  输入参数：
**  返回值：无
** ===================================================================
*/
void bldc_foc_park( HPM_MOTOR_MATH_TYPE currentAlpha, HPM_MOTOR_MATH_TYPE currentBeta, HPM_MOTOR_MATH_TYPE angle ,
                   HPM_MOTOR_MATH_TYPE * currentd, HPM_MOTOR_MATH_TYPE * currentq)
{
    HPM_MOTOR_MATH_TYPE sin_angle,cos_angle;
					
	sin_angle = bldc_foc_sin(angle, PRECISION);
	cos_angle = bldc_foc_cos(angle, PRECISION);
    *currentd = HPM_MOTOR_MATH_MUL(cos_angle , currentAlpha) + HPM_MOTOR_MATH_MUL(sin_angle , currentBeta);
    *currentq = HPM_MOTOR_MATH_MUL(-sin_angle , currentAlpha) + HPM_MOTOR_MATH_MUL(cos_angle , currentBeta);
}
/*
** ===================================================================
**  函数名称：bldc_foc_current_cal
**  功能描述：重构三相电流值
**  输入参数：
**          adc_u\adc_v\adc_w  采集到的三相电流ad值
**          mid_u\mid_v\mid_w  采集到的三相电流ad中点值
**          
**  返回值：current_u\current_v\current_w带方向的三相电流   流出是-   流入是+
** ===================================================================
*/
void bldc_foc_current_cal(BLDC_CONTROL_CURRENT_PARA *par)
{
    par->cal_u = HPM_MOTOR_MATH_FL_MDF(par->adc_u_middle - par->adc_u);
    par->cal_v = HPM_MOTOR_MATH_FL_MDF(par->adc_v_middle - par->adc_v);
    par->cal_w=  HPM_MOTOR_MATH_FL_MDF(-(par->cal_u + par->cal_v));

}
/*
** ===================================================================
**  函数名称：bldc_foc_curpi_contrl
**  功能描述：d轴电流pi调节
**  输入参数：
**          
**          
**  返回值：output 
** ===================================================================
*/
void bldc_foc_pi_contrl(BLDC_CONTRL_PID_PARA *par)
{
   HPM_MOTOR_MATH_TYPE result = 0;//PI运算结果

   /*目标电流和采样电流进行PI调节，计算给定电压*/
   HPM_MOTOR_MATH_TYPE curerr = 0;       
   HPM_MOTOR_MATH_TYPE portion_asp = 0;  
   HPM_MOTOR_MATH_TYPE portion_asi = 0;  

   curerr = par->target - par->cur;
   portion_asp = HPM_MOTOR_MATH_MUL(curerr , (par->I_kp));
   portion_asi = HPM_MOTOR_MATH_MUL(curerr , (par->I_ki))+ par->mem;
   result = portion_asi + portion_asp;

   if(result < (-par->I_max)){
       result = -par->I_max;
   }
   else if(result > par->I_max){
       result = par->I_max;
   }
   else{
       par->mem = portion_asi;
   }
   //目标电流和采样电流进行PI调节，计算给定电压
   par->outval = result;
}
/*
** ===================================================================
**  函数名称：ldc_foc_ctrl_dq_to_pwm
**  功能描述：输入dq轴电流、转子角度、和采样电流，输出pwm
**  输入参数：
**          
**          
**  返回值： 
** ===================================================================
*/
void bldc_foc_ctrl_dq_to_pwm(BLDC_CONTROL_FOC_PARA *par)
{
    par->pwmpar.pwmout.func_set_pwm(&par->pwmpar.pwmout);//750
    par->samplCurpar.func_sampl(&par->samplCurpar);
    bldc_foc_clarke(par->samplCurpar.cal_u,par->samplCurpar.cal_v,par->samplCurpar.cal_w,
                    &par->i_alpha,&par->i_beta);
    
    if(par->pos_estimator_par.func != NULL){
        par->pos_estimator_par.func(par->pos_estimator_par.par);
    }
    bldc_foc_park(par->i_alpha,par->i_beta,par->electric_angle,
                &par->CurrentDPiPar.cur,&par->CurrentQPiPar.cur);
    par->SpeedCalPar.speedtheta = par->electric_angle;
    par->SpeedCalPar.func_getspd(&par->SpeedCalPar);

    par->CurrentDPiPar.func_pid(&par->CurrentDPiPar);
    par->CurrentQPiPar.func_pid(&par->CurrentQPiPar);

    bldc_foc_inv_park(par->CurrentDPiPar.outval,par->CurrentQPiPar.outval,
        &par->u_alpha,&par->u_beta,par->electric_angle);
    par->pwmpar.target_alpha = par->u_alpha;
    par->pwmpar.target_beta = par->u_beta;
    par->pwmpar.func_spwm(&par->pwmpar);
}
