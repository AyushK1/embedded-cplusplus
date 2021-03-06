/**
 * @file ChainMap.h
 * @brief Hasher map implementation.
 *
 * Hasher map is implemented using separate chaining
 * because someone wanted remove operations.
 *
 * @author Jeff Niu
 * @date November 3, 2017
 * @bug No known bugs
 */

#ifndef EMBEDDEDTESTS_CHAINMAP_H
#define EMBEDDEDTESTS_CHAINMAP_H

#include "Utility.h"
#include "Equal.h"
#include "Hash.h"
#include "Pair.h"

#include "../memory/Allocator.h"
#include "../memory/Memory.h"

namespace wlp {

    // Forward declaration of ChainHashMap
    template<class Key,
            class Val,
            class Hasher,
            class Equals>
    class ChainHashMap;

    // Forward declaration of ChainHashMap iterator
    template<class Key,
            class Val,
            class Hasher,
            class Equals>
    struct ChainHashMapIterator;

    // Forward declaration of const ChainHashMap iterator
    template<class Key,
            class Val,
            class Hasher,
            class Equals>
    struct ChainHashMapConstIterator;

    /**
     * Hasher map node comprise the elements of a hash map's
     * backing array, containing an element key and corresponding value.
     * Has pointer to next node in a chain.
     * @tparam Key key type
     * @tparam Val value type
     */
    template<class Key, class Val>
    struct ChainHashMapNode {
        typedef ChainHashMapNode<Key, Val> node_type;
        typedef Key key_type;
        typedef Val val_type;
        /**
         * Pointer to the next node in the chain.
         */
        node_type *next = nullptr;
        /**
         * Node element key value.
         */
        key_type m_key;
        /**
         * Node element value.
         */
        val_type m_val;

        /**
         * Equality operator for const.
         * @param node const node to compare
         * @return true if they are equal
         */
        bool operator==(const node_type &node) const {
            return m_key == node.m_key && m_val == node.m_val;
        }

        /**
         * Equality operator. Two nodes are equal if their keys
         * and values are equal.
         * keys and values are equal.
         * @param node the node compare
         * @return true if they are equal
         */
        bool operator==(node_type &node) const {
            return m_key == node.m_key && m_val == node.m_val;
        }
    };

    /**
     * Iterator class over the elements of a ChainHashMap. Specifically,
     * this class iterates through each chain and then the backing array.
     * @tparam Key   key type
     * @tparam Val   value type
     * @tparam Hasher  hash function
     * @tparam Equals key equality function
     */
    template<class Key,
            class Val,
            class Hasher,
            class Equals>
    struct ChainHashMapIterator {
        typedef ChainHashMap<Key, Val, Hasher, Equals> map_type;
        typedef ChainHashMapIterator<Key, Val, Hasher, Equals> iterator;
        typedef ChainHashMapNode<Key, Val> node_type;

        typedef Val val_type;

        typedef wlp::size_type size_type;

        /**
         * Pointer to the node referenced by this iterator.
         */
        node_type *m_current;
        /**
         * Pointer to the iterated ChainHashMap.
         */
        map_type *m_hash_map;

        /**
         * Default constructor.
         */
        ChainHashMapIterator()
                : m_current(nullptr),
                  m_hash_map(nullptr) {
        }

        /**
         * Create an iterator to a ChainHashMap node.
         * @param node hash map node
         * @param map  parent hash map
         */
        ChainHashMapIterator(node_type *node, map_type *map)
                : m_current(node),
                  m_hash_map(map) {
        }

        /**
         * Copy constructor for const.
         * @param it iterator copy
         */
        ChainHashMapIterator(const iterator &it)
                : m_current(it.m_current),
                  m_hash_map(it.m_hash_map) {
        }

        /**
         * Copy constructor.
         * @param it iterator to copy
         */
        ChainHashMapIterator(iterator &&it)
                : m_current(move(it.m_current)),
                  m_hash_map(move(it.m_hash_map)) {
        }

