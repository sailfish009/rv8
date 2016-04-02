# RISC-V Meta Compiler

A meta compiler that generates RISC-V instruction decoders, disassembler
meta data, opcode maps and documentation from instruction set meta data.

Note: the riscv-isa-sim decoder is included in the decoder
benchmark with and without caching enabled. The meta compiler
switch decoder performance should be compared to spike_nocache
for a fair comparison. Caching will be added to the meta
compiler decoder.

This is the starting point of a RISC-V binary translation investigation.

![ASCII ISA map screenshot](https://raw.githubusercontent.com/michaeljclark/riscv-mc/master/screenshot.png)

## Performance Statistics

```
decoder                 last_insn   insn_count  nanoseconds         code
decode_switch                 jal     33000000        11.36         RV64
decode_spike_nocache          jal     33000000        23.46         RV64
decode_spike_cache            jal     33000000         3.69         RV64
decode_switch                 c.j     32000000        10.14        RV64C
decode_spike_nocache          c_j     32000000        33.45        RV64C
decode_spike_cache            c_j     32000000         3.34        RV64C
```

## Build Instructions

```
make
```

To run the decoder benchmarks:
```
make bench
```

To print a colour opcode map:
```
make map
```

To output a LaTeX opcode table:
```
make latex
```

To print a static switch decoder:
```
make switch
```

To print decoder meta data table:
```
make meta
```

To print decoder opcode enum:
```
make enum
```
