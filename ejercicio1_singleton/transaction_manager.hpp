#pragma once
#include <cstddef>
#include <mutex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class Transaction;

// Cuenta bancaria. Su saldo solo puede ser modificado por Transaction
// (friend), y cada acceso esta protegido por el mutex de la cuenta.
class Account {
public:
    Account(std::string id, double balance)
        : id_(std::move(id)), balance_(balance) {}

    Account(const Account&) = delete;
    Account& operator=(const Account&) = delete;

    double getBalance() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return balance_;
    }
    const std::string& getId() const { return id_; }

private:
    friend class Transaction;
    std::string id_;
    double balance_;
    mutable std::mutex mutex_;
};

// Una operacion individual sobre una cuenta. execute() es atomica:
// verifica y modifica el saldo bajo el lock de la cuenta.
class Transaction {
public:
    enum class Type { Deposit, Withdraw };

    Transaction(Type type, Account* account, double amount)
        : type_(type), account_(account), amount_(amount) {
        if (!account_) throw std::invalid_argument("Cuenta nula");
        if (amount_ <= 0) throw std::invalid_argument("Monto debe ser positivo");
    }

    bool execute() {
        std::lock_guard<std::mutex> lock(account_->mutex_);
        if (type_ == Type::Deposit) {
            account_->balance_ += amount_;
            success_ = true;
        } else if (account_->balance_ >= amount_) {
            account_->balance_ -= amount_;
            success_ = true;
        } else {
            success_ = false;  // fondos insuficientes
        }
        return success_;
    }

    Type type() const { return type_; }
    double amount() const { return amount_; }
    bool succeeded() const { return success_; }
    const std::string& accountId() const { return account_->getId(); }

private:
    Type type_;
    Account* account_;
    double amount_;
    bool success_ = false;
};

// Singleton (Meyers): inicializacion thread-safe desde C++11.
class TransactionManager {
public:
    static TransactionManager& getInstance() {
        static TransactionManager instance;
        return instance;
    }

    TransactionManager(const TransactionManager&) = delete;
    TransactionManager& operator=(const TransactionManager&) = delete;

    bool deposit(Account* account, double amount) {
        return run(Transaction(Transaction::Type::Deposit, account, amount));
    }

    bool withdraw(Account* account, double amount) {
        return run(Transaction(Transaction::Type::Withdraw, account, amount));
    }

    std::size_t historySize() const {
        std::lock_guard<std::mutex> lock(historyMutex_);
        return history_.size();
    }

private:
    TransactionManager() = default;

    bool run(Transaction tx) {
        bool ok = tx.execute();
        std::lock_guard<std::mutex> lock(historyMutex_);
        history_.push_back(std::move(tx));
        return ok;
    }

    mutable std::mutex historyMutex_;
    std::vector<Transaction> history_;
};
