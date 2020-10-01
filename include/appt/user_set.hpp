#pragma once

#include "user_manager.hpp"
#include "user_hash.hpp"
#include <unordered_set>

inline namespace arba
{
namespace appt
{

template <class user_type, class user_sptr_hash>
class user_set : private std::unordered_set<std::shared_ptr<user_type>, user_sptr_hash>
{
private:
    using user_type_sptr = std::shared_ptr<user_type>;
    using user_set_container = std::unordered_set<user_type_sptr, user_sptr_hash>;
    using key_type = typename user_sptr_hash::key_type;

public:
    inline void set_user_manager(user_manager<user_type>& usr_manager);

    using user_set_container::empty;
    using user_set_container::size;
    using user_set_container::begin;
    using user_set_container::end;
    using user_set_container::cbegin;
    using user_set_container::cend;
    using iterator = user_set_container::iterator;
    using const_iterator = user_set_container::const_iterator;

    inline auto insert_user(user_type_sptr user_sptr) { return this->insert(std::move(user_sptr)); }
    inline void erase_user(const_iterator iter) { this->erase(iter); }

    template <typename... args_types>
    std::shared_ptr<user_type> create_user(args_types&&... args);
    const_iterator find_user(const key_type& key) const;
    iterator find_user(const key_type& key);
    void erase_user(const key_type& key);

private:
    user_manager<user_type>* user_manager_ = nullptr;
};

template <class user_type, class user_sptr_hash>
void user_set<user_type, user_sptr_hash>::set_user_manager(user_manager<user_type>& usr_manager)
{
    if (user_manager_)
        throw std::runtime_error("The user_set is already using another user_manager.");
    if (!empty())
        throw std::runtime_error("The user_set should be empty before link it to a user_manager.");
    user_manager_ = &usr_manager;
}

template <class user_type, class user_sptr_hash>
user_set<user_type, user_sptr_hash>::const_iterator
user_set<user_type, user_sptr_hash>::find_user(const key_type& key) const
{
    // when C++20 heterogenous lookup is ready:
//    return this->find(name);
    // instead of:
    return std::find_if(begin(), end(),
                        [&](const user_type_sptr& arg)
                        { return user_sptr_hash::user_id(*arg) == key; });
}

template <class user_type, class user_sptr_hash>
user_set<user_type, user_sptr_hash>::iterator
user_set<user_type, user_sptr_hash>::find_user(const key_type& key)
{
    // when C++20 heterogenous lookup is ready:
//    return this->find(name);
    // instead of:
    return std::find_if(begin(), end(),
                        [&](const user_type_sptr& arg)
                        { return user_sptr_hash::user_id(*arg) == key; });
}

template <class user_type, class user_sptr_hash>
void user_set<user_type, user_sptr_hash>::erase_user(const key_type& key)
{
    auto iter = find_user(key);
    if (iter != end())
    {
        auto user_id = (*iter)->id();
        std::size_t use_count = iter->use_count();
        this->erase(iter);
        if (user_manager_ && use_count == 1)
            user_manager_->release_user(user_id);
    }
}

template <class user_type, class user_sptr_hash>
template <typename... args_types>
std::shared_ptr<user_type> user_set<user_type, user_sptr_hash>::create_user(args_types&&... args)
{
    user_type_sptr user_sptr;
    if (user_manager_)
        user_sptr = user_manager_->create_user(std::forward<args_types>(args)...);
    else
        user_sptr = std::make_shared<user_type>(std::forward<args_types>(args)...);
    this->insert(user_sptr);
    return user_sptr;
}

}
}