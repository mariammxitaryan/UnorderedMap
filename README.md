This project provides a custom implementation of an unordered map data structure in C++. It aims to mimic the functionality of `std::unordered_map` from the C++ Standard Library, offering efficient average-case constant-time complexity for insertion, deletion, and lookup operations.

This implementation utilizes a hash table with separate chaining to handle collisions. It is designed to be flexible, allowing users to specify custom hash functions and key equality predicates.

## <img src="https://img.icons8.com/fluent/24/000000/gear.png"/> Features

* **Generic Key-Value Storage:** Supports arbitrary key and value types.
* **Customizable Hashing:** Allows users to provide their own hash function objects.
* **Customizable Key Equality:** Enables users to define their own key comparison logic.
* **Dynamic Resizing:** Automatically adjusts the number of buckets to maintain performance as the number of elements grows.
* **Standard Library Inspired API:** Provides a familiar interface similar to `std::unordered_map`.
* **Iterators:** Supports both regular and constant iterators for traversing the map.
* **Basic Operations:** Includes essential functions like `insert`, `emplace`, `erase`, `find`, `count`, `contains`, `clear`, `empty`, `size`.
* **Bucket Management:** Offers functions to inspect the number of buckets, load factor, and bucket sizes.

## <img src="https://img.icons8.com/fluent/24/000000/wrench.png"/> Getting Started

### Prerequisites

* A C++ compiler that supports C++11 or later (e.g., g++, clang++).

### Usage

1.  **Include the Header:** Include the `unorderedMapHeader.hpp` file in your C++ project.
2.  **Include the Implementation:** Ensure that the implementation file (likely named something like `unorderedMap.tpp` or similar, depending on how you structure your build) is compiled and linked with your main code. If you are using template implementations in header files, you might just need to include the header.

```c++
#include "unorderedMapHeader.hpp"
#include <iostream>
#include <string>
#include <functional> // For std::hash and std::equal_to

int main() {
    // Create an UnorderedMap with default hash and equality for std::string
    UnorderedMap<std::string, int> myMap;

    // Insert some key-value pairs
    myMap["apple"] = 1;
    myMap["banana"] = 2;
    myMap.insert({"cherry", 3});
    myMap.emplace("date", 4);

    // Access elements
    std::cout << "Value of apple: " << myMap["apple"] << std::endl;

    // Find an element
    auto it = myMap.find("banana");
    if (it != myMap.end()) {
        std::cout << "Found " << it->first << " with value " << it->second << std::endl;
    }

    // Iterate through the map
    std::cout << "Elements in the map:" << std::endl;
    for (const auto& pair : myMap) {
        std::cout << "[" << pair.first << ": " << pair.second << "] ";
    }
    std::cout << std::endl;

    return 0;
}
````

### Custom Hash and Equality

You can provide custom hash functions and key equality predicates when creating an `UnorderedMap`:

```c++
#include "unorderedMapHeader.hpp"
#include <iostream>
#include <string>

// Custom hash function for strings (example)
struct StringHash {
    size_t operator()(const std::string& s) const {
        size_t hash = 0;
        for (char c : s) {
            hash = hash * 31 + c;
        }
        return hash;
    }
};

// Custom key equality predicate for strings (example - redundant for std::string)
struct StringEqual {
    bool operator()(const std::string& s1, const std::string& s2) const {
        return s1 == s2;
    }
};

int main() {
    UnorderedMap<std::string, int, StringHash, StringEqual> customMap;
    customMap["hello"] = 10;
    std::cout << "Value of hello in custom map: " << customMap["hello"] << std::endl;
    return 0;
}
```

### Contributing

Contributions to this project are welcome\! If you find any bugs or have suggestions for improvements, please feel free to open an issue or submit a pull request.

Please follow these guidelines when contributing:

  * Fork the repository.
  * Create a new branch for your feature or bug fix.
  * Make your changes and ensure they are well-tested.
  * Submit a pull request with a clear description of your changes.

