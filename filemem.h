#ifndef  filemem_h
#define  filemem_h

#define EEPROM_SDA  PIN_C4
#define EEPROM_SCL  PIN_C3

//[ size | serial_number | block_size | free_block_start | free_block_end | index_block_start | index_block_end | data_block_start | data_block_end | system_id ]
//short eepromfs_format(char mem_size, int32 ser_num, int16 blck_size, char* sys_id);
short eepromfs_format(char * sys_id);

#endif
