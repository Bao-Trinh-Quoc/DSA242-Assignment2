#ifndef INVENTORY_COMPRESSOR_H
#define INVENTORY_COMPRESSOR_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <utility>
#include "inventory.h"
#include "hash/xMap.h"
#include "heap/Heap.h"
#include "list/XArrayList.h"


namespace std {
    inline ostream& operator<<(ostream& os, const pair<char, int>& p) {
        os << "(" << p.first << ", " << p.second << ")";
        return os;
    }
}


template<int treeOrder>
class HuffmanTree {
public:
    struct HuffmanNode {
        char symbol;
        int freq;
        int insertOrder;  // Add insertion order to track priority
        XArrayList<HuffmanNode*> children;

        // HuffmanNode(char s, int f); //Leaf node
        // HuffmanNode(int f, const  XArrayList<HuffmanNode*>& childs); //Internal node

        HuffmanNode(char s, int f, int order = -1) : symbol(s), freq(f), insertOrder(order), children() {} //Leaf node
        HuffmanNode(int f, const XArrayList<HuffmanNode*>& childs) : symbol('\0'), freq(f), insertOrder(-1), children(childs) {} // Internal node
    };

    HuffmanTree();
    ~HuffmanTree();

    void build(XArrayList<pair<char, int>>& symbolsFreqs);
    void generateCodes(xMap<char, std::string>& table);
    std::string decode(const std::string& huffmanCode);

private:
    HuffmanNode* root;

    // Helper functions
    void deleteTree(HuffmanNode* node);
    void generateCodesHelper(HuffmanNode* node, const string& prefix, xMap<char, std::string>& table);
};

template<int treeOrder>
class InventoryCompressor {
public:
    InventoryCompressor(InventoryManager* manager);
    ~InventoryCompressor();

    void buildHuffman();
    void printHuffmanTable();
    std::string productToString(const List1D<InventoryAttribute>& attributes, const std::string& name);
    std::string encodeHuffman(const List1D<InventoryAttribute>& attributes, const std::string& name);
    std::string decodeHuffman(const std::string& huffmanCode, List1D<InventoryAttribute>& attributesOutput, std::string& nameOutput);

private:
    xMap<char, std::string>* huffmanTable;
    InventoryManager* invManager;
    HuffmanTree<treeOrder>* tree;

    
};


#endif // INVENTORY_COMPRESSOR_H

template <int treeOrder>
HuffmanTree<treeOrder>::HuffmanTree() : root(nullptr)
{
}

template <int treeOrder>
HuffmanTree<treeOrder>::~HuffmanTree()
{
    //TODO
    deleteTree(root);
    root = nullptr;
}

