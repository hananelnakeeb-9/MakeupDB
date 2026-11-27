
#ifndef DATABASE_H
#define DATABASE_H

#include "Product.h"
#include <vector>
#include <string>

class Database {
private:
    std::vector<Product> products;

public:
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;

    int getProductCount() const;

    bool addProduct(const Product& product);
    bool updateProduct(int id, const Product& updatedProduct);
    bool deleteProduct(int id);

    Product* findProduct(int id);
    void displayProducts(int limit = 10) const;
};

#endif
