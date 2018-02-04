#pragma once

#include <vector>
#include <limits>
#include <memory>
#include <string>

#include <glm/glm.hpp>

//Axis-aligned bounding box
struct AABB
{
     glm::vec3 minPoint;
     glm::vec3 maxPoint;

     AABB()
     {
          //Initialize to +/- infinity
          float maxFloat = std::numeric_limits<float>::infinity();
          float minFloat = -std::numeric_limits<float>::infinity();
          
          minPoint = glm::vec3(maxFloat, maxFloat, maxFloat);
          maxPoint = glm::vec3(minFloat, minFloat, minFloat);
     }

	 void updateWithTriangle(const glm::vec4* triangleVertices)
	 {
		 updateWithVertex(triangleVertices[0]);
		 updateWithVertex(triangleVertices[1]);
		 updateWithVertex(triangleVertices[2]);
	 }

	 void updateWithVertex(const glm::vec3& vertex)
	 {
		 if (vertex.x < minPoint.x)
			 minPoint.x = vertex.x;

		 if (vertex.y < minPoint.y)
			 minPoint.y = vertex.y;

		 if (vertex.z < minPoint.z)
			 minPoint.z = vertex.z;

		 if (vertex.x > maxPoint.x)
			 maxPoint.x = vertex.x;

		 if (vertex.y > maxPoint.y)
			 maxPoint.y = vertex.y;

		 if (vertex.z > maxPoint.z)
			 maxPoint.z = vertex.z;
	 }

     void updateWithVertex(const glm::vec4& vertex)
     {
		 updateWithVertex(glm::vec3(vertex));
     }

	 void updateWithAABB(const AABB& bbox)
	 {
		 updateWithVertex(bbox.minPoint);
		 updateWithVertex(bbox.maxPoint);
	 }

	 AABB getTransformedAABB(const glm::mat4 matrix) const
	 {
		 AABB newBB;

		 glm::vec3 size = maxPoint - minPoint;

		 newBB.updateWithVertex(glm::vec3(matrix * glm::vec4(minPoint + glm::vec3(size.x, 0.0f, 0.0f), 1.0f)));
		 newBB.updateWithVertex(glm::vec3(matrix * glm::vec4(minPoint + glm::vec3(size.x, size.y, 0.0f), 1.0f)));
		 newBB.updateWithVertex(glm::vec3(matrix * glm::vec4(minPoint + glm::vec3(size.x, 0.0f, size.z), 1.0f)));
		 newBB.updateWithVertex(glm::vec3(matrix * glm::vec4(minPoint + glm::vec3(0.0f, size.y, 0.0f), 1.0f)));
		 newBB.updateWithVertex(glm::vec3(matrix * glm::vec4(minPoint + glm::vec3(0.0f, 0.0f, size.z), 1.0f)));
		 newBB.updateWithVertex(glm::vec3(matrix * glm::vec4(minPoint + glm::vec3(0.0f, size.y, size.z), 1.0f)));
		 newBB.updateWithVertex(glm::vec3(matrix * glm::vec4(minPoint, 1.0f)));
		 newBB.updateWithVertex(glm::vec3(matrix * glm::vec4(maxPoint, 1.0f)));

		 return newBB;
	 }
};

struct Material
{
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float shininess;
    
    //Index to a texture array
    //If negative - material has no texture
    int textureIndex;

    //Alignment to 128bit, in terms of size
    float aligmetVariableDoNotUse1, aligmetVariableDoNotUse2;
};

struct Mesh
{
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;

    AABB                   bbox;
    glm::mat4              modelMatrix;
    unsigned int           materialIndex;
	std::string			   m_name; // DR
};

//Texture
//Always R8G8B8A8 format
struct Texture
{
    Texture()
    {
        width = height = 0;
        data = nullptr;
    }

    unsigned int width;
    unsigned int height;

	std::string m_name;

    std::shared_ptr<unsigned char> data;
};

struct Scene
{
    //This is the data that interests You
    std::vector<Mesh>       meshes;

    std::vector<Material>   materials;
    std::vector<Texture>    textures;

    AABB                    bbox;
};