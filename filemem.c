#include "filemem.h"
#include "sysdef.h"
#include <24256.C>

char aux81,aux82,aux83;
unsigned int16 aux161,aux162,aux163;


int8 eepromfs_flag_error;
//[ size | serial_number | block_size | free_block_start | free_block_end | index_block_start | index_block_end | data_block_start | data_block_end | system_id ]
//short eepromfs_format(char mem_size, int32 ser_num, int16 blck_size, char* sys_id){
short eepromfs_format(char * sys_id)
{
   unsigned int16 i;

   init_ext_eeprom();

   //write memory struct values...
   for(i=0;i<sysDefaultValues_len;i++)
   {
      write_ext_eeprom(i,sysDefaultValues[i]);
      delay_ms(5);
   }

   //write system_id array...
   i=SYSTEM_ID_START_ADDR;
   while(TRUE){
      write_ext_eeprom(i,*sys_id);
      if(*sys_id=='\0')  break;
      sys_id++;
      i++;
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

//Creates a file. If files is out of bounds, already exists or no space aviable, returns with flag errors
short eepromfs_fileTouch(int8 fileNmr)
{
   eepromfs_flag_error=ERR_NO_ERRORS;

   //Check if file Number is inside limits...
   if(!eepromfs_fileInBounds(fileNmr)) return FALSE;
   
   //Check if file already exists...
   if(!eepromfs_fileExists(fileNmr)) return FALSE;
   
   //Search for an empty block to store the file...
   aux83=eepromfs_findEmptyBlock();
   
   //Now, write INDEX data
   //get INDEX BLOCK START ADDRESS
   aux81=read_ext_eeprom(INDEX_BLOCK_START_ADDR+1);
   aux82=read_ext_eeprom(INDEX_BLOCK_START_ADDR);
   //store it in aux var
   aux161=make16(aux82,aux81);   
   //add file number...
   aux161+=fileNmr;

   //write index data. now index points to block :)   
   write_ext_eeprom(aux161,aux83);
   delay_ms(5);

   //write control values...
   eepromfs_writeBlockIdentifiers(aux83,0x00,0x00);       //indicate that file haves 0 bytes & some other important stuff...

   //return with success...
   return TRUE;

}

//AUXILIAR FUNCTIONS...
short eepromfs_fileInBounds(int8 fileNmr)
{
   //get INDEX BLOCK START ADDRESS
   aux81=read_ext_eeprom(INDEX_BLOCK_START_ADDR+1);
   aux82=read_ext_eeprom(INDEX_BLOCK_START_ADDR);
   //store it in aux var
   aux161=make16(aux82,aux81);

   //get INDEX BLOCK END ADDRESS
   aux81=read_ext_eeprom(INDEX_BLOCK_END_ADDR+1);
   aux82=read_ext_eeprom(INDEX_BLOCK_END_ADDR);
   //store it in aux var
   aux162=make16(aux82,aux81);

   //point to eeprom position were file index should be
   aux161+=fileNmr;
   //if file is out of bounds, abort...
   if(aux161>aux162){ eepromfs_flag_error|=ERR_OUT_OF_BOUNDS; return FALSE; };

   return TRUE;
}

short eepromfs_fileExists(int8 fileNmr)
{
   
   eepromfs_flag_error=ERR_NO_ERRORS;
   
   //get INDEX BLOCK START ADDRESS
   aux81=read_ext_eeprom(INDEX_BLOCK_START_ADDR+1);
   aux82=read_ext_eeprom(INDEX_BLOCK_START_ADDR);
   //store it in aux var
   aux161=make16(aux82,aux81);   
  
   //point to eeprom position were file index should be
   aux161+=fileNmr;

   //if already exists, return FALSE...
   if(read_ext_eeprom(aux161)!=EMPTY_VALUE){
      eepromfs_flag_error|=ERR_FILE_ALREADY_EXISTS;
      return FALSE;
   }else{
      return TRUE;
   }
}

int8 eepromfs_findEmptyBlock(void)
{

   //get INDEX BLOCK START ADDRESS
   aux81=read_ext_eeprom(INDEX_BLOCK_START_ADDR+1);
   aux82=read_ext_eeprom(INDEX_BLOCK_START_ADDR);
   //store it in aux var
   aux161=make16(aux82,aux81);

   //get INDEX BLOCK END ADDRESS
   aux81=read_ext_eeprom(INDEX_BLOCK_END_ADDR+1);
   aux82=read_ext_eeprom(INDEX_BLOCK_END_ADDR);
   //store it in aux var
   aux162=make16(aux82,aux81);

   //Get file max quantity(same as block count...)
   aux162-=aux161;
   aux162++;

   //get FREE BLOCK START ADDRESS
   aux81=read_ext_eeprom(FREE_BLOCK_START_ADDR+1);
   aux82=read_ext_eeprom(FREE_BLOCK_START_ADDR);
   //store it in aux var
   aux161=make16(aux82,aux81);

   //initialize block counter...
   aux163=0;
   
   //search for an empty block(8 blocks at a time)
   while(aux82=read_ext_eeprom(aux161++)==0xFF && aux163<aux162){ aux163+=8; }

   //search especific block in the octet...
   while(bit_test(aux82,0) && aux163<aux162){
      aux163++;
      aux82>>=1;
   }
   
   //return low byte of var witch contains block number...
   return aux163;

}

short eepromfs_writeBlockIdentifiers(int8 blockNmr,int8 control,int8 control2)
{
   //get DATA BLOCK START ADDRESS
   aux81=read_ext_eeprom(DATA_BLOCK_START_ADDR+1);
   aux82=read_ext_eeprom(DATA_BLOCK_START_ADDR);
   //store it in aux var
   aux161=make16(aux82,aux81);
   
   //get BLOCK SIZE
   aux81=read_ext_eeprom(BLOCK_SIZE_ADDR+1);
   aux82=read_ext_eeprom(BLOCK_SIZE_ADDR);
   //store it in aux var
   aux162=make16(aux82,aux81);

   //calculate selected empty block address...
   aux163=aux161+aux162*blockNmr;                               //data block start address + (block size * empty block finded)                                      
   aux163+=aux162-2;                                           //jump to identifiers zone

   //write values...
   write_ext_eeprom(aux163++,control);    
   delay_ms(5);

   write_ext_eeprom(aux163++,control2);
   delay_ms(5);

}


   //short eepromfs_blockState(int16 block)
   //eepromfs_fileTouch(int8 file_name,int8 flags)
   //eepromfs_fileRemove(int8 file)
   //short eepromfs_fileExists(int8 file)
   //int16 eepromfs_fileSize(int8 file)   
   //int8 eepromfs_fileSetProperties(int8 file,int8 properties)
   //int8 eepromfs_fileGetProperties(int8 file)
   //int8 eepromfs_fileCopy(int8 fileSource,int8 fileDestiny)
   //short eepromfs_fileWrite(int8 file,int16 startPos, int16 quantity)
   //int8* eepromfs_fileRead(int8 file,int16 startPos, int16 quantity)

   //???
   //int16 eepromfs_freeSpace(void)
   //int16 eepromfs_totalSpace(void)
   //int16 eepromfs_usedSpace(void)
   //short eepromfs_fileAppend(int8 file, int8* data, int16 length,int8 flags)
