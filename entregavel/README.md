# Mic-1 Interpreter

Este programa implementa um interpretador para a Mic-1 modificada que reconhece e executa as instruções ILOAD x, DUP e BIPUSH byte.

## Funcionalidades

- Reconhece as instruções ILOAD x, DUP e BIPUSH byte de um arquivo de texto
- Traduz estas instruções para uma sequência de microinstruções
- Executa as microinstruções na Mic-1 modificada
- Gera um log detalhado com o estado dos registradores e da memória

## Requisitos

- Compilador C++ com suporte a C++11
- Make

## Estrutura de Arquivos

- `mic1_interpreter.cpp`: Implementação principal do interpretador
- `Makefile`: Arquivo para compilação do projeto
- `registradores_etapa3_tarefa1`: Arquivo com os valores iniciais dos registradores
- `dados_etapa3_tarefa1.txt`: Arquivo com os valores iniciais da memória
- `instrucoes.txt`: Arquivo com as instruções a serem executadas

## Compilação

Para compilar o programa, execute:

```
make
```

## Execução

Para executar o programa com os arquivos padrão, execute:

```
make run_with_files
```

Ou você pode especificar os arquivos de entrada:

```
./mic1_interpreter -r <arquivo_registradores> -m <arquivo_memoria> -i <arquivo_instrucoes>
```

## Formato do Log

O programa gera um arquivo de log `execution_log.txt` que contém:

- O valor de todos os registradores (H, OPC, TOS, CPP, LV, SP, MBR, PC, MDR, MAR) no início e no fim de cada microinstrução
- O registrador que está comandando o barramento B
- Os registradores que estão habilitados para escrita no barramento C
- Os valores contidos nas linhas de memória após a execução de cada instrução

## Particularidades da Implementação

- A instrução BIPUSH tem uma particularidade: quando traduzida para uma sequência de microinstruções, a segunda microinstrução tem seus 8 primeiros bits definidos pelo argumento da instrução.
- As microinstruções são representadas em formato binário de 23 bits.
