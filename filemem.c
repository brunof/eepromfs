#include "filemem.h"
#include "sysdef.h"
#include <24256.C>

int8 eepromfs_flag_error;
//char aux81,aux82,aux83;
//unsigned int16 aux161,aux162,aux163;

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
      write_ext_eeprom(i,EMPTY_VALUE);     //indicate empty block
      delay_ms(5);
   }
   
   //data block shouldnt need to be formated

   //success!
   return TRUE;
}

//Creates a file. If files is out of bounds, already exists or no space aviable, returns with flag errors
short eepromfs_fileTouch(int8 fileNmr)
{
   int8 aux83;
   int16 aux161;

   eepromfs_flag_error=ERR_NO_ERRORS;

   //Check if file Number is inside limits...
   if(!eepromfs_fileInBounds(fileNmr)) return FALSE;
   
   //Check if file already exists...
   if(eepromfs_fileExists(fileNmr)) return FALSE;
   
   //Search for an empty block to store the file...
   aux83=eepromfs_findEmptyBlock();
   
   //Now, write INDEX data
   //get INDEX BLOCK START ADDRESS
   aux161=eepromfs_getAddress(INDEX_BLOCK_START_ADDR);
   //add file number...
   aux161+=fileNmr;

   //write index data. now index points to block :)   
   write_ext_eeprom(aux161,aux83);
   delay_ms(5);

   //write block state & control values...
   eepromfs_setBlockIdentifiers(aux83,1,0x00,0x00);       //indicate that file haves 0 bytes & some other important stuff...

   //return with success...
   return TRUE;
}

//Deletes a file. If files is out of bounds or doesnt exists returns with flag errors
short eepromfs_fileRemove(int8 fileNmr)
{
   int8 state,control,control2;

   int8 aux83;
   int16 aux161;

   //Check that file number is valid & that it exists...
   if(!eepromfs_fileInBounds(fileNmr)) return FALSE;
   if(!eepromfs_fileExists(fileNmr)) return FALSE;

   //we will first, go to file block info...
   aux161=eepromfs_getAddress(INDEX_BLOCK_START_ADDR);
   aux161+=fileNmr;

   //read first block number of file...
   aux83=read_ext_eeprom(aux161);

   //delete this file INDEX BLOCK info...
   write_ext_eeprom(aux161,EMPTY_VALUE);
   delay_ms(5);

   do{
      //get block info...
      eepromfs_getBlockIdentifiers(aux83,&state,&control,&control2);
      //free this block!!!
      eepromfs_setBlockIdentifiers(aux83,0,0x00,0x00);
      //point to next block to erase...
      aux83=control2; 
      //if file continue in other block, continue deleting blocks...
   }while(bit_test(control,7));

   //Success!
   return TRUE;
}


int16 eepromfs_fileSize(int8 fileNmr)
{
   int8 aux83;
   int16 aux161,aux162,aux163;
   int8 state,control,control2;
   
   eepromfs_flag_error=ERR_NO_ERRORS;

   if(!eepromfs_fileInBounds(fileNmr)) return 0;

   if(!eepromfs_fileExists(fileNmr)) return 0;

   //get block size info...
   aux162=eepromfs_getAddress(BLOCK_SIZE_ADDR);
   //write real block size...(!16 bits should change -2 for -3)
   aux162-=2;

   //we will first, go to file block info...
   aux161=eepromfs_getAddress(INDEX_BLOCK_START_ADDR);
   aux161+=fileNmr;
   //read first block number of file...
   aux83=read_ext_eeprom(aux161);   

   //init counter...
   aux163=0;

   //get file first block info...
   eepromfs_getBlockIdentifiers(aux83,&state,&control,&control2);

   //if file continue in other block, continue calculating size...
   while(bit_test(control,7))
   {
      //get next block info...
      eepromfs_getBlockIdentifiers(aux83,&state,&control,&control2);

      //point to next block...
      aux83=control2; 
      
      //add block bytes to size
      aux163+=aux162;
   }

   //if last file block, only add bytes indicated in control2 identifier...
   aux163+=control2;

   //return with file size
   return aux163;

}

short eepromfs_fileWrite(int8 file,int16 startPos, char * data, int16 quantity)
{

}
short eepromfs_fileRead(int8 file,int16 startPos, char * data, int16 quantity)
{

}
short eepromfs_fileCopy(int8 fileSource,int8 fileDestiny)
{

}












/////////////////////////////////////////////////
//AUXILIAR FUNCTIONS...
/////////////////////////////////////////////////

unsigned int16 eepromfs_getAddress(unsigned int16 VarPos)
{
   int8 aux81,aux82;

   aux81=read_ext_eeprom(VarPos+1);
   aux82=read_ext_eeprom(VarPos);

   return make16(aux82,aux81);   
}

