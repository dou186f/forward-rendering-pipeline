#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <cstring>
#include <string>
#include <vector>
#include <cmath>

#include "tinyxml2.h"
#include "Triangle.h"
#include "Helpers.h"
#include "Scene.h"
#include "Vec4WithColor.h"
#include "Vec4.h"

using namespace tinyxml2;
using namespace std;

/*
	Parses XML file
*/
Scene::Scene(const char *xmlPath)
{
	const char *str;
	XMLDocument xmlDoc;
	XMLElement *xmlElement;

	xmlDoc.LoadFile(xmlPath);

	XMLNode *rootNode = xmlDoc.FirstChild();

	// read background color
	xmlElement = rootNode->FirstChildElement("BackgroundColor");
	str = xmlElement->GetText();
	sscanf(str, "%lf %lf %lf", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);

	// read culling
	xmlElement = rootNode->FirstChildElement("Culling");
	if (xmlElement != NULL)
	{
		str = xmlElement->GetText();

		if (strcmp(str, "enabled") == 0)
		{
			this->cullingEnabled = true;
		}
		else
		{
			this->cullingEnabled = false;
		}
	}

	// read cameras
	xmlElement = rootNode->FirstChildElement("Cameras");
	XMLElement *camElement = xmlElement->FirstChildElement("Camera");
	XMLElement *camFieldElement;
	while (camElement != NULL)
	{
		Camera *camera = new Camera();

		camElement->QueryIntAttribute("id", &camera->cameraId);

		// read projection type
		str = camElement->Attribute("type");

		if (strcmp(str, "orthographic") == 0)
		{
			camera->projectionType = ORTOGRAPHIC_PROJECTION;
		}
		else
		{
			camera->projectionType = PERSPECTIVE_PROJECTION;
		}

		camFieldElement = camElement->FirstChildElement("Position");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->position.x, &camera->position.y, &camera->position.z);

		camFieldElement = camElement->FirstChildElement("Gaze");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->gaze.x, &camera->gaze.y, &camera->gaze.z);

		camFieldElement = camElement->FirstChildElement("Up");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->v.x, &camera->v.y, &camera->v.z);

		camera->gaze = normalizeVec3(camera->gaze);
		camera->u = crossProductVec3(camera->gaze, camera->v);
		camera->u = normalizeVec3(camera->u);

		camera->w = inverseVec3(camera->gaze);
		camera->v = crossProductVec3(camera->u, camera->gaze);
		camera->v = normalizeVec3(camera->v);

		camFieldElement = camElement->FirstChildElement("ImagePlane");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf %lf %lf %lf %d %d",
			   &camera->left, &camera->right, &camera->bottom, &camera->top,
			   &camera->near, &camera->far, &camera->horRes, &camera->verRes);

		camFieldElement = camElement->FirstChildElement("OutputName");
		str = camFieldElement->GetText();
		camera->outputFilename = string(str);

		this->cameras.push_back(camera);

		camElement = camElement->NextSiblingElement("Camera");
	}

	// read vertices
	xmlElement = rootNode->FirstChildElement("Vertices");
	XMLElement *vertexElement = xmlElement->FirstChildElement("Vertex");
	int vertexId = 1;

	while (vertexElement != NULL)
	{
		Vec3WithColor *vertex = new Vec3WithColor();
		vertex->vertexId = vertexId;

		str = vertexElement->Attribute("position");
		sscanf(str, "%lf %lf %lf", &vertex->x, &vertex->y, &vertex->z);

		str = vertexElement->Attribute("color");
		sscanf(str, "%lf %lf %lf", &vertex->color.r, &vertex->color.g, &vertex->color.b);

		this->vertices.push_back(vertex);

		vertexElement = vertexElement->NextSiblingElement("Vertex");

		vertexId++;
	}

	// read translations
	xmlElement = rootNode->FirstChildElement("Translations");
	XMLElement *translationElement = xmlElement->FirstChildElement("Translation");
	while (translationElement != NULL)
	{
		Translation *translation = new Translation();

		translationElement->QueryIntAttribute("id", &translation->translationId);

		str = translationElement->Attribute("value");
		sscanf(str, "%lf %lf %lf", &translation->tx, &translation->ty, &translation->tz);

		this->translations.push_back(translation);

		translationElement = translationElement->NextSiblingElement("Translation");
	}

	// read scalings
	xmlElement = rootNode->FirstChildElement("Scalings");
	XMLElement *scalingElement = xmlElement->FirstChildElement("Scaling");
	while (scalingElement != NULL)
	{
		Scaling *scaling = new Scaling();

		scalingElement->QueryIntAttribute("id", &scaling->scalingId);
		str = scalingElement->Attribute("value");
		sscanf(str, "%lf %lf %lf", &scaling->sx, &scaling->sy, &scaling->sz);

		this->scalings.push_back(scaling);

		scalingElement = scalingElement->NextSiblingElement("Scaling");
	}

	// read rotations
	xmlElement = rootNode->FirstChildElement("Rotations");
	XMLElement *rotationElement = xmlElement->FirstChildElement("Rotation");
	while (rotationElement != NULL)
	{
		Rotation *rotation = new Rotation();

		rotationElement->QueryIntAttribute("id", &rotation->rotationId);
		str = rotationElement->Attribute("value");
		sscanf(str, "%lf %lf %lf %lf", &rotation->angle, &rotation->ux, &rotation->uy, &rotation->uz);

		this->rotations.push_back(rotation);

		rotationElement = rotationElement->NextSiblingElement("Rotation");
	}

	// read meshes
	xmlElement = rootNode->FirstChildElement("Meshes");

	XMLElement *meshElement = xmlElement->FirstChildElement("Mesh");
	while (meshElement != NULL)
	{
		Mesh *mesh = new Mesh();

		meshElement->QueryIntAttribute("id", &mesh->meshId);

		// read mesh faces
		char *row;
		char *cloneStr;
		int vertexId1, vertexId2, vertexId3;
		str = meshElement->GetText();
		cloneStr = strdup(str);

		row = strtok(cloneStr, "\n");
		while (row != NULL)
		{
			int result = sscanf(row, "%d %d %d", &vertexId1, &vertexId2, &vertexId3);

			if (result != EOF)
			{
				Vec3WithColor v1 = *(this->vertices[vertexId1 - 1]);
				Vec3WithColor v2 = *(this->vertices[vertexId2 - 1]);
				Vec3WithColor v3 = *(this->vertices[vertexId3 - 1]);

				mesh->triangles.push_back(Triangle(v1, v2, v3));
			}
			row = strtok(NULL, "\n");
		}
		mesh->numberOfTriangles = mesh->triangles.size();
		this->meshes.push_back(mesh);

		meshElement = meshElement->NextSiblingElement("Mesh");
	}

	// read instances
	xmlElement = rootNode->FirstChildElement("Instances");

	XMLElement *instanceElement = xmlElement->FirstChildElement("Instance");
	while (instanceElement != NULL)
	{
		Instance *instance = new Instance();
		int meshId;

		instanceElement->QueryIntAttribute("id", &instance->instanceId);
		instanceElement->QueryIntAttribute("meshId", &meshId);

		instance->mesh = *(this->meshes[meshId - 1]);

		// read projection type
		str = instanceElement->Attribute("type");

		if (strcmp(str, "wireframe") == 0)
		{
			instance->instanceType = WIREFRAME_INSTANCE;
		}
		else
		{
			instance->instanceType = SOLID_INSTANCE;
		}

		// read instance transformations
		XMLElement *instanceTransformationsElement = instanceElement->FirstChildElement("Transformations");
		XMLElement *instanceTransformationElement = instanceTransformationsElement->FirstChildElement("Transformation");

		while (instanceTransformationElement != NULL)
		{
			char transformationType;
			int transformationId;

			str = instanceTransformationElement->GetText();
			sscanf(str, "%c %d", &transformationType, &transformationId);

			instance->transformationTypes.push_back(transformationType);
			instance->transformationIds.push_back(transformationId);

			instanceTransformationElement = instanceTransformationElement->NextSiblingElement("Transformation");
		}

		instance->numberOfTransformations = instance->transformationIds.size();
		this->instances.push_back(instance);

		instanceElement = instanceElement->NextSiblingElement("Instance");
	}
}

