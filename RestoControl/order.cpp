#include <iostream>
#include <fstream>
#include <string>
#include <ctime> 

using namespace std;

// Function to add an order
void addOrder() {
	ifstream menuFile("menu.txt");
	ofstream orderFile("orders.txt", ios::app);  // Append mode to avoid overwriting

	if (!menuFile) {
		cout << "Error: Menu file not found.\n";
		return;
	}

	string itemName;
	cout << "Enter item name to order: ";
	cin >> itemName;

	string menuItem;
	double price;
	bool itemFound = false;

	// Search for the item in the menu
	while (menuFile >> menuItem >> price) {
		if (menuItem == itemName) {
			itemFound = true;
			orderFile << itemName << " " << price << " lv." << endl;
			cout << itemName << " added to orders for " << price << " lv.\n";
			break;
		}
	}

	if (!itemFound) {
		cout << "Error: " << itemName << " is not available in the menu. Order canceled.\n";
	}

	menuFile.close();
	orderFile.close();
}

// Function to cancel an order
void cancelOrder() {
	// First pass: Find the line number of the last order for the given item
	ifstream orderFile("orders.txt");
	if (!orderFile) {
		cout << "Error: Orders file not found.\n";
		return;
	}

	string itemName;
	cout << "Enter item name to cancel: ";
	cin >> itemName;

	string line;
	int lineNumber = 0;
	int lastOccurrenceLine = -1;

	while (getline(orderFile, line)) {
		lineNumber++;
		// It is assumed that the item name is the first word in the line
		size_t pos = line.find(' ');
		if (pos != string::npos) {
			string currentItem = line.substr(0, pos);
			if (currentItem == itemName) {
				lastOccurrenceLine = lineNumber;
			}
		}
	}
	orderFile.close();

	if (lastOccurrenceLine == -1) {
		cout << "Error: Order for " << itemName << " not found.\n";
		return;
	}

	// Second pass: Copy all lines to a temporary file, skipping the last occurrence
	ifstream orderFileSecondPass("orders.txt");
	if (!orderFileSecondPass) {
		cout << "Error: Orders file not found on second pass.\n";
		return;
	}

	ofstream tempFile("temp.txt");
	if (!tempFile) {
		cout << "Error: Unable to create temp file.\n";
		return;
	}

	int currentLine = 0;
	bool canceled = false;
	while (getline(orderFileSecondPass, line)) {
		currentLine++;
		if (currentLine == lastOccurrenceLine && !canceled) {
			// Skip this line
			cout << itemName << " order canceled successfully.\n";
			canceled = true;
			continue;
		}
		tempFile << line << endl;
	}

	orderFileSecondPass.close();
	tempFile.close();

	// Replace the old file with the updated one
	if (remove("orders.txt") != 0) {
		cout << "Error: Failed to remove old orders file.\n";
		return;
	}

	if (rename("temp.txt", "orders.txt") != 0) {
		cout << "Error: Failed to rename temp.txt to orders.txt.\n";
		return;
	}
}

// Function to view all past orders
void viewOrders() {
	ifstream orderFile("orders.txt");

	if (!orderFile) {
		cout << "Error: Orders file not found.\n";
		return;
	}

	string line;
	bool hasOrders = false;

	cout << "\n--- Past Orders ---\n";
	while (getline(orderFile, line)) {
		cout << line << endl;
		hasOrders = true;
	}

	if (!hasOrders) {
		cout << "No orders found.\n";
	}

	orderFile.close();
}

struct OrderItem {
	string name;
	int count = 0;
};
