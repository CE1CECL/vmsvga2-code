/*
 *  Shaders.c
 *  VMsvga2Accel
 *
 *  Created by Zenith432 on January 9th 2011.
 *  Copyright 2011 Zenith432. All rights reserved.
 *
 */

#include "Shaders.h"

#pragma GCC visibility push(hidden)

#pragma mark -
#pragma mark diffuse
#pragma mark -

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T:ps_2_0 diffuse.txt /Fh diffuse.h
//
ps_2_0
dcl v0
mov oC0, v0

// approximately 1 instruction slot used
#endif

unsigned const g_ps20_diffuse[] =
{
	0xFFFF0200, 0x0200001F, 0x80000000, 0x900F0000,
	0x02000001, 0x800F0800, 0x90E40000, 0x0000FFFF
};

#pragma mark -
#pragma mark tex1_diffuse
#pragma mark -

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T:ps_2_0 tex1_diffuse.txt /Fh tex1_diffuse.h
//
//
// Parameters:
//
//   sampler2D s_2D;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   s_2D         s0       1
//

ps_2_0
dcl t0
dcl v0
dcl_2d s0
texldp r0, t0, s0
mul_sat r0, r0, v0
mov oC0, r0

// approximately 3 instruction slots used (1 texture, 2 arithmetic)
#endif

unsigned const g_ps20_tex1_diffuse[] =
{
	0xFFFF0200, 0x0200001F, 0x80000000, 0xB00F0000,
	0x0200001F, 0x80000000, 0x900F0000, 0x0200001F,
	0x90000000, 0xA00F0800, 0x03010042, 0x800F0000,
	0xB0E40000, 0xA0E40800, 0x03000005, 0x801F0000,
	0x80E40000, 0x90E40000, 0x02000001, 0x800F0800,
	0x80E40000, 0x0000FFFF
};

#pragma mark -
#pragma mark tex1_diffuse_specular
#pragma mark -

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T:ps_2_0 tex1_diffuse_specular.txt /Fh tex1_diffuse_specular.h
//
//
// Parameters:
//
//   sampler2D s_2D;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   s_2D         s0       1
//

ps_2_0
dcl t0
dcl v0
dcl v1.xyz
dcl_2d s0
texldp r0, t0, s0
mul_sat r0, r0, v0
add r0.xyz, r0, v1
mov oC0, r0

// approximately 4 instruction slots used (1 texture, 3 arithmetic)
#endif

unsigned const g_ps20_tex1_diffuse_specular[] =
{
	0xFFFF0200, 0x0200001F, 0x80000000, 0xB00F0000,
	0x0200001F, 0x80000000, 0x900F0000, 0x0200001F,
	0x80000000, 0x90070001, 0x0200001F, 0x90000000,
	0xA00F0800, 0x03010042, 0x800F0000, 0xB0E40000,
	0xA0E40800, 0x03000005, 0x801F0000, 0x80E40000,
	0x90E40000, 0x03000002, 0x80070000, 0x80E40000,
	0x90E40001, 0x02000001, 0x800F0800, 0x80E40000,
	0x0000FFFF
};

#pragma mark -
#pragma mark chess
#pragma mark -

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T:ps_2_0 /Fh chess.h chess.txt
//
//
// Parameters:
//
//   sampler2D s_2D;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   s_2D         s0       1
//

ps_2_0
dcl t0
dcl v0
dcl_2d s0
texldp r0, t0, s0
mul_sat r0.w, r0.w, v0.w
mov r0.xyz, v0
mov oC0, r0

// approximately 4 instruction slots used (1 texture, 3 arithmetic)
#endif

unsigned const g_ps20_chess[] =
{
	0xFFFF0200, 0x0200001F, 0x80000000, 0xB00F0000,
	0x0200001F, 0x80000000, 0x900F0000, 0x0200001F,
	0x90000000, 0xA00F0800, 0x03010042, 0x800F0000,
	0xB0E40000, 0xA0E40800, 0x03000005, 0x80180000,
	0x80FF0000, 0x90FF0000, 0x02000001, 0x80070000,
	0x90E40000, 0x02000001, 0x800F0800, 0x80E40000,
	0x0000FFFF
};

#pragma mark -
#pragma mark vlc
#pragma mark -

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T:ps_2_0 /Fh vlc.h vlc.txt
//
//
// Parameters:
//
//   sampler2D s_2D;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   s_2D         s0       1
//

ps_2_0
dcl t0
dcl v0
dcl_2d s0
texldp r0, t0, s0
mul_sat r0.xyz, r0, v0
mov r0.w, v0.w
mov oC0, r0

// approximately 4 instruction slots used (1 texture, 3 arithmetic)
#endif

unsigned const g_ps20_vlc[] =
{
	0xFFFF0200, 0x0200001F, 0x80000000, 0xB00F0000,
	0x0200001F, 0x80000000, 0x900F0000, 0x0200001F,
	0x90000000, 0xA00F0800, 0x03010042, 0x800F0000,
	0xB0E40000, 0xA0E40800, 0x03000005, 0x80170000,
	0x80E40000, 0x90E40000, 0x02000001, 0x80080000,
	0x90FF0000, 0x02000001, 0x800F0800, 0x80E40000,
	0x0000FFFF
};

#pragma mark -
#pragma mark divx
#pragma mark -

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T:ps_2_0 /Fh divx.h divx.txt
//
//
// Parameters:
//
//   float4 c0;
//   float4 c1;
//   sampler2D s0;
//   sampler2D s1;
//   sampler2D s2;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   c0           c0       1
//   c1           c1       1
//   s0           s0       1
//   s1           s1       1
//   s2           s2       1
//

