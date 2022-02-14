#pragma once

#include "polyscope/polyscope.h"

namespace curvscope{


/**
 * Computes mean curvature vectors for a given mesh.
 * The function fetches the registered polyscope object based on name
 *
 * Assumes triagulated mesh
 *
 * @param meshName name of mesh registered to Polyscope
 * @param curvVecs Curvature vectors container, one vector per vertex
 * @param meanCurv Mean curvature at each vertex
 * @param areaVoronoi Compute normalization using Voronoi-mixed area (based on 'Discrete Differential-Geometry Operatorsfor Triangulated 2-Manifolds, Meyer et al.)
 * 					  or standard barycentric subdivision of area (polyscope native area computation)
 */
void meanCurvature(std::string meshName, std::vector<glm::vec3>& curvVecs, std::vector<double>& meanCurv, bool areaVoronoi=true);

/**
 * Computes the cotangent
 *
 * @param theta angle
 * @returns the cotangent of theta
 */
float cotan(double theta);

}