        /**
         * @return reference to the value of the node
         * pointed to by the iterator
         */
        val_type &operator*() const {
            return m_current->m_val;
        }

        /**
         * @return pointer to the value of the node
         * pointed to by the iterator
         */
        val_type *operator->() const {
            return &(operator*());
        }

        /**
         * Increment the iterator to the next available element in
         * the ChainHashMap. If no such element exists, returns pass-the-end
         * iterator. This is pre-fix unary operator.
         * @return this iterator
         */
        iterator &operator++();

        /**
         * Post-fix unary operator.
         * @return this iterator
         */
        iterator operator++(int);

        /**
         * Equality operator for const.
         * @param it iterator to compare
         * @return true if they are equal
         */
        bool operator==(const iterator &it) const {
            return m_current == it.m_current;
        }

        /**
         * Compare two iterators, equal of they point to the
         * same node.
         * @param it iterator to compare
         * @return true if both point to the same node
         */
        bool operator==(iterator &it) const {
            return m_current == it.m_current;
        }

        /**
         * Compare two iterators, unequal if they point to
         * different nodes.
         * @param it iterator to compare
         * @return true if they point to different nodes
         */
        bool operator!=(const iterator &it) const {
            return m_current != it.m_current;
        }

        /**
         * Inequality operator.
         * @param it iterator to compare
         * @return true if they point to different nodes
         */
        bool operator!=(iterator &it) const {
            return m_current != it.m_current;
        }

        /**
         * Assignment operator copies pointer to node
         * and pointer to hash map.
         * @param it iterator to copy
         * @return reference to this iterator
         */
        iterator &operator=(const iterator &it) {
            m_current = it.m_current;
            m_hash_map = it.m_hash_map;
            return *this;
        }

        /**
         * Assignment operator copies pointer to node
         * and pointer to hash map.
         * @param it iterator to copy
         * @return a reference to this iterator
         */
        iterator &operator=(iterator &&it) {
            m_current = move(it.m_current);
            m_hash_map = move(it.m_hash_map);
            return *this;
        }

    };

    /**
     * Constant iterator over a ChainHashMap. Values iterated by
     * this class cannot be modified.
     *
     * @see ChainHashMapIterator
     *
     * @tparam Key   key type
     * @tparam Val   value type
     * @tparam Hasher  hash function
     * @tparam Equals key equality function
     */
    template<class Key,
            class Val,
            class Hasher,
            class Equals>
    struct ChainHashMapConstIterator {
        typedef ChainHashMap<Key, Val, Hasher, Equals> map_type;
        typedef ChainHashMapConstIterator<Key, Val, Hasher, Equals> const_iterator;
        typedef ChainHashMapNode<Key, Val> node_type;

        typedef Val val_type;

        typedef wlp::size_type size_type;

        const node_type *m_current;
        const map_type *m_hash_map;

        ChainHashMapConstIterator()
                : m_current(nullptr),
                  m_hash_map(nullptr) {
        }

        ChainHashMapConstIterator(node_type *node, const map_type *map)
                : m_current(node),
                  m_hash_map(map) {
        }

        ChainHashMapConstIterator(const const_iterator &it)
                : m_current(it.m_current),
                  m_hash_map(it.m_hash_map) {
        }

        ChainHashMapConstIterator(const_iterator &&it)
                : m_current(move(it.m_current)),
                  m_hash_map(move(it.m_hash_map)) {
        }

        const val_type &operator*() const {
            return m_current->m_val;
        }

        const val_type *operator->() const {
            return &(operator*());
        }

        const_iterator &operator++();

        const_iterator operator++(int);

        bool operator==(const const_iterator &it) const {
            return m_current == it.m_current;
        }

        bool operator==(const_iterator &it) const {
            return m_current == it.m_current;
        }

        bool operator!=(const const_iterator &it) const {
            return m_current != it.m_current;
        }