void Scene::assignColorToPixel(int i, int j, Color c)
{
    if (i < 0 || j < 0) return;
    if (i >= (int)this->image.size()) return;
    if (this->image.empty()) return;
    if (j >= (int)this->image[i].size()) return;

    this->image[i][j] = c;
}

/*
	Initializes image with background color
*/
void Scene::initializeImage(Camera *camera)
{
	if (this->image.empty())
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			vector<Color> rowOfColors;
			vector<double> rowOfDepths;

			for (int j = 0; j < camera->verRes; j++)
			{
				rowOfColors.push_back(this->backgroundColor);
				rowOfDepths.push_back(1.01);
			}

			this->image.push_back(rowOfColors);
			this->depth.push_back(rowOfDepths);
		}
	}
	else
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			for (int j = 0; j < camera->verRes; j++)
			{
				assignColorToPixel(i, j, this->backgroundColor);

				this->depth[i][j] = 1.01;
				this->depth[i][j] = 1.01;
				this->depth[i][j] = 1.01;
			}
		}
	}
}

/*
	If given value is less than 0, converts value to 0.
	If given value is more than 255, converts value to 255.
	Otherwise returns value itself.
*/
int Scene::makeBetweenZeroAnd255(double value)
{
	if (value >= 255.0)
		return 255;
	if (value <= 0.0)
		return 0;
	return (int)(value);
}

