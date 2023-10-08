#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>

using namespace std;

// This class is used as an hack than to return void
class NoError
{
};

namespace errors
{

class Error
{
  private:
    string error;

  public:
    explicit Error(const string& errorMsg) : error(errorMsg){};

    string toString() const
    {
        return error;
    }

    void print() const
    {
        cout << error << endl;
    }
};

template <typename V, typename E>
concept ToString = requires(E e)
{
    {
        e.toString()
        } -> std::convertible_to<std::string>;
};

// IMPORTANT: You are not supposed to call `getError()` and `getValue()`
// without checking if isError() or isOk()
template <typename V, typename E> class Result
{
  private:
    variant<V, E> result;

  public:
    explicit Result(const V& value) : result(value)
    {
    }
    explicit Result(const E& error) : result(error)
    {
    }

    bool isError() const
    {
        return holds_alternative<E>(result);
    }

    bool isOk() const
    {
        return !isError();
    }

    E getError() const
    {
        return get<E>(result);
    }

    V getValue() const
    {
        return get<V>(result);
    }
};

} // namespace errors
