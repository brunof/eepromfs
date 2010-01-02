///////////////////////////////////////////////////////////////////////////
////   Library for a 24LC256 serial EEPROM                             ////
////                                                                   ////
////   init_ext_eeprom();    Call before the other functions are used  ////
////                                                                   ////
////   write_ext_eeprom(a, d);  Write the byte d to the address a      ////
////                                                                   ////
////   d = read_ext_eeprom(a);   Read the byte d from the address a    ////
////                                                                   ////
////   The main program may define eeprom_sda                          ////
////   and eeprom_scl to override the defaults below.                  ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////


#ifndef EEPROM_SDA

#define EEPROM_SDA  PIN_B1
#define EEPROM_SCL  PIN_B0

#endif

#use i2c(master, FAST,sda=EEPROM_SDA, scl=EEPROM_SCL)

#define EEPROM_ADDRESS long int
#define EEPROM_SIZE   32768

void init_ext_eeprom()
{
   output_float(EEPROM_SCL);
   output_float(EEPROM_SDA);

}

void write_ext_eeprom(long int address, BYTE data)
{
   short int status;

   do{                           //wait if previous writing...
      i2c_start();
      status=i2c_write(0xa0);      
   }while(status==1);

   i2c_write(address>>8);
   i2c_write(address);
   i2c_write(data);
   i2c_stop();
}

//quantity should be smaller than 0x40 for 24LC256...
void write_page_ext_eeprom(long int address, BYTE* data,BYTE quantity)
{
   short int status;

   do{                           //wait if previous writing...
      i2c_start();
      status=i2c_write(0xa0);      
   }while(status==1);

   i2c_write(address>>8);
   i2c_write(address);
   while(quantity--){
      i2c_write(*data);
      data++;
   }
   i2c_stop();
}

BYTE read_ext_eeprom(long int address) {
   BYTE data;
   short int status;

   do{                           //wait if previous writing...
      i2c_start();
      status=i2c_write(0xa0);      
   }while(status==1);
   
   i2c_write(address>>8);
   i2c_write(address);
   i2c_start();
   i2c_write(0xa1);
   data=i2c_read(0);
   i2c_stop();
   return(data);
}

//quantity should be smaller than 0x8000 for 24LC256...
void read_page_ext_eeprom(long int address, char * ptrOut, long int quantity) {
   short int status;

   if(!quantity) return;

   do{                           //wait if previous writing...
      i2c_start();
      status=i2c_write(0xa0);      
   }while(status==1);
   
   i2c_write(address>>8);
   i2c_write(address);
   i2c_start();
   i2c_write(0xa1);
   while(--quantity){
      *ptrOut=i2c_read();
      ptrOut++;
   }
   *ptrOut=i2c_read(0);
   i2c_stop();
}
