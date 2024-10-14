# A5BAL8P101-C6502

## Explanation of the Title

### A5: **Olivetti Audit 5**
The "A5" in the title refers to the **Olivetti Audit 5**, a 1970s calculator developed by the Italian company Olivetti. This machine inspired elements of the architecture, including parts of the instruction set, some registers, and memory management. The Audit 5 was an early device for computational tasks and its influence reflects the project's intent to evoke a retro computing style, typical of that era.

### BAL: **Basic Assembly Language**
"BAL" stands for **Basic Assembly Language**, the low-level programming language used in many computers from the 1970s. Assembly languages, including BAL, were fundamental in this period, providing a way to write programs that directly interacted with the hardware. This project draws on the simplicity and directness of assembly language to build its architecture, mirroring the limitations and possibilities of computing in the 1970s.

### 8: **8-Bit Architecture**
The number "8" represents the **8-bit architecture** that this system is based on. In the 1970s, most microprocessors were built on 8-bit architecture, meaning the processor could handle 8 bits of data at a time. This project embraces that constraint, simulating the performance and efficiency of early computers, where small data paths and limited memory sizes were the norm.

### P101: **Olivetti Programma 101**
"P101" refers to the **Olivetti Programma 101**, a landmark in computer history as one of the first desktop programmable calculators, often referred to as the first personal computer. Developed in 1965, the Programma 101 introduced many foundational ideas for computer design and programming logic. This project borrows some register structures and logic from the Programma 101, paying homage to its innovation in computing history.

### C: **Commodore**
The "C" stands for **Commodore**, the company that became synonymous with personal computing in the late 1970s and early 1980s. Commodore was the manufacturer that used the **6502 processor** in its iconic computers like the Commodore PET and Commodore 64. The inclusion of Commodore highlights the influence of these early personal computers on the project’s architecture.

### 6502: **MOS Technology 6502 Microprocessor**
The **6502** was a revolutionary **8-bit microprocessor** introduced in 1975 by **MOS Technology**, which played a pivotal role in the development of affordable personal computers. This project is inspired by the logic of the 6502 processor, particularly its low-cost design, simple architecture, and efficient instruction set, all of which made it a dominant force in early computing systems, powering devices like the Apple I, Atari systems, and Commodore products.

## Project Overview

This project includes two major components:

- **An Emulator**: The emulator simulates the architecture of the A5BAL8P101-C6502 system, allowing users to run and test code as if it were on the original hardware. This emulator faithfully models the retro, 8-bit environment, handling instructions, memory, and registers according to the project’s custom architecture.
  
- **An Assembler**: The assembler takes code written in the project-specific assembly language and translates it into machine code that can be executed by the emulator. The assembler parses the instructions, registers, and constants, converting them into the correct binary format for execution.

## Syntax

The project introduces a unique assembly syntax that mirrors the minimalist style of early computers while introducing special notations to help manage registers and constants.

- **Mnemonic (Instruction) First**: Instructions are always written first in the syntax, followed by their operands.
  
- **Registers**: Registers are referred to using the `%` symbol, which is followed by the register’s name.
  - Example: `%RA` refers to the auxillary register.

- **Constants**: When referencing a constant value, use the `$` symbol to indicate a literal numeric value.
  - Example: `$10` refers to the constant value `10`.

- **Register Count**: When using special instructions like `LRZ` or `LZ`, which reset registers, the `~` symbol is used to indicate the number of target registers.
  - Example: `~3` refers to reseting 3 registers

### Example Code:

```assembly
LI %09, $10    ; Load the constant 10 into register 9
LAX %15        ; Move the content of register 15 in the Auxillary Register (RA)
LRZ ~5         ; Resets 5 registers