        bool operator!=(const_iterator &it) const {
            return m_current != it.m_current;
        }

        const_iterator &operator=(const const_iterator &it) {
            m_current = it.m_current;
            m_hash_map = it.m_hash_map;
            return *this;
        }

        const_iterator &operator=(const_iterator &&it) {
            m_current = move(it.m_current);
            m_hash_map = move(it.m_hash_map);
            return *this;
        }
    };

    /**
     * Hasher map implemented using separate chaining,
     * in the spirit of std::unordered_map.
     * @tparam Key   key type
     * @tparam Val   value type
     * @tparam Hasher  hash function
     * @tparam Equals key equality function
     */
    template<class Key,
            class Val,
            class Hasher = Hash<Key, uint16_t>,
            class Equals = Equal<Key>>
    class ChainHashMap {
    public:
        typedef ChainHashMap<Key, Val, Hasher, Equals> map_type;
        typedef ChainHashMapIterator<Key, Val, Hasher, Equals> iterator;
        typedef ChainHashMapConstIterator<Key, Val, Hasher, Equals> const_iterator;
        typedef ChainHashMapNode<Key, Val> node_type;

        typedef Key key_type;
        typedef Val val_type;

        typedef wlp::size_type size_type;
        typedef uint8_t percent_type;

        friend struct ChainHashMapIterator<Key, Val, Hasher, Equals>;
        friend struct ChainHashMapConstIterator<Key, Val, Hasher, Equals>;

    private:
        /**
         * Class hash function instance. Used to hash
         * element keys.
         */
        Hasher m_hash;
        /**
         * Class key equality function. Used to test
         * equality of element keys.
         */
        Equals m_equal;

        /**
         * Allocator to create memory for hash map nodes.
         */
        Allocator m_node_allocator;

        /**
         * Hasher map backing array.
         */
        node_type **m_buckets;

        /**
         * The number of elements currently
         * in the map.
         */
        size_type m_num_elements;
        /**
         * The size of the backing array. Note that the
         * number of elements in the chained hash map
         * may very well exceed the size fo the backing
         * array if the load factor is greater than 100.
         */
        size_type m_capacity;
        /**
         * The maximum load factor as an integer percent
         * before the map performs a rehash. This number
         * may exceed 100.
         */
        percent_type m_max_load;

    public:
        /**
         * Create and initialize an empty hash map. The hash map uses
         * {@code Allocator} to handle memory.
         *
         * @pre the hash map requires definition of an initial bucket array size
         *      and a maximum load factor before rehashing
         *
         * @param n        initial size of the bucket list; each bucket is initialized to nullptr
         * @param max_load an integer value denoting the max percent load factor, e.g. 100 = 1.00
         * @param hash     hash function for the key type, default is {@code wlp::Hasher}
         * @param equal    equality function for the key type, default is {@code wlp::Equals}
         */
        explicit ChainHashMap(
                size_type n = 12,
                percent_type max_load = 75)
                : m_hash(Hasher()),
                  m_equal(Equals()),
                  m_node_allocator{sizeof(node_type), static_cast<size_type>(n * sizeof(node_type))},
                  m_num_elements(0),
                  m_capacity(n),
                  m_max_load(max_load) {
            init_buckets(n);
        }

        /**
         * Disable copy constructor.
         */
        ChainHashMap(const map_type &) = delete;

        /**
         * Move constructor.
         *
         * @param map hash map to copy
         */
        ChainHashMap(map_type &&map) :
                m_hash(move(map.m_hash)),
                m_equal(move(map.m_equal)),
                m_node_allocator(move(map.m_node_allocator)),
                m_buckets(move(map.m_buckets)),
                m_num_elements(move(map.m_num_elements)),
                m_capacity(move(map.m_capacity)),
                m_max_load(move(map.m_max_load)) {
            map.m_num_elements = 0;
            map.m_capacity = 0;
            map.m_buckets = nullptr;
        }

