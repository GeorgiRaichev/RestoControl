#include <iostream>
#include <fstream>
#include <string>
#include <ctime> 

using namespace std;

// Function to add an order
void addOrder() {
	ifstream menuFile("menu.txt");
	ifstream recipesFile("recipes.txt");
	ifstream inventoryFile("inventory.txt");
	ofstream tempInventory("temp_inventory.txt");
	ifstream ordersFile("orders.txt");

	if (!menuFile || !recipesFile || !inventoryFile || !tempInventory) {
		cout << "Error: Unable to open necessary files.\n";
		return;
	}

	// Determine the next order ID
	int orderID = 1;
	string line;
	while (getline(ordersFile, line)) {
		orderID++;
	}
	ordersFile.close();

	string itemName;
	cout << "Enter item name to order: ";
	cin >> itemName;

	string menuItem;
	double price;
	bool itemFound = false;

	// Check if the item exists in the menu
	while (menuFile >> menuItem >> price) {
		if (menuItem == itemName) {
			itemFound = true;

			const int MAX_INGREDIENTS = 20;
			string recipeIngredients[MAX_INGREDIENTS];
			int recipeQuantities[MAX_INGREDIENTS];
			int recipeCount = 0;

			string recipeLine;
			bool sufficientStock = true;

			// Find the recipe for the item
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
							// Last ingredient in the line (fixing the issue)
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
					break;
				}
			}

			inventoryFile.close();
			inventoryFile.open("inventory.txt");

			string inventoryItem;
			int inventoryQuantity;
			string inventoryNames[MAX_INGREDIENTS];
			int inventoryValues[MAX_INGREDIENTS];
			int inventoryCount = 0;

			// Read inventory and store all ingredients in memory
			while (inventoryFile >> inventoryItem >> inventoryQuantity) {
				inventoryNames[inventoryCount] = inventoryItem;
				inventoryValues[inventoryCount] = inventoryQuantity;
				inventoryCount++;
			}
			inventoryFile.close();

			// Check if all ingredients are available
			for (int i = 0; i < recipeCount; i++) {
				bool found = false;
				for (int j = 0; j < inventoryCount; j++) {
					if (recipeIngredients[i] == inventoryNames[j]) {
						found = true;
						if (inventoryValues[j] < recipeQuantities[i]) {
							cout << "Error: Not enough " << recipeIngredients[i] << " in stock for " << itemName << ".\n";
							sufficientStock = false;
						}
					}
				}
				if (!found) {
					cout << "Error: Ingredient " << recipeIngredients[i] << " not found in inventory. Order canceled.\n";
					sufficientStock = false;
				}
			}

			if (!sufficientStock) {
				cout << "Order canceled due to insufficient inventory.\n";
				menuFile.close();
				recipesFile.close();
				tempInventory.close();
				remove("temp_inventory.txt");
				return;
			}

			// Deduct ingredients from inventory
			for (int i = 0; i < recipeCount; i++) {
				for (int j = 0; j < inventoryCount; j++) {
					if (recipeIngredients[i] == inventoryNames[j]) {
						inventoryValues[j] -= recipeQuantities[i]; // Deduct all ingredients
					}
				}
			}

			// Save updated inventory back to file
			ofstream updatedInventory("inventory.txt");
			for (int i = 0; i < inventoryCount; i++) {
				updatedInventory << inventoryNames[i] << " " << inventoryValues[i] << endl;
			}
			updatedInventory.close();

			ofstream orderFile("orders.txt", ios::app);
			if (!orderFile) {
				cout << "Error: Unable to open orders file for writing.\n";
				return;
			}

			orderFile << orderID << " " << itemName << " " << price << " lv." << endl;
			orderFile.close();

			cout << "Order ID " << orderID << " - " << itemName << " added to orders for " << price << " lv.\n";
			break;
		}
	}

	if (!itemFound) {
		cout << "Error: " << itemName << " is not available in the menu. Order canceled.\n";
	}

	menuFile.close();
	recipesFile.close();
}


