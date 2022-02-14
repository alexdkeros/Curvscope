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
 * @param vertexAreasVoronoi If areaVoronoi=true, populates the voronoi vertex areas, ortherwise remains untouched
 * @param areaVoronoi [default=True] Compute normalization using Voronoi-mixed area (based on 'Discrete Differential-Geometry Operatorsfor Triangulated 2-Manifolds, Meyer et al.)
 * 					  or standard barycentric subdivision of area (polyscope native area computation)
 */
void meanCurvature(std::string meshName, std::vector<glm::vec3>& curvVecs, std::vector<double>& meanCurv, std::vector<float>& vertexAreasVoronoi,bool areaVoronoi=true);


/**
 * Updates vertex positions according to the mean curvature flow
 * ! Might result to singularities on pinch points and boundaries (collapse of faces)!
 *
 * @param meshName name of mesh registered to Polyscope
 * @param coords Prior vertex coordinates to be updated (i.e. must be already initialized with the prior vertex coordinates)
 * @param factor Flow factor
 * @param voronoiAreas [default=True] Use Voronoi areas for mean curvature flow (otherwise use Barycentric)
 */
void meanCurvatureFlow(std::string meshName, std::vector<glm::vec3>& coords, float factor, bool voronoiAreas=true);



/**
 * Computes the cotangent
 *
 * @param theta angle
 * @returns the cotangent of theta
 */
float cotan(double theta);

}
