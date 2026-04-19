#include "crow_all.h"
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <mutex>
#include <algorithm>

using namespace std;

struct Transaction {
    string type;
    double amount;
    double balanceAfter;
};

class Account {
private:
    int accountNumber;
    string accountHolderName;
    double balance;
    vector<Transaction> transactionHistory;

    void recordTransaction(const string& type, double amount) {
        transactionHistory.push_back({type, amount, balance});
    }

public:
    Account(int accNum = 0, string name = "") : accountNumber(accNum), accountHolderName(name), balance(0.0) {
        if (accNum != 0) {
            recordTransaction("Account Created", 0.0);
        }
    }

    int getAccountNumber() const { return accountNumber; }
    string getName() const { return accountHolderName; }
    double getBalance() const { return balance; }

    void updateName(const string& newName) {
        accountHolderName = newName;
    }

    bool deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            recordTransaction("Deposit", amount);
            return true;
        }
        return false;
    }

    bool withdraw(double amount) {
        if (amount > 0 && balance >= amount) {
            balance -= amount;
            recordTransaction("Withdrawal", amount);
            return true;
        }
        return false;
    }

    bool transferFunds(Account& recipient, double amount) {
        if (withdraw(amount)) {
            recipient.deposit(amount);
            // Record an extra transaction locally to specify it was a transfer
            recordTransaction("Transfer to A/C " + to_string(recipient.getAccountNumber()), amount);
            return true;
        }
        return false;
    }

    crow::json::wvalue getDetailsJSON() const {
        crow::json::wvalue res;
        res["accountNumber"] = accountNumber;
        res["name"] = accountHolderName;
        res["balance"] = balance;
        return res;
    }

    // New: Generates the mini statement as a JSON array
    crow::json::wvalue getStatementJSON() const {
        crow::json::wvalue res;
        int startIdx = max(0, (int)transactionHistory.size() - 5);
        int idx = 0;
        
        for (size_t i = startIdx; i < transactionHistory.size(); ++i) {
            res["transactions"][idx]["type"] = transactionHistory[i].type;
            res["transactions"][idx]["amount"] = transactionHistory[i].amount;
            res["transactions"][idx]["balanceAfter"] = transactionHistory[i].balanceAfter;
            idx++;
        }
        return res;
    }
};

class Bank {
private:
    vector<Account> accounts;
    int nextAccountNumber = 1001;
    mutex bankMutex; 

public:
    int createAccount(const string& name) {
        lock_guard<mutex> lock(bankMutex);
        Account newAccount(nextAccountNumber, name);
        accounts.push_back(newAccount);
        return nextAccountNumber++;
    }

    Account* getAccount(int accNum) {
        for (size_t i = 0; i < accounts.size(); ++i) {
            if (accounts[i].getAccountNumber() == accNum) return &accounts[i];
        }
        return nullptr;
    }

    bool performDeposit(int accNum, double amount) {
        lock_guard<mutex> lock(bankMutex);
        Account* acc = getAccount(accNum);
        if (acc) return acc->deposit(amount);
        return false;
    }

    bool performWithdraw(int accNum, double amount) {
        lock_guard<mutex> lock(bankMutex);
        Account* acc = getAccount(accNum);
        if (acc) return acc->withdraw(amount);
        return false;
    }

    bool performTransfer(int srcAccNum, int destAccNum, double amount) {
        lock_guard<mutex> lock(bankMutex);
        Account* srcAcc = getAccount(srcAccNum);
        Account* destAcc = getAccount(destAccNum);
        if (srcAcc && destAcc) {
            return srcAcc->transferFunds(*destAcc, amount);
        }
        return false;
    }
};

// CORS Middleware
struct AppCORS {
    struct context {};
    void before_handle(crow::request& req, crow::response& res, context& ctx) {}
    void after_handle(crow::request& req, crow::response& res, context& ctx) {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type");
    }
};

int main() {
    Bank myBank;
    crow::App<AppCORS> app;

    // Route: Create Account
    CROW_ROUTE(app, "/api/account").methods(crow::HTTPMethod::Post)
    ([&myBank](const crow::request& req){
        auto body = crow::json::load(req.body);
        if (!body || !body.has("name")) return crow::response(400, "Missing 'name'");
        
        int accNum = myBank.createAccount(body["name"].s());
        crow::json::wvalue res;
        res["status"] = "success";
        res["accountNumber"] = accNum;
        return crow::response(200, res);
    });

    // Route: Get Account Details
    CROW_ROUTE(app, "/api/account/<int>").methods(crow::HTTPMethod::Get)
    ([&myBank](int accNum){
        Account* acc = myBank.getAccount(accNum);
        if (!acc) return crow::response(404, "Account not found");
        return crow::response(200, acc->getDetailsJSON());
    });

    // Route: Get Mini Statement
    CROW_ROUTE(app, "/api/account/<int>/statement").methods(crow::HTTPMethod::Get)
    ([&myBank](int accNum){
        Account* acc = myBank.getAccount(accNum);
        if (!acc) return crow::response(404, "Account not found");
        return crow::response(200, acc->getStatementJSON());
    });

    // Route: Deposit
    CROW_ROUTE(app, "/api/account/<int>/deposit").methods(crow::HTTPMethod::Post)
    ([&myBank](const crow::request& req, int accNum){
        auto body = crow::json::load(req.body);
        if (!body || !body.has("amount")) return crow::response(400, "Missing 'amount'");
        if (myBank.performDeposit(accNum, body["amount"].d())) return crow::response(200, "{\"status\": \"success\"}");
        return crow::response(400, "Deposit failed");
    });

    // Route: Withdraw
    CROW_ROUTE(app, "/api/account/<int>/withdraw").methods(crow::HTTPMethod::Post)
    ([&myBank](const crow::request& req, int accNum){
        auto body = crow::json::load(req.body);
        if (!body || !body.has("amount")) return crow::response(400, "Missing 'amount'");
        if (myBank.performWithdraw(accNum, body["amount"].d())) return crow::response(200, "{\"status\": \"success\"}");
        return crow::response(400, "Insufficient funds or invalid account");
    });

    // Route: Transfer
    CROW_ROUTE(app, "/api/account/<int>/transfer").methods(crow::HTTPMethod::Post)
    ([&myBank](const crow::request& req, int accNum){
        auto body = crow::json::load(req.body);
        if (!body || !body.has("amount") || !body.has("destination")) return crow::response(400, "Missing data");
        if (myBank.performTransfer(accNum, body["destination"].i(), body["amount"].d())) return crow::response(200, "{\"status\": \"success\"}");
        return crow::response(400, "Transfer failed");
    });

    // Preflight OPTIONS requests for CORS
    CROW_ROUTE(app, "/api/account").methods(crow::HTTPMethod::Options)([]() { return crow::response(204); });
    CROW_ROUTE(app, "/api/account/<int>").methods(crow::HTTPMethod::Options)([](int) { return crow::response(204); });
    CROW_ROUTE(app, "/api/account/<int>/statement").methods(crow::HTTPMethod::Options)([](int) { return crow::response(204); });
    CROW_ROUTE(app, "/api/account/<int>/deposit").methods(crow::HTTPMethod::Options)([](int) { return crow::response(204); });
    CROW_ROUTE(app, "/api/account/<int>/withdraw").methods(crow::HTTPMethod::Options)([](int) { return crow::response(204); });
    CROW_ROUTE(app, "/api/account/<int>/transfer").methods(crow::HTTPMethod::Options)([](int) { return crow::response(204); });

    cout << "🚀 Core Banking API running on http://localhost:8080" << endl;
    app.port(8080).multithreaded().run();
}