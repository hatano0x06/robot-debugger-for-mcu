/********************************************/
/*				SCI0関数					*/
/*					for SH7125 @ HEW		*/
/*					Wrote by conpe_			*/
/*							2010/12/17		*/
/********************************************/
//SCI0はサーボ関連
//SCI0用の送受信関数です。

// バッファを使って順次割り込みにより送信
// バッファのサイズは.hの方で指定
// intprg.cのINT_SCI0_TXI0からintSCI0_tx_buff()を呼び出してね。
// resetprg.cの#define SR_Initは0x00000000にする

// バッファがあふれそうになったら、送信が追いつくまで待つので注意。

//#include <stdarg.h>
#include "iodefine.h"
#include "sci.h"
#include "global.h"


char sci0_tx_buffer[SCI0_TxBuffSize];	//バッファ
unsigned short sci0_tx_writep;			//書き込みポインタ
unsigned short sci0_tx_readp;			//読み込みポインタ


/*********************
SCI0初期化
引数：ボーレート 4800～115200
**********************/
void SCI0_setup(int br){
	unsigned short i = 3 * 8;	

	STB.CR3.BIT._SCI0=0;		// SCI1 動作開始
	SCI0.SCSCR.BYTE = 0x00;
	SCI0.SCSCR.BIT.CKE = 0x00;	// クロックソース
	SCI0.SCSMR.BYTE = 0x00;		// 調歩同期式, 8bit, パリティ無し, ストップビット1, マルチプロセッサ有り
	SCI0.SCSMR.BIT.MP=1;
	SCI0.SCSMR.BIT.CKS = 0;		// クロック
	SCI0.SCSDCR.BIT.DIR = 0x00;	// LSBから送受信
	
	SCI0.SCBRR = Pphi*2*1000000/64/br-1;	//ボーレートの設定
	
	while(i--);

	PFC.PAIORL.BIT.B0=0;
	PFC.PAIORL.BIT.B1=1;

	PFC.PACRL1.BIT.PA0MD = 1;	// PA3 端子機能設定 RXD1入力
	PFC.PACRL1.BIT.PA1MD = 1;	// PA4 端子機能設定 TXD1出力
	
	SCI0.SCSCR.BIT.RE =1;
	SCI0.SCSCR.BIT.TE =1;
	SCI0.SCSCR.BIT.RIE = 1; 	
	//割り込み許可
	INTC.IPRL.BIT._SCI0 = 15;	// 割り込みレベル	
	
//	SCI0.SCSSR.BYTE;                    //ダミーリード
//	SCI0.SCSSR.BYTE = 0x80;             //エラーフラグクリア

}

void SCI0_tx( char data ) 
{
	SCI0.SCTDR = data; 
	SCI0.SCSSR.BIT.TDRE = 0; 
	while( !SCI0.SCSSR.BIT.TDRE ); 
	SCI0.SCSSR.BIT.MPBT = 0; 	
	while( !SCI0.SCSSR.BIT.TEND ); 
	
	return; 
} 



void SCI0_tx_parity(char data){
	
	SCI0.SCTDR = data;				//SCTDR: シリアル送信するデータを格納する8ビットのレジスタ
	SCI0.SCSSR.BIT.MPBT = 1; 		//id用にパリティビットを1にする
	SCI0.SCSSR.BIT.TDRE = 0; 		// フラグクリア 
	while( !SCI0.SCSSR.BIT.TDRE ); 	// SCTDRが空くまで待つ
	while( !SCI0.SCSSR.BIT.TEND ); 

	return; 	
	
}

void SCI0_intpre(void){
	//シリアルの受信割り込みの関数です。割り込みで入るようにintpreg.cの中から飛ばします


	if(SCI0.SCSSR.BIT.MPB==1){
		//これはマルチプロセッサビットの読み取りをしています。もしIDが帰ってくるならばここが１になります。
		sci0_data_get_i=0;
	}
	
    Get_Byte[sci0_data_get_i]=SCI0.SCRDR;//今よみとった値を一時的に格納します。

    SCI0.SCSSR.BIT.RDRF=0;
    if((sci0_data_get_i==1)&&(Get_Byte[0]==sci0_id)&&(Get_Byte[1]==6)){
		//これは確かACKが帰ったときの処理です。
		sci0_miss=1;
		sci0_wait=1;
//		Get_Byte[0]=0;
//		Get_Byte[1]=0;
	}else if((sci0_data_get_i==1)&&((Get_Byte[0]!=sci0_id)||(Get_Byte[1]!=6))){
		//これは確かNACKが帰ったときの処理です。
		sci0_miss=0;
		sci0_wait=1;
//		Get_Byte[0]=0;
//		Get_Byte[1]=0;
	}else{
		sci0_data_get_i++;
	}
}

void SCI0_err(void){//同じくエラー割り込みです。
       SCI0.SCSSR.BYTE &= ~0xb8;
}
