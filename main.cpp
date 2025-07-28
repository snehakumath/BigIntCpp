#include <iostream>
#include "BigInt.h"

int main() {
    BigInt a, b;
    std::cout << "Enter two large numbers:\n";
    std::cin >> a >> b;

    std::cout << "Sum = " << a + b << std::endl;
    std::cout << "Difference = " << a - b << std::endl;
    std::cout << "Product = " << a * b << std::endl;

    int divisor;
    std::cout << "Enter an integer to divide first number by: ";
    std::cin >> divisor;
    std::cout << "Quotient = " << a / divisor << std::endl;

    return 0;
}
