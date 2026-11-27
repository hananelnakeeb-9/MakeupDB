#include "HashIndex.h"
#include <iostream>
#include <cassert>

int main() {

    HashIndex index;

    // TEST 1: Insert text values.
    index.insert(std::string("colourpop"), 101);
    index.insert(std::string("maybelline"), 102);
    index.insert(std::string("colourpop"), 103);

    std::vector<int> results =
        index.exactSearch(std::string("colourpop"));

    assert(results.size() == 2);
    assert(results[0] == 101);
    assert(results[1] == 103);

    std::cout << "TEST 1 PASSED: Duplicate text values\n";

    // TEST 2: Insert and search numeric values.
    index.insert(12.5, 201);
    index.insert(12.5, 202);

    results = index.exactSearch(12.5);

    assert(results.size() == 2);

    std::cout << "TEST 2 PASSED: Numeric values\n";

    // TEST 3: Delete one product ID.
    bool removed = index.remove(
        std::string("colourpop"), 101
        );

    assert(removed);

    results = index.exactSearch(std::string("colourpop"));

    assert(results.size() == 1);
    assert(results[0] == 103);

    std::cout << "TEST 3 PASSED: Deletion\n";

    // TEST 4: Delete a complete key.
    index.remove(std::string("colourpop"), 103);

    results = index.exactSearch(std::string("colourpop"));

    assert(results.empty());

    std::cout << "TEST 4 PASSED: Removing the last duplicate\n";

    // TEST 5: Automatic resizing.
    std::size_t oldCapacity = index.getCapacity();

    for (int i = 0; i < 100; i++) {
        index.insert(
            std::string("brand") + std::to_string(i),
            1000 + i
            );
    }

    assert(index.getCapacity() > oldCapacity);

    std::cout << "TEST 5 PASSED: Automatic resizing\n";

    // TEST 6: Range search is unsupported.
    assert(!index.supportsRangeSearch());

    results = index.rangeSearch(1.0, 20.0);

    assert(results.empty());

    std::cout << "TEST 6 PASSED: Range search limitation\n";

    std::cout << "\nALL HASH TABLE TESTS PASSED!\n";

    return 0;
}
