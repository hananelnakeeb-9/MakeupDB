#ifndef AVLINDEX_H
#define AVLINDEX_H

#include "Index.h"

#include <vector>
#include <string>
#include <optional>
#include <cstddef>

class AVLIndex : public Index {
private:

    struct Node {
        IndexValue key;
        std::vector<int> productIDs;

        Node* left;
        Node* right;

        int height;

        Node(const IndexValue& value, int productID);
    };
Node* removeWholeNode(Node* node, const IndexValue& value);
    Node* root;

    // Basic AVL operations.
    int nodeHeight(Node* node) const;
    int balanceFactor(Node* node) const;
    void updateHeight(Node* node);

    Node* rotateRight(Node* node);
    Node* rotateLeft(Node* node);
    Node* balance(Node* node);

    // Recursive insertion and deletion.
    Node* insertNode(
        Node* node,
        const IndexValue& value,
        int productID
        );

    Node* removeNode(
        Node* node,
        const IndexValue& value,
        int productID,
        bool& removed
        );

    // Search helpers.
    Node* findNode(
        Node* node,
        const IndexValue& value
        ) const;

    Node* findMinimum(Node* node) const;
    Node* findMaximum(Node* node) const;

    // Tree traversal helpers.
    void collectAscending(
        Node* node,
        std::vector<int>& results
        ) const;

    void collectDescending(
        Node* node,
        std::vector<int>& results
        ) const;

    void collectRange(
        Node* node,
        const IndexValue& minimum,
        const IndexValue& maximum,
        std::vector<int>& results
        ) const;

    void collectTopK(
        Node* node,
        std::size_t k,
        std::size_t& visited,
        std::vector<int>& results
        ) const;

    void collectBottomK(
        Node* node,
        std::size_t k,
        std::size_t& visited,
        std::vector<int>& results
        ) const;

    int countNodes(Node* node) const;

    void displayTree(
        Node* node,
        std::string prefix,
        bool isLeft,
        std::string& output
        ) const;

    void destroyTree(Node* node);

    // Compare two keys of the same type.
    int compareValues(
        const IndexValue& first,
        const IndexValue& second
        ) const;

public:

    AVLIndex();
    ~AVLIndex() override;

    // Prevent accidental shallow copies of tree pointers.
    AVLIndex(const AVLIndex&) = delete;
    AVLIndex& operator=(const AVLIndex&) = delete;

    void insert(
        const IndexValue& value,
        int productID
        ) override;

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

    // F5 additional operations.
    std::vector<int> ascending() const;
    std::vector<int> descending() const;

    std::optional<IndexValue> minimum() const;
    std::optional<IndexValue> maximum() const;

    std::vector<int> topK(std::size_t k) const;
    std::vector<int> bottomK(std::size_t k) const;

    int getHeight() const;
    int getNodeCount() const;

    std::string displayStructure() const;
};

#endif
