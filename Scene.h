#ifndef _SCENE_H_
#define _SCENE_H_
#include <vector>
#include "Vec3WithColor.h"
#include "Vec4WithColor.h"
#include "Vec4.h"
#include "Color.h"
#include "Rotation.h"
#include "Scaling.h"
#include "Translation.h"
#include "Camera.h"
#include "Mesh.h"
#include "Instance.h"
#include "Triangle.h"

class Scene
{
public:
	Color backgroundColor;
	bool cullingEnabled;

	std::vector<std::vector<Color> > image;
	std::vector<std::vector<double> > depth;
	std::vector<Camera *> cameras;
	std::vector<Vec3WithColor *> vertices;
	std::vector<Scaling *> scalings;
	std::vector<Rotation *> rotations;
	std::vector<Translation *> translations;
	std::vector<Mesh *> meshes;
	std::vector<Instance *> instances;

	Scene(const char *xmlPath);

	void assignColorToPixel(int i, int j, Color c);
	void initializeImage(Camera *camera);
	int makeBetweenZeroAnd255(double value);
	void writeImageToPPMFile(Camera *camera);
	// My Own Functions
	void triangleRasterization(Vec4WithColor v1, Vec4WithColor v2, Vec4WithColor v3, int width, int height);
	void lineRasterization(Vec4WithColor v0, Vec4WithColor v1, int width, int height);
	bool clipLine(Vec4WithColor &v1, Vec4WithColor &v2);
	// My Own Functions
	void forwardRenderingPipeline(Camera *camera);
};

#endif
