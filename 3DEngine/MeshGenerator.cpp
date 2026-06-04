#include "pch.h"
#include "MeshGenerator.h"
#include <corecrt_math_defines.h>

namespace
{
    struct Vec3
    {
        float x, y, z;
    };

    Vec3 Normalize(const Vec3& v)
    {
        float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

        if (len <= 0.0f)
            return { 0.0f, 0.0f, 0.0f };

        return { v.x / len, v.y / len, v.z / len };
    }

    Vec3 Cross(const Vec3& a, const Vec3& b)
    {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }

    Vec3 Sub(const Vec3& a, const Vec3& b)
    {
        return {
            a.x - b.x,
            a.y - b.y,
            a.z - b.z
        };
    }

    void AddTriangle(
        std::vector<GLfloat>& vertices,
        std::vector<GLuint>& indices,
        GLuint& indexOffset,
        const Vec3& a,
        const Vec3& b,
        const Vec3& c)
    {
        Vec3 u = Sub(b, a);
        Vec3 v = Sub(c, a);

        Vec3 n = Normalize(Cross(u, v));

        const float uv[3][2] = {
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 0.5f, 1.0f }
        };

        const Vec3 verts[3] = { a, b, c };

        for (int i = 0; i < 3; ++i)
        {
            vertices.insert(vertices.end(), {
                verts[i].x,
                verts[i].y,
                verts[i].z,

                n.x,
                n.y,
                n.z,

                uv[i][0],
                uv[i][1]
                });

            indices.push_back(indexOffset++);
        }
    }

    void AddPolygon(
        std::vector<GLfloat>& vertices,
        std::vector<GLuint>& indices,
        GLuint& indexOffset,
        const std::vector<Vec3>& polygon)
    {
        for (size_t i = 1; i < polygon.size() - 1; ++i)
        {
            AddTriangle(
                vertices,
                indices,
                indexOffset,
                polygon[0],
                polygon[i],
                polygon[i + 1]);
        }
    }
}

namespace Engine
{
    std::shared_ptr<Mesh> MeshGenerator::Cube(float size)
    {
        float h = size / 2.0f;

        std::vector<GLfloat> vertices = {
            // (+Z)
            -h, -h,  h,    0.0f,  0.0f,  1.0f,    0.0f, 0.0f,
             h, -h,  h,    0.0f,  0.0f,  1.0f,    1.0f, 0.0f,
             h,  h,  h,    0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
            -h,  h,  h,    0.0f,  0.0f,  1.0f,    0.0f, 1.0f,

            // (-Z)
             h, -h, -h,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f,
            -h, -h, -h,    0.0f,  0.0f, -1.0f,    1.0f, 0.0f,
            -h,  h, -h,    0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
             h,  h, -h,    0.0f,  0.0f, -1.0f,    0.0f, 1.0f,

             // (-X)
             -h, -h, -h,  -1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
             -h, -h,  h,  -1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
             -h,  h,  h,  -1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
             -h,  h, -h,  -1.0f,  0.0f,  0.0f,    0.0f, 1.0f,

             // (+X)
              h, -h,  h,   1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
              h, -h, -h,   1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
              h,  h, -h,   1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
              h,  h,  h,   1.0f,  0.0f,  0.0f,    0.0f, 1.0f,

              // (+Y)
              -h,  h,  h,   0.0f,  1.0f,  0.0f,    0.0f, 0.0f,
               h,  h,  h,   0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
               h,  h, -h,   0.0f,  1.0f,  0.0f,    1.0f, 1.0f,
              -h,  h, -h,   0.0f,  1.0f,  0.0f,    0.0f, 1.0f,

              // (-Y)
              -h, -h, -h,   0.0f, -1.0f,  0.0f,    0.0f, 0.0f,
               h, -h, -h,   0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
               h, -h,  h,   0.0f, -1.0f,  0.0f,    1.0f, 1.0f,
              -h, -h,  h,   0.0f, -1.0f,  0.0f,    0.0f, 1.0f,
        };

        std::vector<GLuint> indices;
        indices.reserve(36);

        for (int i = 0; i < 6; ++i)
        {
            GLuint offset = i * 4;

            indices.push_back(offset + 0);
            indices.push_back(offset + 1);
            indices.push_back(offset + 2);

            indices.push_back(offset + 2);
            indices.push_back(offset + 3);
            indices.push_back(offset + 0);
        }

        return std::make_shared<Mesh>(vertices, indices);
    }


