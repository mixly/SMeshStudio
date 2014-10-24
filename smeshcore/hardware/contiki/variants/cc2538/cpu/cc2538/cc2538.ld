

































 
 
 
 
 
 
 
 
 
 

















 
 
 
   
   
 







 
 
 



 
 
 
 
 
 
 
 
 











       






 
 
 
 
 
 
 
 
 
       
 
        
        
 
 
 
 
 
 
 
 
 
 
 
 
 
 




 
 
 


 
 
 



 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
   
     
       
         
         
         
         
         
         
         
         
         
         
         
         
         
         
         
         
         
         
  
       
     
       
         
         
       
   
 
 
 
 
 








































































MEMORY
{
    FLASH (rx) : ORIGIN = 0x00200000, LENGTH = (0x00080000 - 44)
    FLASH_CCA (RX) : ORIGIN = (0x00200000 + (0x00080000 - 44)), LENGTH = 44
    SRAM (RWX) : ORIGIN = 0x20000000, LENGTH = 0x00008000
}
SECTIONS
{
    .text :
    {
        _text = .;
        KEEP(*(.vectors))
        *(.text*)
        *(.rodata*)
        _etext = .;
    } > FLASH= 0
    .socdata (NOLOAD) :
    {
        *(.udma_channel_control_table)
    } > SRAM
    .data :
    {
        _data = .;
        *(.data*)
        _edata = .;
    } > SRAM AT > FLASH
    .ARM.exidx :
    {
        *(.ARM.exidx*)
    } > FLASH
    .bss :
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > SRAM
    .flashcca :
    {
        KEEP(*(.flashcca))
    } > FLASH_CCA
}
