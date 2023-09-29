#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>

using namespace std;

namespace errors
{

class Error
{
  private:
    string error;

  public:
    explicit Error(const string& errorMsg) : error(errorMsg){};

    string getError() const
    {
        return error;
    }

    void print() const
    {
        cout << error << endl;
    }
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
