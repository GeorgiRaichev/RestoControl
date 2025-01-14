#include <iostream>
#include <string>
#include "menu.h"
#include "inventory.h"
#include "order.h"

using namespace std;

void showWaiterMenu();  // Waiter menu
void showManagerMenu(); // Manager menu
bool isManager = false; // Tracks if the user is a manager

int main() {
    string userType;

    // Select user type
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

    int choice;
    bool exitProgram = false;

    // Main menu loop
    while (!exitProgram) {
        if (isManager) {
            showManagerMenu();
        }
        else {
            showWaiterMenu();
        }

        cin >> choice;

        // Navigation and user selection
        if (isManager) {
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
        else {
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
