#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <chrono>

#include "riscv-endian.h"
#include "riscv-types.h"
#include "riscv-opcodes.h"
#include "riscv-dsm.h"
#include "riscv-dsm-table.h"


/* Spike */

#define unlikely(x) __builtin_expect(x, 0)
#define REGISTER_INSN(proc, name, match, mask) proc->register_insn(match, mask, #name);

typedef uint64_t insn_bits_t;

struct insn_desc_t
{
  insn_bits_t match;
  insn_bits_t mask;
  const char *name;
  uint64_t opcode;
};

struct processor_t
{
	static const size_t OPCODE_CACHE_SIZE = 8191;

	insn_desc_t opcode_cache[OPCODE_CACHE_SIZE];

	std::vector<insn_desc_t> instructions;

	void build_opcode_map();
	void register_base_instructions();
	void register_insn(insn_bits_t match, insn_bits_t mask, const char* name);
	insn_bits_t get_instruction(unsigned char **pc);
	insn_desc_t decode_insn_nocache(insn_bits_t bits);
	insn_desc_t decode_insn(insn_bits_t bits);
};

void processor_t::build_opcode_map()
{
	struct cmp {
		bool operator()(const insn_desc_t& lhs, const insn_desc_t& rhs) {
			if (lhs.match == rhs.match)
				return lhs.mask > rhs.mask;
			return lhs.match > rhs.match;
		}
	};
	std::sort(instructions.begin(), instructions.end(), cmp());

	for (size_t i = 0; i < OPCODE_CACHE_SIZE; i++) {
		opcode_cache[i] = { 1, 0, "unknown", instructions.size() -1 };
	}
}

