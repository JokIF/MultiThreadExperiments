#pragma once
#include <list>
#include <memory>
#include <shared_mutex>
#include <vector>
#include <algorithm>
#include <expected>

namespace ThreadSafeStructs
{
enum class MapError { NotFound };

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

        std::expected<Value, MapError>  get(const Key& key) const;
        void        set_or_update(const Key& key, const Value& value);
        size_type   remove_elem(const Key& key)
        {
            std::unique_lock lock(mtx);
            return bucket.remove_if([&](const bucket_elem& elem) { return elem.first == key; });
        }

    private:
        template<typename Self>
        decltype(auto)    find_elem(this Self&& self, const Key& key) {
            return std::find_if(self.bucket.begin(), self.bucket.end(), [&](const bucket_elem& elem) { return elem.first == key; });
        }

        std::list<bucket_elem>      bucket;
        mutable std::shared_mutex   mtx;
    };

public:
    SimpleThreadSafeMap(size_t bucket_size = 19, Hash hasher = Hash());

    SimpleThreadSafeMap(const SimpleThreadSafeMap&) = delete;
    SimpleThreadSafeMap(SimpleThreadSafeMap&&) = delete;
    SimpleThreadSafeMap& operator=(const SimpleThreadSafeMap&) = delete;
    SimpleThreadSafeMap& operator=(SimpleThreadSafeMap&&) = delete;


    std::expected<Value, MapError>   get_value(const Key& key) const;
    void    set_or_update_value(const Key& key, const Value& value);
    bucket_type::size_type  remove_value(const Key& key);

private:
    template <typename Self>
    decltype(auto)   get_bucket(this Self&& self, const Key& key) {
        return *self.buckets[self.hasher(key) % self.buckets.size()];
    }

    std::vector<std::unique_ptr<bucket_type>> buckets;
    Hash hasher;
};

template <typename Key, typename Value, typename Hash>
SimpleThreadSafeMap<Key, Value, Hash>::SimpleThreadSafeMap(size_t bucket_size, Hash hasher)
    : hasher(std::move(hasher))
{
    buckets.reserve(bucket_size);
    for (size_t i = 0; i < bucket_size; ++i)
        buckets.emplace_back(std::make_unique<bucket_type>());
}

template <typename Key, typename Value, typename Hash>
std::expected<Value, MapError> SimpleThreadSafeMap<Key, Value, Hash>::bucket_type::get(const Key& key) const
{
    std::shared_lock lock(mtx);
    const auto&& iter = find_elem(key);
    if (iter == bucket.end())
        return std::unexpected(MapError::NotFound);

    return iter->second;
}

template <typename Key, typename Value, typename Hash>
void SimpleThreadSafeMap<Key, Value, Hash>::bucket_type::set_or_update(const Key& key, const Value& value)
{
    std::unique_lock lock(mtx);
    auto&& iter = find_elem(key);
    if (iter != bucket.end())
        iter->second = value;
    else
        bucket.emplace_back(key, value);
}

template <typename Key, typename Value, typename Hash>
std::expected<Value, MapError> SimpleThreadSafeMap<Key, Value, Hash>::get_value(const Key& key) const
{
    return get_bucket(key).get(key);
}

template <typename Key, typename Value, typename Hash>
void SimpleThreadSafeMap<Key, Value, Hash>::set_or_update_value(const Key& key, const Value& value)
{
    return get_bucket(key).set_or_update(key, value);
}

template <typename Key, typename Value, typename Hash>
SimpleThreadSafeMap<Key, Value, Hash>::bucket_type::size_type SimpleThreadSafeMap<Key, Value, Hash>::remove_value(const Key& key)
{
    return get_bucket(key).remove_elem(key);
}
}