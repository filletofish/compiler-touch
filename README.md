# llvm-touch

Sample compiler frontend to generate LLVM intermediate representation for simple language.
Our languange consists of integers, mutating variables, cycles and condition statements.

### What is IR?
[Wiki article](https://en.wikipedia.org/wiki/Intermediate_representation)
Ok, then. With LLVM installed you can look at IR of C language by yourself. Just write simple C program.

Use flag `-emit-llvm` and level of optimization `-O0` (from 0 to 3):
`clang -S -emit-llvm -O0 helloworld.c`

Now `hello.ll` contains the IR.

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

#### Its Intermediate representation:

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


### Dependencies: 
* llvm 4.0
* clang

### Compiling:
```
clang++ -g -O3 main.cpp Parser.cpp Lexer.cpp Token.cpp Expressions.cpp IRGeneration.cpp  `llvm-config --cxxflags --ldflags --system-libs --libs core` -o compiler
```

### Usage:

#### Printing intermediate representation:
```
./compiler <program.txt
```

### Sources:

* [Documentation](http://llvm.org/docs/index.html)
* [LLVM Tuturial](http://llvm.org/docs/tutorial/index.html)
* [Article by IBM](https://www.ibm.com/developerworks/ru/library/os-createcompilerllvm1/)
