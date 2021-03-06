#pragma once

#include <stdint.h>

const char keymap[][3] = {
    {2, '1', '!'},
    {3, '2', '@'},
    {4, '3', '#'},
    {5, '4', '$'},
    {6, '5', '%'},
    {7, '6', '^'},
    {8, '7', '&'},
    {9, '8', '*'},
    {10, '9', '('},
    {11, '0', ')'},
    {12, '-', '_'},
    {13, '=', '+'},
    {14, ' ', ' '},
    {15, '\t', '\t'},
    {16, 'q', 'Q'},
    {17, 'w', 'W'},
    {18, 'e', 'E'},
    {19, 'r', 'R'},
    {20, 't', 'T'},
    {21, 'y', 'Y'},
    {22, 'u', 'U'},
    {23, 'i', 'I'},
    {24, 'o', 'O'},
    {25, 'p', 'P'},
    {26, '[', '{'},
    {27, ']', '}'},
    {30, 'a', 'A'},
    {31, 's', 'S'},
    {32, 'd', 'D'},
    {33, 'f', 'F'},
    {34, 'g', 'G'},
    {35, 'h', 'H'},
    {36, 'j', 'J'},
    {37, 'k', 'K'},
    {38, 'l', 'L'},
    {39, ';', ':'},
    {40, '\'', '"'},
    {41, '`', '~'},
    {43, '\\', '|'},
    {44, 'z', 'Z'},
    {45, 'x', 'X'},
    {46, 'c', 'C'},
    {47, 'v', 'V'},
    {48, 'b', 'B'},
    {49, 'n', 'N'},
    {50, 'm', 'M'},
    {51, ',', '<'},
    {52, '.', '>'},
    {53, '/', '?'},
    {74, '-', '-'},
};

#define COUNT(A) (sizeof(A) / sizeof(A[0]))
