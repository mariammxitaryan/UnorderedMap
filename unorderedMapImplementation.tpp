#include "unorderedMapHeader.hpp"

//******************************************************************************
//* @brief Constructs an empty UnorderedMap with a specified initial number   *
//* of buckets, a hash function, and a key equality predicate.        *
//* *
//* @param bucket_count The initial number of buckets to allocate.           *
//* @param hash         The hash function object to use for key hashing.      *
//* @param equal        The key equality predicate object to use for comparing*
//* keys.                                                *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
UnorderedMap<Key,T,Hash,KeyEqual>::UnorderedMap(size_type bucket_count, const Hash& hash, const KeyEqual& equal)
    : buckets_(bucket_count), num_elements_(0), max_load_factor_(1.0f), hasher_(hash), equal_(equal) {}

//******************************************************************************
//* @brief Constructs an UnorderedMap with elements from an initializer list, *
//* a specified initial number of buckets, a hash function, and a key   *
//* equality predicate.                                                *
//* *
//* @param init         The initializer list containing key-value pairs to    *
//* insert.                                                *
//* @param bucket_count The initial number of buckets to allocate.           *
//* @param hash         The hash function object to use for key hashing.      *
//* @param equal        The key equality predicate object to use for comparing*
//* keys.                                                *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
UnorderedMap<Key,T,Hash,KeyEqual>::UnorderedMap(std::initializer_list<value_type> init,
                                                  size_type bucket_count,
                                                  const Hash& hash,
                                                  const KeyEqual& equal)
    : UnorderedMap(bucket_count, hash, equal) {
    for (auto& kv : init) insert(kv);
}

//******************************************************************************
//* @brief Copy constructor. Constructs a new UnorderedMap as a copy of       *
//* another UnorderedMap.                                             *
//* *
//* @param other The UnorderedMap to copy from.                              *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
UnorderedMap<Key,T,Hash,KeyEqual>::UnorderedMap(const UnorderedMap& other)
    : buckets_(other.buckets_.size()), num_elements_(0), max_load_factor_(other.max_load_factor_), hasher_(other.hasher_), equal_(other.equal_) {
    for (auto& bucket : other.buckets_) {
        for (auto& kv : bucket) {
            insert(kv);
        }
    }
}

//******************************************************************************
//* @brief Move constructor. Constructs a new UnorderedMap by moving the       *
//* contents of another UnorderedMap.                                 *
//* *
//* @param other The UnorderedMap to move from. Its state becomes valid but   *
//* unspecified.                                                *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
UnorderedMap<Key,T,Hash,KeyEqual>::UnorderedMap(UnorderedMap&& other) noexcept
    : buckets_(std::move(other.buckets_)), num_elements_(other.num_elements_), max_load_factor_(other.max_load_factor_),
      hasher_(std::move(other.hasher_)), equal_(std::move(other.equal_)) {
    other.num_elements_ = 0;
}

//******************************************************************************
//* @brief Destructor. Clears the UnorderedMap and releases allocated memory. *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
UnorderedMap<Key,T,Hash,KeyEqual>::~UnorderedMap() {
    clear();
}

//******************************************************************************
//* @brief Copy assignment operator. Replaces the contents of this             *
//* UnorderedMap with a copy of another UnorderedMap.                  *
//* *
//* @param other The UnorderedMap to copy from.                              *
//* @return A reference to this UnorderedMap.                                 *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
UnorderedMap<Key,T,Hash,KeyEqual>& UnorderedMap<Key,T,Hash,KeyEqual>::operator=(const UnorderedMap& other) {
    if (this != &other) {
        clear();
        hasher_ = other.hasher_; equal_ = other.equal_;
        max_load_factor_ = other.max_load_factor_;
        buckets_.assign(other.buckets_.size(), {});
        for (auto& bucket : other.buckets_) {
            for (auto& kv : bucket) {
                insert(kv);
            }
        }
    }
    return *this;
}