    std::shared_ptr<Mesh> MeshGenerator::Tetrahedron(float scale)
    {
        std::vector<Vec3> v = {
            Normalize({  1.0f,  1.0f,  1.0f }),
            Normalize({ -1.0f, -1.0f,  1.0f }),
            Normalize({ -1.0f,  1.0f, -1.0f }),
            Normalize({  1.0f, -1.0f, -1.0f })
        };

        for (auto& p : v)
        {
            p.x *= scale;
            p.y *= scale;
            p.z *= scale;
        }

        int faces[4][3] = {
            {0, 2, 1},
            {0, 1, 3},
            {0, 3, 2},
            {1, 2, 3}
        };

        std::vector<GLfloat> vertices;
        std::vector<GLuint> indices;

        GLuint offset = 0;

        for (int i = 0; i < 4; ++i)
        {
            AddTriangle(
                vertices,
                indices,
                offset,
                v[faces[i][0]],
                v[faces[i][1]],
                v[faces[i][2]]);
        }

        return std::make_shared<Mesh>(vertices, indices);
    }


    std::shared_ptr<Mesh> MeshGenerator::Octahedron(float scale)
    {
        const float s = scale;

        Vec3 v[6] = {
            {  s, 0.0f, 0.0f },
            { -s, 0.0f, 0.0f },

            { 0.0f,  s, 0.0f },
            { 0.0f, -s, 0.0f },

            { 0.0f, 0.0f,  s },
            { 0.0f, 0.0f, -s }
        };

        int faces[8][3] = {
            { 0, 2, 4 },
            { 4, 2, 1 },
            { 1, 2, 5 },
            { 5, 2, 0 },

            { 4, 3, 0 },
            { 1, 3, 4 },
            { 5, 3, 1 },
            { 0, 3, 5 }
        };

        std::vector<GLfloat> vertices;
        std::vector<GLuint> indices;

        GLuint offset = 0;

        for (int i = 0; i < 8; ++i)
        {
            AddTriangle(
                vertices,
                indices,
                offset,
                v[faces[i][0]],
                v[faces[i][1]],
                v[faces[i][2]]);
        }

        return std::make_shared<Mesh>(vertices, indices);
    }


    std::shared_ptr<Mesh> MeshGenerator::Icosahedron(float scale)
    {
        const float phi = (1.0f + std::sqrt(5.0f)) * 0.5f;

        std::vector<Vec3> v = {
            Normalize({ -1,  phi, 0 }),
            Normalize({  1,  phi, 0 }),
            Normalize({ -1, -phi, 0 }),
            Normalize({  1, -phi, 0 }),

            Normalize({ 0, -1,  phi }),
            Normalize({ 0,  1,  phi }),
            Normalize({ 0, -1, -phi }),
            Normalize({ 0,  1, -phi }),

            Normalize({  phi, 0, -1 }),
            Normalize({  phi, 0,  1 }),
            Normalize({ -phi, 0, -1 }),
            Normalize({ -phi, 0,  1 })
        };

        for (auto& p : v)
        {
            p.x *= scale;
            p.y *= scale;
            p.z *= scale;
        }

        int faces[20][3] = {
            {0,11,5},
            {0,5,1},
            {0,1,7},
            {0,7,10},
            {0,10,11},

            {1,5,9},
            {5,11,4},
            {11,10,2},
            {10,7,6},
            {7,1,8},

            {3,9,4},
            {3,4,2},
            {3,2,6},
            {3,6,8},
            {3,8,9},

            {4,9,5},
            {2,4,11},
            {6,2,10},
            {8,6,7},
            {9,8,1}
        };

        std::vector<GLfloat> vertices;
        std::vector<GLuint> indices;

        GLuint offset = 0;

        for (int i = 0; i < 20; ++i)
        {
            AddTriangle(
                vertices,
                indices,
                offset,
                v[faces[i][0]],
                v[faces[i][1]],
                v[faces[i][2]]);
        }

        return std::make_shared<Mesh>(vertices, indices);
    }


