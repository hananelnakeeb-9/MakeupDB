/*
============================================================
PROJECT: Makeup Database System
COURSE: Data Structures
TEAM MEMBERS:
1. Hanan Elnakeeb - AUC ID: 900253826

============================================================
1. PROJECT OVERVIEW
============================================================

The Makeup Database System is a C++ application that stores
and manages makeup products.

It loads product records from a text file and allows users
to add, update, delete, display, search, sort, and analyze
products.

The system also supports custom Hash Table and AVL Tree
indexes to make searching more efficient.

The database contains 493 makeup products.

Each product contains:
- ID
- Name
- Brand
- Category
- Product Type
- Price
- Currency
- Rating
- Shade Count

Missing ratings are stored as unavailable values rather
than being treated as zero.

Prices are compared only when they have the same currency.

============================================================
2. SYSTEM ARCHITECTURE
============================================================

The program uses the following architecture:

                 Makeup Database System
                           |
                           v
                       main.cpp
                           |
                           v
                       Database
                           |
             +-------------+-------------+
             |             |             |
             v             v             v
          Product       Index ADT      CRUD
                           |
                  +--------+--------+
                  |                 |
                  v                 v
              HashIndex          AVLIndex
                  |                 |
                  v                 v
             Hash Table         AVL Tree

Product:
Stores the information of one makeup product.

Database:
Stores all products, handles file operations, manages
active indexes, and performs searches.

Index:
Abstract base class defining the common interface for
all indexing implementations.

HashIndex:
Implements a custom Hash Table using open addressing
and linear probing.

AVLIndex:
Implements a self-balancing AVL Tree using rotations.

main.cpp:
Provides the console interface and command interpreter.

============================================================
3. FEATURE IMPLEMENTATION MAP
============================================================

F1 - Dataset:
Loads makeup product records from a text file.
The dataset is based on the Makeup API:
https://makeup-api.herokuapp.com/api/v1/products.json

F2 - CRUD Operations:
Supports adding, updating, deleting, and displaying
products. Product IDs must be unique.

F3 - Index ADT:
Defines an abstract Index interface implemented by
HashIndex and AVLIndex.

F4 - Hash Table:
Uses a custom Hash Table with linear probing.
Supports duplicate values, exact searching, deletion,
and automatic resizing.

F5 - AVL Tree:
Uses a self-balancing binary search tree.
Supports insertion, deletion, exact search, range search,
ascending and descending traversal, minimum, maximum,
top K, bottom K, height, and node count.

F6 - Runtime Index Management:
Allows users to create, remove, and display indexes
while the program is running.
Multiple indexes can be active simultaneously.
CRUD operations synchronize active indexes.
The system uses linear scanning when a suitable
index is unavailable.

F7 - Command Language:
Provides a command interpreter supporting searching,
sorting, filtering, statistics, index management,
record management, and command files.

F8 - Console Interface:
Provides a text-based interface with readable output,
help instructions, and error messages.

============================================================
4. COMMAND LANGUAGE REFERENCE
============================================================

HELP
Displays available commands and examples.

EXIT
Exits the application.

DISPLAY [limit]
Displays products.

EXACT field value
Searches for an exact field value.

RANGE field minimum maximum [currency]
Searches for numeric values within a range.

SORT field ASC|DESC [currency]
Sorts products in ascending or descending order.

TOP field K [currency]
Displays the K highest numeric values.

BOTTOM field K [currency]
Displays the K lowest numeric values.

FILTER field value AND field value
Filters products using two conditions.

COUNT ALL
Displays the total number of products.

COUNT field value
Counts products matching a condition.

AVERAGE numericField conditionField conditionValue
Calculates the average numeric value for matching
products.

CREATE_INDEX field HASH|AVL
Creates an index on a field.

REMOVE_INDEX field
Removes an active index.

SHOW_INDEXES
Displays all active indexes.

ADD id "name" "brand" "category" "productType"
price currency hasRating rating shadeCount
Adds a new product.

UPDATE id "name" "brand" "category" "productType"
price currency hasRating rating shadeCount
Updates an existing product.

DELETE id
Deletes a product using its ID.

SAVE
Saves the database to its data file.

RUN filename
Executes commands from a text file.

============================================================
5. DESIGN DECISIONS
============================================================

1. The Index class is abstract so that the database can
   work with different indexing implementations through
   one common interface.

2. The Hash Table uses open addressing and linear probing
   to resolve collisions.

3. Deleted Hash Table entries use tombstones so that
   searches continue correctly after deletion.

4. The Hash Table automatically grows when its load
   factor reaches the resizing threshold.

5. The AVL Tree uses rotations to maintain balance
   after insertions and deletions.

6. Duplicate index values are stored with their
   corresponding product IDs.

7. Active indexes are stored dynamically so users
   can create and remove indexes during execution.

8. When no suitable index exists, the database uses
   linear search.

9. Missing ratings are excluded from numeric
   calculations.

10. Price comparisons require a matching currency
    to avoid comparing different currencies.

============================================================
6. BUILD AND RUN INSTRUCTIONS
============================================================

Requirements:
- C++17 compatible compiler.
- CMake.
- MinGW or another supported C++ compiler.

Build using Qt Creator:
1. Open the project in Qt Creator.
2. Select the MinGW 64-bit kit.
3. Configure the CMake project.
4. Build the project.
5. Run MakeupDB.

The dataset file must be available at the path expected
by the application.

The command file can be executed using:

RUN commands.txt

============================================================
7. TESTING
============================================================

Separate test programs are provided for the custom
Hash Table and AVL Tree.

The tests cover:
- Insertion and deletion.
- Duplicate values.
- Exact searching.
- Range searching.
- Automatic Hash Table resizing.
- AVL balancing.
- Traversals.
- Minimum and maximum values.
- Top K and bottom K.
- Tree height and node count.

The main application was also tested using console
commands and a command file.
//AI log https://chatgpt.com/share/6b09b8b4-31fc-83ea-bc31-610c709fb8f8
============================================================
END OF DESIGN DOCUMENTATION
*/
#include <QCoreApplication>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cctype>
#include "Database.h"
#include "Product.h"
#include "Index.h"
using namespace std;

