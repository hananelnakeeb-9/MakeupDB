
#ifndef DATABASE_H
#define DATABASE_H

#include "Product.h"
#include "Index.h"

#include <vector>
#include <string>
#include <memory>

class Database {
private:
    std::vector<Product> products;

    // Stores every active index and the field it indexes.
    struct ActiveIndex {
        std::string field;
        std::string type;
        std::unique_ptr<Index> index;
    };

    std::vector<ActiveIndex> activeIndexes;

    // Field and index management helpers.
    std::string normalize(const std::string& value) const;
    bool isValidField(const std::string& field) const;
    bool isMissingValue(const Product& product,
                        const std::string& field) const;

    IndexValue getFieldValue(
        const Product& product,
        const std::string& field
        ) const;

    ActiveIndex* findIndex(const std::string& field);
    const ActiveIndex* findIndex(const std::string& field) const;

    void insertIntoIndexes(const Product& product);
    void removeFromIndexes(const Product& product);
    void rebuildIndexes();

public:
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;

    int getProductCount() const;

    bool addProduct(const Product& product);
    bool updateProduct(int id, const Product& updatedProduct);
    bool deleteProduct(int id);

    Product* findProduct(int id);
    void displayProducts(int limit = 10) const;

    // F6: Runtime index management.
    bool createIndex(
        const std::string& field,
        const std::string& indexType
        );

    bool removeIndex(const std::string& field);

    void displayIndexes() const;

    int getActiveIndexCount() const;

    // F6: Exact search using an index or linear scan.
    std::vector<int> searchExact(
        const std::string& field,
        const IndexValue& value,
        bool& usedIndex,
        std::string& method
        ) const;

    // F6: Numeric range search using AVL or linear scan.
    std::vector<int> searchRange(
        const std::string& field,
        double minimum,
        double maximum,
        bool& usedIndex,
        std::string& method,
        const std::string& currency = ""
        ) const;
};

#endif
