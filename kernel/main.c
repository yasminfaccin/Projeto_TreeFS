#include "task.h"
#include "scheduler.h"
#include "memory.h"
#include "uart.h"
#include "treefs.h"

extern void uart_print(const char*);

/*   Tasks   */

void task1()
{
    while (1)
    {
        uart_print("Task 1 running\n");

        uart_print("Memory used: ");
        uart_print_uint(memory_used());
        uart_print(" bytes\n");

        uart_print("Memory free: ");
        uart_print_uint(memory_free());
        uart_print(" bytes\n\n");

        yield();
    }
}

void task2()
{
    while (1)
    {
        uart_print("Task 2 running\n");

        uart_print("Memory used: ");
        uart_print_uint(memory_used());
        uart_print(" bytes\n");

        uart_print("Memory free: ");
        uart_print_uint(memory_free());
        uart_print(" bytes\n\n");

        yield();
    }
}

void execucao_cenarios(){
    uart_print("\n=== Execucao dos Cenarios ===\n");

    uart_print("\nCenario 1 - Listagem da Raiz \n");
    uart_print("=============================\n");
    ls("/");
    uart_print("\n");

    uart_print("Cenario 2 - Criacao de Diretorio ('/home/aluno')\n");
    uart_print("================================================\n");
    mkdir("/home/aluno");
    uart_print("(Verificando /home...)\n");
    ls("/home");
    uart_print("\n");

    uart_print("Cenario 3- Criacao de Arquivos('/home/aluno/notas.txt')\n");
    uart_print("=======================================================\n");
    int fd = create("/home/aluno/notas.txt");
    uart_print("Timestamp apos criar o arquivo:\n");
    informacoes_timestamp("/home/aluno/notas.txt");
    uart_print("(Verificando /aluno...)\n");
    ls("/home/aluno");

    uart_print("\nCenario 4 - Escrita ('Sistemas Operacionais')\n");
    uart_print("=============================================\n");
    int bytes_escritos = write(fd, "Sistemas Operacionais", 22);
    uart_print("Timestamp apos escrita:\n");
    informacoes_timestamp("/home/aluno/notas.txt");
    uart_print("Total de bytes escritos: ");
    uart_print_uint(bytes_escritos);
    uart_print("\n");

    uart_print("\nCenario 5 - Leitura\n");
    uart_print("====================\n");
    char buffer[30]; // Armazenamento para leitura
    int bytes_lidos = read(fd,buffer,22);
    uart_print("Timestamp apos leitura:\n");
    informacoes_timestamp("/home/aluno/notas.txt");
    uart_print("Total de bytes lidos: ");
    uart_print_uint(bytes_lidos);
    uart_print(" - Buffer: ");
    uart_print(buffer); 
    uart_print("\n");

    uart_print("\nCenario 6 - Remocao ('/home/aluno/notas.txt')\n");
    uart_print("=============================================\n");
    unlink("/home/aluno/notas.txt");
    uart_print("(Verificando /home/aluno...)\n");
    ls("/home/aluno");

    uart_print("\nCenario 7 - Navegacao Hierarquica ('/home')\n");
    uart_print("===========================================\n");
    uart_print("(Verificando /home...)\n");
    ls("/home");

    uart_print("\nCenario 8 - Reutilizacao de inodes e blocos liberados\n");
    uart_print("=====================================================\n");
    uart_print("(Criando teste_cenario.txt em /home/aluno)\n");
    int fd_novo = create("/home/aluno/teste_cenario.txt");
    write(fd_novo, "Reunitilizando bloco...", 23);
    uart_print("(Verificando /home/aluno...)\n");
    ls("/home/aluno");
    uart_print("\n");
}

/*   Kernel   */

void kernel_main()
{
    memory_init();   // OBRIGATÓRIO
    
    fs_init();

    execucao_cenarios();

    while(1);
    
    uart_print("\n=== Kernel ===\n");

    xTaskCreate(task1, 2048, 1);
    xTaskCreate(task2, 2048, 1);

    scheduler_start();

    

    while (1);
}