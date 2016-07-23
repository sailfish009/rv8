//
//  test-fpu.h
//
//  DANGER - This is machine generated code
//

#ifndef test_fpu_h
#define test_fpu_h

typedef signed int         s32;
typedef unsigned int       u32;
typedef signed long long   s64;
typedef unsigned long long u64;
typedef float              f32;
typedef double             f64;

#if _RISCV_SZPTR != _RISCV_SZINT
typedef s64 sx;
typedef s64 sx;
#else
typedef s32 sx;
typedef s32 sx;
#endif

#define FPU_IDENTITY(fn, args...) printf("\tFPU_ASSERT(%s, " fmt_res_##fn ", " fmt_arg_##fn ");\n", #fn, test_##fn(args), args)
#define FPU_ASSERT(fn, result, args...) assert(test_##fn(args) == result)

#define fmt_res_fmadd_s "%ff"
#define fmt_arg_fmadd_s "%ff, %ff, %ff"
inline f32 test_fmadd_s(f32 s1, f32 s2, f32 s3)
{
	f32 d;
	asm("fmadd.s %[d], %[s1], %[s2], %[s3]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2), [s3]"f" (s3)
		:
	);
	return d;
}

#define fmt_res_fmsub_s "%ff"
#define fmt_arg_fmsub_s "%ff, %ff, %ff"
inline f32 test_fmsub_s(f32 s1, f32 s2, f32 s3)
{
	f32 d;
	asm("fmsub.s %[d], %[s1], %[s2], %[s3]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2), [s3]"f" (s3)
		:
	);
	return d;
}

#define fmt_res_fnmsub_s "%ff"
#define fmt_arg_fnmsub_s "%ff, %ff, %ff"
inline f32 test_fnmsub_s(f32 s1, f32 s2, f32 s3)
{
	f32 d;
	asm("fnmsub.s %[d], %[s1], %[s2], %[s3]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2), [s3]"f" (s3)
		:
	);
	return d;
}

#define fmt_res_fnmadd_s "%ff"
#define fmt_arg_fnmadd_s "%ff, %ff, %ff"
inline f32 test_fnmadd_s(f32 s1, f32 s2, f32 s3)
{
	f32 d;
	asm("fnmadd.s %[d], %[s1], %[s2], %[s3]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2), [s3]"f" (s3)
		:
	);
	return d;
}