template <int treeOrder>
void HuffmanTree<treeOrder>::build(XArrayList<pair<char, int>>& symbolsFreqs)
{
    deleteTree(root);
    root = nullptr;

    // handle case when there is just one node for n-ary huffman
    if (symbolsFreqs.size() == 1) {
        char symbol = symbolsFreqs.get(0).first;
        int freq = symbolsFreqs.get(0).second;
         

        // For n-ary tree (n > 2) with a single character
        XArrayList<HuffmanNode*> children;
        
        // No need to add dummy nodes if the frequency is 0
        if (freq == 0) {
            HuffmanNode* leaf = new HuffmanNode(symbol, freq, 0);
            children.add(leaf);
            root = new HuffmanNode(freq, children);
            return;
        }

        // For non-zero frequency in n-ary trees (n > 2)
        // Add treeOrder-1 dummy nodes with frequency 0
        for (int i = 0; i < treeOrder-1; i++) {
            HuffmanNode* dummy = new HuffmanNode('\0', 0, i);
            children.add(dummy);
        }
       
        // Add the actual character node last
        HuffmanNode* leaf = new HuffmanNode(symbol, freq, treeOrder-1);
        children.add(leaf);
       
        // Create the root node with all children
        root = new HuffmanNode(freq, children);
        return;
    }

    int L = symbolsFreqs.size();
    int remainder = (L - 1) % (treeOrder - 1);
    int dummyCount = remainder == 0 ? 0 : (treeOrder - 1) - remainder;
    
    int insertionCounter = 0;
    int (*compareNodes)(HuffmanNode*&, HuffmanNode*&) = [](HuffmanNode*& a, HuffmanNode*& b) -> int {
        if (a->freq != b->freq) {
            return (a->freq < b->freq) ? -1 : 1;
        }
        
        // For equal frequencies:
        // 1. Both dummy nodes - maintain insertion order
        if (a->symbol == '\0' && b->symbol == '\0') {
            return (a->insertOrder < b->insertOrder) ? -1 : 1;
        }
        // 2. Dummy nodes have lowest priority
        if (a->symbol == '\0') return 1;
        if (b->symbol == '\0') return -1;
        // 3. For real nodes with equal frequencies, earlier insertion has higher priority
        return (a->insertOrder < b->insertOrder) ? -1 : 1;
    };
    
    Heap<HuffmanNode*> minHeap(compareNodes);
    
    // First add regular nodes in original order
    for (int i = 0; i < symbolsFreqs.size(); i++) {
        HuffmanNode* node = new HuffmanNode(symbolsFreqs.get(i).first, symbolsFreqs.get(i).second, insertionCounter++);
        minHeap.push(node);
    }
    
    // Add dummy nodes at the end
    for (int i = 0; i < dummyCount; i++) {
        HuffmanNode* dummy = new HuffmanNode('\0', 0, insertionCounter++);
        minHeap.push(dummy);
    }
    
    while (minHeap.size() > 1) {
        int nodesToMerge = min(treeOrder, minHeap.size());
        XArrayList<HuffmanNode*> children;
        int totalFreq = 0;
        
        for (int i = 0; i < nodesToMerge; i++) {
            HuffmanNode* node = minHeap.pop();
            totalFreq += node->freq;
            children.add(node);
        }
        
        HuffmanNode* internal = new HuffmanNode(totalFreq, children);
        internal->insertOrder = insertionCounter++;  // Give internal nodes an insertion order too
        minHeap.push(internal);
    }
    
    if (!minHeap.empty()) {
        root = minHeap.pop();
    }
}

template <int treeOrder>
void HuffmanTree<treeOrder>::generateCodes(xMap<char, std::string> &table)
{
    //TODO
    if (root == nullptr) {
        return;
    }
    
    // Start recursive generation with empty prefix
    generateCodesHelper(root, "", table);
}

template <int treeOrder>
std::string HuffmanTree<treeOrder>::decode(const std::string &huffmanCode)
{
    if (root == nullptr || huffmanCode.empty()) {
        return "";
    }
    
    std::string result;
    HuffmanNode* currentNode = root;
    
    for (char c : huffmanCode) {
        int childIndex;
        if (isdigit(c)) {
            childIndex = c - '0';
        } else if (c >= 'a' && c <= 'f') {
            childIndex = 10 + (c - 'a');
        } else if (c >= 'A' && c <= 'F') {
            childIndex = 10 + (c - 'A');
        } else {
            return "";  // Invalid character
        }
        
        if (childIndex < 0 || childIndex >= currentNode->children.size()) {
            return "";  // Invalid index
        }
        
        currentNode = currentNode->children.get(childIndex);
        
        if (currentNode->children.size() == 0) {
            // Only add non-dummy characters
            if (currentNode->symbol != '\0') {
                result += currentNode->symbol;
            }
            currentNode = root;
        }
    }
    
    // If we didn't end at root, the code was invalid
    if (currentNode != root) {
        return "";
    }
    
    return result;
}

template <int treeOrder>
void HuffmanTree<treeOrder>::deleteTree(HuffmanNode* node) {
    if (node == nullptr) {
        return;
    }
    
    // Delete all children recursively
    for (int i = 0; i < node->children.size(); i++) {
        deleteTree(node->children.get(i));
    }
    
    // Delete the node itself
    delete node;
}

template <int treeOrder>
void HuffmanTree<treeOrder>::generateCodesHelper(HuffmanNode* node, const string& prefix, xMap<char, std::string>& table) {
    if (node == nullptr) {
        return;
    }
    
    // If this is a leaf node (no children), it has a symbol to encode
    if (node->children.size() == 0 && node->symbol != '\0') {
        table.put(node->symbol, prefix);
        return;
    }
    
    // For each child, append its index to the code and recurse
    for (int i = 0; i < node->children.size(); i++) {
        // Convert index to appropriate character based on value
        string childCode;
        if (i < 10) {
            childCode = '0' + i;  // digits 0-9
        } else {
            childCode = 'a' + (i - 10);  // letters a-f for 10-15
        }
        generateCodesHelper(node->children.get(i), prefix + childCode, table);
    }
}




