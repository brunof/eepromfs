#ifndef  filemem_h
#define  filemem_h

//[ size | serial_number | block_size | free_block_start | index_block_start | data_block_start | system_id ]

short format_memory(char mem_size, int32 ser_num, int16 blck_size, char* sys_id);



#endif