#define fmt_res_fadd_s "%ff"
#define fmt_arg_fadd_s "%ff, %ff"
inline f32 test_fadd_s(f32 s1, f32 s2)
{
	f32 d;
	asm("fadd.s %[d], %[s1], %[s2]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_fsub_s "%ff"
#define fmt_arg_fsub_s "%ff, %ff"
inline f32 test_fsub_s(f32 s1, f32 s2)
{
	f32 d;
	asm("fsub.s %[d], %[s1], %[s2]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_fmul_s "%ff"
#define fmt_arg_fmul_s "%ff, %ff"
inline f32 test_fmul_s(f32 s1, f32 s2)
{
	f32 d;
	asm("fmul.s %[d], %[s1], %[s2]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_fdiv_s "%ff"
#define fmt_arg_fdiv_s "%ff, %ff"
inline f32 test_fdiv_s(f32 s1, f32 s2)
{
	f32 d;
	asm("fdiv.s %[d], %[s1], %[s2]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_fsgnj_s "%ff"
#define fmt_arg_fsgnj_s "%ff, %ff"
inline f32 test_fsgnj_s(f32 s1, f32 s2)
{
	f32 d;
	asm("fsgnj.s %[d], %[s1], %[s2]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_fsgnjn_s "%ff"
#define fmt_arg_fsgnjn_s "%ff, %ff"
inline f32 test_fsgnjn_s(f32 s1, f32 s2)
{
	f32 d;
	asm("fsgnjn.s %[d], %[s1], %[s2]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_fsgnjx_s "%ff"
#define fmt_arg_fsgnjx_s "%ff, %ff"
inline f32 test_fsgnjx_s(f32 s1, f32 s2)
{
	f32 d;
	asm("fsgnjx.s %[d], %[s1], %[s2]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_fmin_s "%ff"
#define fmt_arg_fmin_s "%ff, %ff"
inline f32 test_fmin_s(f32 s1, f32 s2)
{
	f32 d;
	asm("fmin.s %[d], %[s1], %[s2]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_fmax_s "%ff"
#define fmt_arg_fmax_s "%ff, %ff"
inline f32 test_fmax_s(f32 s1, f32 s2)
{
	f32 d;
	asm("fmax.s %[d], %[s1], %[s2]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_fsqrt_s "%ff"
#define fmt_arg_fsqrt_s "%ff"
inline f32 test_fsqrt_s(f32 s1)
{
	f32 d;
	asm("fsqrt.s %[d], %[s1]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1)
		:
	);
	return d;
}

#define fmt_res_fle_s "%lldll"
#define fmt_arg_fle_s "%ff, %ff"
inline sx test_fle_s(f32 s1, f32 s2)
{
	sx d;
	asm("fle.s %[d], %[s1], %[s2]\n"
		: [d]"=r" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_flt_s "%lldll"
#define fmt_arg_flt_s "%ff, %ff"
inline sx test_flt_s(f32 s1, f32 s2)
{
	sx d;
	asm("flt.s %[d], %[s1], %[s2]\n"
		: [d]"=r" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_feq_s "%lldll"
#define fmt_arg_feq_s "%ff, %ff"
inline sx test_feq_s(f32 s1, f32 s2)
{
	sx d;
	asm("feq.s %[d], %[s1], %[s2]\n"
		: [d]"=r" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_fcvt_w_s "%d"
#define fmt_arg_fcvt_w_s "%ff"
inline s32 test_fcvt_w_s(f32 s1)
{
	s32 d;
	asm("fcvt.w.s %[d], %[s1]\n"
		: [d]"=r" (d)
		: [s1]"f" (s1)
		:
	);
	return d;
}

#define fmt_res_fcvt_wu_s "%uu"
#define fmt_arg_fcvt_wu_s "%ff"
inline u32 test_fcvt_wu_s(f32 s1)
{
	u32 d;
	asm("fcvt.wu.s %[d], %[s1]\n"
		: [d]"=r" (d)
		: [s1]"f" (s1)
		:
	);
	return d;
}

#define fmt_res_fcvt_s_w "%ff"
#define fmt_arg_fcvt_s_w "%d"
inline f32 test_fcvt_s_w(s32 s1)
{
	f32 d;
	asm("fcvt.s.w %[d], %[s1]\n"
		: [d]"=f" (d)
		: [s1]"r" (s1)
		:
	);
	return d;
}

#define fmt_res_fcvt_s_wu "%ff"
#define fmt_arg_fcvt_s_wu "%uu"
inline f32 test_fcvt_s_wu(u32 s1)
{
	f32 d;
	asm("fcvt.s.wu %[d], %[s1]\n"
		: [d]"=f" (d)
		: [s1]"r" (s1)
		:
	);
	return d;
}

#define fmt_res_fmv_x_s "%lldll"
#define fmt_arg_fmv_x_s "%ff"
inline sx test_fmv_x_s(f32 s1)
{
	sx d;
	asm("fmv.x.s %[d], %[s1]\n"
		: [d]"=r" (d)
		: [s1]"f" (s1)
		:
	);
	return d;
}

#define fmt_res_fclass_s "%lldll"
#define fmt_arg_fclass_s "%ff"
inline sx test_fclass_s(f32 s1)
{
	sx d;
	asm("fclass.s %[d], %[s1]\n"
		: [d]"=r" (d)
		: [s1]"f" (s1)
		:
	);
	return d;
}

#define fmt_res_fmv_s_x "%ff"
#define fmt_arg_fmv_s_x "%lldll"
inline f32 test_fmv_s_x(sx s1)
{
	f32 d;
	asm("fmv.s.x %[d], %[s1]\n"
		: [d]"=f" (d)
		: [s1]"r" (s1)
		:
	);
	return d;
}

#if _RISCV_SZPTR != _RISCV_SZINT
#define fmt_res_fcvt_l_s "%lldll"
#define fmt_arg_fcvt_l_s "%ff"
inline s64 test_fcvt_l_s(f32 s1)
{
	s64 d;
	asm("fcvt.l.s %[d], %[s1]\n"
		: [d]"=r" (d)
		: [s1]"f" (s1)
		:
	);
	return d;
}
#endif

#if _RISCV_SZPTR != _RISCV_SZINT
#define fmt_res_fcvt_lu_s "%lluull"
#define fmt_arg_fcvt_lu_s "%ff"
inline u64 test_fcvt_lu_s(f32 s1)
{
	u64 d;
	asm("fcvt.lu.s %[d], %[s1]\n"
		: [d]"=r" (d)
		: [s1]"f" (s1)
		:
	);
	return d;
}
#endif

#if _RISCV_SZPTR != _RISCV_SZINT
#define fmt_res_fcvt_s_l "%ff"
#define fmt_arg_fcvt_s_l "%lldll"
inline f32 test_fcvt_s_l(s64 s1)
{
	f32 d;
	asm("fcvt.s.l %[d], %[s1]\n"
		: [d]"=f" (d)
		: [s1]"r" (s1)
		:
	);
	return d;
}
#endif

#if _RISCV_SZPTR != _RISCV_SZINT
#define fmt_res_fcvt_s_lu "%ff"
#define fmt_arg_fcvt_s_lu "%lluull"
inline f32 test_fcvt_s_lu(u64 s1)
{
	f32 d;
	asm("fcvt.s.lu %[d], %[s1]\n"
		: [d]"=f" (d)
		: [s1]"r" (s1)
		:
	);
	return d;
}
#endif

#define fmt_res_fmadd_d "%lf"
#define fmt_arg_fmadd_d "%lf, %lf, %lf"
inline f64 test_fmadd_d(f64 s1, f64 s2, f64 s3)
{
	f64 d;
	asm("fmadd.d %[d], %[s1], %[s2], %[s3]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2), [s3]"f" (s3)
		:
	);
	return d;
}

#define fmt_res_fmsub_d "%lf"
#define fmt_arg_fmsub_d "%lf, %lf, %lf"
inline f64 test_fmsub_d(f64 s1, f64 s2, f64 s3)
{
	f64 d;
	asm("fmsub.d %[d], %[s1], %[s2], %[s3]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2), [s3]"f" (s3)
		:
	);
	return d;
}

#define fmt_res_fnmsub_d "%lf"
#define fmt_arg_fnmsub_d "%lf, %lf, %lf"
inline f64 test_fnmsub_d(f64 s1, f64 s2, f64 s3)
{
	f64 d;
	asm("fnmsub.d %[d], %[s1], %[s2], %[s3]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2), [s3]"f" (s3)
		:
	);
	return d;
}

#define fmt_res_fnmadd_d "%lf"
#define fmt_arg_fnmadd_d "%lf, %lf, %lf"
inline f64 test_fnmadd_d(f64 s1, f64 s2, f64 s3)
{
	f64 d;
	asm("fnmadd.d %[d], %[s1], %[s2], %[s3]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2), [s3]"f" (s3)
		:
	);
	return d;
}

#define fmt_res_fadd_d "%lf"
#define fmt_arg_fadd_d "%lf, %lf"
inline f64 test_fadd_d(f64 s1, f64 s2)
{
	f64 d;
	asm("fadd.d %[d], %[s1], %[s2]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_fsub_d "%lf"
#define fmt_arg_fsub_d "%lf, %lf"
inline f64 test_fsub_d(f64 s1, f64 s2)
{
	f64 d;
	asm("fsub.d %[d], %[s1], %[s2]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_fmul_d "%lf"
#define fmt_arg_fmul_d "%lf, %lf"
inline f64 test_fmul_d(f64 s1, f64 s2)
{
	f64 d;
	asm("fmul.d %[d], %[s1], %[s2]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_fdiv_d "%lf"
#define fmt_arg_fdiv_d "%lf, %lf"
inline f64 test_fdiv_d(f64 s1, f64 s2)
{
	f64 d;
	asm("fdiv.d %[d], %[s1], %[s2]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_fsgnj_d "%lf"
#define fmt_arg_fsgnj_d "%lf, %lf"
inline f64 test_fsgnj_d(f64 s1, f64 s2)
{
	f64 d;
	asm("fsgnj.d %[d], %[s1], %[s2]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_fsgnjn_d "%lf"
#define fmt_arg_fsgnjn_d "%lf, %lf"
inline f64 test_fsgnjn_d(f64 s1, f64 s2)
{
	f64 d;
	asm("fsgnjn.d %[d], %[s1], %[s2]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_fsgnjx_d "%lf"
#define fmt_arg_fsgnjx_d "%lf, %lf"
inline f64 test_fsgnjx_d(f64 s1, f64 s2)
{
	f64 d;
	asm("fsgnjx.d %[d], %[s1], %[s2]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_fmin_d "%lf"
#define fmt_arg_fmin_d "%lf, %lf"
inline f64 test_fmin_d(f64 s1, f64 s2)
{
	f64 d;
	asm("fmin.d %[d], %[s1], %[s2]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_fmax_d "%lf"
#define fmt_arg_fmax_d "%lf, %lf"
inline f64 test_fmax_d(f64 s1, f64 s2)
{
	f64 d;
	asm("fmax.d %[d], %[s1], %[s2]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_fcvt_s_d "%ff"
#define fmt_arg_fcvt_s_d "%lf"
inline f32 test_fcvt_s_d(f64 s1)
{
	f32 d;
	asm("fcvt.s.d %[d], %[s1]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1)
		:
	);
	return d;
}

#define fmt_res_fcvt_d_s "%lf"
#define fmt_arg_fcvt_d_s "%ff"
inline f64 test_fcvt_d_s(f32 s1)
{
	f64 d;
	asm("fcvt.d.s %[d], %[s1]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1)
		:
	);
	return d;
}

#define fmt_res_fsqrt_d "%lf"
#define fmt_arg_fsqrt_d "%lf"
inline f64 test_fsqrt_d(f64 s1)
{
	f64 d;
	asm("fsqrt.d %[d], %[s1]\n"
		: [d]"=f" (d)
		: [s1]"f" (s1)
		:
	);
	return d;
}

#define fmt_res_fle_d "%lldll"
#define fmt_arg_fle_d "%lf, %lf"
inline sx test_fle_d(f64 s1, f64 s2)
{
	sx d;
	asm("fle.d %[d], %[s1], %[s2]\n"
		: [d]"=r" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_flt_d "%lldll"
#define fmt_arg_flt_d "%lf, %lf"
inline sx test_flt_d(f64 s1, f64 s2)
{
	sx d;
	asm("flt.d %[d], %[s1], %[s2]\n"
		: [d]"=r" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_feq_d "%lldll"
#define fmt_arg_feq_d "%lf, %lf"
inline sx test_feq_d(f64 s1, f64 s2)
{
	sx d;
	asm("feq.d %[d], %[s1], %[s2]\n"
		: [d]"=r" (d)
		: [s1]"f" (s1), [s2]"f" (s2)
		:
	);
	return d;
}

#define fmt_res_fcvt_w_d "%d"
#define fmt_arg_fcvt_w_d "%lf"
inline s32 test_fcvt_w_d(f64 s1)
{
	s32 d;
	asm("fcvt.w.d %[d], %[s1]\n"
		: [d]"=r" (d)
		: [s1]"f" (s1)
		:
	);
	return d;
}

#define fmt_res_fcvt_wu_d "%uu"
#define fmt_arg_fcvt_wu_d "%lf"
inline u32 test_fcvt_wu_d(f64 s1)
{
	u32 d;
	asm("fcvt.wu.d %[d], %[s1]\n"
		: [d]"=r" (d)
		: [s1]"f" (s1)
		:
	);
	return d;
}

#define fmt_res_fcvt_d_w "%lf"
#define fmt_arg_fcvt_d_w "%d"
inline f64 test_fcvt_d_w(s32 s1)
{
	f64 d;
	asm("fcvt.d.w %[d], %[s1]\n"
		: [d]"=f" (d)
		: [s1]"r" (s1)
		:
	);
	return d;
}

#define fmt_res_fcvt_d_wu "%lf"
#define fmt_arg_fcvt_d_wu "%uu"
inline f64 test_fcvt_d_wu(u32 s1)
{
	f64 d;
	asm("fcvt.d.wu %[d], %[s1]\n"
		: [d]"=f" (d)
		: [s1]"r" (s1)
		:
	);
	return d;
}

#define fmt_res_fclass_d "%lldll"
#define fmt_arg_fclass_d "%lf"
inline sx test_fclass_d(f64 s1)
{
	sx d;
	asm("fclass.d %[d], %[s1]\n"
		: [d]"=r" (d)
		: [s1]"f" (s1)
		:
	);
	return d;
}

#if _RISCV_SZPTR != _RISCV_SZINT
#define fmt_res_fcvt_l_d "%lldll"
#define fmt_arg_fcvt_l_d "%lf"
inline s64 test_fcvt_l_d(f64 s1)
{
	s64 d;
	asm("fcvt.l.d %[d], %[s1]\n"
		: [d]"=r" (d)
		: [s1]"f" (s1)
		:
	);
	return d;
}
#endif

#if _RISCV_SZPTR != _RISCV_SZINT
#define fmt_res_fcvt_lu_d "%lluull"
#define fmt_arg_fcvt_lu_d "%lf"
inline u64 test_fcvt_lu_d(f64 s1)
{
	u64 d;
	asm("fcvt.lu.d %[d], %[s1]\n"
		: [d]"=r" (d)
		: [s1]"f" (s1)
		:
	);
	return d;
}
#endif

#if _RISCV_SZPTR != _RISCV_SZINT
#define fmt_res_fmv_x_d "%lldll"
#define fmt_arg_fmv_x_d "%lf"
inline sx test_fmv_x_d(f64 s1)
{
	sx d;
	asm("fmv.x.d %[d], %[s1]\n"
		: [d]"=r" (d)
		: [s1]"f" (s1)
		:
	);
	return d;
}
#endif

#if _RISCV_SZPTR != _RISCV_SZINT
#define fmt_res_fcvt_d_l "%lf"
#define fmt_arg_fcvt_d_l "%lldll"
inline f64 test_fcvt_d_l(s64 s1)
{
	f64 d;
	asm("fcvt.d.l %[d], %[s1]\n"
		: [d]"=f" (d)
		: [s1]"r" (s1)
		:
	);
	return d;
}
#endif

#if _RISCV_SZPTR != _RISCV_SZINT
#define fmt_res_fcvt_d_lu "%lf"
#define fmt_arg_fcvt_d_lu "%lluull"
inline f64 test_fcvt_d_lu(u64 s1)
{
	f64 d;
	asm("fcvt.d.lu %[d], %[s1]\n"
		: [d]"=f" (d)
		: [s1]"r" (s1)
		:
	);
	return d;
}
#endif

#if _RISCV_SZPTR != _RISCV_SZINT
#define fmt_res_fmv_d_x "%lf"
#define fmt_arg_fmv_d_x "%lldll"
inline f64 test_fmv_d_x(sx s1)
{
	f64 d;
	asm("fmv.d.x %[d], %[s1]\n"
		: [d]"=f" (d)
		: [s1]"r" (s1)
		:
	);
	return d;
}
#endif

#define fmt_res_frcsr "%lldll"
#define fmt_arg_frcsr "%lldll, %lldll"
inline sx test_frcsr(sx s1, sx csr12)
{
	sx d;
	asm("frcsr %[d], %[s1], %[csr12]\n"
		: [d]"=r" (d)
		: [s1]"r" (s1), [csr12]"r" (csr12)
		:
	);
	return d;
}

#define fmt_res_frrm "%lldll"
#define fmt_arg_frrm "%lldll, %lldll"
inline sx test_frrm(sx s1, sx csr12)
{
	sx d;
	asm("frrm %[d], %[s1], %[csr12]\n"
		: [d]"=r" (d)
		: [s1]"r" (s1), [csr12]"r" (csr12)
		:
	);
	return d;
}

#define fmt_res_frflags "%lldll"
#define fmt_arg_frflags "%lldll, %lldll"
inline sx test_frflags(sx s1, sx csr12)
{
	sx d;
	asm("frflags %[d], %[s1], %[csr12]\n"
		: [d]"=r" (d)
		: [s1]"r" (s1), [csr12]"r" (csr12)
		:
	);
	return d;
}

#define fmt_res_fscsr "%lldll"
#define fmt_arg_fscsr "%lldll, %lldll"
inline sx test_fscsr(sx s1, sx csr12)
{
	sx d;
	asm("fscsr %[d], %[s1], %[csr12]\n"
		: [d]"=r" (d)
		: [s1]"r" (s1), [csr12]"r" (csr12)
		:
	);
	return d;
}

#define fmt_res_fsrm "%lldll"
#define fmt_arg_fsrm "%lldll, %lldll"
inline sx test_fsrm(sx s1, sx csr12)
{
	sx d;
	asm("fsrm %[d], %[s1], %[csr12]\n"
		: [d]"=r" (d)
		: [s1]"r" (s1), [csr12]"r" (csr12)
		:
	);
	return d;
}

#define fmt_res_fsflags "%lldll"
#define fmt_arg_fsflags "%lldll, %lldll"
inline sx test_fsflags(sx s1, sx csr12)
{
	sx d;
	asm("fsflags %[d], %[s1], %[csr12]\n"
		: [d]"=r" (d)
		: [s1]"r" (s1), [csr12]"r" (csr12)
		:
	);
	return d;
}

#endif