ps_2_0
dcl v0
dcl t0
dcl t1
dcl t2
dcl_2d s0
dcl_2d s1
dcl_2d s2
texldp r0, t1, s1
texldp r1, t0, s0
texldp r2, t2, s2
add r0.x, r0.x, -c1.x
add r0.y, r1.x, -c0.w
mul r0.y, r0.y, c0.x
mad r0.z, r0.x, -c1.y, r0.y
mad r0.x, r0.x, c0.y, r0.y
mul_sat r1.z, r0.x, v0.z
add r0.x, r2.x, -c1.x
mad r0.z, r0.x, -c1.z, r0.z
mad r0.x, r0.x, c0.z, r0.y
mul_sat r1.x, r0.x, v0.x
mul_sat r1.y, r0.z, v0.y
mul_sat r1.w, v0.w, c1.w
mov oC0, r1

// approximately 16 instruction slots used (3 texture, 13 arithmetic)
#endif

unsigned const g_ps20_divx[] =
{
	0xFFFF0200, 0x0200001F, 0x80000000, 0x900F0000,
	0x0200001F, 0x80000000, 0xB00F0000, 0x0200001F,
	0x80000000, 0xB00F0001, 0x0200001F, 0x80000000,
	0xB00F0002, 0x0200001F, 0x90000000, 0xA00F0800,
	0x0200001F, 0x90000000, 0xA00F0801, 0x0200001F,
	0x90000000, 0xA00F0802, 0x03010042, 0x800F0000,
	0xB0E40001, 0xA0E40801, 0x03010042, 0x800F0001,
	0xB0E40000, 0xA0E40800, 0x03010042, 0x800F0002,
	0xB0E40002, 0xA0E40802, 0x03000002, 0x80010000,
	0x80000000, 0xA1000001, 0x03000002, 0x80020000,
	0x80000001, 0xA1FF0000, 0x03000005, 0x80020000,
	0x80550000, 0xA0000000, 0x04000004, 0x80040000,
	0x80000000, 0xA1550001, 0x80550000, 0x04000004,
	0x80010000, 0x80000000, 0xA0550000, 0x80550000,
	0x03000005, 0x80140001, 0x80000000, 0x90AA0000,
	0x03000002, 0x80010000, 0x80000002, 0xA1000001,
	0x04000004, 0x80040000, 0x80000000, 0xA1AA0001,
	0x80AA0000, 0x04000004, 0x80010000, 0x80000000,
	0xA0AA0000, 0x80550000, 0x03000005, 0x80110001,
	0x80000000, 0x90000000, 0x03000005, 0x80120001,
	0x80AA0000, 0x90550000, 0x03000005, 0x80180001,
	0x90FF0000, 0xA0FF0001, 0x02000001, 0x800F0800,
	0x80E40001, 0x0000FFFF
};

#pragma mark -
#pragma mark QE1
#pragma mark -

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T:ps_2_0 /Fh QE1.h QE1.txt
//
//
// Parameters:
//
//   float4 c0;
//   float4 c1;
//   float4 c2;
//   float4 c3;
//   float4 c4;
//   float4 c5;
//   sampler2D s0;
//   sampler2D s1;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   c0           c0       1
//   c1           c1       1
//   c2           c2       1
//   c3           c3       1
//   c4           c4       1
//   c5           c5       1
//   s0           s0       1
//   s1           s1       1
//

ps_2_0
dcl t0
dcl_2d s0
dcl_2d s1
texldp r0, t0, s0
mov r1.y, c1.x
mov r1.x, r0.x
mov r0.x, r0.y
mov r2.x, r0.z
mov r0.y, c1.x
mov r2.y, c1.x
texld r1, r1, s1
texld r0, r0, s1
texld r2, r2, s1
mov r2.x, r1.x
mov r2.y, r0.y
dp3 r2.w, r2, c2
add r0.x, r2.w, c2.w
mov r0.y, c5.x
dp3 r2.w, r2, c3
dp3 r0.z, r2, c4
add r1.x, r0.z, c4.w
add r2.x, r2.w, c3.w
mov r2.y, c5.x
mov r1.y, c5.x
texld r0, r0, s1
texld r2, r2, s1
texld r1, r1, s1
mov r1.x, r0.x
mov r1.y, r2.y
mov r1.w, c0.x
mov oC0, r1

// approximately 28 instruction slots used (7 texture, 21 arithmetic)
#endif

unsigned const g_ps20_QE1[] =
{
	0xFFFF0200, 0x0200001F, 0x80000000, 0xB00F0000,
	0x0200001F, 0x90000000, 0xA00F0800, 0x0200001F,
	0x90000000, 0xA00F0801, 0x03010042, 0x800F0000,
	0xB0E40000, 0xA0E40800, 0x02000001, 0x80020001,
	0xA0000001, 0x02000001, 0x80010001, 0x80000000,
	0x02000001, 0x80010000, 0x80550000, 0x02000001,
	0x80010002, 0x80AA0000, 0x02000001, 0x80020000,
	0xA0000001, 0x02000001, 0x80020002, 0xA0000001,
	0x03000042, 0x800F0001, 0x80E40001, 0xA0E40801,
	0x03000042, 0x800F0000, 0x80E40000, 0xA0E40801,
	0x03000042, 0x800F0002, 0x80E40002, 0xA0E40801,
	0x02000001, 0x80010002, 0x80000001, 0x02000001,
	0x80020002, 0x80550000, 0x03000008, 0x80080002,
	0x80E40002, 0xA0E40002, 0x03000002, 0x80010000,
	0x80FF0002, 0xA0FF0002, 0x02000001, 0x80020000,
	0xA0000005, 0x03000008, 0x80080002, 0x80E40002,
	0xA0E40003, 0x03000008, 0x80040000, 0x80E40002,
	0xA0E40004, 0x03000002, 0x80010001, 0x80AA0000,
	0xA0FF0004, 0x03000002, 0x80010002, 0x80FF0002,
	0xA0FF0003, 0x02000001, 0x80020002, 0xA0000005,
	0x02000001, 0x80020001, 0xA0000005, 0x03000042,
	0x800F0000, 0x80E40000, 0xA0E40801, 0x03000042,
	0x800F0002, 0x80E40002, 0xA0E40801, 0x03000042,
	0x800F0001, 0x80E40001, 0xA0E40801, 0x02000001,
	0x80010001, 0x80000000, 0x02000001, 0x80020001,
	0x80550002, 0x02000001, 0x80080001, 0xA0000000,
	0x02000001, 0x800F0800, 0x80E40001, 0x0000FFFF
};

