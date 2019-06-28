#include "cpu.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#define DATA_LEN 6

/**
 * Helper function to read a value from the specified index in RAM
 * Returns the read value
 */
unsigned char cpu_ram_read(struct cpu *cpu, unsigned char index)
{
    return cpu->ram[index];
};

/**
 * Helper function to write a value from the specified index in RAM
 */
void cpu_ram_write(struct cpu *cpu, unsigned char index, unsigned char value)
{
    return cpu->ram[index] = value;
};

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
/* 
void cpu_load(struct cpu *cpu)
{
  char data[DATA_LEN] = {
    // From print8.ls8
    0b10000010, // LDI R0,8
    0b00000000,
    0b00001000,
    0b01000111, // PRN R0
    0b00000000,
    0b00000001  // HLT
  };
  int address = 0;
  for (int i = 0; i < DATA_LEN; i++) {
    cpu->ram[address++] = data[i];
  }
  // TODO: Replace this with something less hard-coded
}*/

void cpu_load(struct cpu *cpu, char *filename)
{
    FILE *fp;
    char line[1024];

    int address = 0x00;
    fp = fopen(filename, "r");

    // Checks for file existing
    if (fp == NULL)
    {
        fprintf(stderr, "Error: Opening file - %s does not exist.\n", filename);
        // TODO: Look up exit codes
        exit(2);
    }

    while (fgets(line, 128, fp) != NULL)
    {
        char *endptr;
        unsigned char value = strtoul(line, &endptr, 2);
        if (line == endptr)
        {
            continue;
        }
        cpu->ram[address++] = value;
    }
    fclose(fp);
}

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
    switch (op)
    {
    case ALU_MUL:
        // TODO
        break;

        // TODO: implement more ALU ops
    }
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
    unsigned char operandA = 0;
    unsigned char operandB = 0;
    int running = 1; // True until we get a HLT instruction

    while (running)
    {
        // Why couldn't Beej add more comment to his code example?
        // TODO
        // 1. Get the value of the current instruction (in address PC).
        unsigned char ir = cpu->ram[cpu->pc];
        // 2. Figure out how many operands this next instruction requires
        unsigned char num_operands = ir >> 6;
        // 3. Get the appropriate value(s) of the operands following this instruction
        if (num_operands == 2)
        {
            operandA = cpu_ram_read(cpu, (cpu->pc + 1) & 0xff);
            operandB = cpu_ram_read(cpu, (cpu->pc + 2) & 0xff);
        }
        else if (num_operands == 1)
        {
            operandA = cpu_ram_read(cpu, (cpu->pc + 1) & 0xff);
        }
        else
        {
            //I don't think this is needed...
            return (1);
        }
        int cur_index = ir;

        // 4. switch() over it to decide on a course of action.
        switch (ir)
        {
        // 5. Do whatever the instruction should do according to the spec.
        case HLT:
            running = 0;
            break;

        case LDI:
            cpu->reg[operandA] = operandB;
            break;

        case PUSH:
            // Decrement stack pointer
            cpu->reg[SP]--;
            // Copy value of operand?A into stack pointer
            cpu->ram[cpu->reg[SP]] = cpu->reg[operandA];
            break;

        case POP:
            //Copy the value of  &stackpointer
            cpu->reg[operandA] = cpu->ram[cpu->reg[SP]];
            // Increment stack pointer
            cpu->reg[SP]++;
            break;

        case PRN:
            printf("%d\n", cpu->reg[operandA]);
            break;

        case MUL:
            alu(cpu, ALU_MUL, operandA, operandB);
            break;

        case ADD:
            alu(cpu, ALU_ADD, operandA, operandB);
            break;

        case CMP:
            alu(cpu, ALU_CMP, operandA, operandB);
            break;

        default:
            printf("Error: Unknown instruction %02x at %02x\n", ir, cpu->pc);
            exit(1);
        }
        // 6. Move the PC to the next instruction.
        cpu->pc += num_operands + 1;
    }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
    // TODO: Initialize the PC and other special registers
    //cpu = malloc(sizeOf(struct cpu));

    cpu->pc = 0;
    cpu->reg[7] = 0xF4;
    cpu->flag = 4;
    memset(cpu->reg, 0, sizeof(cpu->reg));
    memset(cpu->ram, 0, sizeof(cpu->ram));
}