void processor_t::register_base_instructions()
{
	#define DECLARE_INSN(name, match, mask) \
		[[gnu::unused]] insn_bits_t name##_match = (match), name##_mask = (mask);
	#include "encoding.h"
	#undef DECLARE_INSN

	#define DEFINE_INSN(name) \
		REGISTER_INSN(this, name, name##_match, name##_mask)
	#include "insn_list.h"
	#undef DEFINE_INSN

	register_insn(0, 0, "unknown");
	build_opcode_map();
}

void processor_t::register_insn(insn_bits_t match, insn_bits_t mask, const char* name)
{
  instructions.push_back(insn_desc_t{match, mask, name, instructions.size()});
}

insn_bits_t processor_t::get_instruction(unsigned char **pc)
{
	unsigned int inst = htole16(*(unsigned short*)*pc);
	unsigned int op1 = inst & 0b11;
	if (op1 == 3) {
		inst |= htole16(*(unsigned short*)(*pc + 2)) << 16;
		*pc += 4;
	} else {
		*pc += 2;
	}
	return inst;
}

insn_desc_t processor_t::decode_insn_nocache(insn_bits_t bits)
{
	insn_desc_t* p = &instructions[0];

	while ((bits & p->mask) != p->match)
		p++;
	insn_desc_t desc = *p;

	if (p->mask != 0 && p > &instructions[0]) {
		if (p->match != (p-1)->match && p->match != (p+1)->match) {
			// move to front of opcode list to reduce miss penalty
			while (--p >= &instructions[0])
				*(p+1) = *p;
			instructions[0] = desc;
		}
	}

	return desc;
}

insn_desc_t processor_t::decode_insn(insn_bits_t bits)
{
  // look up opcode in hash table
  size_t idx = bits % OPCODE_CACHE_SIZE;
  insn_desc_t desc = opcode_cache[idx];

  if (unlikely(bits != desc.match)) {
    // fall back to linear search
    insn_desc_t* p = &instructions[0];
    while ((bits & p->mask) != p->match)
      p++;
    desc = *p;

    if (p->mask != 0 && p > &instructions[0]) {
      if (p->match != (p-1)->match && p->match != (p+1)->match) {
        // move to front of opcode list to reduce miss penalty
        while (--p >= &instructions[0])
          *(p+1) = *p;
        instructions[0] = desc;
      }
    }

    opcode_cache[idx] = desc;
    opcode_cache[idx].match = bits;
  }

  return desc;
}

struct riscv_decode
{
	riscv_wu  op;
};

struct riscv_proc_state
{
	riscv_ptr pc;
};

void riscv_decode_instruction_switch(riscv_decode &dec, riscv_proc_state *proc)
{
	riscv_wu inst = htole16(*(riscv_hu*)proc->pc);
	riscv_wu op1 = inst & 0b11;
	if (op1 == 3) {
		inst |= htole16(*(riscv_hu*)(proc->pc + 2)) << 16;
		proc->pc += 4;
	} else {
		proc->pc += 2;
	}

	switch (((inst >> 0) & 0b11) /* inst[1:0] */) {
		case 0:
			// c.addi4spn c.fld c.lw c.flw c.fsd c.sw c.fsw
			switch (((inst >> 13) & 0b111) /* inst[15:13] */) {
				case 0: dec.op = riscv_op_c_addi4spn; break;
				case 1: dec.op = riscv_op_c_fld; break;
				case 2: dec.op = riscv_op_c_lw; break;
				case 3: dec.op = riscv_op_c_flw; break;
				case 5: dec.op = riscv_op_c_fsd; break;
				case 6: dec.op = riscv_op_c_sw; break;
				case 7: dec.op = riscv_op_c_fsw; break;
			}
			break;
		case 1:
			// c.addi c.jal c.li c.lui c.srli c.srai c.andi c.sub c.xor c.or c.and c.subw ...
			switch (((inst >> 13) & 0b111) /* inst[15:13] */) {
				case 0: dec.op = riscv_op_c_addi; break;
				case 1: dec.op = riscv_op_c_jal; break;
				case 2: dec.op = riscv_op_c_li; break;
				case 3: dec.op = riscv_op_c_lui; break;
				case 4:
					// c.srli c.srai c.andi c.sub c.xor c.or c.and c.subw c.addw
					switch (((inst >> 10) & 0b11) /* inst[11:10] */) {
						case 0: dec.op = riscv_op_c_srli; break;
						case 1: dec.op = riscv_op_c_srai; break;
						case 2: dec.op = riscv_op_c_andi; break;
						case 3:
							// c.sub c.xor c.or c.and c.subw c.addw
							switch (((inst >> 10) & 0b100) | ((inst >> 5) & 0b011) /* inst[12|6:5] */) {
								case 0: dec.op = riscv_op_c_sub; break;
								case 1: dec.op = riscv_op_c_xor; break;
								case 2: dec.op = riscv_op_c_or; break;
								case 3: dec.op = riscv_op_c_and; break;
								case 4: dec.op = riscv_op_c_subw; break;
								case 5: dec.op = riscv_op_c_addw; break;
							}
							break;
					}
					break;
				case 5: dec.op = riscv_op_c_j; break;
				case 6: dec.op = riscv_op_c_beqz; break;
				case 7: dec.op = riscv_op_c_bnez; break;
			}
			break;
		case 2:
			// c.slli c.fldsp c.lwsp c.flwsp c.mv c.add c.fsdsp c.swsp c.fswsp
			switch (((inst >> 13) & 0b111) /* inst[15:13] */) {
				case 0: dec.op = riscv_op_c_slli; break;
				case 1: dec.op = riscv_op_c_fldsp; break;
				case 2: dec.op = riscv_op_c_lwsp; break;
				case 3: dec.op = riscv_op_c_flwsp; break;
				case 4:
					// c.mv c.add
					switch (((inst >> 12) & 0b1) /* inst[12] */) {
						case 0: dec.op = riscv_op_c_mv; break;
						case 1: dec.op = riscv_op_c_add; break;
					}
					break;
				case 5: dec.op = riscv_op_c_fsdsp; break;
				case 6: dec.op = riscv_op_c_swsp; break;
				case 7: dec.op = riscv_op_c_fswsp; break;
			}
			break;
		case 3:
			// beq bne blt bge bltu bgeu jalr jal lui auipc addi slli ...
			switch (((inst >> 2) & 0b11111) /* inst[6:2] */) {
				case 0:
					// lb lh lw ld lbu lhu lwu
					switch (((inst >> 12) & 0b111) /* inst[14:12] */) {
						case 0: dec.op = riscv_op_lb; break;
						case 1: dec.op = riscv_op_lh; break;
						case 2: dec.op = riscv_op_lw; break;
						case 3: dec.op = riscv_op_ld; break;
						case 4: dec.op = riscv_op_lbu; break;
						case 5: dec.op = riscv_op_lhu; break;
						case 6: dec.op = riscv_op_lwu; break;
					}
					break;
				case 1:
					// flw fld
					switch (((inst >> 12) & 0b111) /* inst[14:12] */) {
						case 2: dec.op = riscv_op_flw; break;
						case 3: dec.op = riscv_op_fld; break;
					}
					break;
				case 3:
					// fence fence.i
					switch (((inst >> 12) & 0b111) /* inst[14:12] */) {
						case 0: dec.op = riscv_op_fence; break;
						case 1: dec.op = riscv_op_fence_i; break;
					}
					break;
				case 4:
					// addi slli slti sltiu xori srli srai ori andi
					switch (((inst >> 12) & 0b111) /* inst[14:12] */) {
						case 0: dec.op = riscv_op_addi; break;
						case 1:
							// slli
							switch (((inst >> 26) & 0b111111) /* inst[31:26] */) {
								case 0: dec.op = riscv_op_slli; break;
							}
							break;
						case 2: dec.op = riscv_op_slti; break;
						case 3: dec.op = riscv_op_sltiu; break;
						case 4: dec.op = riscv_op_xori; break;
						case 5:
							// srli srai
							switch (((inst >> 26) & 0b111111) /* inst[31:26] */) {
								case 0: dec.op = riscv_op_srli; break;
								case 16: dec.op = riscv_op_srai; break;
							}
							break;
						case 6: dec.op = riscv_op_ori; break;
						case 7: dec.op = riscv_op_andi; break;
					}
					break;
				case 5: dec.op = riscv_op_auipc; break;
				case 6:
					// addiw slliw srliw sraiw
					switch (((inst >> 12) & 0b111) /* inst[14:12] */) {
						case 0: dec.op = riscv_op_addiw; break;
						case 1:
							// slliw
							switch (((inst >> 25) & 0b1111111) /* inst[31:25] */) {
								case 0: dec.op = riscv_op_slliw; break;
							}
							break;
						case 5:
							// srliw sraiw
							switch (((inst >> 25) & 0b1111111) /* inst[31:25] */) {
								case 0: dec.op = riscv_op_srliw; break;
								case 32: dec.op = riscv_op_sraiw; break;
							}
							break;
					}
					break;
				case 8:
					// sb sh sw sd
					switch (((inst >> 12) & 0b111) /* inst[14:12] */) {
						case 0: dec.op = riscv_op_sb; break;
						case 1: dec.op = riscv_op_sh; break;
						case 2: dec.op = riscv_op_sw; break;
						case 3: dec.op = riscv_op_sd; break;
					}
					break;
				case 9:
					// fsw fsd
					switch (((inst >> 12) & 0b111) /* inst[14:12] */) {
						case 2: dec.op = riscv_op_fsw; break;
						case 3: dec.op = riscv_op_fsd; break;
					}
					break;
				case 11:
					// amoadd.w amoxor.w amoor.w amoand.w amomin.w amomax.w amominu.w amomaxu.w amoswap.w lr.w sc.w amoadd.d ...
					switch (((inst >> 24) & 0b11111000) | ((inst >> 12) & 0b00000111) /* inst[31:27|14:12] */) {
						case 2: dec.op = riscv_op_amoadd_w; break;
						case 3: dec.op = riscv_op_amoadd_d; break;
						case 10: dec.op = riscv_op_amoswap_w; break;
						case 11: dec.op = riscv_op_amoswap_d; break;
						case 18:
							// lr.w
							switch (((inst >> 20) & 0b11111) /* inst[24:20] */) {
								case 0: dec.op = riscv_op_lr_w; break;
							}
							break;
						case 19:
							// lr.d
							switch (((inst >> 20) & 0b11111) /* inst[24:20] */) {
								case 0: dec.op = riscv_op_lr_d; break;
							}
							break;
						case 26: dec.op = riscv_op_sc_w; break;
						case 27: dec.op = riscv_op_sc_d; break;
						case 34: dec.op = riscv_op_amoxor_w; break;
						case 35: dec.op = riscv_op_amoxor_d; break;
						case 66: dec.op = riscv_op_amoor_w; break;
						case 67: dec.op = riscv_op_amoor_d; break;
						case 98: dec.op = riscv_op_amoand_w; break;
						case 99: dec.op = riscv_op_amoand_d; break;
						case 130: dec.op = riscv_op_amomin_w; break;
						case 131: dec.op = riscv_op_amomin_d; break;
						case 162: dec.op = riscv_op_amomax_w; break;
						case 163: dec.op = riscv_op_amomax_d; break;
						case 194: dec.op = riscv_op_amominu_w; break;
						case 195: dec.op = riscv_op_amominu_d; break;
						case 226: dec.op = riscv_op_amomaxu_w; break;
						case 227: dec.op = riscv_op_amomaxu_d; break;
					}
					break;
				case 12:
					// add sub sll slt sltu xor srl sra or and mul mulh ...
					switch (((inst >> 22) & 0b1111111000) | ((inst >> 12) & 0b0000000111) /* inst[31:25|14:12] */) {
						case 0: dec.op = riscv_op_add; break;
						case 1: dec.op = riscv_op_sll; break;
						case 2: dec.op = riscv_op_slt; break;
						case 3: dec.op = riscv_op_sltu; break;
						case 4: dec.op = riscv_op_xor; break;
						case 5: dec.op = riscv_op_srl; break;
						case 6: dec.op = riscv_op_or; break;
						case 7: dec.op = riscv_op_and; break;
						case 8: dec.op = riscv_op_mul; break;
						case 9: dec.op = riscv_op_mulh; break;
						case 10: dec.op = riscv_op_mulhsu; break;
						case 11: dec.op = riscv_op_mulhu; break;
						case 12: dec.op = riscv_op_div; break;
						case 13: dec.op = riscv_op_divu; break;
						case 14: dec.op = riscv_op_rem; break;
						case 15: dec.op = riscv_op_remu; break;
						case 256: dec.op = riscv_op_sub; break;
						case 261: dec.op = riscv_op_sra; break;
					}
					break;
				case 13: dec.op = riscv_op_lui; break;
				case 14:
					// addw subw sllw srlw sraw mulw divw divuw remw remuw
					switch (((inst >> 22) & 0b1111111000) | ((inst >> 12) & 0b0000000111) /* inst[31:25|14:12] */) {
						case 0: dec.op = riscv_op_addw; break;
						case 1: dec.op = riscv_op_sllw; break;
						case 5: dec.op = riscv_op_srlw; break;
						case 8: dec.op = riscv_op_mulw; break;
						case 12: dec.op = riscv_op_divw; break;
						case 13: dec.op = riscv_op_divuw; break;
						case 14: dec.op = riscv_op_remw; break;
						case 15: dec.op = riscv_op_remuw; break;
						case 256: dec.op = riscv_op_subw; break;
						case 261: dec.op = riscv_op_sraw; break;
					}
					break;
				case 16:
					// fmadd.s fmadd.d
					switch (((inst >> 25) & 0b11) /* inst[26:25] */) {
						case 0: dec.op = riscv_op_fmadd_s; break;
						case 1: dec.op = riscv_op_fmadd_d; break;
					}
					break;
				case 17:
					// fmsub.s fmsub.d
					switch (((inst >> 25) & 0b11) /* inst[26:25] */) {
						case 0: dec.op = riscv_op_fmsub_s; break;
						case 1: dec.op = riscv_op_fmsub_d; break;
					}
					break;
				case 18:
					// fnmsub.s fnmsub.d
					switch (((inst >> 25) & 0b11) /* inst[26:25] */) {
						case 0: dec.op = riscv_op_fnmsub_s; break;
						case 1: dec.op = riscv_op_fnmsub_d; break;
					}
					break;
				case 19:
					// fnmadd.s fnmadd.d
					switch (((inst >> 25) & 0b11) /* inst[26:25] */) {
						case 0: dec.op = riscv_op_fnmadd_s; break;
						case 1: dec.op = riscv_op_fnmadd_d; break;
					}
					break;
				case 20:
					// fadd.s fsub.s fmul.s fdiv.s fsgnj.s fsgnjn.s fsgnjx.s fmin.s fmax.s fsqrt.s fadd.d fsub.d ...
					switch (((inst >> 25) & 0b1111111) /* inst[31:25] */) {
						case 0: dec.op = riscv_op_fadd_s; break;
						case 1: dec.op = riscv_op_fadd_d; break;
						case 4: dec.op = riscv_op_fsub_s; break;
						case 5: dec.op = riscv_op_fsub_d; break;
						case 8: dec.op = riscv_op_fmul_s; break;
						case 9: dec.op = riscv_op_fmul_d; break;
						case 12: dec.op = riscv_op_fdiv_s; break;
						case 13: dec.op = riscv_op_fdiv_d; break;
						case 16:
							// fsgnj.s fsgnjn.s fsgnjx.s
							switch (((inst >> 12) & 0b111) /* inst[14:12] */) {
								case 0: dec.op = riscv_op_fsgnj_s; break;
								case 1: dec.op = riscv_op_fsgnjn_s; break;
								case 2: dec.op = riscv_op_fsgnjx_s; break;
							}
							break;
						case 17:
							// fsgnj.d fsgnjn.d fsgnjx.d
							switch (((inst >> 12) & 0b111) /* inst[14:12] */) {
								case 0: dec.op = riscv_op_fsgnj_d; break;
								case 1: dec.op = riscv_op_fsgnjn_d; break;
								case 2: dec.op = riscv_op_fsgnjx_d; break;
							}
							break;
						case 20:
							// fmin.s fmax.s
							switch (((inst >> 12) & 0b111) /* inst[14:12] */) {
								case 0: dec.op = riscv_op_fmin_s; break;
								case 1: dec.op = riscv_op_fmax_s; break;
							}
							break;
						case 21:
							// fmin.d fmax.d
							switch (((inst >> 12) & 0b111) /* inst[14:12] */) {
								case 0: dec.op = riscv_op_fmin_d; break;
								case 1: dec.op = riscv_op_fmax_d; break;
							}
							break;
						case 32:
							// fcvt.s.d
							switch (((inst >> 20) & 0b11111) /* inst[24:20] */) {
								case 1: dec.op = riscv_op_fcvt_s_d; break;
							}
							break;
						case 33:
							// fcvt.d.s
							switch (((inst >> 20) & 0b11111) /* inst[24:20] */) {
								case 0: dec.op = riscv_op_fcvt_d_s; break;
							}
							break;
						case 44:
							// fsqrt.s
							switch (((inst >> 20) & 0b11111) /* inst[24:20] */) {
								case 0: dec.op = riscv_op_fsqrt_s; break;
							}
							break;
						case 45:
							// fsqrt.d
							switch (((inst >> 20) & 0b11111) /* inst[24:20] */) {
								case 0: dec.op = riscv_op_fsqrt_d; break;
							}
							break;
						case 80:
							// fle.s flt.s feq.s
							switch (((inst >> 12) & 0b111) /* inst[14:12] */) {
								case 0: dec.op = riscv_op_fle_s; break;
								case 1: dec.op = riscv_op_flt_s; break;
								case 2: dec.op = riscv_op_feq_s; break;
							}
							break;
						case 81:
							// fle.d flt.d feq.d
							switch (((inst >> 12) & 0b111) /* inst[14:12] */) {
								case 0: dec.op = riscv_op_fle_d; break;
								case 1: dec.op = riscv_op_flt_d; break;
								case 2: dec.op = riscv_op_feq_d; break;
							}
							break;
						case 96:
							// fcvt.w.s fcvt.wu.s fcvt.l.s fcvt.lu.s
							switch (((inst >> 20) & 0b11111) /* inst[24:20] */) {
								case 0: dec.op = riscv_op_fcvt_w_s; break;
								case 1: dec.op = riscv_op_fcvt_wu_s; break;
								case 2: dec.op = riscv_op_fcvt_l_s; break;
								case 3: dec.op = riscv_op_fcvt_lu_s; break;
							}
							break;
						case 97:
							// fcvt.w.d fcvt.wu.d fcvt.l.d fcvt.lu.d
							switch (((inst >> 20) & 0b11111) /* inst[24:20] */) {
								case 0: dec.op = riscv_op_fcvt_w_d; break;
								case 1: dec.op = riscv_op_fcvt_wu_d; break;
								case 2: dec.op = riscv_op_fcvt_l_d; break;
								case 3: dec.op = riscv_op_fcvt_lu_d; break;
							}
							break;
						case 104:
							// fcvt.s.w fcvt.s.wu fcvt.s.l fcvt.s.lu
							switch (((inst >> 20) & 0b11111) /* inst[24:20] */) {
								case 0: dec.op = riscv_op_fcvt_s_w; break;
								case 1: dec.op = riscv_op_fcvt_s_wu; break;
								case 2: dec.op = riscv_op_fcvt_s_l; break;
								case 3: dec.op = riscv_op_fcvt_s_lu; break;
							}
							break;
						case 105:
							// fcvt.d.w fcvt.d.wu fcvt.d.l fcvt.d.lu
							switch (((inst >> 20) & 0b11111) /* inst[24:20] */) {
								case 0: dec.op = riscv_op_fcvt_d_w; break;
								case 1: dec.op = riscv_op_fcvt_d_wu; break;
								case 2: dec.op = riscv_op_fcvt_d_l; break;
								case 3: dec.op = riscv_op_fcvt_d_lu; break;
							}
							break;
						case 112:
							// fmv.x.s fclass.s
							switch (((inst >> 17) & 0b11111000) | ((inst >> 12) & 0b00000111) /* inst[24:20|14:12] */) {
								case 0: dec.op = riscv_op_fmv_x_s; break;
								case 1: dec.op = riscv_op_fclass_s; break;
							}
							break;
						case 113:
							// fmv.x.d fclass.d
							switch (((inst >> 17) & 0b11111000) | ((inst >> 12) & 0b00000111) /* inst[24:20|14:12] */) {
								case 0: dec.op = riscv_op_fmv_x_d; break;
								case 1: dec.op = riscv_op_fclass_d; break;
							}
							break;
						case 120:
							// fmv.s.x
							switch (((inst >> 17) & 0b11111000) | ((inst >> 12) & 0b00000111) /* inst[24:20|14:12] */) {
								case 0: dec.op = riscv_op_fmv_s_x; break;
							}
							break;
						case 121:
							// fmv.d.x
							switch (((inst >> 17) & 0b11111000) | ((inst >> 12) & 0b00000111) /* inst[24:20|14:12] */) {
								case 0: dec.op = riscv_op_fmv_d_x; break;
							}
							break;
					}
					break;
				case 24:
					// beq bne blt bge bltu bgeu
					switch (((inst >> 12) & 0b111) /* inst[14:12] */) {
						case 0: dec.op = riscv_op_beq; break;
						case 1: dec.op = riscv_op_bne; break;
						case 4: dec.op = riscv_op_blt; break;
						case 5: dec.op = riscv_op_bge; break;
						case 6: dec.op = riscv_op_bltu; break;
						case 7: dec.op = riscv_op_bgeu; break;
					}
					break;
				case 25:
					// jalr
					switch (((inst >> 12) & 0b111) /* inst[14:12] */) {
						case 0: dec.op = riscv_op_jalr; break;
					}
					break;
				case 27: dec.op = riscv_op_jal; break;
				case 28:
					// scall sbreak sret sfence.vm wfi mrth mrts hrts csrrw csrrs csrrc csrrwi ...
					switch (((inst >> 12) & 0b111) /* inst[14:12] */) {
						case 0:
							// scall sbreak sret sfence.vm wfi mrth mrts hrts
							switch (((inst >> 15) & 0b11111111111100000) | ((inst >> 7) & 0b00000000000011111) /* inst[31:20|11:7] */) {
								case 0:
									// scall
									switch (((inst >> 15) & 0b11111) /* inst[19:15] */) {
										case 0: dec.op = riscv_op_scall; break;
									}
									break;
								case 32:
									// sbreak
									switch (((inst >> 15) & 0b11111) /* inst[19:15] */) {
										case 0: dec.op = riscv_op_sbreak; break;
									}
									break;
								case 8192:
									// sret
									switch (((inst >> 15) & 0b11111) /* inst[19:15] */) {
										case 0: dec.op = riscv_op_sret; break;
									}
									break;
								case 8224: dec.op = riscv_op_sfence_vm; break;
								case 8256:
									// wfi
									switch (((inst >> 15) & 0b11111) /* inst[19:15] */) {
										case 0: dec.op = riscv_op_wfi; break;
									}
									break;
								case 16544:
									// hrts
									switch (((inst >> 15) & 0b11111) /* inst[19:15] */) {
										case 0: dec.op = riscv_op_hrts; break;
									}
									break;
								case 24736:
									// mrts
									switch (((inst >> 15) & 0b11111) /* inst[19:15] */) {
										case 0: dec.op = riscv_op_mrts; break;
									}
									break;
								case 24768:
									// mrth
									switch (((inst >> 15) & 0b11111) /* inst[19:15] */) {
										case 0: dec.op = riscv_op_mrth; break;
									}
									break;
							}
							break;
						case 1: dec.op = riscv_op_csrrw; break;
						case 2: dec.op = riscv_op_csrrs; break;
						case 3: dec.op = riscv_op_csrrc; break;
						case 5: dec.op = riscv_op_csrrwi; break;
						case 6: dec.op = riscv_op_csrrsi; break;
						case 7: dec.op = riscv_op_csrrci; break;
					}
					break;
			}
			break;
	}
}

void riscv_decode_instruction_dsm_sw(riscv_decode &dec, riscv_proc_state *proc)
{
	const riscv_dsm_entry *pc = riscv_dsm_table;
	riscv_wu match = 0;
	riscv_wu inst = htole16(*(riscv_hu*)proc->pc);
	riscv_wu op1 = inst & 0b11;
	if (op1 == 3) {
		inst |= htole16(*(riscv_hu*)(proc->pc + 2)) << 16;
		proc->pc += 4;
	} else {
		proc->pc += 2;
	}

	while (true) {
		switch (pc->dsm_op) {
			case riscv_dsm_break:
				return;
			case riscv_dsm_table_brk:
				if (match < pc->dsm_val) {
					pc += match + 1;
					break;
				}
				return;
			case riscv_dsm_table_dfl:
				pc += (match < pc->dsm_val) ? match + 1 : pc->dsm_val;
				break;
			case riscv_dsm_match:
				pc += (match == pc->dsm_val) ? 1 : 2;
				break;
			case riscv_dsm_jump:
				match = 0;
				pc += pc->dsm_val;
				break;
			case riscv_dsm_select:
				dec.op = pc->dsm_val;
				return;
			case riscv_dsm_mask_srl0:
			case riscv_dsm_mask_srl1:
			case riscv_dsm_mask_srl2:
			case riscv_dsm_mask_srl3:
			case riscv_dsm_mask_srl4:
			case riscv_dsm_mask_srl5:
			case riscv_dsm_mask_srl6:
			case riscv_dsm_mask_srl7:
			case riscv_dsm_mask_srl8:
			case riscv_dsm_mask_srl9:
			case riscv_dsm_mask_srl10:
			case riscv_dsm_mask_srl11:
			case riscv_dsm_mask_srl12:
			case riscv_dsm_mask_srl13:
			case riscv_dsm_mask_srl14:
			case riscv_dsm_mask_srl15:
			case riscv_dsm_mask_srl16:
			case riscv_dsm_mask_srl17:
			case riscv_dsm_mask_srl18:
			case riscv_dsm_mask_srl19:
			case riscv_dsm_mask_srl20:
			case riscv_dsm_mask_srl21:
			case riscv_dsm_mask_srl22:
			case riscv_dsm_mask_srl23:
			case riscv_dsm_mask_srl24:
			case riscv_dsm_mask_srl25:
			case riscv_dsm_mask_srl26:
			case riscv_dsm_mask_srl27:
			case riscv_dsm_mask_srl28:
			case riscv_dsm_mask_srl29:
			case riscv_dsm_mask_srl30:
			case riscv_dsm_mask_srl31:
				match |= ((inst >> (pc->dsm_op - riscv_dsm_mask_srl0)) & pc->dsm_val);
				pc++;
				break;
		}
	}
}

riscv_wu decode_code_1[] = {
	0x0026c197,
	0xd7018193,
	0x800000e7,
	0x04051063,
	0x0026c517,
	0xcd050513,
	0x00000593,
	0x002a3617,
	0x14460613,
	0x40a60633,
	0x00127317,
	0x07c300e7,
	0x00250117,
	0xfd010113,
	0x00259217,
	0xc0020213,
	0x000032b7,
	0x1002b073,
	0x5180006f,
	0x00800593,
	0x02b57663,
	0x0026e597,
	0xfac58593,
	0x00351513,
	0x00b50533,
	0x00053103,
	0xfe010ee3,
	0xffffe237,
	0x00220233,
	0x00023203,
	0x7980106f,
	0x10200073,
	0xffdff06f
};

riscv_hu decode_code_2[] = {
	0x6197, 0x001d,
	0x8193, 0xd701,
	0x00e7, 0x8000,
	0xe915,
	0x6517, 0x001d,
	0x0513, 0xcd25,
	0x4581,
	0xd617, 0x0020,
	0x0613, 0x1486,
	0x8e09,
	0xc0ef, 0x527c,
	0xa117, 0x001b,
	0x0113, 0xfda1,
	0x3217, 0x001c,
	0x0213, 0xc0a2,
	0x628d,
	0xb073, 0x1002,
	0x006f, 0x3680,
	0x45a1,
	0x7263, 0x02b5,
	0x8597, 0x001d,
	0x8593, 0xfba5,
	0x050e,
	0x952e,
	0x3103, 0x0005,
	0x0ee3, 0xfe01,
	0x7279,
	0x920a,
	0x3203, 0x0002,
	0x106f, 0x1300,
	0x0073, 0x1020,
	0xbff5
};

template <const size_t count>
void decode_dsm_sw(riscv_ptr start, riscv_ptr end, const char *code)
{
	riscv_decode dec;
	riscv_proc_state proc = { 0 };
	std::chrono::time_point<std::chrono::system_clock> s1 = std::chrono::system_clock::now();
	size_t decoded = 0;
	for (size_t i = 0; i < count; i++) {
		proc.pc = (riscv_ptr)start;
		while (proc.pc < end) {
			riscv_decode_instruction_dsm_sw(dec, &proc);
			decoded++;
		}
	}
	std::chrono::time_point<std::chrono::system_clock> s2 = std::chrono::system_clock::now();
	const char* last_insn = riscv_instructions[dec.op];
	double insn_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(s2-s1).count() / (double)decoded;
	printf("%-20s %12s %12lu %12.2lf %12s\n", __func__, last_insn, decoded, insn_ns, code);
}

template <const size_t count>
void decode_switch(riscv_ptr start, riscv_ptr end, const char *code)
{
	riscv_decode dec;
	riscv_proc_state proc = { 0 };
	std::chrono::time_point<std::chrono::system_clock> s1 = std::chrono::system_clock::now();
	size_t decoded = 0;
	for (size_t i = 0; i < count; i++) {
		proc.pc = (riscv_ptr)start;
		while (proc.pc < end) {
			riscv_decode_instruction_switch(dec, &proc);
			decoded++;
		}
	}
	std::chrono::time_point<std::chrono::system_clock> s2 = std::chrono::system_clock::now();
	const char* last_insn = riscv_instructions[dec.op];
	double insn_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(s2-s1).count() / (double)decoded;
	printf("%-20s %12s %12lu %12.2lf %12s\n", __func__, last_insn, decoded, insn_ns, code);
}

template <const size_t count>
void decode_spike_nocache(riscv_ptr start, riscv_ptr end, const char *code)
{
	insn_desc_t insn;
	processor_t proc;
	unsigned char* pc;
	proc.register_base_instructions();
	std::chrono::time_point<std::chrono::system_clock> s1 = std::chrono::system_clock::now();
	size_t decoded = 0;
	for (size_t i = 0; i < count; i++) {
		pc = (unsigned char*)start;
		while (pc < end) {
			insn_bits_t bits = proc.get_instruction(&pc);
			insn = proc.decode_insn_nocache(bits);
			decoded++;
		}
	}
	std::chrono::time_point<std::chrono::system_clock> s2 = std::chrono::system_clock::now();
	const char* last_insn = insn.name;
	double insn_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(s2-s1).count() / (double)decoded;
	printf("%-20s %12s %12lu %12.2lf %12s\n", __func__, last_insn, decoded, insn_ns, code);
}

template <const size_t count>
void decode_spike_cache(riscv_ptr start, riscv_ptr end, const char *code)
{
	insn_desc_t insn;
	processor_t proc;
	unsigned char* pc;
	proc.register_base_instructions();
	std::chrono::time_point<std::chrono::system_clock> s1 = std::chrono::system_clock::now();
	size_t decoded = 0;
	for (size_t i = 0; i < count; i++) {
		pc = (unsigned char*)start;
		while (pc < end) {
			insn_bits_t bits = proc.get_instruction(&pc);
			insn = proc.decode_insn(bits);
			decoded++;
		}
	}
	std::chrono::time_point<std::chrono::system_clock> s2 = std::chrono::system_clock::now();
	const char* last_insn = insn.name;
	double insn_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(s2-s1).count() / (double)decoded;
	printf("%-20s %12s %12lu %12.2lf %12s\n", __func__, last_insn, decoded, insn_ns, code);
}

#define TEST_DECODER(decoder,code,count,type) \
	decoder<count>((riscv_ptr)code, (riscv_ptr)code + sizeof(code), type);

int main()
{
	static const ssize_t count = 1000000;

	printf("%-20s %12s %12s %12s %12s\n", "decoder", "last_insn", "insn_count", "nanoseconds", "code");

	TEST_DECODER(decode_dsm_sw,decode_code_1,count, "RV64");
	TEST_DECODER(decode_switch,decode_code_1,count, "RV64");
	TEST_DECODER(decode_spike_nocache,decode_code_1,count, "RV64");
	TEST_DECODER(decode_spike_cache,decode_code_1,count, "RV64");

	TEST_DECODER(decode_dsm_sw,decode_code_2,count, "RV64C");
	TEST_DECODER(decode_switch,decode_code_2,count, "RV64C");
	TEST_DECODER(decode_spike_nocache,decode_code_2,count, "RV64C");
	TEST_DECODER(decode_spike_cache,decode_code_2,count, "RV64C");
}