#pragma mark -
#pragma mark QE2
#pragma mark -

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T:ps_2_0 /Fh QE2.h QE2.txt
//
//
// Parameters:
//
//   float4 c0;
//   sampler2D s0;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   c0           c0       1
//   s0           s0       1
//

ps_2_0
dcl t1.xy
dcl t2.xy
dcl t3.xy
dcl t4.xy
dcl t5.xy
dcl t6.xy
dcl t7.xy
dcl_2d s0
texld r0, t3, s0
texld r1, t5, s0
texld r2, t4, s0
texld r3, t2, s0
texld r4, t6, s0
texld r5, t1, s0
texld r6, t7, s0
add r0, r0, r1
mul r0, r0, c0.y
mad r0, r2, c0.x, r0
add r1, r3, r4
mad r0, c0.z, r1, r0
add r1, r5, r6
mad r0, c0.w, r1, r0
mov oC0, r0

// approximately 15 instruction slots used (7 texture, 8 arithmetic)
#endif

unsigned const g_ps20_QE2[] =
{
	0xFFFF0200, 0x0200001F, 0x80000000, 0xB0030001,
	0x0200001F, 0x80000000, 0xB0030002, 0x0200001F,
	0x80000000, 0xB0030003, 0x0200001F, 0x80000000,
	0xB0030004, 0x0200001F, 0x80000000, 0xB0030005,
	0x0200001F, 0x80000000, 0xB0030006, 0x0200001F,
	0x80000000, 0xB0030007, 0x0200001F, 0x90000000,
	0xA00F0800, 0x03000042, 0x800F0000, 0xB0E40003,
	0xA0E40800, 0x03000042, 0x800F0001, 0xB0E40005,
	0xA0E40800, 0x03000042, 0x800F0002, 0xB0E40004,
	0xA0E40800, 0x03000042, 0x800F0003, 0xB0E40002,
	0xA0E40800, 0x03000042, 0x800F0004, 0xB0E40006,
	0xA0E40800, 0x03000042, 0x800F0005, 0xB0E40001,
	0xA0E40800, 0x03000042, 0x800F0006, 0xB0E40007,
	0xA0E40800, 0x03000002, 0x800F0000, 0x80E40000,
	0x80E40001, 0x03000005, 0x800F0000, 0x80E40000,
	0xA0550000, 0x04000004, 0x800F0000, 0x80E40002,
	0xA0000000, 0x80E40000, 0x03000002, 0x800F0001,
	0x80E40003, 0x80E40004, 0x04000004, 0x800F0000,
	0xA0AA0000, 0x80E40001, 0x80E40000, 0x03000002,
	0x800F0001, 0x80E40005, 0x80E40006, 0x04000004,
	0x800F0000, 0xA0FF0000, 0x80E40001, 0x80E40000,
	0x02000001, 0x800F0800, 0x80E40000, 0x0000FFFF
};

#pragma mark -
#pragma mark QE3
#pragma mark -

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T:ps_2_0 /FhQE3.h QE3.txt
//
//
// Parameters:
//
//   float4 c0;
//   float4 c1;
//   float4 c2;
//   float4 c3;
//   float4 c4;
//   float4 c5;
//   sampler2D s0;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   c0           c0       1
//   c1           c1       1
//   c2           c2       1
//   c3           c3       1
//   c4           c4       1
//   c5           c5       1
//   s0           s0       1
//

ps_2_0
dcl t0.xyz
dcl t1.xy
dcl t2.xy
dcl t3.xy
dcl t4.xy
dcl t5.xy
dcl t6.xy
dcl t7.xy
dcl_2d s0
texld r0, t4, s0
texld r1, t7, s0
texld r2, t5, s0
texld r3, t6, s0
texld r4, t3, s0
texld r5, t2, s0
texld r6, t1, s0
add r0, r0, r1
mul r0, r0, c4.y
add r1, r2, r3
mad r0, r1, c4.x, r0
add r1.xyz, t0, c3
dp3 r1.w, r1, c1
dp3 r1.x, r1, c2
add r1.y, r1.x, c2.w
add r1.x, r1.w, c1.w
add r2.x, t0.x, c3.z
add r2.y, t0.y, c3.w
add r2.z, t0.z, c3.x
dp3 r2.w, r2, c1
dp3 r1.z, r2, c2
add r2.y, r1.z, c2.w
add r2.x, r2.w, c1.w
add r3.xyz, t0, c0
dp3 r3.w, r3, c1
dp3 r1.z, r3, c2
add r3.y, r1.z, c2.w
add r3.x, r3.w, c1.w
texld r1, r1, s0
texld r2, r2, s0
texld r3, r3, s0
add r1, r1, r4
mad r0, c4.z, r1, r0
add r1, r2, r5
mad r0, c4.w, r1, r0
add r1, r3, r6
mad r0, c5.x, r1, r0
mov oC0, r0

// approximately 38 instruction slots used (10 texture, 28 arithmetic)
#endif

