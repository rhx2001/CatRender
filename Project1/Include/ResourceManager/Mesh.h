#pragma once
#define TINYOBJLOADER_IMPLEMENTATION
#define GLM_ENABLE_EXPERIMENTAL
#include <string>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <array>
#include <vector>
#include "glm/gtx/hash.hpp"

class Mesh
{

public:
    std::string name;

    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;

        bool operator==(const Vertex& other) const {
            return pos == other.pos && color == other.color && texCoord == other.texCoord;
        }

    };



private:

    std::vector<Vertex> vertices{};
    std::vector<uint32_t> indices{};

    std::string path;

    uint32_t vertexBufferId = NULL;
    uint32_t indexBufferId = NULL;

public:

	Mesh(std::string const& path, std::string const& name):path(path), name(name){}

	~Mesh();

	void loadMesh(const std::string& path);

    void setBufferIds(uint32_t vbId, uint32_t ibId);

    void setVertexBufferId(uint32_t vbId);

    void setIndexBufferId(uint32_t ibId);

    uint32_t getVertexBufferId() const { return vertexBufferId; }

    uint32_t getIndexBufferId() const { return indexBufferId; }

    const std::vector<Vertex>& getVertices() const { return vertices; }

    const std::vector<uint32_t>& getIndices() const { return indices; }

    static VkVertexInputBindingDescription getBindingDescription();

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();




};



namespace std {
    template<> struct hash<Mesh::Vertex> {
        size_t operator()(Mesh::Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}