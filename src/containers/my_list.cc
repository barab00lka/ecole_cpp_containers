#include "../include/my_list.h"

#include <string>
using std::string;

namespace my {

template class List<char>;
template class List<int>;
template class List<double>;
template class List<string>;
}  // namespace my
