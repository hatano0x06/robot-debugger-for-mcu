/********************************************/
/*				SCI1関数					*/
/*					for SH7125 @ HEW		*/
/*					Wrote by conpe_			*/
/*							2010/12/17		*/
/********************************************/
//SCI2はbluetoothとの相互通信
//SCI2用の送受信関数です。

// バッファを使って順次割り込みにより送信
// バッファのサイズは.hの方で指定
// intprg.cのINT_SCI1_TXI1からintSCI1_tx_buff()を呼び出してね。
// resetprg.cの#define SR_Initは0x00000000にする

// バッファがあふれそうになったら、送信が追いつくまで待つので注意。

//#include <stdarg.h>
#include "iodefine.h"
#include "sci.h"
#include "global.h"


char sci2_tx_buffer[SCI2_TxBuffSize];	//バッファ
unsigned short sci2_tx_writep;			//書き込みポインタ
unsigned short sci2_tx_readp;			//読み込みポインタ


/*********************
SCI1初期化
引数：ボーレート 4800～115200
**********************/
void SCI2_setup(int br){
	unsigned short i = 8 * 8;	

	STB.CR3.BIT._SCI2=0;		// SCI1 動作開始
	SCI2.SCSCR.BYTE = 0x00;
	SCI2.SCSCR.BIT.CKE = 0x00;	// クロックソース
	SCI2.SCSMR.BYTE = 0x00;		// 調歩同期式, 8bit, パリティ無し, ストップビット1, マルチプロセッサ無し
	SCI2.SCSMR.BIT.CKS = 0;		// クロック
	SCI2.SCSDCR.BIT.DIR = 0x00;	// LSBから送受信
	
	SCI2.SCBRR = Pphi*2*1000000/64/br-(115200>br);	//ボーレートの設定
//	SCI2.SCBRR = 4;	//ボーレートの設定
	while(i--);

	PFC.PAIORL.BIT.B8=0;
	PFC.PAIORL.BIT.B9=1;

	PFC.PACRL3.BIT.PA8MD = 6;	// PA3 端子機能設定 RXD1入力
	PFC.PACRL3.BIT.PA9MD = 6;	// PA4 端子機能設定 TXD1出力
	
	SCI2.SCSCR.BIT.RE =1;
	SCI2.SCSCR.BIT.TE =1;
	SCI2.SCSCR.BIT.RIE = 1; 	
	INTC.IPRL.BIT._SCI2 = 15;	// 割り込みレベル	
//	SCI2.SCSSR.BYTE;	

	/*
	volatile int i;

	STB.CR3.BIT._SCI2 = 0;          // SCI2をスタンバイから復帰

	SCI2.SCSCR.BYTE = 0;             // 全ての通常動作・割り込みをカット  
	SCI2.SCSMR.BYTE = 0;             // 調歩同期式 8bit パリティなし


	SCI2.SCBRR = Pphi*2*1000000/64/br-(115200>br);                  // シリアルボーレート代入
	for (i = 0; i < 3000; i++) ;
	SCI2.SCSCR.BYTE = 0xf0;          // 送受信・送受信割り込み許可
	SCI2.SCSSR.BYTE;                 // 一度リード
	SCI2.SCSSR.BYTE = 0;             // フラグ書き込み

	INTC.IPRL.BIT._SCI2 = 15;       // 割り込み優先順位設定
*/
}


void SCI2_intpre(void){
	//シリアルの受信割り込みの関数です。割り込みで入るようにintpreg.cの中から飛ばします
	PS2_command=SCI2.SCRDR;
    SCI2.SCSSR.BIT.RDRF=0;
}

void SCI2_err(void){//同じくエラー割り込みです。
       SCI2.SCSSR.BYTE &= ~0xb8;
	   SCI2.SCSPTR.BIT.EIO=0;
}

void SCI2_tx(char data){
	while(SCI2.SCSSR.BIT.TDRE == 0);	// SCTDRが空くまで待つ
	SCI2.SCTDR = data;			// SCTDRに送信データを書き込む
	SCI2.SCSSR.BIT.TDRE = 0;		// フラグクリア
}

char SCI2_rx(void){
	char data;
	
	if(SCI2.SCSSR.BIT.RDRF){
		SCI2.SCSSR.BIT.RDRF = 0;	// RDRFクリア
		data = SCI2.SCRDR;
		PE.DRL.BIT.B7=0;	
		
	}else{
		PE.DRL.BIT.B7=1;	
		data = 'b';
	}
	
	return data;
}


