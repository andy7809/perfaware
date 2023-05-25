#include <stdio.h>
#include <string.h>
#include "sim86_shared.h"

u32 getFileSize(FILE *f) {
    fseek(f, 0L, SEEK_END);
    u32 sz = ftell(f);
    rewind(f);
    return sz;
}

void LogInstruction(char const *name, u16 previous, u16 next) {
    printf("%s:0x%x->0x%x", Sim86_RegisterNameFromOperand(&Operands[0].Register), mem[ins->Operands[0].Register.Index], mem[ins->Operands[1].Register.Index]);
}

void Mov(instruction* ins, u16* mem) {
    switch(ins->Operands[1].Type) {
     case operand_type::Operand_Register:
        printf("%s:0x%x->0x%x", Sim86_RegisterNameFromOperand(&Operands[0].Register), mem[ins->Operands[0].Register.Index], mem[ins->Operands[1].Register.Index]);
        mem[ins->Operands[0].Register.Index] = mem[ins->Operands[1].Register.Index];
        break;
     case operand_type::Operand_Immediate:
        printf("%s:0x%x->0x%x", Sim86_RegisterNameFromOperand(&Operands[0].Register), mem[ins->Operands[0].Register.Index], ins->Operands[1].Immediate.Valu);
        mem[ins->Operands[0].Register.Index] = ins->Operands[1].Immediate.Value;
        break;
     default:
        break;
    }
}


void Sub(instruction* ins, u16* mem, u16* flags, bool updateMem) {
    u16 val = 0;
    switch(ins->Operands[1].Type) {
     case operand_type::Operand_Register:
        val = mem[ins->Operands[0].Register.Index] - mem[ins->Operands[1].Register.Index];
        break;
     case operand_type::Operand_Immediate:
        val = mem[ins->Operands[0].Register.Index] - ins->Operands[1].Immediate.Value;
        break;
     default:
        break;
    }

    if (updateMem)  {
        mem[ins->Operands[0].Register.Index] = val;
    }

    u32 signMask = 0x0080;
    u32 bitvalue = val & 0x8000;
    if (bitvalue == 0) {
        *flags &= ~signMask;
    } else {
        *flags |= signMask;
    }

    u32 zeroMask = 0x0040;
    if (val != 0){
       *flags &= ~zeroMask; 
    } else {
       *flags |= zeroMask;
    }
}

void Add(instruction* ins, u16* mem, u16* flags) {
    switch(ins->Operands[1].Type) {
     case operand_type::Operand_Register:
        printf("%s:0x%x->0x%x", Sim86_RegisterNameFromOperand(&Operands[0].Register), mem[ins->Operands[0].Register.Index], mem[ins->Operands[1].Register.Index]);
        mem[ins->Operands[0].Register.Index] += mem[ins->Operands[1].Register.Index];
        break;
     case operand_type::Operand_Immediate:
        mem[ins->Operands[0].Register.Index] += ins->Operands[1].Immediate.Value;
        break;
     default:
        break;
    }
    

    u32 signMask = 0x0080;
    u32 bitvalue = mem[ins->Operands[0].Register.Index] & 0x8000;
    if (bitvalue == 0) {
        *flags &= ~signMask;
    } else {
        *flags |= signMask;
    }

    u32 zeroMask = 0x0040;
    if (mem[ins->Operands[0].Register.Index] != 0){
       *flags &= ~zeroMask; 
    } else {
       *flags |= zeroMask;
    }
}

// void Jne(instruction* ins, u16* mem, u16* flags, u16* ip) {
//     if (flags & 0x0040) {
//         *ip -= 
//     }
// }

void PrintRegisters(u16* mem) {
    for (u32 i = 1; i < 9; ++i) {
        register_access Reg {.Index=i, .Count=2, .Offset=2};
        printf("%s: %x\n", Sim86_RegisterNameFromOperand(&Reg), mem[i]);
    }
}

void PrintFlags(u16 flags) {
    printf("Flags: ");
    if (flags & 0x80) {
        printf(" SF ");
    }
    if (flags & 0x40) {
        printf(" ZF ");
    }
    printf("\n");
}

void InitMem(u16* mem) {
    for (u32 i = 1; i < 9; ++i) {
        mem[i] = 0;
    }
}

int main(void)
{
    FILE *binary;
    binary = fopen("/Users/andrewwiedenmann/code/computer_enhance/perfaware/part1/listing_0049_conditional_jumps", "rb");
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
    u16 Flags = 0;
    InitMem(Registers);
    u16 InstructionPointer = 0;
    while(InstructionPointer < sizeof(byte_arr))
    {
        instruction Decoded;
        Sim86_Decode8086Instruction(sizeof(byte_arr) - InstructionPointer, byte_arr + InstructionPointer, &Decoded);
        if(Decoded.Op)
        {
            InstructionPointer += Decoded.Size;
            switch (Decoded.Op) {
                case Op_mov:
                    Mov(&Decoded, Registers);
                    break;
                case Op_sub:
                    Sub(&Decoded, Registers, &Flags, true);
                    break;
                case Op_cmp:
                    Sub(&Decoded, Registers, &Flags, false);
                    break;
                case Op_add:
                    Add(&Decoded, Registers, &Flags);
                case Op_jne:
                    printf("jne: %x\n", Decoded.Operands[0].Immediate.Value);
                default:
                    break;
            }
            printf(" IP: %x\n", InstructionPointer);
            printf("\n");
        }
        else
        {
            printf("Unrecognized instruction\n");
            break;
        }
    }
    
    return 0;
}
