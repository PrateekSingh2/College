#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

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

    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            recordTransaction("Deposit", amount);
            cout << "Successfully deposited Rs. " << amount << ". New Balance: Rs. " << balance << "\n";
        } else {
            cout << "Invalid deposit amount!\n";
        }
    }

    bool withdraw(double amount) {
        if (amount > 0 && balance >= amount) {
            balance -= amount;
            recordTransaction("Withdrawal", amount);
            cout << "Successfully withdrew Rs. " << amount << ". New Balance: Rs. " << balance << "\n";
            return true;
        } else {
            cout << "Insufficient balance or invalid amount!\n";
            return false;
        }
    }

    void showAccountDetails() const {
        cout << "\n--- Account Details ---\n";
        cout << "Account Number: " << accountNumber << "\n";
        cout << "Account Holder: " << accountHolderName << "\n";
        cout << "Current Balance: Rs. " << fixed << setprecision(2) << balance << "\n";
        cout << "-----------------------\n";
    }

    // Fund transfer function
    bool transferFunds(Account& recipient, double amount) {
        if (withdraw(amount)) {
            recipient.deposit(amount);
            cout << "Successfully transferred Rs. " << amount << " to A/C: " << recipient.getAccountNumber() << "\n";
            return true;
        }
        return false;
    }

    void generateMiniStatement() const {
        cout << "\n--- Mini Statement for A/C: " << accountNumber << " ---\n";
        cout << left << setw(20) << "Transaction Type" << setw(15) << "Amount" << "Balance After\n";
        cout << "--------------------------------------------------\n";
        
        int startIdx = max(0, (int)transactionHistory.size() - 5);
        for (int i = startIdx; i < transactionHistory.size(); ++i) {
            cout << left << setw(20) << transactionHistory[i].type 
                 << setw(15) << transactionHistory[i].amount 
                 << transactionHistory[i].balanceAfter << "\n";
        }
        cout << "--------------------------------------------------\n";
    }
};

class Bank {
private:
    vector<Account> accounts;
    int nextAccountNumber = 1001;

public:
    void createAccount(string name) {
        Account newAccount(nextAccountNumber, name);
        accounts.push_back(newAccount);
        cout << "\nAccount created successfully! Your Account Number is: " << nextAccountNumber << "\n";
        nextAccountNumber++;
    }

    Account* getAccount(int accNum) {
        for (size_t i = 0; i < accounts.size(); ++i) {
            if (accounts[i].getAccountNumber() == accNum) {
                return &accounts[i];
            }
        }
        cout << "Error: Account not found.\n";
        return nullptr;
    }

    void updateAccountName(int accNum, string newName) {
        Account* acc = getAccount(accNum);
        if (acc) {
            acc->updateName(newName);
            cout << "Account name updated successfully to " << newName << ".\n";
        }
    }

    void deleteAccount(int accNum) {
        for (auto it = accounts.begin(); it != accounts.end(); ++it) {
            if (it->getAccountNumber() == accNum) {
                accounts.erase(it);
                cout << "Account " << accNum << " has been closed and deleted.\n";
                return;
            }
        }
        cout << "Error: Account not found.\n";
    }

    
};

int main() {
    Bank myBank;
    int choice;

    while (true) {
        cout << "\n====== BANKING SYSTEM ======\n";
        cout << "1. Create Account (C)\n";
        cout << "2. View Account Details (R)\n";
        cout << "3. Update Account Name (U)\n";
        cout << "4. Delete Account (D)\n";
        cout << "5. Deposit Money\n";
        cout << "6. Withdraw Money\n";
        cout << "7. Print Mini Statement\n";
        cout << "8. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 8) {
            cout << "Exiting system. Goodbye!\n";
            break;
        }

        string name;
        int accNum;
        double amount;
        Account* acc;

        switch (choice) {
            case 1:
                cout << "Enter Account Holder Name: ";
                cin.ignore();
                getline(cin, name);
                myBank.createAccount(name);
                break;
            case 2:
                cout << "Enter Account Number: ";
                cin >> accNum;
                acc = myBank.getAccount(accNum);
                if (acc) acc->showAccountDetails();
                break;
            case 3:
                cout << "Enter Account Number: ";
                cin >> accNum;
                cout << "Enter New Name: ";
                cin.ignore();
                getline(cin, name);
                myBank.updateAccountName(accNum, name);
                break;
            case 4:
                cout << "Enter Account Number to Delete: ";
                cin >> accNum;
                myBank.deleteAccount(accNum);
                break;
            case 5:
                cout << "Enter Account Number: ";
                cin >> accNum;
                acc = myBank.getAccount(accNum);
                if (acc) {
                    cout << "Enter amount to deposit: ";
                    cin >> amount;
                    acc->deposit(amount);
                }
                break;
            case 6:
                cout << "Enter Account Number: ";
                cin >> accNum;
                acc = myBank.getAccount(accNum);
                if (acc) {
                    cout << "Enter amount to withdraw: ";
                    cin >> amount;
                    acc->withdraw(amount);
                }
                break;
            case 7:
                cout << "Enter Account Number: ";
                cin >> accNum;
                acc = myBank.getAccount(accNum);
                if (acc) acc->generateMiniStatement();
                break;
            default:
                cout << "Invalid choice! Please try again.\n";
        }
    }
    return 0;
}