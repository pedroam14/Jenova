#include "JenovaConsole.h"
<<<<<<< HEAD
#include <strstream>
#include <fstream>
#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#pragma region "Constants"
#define PI 3.14159265358979323846f
#pragma endregion

class Jenova : public JenovaConsole
{
public:
#pragma region "Public Constants"
	double theta;
#pragma endregion
#pragma region "Public Structures"
	struct Vector3D
	{
		double x = 0, y = 0, z = 0, w = 1;
	};
	struct Triangle
	{
		Vector3D points[3];

		wchar_t sym;
		short color;
	};
	struct Mesh
	{
		std::vector<Triangle> triangles;
		bool LoadObjectFromFile(std::string filename)
		{
			std::ifstream file(filename);
			if (!file.is_open())
			{
				return false;
			}
			//cache of verts
			std::vector<Vector3D> verts;
			while (!file.eof())
			{
				char line[128];
				file.getline(line, 128);
				std::strstream str;
				str << line;
				char junk;
				if (line[0] == 'v')
				{
					Vector3D v;
					str >> junk >> v.x >> v.y >> v.z;
					verts.push_back(v);
				}
				if (line[0] == 'f')
				{
					int f[3];
					str >> junk >> f[0] >> f[1] >> f[2];
					triangles.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
				}
			}
			return true;
		}
	};
	struct Matrix4x4
	{
		double matrix[4][4] = { 0 };
	};

#pragma endregion
	Jenova()
	{
		m_sAppName = L"3D Demo";
	}