/*
	Writes contents of image (vector<vector<Color>>) into a PPM file.
*/
void Scene::writeImageToPPMFile(Camera *camera)
{
	ofstream fout;

	fout.open(camera->outputFilename.c_str());

	fout << "P3" << endl;
	fout << "# " << camera->outputFilename << endl;
	fout << camera->horRes << " " << camera->verRes << endl;
	fout << "255" << endl;

	for (int j = camera->verRes - 1; j >= 0; j--)
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			fout << makeBetweenZeroAnd255(this->image[i][j].r) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].g) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].b) << " ";
		}
		fout << endl;
	}
	fout.close();
}

// My Own Functions
void Scene::triangleRasterization(Vec4WithColor v0, Vec4WithColor v1, Vec4WithColor v2, int width, int height)
{
	int minX = std::max(0, (int)std::min(v0.x, std::min(v1.x, v2.x)));
	int minY = std::max(0, (int)std::min(v0.y, std::min(v1.y, v2.y)));

	int maxX = std::min(width - 1, (int)std::max(v0.x, std::max(v1.x, v2.x)));
	int maxY = std::min(height - 1, (int)std::max(v0.y, std::max(v1.y, v2.y)));

	for (int y = minY; y <= maxY; y++) {
		for (int x = minX; x <= maxX; x++) {
			double px = x + 0.5;
			double py = y + 0.5;

			double area = edgeFunction(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y);

			if (area == 0) {
				continue;
			}

			double w0 = edgeFunction(v1.x, v1.y, v2.x, v2.y, px, py) / area;
			double w1 = edgeFunction(v2.x, v2.y, v0.x, v0.y, px, py) / area;
			double w2 = edgeFunction(v0.x, v0.y, v1.x, v1.y, px, py) / area;

			if (w0 >= 0 && w1 >= 0 && w2 >= 0)
			{
				double z = w0 * v0.z + w1 * v1.z + w2 * v2.z;
				if (z < this-> depth[x][y])
				{
					this-> depth[x][y] = z;

					Color c;
					c.r = w0 * v0.color.r + w1 * v1.color.r + w2 * v2.color.r;
					c.g = w0 * v0.color.g + w1 * v1.color.g + w2 * v2.color.g;
					c.b = w0 * v0.color.b + w1 * v1.color.b + w2 * v2.color.b;

					this-> assignColorToPixel(x, y, c);
				}
			}
		}
	}	
}

void Scene::lineRasterization(Vec4WithColor v0, Vec4WithColor v1, int width, int height)
{
	int x0 = (int)std::lround(v0.x);
	int y0 = (int)std::lround(v0.y);
	int x1 = (int)std::lround(v1.x);
	int y1 = (int)std::lround(v1.y);

	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	int sx = (x0 < x1) ? 1 : -1;
	int sy = (y0 < y1) ? 1 : -1;
	int error = dx - dy;

	Color c0 = v0.color;
    Color c1 = v1.color;

	int steps = std::max(dx, dy);
	int count = 0;
	;
	while (true) {
		double t = (steps == 0) ? 0.0 : (double)count / steps;
		Color c;
        c.r = c0.r + (c1.r - c0.r) * t;
        c.g = c0.g + (c1.g - c0.g) * t;
        c.b = c0.b + (c1.b - c0.b) * t;
		assignColorToPixel(x0, y0, c);

		if (x0 == x1 && y0 == y1) {
			break;
		}

		int e2 = 2 * error;
		if (e2 > -dy) {
			error -= dy;
			x0 += sx;
		}
		if (e2 < dx) {
			error += dx;
			y0 += sy;
		}
		count++;
	}
}