// ==========================================
// GENERAL HELPERS
// ==========================================

string lowerText(string value) {
    for (char& c : value) {
        c = static_cast<char>(
            tolower(static_cast<unsigned char>(c))
            );
    }
    return value;
}

bool isNumericField(const string& field) {
    string f = lowerText(field);

    return f == "id" ||
           f == "price" ||
           f == "rating" ||
           f == "shadecount";
}

bool isValidFieldName(const string& field) {
    string f = lowerText(field);

    return f == "id" ||
           f == "name" ||
           f == "brand" ||
           f == "category" ||
           f == "producttype" ||
           f == "price" ||
           f == "currency" ||
           f == "rating" ||
           f == "shadecount";
}

bool getNumericValue(
    const Product& p,
    const string& field,
    double& value
    ) {
    string f = lowerText(field);

    if (f == "id")
        value = p.id;
    else if (f == "price")
        value = p.price;
    else if (f == "rating") {
        if (!p.hasRating)
            return false;

        value = p.rating;
    }
    else if (f == "shadecount")
        value = p.shadeCount;
    else
        return false;

    return true;
}

string getTextValue(const Product& p, const string& field) {
    string f = lowerText(field);

    if (f == "name")
        return p.name;
    if (f == "brand")
        return p.brand;
    if (f == "category")
        return p.category;
    if (f == "producttype")
        return p.productType;
    if (f == "currency")
        return p.currency;

    return "";
}

bool matchesCondition(
    const Product& p,
    const string& field,
    const string& value
    ) {
    if (!isValidFieldName(field))
        return false;

    if (isNumericField(field)) {
        double expected;

        try {
            expected = stod(value);
        }
        catch (...) {
            return false;
        }

        double actual;

        if (!getNumericValue(p, field, actual))
            return false;

        return actual == expected;
    }

    return lowerText(getTextValue(p, field)) == lowerText(value);
}

void printProduct(const Product& p) {
    cout << "ID: " << p.id
         << " | Name: " << p.name
         << " | Brand: " << p.brand
         << " | Category: " << p.category
         << " | Type: " << p.productType
         << " | Price: " << p.price << " " << p.currency
         << " | Rating: ";

    if (p.hasRating)
        cout << p.rating;
    else
        cout << "N/A";

    cout << " | Shades: " << p.shadeCount << '\n';
}

