/* Definitions of target machine for GNU compiler,
   for IBM RS/6000 POWER running AIX V7.1.
   Copyright (C) 2002-2018 Free Software Foundation, Inc.
   Contributed by David Edelsohn (edelsohn@gnu.org).

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

/* The macro SUBTARGET_OVERRIDE_OPTIONS is provided for subtargets, to
   get control in TARGET_OPTION_OVERRIDE.  */

#define SUBTARGET_OVERRIDE_OPTIONS					\
do {									\
  if (TARGET_64BIT && ! TARGET_POWERPC64)				\
    {									\
      rs6000_isa_flags |= OPTION_MASK_POWERPC64;			\
      warning (0, "-maix64 requires PowerPC64 architecture remain enabled"); \
    }									\
  if (TARGET_SOFT_FLOAT && TARGET_LONG_DOUBLE_128)			\
    {									\
      rs6000_long_double_type_size = 64;				\
      if (global_options_set.x_rs6000_long_double_type_size)		\
	warning (0, "soft-float and long-double-128 are incompatible");	\
    }									\
  if (TARGET_POWERPC64 && ! TARGET_64BIT)				\
    {									\
      error ("-maix64 required: 64-bit computation with 32-bit addressing not yet supported"); \
    }									\
  if ((rs6000_isa_flags_explicit					\
       & OPTION_MASK_MINIMAL_TOC) != 0)					\
    {									\
      if (global_options_set.x_rs6000_current_cmodel			\
	  && rs6000_current_cmodel != CMODEL_SMALL)			\
	error ("-mcmodel incompatible with other toc options"); 	\
      SET_CMODEL (CMODEL_SMALL);					\
    }									\
  if (rs6000_current_cmodel != CMODEL_SMALL)				\
    {									\
      TARGET_NO_FP_IN_TOC = 0;						\
      TARGET_NO_SUM_IN_TOC = 0;						\
    }									\
  if (rs6000_current_cmodel == CMODEL_MEDIUM)				\
    {									\
      rs6000_current_cmodel = CMODEL_LARGE;				\
    }									\
} while (0)

#undef ASM_SPEC
#define ASM_SPEC "-u %{maix64:-a64} %(asm_cpu)"

/* Common ASM definitions used by ASM_SPEC amongst the various targets for
   handling -mcpu=xxx switches.  There is a parallel list in driver-rs6000.c to
   provide the default assembler options if the user uses -mcpu=native, so if
   you make changes here, make them there also.  */
#undef ASM_CPU_SPEC
#define ASM_CPU_SPEC \
"%{mcpu=native: %(asm_cpu_native); \
  mcpu=power9: -mpwr9; \
  mcpu=power8: -mpwr8; \
  mcpu=power7: -mpwr7; \
  mcpu=power6x|mcpu=power6: -mpwr6; \
  mcpu=power5+: -mpwr5x; \
  mcpu=power5: -mpwr5; \
  mcpu=power4: -mpwr4; \
  mcpu=power3: -m620; \
  mcpu=powerpc: -mppc; \
  mcpu=rs64a: -mppc; \
  mcpu=603: -m603; \
  mcpu=603e: -m603; \
  mcpu=604: -m604; \
  mcpu=604e: -m604; \
  mcpu=620: -m620; \
  mcpu=630: -m620; \
  mcpu=970|mcpu=G5: -m970; \
  !mcpu*: %{mvsx: -mpwr6; \
	    maltivec: -m970; \
	    maix64|mpowerpc64: -mppc64; \
	    : %(asm_default)}; \
  :%eMissing -mcpu option in ASM_SPEC_CPU?\n} \
-many"

#undef	ASM_DEFAULT_SPEC
#define ASM_DEFAULT_SPEC "-mpwr4"

#undef TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS()     \
  do                                 \
    {                                \
      builtin_define ("_AIX43");     \
      builtin_define ("_AIX51");     \
      builtin_define ("_AIX52");     \
      builtin_define ("_AIX53");     \
      builtin_define ("_AIX61");     \
      builtin_define ("_AIX71");     \
      TARGET_OS_AIX_CPP_BUILTINS (); \
    }                                \
  while (0)

#undef CPP_SPEC
#define CPP_SPEC "%{posix: -D_POSIX_SOURCE}	\
  %{ansi: -D_ANSI_C_SOURCE}			\
  %{maix64: -D__64BIT__}			\
  %{mpe: -I%R/usr/lpp/ppe.poe/include}		\
  %{pthread: -D_THREAD_SAFE}"

/* The GNU C++ standard library requires that these macros be 
   defined.  Synchronize with libstdc++ os_defines.h.  */
#undef CPLUSPLUS_CPP_SPEC                       
#define CPLUSPLUS_CPP_SPEC			\
  "-D_ALL_SOURCE -D__COMPATMATH__		\
   %{maix64: -D__64BIT__}			\
   %{mpe: -I%R/usr/lpp/ppe.poe/include}		\
   %{pthread: -D_THREAD_SAFE}"

#define RS6000_CPU(NAME, CPU, FLAGS)
#include "rs6000-cpus.def"
#undef RS6000_CPU

