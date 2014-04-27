/***********************************************************************/
/*                                                                     */
/*  FILE        :init.c		                                           */
/*  DATE        :Tue, Sep 27, 2011                                     */
/*  DESCRIPTION :Reset Program                                         */
/*  CPU TYPE    :SH7125                                                */
/*                                                                     */
/*  This file is generated by Renesas Project Generator (Ver.4.16).    */
/*                                                                     */
/***********************************************************************/
//初期化

#include <machine.h>
#include "iodefine.h"
#include "global.h"

void ad_init(void){
	STB.CR4.BIT._AD0 = 0;             // AD0 スタンバイ解除
	STB.CR4.BIT._AD1 = 0;             // AD1 スタンバイ解除
	AD0.ADCR.BIT.ADST = 0;            // AD変換停止
	AD1.ADCR.BIT.ADST = 0;            // AD変換停止
	AD0.ADCSR.WORD = 0x0013;
	AD1.ADCSR.WORD = 0x0013; 
}

void cmt_setup(){
	/* CMTの設定 */
	STB.CR4.BIT._CMT    = 0;          // CMT スタンバイ解除
	CMT0.CMCOR          = Pphi*1000*8;
	CMT0.CMCSR.BIT.CMIE = 1;          // 割込み許可	
	CMT0.CMCSR.BIT.CKS  = 0;          // 周辺動作クロック (25MHz)/ 8 
	CMT0.CMCSR.BIT.CMF  = 0;          // CMT0 コンペアフラグクリア
	INTC.IPRJ.BIT._CMT0 = 15;         // 割り込みレベル設定(CMT0)
	
  
	CMT1.CMCSR.BIT.CKS = 0;      // CMT1 CLOCK 24MHz/8
	CMT1.CMCOR = 2765;            // 2765 = 1ms (22.1184MHz)/8
	CMT.CMSTR.BIT.STR1 = 0;      // CMT0 COUNT STOP
  
	CMT.CMSTR.BIT.STR0 = 1; //CMT0カウントスタート	
}

void sci_setup(){

	//出力設定
	//sci0
	set_imask(5);
	
	//SCI1のボーレートはSCI0のボーレートは同じ、もしくはそれ以下
//	SCI0_INIT(76800);
	SCI1_INIT(115200);
	SCI2_INIT(115200);
}

void setup(void)
{
	int i;	
	PFC.PEIORL.BIT.B7=1;
	PE.DRL.BIT.B7=1;
	// クロック発振器(CPG) : PLL=44.2368MHz
	// Iφ=PLL/2, Bφ=PLL/4, Pφ=PLL/4←40Mhz, MPφ=PLL/4
	CPG.FRQCR.WORD = 0x16db;
	
	// PFCの設定
	cmt_setup();
	//ad_init();
	sci_setup();
	sci0_miss=0;
	sci0_wait=0;

	cmt0_count=0;
	cmt0_flag=0;
	PS2_command='z';
	PC_command_flag=0;
	PC_command_number=0;
	PC_move_command_number=0;
	PC_move_command_flag=0;
	for(i=0;i<10;i++){
		from_pc_sci[i]=0;
		pc_command[i]=0;
	}
}