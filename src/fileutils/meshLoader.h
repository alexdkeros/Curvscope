#pragma once

#include <vector>
#include <array>

namespace curvscope{

/**
 * Checks whether the input string has the corresponding suffix
 * (borrowed from Polyscope's demo project)
 *
 * @param str Input string
 * @param suffix End-of-string suffix to check with
 *
 * @return bool True if suffix matches
 */
bool endsWith(const std::string& str, const std::string& suffix);


/**
 * Handles mesh file loading and registration to Polyscope
 * Automatically computes geometric data of mesh, and adds vertex normal option
 * Filetypes supported are .obj (handled by polyscope's loadPolygonSoup)
 *                         .ply (handled by polyscope's loadPolygonSoup)
 *                         .off (custom implementation)
 *
 * @param filename Input mesh file
 *
 * @return mesh name if successful, empty string otherwise
 */
std::string meshLoader(std::string filename);

/**
 * Loads mesh from .off file
 *
 * @param filename Input mesh file
 * @param vertexCoords Container for vertex coordinates
 * @param faceIndices Container for face indices
 *
 * @return
 *
 */
void loadPolygonSoup_OFF(std::string filename, std::vector<std::array<double,3>>& vertexCoords, std::vector<std::vector<size_t>>& faceIndices);


}





