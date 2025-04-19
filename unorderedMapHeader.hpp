#ifndef UNORDERED_MAP_HPP
#define UNORDERED_MAP_HPP

#include <vector>
#include <list>
#include <string>
#include <functional>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <iostream>

template<
    typename Key,
    typename T,
    typename Hash = std::hash<Key>,
    typename KeyEqual = std::equal_to<Key>
>
class UnorderedMap {
public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = std::pair<const Key, T>;
    using size_type = size_t;
    using hasher = Hash;
    using key_equal = KeyEqual;

    class iterator;
    class const_iterator;

    UnorderedMap(size_type bucket_count = DEFAULT_BUCKET_COUNT,
                 const Hash& hash = Hash(),
                 const KeyEqual& equal = KeyEqual());
    UnorderedMap(std::initializer_list<value_type> init,
                 size_type bucket_count = DEFAULT_BUCKET_COUNT,
                 const Hash& hash = Hash(),
                 const KeyEqual& equal = KeyEqual());
    UnorderedMap(const UnorderedMap&);
    UnorderedMap(UnorderedMap&&) noexcept;
    ~UnorderedMap();

    UnorderedMap& operator=(const UnorderedMap&);
    UnorderedMap& operator=(UnorderedMap&&) noexcept;
    UnorderedMap& operator=(std::initializer_list<value_type>);

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    bool empty() const;
    size_type size() const;
    size_type max_size() const;

    void clear();
    std::pair<iterator,bool> insert(const value_type& kv);
    template<class... Args>
    std::pair<iterator,bool> emplace(Args&&... args);
    size_type erase(const Key&);
    void swap(UnorderedMap&) noexcept;

    mapped_type& at(const Key&);
    const mapped_type& at(const Key&) const;
    mapped_type& operator[](const Key&);
    size_type count(const Key&) const;
    iterator find(const Key&);
    const_iterator find(const Key&) const;
    bool contains(const Key&) const;

    size_type bucket_count() const;
    float load_factor() const;
    float max_load_factor() const;
    void max_load_factor(float);
    void rehash(size_type new_count);
    size_type bucket_size(size_type) const;
    size_type bucket(const Key&) const;

    hasher hash_function() const;
    key_equal key_eq() const;

private:
    static constexpr size_type DEFAULT_BUCKET_COUNT = 16;
    std::vector<std::list<value_type>> buckets_;
    size_type num_elements_;
    float max_load_factor_;
    Hash hasher_;
    KeyEqual equal_;

    void rehash_if_needed();
};

template<typename Key, typename T, typename Hash, typename KeyEqual>
class UnorderedMap<Key,T,Hash,KeyEqual>::iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::pair<const Key, T>;
    using reference = value_type&;
    using pointer = value_type*;

    iterator(UnorderedMap* map, size_type bucket, typename std::list<value_type>::iterator it);
    iterator& operator++();
    iterator operator++(int);
    reference operator*() const;
    pointer operator->() const;
    bool operator==(const iterator& other) const;
    bool operator!=(const iterator& other) const;

private:
    UnorderedMap* map_;
    size_type bucket_index_;
    typename std::list<value_type>::iterator list_it_;

    void advance();
};

template<typename Key, typename T, typename Hash, typename KeyEqual>
class UnorderedMap<Key,T,Hash,KeyEqual>::const_iterator {
public:
    using map_value_type = UnorderedMap::value_type;      
    using reference      = const map_value_type&;
    using pointer        = const map_value_type*;
    using iterator_category = std::forward_iterator_tag;

    const_iterator(const UnorderedMap* map,
                   size_type bucket,
                   typename std::list<map_value_type>::const_iterator it);

    const_iterator& operator++();
    const_iterator  operator++(int);
    reference       operator*()  const { return *list_it_; }
    pointer         operator->() const { return &*list_it_; }
    bool            operator==(const const_iterator& o) const
                      { return map_==o.map_ && bucket_index_==o.bucket_index_ && (bucket_index_==map_->buckets_.size() || list_it_==o.list_it_); }
    bool            operator!=(const const_iterator& o) const { return !(*this==o); }

private:
    const UnorderedMap*                      map_;
    size_type                                bucket_index_;
    typename std::list<map_value_type>::const_iterator list_it_;

    void advance();
};

#include "unorderedMapImplementation.tpp"

#endif