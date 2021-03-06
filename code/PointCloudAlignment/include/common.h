#pragma once

#include <functional>

#include <Eigen/Core>
#include <Eigen/QR>

#include "variables.h"
#include "point_normal_k.h"

using namespace std;

typedef Eigen::Vector3i ivec3;
typedef Eigen::Vector2f vec2;
typedef Eigen::Vector3f vec3;
typedef Eigen::Vector4f vec4;
typedef Eigen::Vector3d vec3d;
typedef Eigen::Matrix4f mat4;
typedef Eigen::Matrix3f mat3;
typedef Eigen::Matrix2f mat2;
typedef Eigen::MatrixXf matX;

typedef pcl::Histogram<NB_BINS_APFH * NB_FEATURES_APFH> APFHSignature;

typedef pcl::PointCloud<pcl::PFHSignature125> PFHCloud;
typedef pcl::PointCloud<pcl::FPFHSignature33> FPFHCloud;
typedef pcl::PointCloud<APFHSignature> APFHCloud;

inline float squaredDistance(vec3 p1, vec3 p2) {
    vec3 p = p1 - p2;
    return p.dot(p);
}

inline float distance(vec3 p1, vec3 p2) {
    return std::sqrt(squaredDistance(p1, p2));
}

inline std::vector<int> intersect_sets(std::vector<int> &v1, std::vector<int> &v2) {
    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());

    std::vector<int> vOut;
    std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(vOut));

    return vOut;
}

inline float approxR(float curv, float d1, float d2, float sigma, float epsilon, float density) {
    float sig2 = sigma * sigma;
    float left = d1 * sigma / std::sqrt(epsilon * density);
    float right = d2 * sig2;
    float r = std::pow(1.0f / curv * (left + right), 1.0f/3.0f);
    return r;
}

inline vec3 computePlaneCenter(PointNormalKCloud::Ptr p_cloud, vector<int> indices)
{
    vec3 center(0, 0, 0);

    for(int i: indices)
    {
        PointNormalK p = p_cloud->points[i];
        center += vec3(p.x, p.y, p.z);
    }

    center /= indices.size();

    return center;
}

inline int positive_modulo(int i, int n)
{
    return (i % n + n) % n;
}

inline ivec3 positive_modulo(ivec3 i, int n)
{
    return ivec3(positive_modulo(i.x(), n),
                positive_modulo(i.y(), n),
                positive_modulo(i.z(), n));
}

inline vec3 pclToVec3(pcl::PointXYZRGB p)
{
    return vec3(p.x, p.y, p.z);
}

inline vec3 pclToVec3(PointNormalK p)
{
    return vec3(p.x, p.y, p.z);
}

inline vec4 pointToVec4(pcl::PointNormal &p)
{
    return vec4(p.x, p.y, p.z, 0);
}

inline vec4 normalToVec4(pcl::PointNormal &p)
{
    return vec4(p.normal_x, p.normal_y, p.normal_z, 0);
}

inline float roundTo(float x, int n)
{
    float mult = pow(10.0f, n);
    float x_rounded = x * mult;
    x_rounded = std::round(x_rounded);
    return x_rounded / mult;
}

inline float crossProduct(vec2 x, vec2 y)
{
    return (x.x() * y.y()) - (x.y() * y.x());
}

inline mat3 getRandomRotation()
{
    // QR decomposition on a random 3x3 matrix, Q is the random rotation
    mat3 n = mat3::Random();
    Eigen::HouseholderQR<mat3> qrDecomp(n);
    mat3 Q = qrDecomp.householderQ();
    return Q;
}

inline mat4 getRandomTranslation()
{
    vec3 r = vec3::Random();
    mat4 T = Eigen::Affine3f(Eigen::Translation3f(r)).matrix();
    return T;
}

inline float getStdDeviation(vector<float> list)
{
    float mean(0);

    for(float i: list)
    {
        mean += i;
    }
    mean /= list.size();

    float dev(0);

    for(float i: list)
    {
        dev += pow(i - mean, 2);
    }
    dev /= list.size();
    return sqrt(dev);
}
