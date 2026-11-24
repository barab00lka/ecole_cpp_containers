#include "../include/my_vector.h"

#include <string>
using std::string;

namespace my {
// externalizing template instances for correct linking, feel free to find more
// information
template class Vector<char>;
template class Vector<int>;
template class Vector<double>;
template class Vector<string>;
}  // namespace my
