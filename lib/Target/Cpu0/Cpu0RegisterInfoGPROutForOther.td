//===-- Cpu0RegisterInfoGPROutForOther.td - Cpu0 Register defs ----*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-----------------------------------------------------------------------------===//

//===-----------------------------------------------------------------------------===//
// Register Classes
//===-----------------------------------------------------------------------------===//

def GPROut : RegisterClass<"Cpu0", [i32], 32, (add (sub CPURegs, SW, PC, EPC))>;
