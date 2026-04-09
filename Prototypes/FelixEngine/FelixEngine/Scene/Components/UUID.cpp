//
//  UUID.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/9/26.
//

#include "UUID.hpp"
#include <random>


using namespace std;
namespace Felix {

static random_device sRandomDevice;
static mt19937_64 sEngine(sRandomDevice());
static uniform_int_distribution<uint64_t> sUniformDistribution;

UUID::UUID(): mUUID(sUniformDistribution(sEngine)) {}
UUID::UUID(uint64_t uuid): mUUID(uuid) {}

}