        /**
         * Destroy the hash map, freeing allocated nodes and
         * memory allocated for the array.
         */
        ~ChainHashMap();

    private:
        /**
         * Function called when creating the hash map. This function
         * will allocate memory for the backing array and initialize each
         * element to nullptr.
         *
         * @param n the size of the backing array
         */
        void init_buckets(size_type n);

        /**
         * Obtain the bucket index in an array with the specified
         * number of maximum elements.
         *
         * @param key          the key to hash
         * @param max_elements the maximum number of buckets
         * @return an index i such that 0 <= i < max_elements
         */
        size_type bucket_index(key_type key, size_type max_elements) const {
            return m_hash(key) % max_elements;
        }

        /**
         * Obtain the bucket index of a key in the backing array.
         *
         * @param key the key to hash
         * @return an index i such that 0 <= i < m_max_elements
         */
        size_type hash(key_type key) const {
            return m_hash(key) % m_capacity;
        }

        /**
         * Resize and rehash the hash map if the current load factor
         * exceeds or equals the maximum load factor. This function
         * will double the size of the backing array, allocating a new
         * array, and fully deallocating the previous array.
         *
         * @pre this function will create a new array allocator, however
         *      the same node allocator will be used, which means that
         *      the node allocator will start drawing from heap memory
         *      if the number of elements exceeds the initial max elements
         */
        void ensure_capacity();

    public:
        /**
         * @return the current number of elements that have been
         * inserted into the map
         */
        size_type size() const {
            return m_num_elements;
        }

        /**
         * @return the current size of the backing array
         */
        size_type capacity() const {
            return m_capacity;
        }

        /**
         * @return the maximum load before before rehash
         */
        percent_type max_load() const {
            return m_max_load;
        }

        /**
         * @return true if the map is empty
         */
        bool empty() const {
            return m_num_elements == 0;
        }

        /**
         * @return the node allocator of the map
         */
        const Allocator *get_node_allocator() const {
            return &m_node_allocator;
        }

        /**
         * Obtain an iterator to the first element in the hash map.
         * Returns pass-the-end iterator if there are no elements
         * in the hash map.
         *
         * @return iterator the first element
         */
        iterator begin() {
            if (m_num_elements == 0) {
                return end();
            }
            for (size_type i = 0; i < m_capacity; ++i) {
                if (m_buckets[i]) {
                    return iterator(m_buckets[i], this);
                }
            }
            return end();
        }

        /**
         * @return a pass-the-end iterator for this map
         */
        iterator end() {
            return iterator(nullptr, this);
        }

        /**
         * @see ChainHashMap<Key, Value, Hasher, Equals>::begin()
         * @return a constant iterator to the first element
         */
        const_iterator begin() const {
            if (m_num_elements == 0) {
                return end();
            }
            for (size_type i = 0; i < m_capacity; ++i) {
                if (m_buckets[i]) {
                    return const_iterator(m_buckets[i], this);
                }
            }
            return end();
        }

        /**
         * @see ChainHashMap<Key, Value, Hasher, Equals>::end()
         * @return a constant pass-the-end iterator
         */
        const_iterator end() const {
            return const_iterator(nullptr, this);
        }

        /**
         * Erase all elements in the map, deallocating them
         * and resetting the element count to zero.
         */
        void clear() noexcept;

        /**
         * Attempt to insert an element into the map.
         * Insertion is prevented if there already exists
         * an element with the provided key
         *
         * @param key inserted element key
         * @param val inserted element value
         * @return a pair consisting of an iterator pointing to the
         * inserted element or the element that prevented insertion
         * and a bool indicating whether insertion occurred
         */
        Pair<iterator, bool> insert(key_type key, val_type val);

        /**
         * Attempt to insert an element into the map.
         * If an element with the same key already exists,
         * override the value mapped to by the provided key.
         *
         * @param key inserted element key
         * @param val inserted element value
         * @return a pair consisting of an iterator pointing to the
         * inserted element or the assigned element, and a bool
         * indicating whether insertion occurred
         */
        Pair<iterator, bool> insert_or_assign(key_type key, val_type val);