unsigned const g_ps20_QE3[] =
{
	0xFFFF0200, 0x0200001F, 0x80000000, 0xB0070000,
	0x0200001F, 0x80000000, 0xB0030001, 0x0200001F,
	0x80000000, 0xB0030002, 0x0200001F, 0x80000000,
	0xB0030003, 0x0200001F, 0x80000000, 0xB0030004,
	0x0200001F, 0x80000000, 0xB0030005, 0x0200001F,
	0x80000000, 0xB0030006, 0x0200001F, 0x80000000,
	0xB0030007, 0x0200001F, 0x90000000, 0xA00F0800,
	0x03000042, 0x800F0000, 0xB0E40004, 0xA0E40800,
	0x03000042, 0x800F0001, 0xB0E40007, 0xA0E40800,
	0x03000042, 0x800F0002, 0xB0E40005, 0xA0E40800,
	0x03000042, 0x800F0003, 0xB0E40006, 0xA0E40800,
	0x03000042, 0x800F0004, 0xB0E40003, 0xA0E40800,
	0x03000042, 0x800F0005, 0xB0E40002, 0xA0E40800,
	0x03000042, 0x800F0006, 0xB0E40001, 0xA0E40800,
	0x03000002, 0x800F0000, 0x80E40000, 0x80E40001,
	0x03000005, 0x800F0000, 0x80E40000, 0xA0550004,
	0x03000002, 0x800F0001, 0x80E40002, 0x80E40003,
	0x04000004, 0x800F0000, 0x80E40001, 0xA0000004,
	0x80E40000, 0x03000002, 0x80070001, 0xB0E40000,
	0xA0E40003, 0x03000008, 0x80080001, 0x80E40001,
	0xA0E40001, 0x03000008, 0x80010001, 0x80E40001,
	0xA0E40002, 0x03000002, 0x80020001, 0x80000001,
	0xA0FF0002, 0x03000002, 0x80010001, 0x80FF0001,
	0xA0FF0001, 0x03000002, 0x80010002, 0xB0000000,
	0xA0AA0003, 0x03000002, 0x80020002, 0xB0550000,
	0xA0FF0003, 0x03000002, 0x80040002, 0xB0AA0000,
	0xA0000003, 0x03000008, 0x80080002, 0x80E40002,
	0xA0E40001, 0x03000008, 0x80040001, 0x80E40002,
	0xA0E40002, 0x03000002, 0x80020002, 0x80AA0001,
	0xA0FF0002, 0x03000002, 0x80010002, 0x80FF0002,
	0xA0FF0001, 0x03000002, 0x80070003, 0xB0E40000,
	0xA0E40000, 0x03000008, 0x80080003, 0x80E40003,
	0xA0E40001, 0x03000008, 0x80040001, 0x80E40003,
	0xA0E40002, 0x03000002, 0x80020003, 0x80AA0001,
	0xA0FF0002, 0x03000002, 0x80010003, 0x80FF0003,
	0xA0FF0001, 0x03000042, 0x800F0001, 0x80E40001,
	0xA0E40800, 0x03000042, 0x800F0002, 0x80E40002,
	0xA0E40800, 0x03000042, 0x800F0003, 0x80E40003,
	0xA0E40800, 0x03000002, 0x800F0001, 0x80E40001,
	0x80E40004, 0x04000004, 0x800F0000, 0xA0AA0004,
	0x80E40001, 0x80E40000, 0x03000002, 0x800F0001,
	0x80E40002, 0x80E40005, 0x04000004, 0x800F0000,
	0xA0FF0004, 0x80E40001, 0x80E40000, 0x03000002,
	0x800F0001, 0x80E40003, 0x80E40006, 0x04000004,
	0x800F0000, 0xA0000005, 0x80E40001, 0x80E40000,
	0x02000001, 0x800F0800, 0x80E40000, 0x0000FFFF
};

#pragma mark -
#pragma mark QE4
#pragma mark -

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T:ps_2_0 /FhQE4.h QE4.txt
//
//
// Parameters:
//
//   float4 c0;
//   sampler2D s0;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   c0           c0       1
//   s0           s0       1
//

ps_2_0
dcl t0
dcl_2d s0
texldp r0, t0, s0
mov r0.w, c0.x
mov oC0, r0

// approximately 3 instruction slots used (1 texture, 2 arithmetic)
#endif

unsigned const g_ps20_QE4[] =
{
	0xFFFF0200, 0x0200001F, 0x80000000, 0xB00F0000,
	0x0200001F, 0x90000000, 0xA00F0800, 0x03010042,
	0x800F0000, 0xB0E40000, 0xA0E40800, 0x02000001,
	0x80080000, 0xA0000000, 0x02000001, 0x800F0800,
	0x80E40000, 0x0000FFFF
};

#pragma mark -
#pragma mark QE5
#pragma mark -

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T:ps_2_0 /FhQE5.h QE5.txt
//
//
// Parameters:
//
//   sampler2D s0;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   s0           s0       1
//

ps_2_0
dcl t1.xy
dcl_2d s0
texld r0, t1, s0
mov oC0, r0

// approximately 2 instruction slots used (1 texture, 1 arithmetic)
#endif

unsigned const g_ps20_QE5[] =
{
	0xFFFF0200, 0x0200001F, 0x80000000, 0xB0030001,
	0x0200001F, 0x90000000, 0xA00F0800, 0x03000042,
	0x800F0000, 0xB0E40001, 0xA0E40800, 0x02000001,
	0x800F0800, 0x80E40000, 0x0000FFFF
};

#pragma mark -
#pragma mark QE6
#pragma mark -

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T:ps_2_0 /FhQE6.h QE6.txt
//
//
// Parameters:
//
//   sampler2D s0;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   s0           s0       1
//

