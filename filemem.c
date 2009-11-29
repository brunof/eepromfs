#include "filemem.h"
#include "sysdef.h"
#include <24256.C>

//[ size | serial_number | block_size | free_block_start | free_block_end | index_block_start | index_block_end | data_block_start | data_block_end | system_id ]
//short eepromfs_format(char mem_size, int32 ser_num, int16 blck_size, char* sys_id){
short eepromfs_format(char * sys_id){
   int16 i;

   init_ext_eeprom();

   //write memory struct values...
   for(i=0;i<sysDefaultValues_len;i++)
   {
      write_ext_eeprom(i,sysDefaultValues[i]);
      delay_ms(5);
   }

   //now, format blocks:
   
   //free block FORMAT:
   for(i=FREE_BLOCK_START;i<=FREE_BLOCK_END;i++)
   {
      write_ext_eeprom(i,0x00);
      delay_ms(5);
   }
   
   //index block FORMAT:
   for(i=INDEX_BLOCK_START;i<=INDEX_BLOCK_END;i++)
   {
      write_ext_eeprom(i,0x00);
      delay_ms(5);
   }
   
   //data block shouldnt need to be formated

   //success!
   return TRUE;
}
