#ifndef  filemem_h
#define  filemem_h

//[ size | serial_number | block_size | free_block_start | free_block_end | index_block_start | index_block_end | data_block_start | data_block_end | system_id ]
//short eepromfs_format(char mem_size, int32 ser_num, int16 blck_size, char* sys_id);
short eepromfs_format(char * sys_id);
short eepromfs_fileInBounds(int8 fileNmr);
short eepromfs_fileTouch(int8 fileNmr);

//Aux
short eepromfs_fileInBounds(int8 fileNmr);
short eepromfs_fileExists(int8 fileNmr);
int8 eepromfs_findEmptyBlock(void);
short eepromfs_writeBlockIdentifiers(int8 blockNmr,int8 control,int8 control2);
#endif
