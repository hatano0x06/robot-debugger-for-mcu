/********************************************/
/*				SCI関数						*/
/*					for SH7125 @ HEW		*/
/*					Wrote by conpe_			*/
/*							2010/12/17		*/
/********************************************/

//SCI1用の送受信関数です。

// バッファを使って順次割り込みにより送信
// intprg.cのINT_SCI1_TXI1からintSCI1_tx_buff()を呼び出してね。
// resetprg.cの#define SR_Initは0x00000000にする

// バッファがあふれそうになったら、空くまで待つので注意。


#define Pphi 44.2368/2				//周辺モジュール動作周波数[MHz]
#define SCI0_TxBuffSize 64	//バッファサイズ
#define SCI1_TxBuffSize 64	//バッファサイズ
#define SCI2_TxBuffSize 64	//バッファサイズ