bool Scene::clipLine(Vec4WithColor &v1, Vec4WithColor &v2) 
{
	double x1 = v1.x, y1 = v1.y, z1 = v1.z, w1 = v1.t;
    double x2 = v2.x, y2 = v2.y, z2 = v2.z, w2 = v2.t;

	Color c1 = v1.color;
	Color c2 = v2.color;

	int outcode1 = computeOutCode(x1, y1, z1, w1);
    int outcode2 = computeOutCode(x2, y2, z2, w2);
    bool accept = false;

	int iteration = 0;
    const int MAX_ITERATIONS = 10;

	while (true) {
		if (iteration > MAX_ITERATIONS) {
            return false;
        }
        iteration++;

        if ((outcode1 | outcode2) == 0) {
            accept = true;
            break;
        } 
		else if (outcode1 & outcode2) {
            break;
        } 
		else {
            double x, y, z, w;
            int outcodeOut = outcode1 ? outcode1 : outcode2;
			double t;

            if (outcodeOut & TOP) {
                t = (w1 - y1) / ((y2 - y1) - (w2 - w1));
                x = x1 + t * (x2 - x1);
                y = y1 + t * (y2 - y1);
                z = z1 + t * (z2 - z1);
                w = w1 + t * (w2 - w1);
            } 
			else if (outcodeOut & BOTTOM) { 
                t = (-w1 - y1) / ((y2 - y1) + (w2 - w1));
                x = x1 + t * (x2 - x1);
                y = y1 + t * (y2 - y1);
                z = z1 + t * (z2 - z1);
                w = w1 + t * (w2 - w1);
            } 
			else if (outcodeOut & RIGHT) { 
                t = (w1 - x1) / ((x2 - x1) - (w2 - w1));
                x = x1 + t * (x2 - x1);
                y = y1 + t * (y2 - y1);
                z = z1 + t * (z2 - z1);
                w = w1 + t * (w2 - w1);
            } 
			else if (outcodeOut & LEFT) {
                t = (-w1 - x1) / ((x2 - x1) + (w2 - w1));
                x = x1 + t * (x2 - x1);
                y = y1 + t * (y2 - y1);
                z = z1 + t * (z2 - z1);
                w = w1 + t * (w2 - w1);
            } 
			else if (outcodeOut & NEAR) {
                 t = (w1 - z1) / ((z2 - z1) - (w2 - w1));
                 x = x1 + t * (x2 - x1);
                 y = y1 + t * (y2 - y1);
                 z = z1 + t * (z2 - z1);
                 w = w1 + t * (w2 - w1);
            } 
			else if (outcodeOut & FAR) { 
                 t = (-w1 - z1) / ((z2 - z1) + (w2 - w1));
                 x = x1 + t * (x2 - x1);
                 y = y1 + t * (y2 - y1);
                 z = z1 + t * (z2 - z1);
                 w = w1 + t * (w2 - w1);
            }

			double r = c1.r + t * (c2.r - c1.r);
            double g = c1.g + t * (c2.g - c1.g);
            double b = c1.b + t * (c2.b - c1.b);

			if (outcodeOut == outcode1) {
                x1 = x; y1 = y; z1 = z; w1 = w;
                outcode1 = computeOutCode(x1, y1, z1, w1);
                v1.x = x; v1.y = y; v1.z = z; v1.t = w;
	
				v1.color.r = r;
				v1.color.g = g;
				v1.color.b = b;
            } 
			else {
                x2 = x; y2 = y; z2 = z; w2 = w;
                outcode2 = computeOutCode(x2, y2, z2, w2);
                v2.x = x; v2.y = y; v2.z = z; v2.t = w;

				v2.color.r = r;
                v2.color.g = g;
                v2.color.b = b;
            }
        }
    }
	return accept;
}
// My Own Functions

