# Compiler-touch

Simple compiler frontend to generate intermediate representation for simple language.
The languange consists of integers, mutating variables, cycles and condition statements.

Two modes available:

  * Generating _my own_ intermediate presentation with features:
    * Building Control Flow Graph
    * Using SSA Form    
    * Using Phi Nodes
    
  * Generating LLVM intermediate presentation with power of LLVM Library.

### Example:

Here is simple code written with our language.

#### Code:
```
a = 3
b = 0

for i = 1, b in
    a = a + 3

if b then
    a = a - 2
else
    a = 0
```

#### Control flow graph:

<div id="container" align="middle" >
    <img src="http://i.imgur.com/pKox8Zt.png" alt="" height=400px>
</div>

#### Its intermediate representation (using my own statements):

```
bb #0 entry
		preds: 
		succs: bb #1 loop_cond 
		dominatedBy: 
	a_0 = 3
	b_0 = 0
	i_0 = 1
	branch to: bb #1 loop_cond

bb #1 loop_cond
		preds: bb #0 entry bb #2 loop_body 
		succs: bb #2 loop_body bb #3 loop_cont 
		dominatedBy: bb #0 entry
	i_1 = [i_0 bb #0 entry; i_2 bb #2 loop_body; ]
	a_1 = [a_0 bb #0 entry; a_2 bb #2 loop_body; ]
	branch on: b_0 - i_1 to: bb #2 loop_body or: bb #3 loop_cont

bb #2 loop_body
		preds: bb #1 loop_cond 
		succs: bb #1 loop_cond 
		dominatedBy: bb #1 loop_cond
	a_2 = a_1 + 3
	i_2 = i_1 + 1
	branch to: bb #1 loop_cond

bb #3 loop_cont
		preds: bb #1 loop_cond 
		succs: bb #5 else bb #4 then 
		dominatedBy: bb #1 loop_cond
	branch on: b_0 to: bb #5 else or: bb #4 then

bb #4 then
		preds: bb #3 loop_cont 
		succs: bb #6 if_cont 
		dominatedBy: bb #3 loop_cont
	a_3 = a_1 - 2
	branch to: bb #6 if_cont

bb #5 else
		preds: bb #3 loop_cont 
		succs: bb #6 if_cont 
		dominatedBy: bb #3 loop_cont
	a_4 = 0
	branch to: bb #6 if_cont

bb #6 if_cont
		preds: bb #4 then bb #5 else 
		succs: 
		dominatedBy: bb #3 loop_cont
	a_5 = [a_3 bb #4 then; a_4 bb #5 else; ]
```



#### Its intermediate representation produced with LLVM Library:

```
; ModuleID = 'My Module'
source_filename = "My Module"

define common i32 @main() {
entrypoint:
  %a = alloca i32
  store i32 3, i32* %a
  %b = alloca i32
  store i32 0, i32* %b
  %i = alloca i32
  store i32 1, i32* %i
  br label %loopCoonditionBB

loopCoonditionBB:                                 ; preds = %loop, %entrypoint
  %b1 = load i32, i32* %b
  %i2 = load i32, i32* %i
  %loopcond = icmp slt i32 %i2, %b1
  br i1 %loopcond, label %loop, label %afterloop

loop:                                             ; preds = %loopCoonditionBB
  %a3 = load i32, i32* %a
  %addtmp = fadd i32 %a3, 3
  store i32 %addtmp, i32* %a
  %i4 = load i32, i32* %i
  %nextvar = fadd i32 %i4, 1
  store i32 %nextvar, i32* %i
  br label %loopCoonditionBB

afterloop:                                        ; preds = %loopCoonditionBB
  %b5 = load i32, i32* %b
  %ifcond = icmp ne i32 %b5, 0
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %afterloop
  %a6 = load i32, i32* %a
  %subtmp = fsub i32 %a6, 2
  store i32 %subtmp, i32* %a
  br label %ifcont

else:                                             ; preds = %afterloop
  store i32 0, i32* %a
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %iftmp = phi i32 [ %subtmp, %then ], [ 0, %else ]
  ret i32 %iftmp
}
```

### What is IR?
[Wiki article](https://en.wikipedia.org/wiki/Intermediate_representation)

Ok, then. With LLVM installed you can look at IR of C language by yourself. Just write simple C program.

Use flag `-emit-llvm` and level of optimization `-O0` (from 0 to 3):
`clang -S -emit-llvm -O0 helloworld.c`

Now `hello.ll` contains the IR.

### Dependencies: 
* llvm 4.0
* clang

### Building:
```
./build.sh
```

### Usage:

#### Printing intermediate representation:
```
./compiler <program.txt
```
#### Use cmd argument `-gv` for printing graph viz presentation
#### Use cmd argument `-llvm` for llvm mode

### Sources:

* [Documentation](http://llvm.org/docs/index.html)
* [LLVM Tuturial](http://llvm.org/docs/tutorial/index.html)
* [Article by IBM](https://www.ibm.com/developerworks/ru/library/os-createcompilerllvm1/)
