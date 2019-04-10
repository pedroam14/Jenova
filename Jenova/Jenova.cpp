#include "JenovaConsole.h"
#pragma region "Constants"
#define PI 3.14159265358979323846f
#pragma endregion

class Jenova : public JenovaConsole
{
	struct Vector3D
	{
		float x, y, z;
	};
	struct Triangle
	{
		Vector3D points[3];
	};
	struct Mesh
	{
		std::vector<Triangle> triangles;
	};
	struct Matrix4x4
	{
		float matrix[4][4] = { 0 };
	};

public:
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
		float nearPlane = 0.1f;
		float farPlane = 1000.0f;
		float fieldOfView = 90.0f;
		float aspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
		float fieldOfViewRad = 1.0f / tanf(fieldOfView * 0.5 / 180.0f * PI);

		matrixProjection.matrix[0][0] = aspectRatio * fieldOfViewRad;
		matrixProjection.matrix[1][1] = fieldOfViewRad;
		matrixProjection.matrix[2][2] = farPlane / (farPlane - nearPlane);
		matrixProjection.matrix[3][2] = (-farPlane * nearPlane) / (farPlane - nearPlane);
		matrixProjection.matrix[2][3] = 1.0f;
		matrixProjection.matrix[3][3] = 0.0f;

		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

		//draw triangles
		for (auto triangle : cube.triangles)
		{
			Triangle projectedTriangle;
			//maybe do this in parallel if it's worth it, otherwise doing it explicitly has less overhead than a simple for loop and actually *is* better for very short operations
			MultiplyMatrixVector(triangle.points[0], projectedTriangle.points[0], matrixProjection);
			MultiplyMatrixVector(triangle.points[1], projectedTriangle.points[1], matrixProjection);
			MultiplyMatrixVector(triangle.points[2], projectedTriangle.points[2], matrixProjection);

			//scaling the triangles into view
			projectedTriangle.points[0].x += 1.0f;
			projectedTriangle.points[0].y += 1.0f;
			projectedTriangle.points[1].x += 1.0f;
			projectedTriangle.points[1].y += 1.0f;
			projectedTriangle.points[2].x += 1.0f;
			projectedTriangle.points[2].y += 1.0f;

			projectedTriangle.points[0].x *= .5f * (float)ScreenWidth();
			projectedTriangle.points[0].y *= .5f * (float)ScreenHeight();
			projectedTriangle.points[1].x *= .5f * (float)ScreenWidth();
			projectedTriangle.points[1].y *= .5f * (float)ScreenHeight();
			projectedTriangle.points[2].x *= .5f * (float)ScreenWidth();
			projectedTriangle.points[2].y *= .5f * (float)ScreenHeight();

			DrawTriangle(projectedTriangle.points[0].x, projectedTriangle.points[0].y,
				projectedTriangle.points[1].x, projectedTriangle.points[1].y,
				projectedTriangle.points[2].x, projectedTriangle.points[2].y, PIXEL_SOLID, FG_WHITE);
		}

		return true;
	}

private:
	Mesh cube;
	Matrix4x4 matrixProjection;

	//writes stuff directly to the output vector, without upsetting anything else
	void MultiplyMatrixVector(Vector3D &i, Vector3D &o, Matrix4x4 &m)
	{
		//now you get why the w is there all the time in unity big dumdum
		o.x = i.x * m.matrix[0][0] + i.y * m.matrix[1][0] + i.z * m.matrix[2][0] + m.matrix[3][0];
		o.y = i.x * m.matrix[0][1] + i.y * m.matrix[1][1] + i.z * m.matrix[2][1] + m.matrix[3][1];
		o.z = i.x * m.matrix[0][2] + i.y * m.matrix[1][2] + i.z * m.matrix[2][2] + m.matrix[3][2];
		float w = i.x * m.matrix[0][3] + i.y * m.matrix[1][3] + i.z * m.matrix[2][3] + m.matrix[3][3];

		if (w != 0.0f)
		{
			o.x /= w;
			o.y /= w;
			o.z /= w;
		}
	};
	
};
int main()
{
	Jenova demo;
	if (demo.ConstructConsole(256, 240, 4, 4))
	{
		demo.Start();
	}
	else
	{

	}

}