        /**
         * Erase an element pointed to by the provided pointer.
         *
         * @param pos element to erase
         * @return the iterator to the next element in the map
         * or pass-the-end if there are no more elements afterwards
         */
        iterator &erase(iterator &pos);

        /**
         * @see ChainHashMap<Key, Value, Hasher, Equals>::erase()
         * @param pos const iterator to the element to erase
         * @return const iterator to the next element or pass-the-end
         */
        const_iterator &erase(const_iterator &pos);

        /**
         * Erase the element with the corresponding key.
         *
         * @param key the key of the element to erase
         * @return true if erasure occured
         */
        bool erase(key_type &key);

        /**
         * Returns the value corresponding to a provided key.
         *
         * @param key the key for which to find the value
         * @return the value mapped to by the key
         * @throws KeyException if the key does not map to a value
         */
        iterator at(const key_type &key);

        /**
         * @see ChainHashMap<Key, Value, Hasher, Equals>::at()
         * @param key key for which to find the value
         * @return the mapped value
         * @throws KeyException if the key does not exist
         */
        const_iterator at(const key_type &key) const;

        /**
         * @param key key for which to check existence of a value
         * @return true if the key maps to a value
         */
        bool contains(const key_type &key) const;

        /**
         * Return an iterator to the map element corresponding
         * to the provided key, or pass-the-end if the key does
         * not map to any value in the map.
         *
         * @param key the key to map
         * @return an iterator to the element mapped by the key
         */
        iterator find(const key_type &key);

        /**
         * @see ChainHashMap<Key, Value, Hasher, Equals>::find()
         * @param key the key to map
         * @return a const iterator to the element mapped by the key
         */
        const_iterator find(const key_type &key) const;

        /**
         * Access an element in the hash map by the given key.
         * If the key does not map to any value in the map,
         * then a new value is created and inserted using the default
         * constructor.
         *
         * @param key the key whose value to access
         * @return a reference to the mapped value
         */
        val_type &operator[](const key_type &key);

        /**
         * Disable copy assignment.
         *
         * @return reference to this map
         */
        map_type &operator=(const map_type &) = delete;

        /**
         * Assignment operator performs a deep copy of the
         * contents of the assigned map. Therefore, one
         * should use pass by reference or pointer unless
         * assignment is absolutely necessary.
         *
         * @param map the map to copy
         * @return a reference to this map
         */
        map_type &operator=(map_type &&map);
    };

    template<class Key, class Value, class Hasher, class Equals>
    void ChainHashMap<Key, Value, Hasher, Equals>::init_buckets(ChainHashMap<Key, Value, Hasher, Equals>::size_type n) {
        m_buckets = static_cast<node_type **>(memory_alloc(n * sizeof(node_type *)));
        for (size_type i = 0; i < n; ++i) {
            m_buckets[i] = nullptr;
        }
    }

    template<class Key, class Value, class Hasher, class Equals>
    void ChainHashMap<Key, Value, Hasher, Equals>::ensure_capacity() {
        if (m_num_elements * 100 < m_max_load * m_capacity) {
            return;
        }
        size_type new_capacity = static_cast<size_type>(m_capacity * 2);
        node_type **new_buckets = static_cast<node_type **>(memory_alloc(new_capacity * sizeof(node_type *)));
        for (size_type i = 0; i < new_capacity; ++i) {
            new_buckets[i] = nullptr;
        }
        for (size_type i = 0; i < m_capacity; ++i) {
            if (!m_buckets[i]) {
                continue;
            }
            node_type *cur = m_buckets[i];
            while (cur) {
                size_type k = bucket_index(cur->m_key, new_capacity);
                node_type *first = new_buckets[k];
                node_type *next = cur->next;
                cur->next = first;
                new_buckets[k] = cur;
                cur = next;
            }
        }
        memory_free(m_buckets);
        m_buckets = new_buckets;
        m_capacity = new_capacity;
    }

