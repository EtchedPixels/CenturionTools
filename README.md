# Centurion Tools

An assembler, linker and related tools for the Warrex Centurion CPU-6.

## Basic usage

````
as [-l listfile] [-o objfile] foo.s

ld -b -C addr foo.o -o foo
````

The Warrex elements of the tool chain are new and lightly tested. The core of
the tool chain is well established and used for many other targets including
the TMS9995 and 6800 C compiler kits.

## Status

This is an initial cut at an assembler for what we know of the Warrex
Centurion CPU6 processor. The syntax is primarily taken from the EE200 but
in addition more generic operations like XFR A,B are permitted instead of
just the official XAB.

As with the EE200 the source is on the left, so XFRB AL,BL places AL into
BL, and SUB A,B subtracts A from B (and has the short form SAB).

It should pick the shortest forms for instructions when it can.

Address mode syntax

````
	nn		constant
	(nn)		address
	@(nn)		address, indirected
	(X)		register dereference
	-5(X)		register dereference with offset
	A,(-X)		register pre-decremeent
	(X+)		register post-increment
	12(-S)		register pre-decrement with offset
	4(S+)		register post-increment with offset

	JMP (PC + 6)	jump to PC + 6
	JMP @(PC + 6)	jump to (PC + 6)
````

Note that jump is a bit incoherent because JUMP actually uses the address
generator and stores the generated address into PC. We map

	JMP xxxx
	JMP (xxxx)

to one or two levels of indirection appropriately but don't currently do so
for (PC + 6) which is literally 'on 6 places' so ought to be JUMP PC + 6.
This needs some kind of sanity applying once we have an actual instruction
set.

Instructions referencing memory locations are sized by the instruction form.

## Supported Instructions

* Implict base instructions in the 0x00-0x0F range for CPU4 and CPU6
* Branch instructions for CPU4 and CPU6 (10-1F)
* Byte and word single register arithmetic operations for CPU4
* CPU6 extended word single registerforms. (most of 30-3F)
* DMA operations (2F)
* Two operation instructions for CPU4 including short forms (40-5F)
* CPU4 Instructions on the X register (60-6F except 66/67/6E/6F)
* CPU4 Jump instructions and subroutine calls (70-75/79-7D)
* CPU6 Syscall (jsys) 0x76
* CPU6 Group push/pop 7E/7F
* CPU4 load and stores (80-FF except x6 and x7 forms
* CPU6 CPY block copy A bytes from B to Y (F7)

## Not Yet Supported
* Most CPU6 MMU operations (2E)
* CPU6 Extended width maths (46)
* CPU6 Block operations (47, 67)
* Unknown operations (56, 57, 76, 86. 87, 96, 97, A6, A7, B6, B7, C6, C7, E7, F6)
* CPU6 Multiply/Divide (6E/6F)
* CPU6 Move with extended addressing using different modes to 5x (D6)
* CPU6 Store A to any register file entry ? (D7)
* CPU6 Load A from any register file entry ? (E6)

## Credits

The core of the assembler is derived from code that is

Copyright © 1977-1995 by Robert Swartz.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

This software is provided by the copyright holders and contributors "as is" and
any express or implied warranties, including, but not limited to, the implied
warranties of merchantability and fitness for a particular purpose are
disclaimed. In no event shall the copyright holder or contributors be liable for
any direct, indirect, incidental, special, exemplary, or consequential damages
(including, but not limited to, procurement of substitute goods or services;
loss of use, data, or profits; or business interruption) however caused and on
any theory of liability, whether in contract, strict liability, or tort
(including negligence or otherwise) arising in any way out of the use of this
software, even if advised of the possibility of such damage.
