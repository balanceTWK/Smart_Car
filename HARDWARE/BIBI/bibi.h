#ifndef __BIBI_H
#define __BIBI_H	 
#include "sys.h"

#define BIBI           PBout(11)// PB11
#define BigLED         PBout(10)// PB10

#define LeftCollide    PEin(14)// PE14
#define RightCollide   PEin(15)// PE15
#define Collide        PEin(8)// PE8
#define Stop    PFin(8)// PF8
void BigLED_Init(void);		 				    
#endif



