#ifndef  filemem_h
#define  filemem_h

//[ size | serial_number | block_size | free_block_start | free_block_end | index_block_start | index_block_end | data_block_start | data_block_end | system_id ]
//short eepromfs_format(char mem_size, int32 ser_num, int16 blck_size, char* sys_id); deprecated?
short eepromfs_format(char * sys_id);
short eepromfs_fileTouch(int8 fileNmr);
//short eepromfs_fileCopy(int8 fileSource,int8 fileDestiny);
short eepromfs_fileRemove(int8 fileNmr);
int16 eepromfs_fileSize(int8 fileNmr);
int16 eepromfs_freeSpace(void);
int16 eepromfs_fileWrite(int8 fileNmr,int16 startPos, char * data, int16 quantity);
int16 eepromfs_fileRead(int8 fileNmr,int16 startPos, char * dataReaded, int16 quantity);

//Aux
int16 eepromfs_getBlockAddress(int8 blockNmr);
unsigned int16 eepromfs_getAddress(unsigned int16 VarPos);
int16 eepromfs_fileCalculatePos(int8 fileNmr, int16 position, int8 * parentBlockNmr);
short eepromfs_fileInBounds(int8 fileNmr);
short eepromfs_fileExists(int8 fileNmr);
int8 eepromfs_findEmptyBlock(int8 fromBlock);
short eepromfs_setBlockIdentifiers(int8 blockNmr, short state, int8 control,int8 control2);
short eepromfs_getBlockIdentifiers(int8 blockNmr, short * state,int8 *control, int8 * control2);
#endif
