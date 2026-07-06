# Projeto de Implementação de um Sistema de Arquivos hierárquico chamado TreeFS

O **projeto-base** utilizado neste trabalho está disponível em: `https://github.com/VielF/BasicMicrokernel`

## Descrição
Este projeto consiste na implementação de um **sistema de arquivos hierárquico**, denominado **TreeFS**, desenvolvido sobre um microkernel para arquitetura **RISC-V**.

O objetivo é implementar os principais mecanismos de um sistema de arquivos inspirado nos sistemas Unix, incluindo gerenciamento de diretórios, resolução de caminhos, criação e remoção de arquivos, controle de inodes e blocos físicos, além das funcionalidades extras de timestamps e permissões de acesso.

### Funcionalidades

O TreeFS implementa:
- Sistema de arquivos hierárquico;
- Criação e remoção de diretórios;
- Criação e remoção de arquivos;
- Navegação por caminhos absolutos;
- Gerenciamento de inodes;
- Gerenciamento de blocos físicos;
- Leitura e escrita de arquivos;
- Reutilização de inodes e blocos liberados;
- Timestamps para criação, modificação e acesso (extra);
- Sistema de permissões de acesso (extra).

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
No terminal:
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

### Finalizando o QEMU
Para sair da execução do sistema no terminal:

Pressione:
```bash
Ctrl + a
```
Depois:
```bash
x
```

---

## Implementação

O TreeFS implementa os principais componentes de um sistema de arquivos hierárquico:

### Estruturas principais
- Superbloco;
- Bitmap de inodes;
- Bitmap de blocos;
- Área de inodes;
- Área de dados;
- Estrutura hierárquica de diretórios;
- Tabela de arquivos abertos.

### Operações de arquivos

Foram implementadas:

- Resolução de caminhos (`path_lookup()`);
- Criação de diretórios (`mkdir()`);
- Criação de arquivos (`create()`);
- Listagem de diretórios (`ls()`);
- Escrita em arquivos (`write()`);
- Leitura de arquivos (`read()`);
- Remoção de arquivos (`unlink()`).

---
## Funcionalidades Extras Implementadas
### Timestamps

Foi implementado um sistema simples de controle de tempo dos arquivos.

Cada inode possui três informações:
```bash
created_at
modified_at
accessed_at
```

Esses valores representam:
- **created_at**: momento da criação do arquivo;
- **modified_at**: última alteração realizada através de escrita;
- **accessed_at**: último acesso realizado através de leitura.

O timestamp utiliza um **contador global crescente**, garantindo que cada evento receba um valor maior que o anterior.

Exemplo:
```bash
Criado: 6
Modificado: 7
Ultimo acesso: 8
```

O contador é utilizado apenas como representação lógica de tempo, já que o projeto possui finalidade didática e não utiliza um relógio físico real.

### Sistema de Permissões

Foi implementado um controle simples de permissões baseado em bits:
```bash
PERM_READ  = 4
PERM_WRITE = 2
PERM_EXEC  = 1
```

Cada inode possui um campo:
```bash
uint8_t permissions;
```

As permissões permitem controlar:
- Leitura de arquivos;
- Escrita em arquivos;
- Acesso a diretórios.

Também foi implementada a operação:
```c
chmod()
```
responsável por alterar as permissões de um arquivo.

Exemplo:
```c
chmod("/home/aluno/gabarito.txt", PERM_READ);
```

Após essa alteração, tentativas de escrita são bloqueadas pelo sistema.


---
## Cenários de Teste
### Cenário 1 - Listagem da raiz

Após a inicialização do sistema de arquivos:
```c
fs_init();
ls("/");
```

Resultado esperado:
```bash
home
tmp
bin
```

### Cenário 2 - Criação de diretório

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

### Cenário 3 - Criação de arquivo

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

### Cenário 4 - Escrita em arquivo

Escreve em um arquivo.
```c
write(fd,"Sistemas Operacionais",22);
```

Resultado esperado:
```bash
Bloco fisico alocado: 0
Total de bytes escritos: 22
```

O timestamp de **modificação** é atualizado.

### Cenário 5 - Leitura de arquivo

Faz a leitura de um arquivo.
```c
read(fd,buffer,22);
```

Resultado esperado:
```bash
Total de bytes lidos: 22
Buffer: Sistemas Operacionais
```

O timestamp de **último acesso** é atualizado.

### Cenário 6 - Remoção de arquivo

Faz a remoção de um arquivo.
```c
unlink("/home/aluno/notas.txt");
```

Resultado esperado:
```bash
Bloco fisico liberado: 0
Inode liberado: 5
```

### Cenário 7 - Navegação hierárquica

Lista o conteúdo do diretório /home.
```c
ls("/home");
```

Resultado esperado:
```bash
aluno
```

### Cenário 8 - Reutilização de recursos

Após remover o arquivo anterior:

```bash
Inode alocado: 5
Bloco fisico alocado: 0
```

Estrutura final:
```bash
/
├── home
│   └── aluno
│       └── teste_cenario.txt
├── tmp
└── bin
```

O inode e o bloco físico liberados anteriormente foram reutilizados, demonstrando o correto funcionamento dos bitmaps de gerenciamento de recursos.

### Cenário 9 - Controle de Permissões (Extra)

Criação:
```c
create("/home/aluno/gabarito.txt");
```

Alteração:
```c
chmod("/home/aluno/gabarito.txt", PERM_READ);
```

Tentativa de escrita:
```c
write(fd,"Tentando modificar...",21);
```

Resultado esperado:
```bash
Permissoes alteradas para Read Only.
Tentando modificar arquivo protegido...
Permissao negada para escrita...
O Sistema Operacional bloqueou a modificacao.
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
├──	task.h        # Estrutura TCB e criação de tasks
├── timestamp.h   # Interface do timestamp
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
├── timestamp.c   # Implementação do timestamp
├── trap.c        # Tratamento de interrupções (não implementado)
├── treefs.c      # Implementação do TreeFS
├── uart.c        # Saída serial
└── user.S        # Código de modo usuário
linker.ld         # Layout da memória
Makefile          # Compilação
README.md         # Explicação do projeto
```

### Observação
- Projeto com **foco didático** para compreensão de sistemas operacionais.