    template<class Key, class Value, class Hasher, class Equals>
    void ChainHashMap<Key, Value, Hasher, Equals>::clear() noexcept {
        for (size_type i = 0; i < m_capacity; ++i) {
            node_type *cur = m_buckets[i];
            node_type *next;
            while (cur) {
                next = cur->next;
                m_node_allocator.Deallocate(cur);
                cur = next;
            }
            m_buckets[i] = nullptr;
        }
        m_num_elements = 0;
    }

    template<class Key, class Value, class Hasher, class Equals>
    Pair<typename ChainHashMap<Key, Value, Hasher, Equals>::iterator, bool>
    ChainHashMap<Key, Value, Hasher, Equals>::insert(key_type key, val_type val) {
        ensure_capacity();
        size_type i = hash(key);
        node_type *first = m_buckets[i];
        for (node_type *cur = first; cur; cur = cur->next) {
            if (m_equal(cur->m_key, key)) {
                return Pair<iterator, bool>(iterator(cur, this), false);
            }
        }
        node_type *tmp = static_cast<node_type *>(m_node_allocator.Allocate());
        tmp->m_key = key;
        tmp->m_val = val;
        tmp->next = first;
        m_buckets[i] = tmp;
        ++m_num_elements;
        return Pair<iterator, bool>(iterator(tmp, this), true);
    };

    template<class Key, class Value, class Hasher, class Equals>
    Pair<typename ChainHashMap<Key, Value, Hasher, Equals>::iterator, bool>
    ChainHashMap<Key, Value, Hasher, Equals>::insert_or_assign(key_type key, val_type val) {
        ensure_capacity();
        size_type i = hash(key);
        node_type *first = m_buckets[i];
        for (node_type *cur = first; cur; cur = cur->next) {
            if (m_equal(cur->m_key, key)) {
                cur->m_val = val;
                return Pair<iterator, bool>(iterator(cur, this), false);
            }
        }
        node_type *tmp = static_cast<node_type *>(m_node_allocator.Allocate());
        tmp->m_key = key;
        tmp->m_val = val;
        tmp->next = first;
        m_buckets[i] = tmp;
        ++m_num_elements;
        return Pair<iterator, bool>(iterator(tmp, this), true);
    };

    template<class Key, class Value, class Hasher, class Equals>
    typename ChainHashMap<Key, Value, Hasher, Equals>::iterator &
    ChainHashMap<Key, Value, Hasher, Equals>::erase(iterator &pos) {
        node_type *p_node = pos.m_current;
        if (p_node) {
            node_type *n_node = p_node->next;
            if (n_node) {
                p_node->next = n_node->next;
                p_node->m_key = n_node->m_key;
                p_node->m_val = n_node->m_val;
                m_node_allocator.Deallocate(n_node);
                --m_num_elements;
                return pos;
            } else {
                size_type i = hash(p_node->m_key);
                node_type *c_node = m_buckets[i];
                if (c_node == p_node) {
                    m_node_allocator.Deallocate(c_node);
                    --m_num_elements;
                    m_buckets[i] = nullptr;
                    while (++i < m_capacity && !m_buckets[i]);
                    if (i == m_capacity) {
                        pos.m_current = nullptr;
                        return pos;
                    } else {
                        pos.m_current = m_buckets[i];
                        return pos;
                    }
                } else {
                    while (c_node->next != p_node) {
                        c_node = c_node->next;
                    }
                    m_node_allocator.Deallocate(c_node->next);
                    --m_num_elements;
                    c_node->next = nullptr;
                    while (++i < m_capacity && !m_buckets[i]);
                    if (i == m_capacity) {
                        pos.m_current = nullptr;
                        return pos;
                    } else {
                        pos.m_current = m_buckets[i];
                        return pos;
                    }
                }
            }
        }
        pos.m_current = nullptr;
        return pos;
    }

