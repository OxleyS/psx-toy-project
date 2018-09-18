namespace MeshPoly
{
    enum Type
    {
        MESHPT_TRI_FLAT, MESHPT_QUAD_FLAT,
        MESHPT_TRI_GOUR, MESHPT_QUAD_GOUR,
        MESHPT_TRI_FLAT_TEX, MESHPT_QUAD_FLAT_TEX,
        MESHPT_TRI_GOUR_TEX, MESHPT_QUAD_GOUR_TEX
    };

    enum Lighting
    {
        MESHPL_NONE,
        MESHPL_FLAT,
        MESHPL_SMOOTH
    };

    typedef ::Color Color;
    typedef ::Vec3Short Position;
    typedef ::Vec3Short Normal;
    struct UVCoords { u_char u, v; };

    // If flat lighting:
    // Base
    // 1 Normal
    // +8 bytes

    // If smooth lighting
    // Base
    // 3 or 4 Normal
    // +24-32 bytes

    // Base TriFlat:
    // 1 Color
    // 3 Position
    // 28 bytes

    // Base QuadFlat:
    // 1 Color
    // 4 Position
    // 36 bytes

    // Base TriGour:
    // 3 Color
    // 3 Position
    // 36 bytes

    // Base QuadGour:
    // 4 Color
    // 4 Position
    // 48 bytes

    // Base TriFlatTex:
    // 1 Color
    // 3 Position
    // 3 UVCoords
    // 2 bytes padding
    // 36 bytes

    // Base QuadFlatTex:
    // 1 Color
    // 4 Position
    // 4 UVCoords
    // 44 bytes

    // Base TriGourTex:
    // 3 Color
    // 3 Position
    // 3 UVCoords
    // 2 bytes padding
    // 44 bytes

    // Base QuadGourTex:
    // 4 Color
    // 4 Position
    // 4 UVCoords
    // 56 bytes
}
