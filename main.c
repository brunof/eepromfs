#include "main.h"
#include "filemem.c"

#OPT 9

void main()
{
   char my_id[80] = "0123456789012345678901234567890123456789012345678901234567890123456789012345678";
   char dataIn[40];
   short res;
   int8 i,j,k;

   //res=eepromfs_format(my_id);

   //eepromfs_fileTouch(0);

   eepromfs_fileWrite(eepromfs_fileSize(0),eepromfs_fileSize(0),my_id,10);   
   //printf("El tamanio del archivo %u es de: %Lu bytes.\r\n",j,eepromfs_fileSize(j));
   //printf("El espacio disponible es de: %Lu\r\n",eepromfs_freeSpace());

   eepromfs_fileRead(0,eepromfs_fileSize(0)-5,dataIn,10);
   dataIn[10]='\0';

   printf("Leido:%s\r\n",dataIn);

   //printf("HECHO!");

   while(1);
   
   /*
   for(j=0;j<5;j++){
      if(eepromfs_fileRemove(j)) printf("Archivo eliminado con exito!\r\n"); else printf("Error al intentar eliminar el archivo!\r\n");
   }
   */
}
