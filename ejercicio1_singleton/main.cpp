#include <iostream>
#include <thread>
#include <vector>
#include "transaction_manager.hpp"

int main() {
    auto& manager = TransactionManager::getInstance();
    Account account("A1", 1000);

    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
            manager.deposit(&account, 100);
            manager.withdraw(&account, 50);
        });
    }
    for (auto& t : threads) t.join();

    std::cout << "Balance final: " << account.getBalance() << std::endl;
    std::cout << "Transacciones registradas: " << manager.historySize() << std::endl;
    std::cout << "Direccion manager: " << &TransactionManager::getInstance() << std::endl;
    std::cout << "Direccion manager (2da llamada): " << &TransactionManager::getInstance() << std::endl;
}