//******************************************************************************
//* @brief Move assignment operator. Replaces the contents of this             *
//* UnorderedMap by moving the contents of another UnorderedMap.       *
//* *
//* @param other The UnorderedMap to move from. Its state becomes valid but   *
//* unspecified.                                                *
//* @return A reference to this UnorderedMap.                                 *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
UnorderedMap<Key,T,Hash,KeyEqual>& UnorderedMap<Key,T,Hash,KeyEqual>::operator=(UnorderedMap&& other) noexcept {
    if (this != &other) {
        buckets_ = std::move(other.buckets_);
        num_elements_ = other.num_elements_;
        max_load_factor_ = other.max_load_factor_;
        hasher_ = std::move(other.hasher_);
        equal_ = std::move(other.equal_);
        other.num_elements_ = 0;
    }
    return *this;
}

//******************************************************************************
//* @brief Assignment operator from an initializer list. Replaces the contents*
//* of this UnorderedMap with elements from the initializer list.       *
//* *
//* @param init The initializer list containing key-value pairs to insert.    *
//* @return A reference to this UnorderedMap.                                 *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
UnorderedMap<Key,T,Hash,KeyEqual>& UnorderedMap<Key,T,Hash,KeyEqual>::operator=(std::initializer_list<value_type> init) {
    clear();
    for (auto& kv : init) insert(kv);
    return *this;
}

//******************************************************************************
//* @brief Returns an iterator to the beginning of the UnorderedMap.           *
//* *
//* @return An iterator pointing to the first key-value pair in the map, or    *
//* the end iterator if the map is empty.                             *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::iterator UnorderedMap<Key,T,Hash,KeyEqual>::begin() {
    for (size_type i = 0; i < buckets_.size(); ++i) {
        if (!buckets_[i].empty())
            return iterator(this, i, buckets_[i].begin());
    }
    return end();
}

//******************************************************************************
//* @brief Returns an iterator to the end of the UnorderedMap.                 *
//* *
//* @return An iterator pointing past the last key-value pair in the map.      *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::iterator UnorderedMap<Key,T,Hash,KeyEqual>::end() {
    return iterator(this, buckets_.size(), {});
}

//******************************************************************************
//* @brief Returns a const iterator to the beginning of the UnorderedMap.     *
//* *
//* @return A const iterator pointing to the first key-value pair in the map,  *
//* or the end const iterator if the map is empty.                    *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::const_iterator UnorderedMap<Key,T,Hash,KeyEqual>::begin() const {
    for (size_type i = 0; i < buckets_.size(); ++i) {
        if (!buckets_[i].empty())
            return const_iterator(this, i, buckets_[i].cbegin());
    }
    return end();
}

//******************************************************************************
//* @brief Returns a const iterator to the end of the UnorderedMap.           *
//* *
//* @return A const iterator pointing past the last key-value pair in the map. *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::const_iterator UnorderedMap<Key,T,Hash,KeyEqual>::end() const {
    return const_iterator(this, buckets_.size(), {});
}

//******************************************************************************
//* @brief Checks if the UnorderedMap is empty (contains no elements).         *
//* *
//* @return True if the map is empty, false otherwise.                         *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
bool UnorderedMap<Key,T,Hash,KeyEqual>::empty() const {
    return num_elements_ == 0;
}

//******************************************************************************
//* @brief Returns the number of elements (key-value pairs) in the             *
//* UnorderedMap.                                                     *
//* *
//* @return The number of elements in the map.                                 *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::size_type UnorderedMap<Key,T,Hash,KeyEqual>::size() const {
    return num_elements_;
}

//******************************************************************************
//* @brief Returns the maximum number of elements the UnorderedMap can hold.   *
//* *
//* @return The theoretical maximum size of the map.                           *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::size_type UnorderedMap<Key,T,Hash,KeyEqual>::max_size() const {
    return std::numeric_limits<size_type>::max();
}

