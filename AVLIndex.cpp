#include "AVLIndex.h"
#include <optional>
#include <string>
#include <variant>
#include <vector>

// ==================== NODE ====================

AVLIndex::Node::Node(
    const IndexValue& value,
    int productID
    ) {
    key = value;
    productIDs.push_back(productID);

    left = nullptr;
    right = nullptr;

    height = 1;
}

// ==================== CONSTRUCTOR / DESTRUCTOR ====================

AVLIndex::AVLIndex() {
    root = nullptr;
}

AVLIndex::~AVLIndex() {
    destroyTree(root);
}

void AVLIndex::destroyTree(Node* node) {

    if (node == nullptr) {
        return;
    }

    destroyTree(node->left);
    destroyTree(node->right);

    delete node;
}

// ==================== COMPARISON ====================

// Returns -1, 0, 1 for less, equal, greater.
// Returns 2 when the types differ (text vs numeric).
int AVLIndex::compareValues(
    const IndexValue& first,
    const IndexValue& second
    ) const {

    if (first.index() != second.index()) {
        return 2;
    }

    if (first == second) {
        return 0;
    }

    if (first < second) {
        return -1;
    }

    return 1;
}

// ==================== HEIGHT / BALANCE ====================

int AVLIndex::nodeHeight(Node* node) const {

    if (node == nullptr) {
        return 0;
    }

    return node->height;
}

int AVLIndex::balanceFactor(Node* node) const {

    if (node == nullptr) {
        return 0;
    }

    return nodeHeight(node->left) -
           nodeHeight(node->right);
}

void AVLIndex::updateHeight(Node* node) {

    if (node == nullptr) {
        return;
    }

    int leftHeight = nodeHeight(node->left);
    int rightHeight = nodeHeight(node->right);

    node->height = 1 +
                   (leftHeight > rightHeight ? leftHeight : rightHeight);
}

// ==================== ROTATIONS ====================

// Right rotation (LL case).
AVLIndex::Node* AVLIndex::rotateRight(Node* y) {

    Node* x = y->left;
    Node* middle = x->right;

    x->right = y;
    y->left = middle;

    updateHeight(y);
    updateHeight(x);

    return x;
}

// Left rotation (RR case).
AVLIndex::Node* AVLIndex::rotateLeft(Node* x) {

    Node* y = x->right;
    Node* middle = y->left;

    y->left = x;
    x->right = middle;

    updateHeight(x);
    updateHeight(y);

    return y;
}

// Restore AVL balance after a change.
AVLIndex::Node* AVLIndex::balance(Node* node) {

    if (node == nullptr) {
        return nullptr;
    }

    updateHeight(node);

    int factor = balanceFactor(node);

    // Left-heavy.
    if (factor > 1) {

        // LR case: rotate left first.
        if (balanceFactor(node->left) < 0) {
            node->left = rotateLeft(node->left);
        }

        return rotateRight(node);
    }

    // Right-heavy.
    if (factor < -1) {

        // RL case: rotate right first.
        if (balanceFactor(node->right) > 0) {
            node->right = rotateRight(node->right);
        }

        return rotateLeft(node);
    }

    return node;
}

// ==================== INSERTION ====================

AVLIndex::Node* AVLIndex::insertNode(
    Node* node,
    const IndexValue& value,
    int productID
    ) {

    if (node == nullptr) {
        return new Node(value, productID);
    }

    int comparison = compareValues(value, node->key);

    // Reject a different key type in the same tree.
    if (comparison == 2) {
        return node;
    }

    if (comparison < 0) {

        node->left = insertNode(node->left, value, productID);

    } else if (comparison > 0) {

        node->right = insertNode(node->right, value, productID);

    } else {

        // Duplicate key: add another product ID (if not already present).
        for (int id : node->productIDs) {
            if (id == productID) {
                return node;
            }
        }

        node->productIDs.push_back(productID);
        return node;
    }

    return balance(node);
}

void AVLIndex::insert(
    const IndexValue& value,
    int productID
    ) {
    root = insertNode(root, value, productID);
}

// ==================== SEARCH ====================

AVLIndex::Node* AVLIndex::findNode(
    Node* node,
    const IndexValue& value
    ) const {

    if (node == nullptr) {
        return nullptr;
    }

    int comparison = compareValues(value, node->key);

    if (comparison == 2) {
        return nullptr;
    }

    if (comparison == 0) {
        return node;
    }

    if (comparison < 0) {
        return findNode(node->left, value);
    }

    return findNode(node->right, value);
}

std::vector<int> AVLIndex::exactSearch(
    const IndexValue& value
    ) const {

    Node* result = findNode(root, value);

    if (result == nullptr) {
        return {};
    }

    return result->productIDs;
}

// ==================== MINIMUM / MAXIMUM HELPERS ====================