	bool OnUserCreate() override
	{
		cube.triangles = {
			//south
			{0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f},

			//east
			{1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f},
			{1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f},

			//north
			{1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f},
			{1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},

			//west
			{0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},

			//top
			{0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
			{0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f},

			//bottom
			{1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f},
			{1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
		};
		//replace with whatever model you'd like my duderino
		tifaMesh.LoadObjectFromFile("E:/Pedro/Jenova/Jenova/Jenova/tifa2.obj");

		//projection matrix;
		matrixProjection = Matrix_MakeProjection(90.0, (double)ScreenHeight() / (double)ScreenWidth(), 0.1, 100.0);

		return true;
	}
	bool OnUserUpdate(double elapsedTime) override
	{
		if (GetKey(VK_UP).bHeld)
			vectorCamera.y += 8.0f * elapsedTime; // Travel Upwards

		if (GetKey(VK_DOWN).bHeld)
			vectorCamera.y -= 8.0f * elapsedTime; // Travel Downwards

		// Dont use these two in FPS mode, it is confusing :P
		if (GetKey(VK_LEFT).bHeld)
			vectorCamera.x -= 8.0f * elapsedTime; // Travel Along X-Axis

		if (GetKey(VK_RIGHT).bHeld)
			vectorCamera.x += 8.0f * elapsedTime; // Travel Along X-Axis
		///////

		Vector3D vForward = Vector_Mul(vectorLookDirection, 8.0f * elapsedTime);

		// Standard FPS Control scheme, but turn instead of strafe
		if (GetKey(L'W').bHeld)
			vectorCamera = Vector_Add(vectorCamera, vForward);

		if (GetKey(L'S').bHeld)
			vectorCamera = Vector_Sub(vectorCamera, vForward);

		if (GetKey(L'A').bHeld)
			yaw -= 2.0f * elapsedTime;

		if (GetKey(L'D').bHeld)
			yaw += 2.0f * elapsedTime;

		// Set up "World Tranmsform" though not updating theta
		// makes this a bit redundant
		Matrix4x4 matRotZ, matRotX;
		//theta += 1.0f * elapsedTime; // Uncomment to spin me right round baby right round
		matRotZ = Matrix_MakeRotationZ(theta * 0.5f);
		matRotX = Matrix_MakeRotationX(theta);

		Matrix4x4 matTrans;
		matTrans = Matrix_MakeTranslation(0.0f, 0.0f, 5.0f);

		Matrix4x4 matWorld;
		matWorld = Matrix_MakeIdentity();					  // Form World Matrix
		matWorld = Matrix_MultiplyMatrix(matRotZ, matRotX);   // Transform by rotation
		matWorld = Matrix_MultiplyMatrix(matWorld, matTrans); // Transform by translation

		// Create "Point At" Matrix for camera
		Vector3D vUp = { 0, 1, 0 };
		Vector3D vTarget = { 0, 0, 1 };
		Matrix4x4 matCameraRot = Matrix_MakeRotationY(yaw);
		vectorLookDirection = Matrix_MultiplyVector(matCameraRot, vTarget);
		vTarget = Vector_Add(vectorCamera, vectorLookDirection);
		Matrix4x4 matCamera = Matrix_PointAt(vectorCamera, vTarget, vUp);

		// Make view matrix from camera
		Matrix4x4 matView = Matrix_QuickInverse(matCamera);

		// Store triagles for rastering later
		std::vector<Triangle> vecTrianglesToRaster;

		// Draw Triangles
		for (auto tri : tifaMesh.triangles)
		{
			Triangle triProjected, triTransformed, triViewed;

			// World Matrix Transform
			triTransformed.points[0] = Matrix_MultiplyVector(matWorld, tri.points[0]);
			triTransformed.points[1] = Matrix_MultiplyVector(matWorld, tri.points[1]);
			triTransformed.points[2] = Matrix_MultiplyVector(matWorld, tri.points[2]);

			// Calculate triangle Normal
			Vector3D normal, line1, line2;

			// Get lines either side of triangle
			line1 = Vector_Sub(triTransformed.points[1], triTransformed.points[0]);
			line2 = Vector_Sub(triTransformed.points[2], triTransformed.points[0]);

			// Take cross product of lines to get normal to triangle surface
			normal = Vector_CrossProduct(line1, line2);

			// You normally need to normalise a normal!
			normal = Vector_Normalise(normal);

			// Get Ray from triangle to camera
			Vector3D vectorCameraRay = Vector_Sub(triTransformed.points[0], vectorCamera);

			// If ray is aligned with normal, then triangle is visible
			if (Vector_DotProduct(normal, vectorCameraRay) < 0.0f)
			{
				// Illumination
				Vector3D light_direction = { 0.0f, 1.0f, -1.0f };
				light_direction = Vector_Normalise(light_direction);

				// How "aligned" are light direction and triangle surface normal?
				double dp = max(0.1f, Vector_DotProduct(light_direction, normal));

				// Choose console colours as required (much easier with RGB)
				CHAR_INFO c = GetColour(dp);
				triTransformed.color = c.Attributes;
				triTransformed.sym = c.Char.UnicodeChar;

				// Convert World Space --> View Space
				triViewed.points[0] = Matrix_MultiplyVector(matView, triTransformed.points[0]);
				triViewed.points[1] = Matrix_MultiplyVector(matView, triTransformed.points[1]);
				triViewed.points[2] = Matrix_MultiplyVector(matView, triTransformed.points[2]);
				triViewed.sym = triTransformed.sym;
				triViewed.color = triTransformed.color;

				// Clip Viewed Triangle against near plane, this could form two additional
				// additional triangles.
				int nClippedTriangles = 0;
				Triangle clipped[2];
				nClippedTriangles = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

				// We may end up with multiple triangles form the clip, so project as
				// required
				for (int n = 0; n < nClippedTriangles; n++)
				{
					// Project triangles from 3D --> 2D
					triProjected.points[0] = Matrix_MultiplyVector(matrixProjection, clipped[n].points[0]);
					triProjected.points[1] = Matrix_MultiplyVector(matrixProjection, clipped[n].points[1]);
					triProjected.points[2] = Matrix_MultiplyVector(matrixProjection, clipped[n].points[2]);
					triProjected.color = clipped[n].color;
					triProjected.sym = clipped[n].sym;

					// Scale into view, we moved the normalising into cartesian space
					// out of the matrix.vector function from the previous videos, so
					// do this manually
					triProjected.points[0] = Vector_Div(triProjected.points[0], triProjected.points[0].w);
					triProjected.points[1] = Vector_Div(triProjected.points[1], triProjected.points[1].w);
					triProjected.points[2] = Vector_Div(triProjected.points[2], triProjected.points[2].w);

					// X/Y are inverted so put them back
					triProjected.points[0].x *= -1.0f;
					triProjected.points[1].x *= -1.0f;
					triProjected.points[2].x *= -1.0f;
					triProjected.points[0].y *= -1.0f;
					triProjected.points[1].y *= -1.0f;
					triProjected.points[2].y *= -1.0f;

					// Offset verts into visible normalised space
					Vector3D vOffsetView = { 1, 1, 0 };
					triProjected.points[0] = Vector_Add(triProjected.points[0], vOffsetView);
					triProjected.points[1] = Vector_Add(triProjected.points[1], vOffsetView);
					triProjected.points[2] = Vector_Add(triProjected.points[2], vOffsetView);
					triProjected.points[0].x *= 0.5f * (double)ScreenWidth();
					triProjected.points[0].y *= 0.5f * (double)ScreenHeight();
					triProjected.points[1].x *= 0.5f * (double)ScreenWidth();
					triProjected.points[1].y *= 0.5f * (double)ScreenHeight();
					triProjected.points[2].x *= 0.5f * (double)ScreenWidth();
					triProjected.points[2].y *= 0.5f * (double)ScreenHeight();

					// Store triangle for sorting
					vecTrianglesToRaster.push_back(triProjected);
				}
			}
		}

		// Sort triangles from back to front
		sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](Triangle &t1, Triangle &t2) {
			double z1 = (t1.points[0].z + t1.points[1].z + t1.points[2].z) / 3.0f;
			double z2 = (t2.points[0].z + t2.points[1].z + t2.points[2].z) / 3.0f;
			return z1 > z2;
		});

		// Clear Screen
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

		// Loop through all transformed, viewed, projected, and sorted triangles
		for (auto &triToRaster : vecTrianglesToRaster)
		{
			// Clip triangles against all four screen edges, this could yield
			// a bunch of triangles, so create a queue that we traverse to
			//  ensure we only test new triangles generated against planes
			Triangle clipped[2];
			std::list<Triangle> listTriangles;

			// Add initial triangle
			listTriangles.push_back(triToRaster);
			int nNewTriangles = 1;

			for (int p = 0; p < 4; p++)
			{
				int nTrisToAdd = 0;
				while (nNewTriangles > 0)
				{
					// Take triangle from front of queue
					Triangle test = listTriangles.front();
					listTriangles.pop_front();
					nNewTriangles--;

					// Clip it against a plane. We only need to test each
					// subsequent plane, against subsequent new triangles
					// as all triangles after a plane clip are guaranteed
					// to lie on the inside of the plane. I like how this
					// comment is almost completely and utterly justified
					switch (p)
					{
					case 0:
						nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]);
						break;
					case 1:
						nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, (double)ScreenHeight() - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]);
						break;
					case 2:
						nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]);
						break;
					case 3:
						nTrisToAdd = Triangle_ClipAgainstPlane({ (double)ScreenWidth() - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]);
						break;
					}

					// Clipping may yield a variable number of triangles, so
					// add these new ones to the back of the queue for subsequent
					// clipping against next planes
					for (int w = 0; w < nTrisToAdd; w++)
						listTriangles.push_back(clipped[w]);
				}
				nNewTriangles = listTriangles.size();
			}

			// Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
			for (auto &t : listTriangles)
			{
				FillTriangle(t.points[0].x, t.points[0].y, t.points[1].x, t.points[1].y, t.points[2].x, t.points[2].y, t.sym, t.color);
				//DrawTriangle(t.points[0].x, t.points[0].y, t.points[1].x, t.points[1].y, t.points[2].x, t.points[2].y, PIXEL_SOLID, FG_BLACK);
			}
		}

		return true;
	}

