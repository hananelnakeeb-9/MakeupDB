
#include "Database.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

bool Database::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "Error: Could not open dataset file.\n";
        return false;
    }

    products.clear();

    std::string line;
    std::getline(file, line); // Skip header

    try {
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string field;

            Product p{};

            std::getline(ss, field, '\t');
            p.id = std::stoi(field);

            std::getline(ss, p.name, '\t');
            std::getline(ss, p.brand, '\t');
            std::getline(ss, p.category, '\t');
            std::getline(ss, p.productType, '\t');

            std::getline(ss, field, '\t');
            p.price = std::stod(field);

            std::getline(ss, p.currency, '\t');

            std::getline(ss, field, '\t');
            p.hasRating = (field != "NA");
            p.rating = p.hasRating ? std::stod(field) : 0.0;

            std::getline(ss, field, '\t');
            p.shadeCount = std::stoi(field);

            products.push_back(p);
        }
    }
    catch (...) {
        std::cout << "Error: Invalid record in dataset.\n";
        return false;
    }

    return true;
}

bool Database::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cout << "Error: Could not save dataset.\n";
        return false;
    }

    file << "ID\tName\tBrand\tCategory\tProductType"
         << "\tPrice\tCurrency\tRating\tShadeCount\n";

    for (const Product& p : products) {
        file << p.id << '\t'
             << p.name << '\t'
             << p.brand << '\t'
             << p.category << '\t'
             << p.productType << '\t'
             << std::setprecision(15) << p.price << '\t'
             << p.currency << '\t';

        if (p.hasRating)
            file << p.rating;
        else
            file << "NA";

        file << '\t' << p.shadeCount << '\n';
    }

    return true;
}

int Database::getProductCount() const {
    return static_cast<int>(products.size());
}

Product* Database::findProduct(int id) {
    for (Product& p : products) {
        if (p.id == id)
            return &p;
    }

    return nullptr;
}

bool Database::addProduct(const Product& product) {
    if (findProduct(product.id) != nullptr)
        return false;

    products.push_back(product);
    return true;
}

bool Database::updateProduct(int id, const Product& updatedProduct) {
    Product* existing = findProduct(id);

    if (existing == nullptr)
        return false;

    *existing = updatedProduct;
    existing->id = id;

    return true;
}

bool Database::deleteProduct(int id) {
    for (int i = 0; i < static_cast<int>(products.size()); i++) {
        if (products[i].id == id) {
            products.erase(products.begin() + i);
            return true;
        }
    }

    return false;
}

void Database::displayProducts(int limit) const {
    int count = 0;

    for (const Product& p : products) {
        if (count >= limit)
            break;

        std::cout << "ID: " << p.id
                  << " | Name: " << p.name
                  << " | Brand: " << p.brand
                  << " | Price: " << p.price
                  << " " << p.currency
                  << " | Shades: " << p.shadeCount
                  << '\n';

        count++;
    }

    std::cout << "\nShowing " << count
              << " of " << products.size() << " products.\n";
}
