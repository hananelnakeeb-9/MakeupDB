#ifndef INDEX_H
#define INDEX_H

#include <string>
#include <vector>
#include <variant>

/*
 * Index.h
 *
 * F3: Abstract Index ADT
 *
 * This class defines a common interface for all indexes
 * used by the Makeup Database System.
 *
 * HashIndex and AVLIndex will inherit from this class.
 *
 * The database communicates with indexes through this
 * interface instead of depending on a specific implementation.
 */

// A key can be either text or numeric.
using IndexValue = std::variant<std::string, double>;

// Stores the result of an index search.
struct IndexEntry {
    int productID;
};

// Abstract base class for all indexes.
class Index {
public:

    // Virtual destructor for safe inheritance.
    virtual ~Index() = default;

    // Insert a value associated with a product ID.
    virtual void insert(const IndexValue& value, int productID) = 0;

    // Remove a specific value and product ID.
    virtual bool remove(const IndexValue& value, int productID) = 0;

    // Search for all products with an exact value.
    virtual std::vector<int> exactSearch(
        const IndexValue& value
        ) const = 0;

    // Search for products whose values fall within a range.
    virtual std::vector<int> rangeSearch(
        const IndexValue& minimum,
        const IndexValue& maximum
        ) const = 0;

    // Check whether this index supports range searching.
    virtual bool supportsRangeSearch() const = 0;

    // Return the name of the index implementation.
    virtual std::string getIndexType() const = 0;
};

#endif