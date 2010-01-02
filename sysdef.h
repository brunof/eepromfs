#ifndef  sysdef_h
#define  sysdef_h

#DEFINE  EMPTY_VALUE             0xFFFFFFFF
#DEFINE  BOF                     0x00000000
#DEFINE  EOF                     0xFFFFFFFF

#DEFINE  MAX_PAGE_SIZE           64          //max size of write page size for 24LC256. It may change in other size of memorys...

//type structure:
                  //[ size | serial_number | block_size | free_block_start | index_block_start | data_block_start | system_id ]
//Modifying this values will cause unespected behaivor
#DEFINE  MEMORY_SIZE_ADDR          0           //size_start = 1Byte
#DEFINE  SERIAL_NUMBER_ADDR        1           //serial_number_start = 4Bytes
#DEFINE  BLOCK_SIZE_ADDR           5           //block_size = 2Bytes
#DEFINE  FREE_BLOCK_START_ADDR     7           //free_block_start = 2Bytes
#DEFINE  FREE_BLOCK_END_ADDR       9           //free_block_start = 2Bytes
#DEFINE  INDEX_BLOCK_START_ADDR   11           //index_block_start = 2Bytes
#DEFINE  INDEX_BLOCK_END_ADDR     13           //index_block_start = 2Bytes
#DEFINE  DATA_BLOCK_START_ADDR    15           //data_block_start = 2Bytes
#DEFINE  DATA_BLOCK_END_ADDR      17           //data_block_start = 2Bytes
#DEFINE  SYSTEM_ID_START_ADDR     19           //system_id_start = nBytes

//You can change this values if you want/need...
//[ 32 | XXXX | 256 | 0xf4 | 0x104 | 0x200 | system_id ] 
#DEFINE  MEMORY_SIZE                32         //size = 1Byte (value: 1 = 1KByte)
#DEFINE  SERIAL_NUMBER_1           'A'         //SERIAL_NUMBER = 4 Bytes
#DEFINE  SERIAL_NUMBER_2           'A'
#DEFINE  SERIAL_NUMBER_3           '0'
#DEFINE  SERIAL_NUMBER_4           '1'
#DEFINE  BLOCK_SIZE              0x100          //block_size = 2Bytes

#DEFINE  FREE_BLOCK_START        0x071          //free_block_start = 2Bytes
#DEFINE  FREE_BLOCK_END          0x080          //free_block_end = 2Bytes

#DEFINE  INDEX_BLOCK_START       0x081          //index_block_start = 2Bytes
#DEFINE  INDEX_BLOCK_END         0x0FF           //index_block_end = 2Bytes

#DEFINE  DATA_BLOCK_START        0x100          //data_block_start = 2Bytes
#DEFINE  DATA_BLOCK_END          0x7FFF         //data_block_end = 2Bytes


#DEFINE  FILE_CREATE_IF_NOT_EXISTS     1
#DEFINE  FILE_OVERWRITE                2
#DEFINE  FILE_NO_OVERWRITE             4

#DEFINE  ERR_NO_ERRORS                 0
#DEFINE  ERR_FILE_DOESNT_EXISTS        1
#DEFINE  ERR_FILE_ALREADY_EXISTS       2
#DEFINE  ERR_FILE_IS_READ_ONLY         4
#DEFINE  ERR_TOO_BIG                   8
#DEFINE  ERR_OUT_OF_BOUNDS             16
#DEFINE  ERR_OUT_OF_SPACE              32
#DEFINE  ERR_POSITION_OUT_OF_SIZE      64

/*//AUTOMATIC CALCULATED VALUES:
#IF   (MEMORY_SIZE*1024/BLOCK_SIZE/8) % 8
   #DEFINE  FREE_BLOCK_LENGTH  (MEMORY_SIZE*1024/BLOCK_SIZE/8)+1
#ELSE
   #DEFINE  FREE_BLOCK_LENGTH  (MEMORY_SIZE*1024/BLOCK_SIZE/8)
#ENDIF
*/

#DEFINE  sysDefaultValues_len       30
const char sysDefaultValues[sysDefaultValues_len]= {
                                                      MEMORY_SIZE,
                                                      SERIAL_NUMBER_1,SERIAL_NUMBER_2,SERIAL_NUMBER_3,SERIAL_NUMBER_4,
                                                      make8(BLOCK_SIZE,1),BLOCK_SIZE,
                                                      make8(FREE_BLOCK_START,1),FREE_BLOCK_START,
                                                      make8(FREE_BLOCK_END,1),FREE_BLOCK_END,
                                                      make8(INDEX_BLOCK_START,1),INDEX_BLOCK_START,
                                                      make8(INDEX_BLOCK_END,1),INDEX_BLOCK_END,
                                                      make8(DATA_BLOCK_START,1),DATA_BLOCK_START,
                                                      make8(DATA_BLOCK_END,1),DATA_BLOCK_END,
                                                      'F','A','T','P','I','C',' ','v','1','.','0'
                                                   };

#endif
