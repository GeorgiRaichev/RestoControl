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
    ofstream menuFile("menu.txt", ios::app);
    ofstream recipesFile("recipes.txt", ios::app);

    if (!menuFile || !recipesFile) {
        cout << "Error: Unable to open menu or recipes file.\n";
        return;
    }

    string itemName;
    double itemPrice;

    cout << "Enter item name: ";
    cin >> ws;  // Remove leading whitespace
    getline(cin, itemName);

    cout << "Enter item price: ";
    while (!(cin >> itemPrice)) {
        cout << "Error: Invalid input. Please enter a valid price: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // Save to menu.txt
    menuFile << itemName << " " << itemPrice << endl;
    cout << itemName << " added to the menu for " << itemPrice << " lv.\n";

    menuFile.close();

    // Add recipe for the item
    cout << "Now enter the ingredients for " << itemName << " (type 'done' when finished):\n";

    string ingredient;
    int quantity;
    string recipeLine = itemName;

    while (true) {
        cout << "Enter ingredient name (or 'done' to finish): ";
        cin >> ingredient;

        if (ingredient == "done") {
            break;
        }

        cout << "Enter quantity required: ";
        while (!(cin >> quantity) || quantity <= 0) {
            cout << "Error: Invalid quantity. Enter a positive number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        recipeLine += " " + ingredient + " " + to_string(quantity);
    }

    recipesFile << recipeLine << endl;
    cout << "Recipe for " << itemName << " added to recipes.\n";

    recipesFile.close();
}

struct MenuItem {
    string name;
    double price;
};
// Function to remove an item from the menu
void removeItemFromMenu() {
    ifstream menuFile("menu.txt");
    ifstream recipesFile("recipes.txt");
    ofstream tempMenuFile("temp_menu.txt");
    ofstream tempRecipesFile("temp_recipes.txt");

    if (!menuFile || !recipesFile || !tempMenuFile || !tempRecipesFile) {
        cout << "Error: Unable to open necessary files.\n";
        return;
    }

    string itemName;
    cout << "Enter item name to remove: ";
    cin >> ws;  // Remove leading whitespace
    getline(cin, itemName);  // Allow multi-word item names

    string currentItem;
    double price;
    bool itemFound = false;

    // Remove from menu.txt
    while (menuFile >> currentItem >> price) {
        if (currentItem == itemName) {
            itemFound = true;
            cout << currentItem << " removed from the menu.\n";
        }
        else {
            tempMenuFile << currentItem << " " << price << endl;
        }
    }

    menuFile.close();
    tempMenuFile.close();

    // Remove from recipes.txt
    string recipeLine;
    while (getline(recipesFile, recipeLine)) {
        size_t pos = recipeLine.find(' ');
        if (pos != string::npos) {
            string recipeProduct = recipeLine.substr(0, pos);
            if (recipeProduct == itemName) {
                cout << "Recipe for " << itemName << " removed from recipes.\n";
                continue;  // Skip this line (do not write it to the new file)
            }
        }
        tempRecipesFile << recipeLine << endl;
    }

    recipesFile.close();
    tempRecipesFile.close();

    // Replace old files with updated ones
    remove("menu.txt");
    rename("temp_menu.txt", "menu.txt");

    remove("recipes.txt");
    rename("temp_recipes.txt", "recipes.txt");

    if (!itemFound) {
        cout << "Error: Item \"" << itemName << "\" not found in the menu.\n";
    }
}