    std::shared_ptr<Mesh> MeshGenerator::Dodecahedron(float scale)
    {
        const float phi = (1.0f + std::sqrt(5.0f)) * 0.5f;
        const float a = 1.0f;
        const float b = 1.0f / phi;
        const float c = phi;

        std::vector<Vec3> v = {
            {-a,-a,-a}, {-a,-a, a}, {-a, a,-a}, {-a, a, a},
            { a,-a,-a}, { a,-a, a}, { a, a,-a}, { a, a, a},
            { 0,-b,-c}, { 0,-b, c}, { 0, b,-c}, { 0, b, c},
            {-b,-c, 0}, {-b, c, 0}, { b,-c, 0}, { b, c, 0},
            {-c, 0,-b}, { c, 0,-b}, {-c, 0, b}, { c, 0, b}
        };

        for (auto& p : v)
        {
            p = Normalize(p);
            p.x *= scale; p.y *= scale; p.z *= scale;
        }

        int faces[12][5] = {
            {  0, 16,  2, 10,  8 },
            {  4,  8, 10,  6, 17 },
            {  1,  9, 11,  3, 18 },
            {  5,  9, 11,  7, 19 },
            {  0, 12, 14,  4,  8 },
            {  1, 12, 14,  5,  9 },
            {  2, 13, 15,  6, 10 },
            {  3, 13, 15,  7, 11 },
            {  0, 16, 18,  1, 12 },
            {  2, 16, 18,  3, 13 },
            {  4, 17, 19,  5, 14 },
            {  6, 17, 19,  7, 15 }
        };

        std::vector<GLfloat> vertices;
        std::vector<GLuint> indices;
        GLuint offset = 0;

        for (int i = 0; i < 12; ++i)
        {
            Vec3 center = { 0,0,0 };
            for (int j = 0; j < 5; ++j) {
                center.x += v[faces[i][j]].x;
                center.y += v[faces[i][j]].y;
                center.z += v[faces[i][j]].z;
            }
            center = Normalize(center);

            for (int t = 1; t < 4; ++t)
            {
                Vec3 a0 = v[faces[i][0]];
                Vec3 a1 = v[faces[i][t]];
                Vec3 a2 = v[faces[i][t + 1]];

                Vec3 u = Sub(a1, a0);
                Vec3 vv = Sub(a2, a0);
                Vec3 triNormal = Normalize(Cross(u, vv));

                if (triNormal.x * center.x + triNormal.y * center.y + triNormal.z * center.z < 0.0f)
                {
                    std::swap(a1, a2);
                }

                AddTriangle(vertices, indices, offset, a0, a1, a2);
            }
        }

        return std::make_shared<Mesh>(vertices, indices);
    }


