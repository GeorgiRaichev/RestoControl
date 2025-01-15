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