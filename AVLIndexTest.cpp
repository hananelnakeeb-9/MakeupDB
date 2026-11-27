#include "AVLIndex.h"
#include <iostream>
#include <vector>
#include <string>
#include <optional>

using namespace std;

// Checks whether two vectors contain the same values.
bool checkVector(
    const vector<int>& actual,
    const vector<int>& expected
    ) {
    return actual == expected;
}

// Prints the result of each test.
bool testResult(bool condition, const string& testName) {
    if (condition) {
        cout << "TEST PASSED: " << testName << endl;
        return true;
    }

    cout << "TEST FAILED: " << testName << endl;
    return false;
}

int main() {

    AVLIndex avl;

    // ==========================================
    // TEST 1: INSERTION AND ASCENDING ORDER
    // ==========================================

    avl.insert(30.0, 300);
    avl.insert(20.0, 200);
    avl.insert(40.0, 400);
    avl.insert(10.0, 100);
    avl.insert(25.0, 250);
    avl.insert(35.0, 350);
    avl.insert(50.0, 500);

    vector<int> expectedAscending = {
        100, 200, 250, 300, 350, 400, 500
    };

    if (!testResult(
            checkVector(avl.ascending(), expectedAscending),
            "AVL insertion and ascending order"
            )) return 1;


    // ==========================================
    // TEST 2: DUPLICATE VALUES
    // ==========================================

    avl.insert(35.0, 351);

    vector<int> expectedDuplicates = {350, 351};

    if (!testResult(
            checkVector(avl.exactSearch(35.0), expectedDuplicates),
            "Duplicate values"
            )) return 1;


    // ==========================================
    // TEST 3: EXACT SEARCH
    // ==========================================

    vector<int> expectedExact = {250};

    if (!testResult(
            checkVector(avl.exactSearch(25.0), expectedExact),
            "Exact search"
            )) return 1;


    // ==========================================
    // TEST 4: RANGE SEARCH
    // ==========================================

    vector<int> expectedRange = {
        200, 250, 300, 350, 351, 400
    };

    if (!testResult(
            checkVector(
                avl.rangeSearch(20.0, 40.0),
                expectedRange
                ),
            "Range search"
            )) return 1;


    // ==========================================
    // TEST 5: DESCENDING ORDER
    // ==========================================

    vector<int> expectedDescending = {
        500, 400, 350, 351, 300, 250, 200, 100
    };

    if (!testResult(
            checkVector(avl.descending(), expectedDescending),
            "Descending order"
            )) return 1;


    // ==========================================
    // TEST 6: MINIMUM AND MAXIMUM
    // ==========================================

    optional<IndexValue> minimumValue = avl.minimum();
    optional<IndexValue> maximumValue = avl.maximum();

    bool correctMinimum =
        minimumValue.has_value() &&
        get<double>(*minimumValue) == 10.0;

    bool correctMaximum =
        maximumValue.has_value() &&
        get<double>(*maximumValue) == 50.0;

    if (!testResult(
            correctMinimum && correctMaximum,
            "Minimum and maximum"
            )) return 1;


    // ==========================================
    // TEST 7: TOP K
    // ==========================================

    vector<int> expectedTopK = {500, 400};

    if (!testResult(
            checkVector(avl.topK(2), expectedTopK),
            "Top K"
            )) return 1;


    // ==========================================
    // TEST 8: BOTTOM K
    // ==========================================

    vector<int> expectedBottomK = {100, 200};

    if (!testResult(
            checkVector(avl.bottomK(2), expectedBottomK),
            "Bottom K"
            )) return 1;


    // ==========================================
    // TEST 9: DELETION WITH TWO CHILDREN
    // ==========================================

    bool removed30 = avl.remove(30.0, 300);

    vector<int> expectedAfterDeletion = {350, 351};

    if (!testResult(
            removed30 &&
                checkVector(
                    avl.exactSearch(35.0),
                    expectedAfterDeletion
                    ),
            "Deletion with two children and duplicate successor"
            )) return 1;


    // ==========================================
    // TEST 10: DELETING ONE DUPLICATE
    // ==========================================

    bool removed350 = avl.remove(35.0, 350);

    vector<int> expectedAfterDuplicateDeletion = {351};

    if (!testResult(
            removed350 &&
                checkVector(
                    avl.exactSearch(35.0),
                    expectedAfterDuplicateDeletion
                    ),
            "Deleting one duplicate"
            )) return 1;


    // ==========================================
    // TEST 11: HEIGHT AND NODE COUNT
    // ==========================================

    bool correctHeight = avl.getHeight() > 0;
    bool correctNodeCount = avl.getNodeCount() == 6;

    if (!testResult(
            correctHeight && correctNodeCount,
            "Height and node count"
            )) return 1;


    // ==========================================
    // TEST 12: RANGE SEARCH SUPPORT
    // ==========================================

    if (!testResult(
            avl.supportsRangeSearch(),
            "Range search support"
            )) return 1;


    // ==========================================
    // TEST 13: TREE STRUCTURE DISPLAY
    // ==========================================

    string tree = avl.displayStructure();

    if (!testResult(
            !tree.empty(),
            "Tree structure display"
            )) return 1;


    // ==========================================
    // FINAL RESULT
    // ==========================================

    cout << endl;
    cout << "ALL AVL TREE TESTS PASSED!" << endl;

    cout << endl;
    cout << "AVL TREE STRUCTURE:" << endl;
    cout << tree << endl;

    cout << "Tree height: " << avl.getHeight() << endl;
    cout << "Number of nodes: " << avl.getNodeCount() << endl;

    return 0;
}
