/***********************************************************************/
/*                                                                     */
/*  FILE        :action.cpp	                                           */
/*  DATE        :Tue, Sep 27, 2011                                     */
/*  DESCRIPTION :Reset Program                                         */
/*  CPU TYPE    :SH7125                                                */
/*                                                                     */
/*  This file is generated by Renesas Project Generator (Ver.4.16).    */
/*                                                                     */
/***********************************************************************/
//ロボットの動作
#include "global.h"
#include "servo_class.h"
//#include "basic_servo_class.h"

void walk(servo arm1,servo arm2)
{
	delay_ms_cmt(1000);

	arm1.target_position(2000);
	arm2.target_position(2500);
		
	delay_ms_cmt(1000);

	arm1.target_position(10);
	arm2.target_position(40);

	delay_ms_cmt(0);
}


void delay_ms(int ms=1)
{
	int i=0;
	for(i=0;i<ms*25000;i++){}
}

void delay_ms_cmt(long ms=1000)
{
	while(cmt0_flag){}
	cmt0_count=ms;
	if(ms!=0){
		cmt0_flag=1;
	}
}

