#include <iostream>
#include <cmath>
#include "Helpers.h"

/*
 * Calculate cross product of vec3 a, vec3 b and return resulting vec3.
 */
Vec3 crossProductVec3(Vec3 a, Vec3 b)
{
    return Vec3(a.y * b.z - b.y * a.z, b.x * a.z - a.x * b.z, a.x * b.y - b.x * a.y);
}

/*
 * Calculate dot product of vec3 a, vec3 b and return resulting value.
 */
double dotProductVec3(Vec3 a, Vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

/*
 * Find length (|v|) of vec3 v.
 */
double magnitudeOfVec3(Vec3 v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

/*
 * Normalize the vec3 to make it unit vec3.
 */
Vec3 normalizeVec3(Vec3 v)
{
    double d = magnitudeOfVec3(v);
    return Vec3(v.x / d, v.y / d, v.z / d);
}

/*
 * Return -v (inverse of vec3 v)
 */
Vec3 inverseVec3(Vec3 v)
{
    return Vec3(-v.x, -v.y, -v.z);
}

/*
 * Add vec3 a to vec3 b and return resulting vec3 (a+b).
 */
Vec3 addVec3(Vec3 a, Vec3 b)
{
    return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

/*
 * Subtract vec3 b from vec3 a and return resulting vec3 (a-b).
 */
Vec3 subtractVec3(Vec3 a, Vec3 b)
{
    return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

/*
 * Multiply each element of vec3 with scalar.
 */
Vec3 multiplyVec3WithScalar(Vec3 v, double c)
{
    return Vec3(v.x * c, v.y * c, v.z * c);
}

/*
 * Prints elements in a vec3. Can be used for debugging purposes.
 */
void printVec3(Vec3 v)
{
    std::cout << "(" << v.x << "," << v.y << "," << v.z << ")" << std::endl;
}

/*
 * Check whether vec3 a and vec3 b are equal.
 * In case of equality, returns 1.
 * Otherwise, returns 0.
 */
int areEqualVec3(Vec3 a, Vec3 b)
{

    /* if x difference, y difference and z difference is smaller than threshold, then they are equal */
    if ((ABS((a.x - b.x)) < EPSILON) && (ABS((a.y - b.y)) < EPSILON) && (ABS((a.z - b.z)) < EPSILON))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
 * Returns an identity matrix (values on the diagonal are 1, others are 0).
 */
Matrix4 getIdentityMatrix()
{
    Matrix4 result;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (i == j)
            {
                result.values[i][j] = 1.0;
            }
            else
            {
                result.values[i][j] = 0.0;
            }
        }
    }

    return result;
}

/*
 * Multiply matrices m1 (Matrix4) and m2 (Matrix4) and return the result matrix r (Matrix4).
 */
Matrix4 multiplyMatrixWithMatrix(Matrix4 m1, Matrix4 m2)
{
    Matrix4 result;
    double total;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            total = 0;
            for (int k = 0; k < 4; k++)
            {
                total += m1.values[i][k] * m2.values[k][j];
            }

            result.values[i][j] = total;
        }
    }

    return result;
}

/*
 * Multiply matrix m (Matrix4) with vector v (Vec4WithColor) and store the result in vector r (Vec4WithColor).
 */
Vec4WithColor multiplyMatrixWithVec4WithColor(Matrix4 m, Vec4WithColor v)
{
    double values[4];
    double total;

    for (int i = 0; i < 4; i++)
    {
        total = 0;
        for (int j = 0; j < 4; j++)
        {
            total += m.values[i][j] * v.getNthComponent(j);
        }
        values[i] = total;
    }

    return Vec4WithColor(values[0], values[1], values[2], values[3], v.color);
}

// My Own Helpers Below
Matrix4 getTranslationMatrix(double tx, double ty, double tz)
{
    Matrix4 result = getIdentityMatrix();

    result.values[0][3] = tx;
    result.values[1][3] = ty;
    result.values[2][3] = tz;

    return result;
}

Matrix4 getScalingMatrix(double sx, double sy, double sz)
{
    Matrix4 result = getIdentityMatrix();

    result.values[0][0] = sx;
    result.values[1][1] = sy;
    result.values[2][2] = sz;

    return result;
}

Matrix4 getRotationMatrix(double angle, double ux, double uy, double uz)
{
    Matrix4 result = getIdentityMatrix();

    double radian = angle * 3.14159265358979323846 / 180.0;
    double cos_theta = cos(radian);
    double sin_theta = sin(radian);

    Vec3 u(ux, uy, uz);
    u = normalizeVec3(u);

    result.values[0][0] = cos_theta + u.x * u.x * (1 - cos_theta);
    result.values[0][1] = u.x * u.y * (1 - cos_theta) - u.z * sin_theta;
    result.values[0][2] = u.x * u.z * (1 - cos_theta) + u.y * sin_theta;

    result.values[1][0] = u.y * u.x * (1 - cos_theta) + u.z * sin_theta;
    result.values[1][1] = cos_theta + u.y * u.y * (1 - cos_theta);
    result.values[1][2] = u.y * u.z * (1 - cos_theta) - u.x * sin_theta;

    result.values[2][0] = u.z * u.x * (1 - cos_theta) - u.y * sin_theta;
    result.values[2][1] = u.z * u.y * (1 - cos_theta) + u.x * sin_theta;
    result.values[2][2] = cos_theta + u.z * u.z * (1 - cos_theta);

    return result;
}

Matrix4 getCameraTransformationMatrix(Camera* cam) 
{
    Matrix4 result = getIdentityMatrix();

    result.values[0][0] = cam-> u.x;
    result.values[0][1] = cam-> u.y;
    result.values[0][2] = cam-> u.z;
    
    result.values[0][3] = -(cam-> u.x * cam-> position.x + cam-> u.y * cam-> position.y + cam-> u.z * cam-> position.z);
    
    result.values[1][0] = cam-> v.x;
    result.values[1][1] = cam-> v.y;
    result.values[1][2] = cam-> v.z;
    result.values[1][3] = -(cam-> v.x * cam-> position.x + cam-> v.y * cam-> position.y + cam-> v.z * cam-> position.z);

    result.values[2][0] = cam-> w.x;
    result.values[2][1] = cam-> w.y;
    result.values[2][2] = cam-> w.z;
    result.values[2][3] = -(cam-> w.x * cam-> position.x + cam-> w.y * cam-> position.y + cam-> w.z * cam-> position.z);

    return result;
}

Matrix4 getPerspectiveMatrix(Camera* cam)
{
    Matrix4 result = getIdentityMatrix();

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.values[i][j] = 0.0;
        }
    }

    result.values[0][0] = (2 * cam -> near) / (cam -> right - cam -> left);
    result.values[0][2] = (cam-> left + cam-> right) / (cam-> right - cam-> left);

    result.values[1][1] = (2 * cam -> near) / (cam -> top - cam -> bottom);
    result.values[1][2] = (cam-> top + cam-> bottom) / (cam-> top - cam-> bottom);

    result.values[2][2] = -(cam-> near + cam-> far) / (cam-> far - cam-> near);
    result.values[2][3] = -(2 * cam-> far * cam-> near) / (cam-> far - cam-> near);

    result.values[3][2] = -1.0;
    result.values[3][3] =  0.0;

    return result;
}