// Function to cancel an order
void cancelOrder() {
	ifstream orderFile("orders.txt");
	if (!orderFile) {
		cout << "Error: Orders file not found.\n";
		return;
	}

	int orderID;
	cout << "Enter order ID to cancel: ";
	cin >> orderID;

	string line;
	bool orderFound = false;

	ifstream recipesFile("recipes.txt");
	if (!recipesFile) {
		cout << "Error: Recipes file not found.\n";
		return;
	}

	ofstream tempOrderFile("temp_orders.txt");
	ofstream tempInventoryFile("temp_inventory.txt");
	if (!tempOrderFile || !tempInventoryFile) {
		cout << "Error: Unable to create temporary files.\n";
		return;
	}

	string canceledItem;
	double canceledPrice;
	string currency;
	int currentID;

	const int MAX_INGREDIENTS = 20;
	string recipeIngredients[MAX_INGREDIENTS];
	int recipeQuantities[MAX_INGREDIENTS];
	int recipeCount = 0;

	// Read orders and find the one to cancel
	while (orderFile >> currentID >> canceledItem >> canceledPrice >> currency) {
		if (currentID == orderID && !orderFound) {
			orderFound = true;
			cout << "Order ID " << orderID << " (" << canceledItem << ") canceled successfully.\n";

			// Find the recipe for the canceled item
			string recipeLine;
			while (getline(recipesFile, recipeLine)) {
				size_t pos = recipeLine.find(' ');
				string recipeProduct = recipeLine.substr(0, pos);

				if (recipeProduct == canceledItem) {
					size_t start = pos + 1;
					while (start < recipeLine.size()) {
						pos = recipeLine.find(' ', start);
						if (pos == string::npos) {
							// Fix for last ingredient in recipe
							string ingredient = recipeLine.substr(start);
							size_t lastSpace = ingredient.rfind(' ');
							string quantityStr = ingredient.substr(lastSpace + 1);
							ingredient = ingredient.substr(0, lastSpace);
							int quantity = stoi(quantityStr);

							if (recipeCount < MAX_INGREDIENTS) {
								recipeIngredients[recipeCount] = ingredient;
								recipeQuantities[recipeCount] = quantity;
								recipeCount++;
							}
							break;
						}

						string ingredient = recipeLine.substr(start, pos - start);
						start = pos + 1;

						pos = recipeLine.find(' ', start);
						if (pos == string::npos) {
							// Handle last ingredient separately
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
					break;
				}
			}
		}
		else {
			tempOrderFile << currentID << " " << canceledItem << " " << canceledPrice << " " << currency << endl;
		}
	}

	orderFile.close();
	recipesFile.close();
	tempOrderFile.close();

	if (!orderFound) {
		cout << "Error: Order ID " << orderID << " not found.\n";
		remove("temp_orders.txt");
		return;
	}

	ifstream inventoryFile("inventory.txt");
	if (!inventoryFile) {
		cout << "Error: Inventory file not found.\n";
		return;
	}

	string inventoryItem;
	int inventoryQuantity;
	const int MAX_ITEMS = 50;
	string inventoryItems[MAX_ITEMS];
	int inventoryQuantities[MAX_ITEMS];
	int inventoryCount = 0;

	// Read inventory into arrays
	while (inventoryFile >> inventoryItem >> inventoryQuantity) {
		inventoryItems[inventoryCount] = inventoryItem;
		inventoryQuantities[inventoryCount] = inventoryQuantity;
		inventoryCount++;
	}
	inventoryFile.close();

	// Restore ingredients to inventory (Loop through all)
	for (int i = 0; i < recipeCount; i++) {
		bool found = false;
		for (int j = 0; j < inventoryCount; j++) {
			if (recipeIngredients[i] == inventoryItems[j]) {
				inventoryQuantities[j] += recipeQuantities[i]; // Restore quantity
				found = true;
				break;
			}
		}
		if (!found) {
			cout << "Warning: Ingredient " << recipeIngredients[i] << " not found in inventory. It will not be restored.\n";
		}
	}

	// Write updated inventory back to file
	for (int i = 0; i < inventoryCount; i++) {
		tempInventoryFile << inventoryItems[i] << " " << inventoryQuantities[i] << endl;
	}

	tempInventoryFile.close();

	// Replace original files
	remove("orders.txt");
	rename("temp_orders.txt", "orders.txt");
	remove("inventory.txt");
	rename("temp_inventory.txt", "inventory.txt");
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

	const int MAX_ORDERS = 100;
	string orderNames[MAX_ORDERS];
	int orderCounts[MAX_ORDERS];
	int orderCount = 0;

	int orderID;
	string item;
	double price;
	string currency;

	// Read orders and extract item names correctly
	while (orderFile >> orderID >> item >> price >> currency) {
		if (currency != "lv.") {
			cout << "Warning: Incorrect format in orders file. Skipping entry: " << item << " " << price << " " << currency << endl;
			continue;  // Skip incorrectly formatted lines
		}

		bool found = false;

		// Check if the item is already added
		for (int i = 0; i < orderCount; i++) {
			if (orderNames[i] == item) {
				orderCounts[i]++;
				found = true;
				break;
			}
		}

		// Add new item if not found
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

	if (orderCount == 0) {
		cout << "No orders found.\n";
		return;
	}

	// Sort orders alphabetically (Selection Sort)
	for (int i = 0; i < orderCount - 1; i++) {
		int minIndex = i;
		for (int j = i + 1; j < orderCount; j++) {
			if (orderNames[j] < orderNames[minIndex]) {
				minIndex = j;
			}
		}
		// Swap items
		if (minIndex != i) {
			swap(orderNames[i], orderNames[minIndex]);
			swap(orderCounts[i], orderCounts[minIndex]);
		}
	}

	// Display sorted orders
	cout << "\n--- Sorted Orders ---\n";
	for (int i = 0; i < orderCount; i++) {
		cout << orderNames[i] << " - " << orderCounts[i] << " orders\n";
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
// Function to generate a daily report
void generateReport() {
	
	ifstream orderFile("orders.txt");

	if (!orderFile) {
		cout << "Error: Orders file not found.\n";
		return;
	}

	double totalRevenue = 0;
	string item;
	double price;
	string currency;  

	// Calculate total revenue
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

	// Get current date using localtime_s
	time_t now = time(0);
	tm ltm;
	localtime_s(&ltm, &now);

	int day = ltm.tm_mday;
	int month = 1 + ltm.tm_mon;
	int year = 1900 + ltm.tm_year;

	// Write the report to file
	ofstream reportFile("report.txt", ios::app);
	if (!reportFile) {
		cout << "Error: Unable to create or open report file.\n";
		return;
	}

	reportFile << "Date: " << day << "/" << month << "/" << year
		<< " - Total Revenue: " << totalRevenue << " lv.\n";
	reportFile.close();

	// Clear orders.txt (reset daily revenue)
	ofstream clearOrders("orders.txt", ofstream::trunc);
	if (!clearOrders) {
		cout << "Error: Unable to clear orders file.\n";
		return;
	}
	clearOrders.close();

	// Display the result
	cout << "\n--- Report Generated ---\n";
	cout << "Total Revenue for " << day << "/" << month << "/" << year
		<< ": " << totalRevenue << " lv.\n";
	cout << "Orders have been cleared.\n";
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