#include <bits/stdc++.h>
using namespace std;

const string INPUT_FILE = "mempool.csv";
const string OUTPUT_FILE = "block.txt";
const int MAX_BLOCK_WEIGHT = 4000000;

class TransactionInfo {                    // Class for transaction data structure
    private:
        int fee;
        int weight;
        string tx_id;
        vector<string> parents;

    public: 
        TransactionInfo(vector<string> &row) {
            tx_id = row[0];
            fee = stoi(row[1]);
            weight = stoi(row[2]);
            for(int i = 3; i < row.size(); i++) {
                parents.push_back(row[i]);
            }
        }

        int getFee() {
            return fee;
        }

        int getWeight() {
            return weight;
        }

        string getTx_Id() {
            return tx_id;
        }

        vector<string> getParents() {
            return parents;
        }
};

void readInput(string input_file_name, vector<TransactionInfo*>& transactions)    // Function for reading input from CSV
{
    ifstream fin(input_file_name);

    vector<string> row;
    string word;
    string line;
    
    getline(fin, line);
    
    while(getline(fin, line))
    {
        row.clear();
        stringstream s(line);
        while(getline(s, word, ','))
            row.push_back(word);

        transactions.push_back(new TransactionInfo(row));
    }

    fin.close();

    cout<<"Transaction Count: " << transactions.size() << endl;
}

void writeOutput(vector<string>& tx_IdOfTransactionsInBlock, string output_file_name) {   // Function for writing output file
    ofstream myfile(output_file_name);
    for(auto& tx_id : tx_IdOfTransactionsInBlock)
        myfile << tx_id << "\n";
    myfile.close();
}

vector<TransactionInfo*> removeInvalidTransactions(vector<TransactionInfo*> transactions) {

    /*
     * This function removes those transactions whose all parents are not present before them 
     * in the input transactions list.
     */

    unordered_set<string> seenTransactions;
    vector<TransactionInfo*> validTransactions;

    for(auto &transaction: transactions) {
        bool isValid = true;
        for(auto &parent: transaction->getParents()) {
            if(seenTransactions.find(parent) == seenTransactions.end()) {
                isValid = false;
                break;
            }
        }
        if(isValid) {
            validTransactions.push_back(transaction);
            seenTransactions.insert(transaction->getTx_Id());
        }
    }

    return validTransactions;
}

bool isValid(TransactionInfo* transaction, set<string>& includedTransactionIds) {
    
    /* 
     * This function checks whether the transaction that we are going to include, is 
     * valid or not by checking the inclusion of its parent transactions. 
     */

    for(auto& parent: transaction->getParents()) {
        if(includedTransactionIds.find(parent) == includedTransactionIds.end()) {
            return false;
        }
    }

    return true;
}

int main() {
    vector<TransactionInfo*> transactions;
    readInput(INPUT_FILE, transactions);

    vector<TransactionInfo*> validTransactions = removeInvalidTransactions(transactions);

    cout << "Total valid transactions: " << validTransactions.size() << endl;

    set<string> includedTransactionIds;
    vector<int> includedTransactionIndices;
    set<pair<double, int>, greater<pair<double, int>>> leftOverTransactions;   /* This set includes transaction
                                                                                * indices along ordered according
                                                                                * to fee-weight ratio since
                                                                                * including transactions with higher
                                                                                * fee-weight ratio is greedily better
                                                                                */

    int index = 0;

    for(auto &transaction: validTransactions) {
        leftOverTransactions.insert({((double) transaction->getFee()) / transaction->getWeight(), index++});  
    }

    int totalBlockWeight = 0;
    int maxFee = 0;

    while(!leftOverTransactions.empty() and totalBlockWeight <= MAX_BLOCK_WEIGHT) {
        bool isAnyInsertionHappened = false;
        
        for(auto& element: leftOverTransactions) {
            int index = element.second;

            TransactionInfo* transaction = validTransactions[index];
            string tx_id = transaction->getTx_Id();
            int weight = transaction->getWeight();
            int fee = transaction->getFee();
            
            if(isValid(transaction, includedTransactionIds) and totalBlockWeight + weight <= MAX_BLOCK_WEIGHT) {
                totalBlockWeight += weight;
                maxFee += fee;
                includedTransactionIndices.push_back(index);
                includedTransactionIds.insert(tx_id);
                leftOverTransactions.erase(element);
                isAnyInsertionHappened = true;
                break;
            }
        }
        if(!isAnyInsertionHappened) {
            break;
        }
    }

    sort(includedTransactionIndices.begin(), includedTransactionIndices.end());
    
    vector<string> tx_IdOfTransactionsInBlock;

    for(int index: includedTransactionIndices) {
        tx_IdOfTransactionsInBlock.push_back(validTransactions[index]->getTx_Id());
    }

    cout << "Total fees in final block :" << maxFee << endl;
    cout << "Total weight of final block :" << totalBlockWeight << endl; 
    cout << "Total number of transactions in final block :" << tx_IdOfTransactionsInBlock.size() << endl;

    writeOutput(tx_IdOfTransactionsInBlock, OUTPUT_FILE);
}