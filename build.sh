#!/usr/bin/env bash

clang++ -g -O3 main.cpp Parser/Parser.cpp Lexer/Lexer.cpp Lexer/Token.cpp Parser/Expressions.cpp ControlFlowGraph/IRStatements.cpp ControlFlowGraph/ControlFlowGraph.cpp ControlFlowGraph/BasicBlock.cpp IRGeneration/LLVMIRGenerator.cpp IRGeneration/IRGenerator.cpp IRGeneration/GraphVizPrinter.cpp  `llvm-config --cxxflags --ldflags --system-libs --libs core` -o compiler


