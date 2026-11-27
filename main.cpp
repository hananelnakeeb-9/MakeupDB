
#include <QCoreApplication>
#include <iostream>
#include <limits>
#include <string>

#include "Database.h"

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

    int choice = -1;

    do {
        std::cout << "\n========== MENU ==========\n";
        std::cout << "1. Display products\n";
        std::cout << "2. Add product\n";
        std::cout << "3. Update product\n";
        std::cout << "4. Delete product\n";
        std::cout << "5. Save database\n";
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

        if (choice == 1) {
            database.displayProducts();
        }

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

            if (p.currency != "USD" &&
                p.currency != "usd" &&
                p.currency != "GBP" &&
                p.currency != "gbp" &&
                p.currency != "CAD" &&
                p.currency != "cad" &&
                p.currency != "UNKNOWN" &&
                p.currency != "unknown") {
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

        else if (choice == 5) {
            if (database.saveToFile(filename))
                std::cout << "Database saved successfully.\n";
            else
                std::cout << "Failed to save database.\n";
        }

        else if (choice != 0) {
            std::cout << "Invalid choice.\n";
        }

    } while (choice != 0);

    std::cout << "Goodbye!\n";

    return 0;
}
