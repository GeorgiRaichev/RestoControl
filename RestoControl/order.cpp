#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

using namespace std;

// ==============================
// Constants
// ==============================
const int MAX_INGREDIENTS = 100;
const int MAX_ORDERS = 100;

// ==============================
// Helper Function Declarations
// ==============================

// For addOrder function
int determineNextOrderID();
bool findMenuItem(ifstream& menuFile, const string& itemName, double& price);
bool loadRecipe(const string& itemName, ifstream& recipesFile,
	string recipeIngredients[], int recipeQuantities[], int& recipeCount);
bool readInventory(string inventoryNames[], int inventoryValues[], int& inventoryCount);
bool checkSufficientStock(const string recipeIngredients[], const int recipeQuantities[], int recipeCount,
	const string inventoryNames[], const int inventoryValues[], int inventoryCount);
void deductIngredients(const string recipeIngredients[], const int recipeQuantities[], int recipeCount,
	int inventoryValues[], int inventoryCount, const string inventoryNames[]);
void writeInventory(const string inventoryNames[], const int inventoryValues[], int inventoryCount);
bool appendOrder(int orderID, const string& itemName, double price);

// For cancelOrder function
bool findAndRemoveOrder(int orderID, string& canceledItem, double& canceledPrice, string& currency);
void restoreInventory(const string recipeIngredients[], const int recipeQuantities[], int recipeCount);

// For viewSortedOrders function
bool readAndAggregateOrders(const std::string& filename,
	std::string orderNames[], int orderCounts[], int& orderCount);
void sortOrders(std::string orderNames[], int orderCounts[], int orderCount);
void displaySortedOrders(const std::string orderNames[], const int orderCounts[], int orderCount);

// For report generation functions
bool calculateTotalRevenue(const std::string& ordersFilename, double& totalRevenue);
void getCurrentDate(int& day, int& month, int& year);
bool appendReport(const std::string& reportFilename, int day, int month, int year, double totalRevenue);
bool clearFile(const std::string& filename);

// ==============================
// Main Functions
// ==============================

// Function to add an order
void addOrder() {
	ifstream menuFile("menu.txt");
	ifstream recipesFile("recipes.txt");
	if (!menuFile || !recipesFile) {
		cout << "Error: Unable to open necessary files.\n";
		return;
	}

	// Determine the next order ID
	int orderID = determineNextOrderID();

	string itemName;
	cout << "Enter item name to order: ";
	cin >> itemName;

	double price = 0.0;
	bool itemFound = findMenuItem(menuFile, itemName, price);
	menuFile.close();

	if (!itemFound) {
		cout << "Error: " << itemName << " is not available in the menu. Order canceled.\n";
		recipesFile.close();
		return;
	}

	string recipeIngredients[MAX_INGREDIENTS];
	int recipeQuantities[MAX_INGREDIENTS];
	int recipeCount = 0;

	// Load recipe for the item
	bool recipeLoaded = loadRecipe(itemName, recipesFile, recipeIngredients, recipeQuantities, recipeCount);
	recipesFile.close();

	if (!recipeLoaded) {
		cout << "Error: Recipe for " << itemName << " not found. Order canceled.\n";
		return;
	}

	string inventoryNames[MAX_INGREDIENTS];
	int inventoryValues[MAX_INGREDIENTS];
	int inventoryCount = 0;

	// Read current inventory
	bool inventoryRead = readInventory(inventoryNames, inventoryValues, inventoryCount);
	if (!inventoryRead) {
		cout << "Error: Unable to read inventory.\n";
		return;
	}

	// Check if inventory has sufficient stock for the recipe
	bool sufficientStock = checkSufficientStock(recipeIngredients, recipeQuantities, recipeCount,
		inventoryNames, inventoryValues, inventoryCount);
	if (!sufficientStock) {
		cout << "Order canceled due to insufficient inventory.\n";
		return;
	}

	// Deduct used ingredients from inventory
	deductIngredients(recipeIngredients, recipeQuantities, recipeCount,
		inventoryValues, inventoryCount, inventoryNames);

	// Write updated inventory to file
	writeInventory(inventoryNames, inventoryValues, inventoryCount);

	// Append new order to orders file
	if (appendOrder(orderID, itemName, price)) {
		cout << "Order ID " << orderID << " - " << itemName << " added to orders for " << price << " lv.\n";
	}
}

// ==============================
// Helper Function Definitions for addOrder
// ==============================

int determineNextOrderID() {
	ifstream ordersFile("orders.txt");
	int orderID = 1;
	string line;
	while (getline(ordersFile, line)) {
		orderID++;
	}
	ordersFile.close();
	return orderID;
}