    template<class Key, class Value, class Hasher, class Equals>
    bool ChainHashMap<Key, Value, Hasher, Equals>::erase(key_type &key) {
        size_type i = hash(key);
        node_type *first = m_buckets[i];
        if (first) {
            node_type *cur = first;
            node_type *next = cur->next;
            if (next) {
                if (m_equal(key, cur->m_key)) {
                    cur->m_key = next->m_key;
                    cur->m_val = next->m_val;
                    cur->next = next->next;
                    m_node_allocator.Deallocate(next);
                    --m_num_elements;
                    return true;
                }
                while (next && !m_equal(key, next->m_key)) {
                    cur = next;
                    next = cur->next;
                }
                if (next) {
                    cur->next = next->next;
                    m_node_allocator.Deallocate(next);
                    --m_num_elements;
                    return true;
                }
                return false;
            } else {
                if (!m_equal(key, cur->m_key)) {
                    return false;
                }
                m_node_allocator.Deallocate(cur);
                --m_num_elements;
                m_buckets[i] = nullptr;
                return true;
            }
        }
        return false;
    }

    template<class Key, class Value, class Hasher, class Equals>
    typename ChainHashMap<Key, Value, Hasher, Equals>::iterator
    ChainHashMap<Key, Value, Hasher, Equals>::at(const key_type &key) {
        size_type i = hash(key);
        node_type *cur = m_buckets[i];
        if (!cur) {
            return end();
        }
        while (cur && !m_equal(key, cur->m_key)) {
            cur = cur->next;
        }
        if (!cur) {
            return end();
        }
        return iterator(cur, this);
    }

    template<class Key, class Value, class Hasher, class Equals>
    typename ChainHashMap<Key, Value, Hasher, Equals>::const_iterator
    ChainHashMap<Key, Value, Hasher, Equals>::at(const key_type &key) const {
        size_type i = hash(key);
        node_type *cur = m_buckets[i];
        if (!cur) {
            return end();
        }
        while (cur && !m_equal(key, cur->m_key)) {
            cur = cur->next;
        }
        if (!cur) {
            return end();
        }
        return const_iterator(cur, this);
    }

    template<class Key, class Value, class Hasher, class Equals>
    bool ChainHashMap<Key, Value, Hasher, Equals>::contains(const key_type &key) const {
        size_type i = hash(key);
        node_type *cur = m_buckets[i];
        if (!cur) {
            return false;
        }
        while (cur && !m_equal(key, cur->m_key)) {
            cur = cur->next;
        }
        return cur != nullptr;
    }

    template<class Key, class Value, class Hasher, class Equals>
    typename ChainHashMap<Key, Value, Hasher, Equals>::val_type &
    ChainHashMap<Key, Value, Hasher, Equals>::operator[](const key_type &key) {
        ensure_capacity();
        size_type i = hash(key);
        node_type *first = m_buckets[i];
        node_type *cur = first;
        if (!cur) {
            node_type *ele = static_cast<node_type *>(m_node_allocator.Allocate());
            ++m_num_elements;
            ele->m_key = key;
            ele->m_val = val_type();
            ele->next = nullptr;
            m_buckets[i] = ele;
            return ele->m_val;
        }
        while (cur && !m_equal(key, cur->m_key)) {
            cur = cur->next;
        }
        if (cur) {
            return cur->m_val;
        }
        cur = static_cast<node_type *>(m_node_allocator.Allocate());
        ++m_num_elements;
        cur->m_key = key;
        cur->m_val = val_type();
        cur->next = nullptr;
        cur->next = first;
        m_buckets[i] = cur;
        return cur->m_val;
    }

    template<class Key, class Value, class Hasher, class Equals>
    typename ChainHashMap<Key, Value, Hasher, Equals>::iterator
    ChainHashMap<Key, Value, Hasher, Equals>::find(const key_type &key) {
        size_type i = hash(key);
        node_type *cur = m_buckets[i];
        if (!cur) {
            return end();
        }
        while (cur && !m_equal(key, cur->m_key)) {
            cur = cur->next;
        }
        if (!cur) {
            return end();
        }
        return iterator(cur, this);
    }

