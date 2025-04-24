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
