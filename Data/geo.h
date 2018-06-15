static const float c_rayHitMinimumT = 0.001f;

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
    uint geoID;
};

struct Quad
{
    float3 a,b,c,d;
    float3 normal;
    float3 albedo;
    float3 emissive;
    uint geoID;
};

struct Sphere
{
    float3 position;
    float  radius;
    float3 albedo;
    float3 emissive;
    uint geoID;
};

float ScalarTriple(in float3 a, in float3 b, in float3 c)
{
    return dot(cross(a, b), c);
}

bool RayIntersects (in Ray ray, in Sphere sphere, inout CollisionInfo collisionInfo)
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
    if (collisionTime < c_rayHitMinimumT || (collisionInfo.collisionTime >= 0.0 && collisionTime > collisionInfo.collisionTime))
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
    collisionInfo.geoID = sphere.geoID;
    return true;
}

bool RayIntersects(in Ray ray, in Quad quad, inout CollisionInfo collisionInfo)
{
    // This function adapted from "Real Time Collision Detection" 5.3.5 Intersecting Line Against Quadrilateral
    // IntersectLineQuad()
    float3 pa = quad.a - ray.origin;
    float3 pb = quad.b - ray.origin;
    float3 pc = quad.c - ray.origin;
    // Determine which triangle to test against by testing against diagonal first
    float3 m = cross(pc, ray.direction);
    float3 r;
    float v = dot(pa, m); // ScalarTriple(pq, pa, pc);
    if (v >= 0.0f) {
        // Test intersection against triangle abc
        float u = -dot(pb, m); // ScalarTriple(pq, pc, pb);
        if (u < 0.0f) return false;
        float w = ScalarTriple(ray.direction, pb, pa);
        if (w < 0.0f) return false;
        // Compute r, r = u*a + v*b + w*c, from barycentric coordinates (u, v, w)
        float denom = 1.0f / (u + v + w);
        u *= denom;
        v *= denom;
        w *= denom; // w = 1.0f - u - v;
        r = u*quad.a + v*quad.b + w*quad.c;
    }
    else {
        // Test intersection against triangle dac
        float3 pd = quad.d - ray.origin;
        float u = dot(pd, m); // ScalarTriple(pq, pd, pc);
        if (u < 0.0f) return false;
        float w = ScalarTriple(ray.direction, pa, pd);
        if (w < 0.0f) return false;
        v = -v;
        // Compute r, r = u*a + v*d + w*c, from barycentric coordinates (u, v, w)
        float denom = 1.0f / (u + v + w);
        u *= denom;
        v *= denom;
        w *= denom; // w = 1.0f - u - v;
        r = u*quad.a + v*quad.d + w*quad.c;
    }

    // make sure normal is facing opposite of ray direction.
    // this is for if we are hitting the object from the inside / back side.
    float3 normal = quad.normal;
    if (dot(quad.normal, ray.direction) > 0.0f)
        normal *= -1.0f;

    // figure out the time t that we hit the plane (quad)
    float t;
    if (abs(ray.direction[0]) > 0.0f)
        t = (r[0] - ray.origin[0]) / ray.direction[0];
    else if (abs(ray.direction[1]) > 0.0f)
        t = (r[1] - ray.origin[1]) / ray.direction[1];
    else if (abs(ray.direction[2]) > 0.0f)
        t = (r[2] - ray.origin[2]) / ray.direction[2];

    // only positive time hits allowed!
    if (t < 0.0f)
        return false;

    //enforce min and max distance
    float collisionTime = t;
    if (collisionTime < c_rayHitMinimumT || (collisionInfo.collisionTime >= 0.0 && collisionTime > collisionInfo.collisionTime))
        return false;

    collisionInfo.collisionTime = collisionTime;
    collisionInfo.normal = normal;
    collisionInfo.albedo = quad.albedo;
    collisionInfo.emissive = quad.emissive;
    collisionInfo.geoID = quad.geoID;
    return true;
}