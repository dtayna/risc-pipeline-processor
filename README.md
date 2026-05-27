### RISC Processor with Forwarding and Hazard Detection

A pipelined RISC processor implemented in SystemC featuring forwarding and hazard detection mechanisms to reduce data hazards and improve instruction execution efficiency.

**Block Diagram**
<img width="1860" height="925" alt="image" src="https://github.com/user-attachments/assets/f74e31b3-dd04-4991-b05b-1f360f10e92e" />

This project simulates a classic pipelined RISC architecture using SystemC.
The processor includes the main pipeline stages and implements techniques commonly used in modern CPUs to handle instruction dependencies and pipeline hazards.

### Features
#### 5-stage pipeline architecture
- Instruction Fetch (IF)
- Instruction Decode (ID)
- Execute (EX)
- Memory Access (MEM)
- Write Back (WB)
#### Forwarding
- Reduces unnecessary stalls caused by data hazards
#### Hazard Detection
- Detects load-use hazards
- Inserts stalls when necessary

### Technologies
- C++
- SystemC

### Requiriments
- C++17 compatible compiler
- SystemC installed
- GNU Make

### Run

``` make run```
