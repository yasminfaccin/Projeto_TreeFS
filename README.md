# Projeto de Implementação de um Sistema de Arquivos hierárquico chamado TreeFS

O **projeto-base** utilizado neste trabalho está disponível em: `https://github.com/VielF/BasicMicrokernel`

## Descrição
Este projeto consiste na implementação de um **sistema de arquivos hierárquico**, denominado **TreeFS**, desenvolvido sobre um microkernel para arquitetura **RISC-V**.

O objetivo é implementar os principais mecanismos de um sistema de arquivos inspirado nos sistemas Unix, utilizando diretórios, inodes e gerenciamento de blocos.


## Disciplina:
Sistemas Operacionais

## Acadêmicas:
- Beatriz Pimentel Bagesteiro Alves
- Maria Eduarda Santos
- Yasmin Tarnovski Faccin

## Tecnologias
- Linguagem C
- Assembly RISC-V
- QEMU (emulação do hardware)
- GCC RISC-V `riscv64-unknown-elf-gcc`
- GitHub Codespaces / Linux

## Requisitos de Execução
O projeto deve ser executado em ambiente **Linux** ou **GitHub Codespaces**.

**Dependências necessárias:**
- gcc-riscv64-unknown-elf
- binutils-riscv64-unknown-elf
- qemu-system-riscv64
- make
- gdb-multiarch

Obs: Não é garantido funcionamento em Windows sem adaptações.

###  Instalação no GitHub Codespaces
No terminal:
```bash
sudo apt update
sudo apt install -y \
gcc-riscv64-unknown-elf \
binutils-riscv64-unknown-elf \
qemu-system-misc \
make \
gdb-multiarch
```

### Verificação
No terminal:
```bash
riscv64-unknown-elf-gcc --version
qemu-system-riscv64 --version
```

### Como compilar
No termianl:
```bash
make
```

### Como executar
No terminal:
```bash
qemu-system-riscv64 \
-machine virt \
-m 128M \
-nographic \
-bios default \
-kernel kernel.elf
```

---

## Implementação

O TreeFS implementa os principais componentes de um sistema de arquivos hierárquico:

- Superbloco;
- Bitmap de inodes;
- Bitmap de blocos;
- Área de inodes;
- Área de dados;
- Estrutura de diretórios hierárquicos;
- Resolução de caminhos (`path_lookup()`);
- Criação de diretórios (`mkdir()`);
- Criação de arquivos (`create()`);
- Listagem de diretórios (`ls()`);
- Gerenciamento de inodes;
- Gerenciamento de blocos.

---

### Cenários de Teste
#### Cenário 1 - Listagem da raiz

Após a inicialização do sistema de arquivos:
```c
fs_init();
ls("/");
```

Saída esperada:
```bash
home
tmp
bin
```

#### Cenário 2 - Criação de diretório

Cria um novo diretório dentro de /home.
```c
mkdir("/home/aluno");
```

Estrutura resultante:
```bash
/
├── home
│   └── aluno
├── tmp
└── bin
```

#### Cenário 3 - Criação de arquivo

Cria um arquivo dentro do diretório criado.
```c
create("/home/aluno/notas.txt");
```

Estrutura:
```bash
/
├── home
│   └── aluno
│       └── notas.txt
├── tmp
└── bin
```

#### Cenário 4 - Navegação Hierárquica

Lista o conteúdo do diretório /home.
```c
ls("/home");
```

Saída esperada:
```bash
aluno
```

#### Cenário 5 - Resolução de Caminhos

Localiza um inode a partir de um caminho absoluto.
```c
path_lookup("/home/aluno/notas.txt");
```
O sistema percorre cada diretório até localizar o inode correspondente ao arquivo.

#### Cenário 6 - Gerenciamento de Inodes

Durante a criação de arquivos e diretórios:

- um inode livre é localizado;
- o inode é reservado;
- seus metadados são inicializados;
- o diretório pai recebe uma nova entrada.

#### Cenário 7 - Gerenciamento de Blocos

O sistema mantém um bitmap responsável por controlar:

- blocos livres;
- blocos ocupados.

Os blocos são preparados para serem utilizados pelas operações de leitura e escrita dos arquivos.

#### Cenário 8 - Estrutura Inicial do Sistema

Após executar:
```bash
fs_init();
```

A seguinte estrutura é criada automaticamente:
```bash
/
├── home
├── tmp
└── bin
```

---

## Estrutura:
```bash
boot/
│
├── start.S       # Boot inicial e runtime mínimo
└── trap_entry.S  # Entrada de traps
drivers/
│
└── block.c       # Gerenciamento de blocos
include/
│
├── block.h       # Interface do gerenciamento de blocos
├──	fs.h          # Interface do sistema de arquivos
├── inode.h       # Estruturas e gerenciamento de inodes
├──	memory.h      # Interface do gerenciador de memória
├──	scheduler.h   # Interface do escalonador
├──	string.h      # Funções auxiliares de manipulação de strings
├──	taks.h        # Estrutura TCB e criação de tasks
├── treefs.h      # Interface do TreeFS
└──	uart.h        # Interface de saída serial (UART)
kernel/
│
├── context.S     # Troca de contexto
├── fs.c          # Sistema de arquivos
├── inode.c       # Gerenciamento de inodes
├── main.c        # Inicialização do kernel
├── memory.c      # Heap do kernel
├── scheduler.c   # Escalonador
├── string.c      # Funções auxiliares de string
├── syscall.c     # Chamada de sistema (não implementado)
├── task.c        # Criação de tasks
├── timer.c       # Timer via SBI (não implementado)
├── trap.c        # Tratamento de interrupções (não implementado)
├── treefs.c      # Implementação do TreeFS
├── uart.c        # Saída serial
└── user.S        # Código de modo usuário
linker.ld         # Layout da memória
Makefile          # Compilação
README.md         # Explicação do projeto
```

### Observações
- Projeto com **foco didático** para compreensão de sistemas operacionais.