//******************************************************************************
//* @brief Clears the UnorderedMap, removing all elements.                     *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
void UnorderedMap<Key,T,Hash,KeyEqual>::clear() {
    for (auto& bucket : buckets_) bucket.clear();
    num_elements_ = 0;
}

//******************************************************************************
//* @brief Inserts a new key-value pair into the UnorderedMap.                *
//* *
//* @param kv The key-value pair to insert.                                   *
//* @return A pair containing an iterator to the inserted element (or the      *
//* existing element if the key already exists) and a boolean value    *
//* indicating whether a new element was inserted (true) or not       *
//* (false).                                                         *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
std::pair<typename UnorderedMap<Key,T,Hash,KeyEqual>::iterator, bool>
UnorderedMap<Key,T,Hash,KeyEqual>::insert(const value_type& kv) {
    rehash_if_needed();
    size_type idx = bucket(kv.first);
    for (auto it = buckets_[idx].begin(); it != buckets_[idx].end(); ++it) {
        if (equal_(it->first, kv.first)) {
            return { iterator(this, idx, it), false };
        }
    }
    buckets_[idx].push_back(kv);
    ++num_elements_;
    auto it = std::prev(buckets_[idx].end());
    return { iterator(this, idx, it), true };
}

//******************************************************************************
//* @brief Emplaces a new key-value pair into the UnorderedMap by constructing*
//* the value in-place.                                                *
//* *
//* @param args Arguments to forward to the constructor of the value_type.     *
//* @return A pair containing an iterator to the emplaced element (or the      *
//* existing element if the key already exists) and a boolean value    *
//* indicating whether a new element was emplaced (true) or not       *
//* (false).                                                         *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
template<class... Args>
std::pair<typename UnorderedMap<Key,T,Hash,KeyEqual>::iterator, bool>
UnorderedMap<Key,T,Hash,KeyEqual>::emplace(Args&&... args) {
    value_type kv(std::forward<Args>(args)...);
    return insert(kv);
}

//******************************************************************************
//* @brief Erases the element with the specified key from the UnorderedMap.    *
//* *
//* @param key The key of the element to erase.                               *
//* @return The number of elements erased (either 0 or 1).                     *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::size_type
UnorderedMap<Key,T,Hash,KeyEqual>::erase(const Key& key) {
    size_type idx = bucket(key);
    auto& lst = buckets_[idx];
    for (auto it = lst.begin(); it != lst.end(); ++it) {
        if (equal_(it->first, key)) {
            lst.erase(it);
            --num_elements_;
            return 1;
        }
    }
    return 0;
}

//******************************************************************************
//* @brief Swaps the contents of this UnorderedMap with the contents of       *
//* another UnorderedMap. Does not invalidate iterators.                *
//* *
//* @param other The other UnorderedMap to swap with.                         *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
void UnorderedMap<Key,T,Hash,KeyEqual>::swap(UnorderedMap& other) noexcept {
    using std::swap;
    swap(buckets_, other.buckets_);
    swap(num_elements_, other.num_elements_);
    swap(max_load_factor_, other.max_load_factor_);
    swap(hasher_, other.hasher_);
    swap(equal_, other.equal_);
}

//******************************************************************************
//* @brief Accesses the element at the specified key. Throws an exception if   *
//* the key is not found.                                              *
//* *
//* @param key The key of the element to access.                               *
//* @return A reference to the value associated with the key.                  *
//* @throws std::out_of_range If the key is not found in the map.              *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
T& UnorderedMap<Key,T,Hash,KeyEqual>::at(const Key& key) {
    auto it = find(key);
    if (it == end()) throw std::out_of_range("Key not found");
    return it->second;
}

//******************************************************************************
//* @brief Accesses the element at the specified key (const version). Throws  *
//* an exception if the key is not found.                             *
//* *
//* @param key The key of the element to access.                               *
//* @return A const reference to the value associated with the key.            *
//* @throws std::out_of_range If the key is not found in the map.              *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
const T& UnorderedMap<Key,T,Hash,KeyEqual>::at(const Key& key) const {
    auto it = find(key);
    if (it == end()) throw std::out_of_range("Key not found");
    return it->second;
}