private:
#pragma region "Private variables"
	Mesh cube;
	Mesh tifaMesh;
	Matrix4x4 matrixProjection;

	Vector3D light_direction = { 0.0f, 0.0f, -1.0f };
	Vector3D vectorCamera;
	Vector3D vectorLookDirection; //direction the camera is looking at
	double yaw;

#pragma endregion

#pragma region "Private methods"
	//writes stuff directly to the output vector, without upsetting anything else
	void MultiplyMatrixVector(Vector3D &i, Vector3D &o, Matrix4x4 &m)
	{
		//now you get why the w is there all the time in unity big dumdum
		o.x = i.x * m.matrix[0][0] + i.y * m.matrix[1][0] + i.z * m.matrix[2][0] + m.matrix[3][0];
		o.y = i.x * m.matrix[0][1] + i.y * m.matrix[1][1] + i.z * m.matrix[2][1] + m.matrix[3][1];
		o.z = i.x * m.matrix[0][2] + i.y * m.matrix[1][2] + i.z * m.matrix[2][2] + m.matrix[3][2];
		double w = i.x * m.matrix[0][3] + i.y * m.matrix[1][3] + i.z * m.matrix[2][3] + m.matrix[3][3];

		if (w != 0.0f)
		{
			o.x /= w;
			o.y /= w;
			o.z /= w;
		}
	};
	Vector3D Matrix_MultiplyVector(Matrix4x4 &m, Vector3D &i)
	{
		Vector3D v;
		v.x = i.x * m.matrix[0][0] + i.y * m.matrix[1][0] + i.z * m.matrix[2][0] + i.w * m.matrix[3][0];
		v.y = i.x * m.matrix[0][1] + i.y * m.matrix[1][1] + i.z * m.matrix[2][1] + i.w * m.matrix[3][1];
		v.z = i.x * m.matrix[0][2] + i.y * m.matrix[1][2] + i.z * m.matrix[2][2] + i.w * m.matrix[3][2];
		v.w = i.x * m.matrix[0][3] + i.y * m.matrix[1][3] + i.z * m.matrix[2][3] + i.w * m.matrix[3][3];
		return v;
	}

	Matrix4x4 Matrix_MakeIdentity()
	{
		Matrix4x4 matrix;
		matrix.matrix[0][0] = 1.0f;
		matrix.matrix[1][1] = 1.0f;
		matrix.matrix[2][2] = 1.0f;
		matrix.matrix[3][3] = 1.0f;
		return matrix;
	}

	Matrix4x4 Matrix_MakeRotationX(double fAngleRad)
	{
		Matrix4x4 matrix;
		matrix.matrix[0][0] = 1.0f;
		matrix.matrix[1][1] = cosf(fAngleRad);
		matrix.matrix[1][2] = sinf(fAngleRad);
		matrix.matrix[2][1] = -sinf(fAngleRad);
		matrix.matrix[2][2] = cosf(fAngleRad);
		matrix.matrix[3][3] = 1.0f;
		return matrix;
	}

	Matrix4x4 Matrix_MakeRotationY(double fAngleRad)
	{
		Matrix4x4 matrix;
		matrix.matrix[0][0] = cosf(fAngleRad);
		matrix.matrix[0][2] = sinf(fAngleRad);
		matrix.matrix[2][0] = -sinf(fAngleRad);
		matrix.matrix[1][1] = 1.0f;
		matrix.matrix[2][2] = cosf(fAngleRad);
		matrix.matrix[3][3] = 1.0f;
		return matrix;
	}

	Matrix4x4 Matrix_MakeRotationZ(double fAngleRad)
	{
		Matrix4x4 matrix;
		matrix.matrix[0][0] = cosf(fAngleRad);
		matrix.matrix[0][1] = sinf(fAngleRad);
		matrix.matrix[1][0] = -sinf(fAngleRad);
		matrix.matrix[1][1] = cosf(fAngleRad);
		matrix.matrix[2][2] = 1.0f;
		matrix.matrix[3][3] = 1.0f;
		return matrix;
	}

	Matrix4x4 Matrix_MakeTranslation(double x, double y, double z)
	{
		Matrix4x4 matrix;
		matrix.matrix[0][0] = 1.0f;
		matrix.matrix[1][1] = 1.0f;
		matrix.matrix[2][2] = 1.0f;
		matrix.matrix[3][3] = 1.0f;
		matrix.matrix[3][0] = x;
		matrix.matrix[3][1] = y;
		matrix.matrix[3][2] = z;
		return matrix;
	}

	Matrix4x4 Matrix_MakeProjection(double fFovDegrees, double fAspectRatio, double fNear, double fFar)
	{
		double fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * 3.14159f);
		Matrix4x4 matrix;
		matrix.matrix[0][0] = fAspectRatio * fFovRad;
		matrix.matrix[1][1] = fFovRad;
		matrix.matrix[2][2] = fFar / (fFar - fNear);
		matrix.matrix[3][2] = (-fFar * fNear) / (fFar - fNear);
		matrix.matrix[2][3] = 1.0f;
		matrix.matrix[3][3] = 0.0f;
		return matrix;
	}

	Matrix4x4 Matrix_MultiplyMatrix(Matrix4x4 &m1, Matrix4x4 &m2)
	{
		Matrix4x4 matrix;
		for (int c = 0; c < 4; c++)
		{
			for (int r = 0; r < 4; r++)
			{
				matrix.matrix[r][c] = m1.matrix[r][0] * m2.matrix[0][c] + m1.matrix[r][1] * m2.matrix[1][c] + m1.matrix[r][2] * m2.matrix[2][c] + m1.matrix[r][3] * m2.matrix[3][c];
			}
		}
		return matrix;
	}

	Matrix4x4 Matrix_PointAt(Vector3D &pos, Vector3D &target, Vector3D &up)
	{
		// Calculate new forward direction
		Vector3D newForward = Vector_Sub(target, pos);
		newForward = Vector_Normalise(newForward);

		// Calculate new Up direction
		Vector3D a = Vector_Mul(newForward, Vector_DotProduct(up, newForward));
		Vector3D newUp = Vector_Sub(up, a);
		newUp = Vector_Normalise(newUp);

		// New Right direction is easy, its just cross product
		Vector3D newRight = Vector_CrossProduct(newUp, newForward);

		// Construct Dimensioning and Translation Matrix
		Matrix4x4 matrix;
		matrix.matrix[0][0] = newRight.x;
		matrix.matrix[0][1] = newRight.y;
		matrix.matrix[0][2] = newRight.z;
		matrix.matrix[0][3] = 0.0f;
		matrix.matrix[1][0] = newUp.x;
		matrix.matrix[1][1] = newUp.y;
		matrix.matrix[1][2] = newUp.z;
		matrix.matrix[1][3] = 0.0f;
		matrix.matrix[2][0] = newForward.x;
		matrix.matrix[2][1] = newForward.y;
		matrix.matrix[2][2] = newForward.z;
		matrix.matrix[2][3] = 0.0f;
		matrix.matrix[3][0] = pos.x;
		matrix.matrix[3][1] = pos.y;
		matrix.matrix[3][2] = pos.z;
		matrix.matrix[3][3] = 1.0f;
		return matrix;
	}

	Matrix4x4 Matrix_QuickInverse(Matrix4x4 &m) // Only for Rotation/Translation Matrices
	{
		Matrix4x4 matrix;
		matrix.matrix[0][0] = m.matrix[0][0];
		matrix.matrix[0][1] = m.matrix[1][0];
		matrix.matrix[0][2] = m.matrix[2][0];
		matrix.matrix[0][3] = 0.0f;
		matrix.matrix[1][0] = m.matrix[0][1];
		matrix.matrix[1][1] = m.matrix[1][1];
		matrix.matrix[1][2] = m.matrix[2][1];
		matrix.matrix[1][3] = 0.0f;
		matrix.matrix[2][0] = m.matrix[0][2];
		matrix.matrix[2][1] = m.matrix[1][2];
		matrix.matrix[2][2] = m.matrix[2][2];
		matrix.matrix[2][3] = 0.0f;
		matrix.matrix[3][0] = -(m.matrix[3][0] * matrix.matrix[0][0] + m.matrix[3][1] * matrix.matrix[1][0] + m.matrix[3][2] * matrix.matrix[2][0]);
		matrix.matrix[3][1] = -(m.matrix[3][0] * matrix.matrix[0][1] + m.matrix[3][1] * matrix.matrix[1][1] + m.matrix[3][2] * matrix.matrix[2][1]);
		matrix.matrix[3][2] = -(m.matrix[3][0] * matrix.matrix[0][2] + m.matrix[3][1] * matrix.matrix[1][2] + m.matrix[3][2] * matrix.matrix[2][2]);
		matrix.matrix[3][3] = 1.0f;
		return matrix;
	}

	Vector3D Vector_Add(Vector3D &v1, Vector3D &v2)
	{
		return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
	}

	Vector3D Vector_Sub(Vector3D &v1, Vector3D &v2)
	{
		return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
	}

	Vector3D Vector_Mul(Vector3D &v1, double k)
	{
		return { v1.x * k, v1.y * k, v1.z * k };
	}

	Vector3D Vector_Div(Vector3D &v1, double k)
	{
		return { v1.x / k, v1.y / k, v1.z / k };
	}

	double Vector_DotProduct(Vector3D &v1, Vector3D &v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	double Vector_Length(Vector3D &v)
	{
		return sqrtf(Vector_DotProduct(v, v));
	}

	Vector3D Vector_Normalise(Vector3D &v)
	{
		double l = Vector_Length(v);
		return { v.x / l, v.y / l, v.z / l };
	}

	Vector3D Vector_CrossProduct(Vector3D &v1, Vector3D &v2)
	{
		Vector3D v;
		v.x = v1.y * v2.z - v1.z * v2.y;
		v.y = v1.z * v2.x - v1.x * v2.z;
		v.z = v1.x * v2.y - v1.y * v2.x;
		return v;
	}

	Vector3D Vector_IntersectPlane(Vector3D &plane_p, Vector3D &plane_n, Vector3D &lineStart, Vector3D &lineEnd)
	{
		plane_n = Vector_Normalise(plane_n);
		double plane_d = -Vector_DotProduct(plane_n, plane_p);
		double ad = Vector_DotProduct(lineStart, plane_n);
		double bd = Vector_DotProduct(lineEnd, plane_n);
		double t = (-plane_d - ad) / (bd - ad);
		Vector3D lineStartToEnd = Vector_Sub(lineEnd, lineStart);
		Vector3D lineToIntersect = Vector_Mul(lineStartToEnd, t);
		return Vector_Add(lineStart, lineToIntersect);
	}

	int Triangle_ClipAgainstPlane(Vector3D plane_p, Vector3D plane_n, Triangle &in_tri, Triangle &out_tri1, Triangle &out_tri2)
	{
		// Make sure plane normal is indeed normal
		plane_n = Vector_Normalise(plane_n);

		// Return signed shortest distance from point to plane, plane normal must be normalised
		auto dist = [&](Vector3D &p) {
			Vector3D n = Vector_Normalise(p);
			return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - Vector_DotProduct(plane_n, plane_p));
		};

		//create two temporary storage arrays to classify points either side of plane if distance sign is positive, point lies on "inside" of plane
		Vector3D *inside_points[3];
		int nInsidePointCount = 0;
		Vector3D *outside_points[3];
		int nOutsidePointCount = 0;

		//get signed distance of each point in Triangle to plane
		double d0 = dist(in_tri.points[0]);
		double d1 = dist(in_tri.points[1]);
		double d2 = dist(in_tri.points[2]);

		if (d0 >= 0)
		{
			inside_points[nInsidePointCount++] = &in_tri.points[0];
		}
		else
		{
			outside_points[nOutsidePointCount++] = &in_tri.points[0];
		}
		if (d1 >= 0)
		{
			inside_points[nInsidePointCount++] = &in_tri.points[1];
		}
		else
		{
			outside_points[nOutsidePointCount++] = &in_tri.points[1];
		}
		if (d2 >= 0)
		{
			inside_points[nInsidePointCount++] = &in_tri.points[2];
		}
		else
		{
			outside_points[nOutsidePointCount++] = &in_tri.points[2];
		}

		//now classify Triangle points, and break the input Triangle into smaller output triangles if required with four posssible outcomes

		if (nInsidePointCount == 0)
		{
			//all points lie on the outside of plane, so clip whole Triangle it ceases to exist

			return 0; //no returned triangles are valid
		}

		if (nInsidePointCount == 3)
		{
			//all points lie on the inside of plane, so do nothing and allow the Triangle to simply pass through
			out_tri1 = in_tri;

			return 1; //just the one returned original Triangle is valid
		}

		if (nInsidePointCount == 1 && nOutsidePointCount == 2)
		{
			//triangle should be clipped. As two points lie outside the plane, the Triangle simply becomes a smaller Triangle

			//copy appearance info to new Triangle
			out_tri1.color = in_tri.color;
			out_tri1.sym = in_tri.sym;

			//the inside point is valid, so keep that...
			out_tri1.points[0] = *inside_points[0];

			// but the two new points are at the locations where the original sides of the Triangle (lines) intersect with the plane
			out_tri1.points[1] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
			out_tri1.points[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

			return 1; // Return the newly formed single Triangle
		}

		if (nInsidePointCount == 2 && nOutsidePointCount == 1)
		{
			//triangle should be clipped. As two points lie inside the plane, the clipped Triangle becomes a "quad", but fortunately, we can represent a quad with two new triangles

			//copy appearance info to new triangles
			out_tri1.color = in_tri.color;
			out_tri1.sym = in_tri.sym;

			out_tri2.color = in_tri.color;
			out_tri2.sym = in_tri.sym;

			//the first Triangle consists of the two inside points and a new point determined by the location where one side of the Triangle intersects with the plane
			out_tri1.points[0] = *inside_points[0];
			out_tri1.points[1] = *inside_points[1];
			out_tri1.points[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);

			//the second Triangle is composed of one of he inside points, a new point determined by the intersection of the other side of the triangle and the plane, and the newly created point above
			out_tri2.points[0] = *inside_points[1];
			out_tri2.points[1] = out_tri1.points[2];
			out_tri2.points[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0]);

			return 2; //return two newly formed triangles which form a quad
		}
	}
