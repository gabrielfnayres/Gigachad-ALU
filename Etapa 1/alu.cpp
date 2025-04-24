#include <cstdint>

#include "alu.h"

std::pair<unsigned int, unsigned int> ula6bits(char input[6])
{
    unsigned int carry_out = 0, output = 0;
    int A = input[2] - 48, B = input[3] - 48;

    if (input[4] == '1')
    {
        if (A == 1)
            A = 0;
        else
            A = 1;
    }

    if (input[0] == '0' && input[1] == '0')
    {
        if (A == 1 && B == 1)
            output = 1;
    }
    else if (input[0] == '0' && input[1] == '1')
    {
        if (A == 1 || B == 1)
            output = 1;
    }
    else if (input[0] == '1' && input[1] == '0')
    {
        if (B == 0)
            output = 1;
    }
    else
    {
        char carry_in = input[5] - 48;

        int sum = A + B + carry_in;

        if (sum == 3)
            return {1, 1};
        else if (sum == 2)
            return {0, 1};
        else if (sum == 1)
            return {1, 0};
    }

    return {output, carry_out};
}

// ULA de 8 bits, utilizada na execução das microinstruções
std::tuple<int32_t, int32_t, bool, bool, bool> ula8bits(char input[8], int32_t a, int32_t b)
{ 
    //Saída da ULA = (saída, saída deslocada, N, Z, carry out)
    bool N, Z, carry = 0;
    bool sll8 = input[0] == '1';
    bool sra1 = input[1] == '1';
    bool f0 = input[2] == '1';
    bool f1 = input[3] == '1';
    bool ena = input[4] == '1';
    bool enb = input[5] == '1';
    bool inva = input[6] == '1';
    bool inc = input[7] == '1';

    int32_t input_a = ena ? (inva ? ~a : a) : 0;
    int32_t input_b = enb ? b : 0;
    int32_t output, output_sd;

    if (f0 && f1)
    { // Adição
        // Converter para uint32_t para soma sem sinal
        uint32_t ua = static_cast<uint32_t>(input_a);
        uint32_t ub = static_cast<uint32_t>(input_b);
        uint32_t uinc = inc ? 1 : 0;
        uint64_t sum = static_cast<uint64_t>(ua) + static_cast<uint64_t>(ub) + uinc;
        output = static_cast<int32_t>(sum & 0xFFFFFFFF); 
        carry = (sum >> 32) & 1; // Carry é o bit 32
    }
    else if (!f0 && f1)
    { // OR
        output = input_a | input_b;
        carry = false;
    }
    else if (f0 && !f1)
    { // AND
        output = input_a & input_b;
        carry = false;
    }
    else
    { // XOR
        output = input_a ^ input_b;
        carry = false;
    }

    output_sd = output;

    if (sll8) // deslocamento lógico
        output_sd <<= 8;
    if (sra1) // deslocamento aritmético
        output_sd >>= 1;

    N = output_sd < 0;
    Z = output_sd == 0;

    return {output, output_sd, N, Z, carry};
}