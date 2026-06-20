#include <iostream>
#include <vector>
#include <string>

#define UFBX_IMPLEMENTATION
#include "ufbx.h" // Get this from github.com/ufbx/ufbx

struct Vertex {
    float position[3];
    float normal[3];
    float uv[2];
};

struct MeshData {
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

std::vector<MeshData> loadFBX(const std::string& filepath) {
    std::vector<MeshData> loadedMeshes;

    // 1. Load the FBX document
    ufbx_load_opts opts = { 0 }; // Use default options
    ufbx_error error;
    ufbx_scene* scene = ufbx_load_file(filepath.c_str(), &opts, &error);

    if (!scene) {
        std::cerr << "Failed to load FBX: " << error.description.data << std::endl;
        return loadedMeshes;
    }

    std::cout << "Successfully loaded FBX file! Nodes: " << scene->nodes.count << std::endl;

    // 2. Iterate through all the meshes in the scene
    for (size_t i = 0; i < scene->meshes.count; i++) {
        ufbx_mesh* u_mesh = scene->meshes.data[i];
        
        MeshData meshData;
        meshData.name = u_mesh->name.data ? u_mesh->name.data : "UnnamedMesh";

        // ufbx provides a safe triangulation iterator
        ufbx_mesh_vertices mv = ufbx_get_mesh_vertices(u_mesh);

        // 3. Extract vertex properties (Position, Normal, UV)
        for (size_t f = 0; f < u_mesh->faces.count; f++) {
            ufbx_face face = u_mesh->faces.data[f];
            
            for (uint32_t v = 0; v < face.length; v++) {
                uint32_t index = ufbx_get_face_vertex_index(u_mesh, face, v);
                ufbx_vertex_attrib vert = ufbx_get_vertex_attrib(u_mesh, mv, index);

                Vertex vertex;
                vertex.position[0] = vert.position.x;
                vertex.position[1] = vert.position.y;
                vertex.position[2] = vert.position.z;

                vertex.normal[0] = vert.normal.x;
                vertex.normal[1] = vert.normal.y;
                vertex.normal[2] = vert.normal.z;

                vertex.uv[0] = vert.uv.x;
                vertex.uv[1] = vert.uv.y;

                meshData.vertices.push_back(vertex);
                meshData.indices.push_back(meshData.indices.size());
            }
        }

        loadedMeshes.push_back(meshData);
    }

    // 4. Free allocated scene memory
    ufbx_free_scene(scene);

    return loadedMeshes;
}

int main() {
    std::string fbxFile = "model.fbx"; // Replace with your file path
    std::vector<MeshData> meshes = loadFBX(fbxFile);

    for (const auto& mesh : meshes) {
        std::cout << "Mesh Name: " << mesh.name << std::endl;
        std::cout << "Total Vertices: " << mesh.vertices.size() << std::endl;
    }

    return 0;
}