short eepromfs_fileInBounds(int8 fileNmr)
{
   int16 aux161,aux162;

   //get INDEX BLOCK START ADDRESS
   aux161=eepromfs_getAddress(INDEX_BLOCK_START_ADDR);

   //get INDEX BLOCK END ADDRESS
   aux162=eepromfs_getAddress(INDEX_BLOCK_END_ADDR);

   //point to eeprom position were file index should be
   aux161+=fileNmr;
   //if file is out of bounds, abort...
   if(aux161>aux162){ eepromfs_flag_error|=ERR_OUT_OF_BOUNDS; return FALSE; };

   return TRUE;
}

short eepromfs_fileExists(int8 fileNmr)
{
   int16 aux161;

   eepromfs_flag_error=ERR_NO_ERRORS;
   
   //get INDEX BLOCK START ADDRESS
   aux161=eepromfs_getAddress(INDEX_BLOCK_START_ADDR);
  
   //point to eeprom position were file index should be
   aux161+=fileNmr;

   //if already exists, return FALSE...
   if(read_ext_eeprom(aux161)!=EMPTY_VALUE){
      eepromfs_flag_error|=ERR_FILE_ALREADY_EXISTS;
      return TRUE;
   }else{
      eepromfs_flag_error|=ERR_FILE_DOESNT_EXISTS;
      return FALSE;
   }
}

int8 eepromfs_findEmptyBlock(void)
{
   int8 aux82;
   int16 aux161,aux162,aux163;

   //get INDEX BLOCK START ADDRESS
   aux161=eepromfs_getAddress(INDEX_BLOCK_START_ADDR);

   //get INDEX BLOCK END ADDRESS
   aux162=eepromfs_getAddress(FREE_BLOCK_END_ADDR);

   //Get file max quantity(same as block count...)
   aux162-=aux161;
   aux162++;

   //get FREE BLOCK START ADDRESS
   aux161=eepromfs_getAddress(FREE_BLOCK_START_ADDR);

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

short eepromfs_setBlockIdentifiers(int8 blockNmr, short state, int8 control,int8 control2)
{
   int8 aux81,aux82;
   int16 aux161,aux162,aux163;

   aux82=blockNmr;
   //get FREE BLOCK START ADDRESS
   aux161=eepromfs_getAddress(FREE_BLOCK_START_ADDR);

   while(aux82>7)
   {
      aux161++;
      aux82-=7;
   }

   //get 8 block info...
   aux81=read_ext_eeprom(aux161);

   if(state) bit_set(aux81,blockNmr); else bit_clear(aux81,blockNmr);
   
   //write result...
   write_ext_eeprom(aux161,aux81);
   delay_ms(5);

   //get DATA BLOCK START ADDRESS
   aux161=eepromfs_getAddress(DATA_BLOCK_START_ADDR);
   
   //get BLOCK SIZE
   aux162=eepromfs_getAddress(BLOCK_SIZE_ADDR);

   //calculate selected empty block address...
   aux163=aux161+aux162*blockNmr;                               //data block start address + (block size * empty block finded)                                      
   aux163+=aux162-2;                                           //jump to identifiers zone

   //write values...
   write_ext_eeprom(aux163++,control);    
   delay_ms(5);

   write_ext_eeprom(aux163++,control2);
   delay_ms(5);
}

short eepromfs_getBlockIdentifiers(int8 blockNmr, int8 * state,int8 *control, int8 * control2)
{
   int8 aux81,aux82;
   int16 aux161,aux162;
   
   //if block number is empty value, return with fail...
   if(blockNmr==EMPTY_VALUE) return FALSE;   

   //get FREE BLOCK START ADDRESS
   aux161=eepromfs_getAddress(FREE_BLOCK_START_ADDR);

   aux82=blockNmr;

   while(aux82>7)
   {
      aux161++;
      aux82-=7;
   }

   //get 8 block info...
   aux81=read_ext_eeprom(aux161);

   //and set pointer according to free block value
   if(bit_test(aux81,blockNmr)) *state=TRUE; else *state=FALSE;
   
   //now, lets read block control info...
   aux161=eepromfs_getAddress(DATA_BLOCK_START_ADDR);
   aux162=eepromfs_getAddress(BLOCK_SIZE_ADDR);

   //go to block data init...
   aux161+=(aux162*blockNmr);
   //add bytes to get control address(!16 bits should change -2 for -3)
   aux161+=(aux162-2);

   //read control info...
   *control=read_ext_eeprom(aux161++);
   //read control2 info...
   *control2=read_ext_eeprom(aux161++);

   return TRUE;
}
   
   ////int8 eepromfs_fileSetProperties(int8 file,int8 properties)
   ////int8 eepromfs_fileGetProperties(int8 file)

   //???
   //int16 eepromfs_freeSpace(void)
   //int16 eepromfs_totalSpace(void)
   //int16 eepromfs_usedSpace(void)
