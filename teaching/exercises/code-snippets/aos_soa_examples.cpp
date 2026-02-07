#include <vector>

struct ParticleAoS {
    float x, y, z;
    bool active;
};
std::vector<ParticleAoS> particles;


#include <vector>

struct ParticleSoA {
    std::vector<float> x, y, z;
    std::vector<bool> active;
};

