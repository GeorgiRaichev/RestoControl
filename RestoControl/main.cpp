/**
*
* Solution to course project # 1
* Introduction to programming course
* Faculty of Mathematics and Informatics of Sofia University
* Winter semester 2024/2025
*
* @author Georgi Raychev
* @idnumber 0MI0600451
* @compiler Visual Studio
*
* This file contains helper functions for solving variant 4 of the course project.
*
*/
#include <iostream>
#include <fstream>
#include <string>
#include "menu.h"
#include "inventory.h"
#include "order.h"
#include "date_utils.h"

using namespace std;

void initializeFiles() {

	ofstream ofs("orders.txt", ios::trunc);
}

void showWaiterMenu();  
void showManagerMenu(); 
bool isManager = false; 

void handleManagerChoice(int choice, bool& exitProgram) {
	switch (choice) {
	case 1: displayMenu(); break;
	case 2: addOrder(); break;
	case 3: cancelOrder(); break;
	case 4: viewOrders(); break;
	case 5: viewSortedOrders(); break;
	case 6: checkInventory(); break;
	case 7: issueProduct(); break;
	case 8: addInventory(); break;
	case 9: viewDailyRevenue(); break;
	case 10: generateReport(); break;
	case 11: viewTotalRevenueByDate(); break;
	case 12: addItemToMenu(); break;
	case 13: removeItemFromMenu(); break;
	case 14:
		cout << "Exiting system. Goodbye!\n";
		exitProgram = true;
		break;
	default:
		cout << "Invalid choice. Please try again.\n";
	}
}

void handleWaiterChoice(int choice, bool& exitProgram) {
	switch (choice) {
	case 1: displayMenu(); break;
	case 2: addOrder(); break;
	case 3: cancelOrder(); break;
	case 4: viewOrders(); break;
	case 5: viewSortedOrders(); break;
	case 6: viewDailyRevenue(); break;
	case 7:
		cout << "Exiting system. Goodbye!\n";
		exitProgram = true;
		break;
	default:
		cout << "Invalid choice. Please try again.\n";
	}
}

void selectUserType() {
	string userType;
	while (true) {
		cout << "Enter user type (waiter/manager): ";
		cin >> userType;

		if (userType == "waiter") {
			isManager = false;
			break;
		}
		else if (userType == "manager") {
			isManager = true;
			break;
		}
		else {
			cout << "Invalid input. Please enter 'waiter' or 'manager'.\n";
		}
	}
}

int main() {
	initializeFiles();
	loadDate();
	selectUserType();
	int choice;
	bool exitProgram = false;

	while (!exitProgram) {
		
		isManager ? showManagerMenu() : showWaiterMenu();
		if (!(cin >> choice)) {
			cout << "Invalid input. Please enter a numeric option.\n";
			cin.clear();                
			cin.ignore(10000, '\n');    
			continue;                  
		}

		if (isManager) {
			handleManagerChoice(choice, exitProgram);
		}
		else {
			handleWaiterChoice(choice, exitProgram);
		}
	}
	return 0;
}

// Waiter menu
void showWaiterMenu() {
	cout << "\n=== Waiter Menu ===\n";
	cout << "1. View Menu\n";
	cout << "2. Add Order\n";
	cout << "3. Cancel Order\n";
	cout << "4. View Past Orders\n";
	cout << "5. View Sorted Orders and Item Counts\n";
	cout << "6. View Daily Revenue\n";
	cout << "7. Exit\n";
	cout << "Choose an option: ";
}

// Manager menu
void showManagerMenu() {
	cout << "\n=== Manager Menu ===\n";
	cout << "1. View Menu\n";
	cout << "2. Add Order\n";
	cout << "3. Cancel Order\n";
	cout << "4. View Past Orders\n";
	cout << "5. View Sorted Orders and Item Counts\n";
	cout << "6. Check Inventory\n";
	cout << "7. Issue Product from Inventory\n";
	cout << "8. Add Product to Inventory\n";
	cout << "9. View Daily Revenue\n";
	cout << "10. Generate Daily Report (Resets Revenue)\n";
	cout << "11. View Total Revenue by Date\n";
	cout << "12. Add New Item to Menu\n";
	cout << "13. Remove Item from Menu\n";
	cout << "14. Exit\n";
	cout << "Choose an option: ";
}