void printProducts(const vector<Product>& products) {
    if (products.empty()) {
        cout << "No matching products found.\n";
        return;
    }

    for (const Product& p : products)
        printProduct(p);

    cout << "Total: " << products.size() << " products.\n";
}

// ==========================================
// CUSTOM INSERTION SORT
// ==========================================

void insertionSort(
    vector<Product>& items,
    const string& field,
    bool ascending
    ) {
    for (int i = 1; i < static_cast<int>(items.size()); i++) {
        Product current = items[i];
        int j = i - 1;

        while (j >= 0) {
            bool shouldMove = false;

            if (isNumericField(field)) {
                double a = 0;
                double b = 0;

                bool validA = getNumericValue(items[j], field, a);
                bool validB = getNumericValue(current, field, b);

                if (!validA)
                    shouldMove = validB;
                else if (!validB)
                    shouldMove = false;
                else if (ascending)
                    shouldMove = a > b;
                else
                    shouldMove = a < b;
            }
            else {
                string a = lowerText(getTextValue(items[j], field));
                string b = lowerText(getTextValue(current, field));

                if (ascending)
                    shouldMove = a > b;
                else
                    shouldMove = a < b;
            }

            if (!shouldMove)
                break;

            items[j + 1] = items[j];
            j--;
        }

        items[j + 1] = current;
    }
}

// ==========================================
// HELP
// ==========================================

void showHelp() {
    cout << "\n========== MAKEUP DATABASE COMMANDS ==========\n";

    cout << "\nGENERAL:\n";
    cout << "HELP\n";
    cout << "DISPLAY [limit]\n";
    cout << "EXIT\n";

    cout << "\nSEARCH:\n";
    cout << "EXACT field value\n";
    cout << "Example: EXACT brand colourpop\n";
    cout << "Example: EXACT name \"Lippie Pencil\"\n";

    cout << "\nRANGE:\n";
    cout << "RANGE numericField minimum maximum\n";
    cout << "Example: RANGE rating 3 5\n";
    cout << "RANGE price minimum maximum currency\n";
    cout << "Example: RANGE price 5 15 CAD\n";

    cout << "\nSORT:\n";
    cout << "SORT field ASC\n";
    cout << "SORT field DESC\n";
    cout << "Example: SORT price ASC CAD\n";
    cout << "Example: SORT name DESC\n";

    cout << "\nTOP AND BOTTOM:\n";
    cout << "TOP numericField K\n";
    cout << "BOTTOM numericField K\n";
    cout << "Example: TOP rating 5\n";
    cout << "Example: BOTTOM price 10 CAD\n";

    cout << "\nCONDITIONS:\n";
    cout << "FILTER field value AND field value\n";
    cout << "Example: FILTER brand colourpop AND category lipstick\n";

    cout << "\nSTATISTICS:\n";
    cout << "COUNT ALL\n";
    cout << "COUNT field value\n";
    cout << "AVERAGE numericField conditionField conditionValue\n";
    cout << "Example: AVERAGE rating brand colourpop\n";
    cout << "Example: AVERAGE price currency CAD\n";

    cout << "\nINDEX MANAGEMENT:\n";
    cout << "CREATE_INDEX field HASH\n";
    cout << "CREATE_INDEX field AVL\n";
    cout << "REMOVE_INDEX field\n";
    cout << "SHOW_INDEXES\n";

    cout << "\nRECORD MANAGEMENT:\n";
    cout << "ADD id \"name\" \"brand\" \"category\" \"type\" price currency hasRating rating shadeCount\n";
    cout << "UPDATE id \"name\" \"brand\" \"category\" \"type\" price currency hasRating rating shadeCount\n";
    cout << "DELETE id\n";
    cout << "SAVE\n";

    cout << "\nCOMMAND FILE:\n";
    cout << "RUN filename\n";
    cout << "Example: RUN commands.txt\n";

    cout << "\nTEXT VALUES WITH SPACES MUST BE IN QUOTES.\n";
    cout << "==============================================\n";
}

// ==========================================
// EXECUTE ONE COMMAND
// ==========================================

