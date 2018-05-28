static const float c_rayHitMinimumT = 0.01f;

struct Ray
{
    float3 origin;
    float3 direction;
};

struct CollisionInfo
{
    float collisionTime;  // init to -1
    float3 normal;
    float3 albedo;
    float3 emissive;
};

struct Sphere
{
    float3 position;
    float radius;
    float3 albedo;
    float3 emissive;
};

inline bool RayIntersectsSphere (in Ray ray, in Sphere sphere, inout CollisionInfo collisionInfo)
{
    //get the vector from the center of this circle to where the ray begins.
    float3 m = ray.origin - sphere.position;

    //get the dot product of the above vector and the ray's vector
    float b = dot(m, ray.direction);

    float c = dot(m, m) - sphere.radius * sphere.radius;

    //exit if r's origin outside s (c > 0) and r pointing away from s (b > 0)
    if (c > 0.0 && b > 0.0)
        return false;

    //calculate discriminant
    float discr = b * b - c;

    //a negative discriminant corresponds to ray missing sphere
    if (discr <= 0.0)
        return false;

    //ray now found to intersect sphere, compute smallest t value of intersection
    float collisionTime = -b - sqrt(discr);

    //if t is negative, ray started inside sphere so clamp t to zero and remember that we hit from the inside
    if (collisionTime < 0.0)
        collisionTime = -b + sqrt(discr);

    //enforce min and max distance
    if (collisionTime < c_rayHitMinimumT || collisionInfo.collisionTime >= 0.0 && collisionTime > collisionInfo.collisionTime)
        return false;

    float3 normal = normalize((ray.origin + ray.direction * collisionTime) - sphere.position);

    // make sure normal is facing opposite of ray direction.
    // this is for if we are hitting the object from the inside / back side.
    if (dot(normal, ray.direction) > 0.0f)
        normal *= -1.0f;

    collisionInfo.collisionTime = collisionTime;
    collisionInfo.normal = normal;
    collisionInfo.albedo = sphere.albedo;
    collisionInfo.emissive = sphere.emissive;
    return true;
}