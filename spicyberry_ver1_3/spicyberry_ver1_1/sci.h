/********************************************/
/*				SCI�֐�						*/
/*					for SH7125 @ HEW		*/
/*					Wrote by conpe_			*/
/*							2010/12/17		*/
/********************************************/

//SCI1�p�̑���M�֐��ł��B

// �o�b�t�@���g���ď������荞�݂ɂ�著�M
// intprg.c��INT_SCI1_TXI1����intSCI1_tx_buff()���Ăяo���ĂˁB
// resetprg.c��#define SR_Init��0x00000000�ɂ���

// �o�b�t�@�����ӂꂻ���ɂȂ�����A�󂭂܂ő҂̂Œ��ӁB


#define Pphi 44.2368/2				//���Ӄ��W���[��������g��[MHz]
#define SCI0_TxBuffSize 64	//�o�b�t�@�T�C�Y
#define SCI1_TxBuffSize 64	//�o�b�t�@�T�C�Y
#define SCI2_TxBuffSize 64	//�o�b�t�@�T�C�Y
