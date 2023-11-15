<p align="center">
<img src="img\c2python.png" alt="c2python" width="300" />
</p>

# C2Python

Authors: *Domenico de Gioia*, *Ivan Maddalena*

Professor: *Floriano Scioscia*


This project is a simple implementation of a source-to-source compiler from C to Python for the course *Formal Languages and Compiler* at Polytechnic University of Bari.

The main goal of this project was to create a software that can translate source code written in the C programming language into equivalent Python code. This transpiler was designed to ease the migration of existing C applications to the Python development environment by providing an efficient way to convert code without having to completely rewrite it. The aim of this work is to make a one-way translation between two high-level programming languages to simulate the behavior of a compiler. The compiler was built in C language with the help of the automatic tools *Flex* scanner generator and the *Bison* parser generator.

## Table of contents
- [C2Python](#c2python)
  - [Table of contents](#table-of-contents)
  - [C restriction](#c-restriction)
  - [Project structure](#project-structure)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
  - [Usage](#usage)
  - [Tests](#tests)
  - [Contact](#contact)


## C restriction
Our transpiler works only on a subset of the C language. The restriction of source language, agreed for our project, is composed by the following:
- **Comments**: single-line, multi-line;
- **Data types**: `int`, `float`, `char*`;
- Single and multiple **declarations** of **variables**;
- Single **declarations** of **arrays**;
- **Arithmetic operators**: addition (`+`), subtraction (`-`), multiplication (`*`), division (`/`);
- **Comparison operators**: greater than (`>`), greater than or equal (`>=`), less than (`<`), less than or equal (`<=`), equal (`==`), not equal (`!=`);
- **Logical operators**: and (`&&`), or (`||`), not (`!`);
- **Reference operator** (`&`);
- **Assignment operator** (`=`);
- **Expressions**: the use of logical, arithmetic and comparison operators is allowed, and they can include also variables, array elements, and function calls;
- **Assignment** of an expression value to a variable or an array element;
- **Branching**: `if`-`else` statement;
- **Loop/iteration**: `for` statement;
- **Function**: declarations, calls, `return` statement;;
- **Input**: input from the user with built-in function `scanf`;
- **Output**: standard output in console with built-in function `printf`.

## Project structure
The project consists of several files, listed below:
- `global.h`: global variables and function declarations used in multiple files;
- `scanner.l`: Flex input file containing specifications and rules;
- `parser.y`: Bison input file containing specifications and rules;
- `ast.h` and `ast.c`: definitions of node structures of the Abstract Syntax Tree and functions for their creation and modification;
- `symbol_table.h` and `symbol_table.c`: definitions of Symbol Table structures and functions for table and symbol management;
- `semantic.h` and `semantic.c`: function definitions for semantic checks;
- `translate.h` and `translate.c`: function definitions for code generation;
- `uthash.h`: support library for hash table implementation.

## Prerequisites

- `flex` (2.6.4 and up)
- `bison` (3.8.2 and up)
- `gcc` (11.4.0 and up)
- `make` (4.3 and up)

## Installation

To create the executable:

```bash
git clone https://github.com/domenicodegioia/transpiler_C2Python.git

cd transpiler_C2Python

make
```

## Usage

For basic usage:

```bash
./compiler {input_file.c}
```
For a complete documentation, see [here](https://github.com/domenicodegioia/transpiler_C2Python/blob/989d0a2073361dc01c1e3128667494981c2e7922/C_to_Python_transpiler.pdf).

## Tests

Each subfolder refers to a different kind of instruction to test (e.g. if, functions, classes etc.) and contains some `.c` files for testing the functioning of the transpiler.

## Contact

- [Domenico de Gioia](mailto:d.degioia1@studenti.poliba.it)
- [Ivan Maddalena](mailto:i.maddalena1@studenti.poliba.it)