    template<class Key, class Value, class Hasher, class Equals>
    typename ChainHashMap<Key, Value, Hasher, Equals>::const_iterator
    ChainHashMap<Key, Value, Hasher, Equals>::find(const key_type &key) const {
        size_type i = hash(key);
        node_type *cur = m_buckets[i];
        if (!cur) {
            return end();
        }
        while (cur && !m_equal(key, cur->m_key)) {
            cur = cur->next;
        }
        if (!cur) {
            return end();
        }
        return const_iterator(cur, this);
    }

    template<class Key, class Value, class Hasher, class Equals>
    ChainHashMap<Key, Value, Hasher, Equals>::~ChainHashMap() {
        if (!m_buckets) {
            return;
        }
        for (size_type i = 0; i < m_capacity; ++i) {
            node_type *cur = m_buckets[i];
            if (cur) {
                while (cur) {
                    node_type *next = cur->next;
                    m_node_allocator.Deallocate(cur);
                    cur = next;
                }
            }
        }
        memory_free(m_buckets);
        m_buckets = nullptr;
    }

    template<class Key, class Val, class Hasher, class Equals>
    ChainHashMap<Key, Val, Hasher, Equals> &
    ChainHashMap<Key, Val, Hasher, Equals>::operator=(ChainHashMap<Key, Val, Hasher, Equals> &&map) {
        clear();
        memory_free(m_buckets);
        m_node_allocator = move(map.m_node_allocator);
        m_num_elements = move(map.m_num_elements);
        m_capacity = move(map.m_capacity);
        m_max_load = move(map.m_max_load);
        m_buckets = move(map.m_buckets);
        map.m_num_elements = 0;
        map.m_capacity = 0;
        map.m_buckets = nullptr;
        return *this;
    }

    template<class Key, class Value, class Hasher, class Equals>
    ChainHashMapIterator<Key, Value, Hasher, Equals> &
    ChainHashMapIterator<Key, Value, Hasher, Equals>::operator++() {
        const node_type *old = m_current;
        m_current = m_current->next;
        if (!m_current) {
            size_type i = m_hash_map->hash(old->m_key);
            while (++i < m_hash_map->m_capacity && !m_hash_map->m_buckets[i]);
            if (i == m_hash_map->m_capacity) {
                m_current = nullptr;
            } else {
                m_current = m_hash_map->m_buckets[i];
            }
        }
        return *this;
    }

    template<class Key, class Value, class Hasher, class Equals>
    inline ChainHashMapIterator<Key, Value, Hasher, Equals>
    ChainHashMapIterator<Key, Value, Hasher, Equals>::operator++(int) {
        iterator tmp = *this;
        ++*this;
        return tmp;
    }

    template<class Key, class Value, class Hasher, class Equals>
    ChainHashMapConstIterator<Key, Value, Hasher, Equals> &
    ChainHashMapConstIterator<Key, Value, Hasher, Equals>::operator++() {
        const node_type *old = m_current;
        m_current = m_current->next;
        if (!m_current) {
            size_type i = m_hash_map->hash(old->m_key);
            while (++i < m_hash_map->m_capacity && !m_hash_map->m_buckets[i]);
            if (i == m_hash_map->m_capacity) {
                m_current = nullptr;
            } else {
                m_current = m_hash_map->m_buckets[i];
            }
        }
        return *this;
    }

    template<class Key, class Value, class Hasher, class Equals>
    inline ChainHashMapConstIterator<Key, Value, Hasher, Equals>
    ChainHashMapConstIterator<Key, Value, Hasher, Equals>::operator++(int) {
        const_iterator tmp = *this;
        ++*this;
        return tmp;
    }

}

#endif //EMBEDDEDTESTS_CHAINMAP_H
