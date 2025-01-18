#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// ==============================
// Constants
// ==============================
const int MAX_INGREDIENTS = 100;
const int MAX_ORDERS = 100;

// ==============================
// Helper Functions for displayMenu
// ==============================
void centerHeader(const string& header, int totalWidth) {
    int pad = (totalWidth - static_cast<int>(header.length())) / 2;
    for (int i = 0; i < pad; ++i) {
        cout << " ";
    }
    cout << header << "\n";
}

void printMenuHeader() {
    cout.setf(ios::left, ios::adjustfield);
    cout.width(20);
    cout << "Item";
    cout.setf(ios::right, ios::adjustfield);
    cout.width(10);
    cout << "Price (lv.)" << endl;
    cout << "-------------------------------\n";
}

void printMenuItems(ifstream& file) {
    string item;
    double price;

    cout.setf(ios::fixed, ios::floatfield);
    cout.precision(2);

    while (file >> item >> price) {
        cout.setf(ios::left, ios::adjustfield);
        cout.width(20);
        cout << item;

        cout.setf(ios::right, ios::adjustfield);
        cout.width(8);
        cout << price << " lv." << endl;
    }
}

// ==============================
// displayMenu Function
// ==============================
void displayMenu() {
    ifstream file("menu.txt");
    if (!file) {
        cout << "Error: Menu file not found.\n";
        return;
    }

    int totalWidth = 30;
    string header = "--- Current Menu ---";

    cout << "\n";
    centerHeader(header, totalWidth);

    printMenuHeader();
    printMenuItems(file);

    file.close();
}

// ==============================
// Helper Functions for addItemToMenu
// ==============================
void getItemDetails(string& itemName, double& itemPrice) {
    cout << "Enter item name: ";
    cin >> ws;
    getline(cin, itemName);

    cout << "Enter item price: ";
    while (!(cin >> itemPrice)) {
        cout << "Error: Invalid input. Please enter a valid price: ";
        cin.clear();
        cin.ignore(10000, '\n');  
    }
}

void addRecipeForItem(ofstream& recipesFile, const string& itemName) {
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
            cin.ignore(10000, '\n');  
        }

        recipeLine += " " + ingredient + " " + to_string(quantity);
    }

    recipesFile << recipeLine << endl;
    cout << "Recipe for " << itemName << " added to recipes.\n";
}

// ==============================
// addItemToMenu Function
// ==============================
void addItemToMenu() {
    ofstream menuFile("menu.txt", ios::app);
    ofstream recipesFile("recipes.txt", ios::app);

    if (!menuFile || !recipesFile) {
        cout << "Error: Unable to open menu or recipes file.\n";
        return;
    }

    string itemName;
    double itemPrice;

    getItemDetails(itemName, itemPrice);

    menuFile << itemName << " " << itemPrice << endl;
    cout << itemName << " added to the menu for " << itemPrice << " lv.\n";
    menuFile.close();

    addRecipeForItem(recipesFile, itemName);
    recipesFile.close();
}

// ==============================
// Helper Functions for removeItemFromMenu
// ==============================
void removeItemFromMenuFiles(const string& itemName) {
    ifstream menuFile("menu.txt");
    ifstream recipesFile("recipes.txt");
    ofstream tempMenuFile("temp_menu.txt");
    ofstream tempRecipesFile("temp_recipes.txt");

    if (!menuFile || !recipesFile || !tempMenuFile || !tempRecipesFile) {
        cout << "Error: Unable to open necessary files.\n";
        return;
    }

    string currentItem;
    double price;
    bool itemFound = false;

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

    string recipeLine;
    while (getline(recipesFile, recipeLine)) {
        size_t pos = recipeLine.find(' ');
        if (pos != string::npos) {
            string recipeProduct = recipeLine.substr(0, pos);
            if (recipeProduct == itemName) {
                cout << "Recipe for " << itemName << " removed from recipes.\n";
                continue;
            }
        }
        tempRecipesFile << recipeLine << endl;
    }
    recipesFile.close();
    tempRecipesFile.close();

    if (remove("menu.txt") != 0) {
        cout << "Warning: Failed to remove menu.txt.\n";
    }
    if (rename("temp_menu.txt", "menu.txt") != 0) {
        cout << "Error: Failed to rename temp_menu.txt to menu.txt.\n";
    }

    if (remove("recipes.txt") != 0) {
        cout << "Warning: Failed to remove recipes.txt.\n";
    }
    if (rename("temp_recipes.txt", "recipes.txt") != 0) {
        cout << "Error: Failed to rename temp_recipes.txt to recipes.txt.\n";
    }


    if (!itemFound) {
        cout << "Error: Item \"" << itemName << "\" not found in the menu.\n";
    }
}

// ==============================
// removeItemFromMenu Function
// ==============================
void removeItemFromMenu() {
    cout << "Enter item name to remove: ";
    string itemName;
    cin >> ws;
    getline(cin, itemName);

    removeItemFromMenuFiles(itemName);
}
