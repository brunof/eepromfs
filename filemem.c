#include "filemem.h"

//[ size | serial_number | block_size | free_block_start | index_block_start | data_block_start | system_id ]
short format_memory(char mem_size, int32 ser_num, int16 blck_size, char* sys_id){

   int16 i;
   //write memory size...
   write_ext_eeprom(MEMORY_SIZE_START,mem_size);
   
   //write serial number...
   write_ext_eeprom(SERIAL_NUMBER_START,make8(ser_num,0));
   write_ext_eeprom(SERIAL_NUMBER_START+1,make8(ser_num,1));
   write_ext_eeprom(SERIAL_NUMBER_START+2,make8(ser_num,2));
   write_ext_eeprom(SERIAL_NUMBER_START+3,make8(ser_num,3));
  
   //write block size start
   write_ext_eeprom(BLOCK_SIZE_START,make8(BLOCK_SIZE,0));
   write_ext_eeprom(BLOCK_SIZE_START+1,make8(BLOCK_SIZE,1));
   
   //write free block start
   write_ext_eeprom(FREE_BLOCK_START,make8(FREE_BLOCK,0));
   write_ext_eeprom(FREE_BLOCK_START+1,make8(FREE_BLOCK,1));
   
   //write index block start
   write_ext_eeprom(INDEX_BLOCK_START,make8(INDEX_BLOCK,0));
   write_ext_eeprom(INDEX_BLOCK_START+1,make8(INDEX_BLOCK,1));
    
   //write data block start
   write_ext_eeprom(DATA_BLOCK_START,make8(DATA_BLOCK,0));
   write_ext_eeprom(DATA_BLOCK_START+1,make8(DATA_BLOCK,1));       
    
   //write system ID
   i=SYSTEM_ID_START-1;
   sys_id--;
   do{
      sys_id++;
      i++;
      write_ext_eeprom(i,*sys_id); 
   }while(*sys_id!='\0');    

};
