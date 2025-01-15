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

// Function to sort orders alphabetically (Selection Sort)
void sortOrders(OrderItem orders[], int orderCount) {
	for (int i = 0; i < orderCount - 1; i++) {
		int minIndex = i;
		for (int j = i + 1; j < orderCount; j++) {
			if (orders[j].name < orders[minIndex].name) {
				minIndex = j;
			}
		}
		// Swap if a smaller element is found
		if (minIndex != i) {
			OrderItem temp = orders[i];
			orders[i] = orders[minIndex];
			orders[minIndex] = temp;
		}
	}
}

void viewSortedOrders() {
	ifstream orderFile("orders.txt");

	if (!orderFile) {
		cout << "Error: Orders file not found.\n";
		return;
	}

	OrderItem orders[100];  // Array to store orders
	int orderCount = 0;

	string item;
	double price;
	string currency;  

	// Read orders and add to the array
	while (orderFile >> item >> price >> currency) {
		bool found = false;

		// Check if the item is already added
		for (int i = 0; i < orderCount; i++) {
			if (orders[i].name == item) {
				orders[i].count++;
				found = true;
				break;
			}
		}

		// Add new item if not found
		if (!found) {
			if (orderCount >= 100) {
				cout << "Error: Maximum order limit reached (100).\n";
				break;
			}
			orders[orderCount].name = item;
			orders[orderCount].count = 1;
			orderCount++;
		}
	}

	orderFile.close();

	if (orderCount == 0) {
		cout << "No orders found.\n";
		return;
	}

	// Sort the items alphabetically
	sortOrders(orders, orderCount);

	// Display the result
	cout << "\n--- Sorted Orders ---\n";
	for (int i = 0; i < orderCount; i++) {
		cout << orders[i].name << " - " << orders[i].count << " orders\n";
	}
}
// Function to view daily revenue
void viewDailyRevenue() {
	// Open the orders file for reading
	ifstream orderFile("orders.txt");

	if (!orderFile) {
		cout << "Error: Orders file not found.\n";
		return;
	}

	string item;
	double price;
	string currency;
	double totalRevenue = 0;

	// Read orders and sum the prices
	while (orderFile >> item >> price >> currency) {
		// Validate that the currency is "lv."
		if (currency != "lv.") {
			cout << "Warning: Unexpected currency '" << currency << "' for item '" << item << "'. Skipping this entry.\n";
			continue;
		}

		totalRevenue += price;
	}

	// Check for any read errors (optional)
	if (orderFile.bad()) {
		cout << "Error: An I/O error occurred while reading the orders file.\n";
		orderFile.close();
		return;
	}

	orderFile.close();

	// Display the result
	cout << "\n--- Daily Revenue ---\n";
	cout << "Total Revenue: " << totalRevenue << " lv.\n";
}