ps_2_0
dcl t0
dcl_2d s0
texldp r0, t0, s0
mov oC0, r0

// approximately 2 instruction slots used (1 texture, 1 arithmetic)
#endif

unsigned const g_ps20_QE6[] =
{
	0xFFFF0200, 0x0200001F, 0x80000000, 0xB00F0000,
	0x0200001F, 0x90000000, 0xA00F0800, 0x03010042,
	0x800F0000, 0xB0E40000, 0xA0E40800, 0x02000001,
	0x800F0800, 0x80E40000, 0x0000FFFF
};

#pragma mark -
#pragma mark QE7
#pragma mark -

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T:ps_2_0 /FhQE7.h QE7.txt
//
//
// Parameters:
//
//   float4 c0;
//   sampler2D s0;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   c0           c0       1
//   s0           s0       1
//

ps_2_0
dcl t1
dcl t2
dcl t3
dcl t4
dcl_2d s0
texldp r0, t1, s0
texldp r1, t2, s0
texldp r2, t3, s0
texldp r3, t4, s0
add r0, r0, r1
add r0, r2, r0
add r0, r3, r0
mul r0, r0, c0.x
mov oC0, r0

// approximately 9 instruction slots used (4 texture, 5 arithmetic)
#endif

unsigned const g_ps20_QE7[] =
{
	0xFFFF0200, 0x0200001F, 0x80000000, 0xB00F0001,
	0x0200001F, 0x80000000, 0xB00F0002, 0x0200001F,
	0x80000000, 0xB00F0003, 0x0200001F, 0x80000000,
	0xB00F0004, 0x0200001F, 0x90000000, 0xA00F0800,
	0x03010042, 0x800F0000, 0xB0E40001, 0xA0E40800,
	0x03010042, 0x800F0001, 0xB0E40002, 0xA0E40800,
	0x03010042, 0x800F0002, 0xB0E40003, 0xA0E40800,
	0x03010042, 0x800F0003, 0xB0E40004, 0xA0E40800,
	0x03000002, 0x800F0000, 0x80E40000, 0x80E40001,
	0x03000002, 0x800F0000, 0x80E40002, 0x80E40000,
	0x03000002, 0x800F0000, 0x80E40003, 0x80E40000,
	0x03000005, 0x800F0000, 0x80E40000, 0xA0000000,
	0x02000001, 0x800F0800, 0x80E40000, 0x0000FFFF
};

#pragma mark -
#pragma mark QE8
#pragma mark -

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T:ps_2_0 /FhQE8.h QE8.txt
//
//
// Parameters:
//
//   float4 c0;
//   sampler2D s0;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   c0           c0       1
//   s0           s0       1
//

ps_2_0
dcl t1.xy
dcl t2.xy
dcl t5.xy
dcl t6.xy
dcl_2d s0
texld r0, t1, s0
texld r1, t6, s0
texld r2, t2, s0
texld r3, t5, s0
add r0, r0, r1
add r1, r2, r3
mad r0, c0.z, r0, r1
mov oC0, r0

// approximately 8 instruction slots used (4 texture, 4 arithmetic)
#endif

unsigned const g_ps20_QE8[] =
{
	0xFFFF0200, 0x0200001F, 0x80000000, 0xB0030001,
	0x0200001F, 0x80000000, 0xB0030002, 0x0200001F,
	0x80000000, 0xB0030005, 0x0200001F, 0x80000000,
	0xB0030006, 0x0200001F, 0x90000000, 0xA00F0800,
	0x03000042, 0x800F0000, 0xB0E40001, 0xA0E40800,
	0x03000042, 0x800F0001, 0xB0E40006, 0xA0E40800,
	0x03000042, 0x800F0002, 0xB0E40002, 0xA0E40800,
	0x03000042, 0x800F0003, 0xB0E40005, 0xA0E40800,
	0x03000002, 0x800F0000, 0x80E40000, 0x80E40001,
	0x03000002, 0x800F0001, 0x80E40002, 0x80E40003,
	0x04000004, 0x800F0000, 0xA0AA0000, 0x80E40000,
	0x80E40001, 0x02000001, 0x800F0800, 0x80E40000,
	0x0000FFFF
};

#pragma mark -
#pragma mark QE9
#pragma mark -

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T:ps_2_0 /FhQE9.h QE9.txt
//
//
// Parameters:
//
//   sampler2D s0;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   s0           s0       1
//

ps_2_0
dcl t0
dcl v0
dcl_2d s0
texldp r0, t0, s0
mul r0, r0, v0
mov oC0, r0

// approximately 3 instruction slots used (1 texture, 2 arithmetic)
#endif

unsigned const g_ps20_QE9[] =
{
	0xFFFF0200, 0x0200001F, 0x80000000, 0xB00F0000,
	0x0200001F, 0x80000000, 0x900F0000, 0x0200001F,
	0x90000000, 0xA00F0800, 0x03010042, 0x800F0000,
	0xB0E40000, 0xA0E40800, 0x03000005, 0x800F0000,
	0x80E40000, 0x90E40000, 0x02000001, 0x800F0800,
	0x80E40000, 0x0000FFFF
};

#pragma mark -
#pragma mark QE10
#pragma mark -

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T:ps_2_0 /Fh QE10.h QE10.txt
//
//
// Parameters:
//
//   float4 c0;
//   float4 c1;
//   float4 c2;
//   float4 c3;
//   float4 c4;
//   float4 c5;
//   float4 c6;
//   float4 c7;
//   float4 c8;
//   float4 c9;
//   sampler2D s0;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   c0           c0       1
//   c1           c1       1
//   c2           c2       1
//   c3           c3       1
//   c4           c4       1
//   c5           c5       1
//   c6           c6       1
//   c7           c7       1
//   c8           c8       1
//   c9           c9       1
//   s0           s0       1
//