bool findMenuItem(ifstream& menuFile, const string& itemName, double& price) {
	string menuItem;
	while (menuFile >> menuItem >> price) {
		if (menuItem == itemName) {
			return true;
		}
	}
	return false;
}

bool loadRecipe(const string& itemName, ifstream& recipesFile,
	string recipeIngredients[], int recipeQuantities[], int& recipeCount) {
	string recipeLine;
	recipeCount = 0;
	while (getline(recipesFile, recipeLine)) {
		size_t pos = recipeLine.find(' ');
		string recipeProduct = recipeLine.substr(0, pos);
		if (recipeProduct == itemName) {
			size_t start = pos + 1;
			while (start < recipeLine.size()) {
				pos = recipeLine.find(' ', start);
				if (pos == string::npos) break;

				string ingredient = recipeLine.substr(start, pos - start);
				start = pos + 1;

				pos = recipeLine.find(' ', start);
				if (pos == string::npos) {
					string quantityStr = recipeLine.substr(start);
					int quantity = stoi(quantityStr);
					if (recipeCount < MAX_INGREDIENTS) {
						recipeIngredients[recipeCount] = ingredient;
						recipeQuantities[recipeCount] = quantity;
						recipeCount++;
					}
					break;
				}

				string quantityStr = recipeLine.substr(start, pos - start);
				int quantity = stoi(quantityStr);
				start = pos + 1;

				if (recipeCount < MAX_INGREDIENTS) {
					recipeIngredients[recipeCount] = ingredient;
					recipeQuantities[recipeCount] = quantity;
					recipeCount++;
				}
			}
			return true;
		}
	}
	return false;
}

bool readInventory(string inventoryNames[], int inventoryValues[], int& inventoryCount) {
	ifstream inventoryFile("inventory.txt");
	if (!inventoryFile) return false;
	inventoryCount = 0;
	while (inventoryFile >> inventoryNames[inventoryCount] >> inventoryValues[inventoryCount]) {
		inventoryCount++;
	}
	inventoryFile.close();
	return true;
}

bool checkSufficientStock(const string recipeIngredients[], const int recipeQuantities[], int recipeCount,
	const string inventoryNames[], const int inventoryValues[], int inventoryCount) {
	bool sufficientStock = true;
	for (int i = 0; i < recipeCount; i++) {
		bool found = false;
		for (int j = 0; j < inventoryCount; j++) {
			if (recipeIngredients[i] == inventoryNames[j]) {
				found = true;
				if (inventoryValues[j] < recipeQuantities[i]) {
					cout << "Error: Not enough " << recipeIngredients[i]
						<< " in stock for " << recipeIngredients[i] << ".\n";
					sufficientStock = false;
				}
			}
		}
		if (!found) {
			cout << "Error: Ingredient " << recipeIngredients[i]
				<< " not found in inventory. Order canceled.\n";
			sufficientStock = false;
		}
	}
	return sufficientStock;
}

void deductIngredients(const string recipeIngredients[], const int recipeQuantities[], int recipeCount,
	int inventoryValues[], int inventoryCount, const string inventoryNames[]) {
	for (int i = 0; i < recipeCount; i++) {
		for (int j = 0; j < inventoryCount; j++) {
			if (recipeIngredients[i] == inventoryNames[j]) {
				inventoryValues[j] -= recipeQuantities[i];
			}
		}
	}
}

void writeInventory(const string inventoryNames[], const int inventoryValues[], int inventoryCount) {
	ofstream updatedInventory("inventory.txt");
	for (int i = 0; i < inventoryCount; i++) {
		updatedInventory << inventoryNames[i] << " " << inventoryValues[i] << endl;
	}
	updatedInventory.close();
}

bool appendOrder(int orderID, const string& itemName, double price) {
	ofstream orderFile("orders.txt", ios::app);
	if (!orderFile) {
		cout << "Error: Unable to open orders file for writing.\n";
		return false;
	}
	orderFile << orderID << " " << itemName << " " << price << " lv." << endl;
	orderFile.close();
	return true;
}

// ==============================
// Helper Function Definitions for cancelOrder
// ==============================

bool findAndRemoveOrder(int orderID, string& canceledItem, double& canceledPrice, string& currency) {
	ifstream orderFile("orders.txt");
	ofstream tempOrderFile("temp_orders.txt");
	if (!orderFile || !tempOrderFile) {
		cout << "Error: Unable to open orders files.\n";
		return false;
	}

	bool orderFound = false;
	int currentID;
	while (orderFile >> currentID >> canceledItem >> canceledPrice >> currency) {
		if (currentID == orderID && !orderFound) {
			orderFound = true;
		}
		else {
			tempOrderFile << currentID << " " << canceledItem << " "
				<< canceledPrice << " " << currency << endl;
		}
	}

	orderFile.close();
	tempOrderFile.close();
	return orderFound;
}

