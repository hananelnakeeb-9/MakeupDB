
#include "Database.h"

#include "HashIndex.h"
#include "AVLIndex.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <utility>

// ==========================================
// HELPER FUNCTIONS
// ==========================================

std::string Database::normalize(const std::string& value) const {
    std::string result = value;

    for (char& character : result) {
        character = static_cast<char>(
            std::tolower(static_cast<unsigned char>(character))
            );
    }

    return result;
}


// Check whether a field name is supported.
bool Database::isValidField(const std::string& field) const {
    std::string name = normalize(field);

    return name == "id" ||
           name == "name" ||
           name == "brand" ||
           name == "category" ||
           name == "producttype" ||
           name == "price" ||
           name == "currency" ||
           name == "rating" ||
           name == "shadecount";
}


// Rating is optional in the dataset.
bool Database::isMissingValue(
    const Product& product,
    const std::string& field
    ) const {
    return normalize(field) == "rating" && !product.hasRating;
}


// Get a product's value as an IndexValue.
IndexValue Database::getFieldValue(
    const Product& product,
    const std::string& field
    ) const {
    std::string name = normalize(field);

    if (name == "id") {
        return static_cast<double>(product.id);
    }

    if (name == "name") {
        return product.name;
    }

    if (name == "brand") {
        return product.brand;
    }

    if (name == "category") {
        return product.category;
    }

    if (name == "producttype") {
        return product.productType;
    }

    if (name == "price") {
        return product.price;
    }

    if (name == "currency") {
        return product.currency;
    }

    if (name == "rating") {
        return product.rating;
    }

    if (name == "shadecount") {
        return static_cast<double>(product.shadeCount);
    }

    return std::string("");
}


// Find an active index for a field.
Database::ActiveIndex* Database::findIndex(
    const std::string& field
    ) {
    std::string name = normalize(field);

    for (ActiveIndex& active : activeIndexes) {
        if (active.field == name) {
            return &active;
        }
    }

    return nullptr;
}


const Database::ActiveIndex* Database::findIndex(
    const std::string& field
    ) const {
    std::string name = normalize(field);

    for (const ActiveIndex& active : activeIndexes) {
        if (active.field == name) {
            return &active;
        }
    }

    return nullptr;
}


// ==========================================
// INDEX SYNCHRONIZATION
// ==========================================

void Database::insertIntoIndexes(const Product& product) {
    for (ActiveIndex& active : activeIndexes) {

        if (isMissingValue(product, active.field)) {
            continue;
        }

        IndexValue value = getFieldValue(product, active.field);

        active.index->insert(value, product.id);
    }
}


void Database::removeFromIndexes(const Product& product) {
    for (ActiveIndex& active : activeIndexes) {

        if (isMissingValue(product, active.field)) {
            continue;
        }

        IndexValue value = getFieldValue(product, active.field);

        active.index->remove(value, product.id);
    }
}


// Rebuild all active indexes after loading a new dataset.
void Database::rebuildIndexes() {
    for (ActiveIndex& active : activeIndexes) {

        if (active.type == "hash") {
            active.index = std::make_unique<HashIndex>();
        }
        else {
            active.index = std::make_unique<AVLIndex>();
        }
    }

    for (const Product& product : products) {
        insertIntoIndexes(product);
    }
}


// ==========================================
// LOAD AND SAVE
// ==========================================

bool Database::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "Error: Could not open dataset file.\n";
        return false;
    }

    std::vector<Product> loadedProducts;

    std::string line;

    // Skip the header.
    std::getline(file, line);

    try {
        while (std::getline(file, line)) {
            if (line.empty()) {
                continue;
            }

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

            loadedProducts.push_back(p);
        }
    }
    catch (...) {
        std::cout << "Error: Invalid record in dataset.\n";
        return false;
    }

    // Replace the old records only after successful loading.
    products = std::move(loadedProducts);

    // Rebuild every active index using the new records.
    rebuildIndexes();

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

        if (p.hasRating) {
            file << p.rating;
        }
        else {
            file << "NA";
        }

        file << '\t' << p.shadeCount << '\n';
    }

    return true;
}


// ==========================================
// BASIC DATABASE OPERATIONS
// ==========================================

int Database::getProductCount() const {
    return static_cast<int>(products.size());
}


Product* Database::findProduct(int id) {
    for (Product& product : products) {
        if (product.id == id) {
            return &product;
        }
    }

    return nullptr;
}


bool Database::addProduct(const Product& product) {
    if (findProduct(product.id) != nullptr) {
        return false;
    }

    products.push_back(product);

    // Update all active indexes.
    insertIntoIndexes(products.back());

    return true;
}


bool Database::updateProduct(
    int id,
    const Product& updatedProduct
    ) {
    Product* existing = findProduct(id);

    if (existing == nullptr) {
        return false;
    }

    // Remove the old values from every index.
    removeFromIndexes(*existing);

    // Replace the product but preserve its original ID.
    *existing = updatedProduct;
    existing->id = id;

    // Insert the new values into every index.
    insertIntoIndexes(*existing);

    return true;
}


bool Database::deleteProduct(int id) {
    for (int i = 0; i < static_cast<int>(products.size()); i++) {

        if (products[i].id == id) {

            // Remove the old values from every index first.
            removeFromIndexes(products[i]);

            products.erase(products.begin() + i);

            return true;
        }
    }

    return false;
}


