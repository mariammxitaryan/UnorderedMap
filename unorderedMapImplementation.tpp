#include "unorderedMapHeader.hpp"

// Implementation of UnorderedMap in .tpp

template<typename Key, typename T, typename Hash, typename KeyEqual>
UnorderedMap<Key,T,Hash,KeyEqual>::UnorderedMap(size_type bucket_count, const Hash& hash, const KeyEqual& equal)
    : buckets_(bucket_count), num_elements_(0), max_load_factor_(1.0f), hasher_(hash), equal_(equal) {}

template<typename Key, typename T, typename Hash, typename KeyEqual>
UnorderedMap<Key,T,Hash,KeyEqual>::UnorderedMap(std::initializer_list<value_type> init,
                                                  size_type bucket_count,
                                                  const Hash& hash,
                                                  const KeyEqual& equal)
    : UnorderedMap(bucket_count, hash, equal) {
    for (auto& kv : init) insert(kv);
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
UnorderedMap<Key,T,Hash,KeyEqual>::UnorderedMap(const UnorderedMap& other)
    : buckets_(other.buckets_.size()), num_elements_(0), max_load_factor_(other.max_load_factor_), hasher_(other.hasher_), equal_(other.equal_) {
    for (auto& bucket : other.buckets_) {
        for (auto& kv : bucket) {
            insert(kv);
        }
    }
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
UnorderedMap<Key,T,Hash,KeyEqual>::UnorderedMap(UnorderedMap&& other) noexcept
    : buckets_(std::move(other.buckets_)), num_elements_(other.num_elements_), max_load_factor_(other.max_load_factor_),
      hasher_(std::move(other.hasher_)), equal_(std::move(other.equal_)) {
    other.num_elements_ = 0;
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
UnorderedMap<Key,T,Hash,KeyEqual>::~UnorderedMap() {
    clear();
}

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

template<typename Key, typename T, typename Hash, typename KeyEqual>
UnorderedMap<Key,T,Hash,KeyEqual>& UnorderedMap<Key,T,Hash,KeyEqual>::operator=(std::initializer_list<value_type> init) {
    clear();
    for (auto& kv : init) insert(kv);
    return *this;
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::iterator UnorderedMap<Key,T,Hash,KeyEqual>::begin() {
    for (size_type i = 0; i < buckets_.size(); ++i) {
        if (!buckets_[i].empty())
            return iterator(this, i, buckets_[i].begin());
    }
    return end();
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::iterator UnorderedMap<Key,T,Hash,KeyEqual>::end() {
    return iterator(this, buckets_.size(), {});
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::const_iterator UnorderedMap<Key,T,Hash,KeyEqual>::begin() const {
    for (size_type i = 0; i < buckets_.size(); ++i) {
        if (!buckets_[i].empty())
            return const_iterator(this, i, buckets_[i].cbegin());
    }
    return end();
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::const_iterator UnorderedMap<Key,T,Hash,KeyEqual>::end() const {
    return const_iterator(this, buckets_.size(), {});
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
bool UnorderedMap<Key,T,Hash,KeyEqual>::empty() const {
    return num_elements_ == 0;
}

// template<typename Key, typename T, typename Hash, typename KeyEqual>
// typedef size_t size_t_alias;

template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::size_type UnorderedMap<Key,T,Hash,KeyEqual>::size() const {
    return num_elements_;
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::size_type UnorderedMap<Key,T,Hash,KeyEqual>::max_size() const {
    return std::numeric_limits<size_type>::max();
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
void UnorderedMap<Key,T,Hash,KeyEqual>::clear() {
    for (auto& bucket : buckets_) bucket.clear();
    num_elements_ = 0;
}

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

template<typename Key, typename T, typename Hash, typename KeyEqual>
template<class... Args>
std::pair<typename UnorderedMap<Key,T,Hash,KeyEqual>::iterator, bool>
UnorderedMap<Key,T,Hash,KeyEqual>::emplace(Args&&... args) {
    value_type kv(std::forward<Args>(args)...);
    return insert(kv);
}

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

template<typename Key, typename T, typename Hash, typename KeyEqual>
void UnorderedMap<Key,T,Hash,KeyEqual>::swap(UnorderedMap& other) noexcept {
    using std::swap;
    swap(buckets_, other.buckets_);
    swap(num_elements_, other.num_elements_);
    swap(max_load_factor_, other.max_load_factor_);
    swap(hasher_, other.hasher_);
    swap(equal_, other.equal_);
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
T& UnorderedMap<Key,T,Hash,KeyEqual>::at(const Key& key) {
    auto it = find(key);
    if (it == end()) throw std::out_of_range("Key not found");
    return it->second;
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
const T& UnorderedMap<Key,T,Hash,KeyEqual>::at(const Key& key) const {
    auto it = find(key);
    if (it == end()) throw std::out_of_range("Key not found");
    return it->second;
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
T& UnorderedMap<Key,T,Hash,KeyEqual>::operator[](const Key& key) {
    auto res = insert({key, T{}});
    return res.first->second;
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::size_type
UnorderedMap<Key,T,Hash,KeyEqual>::count(const Key& key) const {
    return contains(key) ? 1 : 0;
}

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

template<typename Key, typename T, typename Hash, typename KeyEqual>
bool UnorderedMap<Key,T,Hash,KeyEqual>::contains(const Key& key) const {
    return find(key) != end();
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::size_type
UnorderedMap<Key,T,Hash,KeyEqual>::bucket_count() const {
    return buckets_.size();
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
float UnorderedMap<Key,T,Hash,KeyEqual>::load_factor() const {
    return static_cast<float>(num_elements_) / buckets_.size();
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
float UnorderedMap<Key,T,Hash,KeyEqual>::max_load_factor() const {
    return max_load_factor_;
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
void UnorderedMap<Key,T,Hash,KeyEqual>::max_load_factor(float ml) {
    max_load_factor_ = ml;
    rehash_if_needed();
}

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

template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::size_type
UnorderedMap<Key,T,Hash,KeyEqual>::bucket_size(size_type i) const {
    return buckets_[i].size();
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::size_type
UnorderedMap<Key,T,Hash,KeyEqual>::bucket(const Key& key) const {
    return hasher_(key) % buckets_.size();
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::hasher UnorderedMap<Key,T,Hash,KeyEqual>::hash_function() const {
    return hasher_;
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::key_equal UnorderedMap<Key,T,Hash,KeyEqual>::key_eq() const {
    return equal_;
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
void UnorderedMap<Key,T,Hash,KeyEqual>::rehash_if_needed() {
    if (load_factor() > max_load_factor_) {
        rehash(buckets_.size() * 2);
    }
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
UnorderedMap<Key,T,Hash,KeyEqual>::iterator::iterator(UnorderedMap* map, size_type bucket, typename std::list<value_type>::iterator it)
    : map_(map), bucket_index_(bucket), list_it_(it) {}

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

template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::iterator& UnorderedMap<Key,T,Hash,KeyEqual>::iterator::operator++() {
    advance();
    return *this;
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::iterator UnorderedMap<Key,T,Hash,KeyEqual>::iterator::operator++(int) {
    iterator tmp = *this;
    advance();
    return tmp;
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::iterator::reference UnorderedMap<Key,T,Hash,KeyEqual>::iterator::operator*() const {
    return *list_it_;
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::iterator::pointer UnorderedMap<Key,T,Hash,KeyEqual>::iterator::operator->() const {
    return &(*list_it_);
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
bool UnorderedMap<Key,T,Hash,KeyEqual>::iterator::operator==(const iterator& other) const {
    return map_ == other.map_ && bucket_index_ == other.bucket_index_ && (bucket_index_ == map_->buckets_.size() || list_it_ == other.list_it_);
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
bool UnorderedMap<Key,T,Hash,KeyEqual>::iterator::operator!=(const iterator& other) const {
    return !(*this == other);
}

// const_iterator

template<typename Key, typename T, typename Hash, typename KeyEqual>
UnorderedMap<Key,T,Hash,KeyEqual>::const_iterator::
const_iterator(const UnorderedMap* map,
               size_type bucket,
               typename std::list<typename UnorderedMap::value_type>::const_iterator it)
  : map_(map)
  , bucket_index_(bucket)
  , list_it_(it)
{}

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

template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::const_iterator&
UnorderedMap<Key,T,Hash,KeyEqual>::const_iterator::operator++() {
    advance();
    return *this;
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
typename UnorderedMap<Key,T,Hash,KeyEqual>::const_iterator
UnorderedMap<Key,T,Hash,KeyEqual>::const_iterator::operator++(int) {
    const_iterator tmp = *this;
    advance();
    return tmp;
}