ps_2_0
dcl t1.xyz
dcl_2d s0
frc r0.w, t1.x
add r0.x, -r0.w, t1.x
add r1.z, -r0.w, c6.x
mov r0.yz, t1
add r2.xyz, r0, c3
dp3 r0.w, r2, c1
dp3 r2.x, r2, c2
add r2.y, r2.x, c2.w
add r2.x, r0.w, c1.w
add r3.xyz, r0, c4
dp3 r0.w, r3, c1
dp3 r2.z, r3, c2
add r3.y, r2.z, c2.w
add r3.x, r0.w, c1.w
add r4.xyz, r0, c5
add r0.xyz, r0, c0
dp3 r0.w, r4, c1
dp3 r2.z, r4, c2
add r4.y, r2.z, c2.w
add r4.x, r0.w, c1.w
dp3 r0.w, r0, c1
dp3 r0.x, r0, c2
add r0.y, r0.x, c2.w
add r0.x, r0.w, c1.w
texld r2, r2, s0
texld r3, r3, s0
texld r4, r4, s0
texld r0, r0, s0
mul r1.y, r1.z, r1.z
mul r1.x, r1.z, r1.y
mov r1.w, c7.w
dp4 r5.w, r1, c9
dp4 r1.y, r1, c8
mul r2, r2, r5.w
mad r2, r3, r1.y, r2
mul r1.z, r1.x, c6.y
mad r1.x, r1.x, -c6.y, c6.x
add r1.x, -r1.y, r1.x
add r1.x, -r5.w, r1.x
mad r2, r4, r1.z, r2
mad r0, r0, r1.x, r2
mov oC0, r0

// approximately 42 instruction slots used (4 texture, 38 arithmetic)
#endif

unsigned const g_ps20_QE10[] =
{
	0xFFFF0200, 0x0200001F, 0x80000000, 0xB0070001,
	0x0200001F, 0x90000000, 0xA00F0800, 0x02000013,
	0x80080000, 0xB0000001, 0x03000002, 0x80010000,
	0x81FF0000, 0xB0000001, 0x03000002, 0x80040001,
	0x81FF0000, 0xA0000006, 0x02000001, 0x80060000,
	0xB0E40001, 0x03000002, 0x80070002, 0x80E40000,
	0xA0E40003, 0x03000008, 0x80080000, 0x80E40002,
	0xA0E40001, 0x03000008, 0x80010002, 0x80E40002,
	0xA0E40002, 0x03000002, 0x80020002, 0x80000002,
	0xA0FF0002, 0x03000002, 0x80010002, 0x80FF0000,
	0xA0FF0001, 0x03000002, 0x80070003, 0x80E40000,
	0xA0E40004, 0x03000008, 0x80080000, 0x80E40003,
	0xA0E40001, 0x03000008, 0x80040002, 0x80E40003,
	0xA0E40002, 0x03000002, 0x80020003, 0x80AA0002,
	0xA0FF0002, 0x03000002, 0x80010003, 0x80FF0000,
	0xA0FF0001, 0x03000002, 0x80070004, 0x80E40000,
	0xA0E40005, 0x03000002, 0x80070000, 0x80E40000,
	0xA0E40000, 0x03000008, 0x80080000, 0x80E40004,
	0xA0E40001, 0x03000008, 0x80040002, 0x80E40004,
	0xA0E40002, 0x03000002, 0x80020004, 0x80AA0002,
	0xA0FF0002, 0x03000002, 0x80010004, 0x80FF0000,
	0xA0FF0001, 0x03000008, 0x80080000, 0x80E40000,
	0xA0E40001, 0x03000008, 0x80010000, 0x80E40000,
	0xA0E40002, 0x03000002, 0x80020000, 0x80000000,
	0xA0FF0002, 0x03000002, 0x80010000, 0x80FF0000,
	0xA0FF0001, 0x03000042, 0x800F0002, 0x80E40002,
	0xA0E40800, 0x03000042, 0x800F0003, 0x80E40003,
	0xA0E40800, 0x03000042, 0x800F0004, 0x80E40004,
	0xA0E40800, 0x03000042, 0x800F0000, 0x80E40000,
	0xA0E40800, 0x03000005, 0x80020001, 0x80AA0001,
	0x80AA0001, 0x03000005, 0x80010001, 0x80AA0001,
	0x80550001, 0x02000001, 0x80080001, 0xA0FF0007,
	0x03000009, 0x80080005, 0x80E40001, 0xA0E40009,
	0x03000009, 0x80020001, 0x80E40001, 0xA0E40008,
	0x03000005, 0x800F0002, 0x80E40002, 0x80FF0005,
	0x04000004, 0x800F0002, 0x80E40003, 0x80550001,
	0x80E40002, 0x03000005, 0x80040001, 0x80000001,
	0xA0550006, 0x04000004, 0x80010001, 0x80000001,
	0xA1550006, 0xA0000006, 0x03000002, 0x80010001,
	0x81550001, 0x80000001, 0x03000002, 0x80010001,
	0x81FF0005, 0x80000001, 0x04000004, 0x800F0002,
	0x80E40004, 0x80AA0001, 0x80E40002, 0x04000004,
	0x800F0000, 0x80E40000, 0x80000001, 0x80E40002,
	0x02000001, 0x800F0800, 0x80E40000, 0x0000FFFF
};

#pragma mark -
#pragma mark QE11
#pragma mark -

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T:ps_2_0 /FoQE11.bin /FhQE11.h /Vng_ps20_QE11 QE11.txt
//
//
// Parameters:
//
//   float4 c1;
//   sampler2D s0;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   c1           c1       1
//   s0           s0       1
//

