/********************************************/
/*				SCI1関数					*/
/*					for SH7125 @ HEW		*/
/*					Wrote by conpe_			*/
/*							2010/12/17		*/
/********************************************/
//SCI1はパソコンとの相互通信
//SCI1用の送受信関数です。

// バッファを使って順次割り込みにより送信
// バッファのサイズは.hの方で指定
// intprg.cのINT_SCI1_TXI1からintSCI1_tx_buff()を呼び出してね。
// resetprg.cの#define SR_Initは0x00000000にする

// バッファがあふれそうになったら、送信が追いつくまで待つので注意。

//#include <stdarg.h>
#include "iodefine.h"
#include "sci.h"
#include "global.h"


char sci1_tx_buffer[SCI1_TxBuffSize];	//バッファ
unsigned short sci1_tx_writep;			//書き込みポインタ
unsigned short sci1_tx_readp;			//読み込みポインタ


/*********************
SCI1初期化
引数：ボーレート 4800～115200
**********************/
void SCI1_setup(int br){
	unsigned short i = 3 * 8;	

	STB.CR3.BIT._SCI1=0;		// SCI1 動作開始
	SCI1.SCSCR.BYTE = 0x00;
	SCI1.SCSCR.BIT.CKE = 0x00;	// クロックソース
	SCI1.SCSMR.BYTE = 0x00;		// 調歩同期式, 8bit, パリティ無し, ストップビット1, マルチプロセッサ無し
	SCI1.SCSMR.BIT.CKS = 0;		// クロック
	SCI1.SCSDCR.BIT.DIR = 0x00;	// LSBから送受信
	
	SCI1.SCBRR = Pphi*2*1000000/64/br-(115200>br);	//ボーレートの設定

	while(i--);


	PFC.PAIORL.BIT.B3=0;
	PFC.PAIORL.BIT.B4=1;

	PFC.PACRL1.BIT.PA3MD = 1;	// PA3 端子機能設定 RXD1入力
	PFC.PACRL2.BIT.PA4MD = 1;	// PA4 端子機能設定 TXD1出力
	
	SCI1.SCSCR.BIT.RE =1;
	SCI1.SCSCR.BIT.TE =1;
	SCI1.SCSCR.BIT.RIE = 1; 	
	INTC.IPRL.BIT._SCI1 = 14;	// 割り込みレベル
}



void SCI1_tx(char data){
	while(SCI1.SCSSR.BIT.TDRE == 0);	// SCTDRが空くまで待つ
	SCI1.SCTDR = data;			// SCTDRに送信データを書き込む
	SCI1.SCSSR.BIT.TDRE = 0;		// フラグクリア
}


/*********************
SCI1 文字送信
引数：なし
返値：なし
TDREの割り込みによって動作
**********************/
void intSCI1_tx_buff(void){
	char data;
	
	SCI1.SCSSR.BIT.TDRE = 0;
	data = SCI1_read_buff();
	if(data==0x15){	//なんかおかしいよ
		data = '-';
	}
	SCI1.SCTDR = data;

	while(SCI1.SCSSR.BIT.TEND!=1);	

	if(sci1_tx_writep == sci1_tx_readp){	//バッファに追いついた
			SCI1.SCSCR.BIT.TIE = 0;			//割り込みoff
	}
}

/*********************
SCI1 bufferを読む
引数：なし
返値：文字 バッファがないときは0x15
**********************/
char SCI1_read_buff(void){
	char data;
	
	if(sci1_tx_writep != sci1_tx_readp){	//バッファがある
		data = sci1_tx_buffer[sci1_tx_readp];
		sci1_tx_readp = inc_buff_p1(sci1_tx_readp);
	}else{	//もう全部読み切ってるよ
		SCI1.SCSSR.BIT.TDRE = 0;			//送信データエンプティ0
		SCI1.SCSCR.BIT.TIE = 0;				//割り込みoff
		return 0x15;	//NAK
	}
	
	return data;
}

/*********************
SCI1 bufferに書く
引数：文字
返値：0x00(正常), 0x15(バッファいっぱい)
**********************/
char SCI1_write_buff(char moji){
	char ack;
	if(inc_buff_p1(sci1_tx_writep) != sci1_tx_readp){
		sci1_tx_buffer[sci1_tx_writep] = moji;
		sci1_tx_writep = inc_buff_p1(sci1_tx_writep);
		
		ack = 0;
	}else{		//バッファいっぱいだよ
		ack = 0x15;	//NAK
	}
	if(SCI1.SCSCR.BIT.TIE==0){	//割り込みオフだった
		SCI1.SCSCR.BIT.TIE = 1;		//オンにする → なぜかいきなり割り込み入る(動作的にはうれしいけど…)
	}

	return ack;
}

/*********************
SCI1 1文字送信
引数：文字
返値：バッファがいっぱいだったら0x15
**********************/
char SCI1_tx_char(char moji){
	char ack=1;
	
	while(ack!=0){	//バッファが空くまで繰り返す
		ack = SCI1_write_buff(moji);	
	}
	return ack; 
}


