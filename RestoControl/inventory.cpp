#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Function to check inventory availability
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

// Function to issue a product from inventory
void issueProduct() {
    ifstream inventoryFile("inventory.txt");
    ofstream tempFile("temp.txt");

    if (!inventoryFile || !tempFile) {
        cout << "Error: Unable to open inventory file.\n";
        return;
    }

    string itemName, currentItem;
    int quantity, amountToIssue;
    bool itemFound = false;

    cout << "Enter item name to issue: ";
    cin >> itemName;
    cout << "Enter quantity to issue: ";
    cin >> amountToIssue;

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

    inventoryFile.close();
    tempFile.close();

    // Replace the old file with the new one
    remove("inventory.txt");
    if (rename("temp.txt", "inventory.txt") != 0) {
        cout << "Error: Failed to rename temp file to inventory.txt.\n";
    }
}

// Function to add a product to the inventory
void addInventory() {
    ifstream inventoryFile("inventory.txt");
    ofstream tempFile("temp.txt");

    if (!inventoryFile || !tempFile) {
        cout << "Error: Unable to open inventory file.\n";
        return;
    }

    string itemName, currentItem;
    int quantity, existingQuantity;
    bool itemFound = false;

    cout << "Enter item name to add: ";
    cin >> itemName;
    cout << "Enter quantity to add: ";
    cin >> quantity;

    while (inventoryFile >> currentItem >> existingQuantity) {
        if (currentItem == itemName) {
            existingQuantity += quantity;
            itemFound = true;
        }
        tempFile << currentItem << " " << existingQuantity << endl;
    }

    // If the product is not found, add a new line
    if (!itemFound) {
        tempFile << itemName << " " << quantity << endl;
        cout << itemName << " added to inventory with quantity " << quantity << endl;
    }
    else {
        cout << quantity << " units of " << itemName << " added to existing stock.\n";
    }

    inventoryFile.close();
    tempFile.close();

    // Replace the old file with the new one
    remove("inventory.txt");
    if (rename("temp.txt", "inventory.txt") != 0) {
        cout << "Error: Failed to rename temp file to inventory.txt.\n";
    }
}