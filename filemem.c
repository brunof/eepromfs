#include "filemem.h"
#include "sysdef.h"
#include "24256.C"

int8 eepromfs_flag_error;

//short eepromfs_format(char mem_size, int32 ser_num, int16 blck_size, char* sys_id){
short eepromfs_format(char * sys_id)
{
   unsigned int16 i;

   init_ext_eeprom();

   //write memory struct values...
   for(i=0;i<sysDefaultValues_len;i++)
   {
      write_ext_eeprom(i,sysDefaultValues[i]);
      DEMORA_EEPROM
   }

   //write system_id array...
   i=SYSTEM_ID_START_ADDR;
   while(TRUE){
      write_ext_eeprom(i,*sys_id);
      DEMORA_EEPROM
      if(*sys_id=='\0')  break;
      sys_id++;
      i++;
   }

   //now, format blocks:

   //free block FORMAT:
   for(i=FREE_BLOCK_START;i<=FREE_BLOCK_END;i++)
   {
      write_ext_eeprom(i,0x00);
      DEMORA_EEPROM
   }
   
   //index block FORMAT:
   for(i=INDEX_BLOCK_START;i<=INDEX_BLOCK_END;i++)
   {
      write_ext_eeprom(i,EMPTY_VALUE);     //indicate empty block
      DEMORA_EEPROM
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
   aux83=eepromfs_findEmptyBlock(0);
   
   if(aux83==EMPTY_VALUE) return FALSE;
   
   //Now, write INDEX data
   //get INDEX BLOCK START ADDRESS
   aux161=eepromfs_getAddress(INDEX_BLOCK_START_ADDR);
   //add file number...
   aux161+=fileNmr;

   //write index data. now index points to block :)   
   write_ext_eeprom(aux161,aux83);
   DEMORA_EEPROM

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
   DEMORA_EEPROM

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

   do{
      //get block info...
      eepromfs_getBlockIdentifiers(aux83,&state,&control,&control2);
      //point to next block...
      aux83=control2; 
      //if file continue in other block, continue blocks...
      
      //add block bytes to size
      if(bit_test(control,7)) aux163+=aux162; else aux163+=control2;
   }while(bit_test(control,7));

   //return with file size
   return aux163;

}

//Writes data in a file...especify file number, starting position were to write
int16 eepromfs_fileWrite(int8 fileNmr,int16 startPos, char * data, int16 quantity)
{
   int16 aux161,aux162,aux163,freeSpace;
   int8 parentBlock,res,oldLastBlock;
   int8 state,control,control2;

   //if nothing to write...Exit(!!!!!!!CHANGE FOR CLOSE FILE!)
   if(!quantity) return 0;

   aux161=eepromfs_fileSize(fileNmr);
   //if startPos is beyond EOF, error...
   if(aux161<startPos) eepromfs_flag_error|=ERR_POSITION_OUT_OF_SIZE;
   
   //get address were to write and block number
   aux162=eepromfs_fileCalculatePos(fileNmr, startPos, &parentBlock);
   
   //jump to begining of block
   aux161=eepromfs_getBlockAddress(parentBlock);
   
   //add real block size to jump to identifier zone...(!16 bits should change -2 for -3)
   aux163=aux161+(eepromfs_getAddress(BLOCK_SIZE_ADDR)-2);
   
   //at this point:  aux161 contains block to write start ADDRESS
   //                aux162 contains ADDRESS to write to
   //                aux163 contains identifier start zone(should not be passed(writing))
   //                parentBlock contiene el bloque actual donde estamos escribiendo...

   //get free space to check if there´s enough free space 
   freeSpace=eepromfs_freeSpace();
   //add free bytes of this block...
   freeSpace+=aux163-aux162;

   //if theres no aviable space, do not bother at all...
   if(quantity>freeSpace){ eepromfs_flag_error|=ERR_OUT_OF_SPACE; return quantity;}

   //store "old" last block...
   oldLastBlock=parentBlock;
   while(quantity)
   {
      if(aux162>=aux163){
         res=eepromfs_findEmptyBlock(0);
         //write block info...(write that this block is NOT free, and that it continues in "res" block.
         if(res==EMPTY_VALUE){
            //if theres no other empty blocks to write to, break!(memory full)
            eepromfs_setBlockIdentifiers(parentBlock,1,0x00,eepromfs_getAddress(BLOCK_SIZE_ADDR)-2);
            break;
         }else{
            eepromfs_setBlockIdentifiers(parentBlock,1,0x80,res);
         }

         //now, continue writing in new block!!!
         parentBlock=res;
         aux162=eepromfs_getBlockAddress(res);
         aux163=aux162+(eepromfs_getAddress(BLOCK_SIZE_ADDR)-2);
      }else{
         write_ext_eeprom(aux162,*data);
         DEMORA_EEPROM
         data++;
         aux162++;
         quantity--;
      }
   }

   //compare: if last byte written is in the same old last block, keep the biggest block quantity...(IN DISCUSS)
   if(oldLastBlock==parentBlock){
      eepromfs_getBlockIdentifiers(oldLastBlock,&state,&control,&control2);
      if(control2<(aux162-eepromfs_getBlockAddress(parentBlock))){
         eepromfs_setBlockIdentifiers(parentBlock,1,0x00,aux162-eepromfs_getBlockAddress(parentBlock));
      }
   }else{
      //write last block(if valid) info...
      if(res!=EMPTY_VALUE){
         eepromfs_setBlockIdentifiers(parentBlock,1,0x00,aux162-eepromfs_getBlockAddress(parentBlock));
      }
   }
   
   return quantity;
}








int16 eepromfs_fileRead(int8 file,int16 startPos, char * data, int16 quantity)
{

}
short eepromfs_fileCopy(int8 fileSource,int8 fileDestiny)
{

}



/////////////////////////////////////////////////
//AUXILIAR FUNCTIONS...
/////////////////////////////////////////////////
int16 eepromfs_fileCalculatePos(int8 fileNmr, int16 position, int8 * parentBlockNmr)
{
   int8 aux83,state,control,control2;
   int16 aux161,blockSize,aux163;
   
   aux161=eepromfs_getAddress(INDEX_BLOCK_START_ADDR);
   aux161+=fileNmr;
   //read first block number of file...
   aux83=read_ext_eeprom(aux161);   

   blockSize=eepromfs_getAddress(BLOCK_SIZE_ADDR);
   //get real block size...(!16 bits should change -2 for -3)
   blockSize-=2;

   //get file first block info...
   eepromfs_getBlockIdentifiers(aux83,&state,&control,&control2);

   //if file continue in other block, continue calculating position...
   while(bit_test(control,7) && position>blockSize)
   {
      //get next block info...
      eepromfs_getBlockIdentifiers(aux83,&state,&control,&control2);

      //if next block, point to next block...
      if(bit_test(control,7)){
         aux83=control2;
         //substract an entire block :)
         position-=blockSize;
      }
   }

   //fill parent block with block Number...
   *parentBlockNmr=aux83;

   //we're reaching to the end of the search...lets calculate final position...
   //first, go to last data block address 
   aux161=eepromfs_getBlockAddress(aux83);
   //and add remaining bytes...
   aux161+=position;

   //return with address
   return aux161;
}


int16 eepromfs_getBlockAddress(int8 blockNmr)
{
   int16 aux161,aux162;
   aux161=eepromfs_getAddress(BLOCK_SIZE_ADDR);
   aux162=eepromfs_getAddress(DATA_BLOCK_START_ADDR);

   return aux162+aux162*blockNmr;
}

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

//search for an empty block starting from the block passed. it returns next block free or EMPTY_VALUE+ERROR if no more blocks avaiable...
int8 eepromfs_findEmptyBlock(int8 fromBlock)
{
   int8 aux82;
   int16 aux161,aux162,aux163;

   //get INDEX BLOCK START ADDRESS
   aux161=eepromfs_getAddress(INDEX_BLOCK_START_ADDR);

   //get INDEX BLOCK END ADDRESS
   aux162=eepromfs_getAddress(INDEX_BLOCK_END_ADDR);

   //Get file max quantity(same as block count...)
   aux162-=aux161;
   aux162++;

   //get FREE BLOCK START ADDRESS
   aux161=eepromfs_getAddress(FREE_BLOCK_START_ADDR);
   aux161+=fromBlock/8;

   //initialize block counter...
   aux163=fromBlock;

   //find if theres a free block in actual block octet
   aux82=read_ext_eeprom(aux161);
   while(aux163%8){
      if(bit_test(aux82,aux163%8)) aux163++; else break;
   }

   //if theres one, return low byte with block number...
   if(aux163%8 && aux163<aux162) return aux163;

   //search for an empty block(8 blocks at a time)
   while(read_ext_eeprom(aux161)==0xFF && aux163<aux162){
      aux163+=8;
      aux161++;
   }

   //get the one who have an empty block
   aux82=read_ext_eeprom(aux161);

   //search especific block in the octet...
   while(bit_test(aux82,0) && aux163<aux162){
      aux163++;
      aux82>>=1;
   }

   //no empty blocks avaiable?
   if(aux163>=aux162){
      eepromfs_flag_error|=ERR_OUT_OF_SPACE;
      return EMPTY_VALUE;
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
      aux82-=8;
   }

   //get 8 block info...
   aux81=read_ext_eeprom(aux161);

   if(state) bit_set(aux81,aux82); else bit_clear(aux81,aux82);
   
   //write result...
   write_ext_eeprom(aux161,aux81);
   DEMORA_EEPROM
   
   //get BLOCK SIZE
   aux162=eepromfs_getAddress(BLOCK_SIZE_ADDR);

   //calculate selected empty block address...
   aux163=eepromfs_getBlockAddress(blockNmr);                               //data block start address + (block size * empty block finded)                                      
   //add real block size...(!16 bits should change -2 for -3)
   aux163+=aux162-2;                                           //jump to identifiers zone

   //write values...
   write_ext_eeprom(aux163++,control);    
   DEMORA_EEPROM

   write_ext_eeprom(aux163++,control2);
   DEMORA_EEPROM

   return TRUE;
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
      aux82-=8;
   }

   //get 8 block info...
   aux81=read_ext_eeprom(aux161);

   //and set pointer according to free block value
   if(bit_test(aux81,aux82)) *state=TRUE; else *state=FALSE;
   
   //now, lets read block control info...
   aux162=eepromfs_getAddress(BLOCK_SIZE_ADDR);

   //go to block data init...
   aux161=eepromfs_getBlockAddress(blockNmr);
   //add bytes to get control address(!16 bits should change -2 for -3)
   aux161+=(aux162-2);

   //read control info...
   *control=read_ext_eeprom(aux161++);
   //read control2 info...
   *control2=read_ext_eeprom(aux161++);

   return TRUE;
}
   
int16 eepromfs_freeSpace(void){
   int16 quantity,aux162;
   char blockNmr;

   //get block size...(!16 bits should change -2 for -3)
   aux162=eepromfs_getAddress(BLOCK_SIZE_ADDR);   
   aux162-=2;

   quantity=0;
   blockNmr=0;
   while(blockNmr!=EMPTY_VALUE){
      blockNmr=eepromfs_findEmptyBlock(blockNmr);
      if(blockNmr!=EMPTY_VALUE){ quantity+=aux162; blockNmr++; };
   }
   
   return quantity;
}
   
   ////int8 eepromfs_fileSetProperties(int8 file,int8 properties)
   ////int8 eepromfs_fileGetProperties(int8 file)

   //???
   //int16 eepromfs_totalSpace(void)
   //int16 eepromfs_usedSpace(void)