ps_2_0
dcl t0
dcl t1.xy
dcl_2d s0
texld r0, t1, s0
mul r0.w, r0.w, t0.w
mul r0.xyz, t0, c1
mov oC0, r0

// approximately 4 instruction slots used (1 texture, 3 arithmetic)
#endif

unsigned const g_ps20_QE11[] =
{
	0xFFFF0200, 0x0200001F, 0x80000000, 0xB00F0000,
	0x0200001F, 0x80000000, 0xB0030001, 0x0200001F,
	0x90000000, 0xA00F0800, 0x03000042, 0x800F0000,
	0xB0E40001, 0xA0E40800, 0x03000005, 0x80080000,
	0x80FF0000, 0xB0FF0000, 0x03000005, 0x80070000,
	0xB0E40000, 0xA0E40001, 0x02000001, 0x800F0800,
	0x80E40000, 0x0000FFFF
};

#pragma mark -
#pragma mark FR1
#pragma mark -

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T:ps_2_0 /FhFR1.h FR1.txt
//
ps_2_0
dcl t0
mov oC0, t0

// approximately 1 instruction slot used
#endif

unsigned const g_ps20_FR1[] =
{
	0xFFFF0200, 0x0200001F, 0x80000000, 0xB00F0000,
	0x02000001, 0x800F0800, 0xB0E40000, 0x0000FFFF
};

#pragma mark -
#pragma mark FR2
#pragma mark -

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T:ps_2_0 /FhFR2.h FR2.txt
//
//
// Parameters:
//
//   sampler2D s0;
//   sampler2D s1;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   s0           s0       1
//   s1           s1       1
//

ps_2_0
dcl t0
dcl t1.xy
dcl t2.xy
dcl_2d s0
dcl_2d s1
texld r0, t1, s0
texld r1, t2, s1
mul r0, r0, t0
mul r0, r1.w, r0
mov oC0, r0

// approximately 5 instruction slots used (2 texture, 3 arithmetic)
#endif

unsigned const g_ps20_FR2[] =
{
	0xFFFF0200, 0x0200001F, 0x80000000, 0xB00F0000,
	0x0200001F, 0x80000000, 0xB0030001, 0x0200001F,
	0x80000000, 0xB0030002, 0x0200001F, 0x90000000,
	0xA00F0800, 0x0200001F, 0x90000000, 0xA00F0801,
	0x03000042, 0x800F0000, 0xB0E40001, 0xA0E40800,
	0x03000042, 0x800F0001, 0xB0E40002, 0xA0E40801,
	0x03000005, 0x800F0000, 0x80E40000, 0xB0E40000,
	0x03000005, 0x800F0000, 0x80FF0001, 0x80E40000,
	0x02000001, 0x800F0800, 0x80E40000, 0x0000FFFF
};

#pragma mark -
#pragma mark FR3
#pragma mark -

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T:ps_2_0 /FhFR3.h FR3.txt
//
//
// Parameters:
//
//   sampler2D s0;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   s0           s0       1
//

ps_2_0
dcl t0.xy
dcl_2d s0
texld r0, t0, s0
mov oC0, r0

// approximately 2 instruction slots used (1 texture, 1 arithmetic)
#endif

unsigned const g_ps20_FR3[] =
{
	0xFFFF0200, 0x0200001F, 0x80000000, 0xB0030000,
	0x0200001F, 0x90000000, 0xA00F0800, 0x03000042,
	0x800F0000, 0xB0E40000, 0xA0E40800, 0x02000001,
	0x800F0800, 0x80E40000, 0x0000FFFF
};

#pragma mark -
#pragma mark FR4
#pragma mark -

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T:ps_2_0 /FhFR4.h FR4.txt
//
//
// Parameters:
//
//   sampler2D s0;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   s0           s0       1
//

ps_2_0
dcl t0
dcl t1.xy
dcl_2d s0
texld r0, t1, s0
mul r0, r0, t0
mov oC0, r0

// approximately 3 instruction slots used (1 texture, 2 arithmetic)
#endif

unsigned const g_ps20_FR4[] =
{
	0xFFFF0200, 0x0200001F, 0x80000000, 0xB00F0000,
	0x0200001F, 0x80000000, 0xB0030001, 0x0200001F,
	0x90000000, 0xA00F0800, 0x03000042, 0x800F0000,
	0xB0E40001, 0xA0E40800, 0x03000005, 0x800F0000,
	0x80E40000, 0xB0E40000, 0x02000001, 0x800F0800,
	0x80E40000, 0x0000FFFF
};

#pragma mark -
#pragma mark red
#pragma mark -

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T:ps_2_0 /Fhred.h red.txt
//
ps_2_0
def c0, 1, 0, 0, 1
mov oC0, c0

// approximately 1 instruction slot used
#endif

unsigned const g_ps20_red[] =
{
	0xFFFF0200, 0x05000051, 0xA00F0000, 0x3F800000,
	0x00000000, 0x00000000, 0x3F800000, 0x02000001,
	0x800F0800, 0xA0E40000, 0x0000FFFF
};

#pragma mark -
#pragma mark debug
#pragma mark -

#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T:ps_2_0 /Fhdebug.h debug.txt
//
//
// Parameters:
//
//   sampler2D s0;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   s0           s0       1
//

ps_2_0
def c0, 1, 0, 0, 0
dcl t0
dcl_2d s0
texldp r0, t0, s0
mov r0.w, c0.x
mov oC0, r0

// approximately 3 instruction slots used (1 texture, 2 arithmetic)
#endif