#pragma region "Public Stuff to be sent over to OpenGL"
public:
	Mesh publicMesh;
public:
	void SetMesh() {
		publicMesh.LoadObjectFromFile("tifa2.obj");
	}
#pragma endregion

	CHAR_INFO GetColour(double lum)
	{
		short bg_col, fg_col;
		wchar_t sym;
		int pixel_bw = (int)(13.0f * lum);
		switch (pixel_bw)
		{
		case 0:
			bg_col = BG_BLACK;
			fg_col = FG_BLACK;
			sym = PIXEL_SOLID;
			break;

		case 1:
			bg_col = BG_BLACK;
			fg_col = FG_DARK_GREY;
			sym = PIXEL_QUARTER;
			break;
		case 2:
			bg_col = BG_BLACK;
			fg_col = FG_DARK_GREY;
			sym = PIXEL_HALF;
			break;
		case 3:
			bg_col = BG_BLACK;
			fg_col = FG_DARK_GREY;
			sym = PIXEL_THREEQUARTERS;
			break;
		case 4:
			bg_col = BG_BLACK;
			fg_col = FG_DARK_GREY;
			sym = PIXEL_SOLID;
			break;

		case 5:
			bg_col = BG_DARK_GREY;
			fg_col = FG_GREY;
			sym = PIXEL_QUARTER;
			break;
		case 6:
			bg_col = BG_DARK_GREY;
			fg_col = FG_GREY;
			sym = PIXEL_HALF;
			break;
		case 7:
			bg_col = BG_DARK_GREY;
			fg_col = FG_GREY;
			sym = PIXEL_THREEQUARTERS;
			break;
		case 8:
			bg_col = BG_DARK_GREY;
			fg_col = FG_GREY;
			sym = PIXEL_SOLID;
			break;

		case 9:
			bg_col = BG_GREY;
			fg_col = FG_WHITE;
			sym = PIXEL_QUARTER;
			break;
		case 10:
			bg_col = BG_GREY;
			fg_col = FG_WHITE;
			sym = PIXEL_HALF;
			break;
		case 11:
			bg_col = BG_GREY;
			fg_col = FG_WHITE;
			sym = PIXEL_THREEQUARTERS;
			break;
		case 12:
			bg_col = BG_GREY;
			fg_col = FG_WHITE;
			sym = PIXEL_SOLID;
			break;
		default:
			bg_col = BG_BLACK;
			fg_col = FG_BLACK;
			sym = PIXEL_SOLID;
		}

		CHAR_INFO c;
		c.Attributes = bg_col | fg_col;
		c.Char.UnicodeChar = sym;
		return c;
	}
};
int main(void)
{

	Jenova demo;
	///*
	if (demo.ConstructConsole(256, 240, 4, 4))
	{
		demo.Start();
	}

	//*/

	//creates the glfw window
	GLFWwindow *window;


	//initialize the library
	if (!glfwInit())
	{
		return -1;
	}


	//create a windowed mode window and its OpenGL context

	window = glfwCreateWindow(640, 480, "Jenova Engine", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	//make the window's context current
	glfwMakeContextCurrent(window);

	//we can only use glew after we have a valid opengl context 
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Something went wrong somewhere :/";
	}


	/*
	unsigned int buffer;
	if (demo.publicMesh.LoadObjectFromFile("tifa2.obj"))
	{
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(demo.publicMesh), 3 * demo.publicMesh.triangles.size * sizeof(double), )
	}
	*/
	//loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		//render here
		glClear(GL_COLOR_BUFFER_BIT);

		glBegin(GL_TRIANGLES);

		glEnd();

		//swap front and back buffers
		glfwSwapBuffers(window);

		//poll for and process events
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
=======
#pragma region "Constants"
#define PI 3.14159265358979323846f
#pragma endregion

class Jenova : public JenovaConsole
{
	struct Vector3D
	{
		double x = 0, y = 0, z = 0;
	};
	struct Triangle
	{
		Vector3D points[3];

		wchar_t sym;
		short color;
	};
	struct Mesh
	{
		std::vector<Triangle> triangles;
	};
	struct Matrix4x4
	{
		double matrix[4][4] = { 0 };
	};

public:
#pragma region "Public Constants"
	double theta;
#pragma endregion
	Jenova()
	{
		m_sAppName = L"3D Demo";
	}

	bool OnUserCreate() override
	{
		cube.triangles = {
			//south
			{0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f},

			//east
			{1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f},
			{1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f},

			//north
			{1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f},
			{1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},

			//west
			{0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},

			//top
			{0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
			{0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f},

			//bottom
			{1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f},
			{1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
		};

		//projection matrix;
		double nearPlane = 0.1f;
		double farPlane = 1000.0f;
		double fieldOfView = 90.0f;
		double aspectRatio = (double)ScreenHeight() / (double)ScreenWidth();
		double fieldOfViewRad = 1.0f / tanf(fieldOfView * 0.5 / 180.0f * PI);

		matrixProjection.matrix[0][0] = aspectRatio * fieldOfViewRad;
		matrixProjection.matrix[1][1] = fieldOfViewRad;
		matrixProjection.matrix[2][2] = farPlane / (farPlane - nearPlane);
		matrixProjection.matrix[3][2] = (-farPlane * nearPlane) / (farPlane - nearPlane);
		matrixProjection.matrix[2][3] = 1.0f;
		matrixProjection.matrix[3][3] = 0.0f;

		return true;
	}
	bool OnUserUpdate(double elapsedTime) override
	{
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

		Matrix4x4 matrixRotationZ, matrixRotationX;
		Vector3D normal, line1, line2;
		theta += 1.0 * elapsedTime;

		//z rotation
		matrixRotationZ.matrix[0][0] = cos(theta);
		matrixRotationZ.matrix[0][1] = sin(theta);
		matrixRotationZ.matrix[1][0] = -sin(theta);
		matrixRotationZ.matrix[1][1] = cos(theta);
		matrixRotationZ.matrix[2][2] = 1;
		matrixRotationZ.matrix[3][3] = 1;

		//x rotation
		matrixRotationX.matrix[0][0] = 1;
		matrixRotationX.matrix[1][1] = cos(theta * 0.5);
		matrixRotationX.matrix[1][2] = sin(theta * 0.5);
		matrixRotationX.matrix[2][1] = -sin(theta * 0.5);
		matrixRotationX.matrix[2][2] = cos(theta * 0.5);
		matrixRotationX.matrix[3][3] = 1;

		//maybe do this in parallel if it's worth it, otherwise doing it explicitly has less overhead than a simple for loop and actually *is* better for very short operations
		//draw triangles
#pragma omp parallel for
		for (auto triangle : cube.triangles)
		{
			Triangle projectedTriangle, translatedTriangle, triangleRotatedZ, triangleRotatedZX;

			//z axis rotation
			MultiplyMatrixVector(triangle.points[0], triangleRotatedZ.points[0], matrixRotationZ);
			MultiplyMatrixVector(triangle.points[1], triangleRotatedZ.points[1], matrixRotationZ);
			MultiplyMatrixVector(triangle.points[2], triangleRotatedZ.points[2], matrixRotationZ);
			//x axis rotation
			MultiplyMatrixVector(triangleRotatedZ.points[0], triangleRotatedZX.points[0], matrixRotationX);
			MultiplyMatrixVector(triangleRotatedZ.points[1], triangleRotatedZX.points[1], matrixRotationX);
			MultiplyMatrixVector(triangleRotatedZ.points[2], triangleRotatedZX.points[2], matrixRotationX);

			//offset to the screen
			translatedTriangle = triangleRotatedZX;
			translatedTriangle.points[0].z += 3.0f;
			translatedTriangle.points[1].z += 3.0f;
			translatedTriangle.points[2].z += 3.0f;

			//normal calculation

			line1.x = translatedTriangle.points[1].x - translatedTriangle.points[0].x;
			line1.y = translatedTriangle.points[1].y - translatedTriangle.points[0].y;
			line1.z = translatedTriangle.points[1].z - translatedTriangle.points[0].z;

			line2.x = translatedTriangle.points[2].x - translatedTriangle.points[0].x;
			line2.y = translatedTriangle.points[2].y - translatedTriangle.points[0].y;
			line2.z = translatedTriangle.points[2].z - translatedTriangle.points[0].z;

			normal.x = line1.y * line2.z - line1.z * line2.y;
			normal.y = line1.z * line2.x - line1.x * line2.z;
			normal.z = line1.x * line2.y - line1.y * line2.x;

			normal.x = line1.y * line2.z - line1.z * line2.y;
			normal.y = line1.z * line2.x - line1.x * line2.z;
			normal.z = line1.x * line2.y - line1.y * line2.x;

			// It's normally normal to normalise the normal
			double l = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
			normal.x /= l;
			normal.y /= l;
			normal.z /= l;

			//we only want to do the projection if we can see it, the normal z value being less than zero which means it's facing the screen
			//if (normal.z < 0)
			//actually!!!! we want to calculate if it's visible or not in respect to the camera
			if (normal.x * (translatedTriangle.points[0].x - vectorCamera.x) +
				normal.y * (translatedTriangle.points[0].y - vectorCamera.y) +
				normal.z * (translatedTriangle.points[0].z - vectorCamera.z) < 0.0)
			{

				Vector3D light_direction = { 0.0f, 0.0f, -1.0f };
				//lighting
				double l = l = sqrtf(light_direction.x*light_direction.x + light_direction.y*light_direction.y + light_direction.z*light_direction.z);
				light_direction.x /= l; light_direction.y /= l; light_direction.z /= l;
				double  dp = normal.x * light_direction.x + normal.y * light_direction.y + normal.z * light_direction.z;


				//very console specific stuff, change to a proper render API later
				CHAR_INFO c = GetColour(dp);
				translatedTriangle.color = c.Attributes;
				translatedTriangle.sym = c.Char.UnicodeChar;

				//3D -> 2D projection

				MultiplyMatrixVector(translatedTriangle.points[0], projectedTriangle.points[0], matrixProjection);
				MultiplyMatrixVector(translatedTriangle.points[1], projectedTriangle.points[1], matrixProjection);
				MultiplyMatrixVector(translatedTriangle.points[2], projectedTriangle.points[2], matrixProjection);
				projectedTriangle.color = translatedTriangle.color;
				projectedTriangle.sym = translatedTriangle.sym;

				//scaling the triangles into view
				projectedTriangle.points[0].x += 1.0;
				projectedTriangle.points[0].y += 1.0;
				projectedTriangle.points[1].x += 1.0;
				projectedTriangle.points[1].y += 1.0;
				projectedTriangle.points[2].x += 1.0;
				projectedTriangle.points[2].y += 1.0;

				projectedTriangle.points[0].x *= .5 * (double)ScreenWidth();
				projectedTriangle.points[0].y *= .5 * (double)ScreenHeight();
				projectedTriangle.points[1].x *= .5 * (double)ScreenWidth();
				projectedTriangle.points[1].y *= .5 * (double)ScreenHeight();
				projectedTriangle.points[2].x *= .5 * (double)ScreenWidth();
				projectedTriangle.points[2].y *= .5 * (double)ScreenHeight();

				//wireframe mode:
				/*
				DrawTriangle(projectedTriangle.points[0].x, projectedTriangle.points[0].y,
					projectedTriangle.points[1].x, projectedTriangle.points[1].y,
					projectedTriangle.points[2].x, projectedTriangle.points[2].y, projectedTriangle.sym, projectedTriangle.color);
				*/
				//solid mode:
				FillTriangle(projectedTriangle.points[0].x, projectedTriangle.points[0].y,
					projectedTriangle.points[1].x, projectedTriangle.points[1].y,
					projectedTriangle.points[2].x, projectedTriangle.points[2].y, projectedTriangle.sym, projectedTriangle.color);
			}
		}

		return true;
	}

private:
#pragma region "Private variables"
	Mesh cube;
	Matrix4x4 matrixProjection;

	Vector3D vectorCamera;

#pragma endregion

	//writes stuff directly to the output vector, without upsetting anything else
	void MultiplyMatrixVector(Vector3D &i, Vector3D &o, Matrix4x4 &m)
	{
		//now you get why the w is there all the time in unity big dumdum
		o.x = i.x * m.matrix[0][0] + i.y * m.matrix[1][0] + i.z * m.matrix[2][0] + m.matrix[3][0];
		o.y = i.x * m.matrix[0][1] + i.y * m.matrix[1][1] + i.z * m.matrix[2][1] + m.matrix[3][1];
		o.z = i.x * m.matrix[0][2] + i.y * m.matrix[1][2] + i.z * m.matrix[2][2] + m.matrix[3][2];
		double w = i.x * m.matrix[0][3] + i.y * m.matrix[1][3] + i.z * m.matrix[2][3] + m.matrix[3][3];

		if (w != 0.0f)
		{
			o.x /= w;
			o.y /= w;
			o.z /= w;
		}
	};
	CHAR_INFO GetColour(double lum)
	{
		short bg_col, fg_col;
		wchar_t sym;
		int pixel_bw = (int)(13.0f*lum);
		switch (pixel_bw)
		{
		case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID; break;

		case 1: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
		case 2: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
		case 3: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
		case 4: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

		case 5: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_QUARTER; break;
		case 6: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_HALF; break;
		case 7: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
		case 8: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_SOLID; break;

		case 9:  bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
		case 10: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
		case 11: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
		case 12: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
		default:
			bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID;
		}

		CHAR_INFO c;
		c.Attributes = bg_col | fg_col;
		c.Char.UnicodeChar = sym;
		return c;
	}
};
int main()
{
	Jenova demo;
	if (demo.ConstructConsole(256*2, 240*2, 2, 2))
	{
		demo.Start();
	}
	else
	{
	}
>>>>>>> 8f5fedf08767b8319f51a966f50910324b31f106
}