AVLIndex::Node* AVLIndex::findMinimum(Node* node) const {

    if (node == nullptr) {
        return nullptr;
    }

    while (node->left != nullptr) {
        node = node->left;
    }

    return node;
}

AVLIndex::Node* AVLIndex::findMaximum(Node* node) const {

    if (node == nullptr) {
        return nullptr;
    }

    while (node->right != nullptr) {
        node = node->right;
    }

    return node;
}

// ==================== DELETION ====================

// Removes an entire tree node, including all its product IDs.
// Used when replacing a node with its in-order successor.
AVLIndex::Node* AVLIndex::removeWholeNode(
    Node* node,
    const IndexValue& value
    ) {

    if (node == nullptr) {
        return nullptr;
    }

    int comparison = compareValues(value, node->key);

    // Text and numeric keys cannot be mixed.
    if (comparison == 2) {
        return node;
    }

    if (comparison < 0) {
        node->left = removeWholeNode(node->left, value);
    }
    else if (comparison > 0) {
        node->right = removeWholeNode(node->right, value);
    }
    else {

        // Case 1: No left child.
        if (node->left == nullptr) {
            Node* replacement = node->right;
            delete node;
            return replacement;
        }

        // Case 2: No right child.
        if (node->right == nullptr) {
            Node* replacement = node->left;
            delete node;
            return replacement;
        }

        // Case 3: Two children.
        // Replace this node with its in-order successor.
        Node* successor = findMinimum(node->right);

        // Copy the key first: the successor node is deleted below,
        // so we must not keep a reference into it.
        IndexValue successorKey = successor->key;

        node->key = successorKey;
        node->productIDs = successor->productIDs;

        node->right = removeWholeNode(node->right, successorKey);
    }

    return balance(node);
}

// Removes one product ID associated with a key.
AVLIndex::Node* AVLIndex::removeNode(
    Node* node,
    const IndexValue& value,
    int productID,
    bool& removed
    ) {

    if (node == nullptr) {
        return nullptr;
    }

    int comparison = compareValues(value, node->key);

    // Text and numeric keys cannot be mixed.
    if (comparison == 2) {
        return node;
    }

    if (comparison < 0) {
        node->left = removeNode(node->left, value, productID, removed);
    }
    else if (comparison > 0) {
        node->right = removeNode(node->right, value, productID, removed);
    }
    else {

        // Find the requested product ID.
        int position = -1;

        for (int i = 0;
             i < static_cast<int>(node->productIDs.size());
             i++) {

            if (node->productIDs[i] == productID) {
                position = i;
                break;
            }
        }

        // The product ID does not exist.
        if (position == -1) {
            return node;
        }

        // Remove only the requested product ID.
        node->productIDs.erase(node->productIDs.begin() + position);

        removed = true;

        // Keep the node if other IDs still use this key.
        if (!node->productIDs.empty()) {
            return node;
        }

        // No product IDs remain, so remove the tree node.

        // Case 1: No left child.
        if (node->left == nullptr) {
            Node* replacement = node->right;
            delete node;
            return replacement;
        }

        // Case 2: No right child.
        if (node->right == nullptr) {
            Node* replacement = node->left;
            delete node;
            return replacement;
        }

        // Case 3: Two children.
        // Copy the successor's key and ALL its product IDs,
        // then remove the entire successor node.
        Node* successor = findMinimum(node->right);

        IndexValue successorKey = successor->key;

        node->key = successorKey;
        node->productIDs = successor->productIDs;

        node->right = removeWholeNode(node->right, successorKey);
    }

    // Restore AVL balance after deletion.
    return balance(node);
}

// Public deletion function.
bool AVLIndex::remove(
    const IndexValue& value,
    int productID
    ) {

    bool removed = false;

    root = removeNode(root, value, productID, removed);

    return removed;
}

// ==================== RANGE SEARCH ====================

void AVLIndex::collectRange(
    Node* node,
    const IndexValue& minimum,
    const IndexValue& maximum,
    std::vector<int>& results
    ) const {

    if (node == nullptr) {
        return;
    }

    int lowerComparison = compareValues(node->key, minimum);
    int upperComparison = compareValues(node->key, maximum);

    if (lowerComparison == 2 || upperComparison == 2) {
        return;
    }

    if (lowerComparison > 0) {
        collectRange(node->left, minimum, maximum, results);
    }

    if (lowerComparison >= 0 && upperComparison <= 0) {
        for (int id : node->productIDs) {
            results.push_back(id);
        }
    }

    if (upperComparison < 0) {
        collectRange(node->right, minimum, maximum, results);
    }
}

std::vector<int> AVLIndex::rangeSearch(
    const IndexValue& minimum,
    const IndexValue& maximum
    ) const {

    std::vector<int> results;

    if (minimum.index() != maximum.index()) {
        return results;
    }

    if (compareValues(minimum, maximum) > 0) {
        return results;
    }

    collectRange(root, minimum, maximum, results);

    return results;
}

