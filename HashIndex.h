#ifndef HASHINDEX_H
#define HASHINDEX_H

#include "Index.h"

#include <vector>
#include <string>
#include <cstddef>
#include <cstdint>

class HashIndex : public Index {
private:

    enum class BucketState {
        EMPTY,
        OCCUPIED,
        DELETED
    };

    struct Bucket {
        IndexValue key;
        std::vector<int> productIDs;
        BucketState state = BucketState::EMPTY;
    };

    std::vector<Bucket> table;

    // Number of buckets containing active keys.
    std::size_t itemCount;

    // Number of deleted buckets (tombstones).
    std::size_t deletedCount;

    // Initial table capacity.
    static constexpr std::size_t INITIAL_CAPACITY = 11;

    // Calculate a hash value for text or numeric keys.
    std::size_t hashValue(const IndexValue& value) const;

    // Check whether two index values are equal.
    bool valuesEqual(
        const IndexValue& first,
        const IndexValue& second
        ) const;

    // Find an existing key. Returns table.size() if not found.
    std::size_t findKey(const IndexValue& value) const;

    // Find a suitable bucket for a new key.
    std::size_t findInsertPosition(const IndexValue& value) const;

    // Increase table capacity and reinsert existing keys.
    void resizeTable();

public:

    HashIndex();

    void insert(const IndexValue& value, int productID) override;

    bool remove(
        const IndexValue& value,
        int productID
        ) override;

    std::vector<int> exactSearch(
        const IndexValue& value
        ) const override;

    std::vector<int> rangeSearch(
        const IndexValue& minimum,
        const IndexValue& maximum
        ) const override;

    bool supportsRangeSearch() const override;

    std::string getIndexType() const override;

    // Useful information for testing and displaying the index.
    std::size_t getSize() const;

    std::size_t getCapacity() const;

    double getLoadFactor() const;
};

#endif
