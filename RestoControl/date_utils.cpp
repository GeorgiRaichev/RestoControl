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
* This file contains helper functions for solving variant 1 of the course project.
*
*/



#include "date_utils.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

Date currentDate;  

void saveDate() {
    ofstream outfile("date.txt", ios::trunc);
    if (!outfile) return;
    outfile << (currentDate.day < 10 ? "0" : "") << currentDate.day << "/"
        << (currentDate.month < 10 ? "0" : "") << currentDate.month << "/"
        << currentDate.year;
    outfile.close();
}

void loadDate() {
    ifstream infile("date.txt");
    string dateStr;
    if (infile >> dateStr && dateStr.size() == 10) {
        currentDate.day = stoi(dateStr.substr(0, 2));
        currentDate.month = stoi(dateStr.substr(3, 2));
        currentDate.year = stoi(dateStr.substr(6, 4));
    }
    else {
        currentDate = { 1, 1, 2025 };
        saveDate(); 
    }
    infile.close();
}

void incrementDate() {
    currentDate.day++;
    int dim = daysInMonth(currentDate.month, currentDate.year);
    if (currentDate.day > dim) {
        currentDate.day = 1;
        currentDate.month++;
        if (currentDate.month > 12) {
            currentDate.month = 1;
            currentDate.year++;
        }
    }
    saveDate();
}

void getCurrentDate(int& day, int& month, int& year) {
    day = currentDate.day;
    month = currentDate.month;
    year = currentDate.year;
}

int daysInMonth(int month, int year) {
    switch (month) {
    case 1: case 3: case 5: case 7: case 8: case 10: case 12:
        return 31;
    case 4: case 6: case 9: case 11:
        return 30;
    case 2:
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
            return 29;
        else
            return 28;
    default:
        return 30;
    }
}
