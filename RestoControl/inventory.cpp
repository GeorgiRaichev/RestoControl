#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// ==============================
// Function: checkInventory
// ==============================
// Reads and displays the current inventory.
void checkInventory() {
    ifstream inventoryFile("inventory.txt");

    if (!inventoryFile) {
        cout << "Error: Inventory file not found.\n";
        return;
    }

    string item;
    int quantity;

    cout << "\n--- Current Inventory ---\n";
    while (inventoryFile >> item >> quantity) {
        cout << item << " - " << quantity << " available\n";
    }

    inventoryFile.close();
}

// ==============================
// Helper Functions for issueProduct
// ==============================

// Processes the issuance of a specified quantity for a given item.
void processIssueProduct(ifstream& inventoryFile, ofstream& tempFile, const string& itemName, int amountToIssue) {
    string currentItem;
    int quantity;
    bool itemFound = false;

    while (inventoryFile >> currentItem >> quantity) {
        if (currentItem == itemName) {
            itemFound = true;
            if (quantity >= amountToIssue) {
                quantity -= amountToIssue;
                cout << amountToIssue << " " << itemName << " issued. Remaining: " << quantity << endl;
            }
            else {
                cout << "Error: Not enough " << itemName << " in stock.\n";
            }
        }
        tempFile << currentItem << " " << quantity << endl;
    }

    if (!itemFound) {
        cout << "Error: Item " << itemName << " not found in inventory.\n";
    }
}

// ==============================
// Function: issueProduct
// ==============================
// Issues a specified quantity of a product from the inventory.
void issueProduct() {
    ifstream inventoryFile("inventory.txt");
    ofstream tempFile("temp.txt");

    if (!inventoryFile || !tempFile) {
        cout << "Error: Unable to open inventory file.\n";
        return;
    }

    string itemName;
    int amountToIssue;

    cout << "Enter item name to issue: ";
    cin >> itemName;
    cout << "Enter quantity to issue: ";
    cin >> amountToIssue;

    processIssueProduct(inventoryFile, tempFile, itemName, amountToIssue);

    inventoryFile.close();
    tempFile.close();

    // Replace the old inventory file with the updated temporary file
    remove("inventory.txt");
    if (rename("temp.txt", "inventory.txt") != 0) {
        cout << "Error: Failed to rename temp file to inventory.txt.\n";
    }
}

// ==============================
// Helper Functions for addInventory
// ==============================

// Processes adding a specified quantity to an item in the inventory.
void processAddInventory(ifstream& inventoryFile, ofstream& tempFile, const string& itemName, int quantity) {
    string currentItem;
    int existingQuantity;
    bool itemFound = false;

    while (inventoryFile >> currentItem >> existingQuantity) {
        if (currentItem == itemName) {
            existingQuantity += quantity;
            itemFound = true;
        }
        tempFile << currentItem << " " << existingQuantity << endl;
    }

    // If the product was not found, add it to the end
    if (!itemFound) {
        tempFile << itemName << " " << quantity << endl;
        cout << itemName << " added to inventory with quantity " << quantity << endl;
    }
    else {
        cout << quantity << " units of " << itemName << " added to existing stock.\n";
    }
}

// ==============================
// Function: addInventory
// ==============================
// Adds a specified quantity of a product to the inventory.
void addInventory() {
    ifstream inventoryFile("inventory.txt");
    ofstream tempFile("temp.txt");

    if (!inventoryFile || !tempFile) {
        cout << "Error: Unable to open inventory file.\n";
        return;
    }

    string itemName;
    int quantity;

    cout << "Enter item name to add: ";
    cin >> itemName;
    cout << "Enter quantity to add: ";
    cin >> quantity;

    processAddInventory(inventoryFile, tempFile, itemName, quantity);

    inventoryFile.close();
    tempFile.close();

    // Replace the old inventory file with the updated temporary file
    remove("inventory.txt");
    if (rename("temp.txt", "inventory.txt") != 0) {
        cout << "Error: Failed to rename temp file to inventory.txt.\n";
    }
}
