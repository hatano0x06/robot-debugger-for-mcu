/********************************************/
/*				SCI1�֐�					*/
/*					for SH7125 @ HEW		*/
/*					Wrote by conpe_			*/
/*							2010/12/17		*/
/********************************************/
//SCI2��bluetooth�Ƃ̑��ݒʐM
//SCI2�p�̑���M�֐��ł��B

// �o�b�t�@���g���ď������荞�݂ɂ�著�M
// �o�b�t�@�̃T�C�Y��.h�̕��Ŏw��
// intprg.c��INT_SCI1_TXI1����intSCI1_tx_buff()���Ăяo���ĂˁB
// resetprg.c��#define SR_Init��0x00000000�ɂ���

// �o�b�t�@�����ӂꂻ���ɂȂ�����A���M���ǂ����܂ő҂̂Œ��ӁB

//#include <stdarg.h>
#include "iodefine.h"
#include "sci.h"
#include "global.h"


char sci2_tx_buffer[SCI2_TxBuffSize];	//�o�b�t�@
unsigned short sci2_tx_writep;			//�������݃|�C���^
unsigned short sci2_tx_readp;			//�ǂݍ��݃|�C���^


/*********************
SCI1������
�����F�{�[���[�g 4800�`115200
**********************/
void SCI2_setup(int br){
	unsigned short i = 8 * 8;	

	STB.CR3.BIT._SCI2=0;		// SCI1 ����J�n
	SCI2.SCSCR.BYTE = 0x00;
	SCI2.SCSCR.BIT.CKE = 0x00;	// �N���b�N�\�[�X
	SCI2.SCSMR.BYTE = 0x00;		// ����������, 8bit, �p���e�B����, �X�g�b�v�r�b�g1, �}���`�v���Z�b�T����
	SCI2.SCSMR.BIT.CKS = 0;		// �N���b�N
	SCI2.SCSDCR.BIT.DIR = 0x00;	// LSB���瑗��M
	
	SCI2.SCBRR = Pphi*2*1000000/64/br-(115200>br);	//�{�[���[�g�̐ݒ�
//	SCI2.SCBRR = 4;	//�{�[���[�g�̐ݒ�
	while(i--);

	PFC.PAIORL.BIT.B8=0;
	PFC.PAIORL.BIT.B9=1;

	PFC.PACRL3.BIT.PA8MD = 6;	// PA3 �[�q�@�\�ݒ� RXD1����
	PFC.PACRL3.BIT.PA9MD = 6;	// PA4 �[�q�@�\�ݒ� TXD1�o��
	
	SCI2.SCSCR.BIT.RE =1;
	SCI2.SCSCR.BIT.TE =1;
	SCI2.SCSCR.BIT.RIE = 1; 	
	INTC.IPRL.BIT._SCI2 = 15;	// ���荞�݃��x��	
//	SCI2.SCSSR.BYTE;	

	/*
	volatile int i;

	STB.CR3.BIT._SCI2 = 0;          // SCI2���X�^���o�C���畜�A

	SCI2.SCSCR.BYTE = 0;             // �S�Ă̒ʏ퓮��E���荞�݂��J�b�g  
	SCI2.SCSMR.BYTE = 0;             // ���������� 8bit �p���e�B�Ȃ�


	SCI2.SCBRR = Pphi*2*1000000/64/br-(115200>br);                  // �V���A���{�[���[�g���
	for (i = 0; i < 3000; i++) ;
	SCI2.SCSCR.BYTE = 0xf0;          // ����M�E����M���荞�݋���
	SCI2.SCSSR.BYTE;                 // ��x���[�h
	SCI2.SCSSR.BYTE = 0;             // �t���O��������

	INTC.IPRL.BIT._SCI2 = 15;       // ���荞�ݗD�揇�ʐݒ�
*/
}


void SCI2_intpre(void){
	//�V���A���̎�M���荞�݂̊֐��ł��B���荞�݂œ���悤��intpreg.c�̒������΂��܂�
	PS2_command=SCI2.SCRDR;
    SCI2.SCSSR.BIT.RDRF=0;
}

void SCI2_err(void){//�������G���[���荞�݂ł��B
       SCI2.SCSSR.BYTE &= ~0xb8;
	   SCI2.SCSPTR.BIT.EIO=0;
}

void SCI2_tx(char data){
	while(SCI2.SCSSR.BIT.TDRE == 0);	// SCTDR���󂭂܂ő҂�
	SCI2.SCTDR = data;			// SCTDR�ɑ��M�f�[�^����������
	SCI2.SCSSR.BIT.TDRE = 0;		// �t���O�N���A
}

char SCI2_rx(void){
	char data;
	
	if(SCI2.SCSSR.BIT.RDRF){
		SCI2.SCSSR.BIT.RDRF = 0;	// RDRF�N���A
		data = SCI2.SCRDR;
		PE.DRL.BIT.B7=0;	
		
	}else{
		PE.DRL.BIT.B7=1;	
		data = 'b';
	}
	
	return data;
}


