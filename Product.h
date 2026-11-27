
#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>

struct Product {
    int id;

    std::string name;
    std::string brand;
    std::string category;
    std::string productType;

    double price;
    std::string currency;

    double rating;
    bool hasRating;

    int shadeCount;
};

#endif