unsigned const g_ps20_debug[] =
{
	0xFFFF0200, 0x05000051, 0xA00F0000, 0x3F800000,
	0x00000000, 0x00000000, 0x00000000, 0x0200001F,
	0x80000000, 0xB00F0000, 0x0200001F, 0x90000000,
	0xA00F0800, 0x03010042, 0x800F0000, 0xB0E40000,
	0xA0E40800, 0x02000001, 0x80080000, 0xA0000000,
	0x02000001, 0x800F0800, 0x80E40000, 0x0000FFFF
};

#pragma mark -
#pragma mark Summary
#pragma mark -

struct FixedShaderDescriptor const g_fixed_shaders[] =
{
	{
		{ 0x385544c556f818c0ULL, 0x76cbffeec186dc1aULL },
		&g_ps20_diffuse[0],
		sizeof g_ps20_diffuse,
		0xFFFFFFFFU,
		0U
	},
	{
		{ 0x378e32c0b2333c3fULL, 0x8e1a2beb36cfad85ULL },
		&g_ps20_tex1_diffuse[0],
		sizeof g_ps20_tex1_diffuse,
		0xFFFFFFF0U,
		1U
	},
	{
		{ 0x4ef6e59e6c7e3edbULL, 0x2427f962846d9bb4ULL },
		&g_ps20_tex1_diffuse_specular[0],
		sizeof g_ps20_tex1_diffuse_specular,
		0xFFFFFFF0U,
		1U
	},
	{
		{ 0x7921856996d25590ULL, 0x08344bdbe6b0423aULL },
		&g_ps20_chess[0],
		sizeof g_ps20_chess,
		0xFFFFFFF0U,
		1U
	},
	{
		{ 0xd8420fec285af329ULL, 0xfcb7f914aee4e615ULL },
		&g_ps20_vlc[0],
		sizeof g_ps20_vlc,
		0xFFFFFFF0U,
		1U
	},
	{
		{ 0xa0550f0964fb51f0ULL, 0xe47de607d1573594ULL },
		&g_ps20_divx[0],
		sizeof g_ps20_divx,
		0xFFFFF210U,
		7U
	},
	{
		{ 0x108dea138e2def4aULL, 0x7c37ed0f7881c58aULL },
		&g_ps20_FR1[0],
		sizeof g_ps20_FR1,
		0xFFFFFFFFU,
		0U
	},
	{
		{ 0x92b03f111abfcbf1ULL, 0x71ddaf2c5c7d76d8ULL },
		&g_ps20_FR2[0],
		sizeof g_ps20_FR2,
		0xFFFFF10FU,
		6U
	},
	{
		{ 0x0775457e6d3446b6ULL, 0x3bc4b59c1a86f59fULL },
		&g_ps20_FR3[0],
		sizeof g_ps20_FR3,
		0xFFFFFFF0U,
		1U
	},
	{
		{ 0x90639fecc437119bULL, 0x8f9e18b41311f978ULL },
		&g_ps20_FR4[0],
		sizeof g_ps20_FR4,
		0xFFFFFF0FU,
		2U
	},
	{
		{ 0x34d277542392f353ULL, 0x7e7dac7802609d0aULL },
		&g_ps20_QE1[0],
		sizeof g_ps20_QE1,
		0xFFFFFFF0U,
		1U
	},
	{
		{ 0x82a331a772da312eULL, 0x325912431c283058ULL },
		&g_ps20_QE2[0],
		sizeof g_ps20_QE2,
		0x0000000FU,
		254U
	},
	{
		{ 0xdc65a6daa2791397ULL, 0x2ccda5e131a56479ULL },
		&g_ps20_QE3[0],
		sizeof g_ps20_QE3,
		0x00000000U,
		255U
	},
	{
		{ 0x580d311c3c82b481ULL, 0x727f4681dd9bee63ULL },
		&g_ps20_QE4[0],
		sizeof g_ps20_QE4,
		0xFFFFFFF0U,
		1U
	},
	{
		{ 0xf9a60ab41ec3d374ULL, 0x9e5d1d796d836586ULL },
		&g_ps20_QE5[0],
		sizeof g_ps20_QE5,
		0xFFFFFF0FU,
		2U
	},
	{
		{ 0xe8ea0a3f3c341fe3ULL, 0x27fb4e46e5d44e5eULL },
		&g_ps20_QE6[0],
		sizeof g_ps20_QE6,
		0xFFFFFFF0U,
		1U
	},
	{
		{ 0x315b4daabc47d864ULL, 0x6e5a96b5088108c5ULL },
		&g_ps20_QE7[0],
		sizeof g_ps20_QE7,
		0xFFF0000FU,
		30U
	},
	{
		{ 0x36bb496ef8735f38ULL, 0x613569fb640edb82ULL },
		&g_ps20_QE8[0],
		sizeof g_ps20_QE8,
		0xF000000FU,
		126U
	},
	{
		{ 0x7a08f2eb33e9ab68ULL, 0x3f727ba32a7765fbULL },
		&g_ps20_QE9[0],
		sizeof g_ps20_QE9,
		0xFFFFFFF0U,
		1U
	},
	{ // Note: next same function as QE6
		{ 0x8f2e529bb202979eULL, 0xa1cf08e032e22424ULL },
		&g_ps20_QE6[0],
		sizeof g_ps20_QE6,
		0xFFFFFFF0U,
		1U
	},
	{ // Note: s0 uses non-normalized coordinates for this and it's not handled
		{ 0xac611dbc572f3614ULL, 0x53969198994417e3ULL },
		&g_ps20_QE10[0],
		sizeof g_ps20_QE10,
		0xFFFFFFFFU,
		0U
	},
	{
		{ 0xbb3a2191f2e8dc2eULL, 0x10b8ffcca70bcce4ULL },
		&g_ps20_QE11[0],
		sizeof g_ps20_QE11,
		0xFFFFFF0FU,
		2U
	}
};

#pragma GCC visibility pop