#pragma once

#include "robin-hood-hashing/robin_hood.h"

namespace minote::base {

template<typename Key, typename T>
using hashmap = robin_hood::unordered_node_map<Key, T>;

}