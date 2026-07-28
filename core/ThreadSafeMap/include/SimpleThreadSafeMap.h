#pragma once
#include <list>
#include <memory>
#include <shared_mutex>
#include <vector>
#include <algorithm>

namespace ThreadSafeStructs
{
template <typename Key, typename Value, typename Hash = std::hash<Key>>
class SimpleThreadSafeMap
{
    class bucket_type
    {
    public:
        using bucket_elem = std::pair<Key, Value>;
        using bucket_elem_iter = std::list<bucket_elem>::iterator;
        using size_type = std::list<bucket_elem>::size_type;


        bucket_type() = default;

        Value           get_elem_for(const Key& key, const Value& default_value);
        void            set_or_update_elem_for(const Key& key, const Value& value);
        size_type       remove_elem_for(const Key& key);

    private:
        bucket_elem_iter    find_elem_for(const Key& key);

        std::list<bucket_elem>      bucket;
        mutable std::shared_mutex   mtx;
    };

public:
    SimpleThreadSafeMap(size_t bucket_size = 19, const Hash& hasher = Hash());

    SimpleThreadSafeMap(const SimpleThreadSafeMap&) = delete;
    SimpleThreadSafeMap(SimpleThreadSafeMap&&) = delete;
    SimpleThreadSafeMap& operator=(const SimpleThreadSafeMap&) = delete;
    SimpleThreadSafeMap& operator=(SimpleThreadSafeMap&&) = delete;


    Value   value_for(const Key& key, const Value& default_value = Value()) const;
    void    set_or_update_value_for(const Key& key, const Value& value);
    bucket_type::size_type  remove_value_for(const Key& key);

private:
    bucket_type&   get_bucket(const Key& key) const;

    std::vector<std::unique_ptr<bucket_type>> buckets;
    Hash hasher;
};
template <typename Key, typename Value, typename Hash>
SimpleThreadSafeMap<Key, Value, Hash>::SimpleThreadSafeMap(size_t bucket_size, const Hash& hasher)
    : buckets(bucket_size), hasher(hasher)
{
    for (size_t i = 0; i < bucket_size; ++i)
        buckets.emplace_back(std::make_unique<bucket_type>());
}


template <typename Key, typename Value, typename Hash>
SimpleThreadSafeMap<Key, Value, Hash>::bucket_type& SimpleThreadSafeMap<Key, Value, Hash>::get_bucket(const Key& key) const
{
    const size_t bucket_index = hasher(key) % buckets.size();
    return *buckets[bucket_index];
}

template <typename Key, typename Value, typename Hash>
Value SimpleThreadSafeMap<Key, Value, Hash>::bucket_type::get_elem_for(const Key& key, const Value& default_value)
{
    std::shared_lock lock(mtx);
    auto&& iter = find_elem_for(key);
    if (iter == bucket.end())
        return default_value;

    return iter->second;
}

template <typename Key, typename Value, typename Hash>
void SimpleThreadSafeMap<Key, Value, Hash>::bucket_type::set_or_update_elem_for(const Key& key, const Value& value)
{
    std::unique_lock lock(mtx);
    auto&& iter = find_elem_for(key);
    if (iter != bucket.end())
        iter->second = value;
    else
        bucket.emplace_back(key, value);
    
}

template <typename Key, typename Value, typename Hash>
SimpleThreadSafeMap<Key, Value, Hash>::bucket_type::size_type SimpleThreadSafeMap<Key, Value, Hash>::bucket_type::remove_elem_for(const Key& key)
{
    std::unique_lock lock(mtx);
    return bucket.remove_if([&](const bucket_elem& elem) { return elem.first == key; });    
}

template <typename Key, typename Value, typename Hash>
SimpleThreadSafeMap<Key, Value, Hash>::bucket_type::bucket_elem_iter SimpleThreadSafeMap<Key, Value, Hash>::bucket_type::find_elem_for(const Key& key)
{
    return std::find_if(bucket.begin(), bucket.end(), [&](const bucket_elem& elem) { return elem.first == key; });
}

template <typename Key, typename Value, typename Hash>
Value SimpleThreadSafeMap<Key, Value, Hash>::value_for(const Key& key, const Value& default_value) const
{
    return get_bucket(key).get_elem_for(key, default_value);
}

template <typename Key, typename Value, typename Hash>
void SimpleThreadSafeMap<Key, Value, Hash>::set_or_update_value_for(const Key& key, const Value& value)
{
    return get_bucket(key).set_or_update_elem_for(key, value);
}

template <typename Key, typename Value, typename Hash>
SimpleThreadSafeMap<Key, Value, Hash>::bucket_type::size_type SimpleThreadSafeMap<Key, Value, Hash>::remove_value_for(const Key& key)
{
    return get_bucket(key).remove_elem_for(key);
}
}