//******************************************************************************
//* @brief Accesses or inserts the element at the specified key. If the key   *
//* is not found, it inserts a new element with the key and a            *
//* default-constructed value.                                         *
//* *
//* @param key The key of the element to access or insert.                    *
//* @return A reference to the value associated with the key.                  *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
T& UnorderedMap<Key,T,Hash,KeyEqual>::operator[](const Key& key) {
    auto res = insert({key, T{}});
    return res.first->second;
}

//******************************************************************************
//* @brief Returns the number of elements with the specified key. Since keys  *
//* in an UnorderedMap are unique, this will always be 0 or 1.          *
//* *
//* @param key The key to search for.                                         *
//* @return 1 if an element with the specified key exists, 0 otherwise.       *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::size_type
UnorderedMap<Key,T,Hash,KeyEqual>::count(const Key& key) const {
    return contains(key) ? 1 : 0;
}

//******************************************************************************
//* @brief Finds the element with the specified key.                          *
//* *
//* @param key The key to search for.                                         *
//* @return An iterator to the element with the specified key, or the end      *
//* iterator if the key is not found.                                  *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::iterator
UnorderedMap<Key,T,Hash,KeyEqual>::find(const Key& key) {
    size_type idx = bucket(key);
    for (auto it = buckets_[idx].begin(); it != buckets_[idx].end(); ++it) {
        if (equal_(it->first, key)) {
            return iterator(this, idx, it);
        }
    }
    return end();
}

//******************************************************************************
//* @brief Finds the element with the specified key (const version).          *
//* *
//* @param key The key to search for.                                         *
//* @return A const iterator to the element with the specified key, or the end*
//* const iterator if the key is not found.                            *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::const_iterator
UnorderedMap<Key,T,Hash,KeyEqual>::find(const Key& key) const {
    size_type idx = bucket(key);
    for (auto it = buckets_[idx].cbegin(); it != buckets_[idx].cend(); ++it) {
        if (equal_(it->first, key)) {
            return const_iterator(this, idx, it);
        }
    }
    return end();
}

//******************************************************************************
//* @brief Checks if the UnorderedMap contains an element with the specified  *
//* key.                                                               *
//* *
//* @param key The key to search for.                                         *
//* @return True if an element with the specified key exists, false otherwise.*
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
bool UnorderedMap<Key,T,Hash,KeyEqual>::contains(const Key& key) const {
    return find(key) != end();
}

//******************************************************************************
//* @brief Returns the current number of buckets in the UnorderedMap.         *
//* *
//* @return The number of buckets.                                            *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::size_type
UnorderedMap<Key,T,Hash,KeyEqual>::bucket_count() const {
    return buckets_.size();
}

//******************************************************************************
//* @brief Returns the average number of elements per bucket (load factor).    *
//* *
//* @return The load factor of the UnorderedMap.                             *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
float UnorderedMap<Key,T,Hash,KeyEqual>::load_factor() const {
    return static_cast<float>(num_elements_) / buckets_.size();
}

//******************************************************************************
//* @brief Returns the maximum load factor used to trigger rehashing.         *
//* *
//* @return The maximum load factor.                                          *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
float UnorderedMap<Key,T,Hash,KeyEqual>::max_load_factor() const {
    return max_load_factor_;
}

//******************************************************************************
//* @brief Sets the maximum load factor, which determines when rehashing occurs.*
//* If the current load factor exceeds the new maximum, rehashing may occur. *
//* *
//* @param ml The new maximum load factor.                                   *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
void UnorderedMap<Key,T,Hash,KeyEqual>::max_load_factor(float ml) {
    max_load_factor_ = ml;
    rehash_if_needed();
}

