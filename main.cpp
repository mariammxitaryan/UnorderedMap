#include "unorderedMapHeader.hpp"

int main() {
    using Map = UnorderedMap<std::string, int>;

    std::cout << "--- Creating maps ---" << std::endl;
    Map m1;  // default constructor
    std::cout << "m1.empty(): " << m1.empty() << "\n";
    std::cout << "m1.max_size(): " << m1.max_size() << "\n\n";

    // initializer-list constructor
    Map m5{{"hello", 100}, {"world", 200}};
    std::cout << "m5 contents: ";
    for (const auto& kv : m5) std::cout << kv.first << "=" << kv.second << " ";
    std::cout << "\n";

    std::cout << "--- Insertion and Access ---" << std::endl;
    auto [it1, inserted1] = m1.insert({"one", 1});
    std::cout << "Inserted {one,1}: " << inserted1 << "\n";
    m1.emplace("two", 2);
    m1["three"] = 3;
    std::cout << "m1.at(\"three\"): " << m1.at("three") << "\n";
    std::cout << "m1[\"four\"] default-initialized to " << m1["four"] << "\n";

    std::cout << "\n--- Lookup ---" << std::endl;
    std::cout << "m1.contains(\"two\"): " << m1.contains("two") << "\n";
    std::cout << "m1.count(\"five\"): " << m1.count("five") << "\n";
    if (auto it2 = m1.find("two"); it2 != m1.end())
        std::cout << "Found two via find(): " << it2->second << "\n";

    std::cout << "\n--- Traversal ---" << std::endl;
    std::cout << "Elements in m1: ";
    for (auto it = m1.begin(); it != m1.end(); ++it)
        std::cout << it->first << "=" << it->second << " ";
    std::cout << "\n";
    std::cout << "(Using range-for) ";
    for (auto& kv : m1)
        std::cout << kv.first << "=" << kv.second << " ";
    std::cout << "\n";

    std::cout << "\n--- Bucket Info & Rehashing ---" << std::endl;
    std::cout << "bucket_count: " << m1.bucket_count() << "\n";
    std::cout << "load_factor: " << m1.load_factor() << "\n";
    std::cout << "max_load_factor: " << m1.max_load_factor() << "\n";
    m1.max_load_factor(0.5f);
    std::cout << "After setting max_load_factor to 0.5, buckets: " << m1.bucket_count()
              << ", load_factor: " << m1.load_factor() << "\n";
    m1.rehash(32);
    std::cout << "After rehash(32), bucket_count: " << m1.bucket_count() << "\n";
    std::cout << "bucket_size(0): " << m1.bucket_size(0) << "\n";
    std::cout << "bucket index for \"one\": " << m1.bucket("one") << "\n";

    std::cout << "Hash of 'one': " << m1.hash_function()("one") << "\n";
    std::cout << "Key equal('one','two'): " << m1.key_eq()("one","two") << "\n";

    std::cout << "\n--- Element Removal ---" << std::endl;
    std::cout << "erase('two'): " << m1.erase("two") << " (1=removed,0=absent)\n";

    std::cout << "\n--- Copy, Move, Swap ---" << std::endl;
    Map m2 = m1;  // copy-constructor
    std::cout << "m2 (copied from m1): ";
    for (auto& kv: m2) std::cout << kv.first << "=" << kv.second << " ";
    std::cout << "\n";

    Map m3{{"A", 10}, {"B", 20}};
    std::cout << "Before swap: m2.size()=" << m2.size() << ", m3.size()=" << m3.size() << "\n";
    m2.swap(m3);
    std::cout << "After swap: m2: "; for (auto& kv: m2) std::cout << kv.first << "=" << kv.second << " "; std::cout << "\n";
    std::cout << "After swap: m3: "; for (auto& kv: m3) std::cout << kv.first << "=" << kv.second << " "; std::cout << "\n";

    Map m4 = std::move(m3);  // move-constructor
    std::cout << "m4 (moved from m3): "; for (auto& kv: m4) std::cout << kv.first << "=" << kv.second << " "; std::cout << "\n";
    std::cout << "m3.empty() after move: " << m3.empty() << "\n";

    m4.clear();
    std::cout << "m4.empty() after clear(): " << m4.empty() << "\n";

    return 0;
}
