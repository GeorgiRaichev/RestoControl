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