// ==================== ASCENDING / DESCENDING ====================

void AVLIndex::collectAscending(
    Node* node,
    std::vector<int>& results
    ) const {

    if (node == nullptr) {
        return;
    }

    collectAscending(node->left, results);

    for (int id : node->productIDs) {
        results.push_back(id);
    }

    collectAscending(node->right, results);
}

void AVLIndex::collectDescending(
    Node* node,
    std::vector<int>& results
    ) const {

    if (node == nullptr) {
        return;
    }

    collectDescending(node->right, results);

    for (int id : node->productIDs) {
        results.push_back(id);
    }

    collectDescending(node->left, results);
}

std::vector<int> AVLIndex::ascending() const {

    std::vector<int> results;

    collectAscending(root, results);

    return results;
}

std::vector<int> AVLIndex::descending() const {

    std::vector<int> results;

    collectDescending(root, results);

    return results;
}

// ==================== MINIMUM / MAXIMUM ====================

std::optional<IndexValue> AVLIndex::minimum() const {

    Node* node = findMinimum(root);

    if (node == nullptr) {
        return std::nullopt;
    }

    return node->key;
}

std::optional<IndexValue> AVLIndex::maximum() const {

    Node* node = findMaximum(root);

    if (node == nullptr) {
        return std::nullopt;
    }

    return node->key;
}

// ==================== TOP K / BOTTOM K ====================
// Note: "visited" counts distinct keys (tree nodes), not product IDs.

void AVLIndex::collectTopK(
    Node* node,
    std::size_t k,
    std::size_t& visited,
    std::vector<int>& results
    ) const {

    if (node == nullptr || visited >= k) {
        return;
    }

    collectTopK(node->right, k, visited, results);

    if (visited >= k) {
        return;
    }

    for (int id : node->productIDs) {
        results.push_back(id);
    }

    visited++;

    collectTopK(node->left, k, visited, results);
}

void AVLIndex::collectBottomK(
    Node* node,
    std::size_t k,
    std::size_t& visited,
    std::vector<int>& results
    ) const {

    if (node == nullptr || visited >= k) {
        return;
    }

    collectBottomK(node->left, k, visited, results);

    if (visited >= k) {
        return;
    }

    for (int id : node->productIDs) {
        results.push_back(id);
    }

    visited++;

    collectBottomK(node->right, k, visited, results);
}

std::vector<int> AVLIndex::topK(std::size_t k) const {

    std::vector<int> results;
    std::size_t visited = 0;

    collectTopK(root, k, visited, results);

    return results;
}

std::vector<int> AVLIndex::bottomK(std::size_t k) const {

    std::vector<int> results;
    std::size_t visited = 0;

    collectBottomK(root, k, visited, results);

    return results;
}

// ==================== HEIGHT / NODE COUNT ====================

int AVLIndex::getHeight() const {
    return nodeHeight(root);
}

int AVLIndex::countNodes(Node* node) const {

    if (node == nullptr) {
        return 0;
    }

    return 1 +
           countNodes(node->left) +
           countNodes(node->right);
}

int AVLIndex::getNodeCount() const {
    return countNodes(root);
}

// ==================== DISPLAY TREE ====================

void AVLIndex::displayTree(
    Node* node,
    std::string prefix,
    bool isLeft,
    std::string& output
    ) const {

    if (node == nullptr) {
        return;
    }

    output += prefix;

    output += isLeft ? "|-- " : "`-- ";

    if (std::holds_alternative<std::string>(node->key)) {
        output += std::get<std::string>(node->key);
    } else {
        output += std::to_string(std::get<double>(node->key));
    }

    output += " [IDs: ";

    for (std::size_t i = 0; i < node->productIDs.size(); i++) {

        output += std::to_string(node->productIDs[i]);

        if (i + 1 < node->productIDs.size()) {
            output += ", ";
        }
    }

    output += "]\n";

    displayTree(
        node->left,
        prefix + (isLeft ? "|   " : "    "),
        true,
        output
        );

    displayTree(
        node->right,
        prefix + (isLeft ? "|   " : "    "),
        false,
        output
        );
}

std::string AVLIndex::displayStructure() const {

    std::string output;

    if (root == nullptr) {
        return "Tree is empty.\n";
    }

    output += "Root: ";

    if (std::holds_alternative<std::string>(root->key)) {
        output += std::get<std::string>(root->key);
    } else {
        output += std::to_string(std::get<double>(root->key));
    }

    output += "\n";

    displayTree(root->left, "", true, output);
    displayTree(root->right, "", false, output);

    return output;
}

// ==================== INDEX ADT ====================

bool AVLIndex::supportsRangeSearch() const {
    return true;
}

std::string AVLIndex::getIndexType() const {
    return "AVL Tree";
}