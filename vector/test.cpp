#include "my_vector.h"
#include <iostream>
#include <string>

static int g_fail = 0;

// 检查失败就停掉当前这个测试，避免后面越界访问直接崩掉
#define CHECK(cond)                                                     \
    do {                                                                \
        if (!(cond)) {                                                  \
            std::cout << "  FAIL line " << __LINE__ << ": " #cond << "\n"; \
            ++g_fail;                                                   \
            return;                                                     \
        }                                                               \
    } while (0)

void test_push_back()
{
    zcx::vector<int> v;
    CHECK(v.empty());
    CHECK(v.size() == 0);

    for (int i = 1; i <= 5; ++i) v.push_back(i);

    CHECK(v.size() == 5);
    CHECK(v.capacity() >= 5);
    CHECK(v[0] == 1 && v[4] == 5);
    CHECK(v.front() == 1);
    CHECK(v.back() == 5);
    v[2] = 99;
    CHECK(v[2] == 99);
}

void test_growth_and_reserve()
{
    zcx::vector<int> v;
    v.reserve(50);
    CHECK(v.capacity() >= 50);
    CHECK(v.size() == 0);

    size_t cap = v.capacity();
    v.reserve(10); // 小于现有容量，不应缩容
    CHECK(v.capacity() == cap);

    for (int i = 0; i < 100; ++i) v.push_back(i);
    CHECK(v.size() == 100);
    CHECK(v.capacity() >= 100);
    for (int i = 0; i < 100; ++i) CHECK(v[i] == i);
}

void test_insert_erase()
{
    zcx::vector<int> v;
    for (int i = 1; i <= 4; ++i) v.push_back(i); // 1 2 3 4
    CHECK(v.size() == 4);

    auto it = v.insert(v.begin(), 0);           // 0 1 2 3 4
    CHECK(v.size() == 5);
    CHECK(*it == 0);
    CHECK(v[0] == 0 && v[4] == 4);

    it = v.insert(v.begin() + 2, 99);           // 0 1 99 2 3 4
    CHECK(v.size() == 6);
    CHECK(*it == 99);
    CHECK(v[3] == 2);

    it = v.erase(v.begin() + 2);                // 0 1 2 3 4
    CHECK(v.size() == 5);
    CHECK(*it == 2);                            // erase 返回下一个元素

    v.erase(v.end() - 1);                       // 0 1 2 3
    CHECK(v.size() == 4);
    CHECK(v.back() == 3);
}

void test_pop_resize_clear()
{
    zcx::vector<int> v;
    for (int i = 1; i <= 3; ++i) v.push_back(i);
    CHECK(v.size() == 3);

    v.pop_back();
    CHECK(v.size() == 2);
    CHECK(v.back() == 2);

    v.resize(5, 7);                             // 1 2 7 7 7
    CHECK(v.size() == 5);
    CHECK(v[2] == 7 && v[4] == 7);

    v.resize(1);                                // 1
    CHECK(v.size() == 1);
    CHECK(v[0] == 1);

    size_t cap = v.capacity();
    v.clear();
    CHECK(v.size() == 0);
    CHECK(v.empty());
    CHECK(v.capacity() == cap);                 // clear 不释放容量
}

void test_copy_and_assign()
{
    zcx::vector<int> v1;
    for (int i = 1; i <= 3; ++i) v1.push_back(i);
    CHECK(v1.size() == 3);

    zcx::vector<int> v2(v1);                    // 拷贝构造
    CHECK(v2.size() == 3);
    CHECK(v2[0] == 1 && v2[2] == 3);
    v2[0] = 100;                                // 改拷贝不能影响原对象
    CHECK(v1[0] == 1);

    zcx::vector<int> v3;
    v3 = v1;                                    // 拷贝赋值
    CHECK(v3.size() == 3);
    CHECK(v3[1] == 2);

    v3 = v3;                                    // 自赋值不能出事
    CHECK(v3.size() == 3);
    CHECK(v3[0] == 1 && v3[2] == 3);
}

void test_string_deep_copy()
{
    // 用 string 测扩容搬数据：memcpy 浅拷贝会在这里重复释放 / 数据坏掉
    zcx::vector<std::string> v;
    for (int i = 0; i < 20; ++i) v.push_back("hello_" + std::to_string(i));
    CHECK(v.size() == 20);
    CHECK(v[0] == "hello_0");
    CHECK(v[19] == "hello_19");

    zcx::vector<std::string> copy(v);
    CHECK(copy.size() == 20);
    copy[0] = "changed";
    CHECK(v[0] == "hello_0");
    CHECK(copy[0] == "changed");
}

void test_constructors_and_range_for()
{
    zcx::vector<int> v1(5, 7);                  // 7 7 7 7 7
    CHECK(v1.size() == 5);
    CHECK(v1[0] == 7 && v1[4] == 7);

    int arr[] = {10, 20, 30};
    zcx::vector<int> v2(arr, arr + 3);          // 区间构造
    CHECK(v2.size() == 3);
    CHECK(v2[0] == 10 && v2[2] == 30);

    int sum = 0;
    for (int x : v2) sum += x;                  // 范围 for 靠 begin/end
    CHECK(sum == 60);

    const zcx::vector<int>& cv = v2;            // const 访问
    CHECK(cv[1] == 20);
    CHECK(cv.front() == 10);
    CHECK(cv.back() == 30);
}

void test_swap()
{
    zcx::vector<int> v1;
    v1.push_back(1);
    v1.push_back(2);
    zcx::vector<int> v2;
    v2.push_back(9);
    CHECK(v1.size() == 2 && v2.size() == 1);

    v1.swap(v2);
    CHECK(v1.size() == 1);
    CHECK(v1[0] == 9);
    CHECK(v2.size() == 2);
    CHECK(v2[0] == 1 && v2[1] == 2);
}

int main()
{
    struct { const char* name; void (*fn)(); } tests[] = {
        {"push_back", test_push_back},
        {"growth_and_reserve", test_growth_and_reserve},
        {"insert_erase", test_insert_erase},
        {"pop_resize_clear", test_pop_resize_clear},
        {"copy_and_assign", test_copy_and_assign},
        {"string_deep_copy", test_string_deep_copy},
        {"constructors_and_range_for", test_constructors_and_range_for},
        {"swap", test_swap},
    };

    for (auto& t : tests) {
        int before = g_fail;
        t.fn();
        std::cout << (g_fail == before ? "[PASS] " : "[FAIL] ") << t.name << "\n";
    }

    std::cout << (g_fail == 0 ? "ALL PASS\n" : "SOME FAILED\n");
    return g_fail == 0 ? 0 : 1;
}
