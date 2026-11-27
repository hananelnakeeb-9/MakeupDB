
#include <QCoreApplication>
#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <variant>
#include <cctype>

#include "Database.h"
#include "Index.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    Database database;

    const std::string filename = "data/makeup_products.txt";

    if (!database.loadFromFile(filename)) {
        std::cout << "Failed to load database.\n";
        return 1;
    }

    std::cout << "MAKEUP DATABASE SYSTEM\n";
    std::cout << database.getProductCount()
              << " products loaded successfully.\n";

    // Display search results.
    auto displaySearchResults = [&](const std::vector<int>& ids) {
        if (ids.empty()) {
            std::cout << "No matching products found.\n";
            return;
        }

        for (int id : ids) {
            Product* p = database.findProduct(id);

            if (p != nullptr) {
                std::cout << "ID: " << p->id
                          << " | Name: " << p->name
                          << " | Brand: " << p->brand
                          << " | Price: " << p->price << " "
                          << p->currency
                          << " | Shades: " << p->shadeCount
                          << '\n';
            }
        }

        std::cout << "Found " << ids.size()
                  << " matching products.\n";
    };

    int choice = -1;

    do {
        std::cout << "\n========== MENU ==========\n";
        std::cout << "1. Display products\n";
        std::cout << "2. Add product\n";
        std::cout << "3. Update product\n";
        std::cout << "4. Delete product\n";
        std::cout << "5. Save database\n";
        std::cout << "6. Create index\n";
        std::cout << "7. Remove index\n";
        std::cout << "8. Display active indexes\n";
        std::cout << "9. Exact search\n";
        std::cout << "10. Numeric range search\n";
        std::cout << "0. Exit\n";
        std::cout << "Choose: ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
                );
            std::cout << "Invalid input. Please enter a number.\n";
            choice = -1;
            continue;
        }

        // F1: DISPLAY PRODUCTS
        if (choice == 1) {
            database.displayProducts();
        }

        // F2: ADD OR UPDATE PRODUCT
        else if (choice == 2 || choice == 3) {
            Product p{};

            if (choice == 3) {
                std::cout << "Enter ID to update: ";

                if (!(std::cin >> p.id)) {
                    std::cin.clear();
                    std::cin.ignore(
                        std::numeric_limits<std::streamsize>::max(), '\n'
                        );
                    std::cout << "Invalid ID.\n";
                    continue;
                }

                if (database.findProduct(p.id) == nullptr) {
                    std::cout << "Product not found.\n";
                    continue;
                }
            }
            else {
                std::cout << "Enter new product ID: ";

                if (!(std::cin >> p.id)) {
                    std::cin.clear();
                    std::cin.ignore(
                        std::numeric_limits<std::streamsize>::max(), '\n'
                        );
                    std::cout << "Invalid ID.\n";
                    continue;
                }
            }

            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
                );

            std::cout << "Name: ";
            std::getline(std::cin, p.name);

            std::cout << "Brand: ";
            std::getline(std::cin, p.brand);

            std::cout << "Category: ";
            std::getline(std::cin, p.category);

            std::cout << "Product type: ";
            std::getline(std::cin, p.productType);

            if (p.name.empty() || p.brand.empty() ||
                p.category.empty() || p.productType.empty()) {
                std::cout << "Fields cannot be empty.\n";
                continue;
            }

            std::cout << "Price: ";

            if (!(std::cin >> p.price) || p.price < 0) {
                std::cin.clear();
                std::cin.ignore(
                    std::numeric_limits<std::streamsize>::max(), '\n'
                    );
                std::cout << "Invalid price.\n";
                continue;
            }

            std::cout << "Currency (USD, GBP, CAD, UNKNOWN): ";
            std::cin >> p.currency;

            for (char& c : p.currency) {
                c = static_cast<char>(
                    std::toupper(static_cast<unsigned char>(c))
                    );
            }

            if (p.currency != "USD" && p.currency != "GBP" &&
                p.currency != "CAD" && p.currency != "UNKNOWN") {
                std::cout << "Invalid currency.\n";
                continue;
            }

            std::cout << "Has rating? (1 = yes, 0 = no): ";

            if (!(std::cin >> p.hasRating) ||
                (p.hasRating != 0 && p.hasRating != 1)) {
                std::cin.clear();
                std::cin.ignore(
                    std::numeric_limits<std::streamsize>::max(), '\n'
                    );
                std::cout << "Invalid rating choice.\n";
                continue;
            }

            if (p.hasRating) {
                std::cout << "Rating: ";

                if (!(std::cin >> p.rating) ||
                    p.rating < 0 || p.rating > 5) {
                    std::cin.clear();
                    std::cin.ignore(
                        std::numeric_limits<std::streamsize>::max(), '\n'
                        );
                    std::cout << "Invalid rating. Enter a value from 0 to 5.\n";
                    continue;
                }
            }
            else {
                p.rating = 0;
            }

            std::cout << "Shade count: ";

            if (!(std::cin >> p.shadeCount) || p.shadeCount < 0) {
                std::cin.clear();
                std::cin.ignore(
                    std::numeric_limits<std::streamsize>::max(), '\n'
                    );
                std::cout << "Invalid shade count.\n";
                continue;
            }

            bool success;

            if (choice == 2)
                success = database.addProduct(p);
            else
                success = database.updateProduct(p.id, p);

            if (success)
                std::cout << "Operation successful!\n";
            else
                std::cout << "Operation failed. Check the ID.\n";
        }

        // F2: DELETE PRODUCT
        else if (choice == 4) {
            int id;

            std::cout << "Enter product ID to delete: ";

            if (!(std::cin >> id)) {
                std::cin.clear();
                std::cin.ignore(
                    std::numeric_limits<std::streamsize>::max(), '\n'
                    );
                std::cout << "Invalid ID.\n";
                continue;
            }

            if (database.deleteProduct(id))
                std::cout << "Product deleted successfully.\n";
            else
                std::cout << "Product not found.\n";
        }

        // F2: SAVE DATABASE
        else if (choice == 5) {
            if (database.saveToFile(filename))
                std::cout << "Database saved successfully.\n";
            else
                std::cout << "Failed to save database.\n";
        }

        // F6: CREATE INDEX
        else if (choice == 6) {
            std::string field, type;

            std::cout << "Enter field name: ";
            std::cin >> field;

            std::cout << "Index type (HASH or AVL): ";
            std::cin >> type;

            for (char& c : type) {
                c = static_cast<char>(
                    std::tolower(static_cast<unsigned char>(c))
                    );
            }

            if (database.createIndex(field, type))
                std::cout << "Index created successfully!\n";
            else
                std::cout << "Could not create index.\n";
        }

        // F6: REMOVE INDEX
        else if (choice == 7) {
            std::string field;

            std::cout << "Enter field name: ";
            std::cin >> field;

            if (database.removeIndex(field))
                std::cout << "Index removed successfully!\n";
            else
                std::cout << "No active index found for this field.\n";
        }

        // F6: DISPLAY ACTIVE INDEXES
        else if (choice == 8) {
            database.displayIndexes();

            std::cout << "Active indexes: "
                      << database.getActiveIndexCount() << '\n';
        }

        // F6: EXACT SEARCH
        else if (choice == 9) {
            std::string field;
            std::string value;
            std::string method;
            bool usedIndex = false;

            std::cout << "Enter field name: ";
            std::cin >> field;

            std::string normalizedField = field;

            for (char& c : normalizedField) {
                c = static_cast<char>(
                    std::tolower(static_cast<unsigned char>(c))
                    );
            }

            bool numericField =
                normalizedField == "id" ||
                normalizedField == "price" ||
                normalizedField == "rating" ||
                normalizedField == "shadecount";

            std::vector<int> results;

            if (numericField) {
                double number;

                std::cout << "Enter numeric value: ";

                if (!(std::cin >> number)) {
                    std::cin.clear();
                    std::cin.ignore(
                        std::numeric_limits<std::streamsize>::max(), '\n'
                        );
                    std::cout << "Invalid numeric value.\n";
                    continue;
                }

                results = database.searchExact(
                    field, IndexValue(number), usedIndex, method
                    );
            }
            else {
                std::cout << "Enter exact text value: ";

                std::cin.ignore(
                    std::numeric_limits<std::streamsize>::max(), '\n'
                    );

                std::getline(std::cin, value);

                results = database.searchExact(
                    field, IndexValue(value), usedIndex, method
                    );
            }

            std::cout << "Search method: " << method << '\n';
            std::cout << "Index used: "
                      << (usedIndex ? "Yes" : "No") << '\n';

            displaySearchResults(results);
        }

        // F6: NUMERIC RANGE SEARCH
        else if (choice == 10) {
            std::string field;
            std::string currency = "";
            std::string method;
            double minimum, maximum;
            bool usedIndex = false;

            std::vector<int> results;

            std::cout << "Enter numeric field (price, rating, id, shadeCount): ";
            std::cin >> field;

            std::cout << "Enter minimum value: ";

            if (!(std::cin >> minimum)) {
                std::cin.clear();
                std::cin.ignore(
                    std::numeric_limits<std::streamsize>::max(), '\n'
                    );
                std::cout << "Invalid minimum value.\n";
                continue;
            }

            std::cout << "Enter maximum value: ";

            if (!(std::cin >> maximum)) {
                std::cin.clear();
                std::cin.ignore(
                    std::numeric_limits<std::streamsize>::max(), '\n'
                    );
                std::cout << "Invalid maximum value.\n";
                continue;
            }

            std::string normalizedField = field;

            for (char& c : normalizedField) {
                c = static_cast<char>(
                    std::tolower(static_cast<unsigned char>(c))
                    );
            }

            if (normalizedField == "price") {
                std::cout << "Enter currency (USD, GBP, CAD): ";
                std::cin >> currency;

                for (char& c : currency) {
                    c = static_cast<char>(
                        std::toupper(static_cast<unsigned char>(c))
                        );
                }
            }

            results = database.searchRange(
                field, minimum, maximum,
                usedIndex, method, currency
                );

            std::cout << "Search method: " << method << '\n';
            std::cout << "Index used: "
                      << (usedIndex ? "Yes" : "No") << '\n';

            displaySearchResults(results);
        }

        else if (choice == 0) {
            // Exit the application.
        }

        else {
            std::cout << "Invalid choice.\n";
        }

    } while (choice != 0);

    std::cout << "Goodbye!\n";

    return 0;
}