template <int treeOrder>
InventoryCompressor<treeOrder>::InventoryCompressor(InventoryManager *manager)
{
    //TODO
    this->invManager = manager;
    this->huffmanTable = new xMap<char, std::string>(
        [](char& c, int capacity) -> int { 
            return static_cast<int>(c) % capacity; 
        }
    );
    this->tree = new HuffmanTree<treeOrder>();
}

template <int treeOrder>
InventoryCompressor<treeOrder>::~InventoryCompressor()
{
    //TODO
    delete huffmanTable;
    delete tree;
}

template <int treeOrder>
void InventoryCompressor<treeOrder>::buildHuffman()
{
    //TODO

    // Create frequency map
    xMap<char, int> freqMap(
        [](char& c, int capacity) -> int { 
            return static_cast<int>(c) % capacity; 
        }
    );

    // Count frequencies for all products
    for (int i = 0; i < invManager->size(); i++) {
        std::string productStr = productToString(
            invManager->getProductAttributes(i),
            invManager->getProductName(i)
        );
        
        for (char c : productStr) {
            if (freqMap.containsKey(c)) {
                freqMap.put(c, freqMap.get(c) + 1);
            } else {
                freqMap.put(c, 1);
            }
        }
    }
    
    // Convert to list of pairs for Huffman tree
    XArrayList<std::pair<char, int>> symbolFreqs;
    DLinkedList<char> keys = freqMap.keys();
    for (char c : keys) {
        symbolFreqs.add(std::make_pair(c, freqMap.get(c)));
    }
    
    // Build Huffman tree and generate codes
    tree->build(symbolFreqs);
    tree->generateCodes(*huffmanTable);
}

template <int treeOrder>
void InventoryCompressor<treeOrder>::printHuffmanTable() {
    DLinkedList<char> keys = huffmanTable->keys();
    for (char ch : keys) {
        std::cout << "'" << ch << "' : " << huffmanTable->get(ch) << std::endl;
    }
}

template <int treeOrder>
std::string InventoryCompressor<treeOrder>::productToString(const List1D<InventoryAttribute> &attributes, const std::string &name)
{
    //TODO
    stringstream ss;
    ss << name << ":";

    for (int i = 0; i < attributes.size(); i++) {
        if (i > 0) 
            ss << ", ";

        ss << "(" << attributes.get(i).name << ":" << std::fixed << std::setprecision(6)
           << attributes.get(i).value << ")";
    }


    return ss.str();
}

template <int treeOrder>
std::string InventoryCompressor<treeOrder>::encodeHuffman(const List1D<InventoryAttribute> &attributes, const std::string &name)
{
    //TODO
    std::string productStr = productToString(attributes, name);
    std::string encoded;
    
    for (char c : productStr) {
        encoded += huffmanTable->get(c);
    }
    
    return encoded;
}

template <int treeOrder>
std::string InventoryCompressor<treeOrder>::decodeHuffman(const std::string &huffmanCode, List1D<InventoryAttribute> &attributesOutput, std::string &nameOutput)
{
    //TODO
    // Decode the Huffman code
    std::string decoded = tree->decode(huffmanCode);
    if (decoded.empty()) return "";
    
    // Parse the decoded string
    size_t colonPos = decoded.find(':');
    if (colonPos == std::string::npos) return decoded;
    
    // Extract name
    nameOutput = decoded.substr(0, colonPos);
    
    // Parse attributes
    size_t pos = colonPos + 1;
    
    while (pos < decoded.length()) {
        // Find start of attribute
        size_t openParen = decoded.find('(', pos);
        if (openParen == std::string::npos) break;
        
        // Find end of attribute
        size_t closeParen = decoded.find(')', openParen);
        if (closeParen == std::string::npos) break;
        
        // Extract attribute string
        std::string attrStr = decoded.substr(openParen + 1, closeParen - openParen - 1);
        size_t colonPos = attrStr.find(':');
        if (colonPos != std::string::npos) {
            std::string attrName = attrStr.substr(0, colonPos);
            double attrValue = std::stod(attrStr.substr(colonPos + 1));
            attributesOutput.add(InventoryAttribute(attrName, attrValue));
        }
        
        pos = closeParen + 1;
    }
    
    return decoded;
}