//******************************************************************************
//* @brief Rehashes the UnorderedMap to have at least the specified number of   *
//* buckets. All existing elements are moved to the new buckets.           *
//* *
//* @param new_count The desired new number of buckets.                       *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
void UnorderedMap<Key,T,Hash,KeyEqual>::rehash(size_type new_count) {
    std::vector<std::list<value_type>> new_buckets(new_count);
    for (auto& bucket : buckets_) {
        for (auto& kv : bucket) {
            size_type idx = Hash(hasher_)(kv.first) % new_count;
            new_buckets[idx].push_back(std::move(const_cast<value_type&>(kv)));
        }
    }
    buckets_.swap(new_buckets);
}

//******************************************************************************
//* @brief Returns the number of elements in the specified bucket.            *
//* *
//* @param i The index of the bucket.                                         *
//* @return The number of elements in the i-th bucket.                       *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::size_type
UnorderedMap<Key,T,Hash,KeyEqual>::bucket_size(size_type i) const {
    return buckets_[i].size();
}

//******************************************************************************
//* @brief Returns the bucket index for the specified key.                    *
//* *
//* @param key The key to get the bucket index for.                           *
//* @return The index of the bucket where the key would be placed.          *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::size_type
UnorderedMap<Key,T,Hash,KeyEqual>::bucket(const Key& key) const {
    return hasher_(key) % buckets_.size();
}

//******************************************************************************
//* @brief Returns the hash function object used by the UnorderedMap.         *
//* *
//* @return The hash function object.                                         *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::hasher UnorderedMap<Key,T,Hash,KeyEqual>::hash_function() const {
    return hasher_;
}

//******************************************************************************
//* @brief Returns the key equality predicate object used by the              *
//* UnorderedMap.                                                     *
//* *
//* @return The key equality predicate object.                                *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::key_equal UnorderedMap<Key,T,Hash,KeyEqual>::key_eq() const {
    return equal_;
}

//******************************************************************************
//* @brief Checks if rehashing is needed based on the current load factor and *
//* the maximum load factor, and performs rehashing if necessary.           *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
void UnorderedMap<Key,T,Hash,KeyEqual>::rehash_if_needed() {
    if (load_factor() > max_load_factor_) {
        rehash(buckets_.size() * 2);
    }
}

//******************************************************************************
//* @brief Constructs an iterator for the UnorderedMap.                       *
//* *
//* @param map   A pointer to the UnorderedMap this iterator belongs to.       *
//* @param bucket The index of the current bucket the iterator is pointing to. *
//* @param it    The iterator within the std::list of the current bucket.     *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
UnorderedMap<Key,T,Hash,KeyEqual>::iterator::iterator(UnorderedMap* map, size_type bucket, typename std::list<value_type>::iterator it)
    : map_(map), bucket_index_(bucket), list_it_(it) {}

//******************************************************************************
//* @brief Advances the iterator to the next element in the UnorderedMap.      *
//* It moves to the next element in the current bucket, or to the first      *
//* element of the next non-empty bucket if the current bucket is exhausted.*
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
void UnorderedMap<Key,T,Hash,KeyEqual>::iterator::advance() {
    if (bucket_index_ >= map_->buckets_.size()) return;
    ++list_it_;
    while (bucket_index_ < map_->buckets_.size() && list_it_ == map_->buckets_[bucket_index_].end()) {
        ++bucket_index_;
        if (bucket_index_ < map_->buckets_.size())
            list_it_ = map_->buckets_[bucket_index_].begin();
    }
}

//******************************************************************************
//* @brief Pre-increment operator for the iterator. Advances the iterator and  *
//* returns a reference to the incremented iterator.                      *
//* *
//* @return A reference to the incremented iterator.                         *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::iterator& UnorderedMap<Key,T,Hash,KeyEqual>::iterator::operator++() {
    advance();
    return *this;
}

//******************************************************************************
//* @brief Post-increment operator for the iterator. Advances the iterator and *
//* returns a copy of the iterator before the increment.                 *
//* *
//* @return A copy of the iterator before the increment.                     *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::iterator UnorderedMap<Key,T,Hash,KeyEqual>::iterator::operator++(int) {
    iterator tmp = *this;
    advance();
    return tmp;
}

