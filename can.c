/********************************************

------------------------------------
Code Warrior 5.9.0
Target : MC9S12XET256
Crystal: 16.000Mhz
busclock:40.000MHz
pllclock:80.000MHz
============================================*/
#include "derivative.h" 
#include "global_variables.h"
#include "def_all.h"


#ifndef NO_ERR
  #define NO_ERR 0x00
  #define IS_ERR 0x01
#endif
 



#pragma CODE_SEG DEFAULT

unsigned long extended_id_mscanid(unsigned long id)
//extend frame id
{
   unsigned long tmID=0;
   tmID=id<<1;   
   tmID=((tmID&0x3ff80000)<<2)|(tmID&0x000fffff|0x00180000);   
   return tmID;   
}  
unsigned long standard_id_mscanid(unsigned long id)
//extend frame id
{
   unsigned long tmID=0;
   tmID=id<<21;   
   //tmID=((tmID&0x3ff80000)<<2)|(tmID&0x000fffff|0x00180000);   
   return tmID;   
}
// -------------------------------sendframe-----------------------
int CAN0SendFrame(unsigned long id, unsigned char priority, unsigned char length, unsigned char *txdata )
{    
  unsigned char index;                          // number for read message
  unsigned char tbsel = 0;                      // symbol for CAN0TBSEL     
  int cnt=0;  
  
  if (!CAN0TFLG) return IS_ERR;                 /* Is Transmit Buffer full?? */            
  CAN0TBSEL = CAN0TFLG;                         /* Select lowest empty buffer */
  tbsel = CAN0TBSEL;		                        /* Backup selected buffer */       
  /* Load Id to IDR Registers */
  *((unsigned long *) ((unsigned long)(&CAN0TXIDR0)))= id;
  if(length>8) length=8;    
  for (index=0;index<length;index++) 
  {
     *(&CAN0TXDSR0 + index) = txdata[index];   //  Load data to Tx buffer  Data Segment Registers(ONLY 8 BYTES?)                                                   
  }
  CAN0TXDLR  = length;                          /* Set Data Length Code */
  CAN0TXTBPR = priority;                        /* Set Priority */
  CAN0TFLG   = tbsel;	                          /* Start transmission */
  while ((CAN0TFLG & tbsel) != tbsel)
    if(++cnt>10000) return IS_ERR;           //  Wait for Transmission completion
  return NO_ERR;
}
// -------------------------------sendframe-----------------------
int CAN1SendFrame(unsigned long id, unsigned char priority, unsigned char length, unsigned char *txdata )
{    
  unsigned char index;                          // number for read message
  unsigned char tbsel = 0;                      // symbol for CAN0TBSEL     
  int cnt=0;  
  
  if (!CAN1TFLG) return IS_ERR;                 /* Is Transmit Buffer full?? */            
  CAN1TBSEL = CAN1TFLG;                         /* Select lowest empty buffer */
  tbsel = CAN1TBSEL;		                        /* Backup selected buffer */       
  /* Load Id to IDR Registers */
  *((unsigned long *) ((unsigned long)(&CAN1TXIDR0)))= id;
  if(length>8) length=8;    
  for (index=0;index<length;index++) 
  {
     *(&CAN1TXDSR0 + index) = txdata[index];   //  Load data to Tx buffer  Data Segment Registers(ONLY 8 BYTES?)                                                   
  }
  CAN1TXDLR  = length;                          /* Set Data Length Code */
  CAN1TXTBPR = priority;                        /* Set Priority */
  CAN1TFLG   = tbsel;	                          /* Start transmission */
  while ((CAN1TFLG & tbsel) != tbsel)
    if(++cnt>10000) return IS_ERR;           //  Wait for Transmission completion
  return NO_ERR;
}

// -------------------------------sendframe-----------------------
int CAN4SendFrame(unsigned long id, unsigned char priority, unsigned char length, unsigned char *txdata )
{    
  unsigned char index;                          // number for read message
  unsigned char tbsel = 0;                      // symbol for CAN4TBSEL 
  int cnt=0;    
  
  if (!CAN4TFLG) return IS_ERR;                 /* Is Transmit Buffer full?? */            
  CAN4TBSEL = CAN4TFLG;                         /* Select lowest empty buffer */
  tbsel = CAN4TBSEL;		                        /* Backup selected buffer */       
  /* Load Id to IDR Registers */
  *((unsigned long *) ((unsigned long)(&CAN4TXIDR0)))= id;//½ÓÊÕ·½
  if(length>8) length=8;    
  for (index=0;index<length;index++) 
  {
     *(&CAN4TXDSR0 + index) = txdata[index];   //  Load data to Tx buffer  Data Segment Registers(ONLY 8 BYTES?)                                                   
  }
  CAN4TXDLR  = length;                          /* Set Data Length Code */
  CAN4TXTBPR = priority;                        /* Set Priority */
  CAN4TFLG   = tbsel;	                          /* Start transmission */
  //printp("\nCAN4TXIDR0~CAN4TXIDR3:%04x,%04x,%04x,%04x",CAN4TXIDR0,CAN4TXIDR1,CAN4TXIDR2,CAN4TXIDR3) ;
  while ((CAN4TFLG & tbsel) != tbsel)
    if(++cnt>10000) return IS_ERR;          //  Wait for Transmission completion
  //putstr("\nCAN4 Sending is successful!");
  return NO_ERR;
}

//byte can0rxdata[3][8];
//byte can4rxdata[3][8];

#pragma CODE_SEG __NEAR_SEG NON_BANKED 
void interrupt 38  CAN0RxISR(void)
//void interrupt   CAN0RxISR(void)
{
    byte length,index;  
    unsigned long id,temp1;
    length = (CAN0RXDLR & 0x0F);
    if ((CAN0RXIDR1&0x08)==0)//standard frame
    {
        id = *((unsigned long *) ((unsigned long)(&CAN0RXIDR0)));
        g_str_can0data.u32_can_id = id>>21;
    } 
    else  //extend frame
    {
        id = *((unsigned long *) ((unsigned long)(&CAN0RXIDR0)));
        temp1 = (id & 0xffe00000)>>3        ;
        id = (id & 0x0007fffe) >> 1;
        g_str_can0data.u32_can_id = temp1 | id;
    }
        
        
    //g_str_can0data.u32_can_id = *((unsigned long *) ((unsigned long)(&CAN0RXIDR0)));
    for (index=0; index<length; index++) 
    {
        g_str_can0data.u8_can_data[index] = *(&CAN0RXDSR0 + index); /* Get received data */  
    }
    CAN0RFLG = 0x01;   /* Clear RXF */
}
void interrupt 54  CAN4RxISR(void)
//void interrupt   CAN4RxISR(void)
{
    byte length,index,rxdlr; 
    length = (CAN4RXDLR & 0x0F);
    rxdlr=CAN4RXIDR0&0X0F;
    for (index=0; index<length; index++) 
    {
        //can4rxdata[rxdlr][index] = *(&CAN4RXDSR0 + index); /* Get received data */  
    }
        CAN4RFLG = 0x01;   /* Clear RXF */
} 
