#include <stdio.h>

#include "sim86_shared.h"

u32 getFileSize(FILE *f) {
    fseek(f, 0L, SEEK_END);
    u32 sz = ftell(f);
    rewind(f);
    return sz;
}

int main(void)
{
    FILE *binary;
    binary = fopen("/Users/andrewwiedenmann/code/computer_enhance/perfaware/part1/listing_0037_single_register_mov", "rb");
    u32 size = getFileSize(binary);
    u8 byte_arr[size];
    for (int i = 0; i < size; ++i) {
        byte_arr[i] = fgetc(binary);
        printf("%x\n", byte_arr[i]);
    }
    u32 Version = Sim86_GetVersion();
    printf("Sim86 Version: %u (expected %u)\n", Version, SIM86_VERSION);
    if(Version != SIM86_VERSION)
    {
        printf("ERROR: Header file version doesn't match DLL.\n");
        return -1;
    }
    
    u16 Registers[8];
    u32 Offset = 0;
    while(Offset < sizeof(byte_arr))
    {
        instruction Decoded;
        Sim86_Decode8086Instruction(sizeof(byte_arr) - Offset, byte_arr + Offset, &Decoded);
        if(Decoded.Op)
        {
            Offset += Decoded.Size;
            printf("Size:%u Op:%s Flags:0x%x\n", Decoded.Size, Sim86_MnemonicFromOperationType(Decoded.Op), Decoded.Flags);
            printf("Operands:%s\n", (&Decoded.Operands[0].Register));
            printf("Operands:%s\n", Sim86_RegisterNameFromOperand(&Decoded.Operands[1].Register));            
        }
        else
        {
            printf("Unrecognized instruction\n");
            break;
        }
    }
    
    return 0;
}