//******************************************************************************
//* @brief Dereference operator for the iterator. Returns a reference to the   *
//* key-value pair the iterator is currently pointing to.                *
//* *
//* @return A reference to the current key-value pair.                       *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::iterator::reference UnorderedMap<Key,T,Hash,KeyEqual>::iterator::operator*() const {
    return *list_it_;
}

//******************************************************************************
//* @brief Arrow operator for the iterator. Returns a pointer to the key-value*
//* pair the iterator is currently pointing to.                          *
//* *
//* @return A pointer to the current key-value pair.                         *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::iterator::pointer UnorderedMap<Key,T,Hash,KeyEqual>::iterator::operator->() const {
    return &(*list_it_);
}

//******************************************************************************
//* @brief Equality comparison operator for two iterators. Returns true if both*
//* iterators point to the same element or both are end iterators.        *
//* *
//* @param other The other iterator to compare with.                         *
//* @return True if the iterators are equal, false otherwise.                *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
bool UnorderedMap<Key,T,Hash,KeyEqual>::iterator::operator==(const iterator& other) const {
    return map_ == other.map_ && bucket_index_ == other.bucket_index_ && (bucket_index_ == map_->buckets_.size() || list_it_ == other.list_it_);
}

//******************************************************************************
//* @brief Inequality comparison operator for two iterators. Returns true if   *
//* the iterators are not equal.                                         *
//* *
//* @param other The other iterator to compare with.                         *
//* @return True if the iterators are not equal, false otherwise.            *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
bool UnorderedMap<Key,T,Hash,KeyEqual>::iterator::operator!=(const iterator& other) const {
    return !(*this == other);
}

//******************************************************************************
//* @brief Constructs a const iterator for the UnorderedMap.                   *
//* *
//* @param map   A pointer to the const UnorderedMap this iterator belongs to. *
//* @param bucket The index of the current bucket the iterator is pointing to. *
//* @param it    The const iterator within the std::list of the current bucket.*
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
UnorderedMap<Key,T,Hash,KeyEqual>::const_iterator::
const_iterator(const UnorderedMap* map,
               size_type bucket,
               typename std::list<typename UnorderedMap::value_type>::const_iterator it)
  : map_(map)
  , bucket_index_(bucket)
  , list_it_(it)
{}

//******************************************************************************
//* @brief Advances the const iterator to the next element in the             *
//* UnorderedMap. It moves to the next element in the current bucket, or to  *
//* the first element of the next non-empty bucket if the current bucket is  *
//* exhausted.                                                           *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
void UnorderedMap<Key,T,Hash,KeyEqual>::const_iterator::advance() {
    if (bucket_index_ >= map_->buckets_.size()) return;
    ++list_it_;
    while (bucket_index_ < map_->buckets_.size() &&
           list_it_ == map_->buckets_[bucket_index_].cend()) {
        ++bucket_index_;
        if (bucket_index_ < map_->buckets_.size())
            list_it_ = map_->buckets_[bucket_index_].cbegin();
    }
}

//******************************************************************************
//* @brief Pre-increment operator for the const iterator. Advances the        *
//* iterator and returns a reference to the incremented iterator.          *
//* *
//* @return A reference to the incremented const iterator.                    *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::const_iterator&
UnorderedMap<Key,T,Hash,KeyEqual>::const_iterator::operator++() {
    advance();
    return *this;
}

//******************************************************************************
//* @brief Post-increment operator for the const iterator. Advances the       *
//* iterator and returns a copy of the iterator before the increment.      *
//* *
//* @return A copy of the const iterator before the increment.                *
//******************************************************************************
template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::const_iterator
UnorderedMap<Key,T,Hash,KeyEqual>::const_iterator::operator++(int) {
    const_iterator tmp = *this;
    advance();
    return tmp;
}