    std::shared_ptr<Mesh> MeshGenerator::Sphere(float radius, int segments, int rings)
    {
        std::vector<GLfloat> vertices;
        std::vector<GLuint> indices;

        if (segments < 3) segments = 3;
        if (rings < 3) rings = 3;

        for (int r = 0; r <= rings; ++r)
        {
            float phi = M_PI * (float)r / (float)rings;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            for (int s = 0; s <= segments; ++s)
            {
                float theta = 2.0f * M_PI * (float)s / (float)segments;
                float sinTheta = sin(theta);
                float cosTheta = cos(theta);

                float x = cosTheta * sinPhi;
                float y = cosPhi;
                float z = sinTheta * sinPhi;

                vertices.push_back(x * radius);
                vertices.push_back(y * radius);
                vertices.push_back(z * radius);

                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                float u = (float)s / (float)segments;
                float v = 1.0f - ((float)r / (float)rings);
                vertices.push_back(u);
                vertices.push_back(v);
            }
        }

        for (int r = 0; r < rings; ++r)
        {
            for (int s = 0; s < segments; ++s)
            {
                GLuint currentRingOffset = r * (segments + 1);
                GLuint nextRingOffset = (r + 1) * (segments + 1);

                GLuint topLeft = currentRingOffset + s;
                GLuint topRight = topLeft + 1;
                GLuint bottomLeft = nextRingOffset + s;
                GLuint bottomRight = bottomLeft + 1;

                indices.push_back(topLeft);
                indices.push_back(topRight);    
                indices.push_back(bottomLeft);

                indices.push_back(topRight);
                indices.push_back(bottomRight);
                indices.push_back(bottomLeft);
            }
        }

        return std::make_shared<Mesh>(vertices, indices);
    }


    std::shared_ptr<Mesh> MeshGenerator::Sprite(const std::shared_ptr<Texture2D> pTexture, const std::string initialSubTextureName)
    {
        const float h = 0.5f;
        const float z = 0.0f;

        const auto& subTexture = pTexture->getSubTexture(initialSubTextureName);

        std::vector<GLfloat> vertexCoordinate =
        {
            -h, -h,  z,            0.0f,  0.0f,  1.0f,      subTexture.leftBottomUV.x, subTexture.leftBottomUV.y,
             h, -h,  z,            0.0f,  0.0f,  1.0f,      subTexture.rightTopUV.x,   subTexture.leftBottomUV.y,
             h,  h,  z,            0.0f,  0.0f,  1.0f,      subTexture.rightTopUV.x,   subTexture.rightTopUV.y,
            -h,  h,  z,            0.0f,  0.0f,  1.0f,      subTexture.leftBottomUV.x, subTexture.rightTopUV.y,
        };


        std::vector<GLuint> indices = {
            0, 1, 2,
            2, 3, 0
        };

        return std::make_shared<Mesh>(vertexCoordinate, indices);
    }


    std::shared_ptr<Mesh> MeshGenerator::Text(const std::shared_ptr<Texture2D> pTexture, const std::string& text, float spacing = 0.0f)
    {
        std::vector<GLfloat> vertexCoordinates;
        std::vector<GLuint> indices;

        vertexCoordinates.reserve(text.size() * 32);
        indices.reserve(text.size() * 6);

        float cursorX = 0.0f;
        const float z = 0.0f;

        for (size_t i = 0; i < text.size(); ++i)
        {
            char c = text[i];

            std::string charName(1, c);

            //if (c == ' ') {
            //    cursorX += 0.5f; // Space
            //    continue;
            //}

            const auto& subTexture = pTexture->getSubTexture(charName);

            float charWidth = 0.5f;
            float charHeight = 0.5f;

            float left = cursorX;
            float right = cursorX + charWidth;
            float bottom = -charHeight / 2.0f;
            float top = charHeight / 2.0f;

            vertexCoordinates.insert(vertexCoordinates.end(), {
                left,  bottom, z,   0.0f, 0.0f, 1.0f,   subTexture.leftBottomUV.x, subTexture.leftBottomUV.y,
                right, bottom, z,   0.0f, 0.0f, 1.0f,   subTexture.rightTopUV.x,   subTexture.leftBottomUV.y,
                right, top,    z,   0.0f, 0.0f, 1.0f,   subTexture.rightTopUV.x,   subTexture.rightTopUV.y,
                left,  top,    z,   0.0f, 0.0f, 1.0f,   subTexture.leftBottomUV.x, subTexture.rightTopUV.y
                });

            GLuint offset = static_cast<GLuint>(i * 4);
            indices.insert(indices.end(), {
                offset + 0, offset + 1, offset + 2,
                offset + 2, offset + 3, offset + 0
                });

            cursorX += charWidth + spacing;
        }

        return std::make_shared<Mesh>(vertexCoordinates, indices);
    }
}