void restoreInventory(const string recipeIngredients[], const int recipeQuantities[], int recipeCount) {
	const int MAX_ITEMS = 50;
	string inventoryItems[MAX_ITEMS];
	int inventoryQuantities[MAX_ITEMS];
	int inventoryCount = 0;

	if (!readInventory(inventoryItems, inventoryQuantities, inventoryCount)) {
		cout << "Error: Inventory file not found.\n";
		return;
	}

	for (int i = 0; i < recipeCount; i++) {
		bool found = false;
		for (int j = 0; j < inventoryCount; j++) {
			if (recipeIngredients[i] == inventoryItems[j]) {
				inventoryQuantities[j] += recipeQuantities[i];
				found = true;
				break;
			}
		}
		if (!found) {
			cout << "Warning: Ingredient " << recipeIngredients[i]
				<< " not found in inventory. It will not be restored.\n";
		}
	}

	writeInventory(inventoryItems, inventoryQuantities, inventoryCount);
}

void cancelOrder() {
	int orderID;
	cout << "Enter order ID to cancel: ";

	while (!(cin >> orderID)) {
		cout << "Error: Invalid input. Please enter a numeric order ID: ";
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}

	string canceledItem, currency;
	double canceledPrice;
	bool orderFound = findAndRemoveOrder(orderID, canceledItem, canceledPrice, currency);

	if (!orderFound) {
		cout << "Error: Order ID " << orderID << " not found.\n";
		remove("temp_orders.txt");
		return;
	}

	cout << "Order ID " << orderID << " (" << canceledItem << ") canceled successfully.\n";

	string recipeIngredients[MAX_INGREDIENTS];
	int recipeQuantities[MAX_INGREDIENTS];
	int recipeCount = 0;

	ifstream recipesFile("recipes.txt");
	if (!recipesFile) {
		cout << "Error: Recipes file not found.\n";
		return;
	}
	loadRecipe(canceledItem, recipesFile, recipeIngredients, recipeQuantities, recipeCount);
	recipesFile.close();

	restoreInventory(recipeIngredients, recipeQuantities, recipeCount);

	remove("orders.txt");
	rename("temp_orders.txt", "orders.txt");
}

// ==============================
// Functions for Viewing Orders and Revenue
// ==============================

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

void viewSortedOrders() {
	std::string orderNames[MAX_ORDERS];
	int orderCounts[MAX_ORDERS] = { 0 };
	int orderCount = 0;

	if (!readAndAggregateOrders("orders.txt", orderNames, orderCounts, orderCount)) {
		return;
	}

	if (orderCount == 0) {
		cout << "No orders found.\n";
		return;
	}

	sortOrders(orderNames, orderCounts, orderCount);
	displaySortedOrders(orderNames, orderCounts, orderCount);
}

bool readAndAggregateOrders(const std::string& filename,
	std::string orderNames[], int orderCounts[], int& orderCount) {
	ifstream orderFile(filename);
	if (!orderFile) {
		cout << "Error: Orders file not found.\n";
		return false;
	}

	orderCount = 0;
	int orderID;
	std::string item;
	double price;
	std::string currency;

	while (orderFile >> orderID >> item >> price >> currency) {
		if (currency != "lv.") {
			cout << "Warning: Incorrect format in orders file. Skipping entry: "
				<< item << " " << price << " " << currency << endl;
			continue;
		}

		bool found = false;
		for (int i = 0; i < orderCount; i++) {
			if (orderNames[i] == item) {
				orderCounts[i]++;
				found = true;
				break;
			}
		}

		if (!found) {
			if (orderCount >= MAX_ORDERS) {
				cout << "Error: Maximum order limit reached (" << MAX_ORDERS << ").\n";
				break;
			}
			orderNames[orderCount] = item;
			orderCounts[orderCount] = 1;
			orderCount++;
		}
	}

	orderFile.close();
	return true;
}

void sortOrders(std::string orderNames[], int orderCounts[], int orderCount) {
	for (int i = 0; i < orderCount - 1; i++) {
		int minIndex = i;
		for (int j = i + 1; j < orderCount; j++) {
			if (orderNames[j] < orderNames[minIndex]) {
				minIndex = j;
			}
		}
		if (minIndex != i) {
			std::string tempName = orderNames[i];
			orderNames[i] = orderNames[minIndex];
			orderNames[minIndex] = tempName;

			int tempCount = orderCounts[i];
			orderCounts[i] = orderCounts[minIndex];
			orderCounts[minIndex] = tempCount;
		}
	}
}