#undef  TARGET_DEFAULT
#define TARGET_DEFAULT (MASK_PPC_GPOPT | MASK_PPC_GFXOPT | MASK_MFCRF)

#undef  PROCESSOR_DEFAULT
#define PROCESSOR_DEFAULT PROCESSOR_POWER7
#undef  PROCESSOR_DEFAULT64
#define PROCESSOR_DEFAULT64 PROCESSOR_POWER7

/* AIX 7.1 kernel and assembler have necessary support for Altivec and VSX.  */
#undef OS_MISSING_ALTIVEC

/* Define this macro as a C expression for the initializer of an
   array of string to tell the driver program which options are
   defaults for this target and thus do not need to be handled
   specially when using `MULTILIB_OPTIONS'.

   Do not define this macro if `MULTILIB_OPTIONS' is not defined in
   the target makefile fragment or if none of the options listed in
   `MULTILIB_OPTIONS' are set by default.  *Note Target Fragment::.  */

#undef	MULTILIB_DEFAULTS

#undef LIB_SPEC
#define LIB_SPEC "%{pg:-L%R/lib/profiled -L%R/usr/lib/profiled}\
   %{p:-L%R/lib/profiled -L%R/usr/lib/profiled}\
   %{!maix64:%{!shared:%{g*:-lg}}}\
   %{fprofile-arcs|fprofile-generate*|coverage:-lpthreads}\
   %{mpe:-L%R/usr/lpp/ppe.poe/lib -lmpi -lvtd}\
   %{pthread:-lpthreads} -lc"

#undef LINK_SPEC
#define LINK_SPEC "-bpT:0x10000000 -bpD:0x20000000 %{!r:-btextro}\
   %{static:-bnso %(link_syscalls) } %{shared:-bM:SRE %{!e:-bnoentry}}\
   %{!maix64:%{!shared:%{g*: %(link_libg) }}} %{maix64:-b64}\
   %{mpe:-binitfini:poe_remote_main}"

#undef STARTFILE_SPEC
#define STARTFILE_SPEC "%{!shared:\
   %{maix64:%{pg:gcrt0_64%O%s;:%{p:mcrt0_64%O%s;:crt0_64%O%s}};:\
     %{pthread:%{pg:gcrt0_r%O%s;:%{p:mcrt0_r%O%s;:crt0_r%O%s}};:\
       %{pg:gcrt0%O%s;:%{p:mcrt0%O%s;:crt0%O%s}}}}}\
   %{shared:crtcxa_s%O%s;:crtcxa%O%s} crtdbase%O%s"

/* AIX V5 typedefs ptrdiff_t as "long" while earlier releases used "int".  */

#undef PTRDIFF_TYPE
#define PTRDIFF_TYPE "long int"

/* Type used for wchar_t, as a string used in a declaration.  */
#undef  WCHAR_TYPE
#define WCHAR_TYPE (!TARGET_64BIT ? "short unsigned int" : "unsigned int")

/* Width of wchar_t in bits.  */
#undef  WCHAR_TYPE_SIZE
#define WCHAR_TYPE_SIZE (!TARGET_64BIT ? 16 : 32)

/* AIX 4.2 and above provides initialization and finalization function
   support from linker command line.  */
#undef HAS_INIT_SECTION
#define HAS_INIT_SECTION

#undef LD_INIT_SWITCH
#define LD_INIT_SWITCH "-binitfini"

#ifndef _AIX52
extern long long int    atoll(const char *);  
#endif

/* This target uses the aix64.opt file.  */
#define TARGET_USES_AIX64_OPT 1

/* Large TOC Support */
#ifdef HAVE_LD_LARGE_TOC
#undef TARGET_CMODEL
#define TARGET_CMODEL rs6000_current_cmodel
#define SET_CMODEL(opt) rs6000_current_cmodel = opt
#else
#define SET_CMODEL(opt) do {} while (0)
#endif

/* This target defines SUPPORTS_WEAK and TARGET_ASM_NAMED_SECTION,
   but does not have crtbegin/end.  */

#define TARGET_AIX_VERSION 71

/* AIX 7.1 supports DWARF3 debugging, but XCOFF remains the default.  */
#define DWARF2_DEBUGGING_INFO 1
#define PREFERRED_DEBUGGING_TYPE XCOFF_DEBUG
#define DEBUG_INFO_SECTION	"0x10000"
#define DEBUG_LINE_SECTION	"0x20000"
#define DEBUG_PUBNAMES_SECTION	"0x30000"
#define DEBUG_PUBTYPES_SECTION	"0x40000"
#define DEBUG_ARANGES_SECTION	"0x50000"
#define DEBUG_ABBREV_SECTION	"0x60000"
#define DEBUG_STR_SECTION	"0x70000"
#define DEBUG_RANGES_SECTION	"0x80000"
#define DEBUG_LOC_SECTION	"0x90000"
#define DEBUG_FRAME_SECTION	"0xA0000"
#define DEBUG_MACINFO_SECTION	"0xB0000"
#define DEBUG_MACRO_SECTION	"0xB0000"

