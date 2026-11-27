#include "HashIndex.h"
#include <cstring>
#include <cstdint>
HashIndex::HashIndex()
    : table(INITIAL_CAPACITY),
    itemCount(0),
    deletedCount(0) {
}

// FNV-style hashing for strings and bit-based hashing for numbers.
std::size_t HashIndex::hashValue(const IndexValue& value) const {

    if (std::holds_alternative<std::string>(value)) {

        const std::string& text =
            std::get<std::string>(value);

        std::uint64_t hash = 14695981039346656037ULL;

        for (unsigned char character : text) {
            hash ^= character;
            hash *= 1099511628211ULL;
        }

        return static_cast<std::size_t>(hash);
    }

    double number = std::get<double>(value);

    // Make +0.0 and -0.0 produce the same hash.
    if (number == 0.0) {
        number = 0.0;
    }

    std::uint64_t bits = 0;
    std::memcpy(&bits, &number, sizeof(double));

    // Mix the bits to distribute numeric keys.
    bits ^= bits >> 33;
    bits *= 0xff51afd7ed558ccdULL;
    bits ^= bits >> 33;
    bits *= 0xc4ceb9fe1a85ec53ULL;
    bits ^= bits >> 33;

    return static_cast<std::size_t>(bits);
}

// Compare two values, including their types.
bool HashIndex::valuesEqual(
    const IndexValue& first,
    const IndexValue& second
    ) const {

    if (first.index() != second.index()) {
        return false;
    }

    return first == second;
}

// Search for an existing key using linear probing.
std::size_t HashIndex::findKey(
    const IndexValue& value
    ) const {

    std::size_t capacity = table.size();

    std::size_t position = hashValue(value) % capacity;

    for (std::size_t i = 0; i < capacity; i++) {

        std::size_t current = (position + i) % capacity;

        if (table[current].state == BucketState::EMPTY) {
            return capacity;
        }

        if (table[current].state == BucketState::OCCUPIED &&
            valuesEqual(table[current].key, value)) {

            return current;
        }
    }

    return capacity;
}

// Find a position for a new key.
// Deleted buckets can be reused.
std::size_t HashIndex::findInsertPosition(
    const IndexValue& value
    ) const {

    std::size_t capacity = table.size();

    std::size_t position = hashValue(value) % capacity;

    std::size_t firstDeleted = capacity;

    for (std::size_t i = 0; i < capacity; i++) {

        std::size_t current = (position + i) % capacity;

        if (table[current].state == BucketState::OCCUPIED) {
            continue;
        }

        if (table[current].state == BucketState::DELETED) {

            if (firstDeleted == capacity) {
                firstDeleted = current;
            }

            continue;
        }

        // Found an empty bucket.
        if (firstDeleted != capacity) {
            return firstDeleted;
        }

        return current;
    }

    return firstDeleted;
}

// Resize the table and reinsert all active keys.
void HashIndex::resizeTable() {

    std::vector<Bucket> oldTable = table;

    table.clear();
    table.resize(oldTable.size() * 2 + 1);

    itemCount = 0;
    deletedCount = 0;

    for (const Bucket& oldBucket : oldTable) {

        if (oldBucket.state == BucketState::OCCUPIED) {

            std::size_t position =
                findInsertPosition(oldBucket.key);

            table[position].key = oldBucket.key;
            table[position].productIDs = oldBucket.productIDs;
            table[position].state = BucketState::OCCUPIED;

            itemCount++;
        }
    }
}

// Insert a value and its associated product ID.
void HashIndex::insert(
    const IndexValue& value,
    int productID
    ) {

    // First check whether this key already exists.
    std::size_t existingPosition = findKey(value);

    if (existingPosition != table.size()) {

        std::vector<int>& ids =
            table[existingPosition].productIDs;

        // Avoid inserting the same product ID twice.
        for (int id : ids) {
            if (id == productID) {
                return;
            }
        }

        ids.push_back(productID);
        return;
    }

    // Resize before the load factor reaches 0.7.
    if ((itemCount + deletedCount + 1) * 10 >=
        table.size() * 7) {

        resizeTable();
    }

    std::size_t position = findInsertPosition(value);

    // Safety check.
    if (position == table.size()) {
        resizeTable();
        position = findInsertPosition(value);
    }

    if (table[position].state == BucketState::DELETED) {
        deletedCount--;
    }

    table[position].key = value;
    table[position].productIDs.clear();
    table[position].productIDs.push_back(productID);
    table[position].state = BucketState::OCCUPIED;

    itemCount++;
}

// Remove one product ID associated with a key.
bool HashIndex::remove(
    const IndexValue& value,
    int productID
    ) {

    std::size_t position = findKey(value);

    if (position == table.size()) {
        return false;
    }

    std::vector<int>& ids = table[position].productIDs;

    for (std::size_t i = 0; i < ids.size(); i++) {

        if (ids[i] == productID) {

            ids.erase(ids.begin() + i);

            // If no products remain for this key,
            // mark the bucket as deleted.
            if (ids.empty()) {

                table[position].state = BucketState::DELETED;
                itemCount--;
                deletedCount++;
            }

            return true;
        }
    }

    return false;
}

// Return all product IDs matching an exact value.
std::vector<int> HashIndex::exactSearch(
    const IndexValue& value
    ) const {

    std::size_t position = findKey(value);

    if (position == table.size()) {
        return {};
    }

    return table[position].productIDs;
}

// Hash tables do not efficiently support range searches.
std::vector<int> HashIndex::rangeSearch(
    const IndexValue& minimum,
    const IndexValue& maximum
    ) const {

    (void)minimum;
    (void)maximum;

    return {};
}

bool HashIndex::supportsRangeSearch() const {
    return false;
}

std::string HashIndex::getIndexType() const {
    return "Hash Table";
}

std::size_t HashIndex::getSize() const {
    return itemCount;
}

std::size_t HashIndex::getCapacity() const {
    return table.size();
}

double HashIndex::getLoadFactor() const {

    if (table.empty()) {
        return 0.0;
    }

    return static_cast<double>(itemCount) / table.size();
}
