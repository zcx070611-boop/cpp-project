#pragma once

#include <cstddef>
#include <cassert>
#include <utility>

namespace zcx
{
    template <class T>
    class vector
    {
    public:
        typedef T *iterator;
        typedef const T *const_iterator;

        // ---------- 构造 / 析构 / 拷贝 ----------
        vector()
            : _start(nullptr), _finish(nullptr), _end_of_storage(nullptr)
        {
        }

        vector(size_t n, const T &val = T())
            : _start(nullptr), _finish(nullptr), _end_of_storage(nullptr)
        {
            // TODO: 先 reserve(n)，再把 val 放进去 n 次
            reserve(n);
            for (size_t i = 0; i < n; ++i)
                push_back(val);
        }

        // 专门接 v(5, 7) 这种两个 int 的调用，避免被下面的区间构造抢走
        vector(int n, const T &val = T())
            : _start(nullptr), _finish(nullptr), _end_of_storage(nullptr)
        {
            // TODO: 和上面 size_t 版本做一样的事
            assert(n >= 0);
            reserve(static_cast<size_t>(n));
            for (int i = 0; i < n; ++i)
                push_back(val);
        }

        template <class InputIterator>
        vector(InputIterator first, InputIterator last)
            : _start(nullptr), _finish(nullptr), _end_of_storage(nullptr)
        {
            // TODO: 遍历 [first, last)，逐个 push_back

            while (first != last)
            {
                push_back(*first);
                ++first;
            }
        }

        vector(const vector &v)
            : _start(nullptr), _finish(nullptr), _end_of_storage(nullptr)
        {
            // TODO: 深拷贝 v 的元素，不能只拷三个指针
            _start = new T[v.capacity()];
            _finish = _start + v.size();
            _end_of_storage = _start + v.capacity();
            for (int i = 0; i < v.size(); i++)
            {
                _start[i] = v[i];
            }
        }

        vector &operator=(vector v)
        {
            // TODO: 深拷贝赋值，注意自赋值 v = v 不能出事
            swap(v);
            return *this;
        }

        ~vector()
        {
            // TODO: 释放 _start 指向的空间，并把三个指针置空
            delete[] _start;
            _start = _finish = _end_of_storage = nullptr;
        }

        // ---------- 迭代器 ----------
        iterator begin() { return _start; }
        iterator end() { return _finish; }
        const_iterator begin() const { return _start; }
        const_iterator end() const { return _finish; }

        // ---------- 容量 ----------
        size_t size() const { return _finish - _start; }
        size_t capacity() const { return _end_of_storage - _start; }
        bool empty() const { return _start == _finish; }

        void reserve(size_t n)
        {
            // TODO: n <= capacity() 时什么都不做
            // TODO: 开新空间，逐个拷贝旧元素（不能用 memcpy 搬 string 这类对象）
            // TODO: 释放旧空间，更新三个指针
            if (n <= capacity())
            {
                return;
            }
            else
            {
                int sz = size();
                iterator tmp = new T[n];
                for (int i = 0; i < sz; i++)
                {
                    tmp[i] = _start[i];
                }
                delete[] _start;
                _start = tmp;
                _end_of_storage = _start + n;
                _finish = _start + sz;
            }
        }

        // TODO: n < size() 时缩小；n > size() 时扩容并用 val 填满
        void resize(size_t n, const T &val = T())
        {
            if (n < size())
            {
                _finish = _start + n;
            }
            else if (n > size())
            {
                T tmp = val;
                reserve(n);
                while (_finish < _start + n)
                {
                    *_finish = tmp;
                    ++_finish;
                }
            }
        }

        // ---------- 元素访问 ----------
        T &operator[](size_t pos)
        {
            assert(pos < size());
            return _start[pos];
        }
        const T &operator[](size_t pos) const
        {
            assert(pos < size());
            return _start[pos];
        }
        T &front()
        {
            assert(!empty());
            return *_start;
        }
        const T &front() const
        {
            assert(!empty());
            return *_start;
        }
        T &back()
        {
            assert(!empty());
            return *(_finish - 1);
        }
        const T &back() const
        {
            assert(!empty());
            return *(_finish - 1);
        }

        // ---------- 修改 ----------
        void push_back(const T &val)
        {
            // TODO: 满了先扩容，再放元素
            if (_finish == _end_of_storage)
            {
                size_t newcap = capacity() == 0 ? 4 : capacity() + capacity() / 2;
                if (newcap <= capacity())
                    newcap = capacity() + 1;
                reserve(newcap);
            }
            _start[size()] = val;
            _finish++;
        }

        void pop_back()
        {
            // TODO: 非空时让 _finish 往前退一格
            if (!empty())
            {
                _finish--;
            }
        }

        iterator insert(iterator pos, const T &val)
        {
            // TODO: 先算 pos 的下标，扩容后 pos 会失效，要靠下标找回新位置
            // TODO: 把 pos 及之后的元素往后挪一格，放入 val，返回新位置
            int id = pos - _start;
            if (_finish == _end_of_storage)
            {
                size_t newcap = capacity() == 0 ? 4 : capacity() + capacity() / 2;
                if (newcap <= capacity())
                    newcap = capacity() + 1;
                reserve(newcap);
            }
            for (int i = size() - 1; i >= id; i--)
            {
                _start[i + 1] = _start[i];
            }
            _start[id] = val;
            pos = _start + id;
            _finish++;
            return pos;
        }

        iterator erase(iterator pos)
        {
            // TODO: 把 pos 之后的元素往前挪一格，返回被删位置的下一个元素
            assert(pos >= _start && pos < _finish);

            int id = pos - _start;
            for (int i = id; i + 1 < size(); i++)
            {
                _start[i] = _start[i + 1];
            }

            _finish--;
            return pos;
        }

        void clear()
        {
            _finish = _start;
        }

        void swap(vector &v)
        {
            // TODO: 只交换三个指针，不要逐个交换元素
            std::swap(_start, v._start);
            std::swap(_finish, v._finish);
            std::swap(_end_of_storage, v._end_of_storage);
        }

    private:
        T *_start;
        T *_finish;         // 真实有数据的下一个位置
        T *_end_of_storage; // 存储结尾的下一个位置
    };
}
