#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>  // For output formatting

using namespace std;

// Function to display the current menu with price formatting
void displayMenu() {
    ifstream file("menu.txt");

    if (!file) {
        cout << "Error: Menu file not found.\n";
        return;
    }

    string item;
    double price;

    cout << "\n--- Current Menu ---\n";
    cout << left << setw(20) << "Item" << right << setw(10) << "Price (lv.)" << endl;
    cout << "-------------------------------\n";

    while (file >> item >> price) {
        cout << left << setw(20) << item
            << right << setw(8) << fixed << setprecision(2) << price << " lv." << endl;
    }

    file.close();
}

// Function to add a new item to the menu
void addItemToMenu() {
    ofstream menuFile("menu.txt", ios::app);  // Append to the end of the file

    if (!menuFile) {
        cout << "Error: Unable to open menu file.\n";
        return;
    }

    string itemName;
    double itemPrice;

    cout << "Enter item name: ";
    cin >> itemName;
    cout << "Enter item price: ";
    cin >> itemPrice;

    // Write to the menu
    menuFile << itemName << " " << itemPrice << endl;
    cout << itemName << " added to the menu for " << itemPrice << " lv.\n";

    menuFile.close();
}

struct MenuItem {
    string name;
    double price;
};
// Function to remove an item from the menu
void removeItemFromMenu() {
    ifstream menuFile("menu.txt");
    ofstream tempFile("temp.txt");

    if (!menuFile.is_open()) {
        cout << "Error: Unable to open menu file.\n";
        return;
    }

    if (!tempFile.is_open()) {
        cout << "Error: Unable to create temporary file.\n";
        menuFile.close();
        return;
    }

    string itemName;
    cout << "Enter item name to remove: ";
    cin >> ws; // To consume any leading whitespace
    getline(cin, itemName); // To allow item names with spaces

    string currentItem;
    double price;
    bool itemFound = false;

    // Read each line from menu.txt
    while (menuFile >> ws && getline(menuFile, currentItem, ' ')) {
        // Read the price
        if (!(menuFile >> price)) {
            // If price is not read correctly, skip the rest of the line
            string skipLine;
            getline(menuFile, skipLine);
            continue;
        }

        // Check if the current item matches the item to be removed
        if (currentItem == itemName) {
            itemFound = true;
            cout << currentItem << " removed from the menu.\n";
            // Skip writing this item to temp.txt
            // break;
        }
        else {
            // Write the item back to temp.txt
            tempFile << currentItem << " " << price << endl;
        }

        // Clear any remaining characters in the line
        menuFile.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    menuFile.close();
    tempFile.close();

    if (itemFound) {
        // Remove the original menu file
        if (remove("menu.txt") != 0) {
            cout << "Error: Failed to remove the original menu file.\n";
            return;
        }

        // Rename temp.txt to menu.txt
        if (rename("temp.txt", "menu.txt") != 0) {
            cout << "Error: Failed to rename temp.txt to menu.txt.\n";
            return;
        }
    }
    else {
        // If the item was not found, delete the temp file
        remove("temp.txt");
        cout << "Error: Item \"" << itemName << "\" not found in the menu.\n";
    }
}