/*
	Transformations, clipping, culling, rasterization are done here.
*/
void Scene::forwardRenderingPipeline(Camera *camera)
{
	Matrix4 viewMatrix = getCameraTransformationMatrix(camera);
	Matrix4 projectionMatrix;

	if (camera -> projectionType == 1) {
		projectionMatrix = getPerspectiveMatrix(camera);
	}
	else {
		projectionMatrix = getOrthographicMatrix(camera);
	}

	Matrix4 viewProjectionMatrix = multiplyMatrixWithMatrix(projectionMatrix, viewMatrix);

	for (size_t i = 0; i < this-> instances.size(); i++) {
		Instance *instance = this-> instances[i];

		Matrix4 modelMatrix = getIdentityMatrix();
		for (size_t j = 0; j < instance -> transformationIds.size(); j++) {
			char type = instance -> transformationTypes[j];
			int id = instance -> transformationIds[j];

			Matrix4 transformationMatrix;

			if (type == 't') {
				Translation *t = this-> translations[id - 1];
				transformationMatrix = getTranslationMatrix(t -> tx, t -> ty, t -> tz);
			}
			else if (type == 'r') {
				Rotation *r = this-> rotations[id - 1];
				transformationMatrix = getRotationMatrix(r -> angle, r -> ux, r -> uy, r ->uz);
			}
			else if (type == 's') {
				Scaling *s = this-> scalings[id - 1];
				transformationMatrix = getScalingMatrix(s -> sx, s -> sy, s -> sz); 
			}

			modelMatrix = multiplyMatrixWithMatrix(transformationMatrix, modelMatrix);
		}

		Matrix4 finalMatrix = multiplyMatrixWithMatrix(viewProjectionMatrix, modelMatrix);

		for (size_t k = 0; k < instance -> mesh.triangles.size(); k++) {
			Triangle currentTriangle = instance -> mesh.triangles[k];
			
			if (this-> cullingEnabled) {
				Vec3WithColor v1 = currentTriangle.v1;
				Vec3WithColor v2 = currentTriangle.v2;
				Vec3WithColor v3 = currentTriangle.v3;

				Vec4WithColor v1_world = multiplyMatrixWithVec4WithColor(modelMatrix, Vec4WithColor(v1.x, v1.y, v1.z, 1.0, v1.color));
				Vec4WithColor v2_world = multiplyMatrixWithVec4WithColor(modelMatrix, Vec4WithColor(v2.x, v2.y, v2.z, 1.0, v2.color));
				Vec4WithColor v3_world = multiplyMatrixWithVec4WithColor(modelMatrix, Vec4WithColor(v3.x, v3.y, v3.z, 1.0, v3.color));

				Vec3 a = Vec3(v1_world.x, v1_world.y, v1_world.z);
				Vec3 b = Vec3(v2_world.x, v2_world.y, v2_world.z);
				Vec3 c = Vec3(v3_world.x, v3_world.y, v3_world.z);

				Vec3 edge1 = subtractVec3(b, a);
				Vec3 edge2 = subtractVec3(c, a);
				Vec3 normal = normalizeVec3(crossProductVec3(edge1, edge2));

				Vec3 viewVec = subtractVec3(a, camera->position);

				if (dotProductVec3(normal, viewVec) > 0) {
						continue;
				}
			}

			Vec4WithColor v1_4(currentTriangle.v1.x, currentTriangle.v1.y, currentTriangle.v1.z, 1.0, currentTriangle.v1.color);
			Vec4WithColor v2_4(currentTriangle.v2.x, currentTriangle.v2.y, currentTriangle.v2.z, 1.0, currentTriangle.v2.color);
			Vec4WithColor v3_4(currentTriangle.v3.x, currentTriangle.v3.y, currentTriangle.v3.z, 1.0, currentTriangle.v3.color);

			v1_4 = multiplyMatrixWithVec4WithColor(finalMatrix, v1_4);
			v2_4 = multiplyMatrixWithVec4WithColor(finalMatrix, v2_4);
			v3_4 = multiplyMatrixWithVec4WithColor(finalMatrix, v3_4);

			int width = camera -> horRes;
			int height = camera -> verRes;

			if (instance->instanceType == WIREFRAME_INSTANCE) {
                Vec4WithColor edge1_v1 = v1_4, edge1_v2 = v2_4;
                Vec4WithColor edge2_v1 = v2_4, edge2_v2 = v3_4;
                Vec4WithColor edge3_v1 = v3_4, edge3_v2 = v1_4;

                if (clipLine(edge1_v1, edge1_v2)) {
                    if(edge1_v1.t != 0) { edge1_v1.x /= edge1_v1.t; edge1_v1.y /= edge1_v1.t; edge1_v1.z /= edge1_v1.t; }
                    if(edge1_v2.t != 0) { edge1_v2.x /= edge1_v2.t; edge1_v2.y /= edge1_v2.t; edge1_v2.z /= edge1_v2.t; }
                    
                    edge1_v1.x = (edge1_v1.x + 1.0) * (width - 1) / 2.0;
                    edge1_v1.y = (edge1_v1.y + 1.0) * (height - 1) / 2.0;
                    edge1_v2.x = (edge1_v2.x + 1.0) * (width - 1) / 2.0;
                    edge1_v2.y = (edge1_v2.y + 1.0) * (height - 1) / 2.0;
                    
                    lineRasterization(edge1_v1, edge1_v2, width, height);
                }

                if (clipLine(edge2_v1, edge2_v2)) {
                    if(edge2_v1.t != 0) { edge2_v1.x /= edge2_v1.t; edge2_v1.y /= edge2_v1.t; edge2_v1.z /= edge2_v1.t; }
                    if(edge2_v2.t != 0) { edge2_v2.x /= edge2_v2.t; edge2_v2.y /= edge2_v2.t; edge2_v2.z /= edge2_v2.t; }
                    
                    edge2_v1.x = (edge2_v1.x + 1.0) * (width - 1) / 2.0;
                    edge2_v1.y = (edge2_v1.y + 1.0) * (height - 1) / 2.0;
                    edge2_v2.x = (edge2_v2.x + 1.0) * (width - 1) / 2.0;
                    edge2_v2.y = (edge2_v2.y + 1.0) * (height - 1) / 2.0;
                    
                    lineRasterization(edge2_v1, edge2_v2, width, height);
                }

                if (clipLine(edge3_v1, edge3_v2)) {
                    if(edge3_v1.t != 0) { edge3_v1.x /= edge3_v1.t; edge3_v1.y /= edge3_v1.t; edge3_v1.z /= edge3_v1.t; }
                    if(edge3_v2.t != 0) { edge3_v2.x /= edge3_v2.t; edge3_v2.y /= edge3_v2.t; edge3_v2.z /= edge3_v2.t; }
                    
                    edge3_v1.x = (edge3_v1.x + 1.0) * (width - 1) / 2.0;
                    edge3_v1.y = (edge3_v1.y + 1.0) * (height - 1) / 2.0;
                    edge3_v2.x = (edge3_v2.x + 1.0) * (width - 1) / 2.0;
                    edge3_v2.y = (edge3_v2.y + 1.0) * (height - 1) / 2.0;
                    
                    lineRasterization(edge3_v1, edge3_v2, width, height);
                }
            }
            else {
                if (v1_4.t <= 0 || v2_4.t <= 0 || v3_4.t <= 0) {
					continue; 
				}
                if (v1_4.t != 0) { 
					v1_4.x /= v1_4.t; v1_4.y /= v1_4.t; v1_4.z /= v1_4.t; v1_4.t = 1.0; 
				}
                if (v2_4.t != 0) { 
					v2_4.x /= v2_4.t; v2_4.y /= v2_4.t; v2_4.z /= v2_4.t; v2_4.t = 1.0;
				}
                if (v3_4.t != 0) { 
					v3_4.x /= v3_4.t; v3_4.y /= v3_4.t; v3_4.z /= v3_4.t; v3_4.t = 1.0; 
				}

                v1_4.x = (v1_4.x + 1.0) * (width - 1) / 2.0;
                v1_4.y = (v1_4.y + 1.0) * (height - 1) / 2.0;
                v2_4.x = (v2_4.x + 1.0) * (width - 1) / 2.0;
                v2_4.y = (v2_4.y + 1.0) * (height - 1) / 2.0;
                v3_4.x = (v3_4.x + 1.0) * (width - 1) / 2.0;
                v3_4.y = (v3_4.y + 1.0) * (height - 1) / 2.0;

                triangleRasterization(v1_4, v2_4, v3_4, width, height);
            }
		}
	}
}