bool executeCommand(
    Database& database,
    const string& commandLine,
    const string& dataFile,
    bool fromFile = false
    ) {
    istringstream input(commandLine);

    string command;
    input >> command;

    command = lowerText(command);

    if (command.empty() || command[0] == '#')
        return true;

    // ======================================
    // HELP
    // ======================================

    if (command == "help") {
        showHelp();
    }

    // ======================================
    // EXIT
    // ======================================

    else if (command == "exit") {
        if (fromFile) {
            cout << "EXIT inside a command file is ignored.\n";
        }
        else {
            return false;
        }
    }

    // ======================================
    // DISPLAY
    // ======================================

    else if (command == "display") {
        int limit = database.getProductCount();

        if (!(input >> limit)) {
            input.clear();
            limit = database.getProductCount();
        }

        if (limit < 0) {
            cout << "Invalid display limit.\n";
        }
        else {
            database.displayProducts(limit);
        }
    }

    // ======================================
    // EXACT SEARCH
    // ======================================

    else if (command == "exact") {
        string field, value;

        if (!(input >> field >> quoted(value))) {
            cout << "Invalid EXACT syntax.\n";
            return true;
        }

        if (!isValidFieldName(field)) {
            cout << "Invalid field.\n";
            return true;
        }

        bool usedIndex = false;
        string method;
        vector<int> ids;

        if (isNumericField(field)) {
            double number;

            try {
                number = stod(value);
            }
            catch (...) {
                cout << "Invalid numeric value.\n";
                return true;
            }

            ids = database.searchExact(
                field,
                IndexValue(number),
                usedIndex,
                method
                );
        }
        else {
            ids = database.searchExact(
                field,
                IndexValue(value),
                usedIndex,
                method
                );
        }

        cout << "Search method: " << method << '\n';

        vector<Product> results;

        for (int id : ids) {
            Product* p = database.findProduct(id);

            if (p != nullptr)
                results.push_back(*p);
        }

        printProducts(results);
    }

    // ======================================
    // NUMERIC RANGE
    // ======================================

    else if (command == "range") {
        string field, currency = "";
        double minimum, maximum;

        if (!(input >> field >> minimum >> maximum)) {
            cout << "Invalid RANGE syntax.\n";
            return true;
        }

        if (!isNumericField(field)) {
            cout << "RANGE requires a numeric field.\n";
            return true;
        }

        if (lowerText(field) == "price" && !(input >> currency)) {
            cout << "Price range requires a currency.\n";
            return true;
        }

        bool usedIndex = false;
        string method;

        vector<int> ids = database.searchRange(
            field,
            minimum,
            maximum,
            usedIndex,
            method,
            currency
            );

        cout << "Search method: " << method << '\n';

        vector<Product> results;

        for (int id : ids) {
            Product* p = database.findProduct(id);

            if (p != nullptr)
                results.push_back(*p);
        }

        printProducts(results);
    }

    // ======================================
    // SORT
    // ======================================

    else if (command == "sort") {
        string field, order;

        if (!(input >> field >> order)) {
            cout << "Invalid SORT syntax.\n";
            return true;
        }

        order = lowerText(order);

        if (!isValidFieldName(field) ||
            (order != "asc" && order != "desc")) {
            cout << "Invalid field or sorting order.\n";
            return true;
        }

        string currency = "";

        if (lowerText(field) == "price") {
            if (!(input >> currency)) {
                cout << "Price sorting requires a currency.\n";
                return true;
            }
        }

        vector<Product> results;

        for (const Product& p : database.getProducts()) {
            if (lowerText(field) == "price" &&
                lowerText(p.currency) != lowerText(currency)) {
                continue;
            }

            results.push_back(p);
        }

        insertionSort(results, field, order == "asc");

        cout << "Sorted " << results.size()
             << " products by " << field << " "
             << order << ".\n";

        printProducts(results);
    }

    // ======================================
    // TOP K / BOTTOM K
    // ======================================

    else if (command == "top" || command == "bottom") {
        string field, currency = "";
        int k;

        if (!(input >> field >> k)) {
            cout << "Invalid TOP/BOTTOM syntax.\n";
            return true;
        }

        if (!isNumericField(field)) {
            cout << "TOP/BOTTOM requires a numeric field.\n";
            return true;
        }

        if (k <= 0) {
            cout << "K must be greater than zero.\n";
            return true;
        }

        if (lowerText(field) == "price" && !(input >> currency)) {
            cout << "Price TOP/BOTTOM requires a currency.\n";
            return true;
        }

        vector<Product> results;

        for (const Product& p : database.getProducts()) {
            if (lowerText(field) == "price" &&
                lowerText(p.currency) != lowerText(currency)) {
                continue;
            }

            double value;

            if (getNumericValue(p, field, value))
                results.push_back(p);
        }

        bool ascending = (command == "bottom");

        insertionSort(results, field, ascending);

        if (static_cast<int>(results.size()) > k)
            results.resize(k);

        cout << (command == "top" ? "TOP " : "BOTTOM ")
             << k << " by " << field << ":\n";

        printProducts(results);
    }

    // ======================================
    // TWO-FIELD FILTER
    // ======================================

    else if (command == "filter") {
        string field1, value1, andWord, field2, value2;

        if (!(input >> field1
              >> quoted(value1)
              >> andWord
              >> field2
              >> quoted(value2))) {
            cout << "Invalid FILTER syntax.\n";
            return true;
        }

        if (lowerText(andWord) != "and" ||
            !isValidFieldName(field1) ||
            !isValidFieldName(field2)) {
            cout << "Invalid FILTER conditions.\n";
            return true;
        }

        vector<Product> results;

        for (const Product& p : database.getProducts()) {
            if (matchesCondition(p, field1, value1) &&
                matchesCondition(p, field2, value2)) {
                results.push_back(p);
            }
        }

        printProducts(results);
    }

    // ======================================
    // COUNT
    // ======================================

    else if (command == "count") {
        string field, value;

        if (!(input >> field)) {
            cout << "Invalid COUNT syntax.\n";
            return true;
        }

        if (lowerText(field) == "all") {
            cout << "Total products: "
                 << database.getProductCount() << '\n';
        }
        else if (input >> quoted(value)) {
            if (!isValidFieldName(field)) {
                cout << "Invalid field.\n";
                return true;
            }

            int count = 0;

            for (const Product& p : database.getProducts()) {
                if (matchesCondition(p, field, value))
                    count++;
            }

            cout << "Count: " << count << '\n';
        }
        else {
            cout << "Invalid COUNT syntax.\n";
        }
    }

    // ======================================
    // AVERAGE
    // ======================================

    else if (command == "average") {
        string numericField;
        string conditionField;
        string conditionValue;

        if (!(input >> numericField
              >> conditionField
              >> quoted(conditionValue))) {
            cout << "Invalid AVERAGE syntax.\n";
            return true;
        }

        if (!isNumericField(numericField) ||
            !isValidFieldName(conditionField)) {
            cout << "Invalid field.\n";
            return true;
        }

        // Prices must be averaged within one currency.
        if (lowerText(numericField) == "price" &&
            lowerText(conditionField) != "currency") {
            cout << "Price average requires a currency condition.\n";
            cout << "Example: AVERAGE price currency CAD\n";
            return true;
        }

        if (lowerText(numericField) == "price" &&
            lowerText(conditionValue) == "unknown") {
            cout << "Cannot calculate a price average for UNKNOWN currency.\n";
            return true;
        }

        double total = 0;
        int count = 0;

        for (const Product& p : database.getProducts()) {
            if (!matchesCondition(p, conditionField, conditionValue))
                continue;

            double value;

            if (getNumericValue(p, numericField, value)) {
                total += value;
                count++;
            }
        }

        if (count == 0) {
            cout << "No matching numeric values.\n";
        }
        else {
            cout << fixed << setprecision(2);

            cout << "Count: " << count << '\n';

            cout << "Average " << numericField << ": "
                 << total / count << '\n';

            cout.unsetf(ios::floatfield);
        }
    }
    // CREATE INDEX
    else if (command == "create_index") {
        string field, type;

        if (!(input >> field >> type)) {
            cout << "Invalid CREATE_INDEX syntax.\n";
            return true;
        }

        database.createIndex(field, type);
    }
    // REMOVE INDEX
    else if (command == "remove_index") {
        string field;

        if (!(input >> field)) {
            cout << "Invalid REMOVE_INDEX syntax.\n";
            return true;
        }

        database.removeIndex(field);
    }
    // SHOW INDEXES
    else if (command == "show_indexes") {
        database.displayIndexes();
    }
    // DELETE RECORD
    else if (command == "delete") {
        int id;

        if (!(input >> id)) {
            cout << "Invalid DELETE syntax.\n";
            return true;
        }

        if (database.deleteProduct(id))
            cout << "Product deleted successfully.\n";
        else
            cout << "Product not found.\n";
    }
    // ADD / UPDATE RECORD
    else if (command == "add" || command == "update") {
        Product p{};
        int hasRating;
        if (!(input >> p.id
              >> quoted(p.name)
              >> quoted(p.brand)
              >> quoted(p.category)
              >> quoted(p.productType)
              >> p.price
              >> p.currency
              >> hasRating
              >> p.rating
              >> p.shadeCount)) {
            cout << "Invalid ADD/UPDATE syntax.\n";
            return true;
        }
        if (p.name.empty() ||
            p.brand.empty() ||
            p.category.empty() ||
            p.productType.empty() ||
            p.price < 0 ||
            p.shadeCount < 0 ||
            (hasRating != 0 && hasRating != 1) ||
            (hasRating == 1 && (p.rating < 0 || p.rating > 5))) {
            cout << "Invalid product values.\n";
            return true;
        }

        p.currency = lowerText(p.currency);

        for (char& c : p.currency) {
            c = static_cast<char>(
                toupper(static_cast<unsigned char>(c))
                );
        }

        if (p.currency != "USD" &&
            p.currency != "GBP" &&
            p.currency != "CAD" &&
            p.currency != "UNKNOWN") {
            cout << "Invalid currency.\n";
            return true;
        }

        p.hasRating = (hasRating == 1);

        if (!p.hasRating)
            p.rating = 0;

        bool success;

        if (command == "add")
            success = database.addProduct(p);
        else
            success = database.updateProduct(p.id, p);

        if (success)
            cout << "Product operation successful.\n";
        else
            cout << "Product operation failed. Check the ID.\n";
    }

    // ======================================
    // SAVE
    // ======================================

    else if (command == "save") {
        if (database.saveToFile(dataFile))
            cout << "Database saved successfully.\n";
        else
            cout << "Could not save database.\n";
    }

    // ======================================
    // RUN COMMAND FILE
    // ======================================

    else if (command == "run") {
        string filename;

        if (!(input >> filename)) {
            cout << "Invalid RUN syntax.\n";
            return true;
        }

        ifstream commandFile(filename);

        if (!commandFile.is_open()) {
            cout << "Could not open command file: "
                 << filename << '\n';
            return true;
        }

        cout << "\nRunning command file: "
             << filename << "\n";

        string line;
        int lineNumber = 0;

        while (getline(commandFile, line)) {
            lineNumber++;

            if (line.empty() || line[0] == '#')
                continue;

            cout << "\n[" << lineNumber << "] "
                 << line << '\n';

            if (lowerText(line).find("run ") == 0) {
                cout << "Nested RUN commands are not supported.\n";
                continue;
            }

            executeCommand(database, line, dataFile, true);
        }

        cout << "\nFinished command file.\n";
    }

    // ======================================
    // INVALID COMMAND
    // ======================================

    else {
        cout << "Invalid command: " << command << '\n';
        cout << "Type HELP to see available commands.\n";
    }

    return true;
}

// ==========================================
// MAIN
// ==========================================

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    Database database;

    const string dataFile = "data/makeup_products.txt";

    if (!database.loadFromFile(dataFile)) {
        cout << "Failed to load database.\n";
        return 1;
    }

    cout << "\n====================================\n";
    cout << "       MAKEUP DATABASE SYSTEM\n";
    cout << "====================================\n";

    cout << database.getProductCount()
         << " products loaded successfully.\n";

    cout << "Type HELP to see available commands.\n";

    string line;

    while (true) {
        cout << "\nDB> ";

        if (!getline(cin, line))
            break;

        if (line.empty())
            continue;

        if (!executeCommand(database, line, dataFile))
            break;
    }

    cout << "Goodbye!\n";

    return 0;
}
