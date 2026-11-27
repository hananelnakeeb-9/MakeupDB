
#include <QCoreApplication>
#include <iostream>
#include <limits>

#include "Database.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    Database database;

    const std::string filename = "data/makeup_products.txt";

    if (!database.loadFromFile(filename))
        return 1;

    std::cout << "MAKEUP DATABASE SYSTEM\n";
    std::cout << database.getProductCount()
              << " products loaded successfully.\n";

    int choice;

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
            std::cout << "Invalid input.\n";
            continue;
        }

        if (choice == 1) {
            database.displayProducts();
        }

        else if (choice == 2 || choice == 3) {
            Product p{};

            if (choice == 3) {
                std::cout << "Enter ID to update: ";
                std::cin >> p.id;

                if (database.findProduct(p.id) == nullptr) {
                    std::cout << "Product not found.\n";
                    continue;
                }
            }
            else {
                std::cout << "Enter new product ID: ";
                std::cin >> p.id;
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

            std::cout << "Price: ";
            std::cin >> p.price;

            std::cout << "Currency (USD, GBP, CAD, UNKNOWN): ";
            std::cin >> p.currency;

            std::cout << "Has rating? (1 = yes, 0 = no): ";
            std::cin >> p.hasRating;

            if (p.hasRating) {
                std::cout << "Rating: ";
                std::cin >> p.rating;
            }
            else {
                p.rating = 0;
            }

            std::cout << "Shade count: ";
            std::cin >> p.shadeCount;

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
            std::cin >> id;

            if (database.deleteProduct(id))
                std::cout << "Product deleted successfully.\n";
            else
                std::cout << "Product not found.\n";
        }

        else if (choice == 5) {
            if (database.saveToFile(filename))
                std::cout << "Database saved successfully.\n";
        }

        else if (choice != 0) {
            std::cout << "Invalid choice.\n";
        }

    } while (choice != 0);

    std::cout << "Goodbye!\n";
    return 0;
}
