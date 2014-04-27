-------- PROJECT GENERATOR --------
PROJECT NAME :	spicyberry_ver1_1
PROJECT DIRECTORY :	C:\WorkSpace\spicyberry_ver1_1\spicyberry_ver1_1
CPU SERIES :	SH-2
CPU TYPE :	SH7125
TOOLCHAIN NAME :	Renesas SuperH RISC engine Standard Toolchain
TOOLCHAIN VERSION :	9.3.0.0
GENERATION FILES :
    C:\WorkSpace\spicyberry_ver1_1\spicyberry_ver1_1\dbsct.c
        Setting of B,R Section
    C:\WorkSpace\spicyberry_ver1_1\spicyberry_ver1_1\typedefine.h
        Aliases of Integer Type
    C:\WorkSpace\spicyberry_ver1_1\spicyberry_ver1_1\sbrk.c
        Program of sbrk
    C:\WorkSpace\spicyberry_ver1_1\spicyberry_ver1_1\iodefine.h
        Definition of I/O Register
    C:\WorkSpace\spicyberry_ver1_1\spicyberry_ver1_1\intprg.c
        Interrupt Program
    C:\WorkSpace\spicyberry_ver1_1\spicyberry_ver1_1\vecttbl.c
        Initialize of Vector Table
    C:\WorkSpace\spicyberry_ver1_1\spicyberry_ver1_1\vect.h
        Definition of Vector
    C:\WorkSpace\spicyberry_ver1_1\spicyberry_ver1_1\resetprg.c
        Reset Program
    C:\WorkSpace\spicyberry_ver1_1\spicyberry_ver1_1\spicyberry_ver1_1.cpp
        Main Program
    C:\WorkSpace\spicyberry_ver1_1\spicyberry_ver1_1\sbrk.h
        Header file of sbrk file
    C:\WorkSpace\spicyberry_ver1_1\spicyberry_ver1_1\stacksct.h
        Setting of Stack area
START ADDRESS OF SECTION :
 H'000000000	DVECTTBL,DINTTBL,PIntPRG
 H'000000800	PResetPRG
 H'000001000	P,C,C$BSEC,C$DSEC,C$INIT,D
 H'0FFFFA000	B,R
 H'0FFFFBC00	S

* When the user program is executed,
* the interrupt mask has been masked.
* 
* Program start H'1000.
* RAM start H'FFFFA000.

DATE & TIME : 2011/10/07 11:44:43