Matrix4 getOrthographicMatrix(Camera* cam)
{
    Matrix4 result = getIdentityMatrix();

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.values[i][j] = 0.0;
        }
    }

    result.values[0][0] = 2.0 / (cam -> right - cam -> left);
    result.values[0][3] = -(cam -> left + cam -> right) / (cam -> right - cam -> left);

    result.values[1][1] = 2.0 / (cam -> top - cam -> bottom);
    result.values[1][3] = -(cam -> top + cam -> bottom) / (cam -> top - cam -> bottom);

    result.values[2][2] = -2.0 / (cam -> far - cam -> near);
    result.values[2][3] = -(cam -> far + cam -> near) / (cam -> far - cam -> near);

    result.values[3][3] =  1.0;

    return result;
}

double edgeFunction(double x0, double y0, double x1, double y1, double x, double y)
{
    return (x - x0) * (y1 - y0) - (y - y0) * (x1 - x0);
}

int computeOutCode(double x, double y, double z, double w) {
    int code = INSIDE;
    
    if (x < -w) {
        code |= LEFT;
    }
    else if (x > w) {
        code |= RIGHT;
    }

    if (y < -w) {
        code |= BOTTOM;
    }
    else if (y > w) {
        code |= TOP;
    }

    if (z < -w) {
        code |= FAR;
    }
    else if (z > w) {
        code |= NEAR;
    }
    return code;
}