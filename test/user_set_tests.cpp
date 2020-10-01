#include <appt/user_set.hpp>
#include <gtest/gtest.h>
#include <unordered_set>
#include <cstdlib>

using namespace std::string_literals;

//------------
// application
//------------

class ut_user : public appt::user
{
public:
    virtual ~ut_user() = default;

    ut_user(const std::string& name = "") : name_(name) {}
    const std::string& name() const { return name_; }

public:
    std::string name_;
};

using ut_user_sptr = std::shared_ptr<ut_user>;

class ut_user_2 : public appt::user
{
public:
    virtual ~ut_user_2() = default;

    ut_user_2(const std::string& name = "") : name(name) {}

    std::string name;
};

using ut_user_2_sptr = std::shared_ptr<ut_user_2>;

//--------------------------------------------------------------------------------

TEST(user_set_tests, test_user_set_id)
{
    using sptr_hash = appt::user_sptr_id_hash<ut_user>;

    appt::user_set<ut_user, sptr_hash> ut_user_set;
    std::shared_ptr user_sptr = std::make_shared<ut_user>("Zeus");
    user_sptr->set_id(6);
    appt::user::id_type id = user_sptr->id();
    ut_user_set.insert_user(user_sptr);
    ut_user_set.insert_user(std::make_shared<ut_user>("Athena"));
    auto iter = ut_user_set.find_user(id);
    ASSERT_TRUE(iter != ut_user_set.end());
    ASSERT_EQ((*iter)->id(), id);
    ut_user_set.erase_user(id);
    iter = ut_user_set.find_user(id);
    ASSERT_TRUE(iter == ut_user_set.end());
}

TEST(user_set_tests, test_user_set_name)
{
    using sptr_hash = appt::user_sptr_name_hash<ut_user>;

    appt::user_set<ut_user, sptr_hash> ut_user_set;
    std::string user_name("Zeus");
    std::shared_ptr user_sptr = std::make_shared<ut_user>(user_name);
    ut_user_set.insert_user(user_sptr);
    ut_user_set.insert_user(std::make_shared<ut_user>("Athena"));
    auto iter = ut_user_set.find_user(user_name);
    ASSERT_TRUE(iter != ut_user_set.end());
    ASSERT_EQ((*iter)->name(), user_name);
    ut_user_set.erase_user(user_name);
    iter = ut_user_set.find_user(user_name);
    ASSERT_TRUE(iter == ut_user_set.end());
}

TEST(user_set_tests, test_user_set_name_2)
{
    using sptr_hash = appt::user_sptr_name_hash<ut_user_2>;

    appt::user_set<ut_user_2, sptr_hash> ut_user_set;
    std::string user_name("Zeus");
    std::shared_ptr user_sptr = std::make_shared<ut_user_2>(user_name);
    ut_user_set.insert_user(user_sptr);
    ut_user_set.insert_user(std::make_shared<ut_user_2>("Athena"));
    auto iter = ut_user_set.find_user(user_name);
    ASSERT_TRUE(iter != ut_user_set.end());
    ASSERT_EQ((*iter)->name, user_name);
    ut_user_set.erase_user(user_name);
    iter = ut_user_set.find_user(user_name);
    ASSERT_TRUE(iter == ut_user_set.end());
}

TEST(user_set_tests, test_user_set_mem_fn)
{
    using sptr_hash = appt::user_sptr_mem_fn_hash<ut_user, std::string, &ut_user::name>;

    appt::user_set<ut_user, sptr_hash> ut_user_set;
    std::string user_name("Zeus");
    std::shared_ptr user_sptr = std::make_shared<ut_user>(user_name);
    ut_user_set.insert_user(user_sptr);
    ut_user_set.insert_user(std::make_shared<ut_user>("Athena"));
    auto iter = ut_user_set.find_user(user_name);
    ASSERT_TRUE(iter != ut_user_set.end());
    ASSERT_EQ((*iter)->name(), user_name);
    ut_user_set.erase_user(user_name);
    iter = ut_user_set.find_user(user_name);
    ASSERT_TRUE(iter == ut_user_set.end());
}

TEST(user_set_tests, test_user_set_mem)
{
    using sptr_hash = appt::user_sptr_mem_hash<ut_user_2, std::string, &ut_user_2::name>;

    appt::user_set<ut_user_2, sptr_hash> ut_user_set;
    std::string user_name("Zeus");
    std::shared_ptr user_sptr = std::make_shared<ut_user_2>(user_name);
    ut_user_set.insert_user(user_sptr);
    ut_user_set.insert_user(std::make_shared<ut_user_2>("Athena"));
    auto iter = ut_user_set.find_user(user_name);
    ASSERT_TRUE(iter != ut_user_set.end());
    ASSERT_EQ((*iter)->name, user_name);
    ut_user_set.erase_user(user_name);
    iter = ut_user_set.find_user(user_name);
    ASSERT_TRUE(iter == ut_user_set.end());
}

TEST(user_set_tests, test_user_set_id_with_user_manager)
{
    using sptr_hash = appt::user_sptr_id_hash<ut_user>;

    appt::user_manager<ut_user> user_manager;

    appt::user_set<ut_user, sptr_hash> ut_user_set;
    ut_user_set.set_user_manager(user_manager);

    ut_user_sptr alpha_user = ut_user_set.create_user("Alpha");
    appt::user_id alpha_id = alpha_user->id();
    ut_user_set.create_user("Beta");

    auto iter = ut_user_set.find_user(alpha_id);
    ASSERT_TRUE(iter != ut_user_set.end());
    ASSERT_EQ((*iter)->id(), alpha_id);
    ASSERT_EQ((*iter)->name(), "Alpha");
    ASSERT_EQ(user_manager.shared_user(alpha_id), alpha_user);

    alpha_user.reset();
    ut_user_set.erase_user(alpha_id);
    iter = ut_user_set.find_user(alpha_id);
    ASSERT_TRUE(iter == ut_user_set.end());
    ASSERT_EQ(user_manager.shared_user(alpha_id), nullptr);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}