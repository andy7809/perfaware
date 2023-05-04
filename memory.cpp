#include <stdio.h>
#include <string.h>
#include "sim86_shared.h"

u32 getFileSize(FILE *f) {
    fseek(f, 0L, SEEK_END);
    u32 sz = ftell(f);
    rewind(f);
    return sz;
}

void Mov(instruction* ins, u16* mem) {
    
    mem[ins->Operands[0].Register.Index] = ins->Operands[1].Immediate.Value;
}

void PrintRegisters(u16* mem) {
    for (u32 i = 1; i < 9; ++i) {
        register_access Reg {.Index=i, .Count=2, .Offset=2};
        printf("%s: %x\n", Sim86_RegisterNameFromOperand(&Reg), mem[i]);
    }
}

void InitMem(u16* mem) {
    for (u32 i = 1; i < 9; ++i) {
        mem[i] = 0;
    }
}

int main(void)
{
    FILE *binary;
    binary = fopen("/Users/andrewwiedenmann/code/computer_enhance/perfaware/part1/listing_0043_immediate_movs", "rb");
    u32 size = getFileSize(binary);
    u8 byte_arr[size];
    for (int i = 0; i < size; ++i) {
        byte_arr[i] = fgetc(binary);
    }
    u32 Version = Sim86_GetVersion();
    printf("Sim86 Version: %u (expected %u)\n", Version, SIM86_VERSION);
    if(Version != SIM86_VERSION)
    {
        printf("ERROR: Header file version doesn't match DLL.\n");
        return -1;
    }
    
    u16 Registers[9];
    InitMem(Registers);
    u32 Offset = 0;
    while(Offset < sizeof(byte_arr))
    {
        instruction Decoded;
        Sim86_Decode8086Instruction(sizeof(byte_arr) - Offset, byte_arr + Offset, &Decoded);
        if(Decoded.Op)
        {
            Offset += Decoded.Size;
            if (!strcmp(Sim86_MnemonicFromOperationType(Decoded.Op), "mov")) {
                Mov(&Decoded, Registers);
                PrintRegisters(Registers);
                printf("\n");
            }           
        }
        else
        {
            printf("Unrecognized instruction\n");
            break;
        }
    }
    
    return 0;
}
