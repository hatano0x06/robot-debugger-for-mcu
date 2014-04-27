/********************************************/
/*				SCI0�֐�					*/
/*					for SH7125 @ HEW		*/
/*					Wrote by conpe_			*/
/*							2010/12/17		*/
/********************************************/
//SCI0�̓T�[�{�֘A
//SCI0�p�̑���M�֐��ł��B

// �o�b�t�@���g���ď������荞�݂ɂ�著�M
// �o�b�t�@�̃T�C�Y��.h�̕��Ŏw��
// intprg.c��INT_SCI0_TXI0����intSCI0_tx_buff()���Ăяo���ĂˁB
// resetprg.c��#define SR_Init��0x00000000�ɂ���

// �o�b�t�@�����ӂꂻ���ɂȂ�����A���M���ǂ����܂ő҂̂Œ��ӁB

//#include <stdarg.h>
#include "iodefine.h"
#include "sci.h"
#include "global.h"


char sci0_tx_buffer[SCI0_TxBuffSize];	//�o�b�t�@
unsigned short sci0_tx_writep;			//�������݃|�C���^
unsigned short sci0_tx_readp;			//�ǂݍ��݃|�C���^


/*********************
SCI0������
�����F�{�[���[�g 4800�`115200
**********************/
void SCI0_setup(int br){
	unsigned short i = 3 * 8;	

	STB.CR3.BIT._SCI0=0;		// SCI1 ����J�n
	SCI0.SCSCR.BYTE = 0x00;
	SCI0.SCSCR.BIT.CKE = 0x00;	// �N���b�N�\�[�X
	SCI0.SCSMR.BYTE = 0x00;		// ����������, 8bit, �p���e�B����, �X�g�b�v�r�b�g1, �}���`�v���Z�b�T�L��
	SCI0.SCSMR.BIT.MP=1;
	SCI0.SCSMR.BIT.CKS = 0;		// �N���b�N
	SCI0.SCSDCR.BIT.DIR = 0x00;	// LSB���瑗��M
	
	SCI0.SCBRR = Pphi*2*1000000/64/br-1;	//�{�[���[�g�̐ݒ�
	
	while(i--);

	PFC.PAIORL.BIT.B0=0;
	PFC.PAIORL.BIT.B1=1;

	PFC.PACRL1.BIT.PA0MD = 1;	// PA3 �[�q�@�\�ݒ� RXD1����
	PFC.PACRL1.BIT.PA1MD = 1;	// PA4 �[�q�@�\�ݒ� TXD1�o��
	
	SCI0.SCSCR.BIT.RE =1;
	SCI0.SCSCR.BIT.TE =1;
	SCI0.SCSCR.BIT.RIE = 1; 	
	//���荞�݋���
	INTC.IPRL.BIT._SCI0 = 15;	// ���荞�݃��x��	
	
//	SCI0.SCSSR.BYTE;                    //�_�~�[���[�h
//	SCI0.SCSSR.BYTE = 0x80;             //�G���[�t���O�N���A

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
	
	SCI0.SCTDR = data;				//SCTDR: �V���A�����M����f�[�^���i�[����8�r�b�g�̃��W�X�^
	SCI0.SCSSR.BIT.MPBT = 1; 		//id�p�Ƀp���e�B�r�b�g��1�ɂ���
	SCI0.SCSSR.BIT.TDRE = 0; 		// �t���O�N���A 
	while( !SCI0.SCSSR.BIT.TDRE ); 	// SCTDR���󂭂܂ő҂�
	while( !SCI0.SCSSR.BIT.TEND ); 

	return; 	
	
}

void SCI0_intpre(void){
	//�V���A���̎�M���荞�݂̊֐��ł��B���荞�݂œ���悤��intpreg.c�̒������΂��܂�


	if(SCI0.SCSSR.BIT.MPB==1){
		//����̓}���`�v���Z�b�T�r�b�g�̓ǂݎ������Ă��܂��B����ID���A���Ă���Ȃ�΂������P�ɂȂ�܂��B
		sci0_data_get_i=0;
	}
	
    Get_Byte[sci0_data_get_i]=SCI0.SCRDR;//����݂Ƃ����l���ꎞ�I�Ɋi�[���܂��B

    SCI0.SCSSR.BIT.RDRF=0;
    if((sci0_data_get_i==1)&&(Get_Byte[0]==sci0_id)&&(Get_Byte[1]==6)){
		//����͊m��ACK���A�����Ƃ��̏����ł��B
		sci0_miss=1;
		sci0_wait=1;
//		Get_Byte[0]=0;
//		Get_Byte[1]=0;
	}else if((sci0_data_get_i==1)&&((Get_Byte[0]!=sci0_id)||(Get_Byte[1]!=6))){
		//����͊m��NACK���A�����Ƃ��̏����ł��B
		sci0_miss=0;
		sci0_wait=1;
//		Get_Byte[0]=0;
//		Get_Byte[1]=0;
	}else{
		sci0_data_get_i++;
	}
}

void SCI0_err(void){//�������G���[���荞�݂ł��B
       SCI0.SCSSR.BYTE &= ~0xb8;
}
