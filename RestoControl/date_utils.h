#ifndef DATE_UTILS_H
#define DATE_UTILS_H

struct Date {
    int day;
    int month;
    int year;
};

extern Date currentDate;

void saveDate();
void loadDate();
void incrementDate();
void getCurrentDate(int& day, int& month, int& year);
int daysInMonth(int month, int year);

#endif