void Database::displayProducts(int limit) const {
    int count = 0;

    for (const Product& p : products) {
        if (count >= limit) {
            break;
        }

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
              << " of " << products.size()
              << " products.\n";
}


// ==========================================
// F6: CREATE INDEX
// ==========================================

bool Database::createIndex(
    const std::string& field,
    const std::string& indexType
    ) {
    std::string name = normalize(field);
    std::string type = normalize(indexType);

    if (!isValidField(name)) {
        std::cout << "Error: Unsupported field.\n";
        return false;
    }

    if (type != "hash" && type != "avl") {
        std::cout << "Error: Index type must be hash or avl.\n";
        return false;
    }

    if (findIndex(name) != nullptr) {
        std::cout << "Error: An index already exists for "
                  << name << ".\n";
        return false;
    }

    ActiveIndex active;

    active.field = name;
    active.type = type;

    if (type == "hash") {
        active.index = std::make_unique<HashIndex>();
    }
    else {
        active.index = std::make_unique<AVLIndex>();
    }

    // Populate the new index with existing products.
    for (const Product& product : products) {

        if (isMissingValue(product, name)) {
            continue;
        }

        IndexValue value = getFieldValue(product, name);

        active.index->insert(value, product.id);
    }

    activeIndexes.push_back(std::move(active));

    std::cout << "Created " << type
              << " index on " << name << ".\n";

    return true;
}


// ==========================================
// F6: REMOVE INDEX
// ==========================================

bool Database::removeIndex(const std::string& field) {
    std::string name = normalize(field);

    for (int i = 0;
         i < static_cast<int>(activeIndexes.size());
         i++) {

        if (activeIndexes[i].field == name) {

            activeIndexes.erase(activeIndexes.begin() + i);

            std::cout << "Removed index on "
                      << name << ".\n";

            return true;
        }
    }

    std::cout << "Error: No index exists on "
              << name << ".\n";

    return false;
}


// ==========================================
// F6: DISPLAY ACTIVE INDEXES
// ==========================================

void Database::displayIndexes() const {
    if (activeIndexes.empty()) {
        std::cout << "No active indexes.\n";
        return;
    }

    std::cout << "\nACTIVE INDEXES\n";
    std::cout << "-------------------------\n";

    for (const ActiveIndex& active : activeIndexes) {
        std::cout << "Field: " << active.field
                  << " | Type: " << active.index->getIndexType()
                  << '\n';
    }

    std::cout << "Total active indexes: "
              << activeIndexes.size() << "\n";
}


int Database::getActiveIndexCount() const {
    return static_cast<int>(activeIndexes.size());
}


// ==========================================
// F6: EXACT SEARCH
// ==========================================

std::vector<int> Database::searchExact(
    const std::string& field,
    const IndexValue& value,
    bool& usedIndex,
    std::string& method
    ) const {
    std::vector<int> results;

    std::string name = normalize(field);

    usedIndex = false;

    if (!isValidField(name)) {
        method = "Invalid field";
        return results;
    }

    const ActiveIndex* active = findIndex(name);

    // Use the index if one exists for this field.
    if (active != nullptr) {
        results = active->index->exactSearch(value);

        usedIndex = true;
        method = active->index->getIndexType() + " index";

        return results;
    }

    // Otherwise, perform a linear scan.
    method = "Linear scan";

    for (const Product& product : products) {

        if (isMissingValue(product, name)) {
            continue;
        }

        IndexValue productValue = getFieldValue(product, name);

        if (productValue == value) {
            results.push_back(product.id);
        }
    }

    return results;
}


// ==========================================
// F6: NUMERIC RANGE SEARCH
// ==========================================

std::vector<int> Database::searchRange(
    const std::string& field,
    double minimum,
    double maximum,
    bool& usedIndex,
    std::string& method,
    const std::string& currency
    ) const {
    std::vector<int> results;

    std::string name = normalize(field);

    usedIndex = false;

    if (!isValidField(name)) {
        method = "Invalid field";
        return results;
    }

    if (minimum > maximum) {
        method = "Invalid range";
        return results;
    }
    if (name == "price" &&
        (currency.empty() || normalize(currency) == "unknown")) {
        method = "Price range requires a known currency";
        return results;
    }

    // Range search is supported only for numeric fields.
    if (name != "id" &&
        name != "price" &&
        name != "rating" &&
        name != "shadecount") {

        method = "Range search requires a numeric field";
        return results;
    }

    const ActiveIndex* active = findIndex(name);

    // Use the index only if it supports range searching.
    if (active != nullptr &&
        active->index->supportsRangeSearch()) {

        std::vector<int> indexedResults =
            active->index->rangeSearch(minimum, maximum);

        if (name == "price") {
            for (const Product& product : products) {
                if (normalize(product.currency) != normalize(currency)) {
                    continue;
                }

                if (std::find(
                        indexedResults.begin(),
                        indexedResults.end(),
                        product.id
                        ) != indexedResults.end()) {
                    results.push_back(product.id);
                }
            }
        }
        else {
            results = indexedResults;
        }

        usedIndex = true;
        method = active->index->getIndexType() + " index";

        return results;
    }

    // Otherwise, use a linear scan.
    method = "Linear scan";

    // Linear scan
    for (const Product& product : products) {

        // If searching prices, only compare products with the requested currency
        if (name == "price" &&
            normalize(product.currency) != normalize(currency)) {
            continue;
        }

        double value = std::get<double>(getFieldValue(product, name));

        if (value >= minimum && value <= maximum) {
            results.push_back(product.id);
        }
    }

    return results;
}
