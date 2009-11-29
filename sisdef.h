#ifndef  sisdef_h
#define  sisdef_h

//type structure:
                  //[ size | serial_number | block_size | free_block_start | index_block_start | data_block_start | system_id ]
#DEFINE  MEMORY_SIZE_START    0           //size_start = 1Byte
#DEFINE  SERIAL_NUMBER_START  1           //serial_number_start = 4Bytes
#DEFINE  BLOCK_SIZE_START     5           //block_size_start = 2Bytes
#DEFINE  FREE_BLOCK_START     7           //free_block_start = 2Bytes
#DEFINE  INDEX_BLOCK_START    9           //index_block_start = 2Bytes
#DEFINE  DATA_BLOCK_START    11           //data_block_start = 2Bytes
#DEFINE  SYSTEM_ID_START     13           //system_id_start = nBytes




//[ 32 | XXXX | 256 | 0xf4 | 0x104 | 0x200 | system_id ] 
#DEFINE  MEMORY_SIZE          32             //size = 1Byte (value: 1 = 1KByte)
#DEFINE  BLOCK_SIZE          256            //block_size = 2Bytes (value: 1 = 1Byte)
#DEFINE  FREE_BLOCK        0x0F4          //free_block_start = 2Bytes
#DEFINE  INDEX_BLOCK       0x104          //index_block_start = 2Bytes
#DEFINE  DATA_BLOCK        0x200          //data_block_start = 2Bytes

#endif