/*********************
SCI1 文字列送信
引数：文字列
**********************/
void SCI1_tx_str(char *in_str){
	
	while(*in_str != '\0'){			//\0にしたけど大丈夫？まえは0
		SCI1_tx_char(*in_str);
    	in_str++;
	}
}


/*********************
SCI1 10進数字送信
引数：数字,桁数
返値：なし
longまでの数
**********************/
void SCI1_tx_d(long fig){
	unsigned int i;
	char buf[16];
	
	i=0;
	do{		//とりあえず下のケタから埋めていく
		buf[i]=( (fig%10) | '0');
		i++;
		if(i>15)	fig=0;
		fig = fig/10;	//その桁を1の位に持ってくる
	}while(fig);		//割り切ったら終わり
	
	while(i>0){			//bufの中身を後ろから出力
		SCI1_tx_char(buf[i-1]);
		i--;
	}
	
}


/*********************
SCI1 16進数字送信(小文字)
引数：数字,桁数
返値：なし
longまでの数
**********************/
void SCI1_tx_x(long fig){
	unsigned int i;
	char buf[16];
	char tmp;
	
	i=0;
	do{		//とりあえず下のケタから埋めていく
		tmp = (fig & 0xf); 	//下のケタを抽出
		if(tmp<10){			//0～9
			buf[i]=( tmp | '0');
		}else{
			buf[i]=( (tmp-10) + 'a');
		}
		i++;
		if(i>15)	fig=0;
		fig = fig>>4;		//その桁を1の位に持ってくる
	}while(fig);			//割り切ったら終わり
	
	while(i>0){			//bufの中身を後ろから出力
		SCI1_tx_char(buf[i-1]);
		i--;
	}
	
}

/*********************
SCI1 16進数字送信(大文字)
引数：数字,桁数
返値：なし
longまでの数
**********************/
void SCI1_tx_X(long fig){
	unsigned int i;
	char buf[16];
	char tmp;
	
	i=0;
	do{		//とりあえず下のケタから埋めていく
		tmp = (fig & 0xf); 	//下のケタを抽出
		if(tmp<10){			//0～9
			buf[i]=( tmp | '0');
		}else{
			buf[i]=( (tmp-10) + 'A');
		}
		i++;
		if(i>15)	fig = 0;
		fig = fig>>4;		//その桁を1の位に持ってくる
	}while(fig);			//割り切ったら終わり
	
	while(i>0){			//bufの中身を後ろから出力
		SCI1_tx_char(buf[i-1]);
		i--;
	}
}


/*********************
SCI1 1文字受信
引数：なし
返値：文字
来るまで待つので注意
**********************/
char SCI1_rx_char(void){
	char data;
	
	while ((SCI1.SCSSR.BIT.RDRF) == 0);	// 受信待ち
	SCI1.SCSSR.BIT.RDRF = 0;	// RDRFクリア
	data = SCI1.SCRDR;
	
	return data;
}


/*********************
SCI1 1文字受信(待ちなし)
引数：なし
返値：文字
呼び出しもとで来てるか確認する必要あり
**********************/
char SCI1_rx_char_nowait(void){
	char data;
	
	if(SCI1.SCSSR.BIT.RDRF){
		SCI1.SCSSR.BIT.RDRF = 0;	// RDRFクリア
		data = SCI1.SCRDR;
	}else{
		data = 0x15;
	}
	
	return data;
}



/*********************
SCI1 バッファポインタのインクリメント
引数：今のポインタ
返値：次のポインタ
**********************/
unsigned short inc_buff_p1(unsigned short p){
	if(p < SCI1_TxBuffSize-1){
		p++;
	}else{
		p = 0;
	}
	
	return p;
}


void SCI1_intpre(void){
	//シリアルの受信割り込みの関数です。割り込みで入るようにintpreg.cの中から飛ばします

//	PS2_command=SCI1.SCRDR;
	PC_command_number++;
    from_pc_sci[PC_command_number]=SCI1.SCRDR;//今よみとった値を一時的に格納します。
    SCI1.SCSSR.BIT.RDRF=0;
	if(PC_command_number>1){
		if(from_pc_sci[PC_command_number-1]==255 && from_pc_sci[PC_command_number]==0 && PC_command_flag==0){
			//使える信号が来る
			PC_command_flag=1;

		}else if(from_pc_sci[PC_command_number-1]==255 && from_pc_sci[PC_command_number]==0 && PC_command_flag==1){
			//使える信号が入った　実行
			PC_move_command_flag=1;
			PC_move_command_number=PC_command_number-3;
			PC_command_flag=0;
			PC_command_number=0;
		}else if(PC_command_flag==1){
			pc_command[PC_command_number-3]=from_pc_sci[PC_command_number];
		}
	}
}

void SCI1_err(void){//同じくエラー割り込みです。
       SCI1.SCSSR.BYTE &= ~0xb8;
}
