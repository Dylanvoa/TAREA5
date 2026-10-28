#include <iostream>
#include "expression.hpp"

int main() {
    auto x = std::make_shared<Variable>();

    // f(x) = (x^2 + 3x) * sin(x)
    auto expr =
        std::make_shared<Multiply>(
            std::make_shared<Add>(
                std::make_shared<Power>(
                    x,
                    std::make_shared<Constant>(2)
                ),
                std::make_shared<Multiply>(
                    std::make_shared<Constant>(3),
                    x
                )
            ),
            std::make_shared<Sin>(x)
        );

    auto deriv = expr->derivative();

    std::cout << "f(x) = " << expr->toString() << std::endl;
    std::cout << "f'(x) = " << deriv->toString() << std::endl;
    std::cout << "f(2) = " << expr->evaluate(2) << std::endl;
    std::cout << "f'(2) = " << deriv->evaluate(2) << std::endl;

    // Anidamiento arbitrario: g(x) = ln(exp(x^2) / cos(x))
    auto g = std::make_shared<Ln>(
        std::make_shared<Divide>(
            std::make_shared<Exp>(std::make_shared<Power>(x, std::make_shared<Constant>(2))),
            std::make_shared<Cos>(x)));
    std::cout << "\ng(x) = " << g->toString() << std::endl;
    std::cout << "g'(x) = " << g->derivative()->toString() << std::endl;
    std::cout << "g'(1) = " << g->derivative()->evaluate(1) << " (esperado 2 + tan(1) = "
              << 2 + std::tan(1.0) << ")" << std::endl;
}