void displaySortedOrders(const std::string orderNames[], const int orderCounts[], int orderCount) {
	cout << "\n--- Sorted Orders ---\n";
	for (int i = 0; i < orderCount; i++) {
		cout << orderNames[i] << " - " << orderCounts[i] << " orders\n";
	}
}

void viewDailyRevenue() {
	ifstream orderFile("orders.txt");

	if (!orderFile) {
		cout << "Error: Orders file not found.\n";
		return;
	}

	int orderID;
	string item;
	double price;
	string currency;
	double totalRevenue = 0;

	while (orderFile >> orderID >> item >> price >> currency) {
		if (currency != "lv.") {
			cout << "Warning: Unexpected currency '" << currency << "' for item '" << item << "'. Skipping entry.\n";
			continue;
		}
		totalRevenue += price;
	}

	orderFile.close();

	cout << "\n--- Daily Revenue ---\n";
	cout << "Total Revenue: " << totalRevenue << " lv.\n";
}

void generateReport() {
	double totalRevenue = 0.0;

	if (!calculateTotalRevenue("orders.txt", totalRevenue)) {
		return;
	}

	int day, month, year;
	getCurrentDate(day, month, year);

	if (!appendReport("report.txt", day, month, year, totalRevenue)) {
		return;
	}

	if (!clearFile("orders.txt")) {
		return;
	}

	cout << "\n--- Report Generated ---\n";
	cout << "Total Revenue for " << day << "/" << month << "/" << year
		<< ": " << totalRevenue << " lv.\n";
	cout << "Orders have been cleared.\n";
}

bool calculateTotalRevenue(const std::string& ordersFilename, double& totalRevenue) {
	ifstream orderFile(ordersFilename);
	if (!orderFile) {
		cout << "Error: Orders file not found.\n";
		return false;
	}

	int orderID;
	std::string item;
	double price;
	std::string currency;
	totalRevenue = 0.0;

	while (orderFile >> orderID >> item >> price >> currency) {
		if (currency != "lv.") {
			cout << "Warning: Unexpected currency '" << currency
				<< "' for item '" << item << "'. Skipping entry.\n";
			continue;
		}
		totalRevenue += price;
	}
	orderFile.close();
	return true;
}

void getCurrentDate(int& day, int& month, int& year) {
	time_t now = time(0);
	tm ltm;
	localtime_s(&ltm, &now);
	day = ltm.tm_mday;
	month = 1 + ltm.tm_mon;
	year = 1900 + ltm.tm_year;
}

bool appendReport(const std::string& reportFilename, int day, int month, int year, double totalRevenue) {
	ofstream reportFile(reportFilename, ios::app);
	if (!reportFile) {
		cout << "Error: Unable to create or open report file.\n";
		return false;
	}

	reportFile << "Date: " << day << "/" << month << "/" << year
		<< " - Total Revenue: " << totalRevenue << " lv.\n";
	reportFile.close();
	return true;
}

bool clearFile(const std::string& filename) {
	ofstream file(filename, ofstream::trunc);
	if (!file) {
		cout << "Error: Unable to clear file " << filename << ".\n";
		return false;
	}
	file.close();
	return true;
}

void viewTotalRevenueByDate() {
	ifstream reportFile("report.txt");

	if (!reportFile) {
		cout << "Error: Report file not found.\n";
		return;
	}

	int startDay, startMonth, startYear;
	char slash;

	cout << "Enter start date (dd/mm/yyyy): ";
	cin >> startDay >> slash >> startMonth >> slash >> startYear;

	string line;
	bool found = false;

	cout << "\n--- Reports from " << startDay << "/" << startMonth << "/" << startYear << " to now ---\n";

	while (getline(reportFile, line)) {
		int reportDay, reportMonth, reportYear;
		double revenue;

		sscanf_s(line.c_str(), "Date: %d/%d/%d - Total Revenue: %lf lv.",
			&reportDay, &reportMonth, &reportYear, &revenue);

		if (reportYear > startYear ||
			(reportYear == startYear && reportMonth > startMonth) ||
			(reportYear == startYear && reportMonth == startMonth && reportDay >= startDay)) {
			cout << line << endl;
			found = true;
		}
	}

	if (!found) {
		cout << "No reports found from this date.\n";
	}

	reportFile.close();
}
