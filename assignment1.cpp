// Tên:  Vu Ngoc Linh
// MSSV: 51201929

#include <Windows.h>
#include <math.h>
#include <GL.H>
#include <glut.h>
#include <iostream>
using namespace std;

#define PI			3.1415926
#define	COLORNUM	14
#define DEG2RAD		3.1415926f/180.0f

float	ColorArr[COLORNUM][3] = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, { 0.0,  0.0, 1.0}, // red, green, blue
								 {1.0, 1.0,  0.0}, { 1.0, 0.0, 1.0},{ 0.0, 1.0, 1.0}, 
								 {0.3, 0.3, 0.3}, {0.5, 0.5, 0.5}, { 0.9,  0.9, 0.9},
								 {1.0, 0.5,  0.5}, { 0.5, 1.0, 0.5},{ 0.5, 0.5, 1.0},
								 {0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}};

class Point3 {
public:
	float x, y, z;

public:
	void set(float dx, float dy, float dz) {
		x = dx;
		y = dy;
		z = dz;
	}
	void set(Point3& p) {
		x = p.x;
		y = p.y;
		z = p.z;
	}

	// Constructor and Destructor
	Point3() { x = y = z = 0; };
	Point3(float dx, float dy, float dz) {
		x = dx;
		y = dy;
		z = dz;
	}
};

class Color3 {
public:
	float r, g, b;

public:
	void set(float red, float green, float blue) {
		r = red;
		g = green;
		b = blue;
	}
	void set(Color3& c) {
		r = c.r;
		g = c.g;
		b = c.b;
	}

	// Constructor and Destructor
	Color3() { r = g = b = 0; }
	Color3(float red, float green, float blue) {
		r = red;
		g = green;
		b = blue;
	}
};

class Vector3 {
public:
	float x, y, z;

public:
	void set(float dx, float dy, float dz) {
		x = dx;
		y = dy;
		z = dz;
	}
	void set(Vector3& v) {
		x = v.x;
		y = v.y;
		z = v.z;
	}
	void flip() {
		x = -x;
		y = -y;
		z = -z;
	}
	void normalize() {
		float temp = sqrt(x*x + y*y + z*z);
		x = x/temp;
		y = y/temp;
		z = z/temp;
	}
	Vector3 cross(Vector3 b) {
		Vector3 c(y*b.z-z*b.y, z*b.x-x*b.z, x*b.y-y*b.x);
		return c;
	}
	float dot(Vector3 b) {
		return x*b.x + y*b.y + z*b.z;
	}

	// Constructor and Destructor
	Vector3() { x = y = z = 0; }
	Vector3(float dx, float dy, float dz) {
		x = dx;
		y = dy;
		z = dz;
	}
	Vector3(Vector3& v) {
		x = v.x;
		y = v.y;
		z = v.z;
	}
};

class VertexID {
public :
	int		vertIndex;
	int		colorIndex;
};

class Face {
public:
	int			nVerts;
	VertexID*	vert;

public:
	Face() {
		nVerts = 0;
		vert = NULL;
	}
	~Face() {
		if (vert != NULL) {
			delete[] vert;
			vert = NULL;
		}
		nVerts = 0;
	}
};

class Mesh {
public:
	int		numVerts;
	Point3* pt;

	int		numFaces;
	Face*	face;

	float	slideX, slideY, slideZ;
	float	rotateX, rotateY, rotateZ;

public:
	// CONSTRUCTOR AND DESTRUCTOR
	Mesh() {
		numVerts = 0;
		pt = NULL;
		numFaces = 0;
		face = NULL;
		slideX = slideY = slideZ = 0;
		rotateX = rotateY = rotateZ = 0;
	}
	~Mesh() {
		if (pt != NULL) {
			delete[] pt;
		}
		if (face != NULL) {
			delete[] face;
		}
		numVerts = 0;
		numFaces = 0;
	}

	void DrawWireframe();
	void DrawColor();
	void SetColor(int colorIdx);

	void CreateCuboid(float fSizeX, float fSizeY, float fSizeZ);
	void CreateCylinder(int nSegment, float fHeight, float fRadius);
	void CreateSphere(int nSlice, int nStack, float radius);
	void CreateUShape(float	fSizeX, float fSizeY, float	fSizeZ, float fThick);
	void CreateYPlane(int row, int col, int size);
	void CreateDodecahedron();
	void CreateIsocahedron();
	void CreateTruncatedcube(float size); 
};

void Mesh::DrawWireframe() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (int f = 0; f < numFaces; f++) {
		glBegin(GL_POLYGON);
		for (int v = 0; v < face[f].nVerts; v++) {
			int iv = face[f].vert[v].vertIndex;
			glVertex3f(pt[iv].x, pt[iv].y, pt[iv].z);
		}
		glEnd();
	}
}

void Mesh::DrawColor() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	for (int f = 0; f < numFaces; f++) {
		glBegin(GL_POLYGON);
		for (int v = 0; v < face[f].nVerts; v++) {
			int iv = face[f].vert[v].vertIndex;
			int ic = face[f].vert[v].colorIndex;

			//ic = f % COLORNUM;
			glColor3f(ColorArr[ic][0], ColorArr[ic][1], ColorArr[ic][2]); 
			glVertex3f(pt[iv].x, pt[iv].y, pt[iv].z);
		}
		glEnd();
	}
};

void Mesh::SetColor(int colorIdx) {
	for (int f = 0; f < numFaces; f++)
	{
		for (int v = 0; v < face[f].nVerts; v++)
		{
			face[f].vert[v].colorIndex = colorIdx;
		}
	}
}

void Mesh::CreateCuboid(float fSizeX, float fSizeY, float fSizeZ)
{
	int i;

	numVerts = 8;
	pt = new Point3[numVerts];
	pt[0].set(-fSizeX,  fSizeY,  fSizeZ);
	pt[1].set( fSizeX,  fSizeY,  fSizeZ);
	pt[2].set( fSizeX,  fSizeY, -fSizeZ);
	pt[3].set(-fSizeX,  fSizeY, -fSizeZ);
	pt[4].set(-fSizeX, -fSizeY,  fSizeZ);
	pt[5].set( fSizeX, -fSizeY,  fSizeZ);
	pt[6].set( fSizeX, -fSizeY, -fSizeZ);
	pt[7].set(-fSizeX, -fSizeY, -fSizeZ);

	numFaces= 6;
	face = new Face[numFaces];

	//Left face
	face[0].nVerts = 4;
	face[0].vert = new VertexID[face[0].nVerts];
	face[0].vert[0].vertIndex = 1;
	face[0].vert[1].vertIndex = 5;
	face[0].vert[2].vertIndex = 6;
	face[0].vert[3].vertIndex = 2;
	for(i = 0; i<face[0].nVerts ; i++)
		face[0].vert[i].colorIndex = 0;
	
	//Right face
	face[1].nVerts = 4;
	face[1].vert = new VertexID[face[1].nVerts];
	face[1].vert[0].vertIndex = 0;
	face[1].vert[1].vertIndex = 3;
	face[1].vert[2].vertIndex = 7;
	face[1].vert[3].vertIndex = 4;
	for(i = 0; i<face[1].nVerts ; i++)
		face[1].vert[i].colorIndex = 1;

	//top face
	face[2].nVerts = 4;
	face[2].vert = new VertexID[face[2].nVerts];
	face[2].vert[0].vertIndex = 0;
	face[2].vert[1].vertIndex = 1;
	face[2].vert[2].vertIndex = 2;
	face[2].vert[3].vertIndex = 3;
	for(i = 0; i<face[2].nVerts ; i++)
		face[2].vert[i].colorIndex = 2;

	//bottom face
	face[3].nVerts = 4;
	face[3].vert = new VertexID[face[3].nVerts];
	face[3].vert[0].vertIndex = 7;
	face[3].vert[1].vertIndex = 6;
	face[3].vert[2].vertIndex = 5;
	face[3].vert[3].vertIndex = 4;
	for(i = 0; i<face[3].nVerts ; i++)
		face[3].vert[i].colorIndex = 3;

	//near face
	face[4].nVerts = 4;
	face[4].vert = new VertexID[face[4].nVerts];
	face[4].vert[0].vertIndex = 4;
	face[4].vert[1].vertIndex = 5;
	face[4].vert[2].vertIndex = 1;
	face[4].vert[3].vertIndex = 0;
	for(i = 0; i<face[4].nVerts ; i++)
		face[4].vert[i].colorIndex = 4;

	//Far face
	face[5].nVerts = 4;
	face[5].vert = new VertexID[face[5].nVerts];
	face[5].vert[0].vertIndex = 3;
	face[5].vert[1].vertIndex = 2;
	face[5].vert[2].vertIndex = 6;
	face[5].vert[3].vertIndex = 7;
	for(i = 0; i<face[5].nVerts ; i++)
		face[5].vert[i].colorIndex = 5;
}

void Mesh::CreateCylinder(int nSegment, float fHeight, float fRadius)
{
	numVerts = 2 * nSegment + 2;
	pt = new Point3[numVerts];
	float sg = 360.0/nSegment;

	//Mat duoi
	for(int i = 0; i < nSegment; i++){
		pt[i].set(fRadius*cos(DEG2RAD*i*sg), -fHeight/2, fRadius*sin(DEG2RAD*i*sg));
	}
	pt[nSegment].set(0, -fHeight/2, 0);

	//Mat tren
	for(int i = nSegment+1; i < 2*nSegment+1; i++){
		pt[i].set(fRadius*cos(DEG2RAD*(i-nSegment-1)*sg), fHeight/2, fRadius*sin(DEG2RAD*(i-nSegment-1)*sg));
	}
	pt[2*nSegment + 1].set(0, fHeight/2, 0); 


	numFaces = 3 * nSegment;
	face = new Face[numFaces];

	
	for(int i = 0; i < nSegment; i++){
		// Mat ngang
		face[i].nVerts = 4;
		face[i].vert = new VertexID[4];
		face[i].vert[0].vertIndex = i;
		face[i].vert[1].vertIndex = nSegment + i + 1;
		if (i != nSegment - 1){
			face[i].vert[2].vertIndex = nSegment + i + 2;
			face[i].vert[3].vertIndex =  i + 1;
		}
		else {
			face[i].vert[2].vertIndex = nSegment + 1;
			face[i].vert[3].vertIndex = 0;
		}

		// Mat duoi
		face[i+nSegment].nVerts = 3;
		face[i+nSegment].vert = new VertexID[3];
		face[i+nSegment].vert[0].vertIndex = nSegment;
		face[i+nSegment].vert[1].vertIndex = i;
		if (i != nSegment - 1)
			face[i+nSegment].vert[2].vertIndex = i + 1;
		else
			face[i+nSegment].vert[2].vertIndex = 0;

		// Mat tren
		face[i+2*nSegment].nVerts = 3;
		face[i+2*nSegment].vert = new VertexID[3];
		face[i+2*nSegment].vert[0].vertIndex = 2*nSegment + 1;
		face[i+2*nSegment].vert[2].vertIndex = i + nSegment + 1;
		if (i != nSegment -1)
			face[i+2*nSegment].vert[1].vertIndex = i + nSegment + 2 ;		
		else
			face[i+2*nSegment].vert[1].vertIndex = nSegment + 1;
	}
}

void Mesh::CreateSphere(int nSlice, int nStack, float radius)
{
	// Stack : noi 2 cuc trai dat - kinh do
	// Slice : Xich dao va cac duong vi do
	numVerts = nSlice * nStack;
	pt = new Point3[numVerts];
	float segment_u = 360./nStack;	// u
	float segment_v = 180./nSlice;	// v
	//float span = radius/n;
	int v = 0;
	for (int i = 0; i < nStack; i++){
		for(int j = 0; j < nSlice; j++){
			pt[v].set(radius*sin(i*segment_u*DEG2RAD)*cos(j*segment_v*DEG2RAD), 
				radius*sin(i*segment_u*DEG2RAD)*sin(j*segment_v*DEG2RAD) ,  radius*cos(i*segment_u*DEG2RAD));
			v++;
		}
	}

	numFaces = nSlice * nStack * 2;
	face = new Face[numFaces];

	for (int k = 0; k <nSlice ; k++){	
		for(int i = 0; i < nStack; i++){
			face[k*nSlice + i].nVerts = 4;
			face[k*nSlice + i].vert = new VertexID[face[k*nSlice + i].nVerts];

			face[k*nSlice + i].vert[0].vertIndex = k*nSlice + i;
			face[k*nSlice + i].vert[1].vertIndex = k*nSlice + (i + 1)%nStack;
			face[k*nSlice + i].vert[2].vertIndex = (k + 1)%nStack*nSlice + (i + 1)%nStack;
			face[k*nSlice + i].vert[3].vertIndex = (k + 1)%nSlice*nSlice + i;

			for(int j = 0; j<face[k*nSlice + i].nVerts ; j++)
				face[k*nSlice + i].vert[j].colorIndex = rand() % 14;
		}
	}
}

void Mesh::CreateUShape(float fSizeX, float fSizeY, float fSizeZ, float fThick)
{
	int i, j;

	numVerts = 20;
	pt = new Point3[numVerts];
	pt[0].set(-fSizeX/2,         fSizeY/2,  fSizeZ/2);
	pt[1].set(-fSizeX/2+fThick,  fSizeY/2,  fSizeZ/2);
	pt[2].set(-fSizeX/2+fThick,  fSizeY/2, -fSizeZ/2+fThick);
	pt[3].set( fSizeX/2-fThick,  fSizeY/2, -fSizeZ/2+fThick);
	pt[4].set( fSizeX/2-fThick,  fSizeY/2,  fSizeZ/2);
	pt[5].set( fSizeX/2,         fSizeY/2,  fSizeZ/2);
	pt[6].set( fSizeX/2,         fSizeY/2, -fSizeZ/2);
	pt[7].set( fSizeX/2-fThick,  fSizeY/2, -fSizeZ/2);
	pt[8].set(-fSizeX/2+fThick,  fSizeY/2, -fSizeZ/2);
	pt[9].set(-fSizeX/2,         fSizeY/2, -fSizeZ/2);

	pt[10].set(-fSizeX/2,        -fSizeY/2,  fSizeZ/2);
	pt[11].set(-fSizeX/2+fThick, -fSizeY/2,  fSizeZ/2);
	pt[12].set(-fSizeX/2+fThick, -fSizeY/2, -fSizeZ/2+fThick);
	pt[13].set( fSizeX/2-fThick, -fSizeY/2, -fSizeZ/2+fThick);
	pt[14].set( fSizeX/2-fThick, -fSizeY/2,  fSizeZ/2);
	pt[15].set( fSizeX/2,        -fSizeY/2,  fSizeZ/2);
	pt[16].set( fSizeX/2,        -fSizeY/2, -fSizeZ/2);
	pt[17].set( fSizeX/2-fThick, -fSizeY/2, -fSizeZ/2);
	pt[18].set(-fSizeX/2+fThick, -fSizeY/2, -fSizeZ/2);
	pt[19].set(-fSizeX/2,        -fSizeY/2, -fSizeZ/2);

	numFaces= 16;
	face = new Face[numFaces];

	for (i = 0; i < 3; i++) {
		// Top face
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 2*i;
		face[i].vert[1].vertIndex = 2*i + 1;
		face[i].vert[2].vertIndex = 9 - i - 1;
		face[i].vert[3].vertIndex = 9 - i;
		for(j = 0; j<face[i].nVerts ; j++)
			face[i].vert[j].colorIndex = 0;

		// Bottom face
		face[i+3].nVerts = 4;
		face[i+3].vert = new VertexID[face[i+3].nVerts];
		face[i+3].vert[3].vertIndex = 2*i + 10;
		face[i+3].vert[2].vertIndex = 2*i + 1 + 10;
		face[i+3].vert[1].vertIndex = 9 - i - 1 + 10;
		face[i+3].vert[0].vertIndex = 9 - i + 10;
		for(j = 0; j<face[i+3].nVerts ; j++)
			face[i+3].vert[j].colorIndex = 0;
	}

	for (i = 0; i < 10; i++) {
		face[i+6].nVerts = 4;
		face[i+6].vert = new VertexID[face[i+6].nVerts];
		if (i == 0) {
			face[i+6].vert[0].vertIndex = 0;
			face[i+6].vert[1].vertIndex = 9;
			face[i+6].vert[2].vertIndex = 19;
			face[i+6].vert[3].vertIndex = 10;
		} else {
			face[i+6].vert[0].vertIndex = i;
			face[i+6].vert[1].vertIndex = i - 1;
			face[i+6].vert[2].vertIndex = i + 9;
			face[i+6].vert[3].vertIndex = i + 10;
		}
		for(j = 0; j<face[i+6].nVerts ; j++)
			face[i+6].vert[j].colorIndex = 0;
	}
}


void Mesh::CreateYPlane(int row, int col, int size)
{
	numVerts = (row + 1)*(col + 1);
	pt = new Point3[numVerts];
	for (int i = 0; i <= row; i++)
		for (int j = 0; j <= col; j++)	
			pt[i + j + col * i].set((-col/2 + j)*size, 0, (-row/2 + i)*size);

	numFaces = row*col;
	face = new Face[numFaces];
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			int index = i + j + (col - 1)*i;
			int vert = i + j + col * i; // toa do dinh dau tien cua mat index
			face[index].nVerts = 4;
			face[index].vert = new VertexID[4];
			face[index].vert[0].vertIndex = vert;
			face[index].vert[1].vertIndex = i + j + 1 + col*(i + 1);
			face[index].vert[2].vertIndex = i + j + 2 + col*(i + 1);
			face[index].vert[3].vertIndex = i + j + col*i + 1;

			for (int i = 0; i < 4; i++)
				if (vert % 2 == 0)
					face[index].vert[i].colorIndex = 12;
				else
					face[index].vert[i].colorIndex = 13;
		}
	}
}

void Mesh::CreateIsocahedron() 
{
	numVerts = 12;
	float t = (sqrt(5.0)-1)/2;
	pt = new Point3[numVerts];
	pt[0].set(0, 1, t);
	pt[1].set(0, 1, -t);
	pt[2].set(1, t, 0);
	pt[3].set(1, -t, 0);
	pt[4].set(0, -1, -t);
	pt[5].set(0, -1, t);
	pt[6].set(t, 0, 1);
	pt[7].set(-t, 0, 1);
	pt[8].set(t, 0, -1);
	pt[9].set(-t, 0, -1);
	pt[10].set(-1, t, 0);
	pt[11].set(-1, -t, 0);

	numFaces = 20;
	face = new Face[numFaces];
	face[0].nVerts = 3;
	face[0].vert = new VertexID[3];
	face[0].vert[0].vertIndex = 0;
	face[0].vert[1].vertIndex = 2;
	face[0].vert[2].vertIndex = 6;

	face[1].nVerts = 3;
	face[1].vert = new VertexID[3];
	face[1].vert[0].vertIndex = 6;
	face[1].vert[1].vertIndex = 2;
	face[1].vert[2].vertIndex = 3;

	face[2].nVerts = 3;
	face[2].vert = new VertexID[3];
	face[2].vert[0].vertIndex = 6;
	face[2].vert[1].vertIndex = 3;
	face[2].vert[2].vertIndex = 5;

	face[3].nVerts = 3;
	face[3].vert = new VertexID[3];
	face[3].vert[0].vertIndex = 7;
	face[3].vert[1].vertIndex = 6;
	face[3].vert[2].vertIndex = 5;

	face[4].nVerts = 3;
	face[4].vert = new VertexID[3];
	face[4].vert[0].vertIndex = 7;
	face[4].vert[1].vertIndex = 0;
	face[4].vert[2].vertIndex = 6;

	face[5].nVerts = 3;
	face[5].vert = new VertexID[3];
	face[5].vert[0].vertIndex = 2;
	face[5].vert[1].vertIndex = 8;
	face[5].vert[2].vertIndex = 3;

	face[6].nVerts = 3;
	face[6].vert = new VertexID[3];
	face[6].vert[0].vertIndex = 1;
	face[6].vert[1].vertIndex = 8;
	face[6].vert[2].vertIndex = 2;

	face[7].nVerts = 3;
	face[7].vert = new VertexID[3];
	face[7].vert[0].vertIndex = 0;
	face[7].vert[1].vertIndex = 1;
	face[7].vert[2].vertIndex = 2;

	face[8].nVerts = 3;
	face[8].vert = new VertexID[3];
	face[8].vert[0].vertIndex = 10;
	face[8].vert[1].vertIndex = 1;
	face[8].vert[2].vertIndex = 0;

	face[9].nVerts = 3;
	face[9].vert = new VertexID[3];
	face[9].vert[0].vertIndex = 10;
	face[9].vert[1].vertIndex = 9;
	face[9].vert[2].vertIndex = 1;

	face[10].nVerts = 3;
	face[10].vert = new VertexID[3];
	face[10].vert[0].vertIndex = 1;
	face[10].vert[1].vertIndex = 9;
	face[10].vert[2].vertIndex = 8;

	face[11].nVerts = 3;
	face[11].vert = new VertexID[3];
	face[11].vert[0].vertIndex = 3;
	face[11].vert[1].vertIndex = 8;
	face[11].vert[2].vertIndex = 4;

	face[12].nVerts = 3;
	face[12].vert = new VertexID[3];
	face[12].vert[0].vertIndex = 5;
	face[12].vert[1].vertIndex = 3;
	face[12].vert[2].vertIndex = 4;

	face[13].nVerts = 3;
	face[13].vert = new VertexID[3];
	face[13].vert[0].vertIndex = 11;
	face[13].vert[1].vertIndex = 5;
	face[13].vert[2].vertIndex = 4;

	face[14].nVerts = 3;
	face[14].vert = new VertexID[3];
	face[14].vert[0].vertIndex = 11;
	face[14].vert[1].vertIndex = 10;
	face[14].vert[2].vertIndex = 7;

	face[15].nVerts = 3;
	face[15].vert = new VertexID[3];
	face[15].vert[0].vertIndex = 7;
	face[15].vert[1].vertIndex = 10;
	face[15].vert[2].vertIndex = 0;

	face[16].nVerts = 3;
	face[16].vert = new VertexID[3];
	face[16].vert[0].vertIndex = 9;
	face[16].vert[1].vertIndex = 11;
	face[16].vert[2].vertIndex = 4;

	face[17].nVerts = 3;
	face[17].vert = new VertexID[3];
	face[17].vert[0].vertIndex = 9;
	face[17].vert[1].vertIndex = 4;
	face[17].vert[2].vertIndex = 8;

	face[18].nVerts = 3;
	face[18].vert = new VertexID[3];
	face[18].vert[0].vertIndex = 11;
	face[18].vert[1].vertIndex = 7;
	face[18].vert[2].vertIndex = 5;

	face[19].nVerts = 3;
	face[19].vert = new VertexID[3];
	face[19].vert[0].vertIndex = 11;
	face[19].vert[1].vertIndex = 9;
	face[19].vert[2].vertIndex = 10;

	int color = 1;
	for (int i = 0; i < numFaces; i++) {
		for (int j = 0; j < 3; j++)
			face[i].vert[j].colorIndex = color;
		color = (color+1)%14;
	}
}

void Mesh::CreateDodecahedron()
{	
	int i;

	numVerts = 20;
	pt = new Point3[numVerts];
	Mesh iso;
	iso.CreateIsocahedron();
	for (i = 0; i < numVerts; i++) {
		float x = ( iso.pt[iso.face[i].vert[0].vertIndex].x + 
					iso.pt[iso.face[i].vert[1].vertIndex].x +
					iso.pt[iso.face[i].vert[2].vertIndex].x ) / 3.0;
		float y = ( iso.pt[iso.face[i].vert[0].vertIndex].y + 
					iso.pt[iso.face[i].vert[1].vertIndex].y +
					iso.pt[iso.face[i].vert[2].vertIndex].y ) / 3.0;
		float z = ( iso.pt[iso.face[i].vert[0].vertIndex].z + 
					iso.pt[iso.face[i].vert[1].vertIndex].z +
					iso.pt[iso.face[i].vert[2].vertIndex].z ) / 3.0;
		pt[i].set(x, y, z);
	}

	numFaces = 12;
	face = new Face[numFaces];
	face[0].nVerts = 5;
	face[0].vert = new VertexID[5];
	face[0].vert[0].vertIndex = 0;
	face[0].vert[1].vertIndex = 4;
	face[0].vert[2].vertIndex = 15;
	face[0].vert[3].vertIndex = 8;
	face[0].vert[4].vertIndex = 7;

	face[1].nVerts = 5;
	face[1].vert = new VertexID[5];
	face[1].vert[0].vertIndex = 6;
	face[1].vert[1].vertIndex = 7;
	face[1].vert[2].vertIndex = 8;
	face[1].vert[3].vertIndex = 9;
	face[1].vert[4].vertIndex = 10;

	face[2].nVerts = 5;
	face[2].vert = new VertexID[5];
	face[2].vert[0].vertIndex = 5;
	face[2].vert[1].vertIndex = 1;
	face[2].vert[2].vertIndex = 0;
	face[2].vert[3].vertIndex = 7;
	face[2].vert[4].vertIndex = 6;

	face[3].nVerts = 5;
	face[3].vert = new VertexID[5];
	face[3].vert[0].vertIndex = 12;
	face[3].vert[1].vertIndex = 2;
	face[3].vert[2].vertIndex = 1;
	face[3].vert[3].vertIndex = 5;
	face[3].vert[4].vertIndex = 11;

	face[4].nVerts = 5;
	face[4].vert = new VertexID[5];
	face[4].vert[0].vertIndex = 13;
	face[4].vert[1].vertIndex = 12;
	face[4].vert[2].vertIndex = 11;
	face[4].vert[3].vertIndex = 17;
	face[4].vert[4].vertIndex = 16;

	face[5].nVerts = 5;
	face[5].vert = new VertexID[5];
	face[5].vert[0].vertIndex = 3;
	face[5].vert[1].vertIndex = 2;
	face[5].vert[2].vertIndex = 12;
	face[5].vert[3].vertIndex = 13;
	face[5].vert[4].vertIndex = 18;

	face[6].nVerts = 5;
	face[6].vert = new VertexID[5];
	face[6].vert[0].vertIndex = 0;
	face[6].vert[1].vertIndex = 1;
	face[6].vert[2].vertIndex = 2;
	face[6].vert[3].vertIndex = 3;
	face[6].vert[4].vertIndex = 4;

	face[7].nVerts = 5;
	face[7].vert = new VertexID[5];
	face[7].vert[0].vertIndex = 4;
	face[7].vert[1].vertIndex = 3;
	face[7].vert[2].vertIndex = 18;
	face[7].vert[3].vertIndex = 14;
	face[7].vert[4].vertIndex = 15;

	face[8].nVerts = 5;
	face[8].vert = new VertexID[5];
	face[8].vert[0].vertIndex = 11;
	face[8].vert[1].vertIndex = 5;
	face[8].vert[2].vertIndex = 6;
	face[8].vert[3].vertIndex = 10;
	face[8].vert[4].vertIndex = 17;

	face[9].nVerts = 5;
	face[9].vert = new VertexID[5];
	face[9].vert[0].vertIndex = 16;
	face[9].vert[1].vertIndex = 17;
	face[9].vert[2].vertIndex = 10;
	face[9].vert[3].vertIndex = 9;
	face[9].vert[4].vertIndex = 19;

	face[10].nVerts = 5;
	face[10].vert = new VertexID[5];
	face[10].vert[0].vertIndex = 15;
	face[10].vert[1].vertIndex = 14;
	face[10].vert[2].vertIndex = 19;
	face[10].vert[3].vertIndex = 9;
	face[10].vert[4].vertIndex = 8;

	face[11].nVerts = 5;
	face[11].vert = new VertexID[5];
	face[11].vert[0].vertIndex = 18;
	face[11].vert[1].vertIndex = 13;
	face[11].vert[2].vertIndex = 16;
	face[11].vert[3].vertIndex = 19;
	face[11].vert[4].vertIndex = 14;

	int color = 1;
	for (i = 0; i < numFaces; i++) {
		for (int j = 0; j < 5; j++)
			face[i].vert[j].colorIndex = color;
		color = (color+1)%14;
	}
}

void Mesh::CreateTruncatedcube(float size) {
	numVerts = 24;
	pt = new Point3[numVerts];
	pt[0].set(-size,   size,  size/2);
	pt[1].set(-size/2, size,  size);
	pt[2].set( size/2, size,  size);
	pt[3].set( size,   size,  size/2);
	pt[4].set( size,   size, -size/2);
	pt[5].set( size/2, size, -size);
	pt[6].set(-size/2, size, -size);
	pt[7].set(-size,   size, -size/2);

	pt[8].set( -size, size/2,  size);
	pt[9].set(  size, size/2,  size);
	pt[10].set( size, size/2, -size);
	pt[11].set(-size, size/2, -size);

	pt[12].set(-size, -size/2,  size);
	pt[13].set( size, -size/2,  size);
	pt[14].set( size, -size/2, -size);
	pt[15].set(-size, -size/2, -size);

	pt[16].set(-size,   -size,  size/2);
	pt[17].set(-size/2, -size,  size);
	pt[18].set( size/2, -size,  size);
	pt[19].set( size,   -size,  size/2);
	pt[20].set( size,   -size, -size/2);
	pt[21].set( size/2, -size, -size);
	pt[22].set(-size/2, -size, -size);
	pt[23].set(-size,   -size, -size/2);

	numFaces = 14;
	face = new Face[numFaces];
	face[0].nVerts = 8;
	face[0].vert = new VertexID[8];
	face[0].vert[0].vertIndex = 0;
	face[0].vert[1].vertIndex = 1;
	face[0].vert[2].vertIndex = 2;
	face[0].vert[3].vertIndex = 3;
	face[0].vert[4].vertIndex = 4;
	face[0].vert[5].vertIndex = 5;
	face[0].vert[6].vertIndex = 6;
	face[0].vert[7].vertIndex = 7;

	face[1].nVerts = 8;
	face[1].vert = new VertexID[8];
	face[1].vert[0].vertIndex = 1;
	face[1].vert[1].vertIndex = 8;
	face[1].vert[2].vertIndex = 12;
	face[1].vert[3].vertIndex = 17;
	face[1].vert[4].vertIndex = 18;
	face[1].vert[5].vertIndex = 13;
	face[1].vert[6].vertIndex = 9;
	face[1].vert[7].vertIndex = 2;

	face[2].nVerts = 8;
	face[2].vert = new VertexID[8];
	face[2].vert[0].vertIndex = 23;
	face[2].vert[1].vertIndex = 22;
	face[2].vert[2].vertIndex = 21;
	face[2].vert[3].vertIndex = 20;
	face[2].vert[4].vertIndex = 19;
	face[2].vert[5].vertIndex = 18;
	face[2].vert[6].vertIndex = 17;
	face[2].vert[7].vertIndex = 16;

	face[3].nVerts = 8;
	face[3].vert = new VertexID[8];
	face[3].vert[0].vertIndex = 22;
	face[3].vert[1].vertIndex = 15;
	face[3].vert[2].vertIndex = 11;
	face[3].vert[3].vertIndex = 6;
	face[3].vert[4].vertIndex = 5;
	face[3].vert[5].vertIndex = 10;
	face[3].vert[6].vertIndex = 14;
	face[3].vert[7].vertIndex = 21;

	face[4].nVerts = 8;
	face[4].vert = new VertexID[8];
	face[4].vert[0].vertIndex = 0;
	face[4].vert[1].vertIndex = 7;
	face[4].vert[2].vertIndex = 11;
	face[4].vert[3].vertIndex = 15;
	face[4].vert[4].vertIndex = 23;
	face[4].vert[5].vertIndex = 16;
	face[4].vert[6].vertIndex = 12;
	face[4].vert[7].vertIndex = 8;

	face[5].nVerts = 8;
	face[5].vert = new VertexID[8];
	face[5].vert[0].vertIndex = 4;
	face[5].vert[1].vertIndex = 3;
	face[5].vert[2].vertIndex = 9;
	face[5].vert[3].vertIndex = 13;
	face[5].vert[4].vertIndex = 19;
	face[5].vert[5].vertIndex = 20;
	face[5].vert[6].vertIndex = 14;
	face[5].vert[7].vertIndex = 10;

	face[6].nVerts = 3;
	face[6].vert = new VertexID[3];
	face[6].vert[0].vertIndex = 1;
	face[6].vert[1].vertIndex = 0;
	face[6].vert[2].vertIndex = 8;

	face[7].nVerts = 3;
	face[7].vert = new VertexID[3];
	face[7].vert[0].vertIndex = 3;
	face[7].vert[1].vertIndex = 2;
	face[7].vert[2].vertIndex = 9;

	face[8].nVerts = 3;
	face[8].vert = new VertexID[3];
	face[8].vert[0].vertIndex = 5;
	face[8].vert[1].vertIndex = 4;
	face[8].vert[2].vertIndex = 10;

	face[9].nVerts = 3;
	face[9].vert = new VertexID[3];
	face[9].vert[0].vertIndex = 7;
	face[9].vert[1].vertIndex = 6;
	face[9].vert[2].vertIndex = 11;

	face[10].nVerts = 3;
	face[10].vert = new VertexID[3];
	face[10].vert[0].vertIndex = 16;
	face[10].vert[1].vertIndex = 17;
	face[10].vert[2].vertIndex = 12;

	face[11].nVerts = 3;
	face[11].vert = new VertexID[3];
	face[11].vert[0].vertIndex = 18;
	face[11].vert[1].vertIndex = 19;
	face[11].vert[2].vertIndex = 13;

	face[6].nVerts = 3;
	face[6].vert = new VertexID[3];
	face[6].vert[0].vertIndex = 20;
	face[6].vert[1].vertIndex = 21;
	face[6].vert[2].vertIndex = 14;

	face[6].nVerts = 3;
	face[6].vert = new VertexID[3];
	face[6].vert[0].vertIndex = 22;
	face[6].vert[1].vertIndex = 23;
	face[6].vert[2].vertIndex = 15;

	int color = 1;
	for (int i = 0; i < numFaces; i++) {
		for (int j = 0; j < face[i].nVerts; j++)
			face[i].vert[j].colorIndex = color;
		color = (color+1)%14;
	}
}

int		screenWidth = 600;
int		screenHeight= 600;

bool	bWireFrame = false;
bool	b4View = false;
bool	isCartoon = false;

float	YPlanePos = 0;

float	cylBaseRadius = 1.2;
float	cylBaseHeight = 0.2; 
float	cylBaseRotateAngle = 0.0;

float	cuboidBaseSizeXZ = 0.3;
float	cuboidBaseSizeY = 3.0;
float	cuboidRotateAngle = 0;

float	cylAxisRadius = 0.1;
float	cylAxisHeight = 0.3; 
float	cylAxisOffset = 0.2;
float	cylAxisRotateAngle = 0;

Mesh	planeY;
Mesh	cylBase;
Mesh	cuboidBase;
Mesh    cylAxis;
Mesh	uShape;
Mesh	cubAxis;
Mesh	cylMain;
Mesh	sphere;

Mesh	iso;
Mesh	dode;
Mesh	sphe;
Mesh	trun;

float camera_angle;
float camera_height;
float camera_dis;
float camera_X, camera_Y, camera_Z;
float lookAt_X, lookAt_Y, lookAt_Z;

void mySetupCameraVolume(int nType)
{
	if(nType == 4)
	{
		glMatrixMode(GL_PROJECTION);			// set projection matrix current matrix
		glLoadIdentity();						// reset projection matrix

		// calculate aspect ratio of window
		gluPerspective(60.0f,(GLfloat)screenWidth/(GLfloat)screenHeight,1.0f,1000.0f);
	}
	else 
	{
		glMatrixMode(GL_PROJECTION);			// set projection matrix current matrix
		glLoadIdentity();						// reset projection matrix
		glOrtho(-5, 5, -5, 5, -1000, 1000);
	}
	
}

void changeCameraPos()
{
	camera_Y = camera_height;
	camera_X = camera_dis * cos(camera_angle);
	camera_Z = camera_dis * sin(camera_angle);
}


void mySpecialKeyboard(int theKey, int mouseX, int mouseY)
{
	switch(theKey) {

	case GLUT_KEY_UP:
		camera_height += 0.1;
		changeCameraPos();
		break;
	
	case GLUT_KEY_DOWN:
		camera_height -= 0.1;
		changeCameraPos();
		break;

	case GLUT_KEY_RIGHT:
		camera_angle += 0.1;
		changeCameraPos();
		break;

	case GLUT_KEY_LEFT:
		camera_angle -= 0.1;
		changeCameraPos();
		break;
	
	default:
		break;
	}

	glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y)
{
	float	fRInc;
	float	fAngle;
    switch(key)
    {
	case '1':
		cylBaseRotateAngle += 2;
		if(cylBaseRotateAngle > 360)
			cylBaseRotateAngle -= 360;
		break;
	case '2':
		cylBaseRotateAngle -= 2;
		if(cylBaseRotateAngle < 0)
			cylBaseRotateAngle += 360;
		break;
	case '3':
		cylAxisRotateAngle += 2;
		if(cylAxisRotateAngle > 360)
			cylAxisRotateAngle -= 360;
		break;
	case '4':
		cylAxisRotateAngle -= 2;
		if(cylAxisRotateAngle < 0)
			cylAxisRotateAngle += 360;
		break;
	case '5':
		cuboidRotateAngle += 2;
		if (cuboidRotateAngle > 360)
			cuboidRotateAngle -= 360;
		break;
	case '6':
		cuboidRotateAngle -= 2;
		if (cuboidRotateAngle < 0)
			cuboidRotateAngle += 360;
		break;
	case 'w':
	case 'W':
		bWireFrame = !bWireFrame;
		break;
	case 'a':
	case 'A':
		isCartoon = !isCartoon;
		break;
	case 'v':
	case 'V':
		b4View = !b4View;
		break;
	case '+' :
		camera_dis += 0.1;
		changeCameraPos();
		break;
	case '-' : 
		camera_dis -= 0.1;
		changeCameraPos();
		break;
	}
    glutPostRedisplay();
}

bool cylBaseRotate = true;
bool cylAxisRotate = true;
bool cuboidRotate = true;
void cartoonAction(int val) 
{
	if (isCartoon) {
		if (cylBaseRotate)
			cylBaseRotateAngle += val;
		else
			cylBaseRotateAngle -= val;
		if (cylAxisRotate)
			cylAxisRotateAngle += 2*val;
		else
			cylAxisRotateAngle -= 2*val;
		if (cuboidRotate)
			cuboidRotateAngle += 3*val;
		else
			cuboidRotateAngle -= 3*val;

		if (cylBaseRotateAngle > 360 || cylBaseRotateAngle < 0)
			cylBaseRotate = !cylBaseRotate;
		if (cylAxisRotateAngle > 360 || cylAxisRotateAngle < 0)
			cylAxisRotate = !cylAxisRotate;
		if (cuboidRotateAngle > 360 || cuboidRotateAngle < 0)
			cuboidRotate = !cuboidRotate;
	}
	glutPostRedisplay();
	glutTimerFunc(30, cartoonAction, 2);
}

void drawFloor()
{
	glPushMatrix();
		glTranslated(planeY.slideX, planeY.slideY, planeY.slideZ);
		if(bWireFrame)
			planeY.DrawWireframe();
		else
			planeY.DrawColor();
	glPopMatrix();
}
void drawCylinderBase()
{
	glPushMatrix();		
		glRotated(cylBaseRotateAngle, 0, 1, 0);
		//////////////////////////////////
		glTranslated(0, cylBase.slideY, 0);
		if(bWireFrame)
			cylBase.DrawWireframe();
		else
			cylBase.DrawColor();
	glPopMatrix();
}
void drawCuboidBase()
{
	glPushMatrix();		
		glRotated(cylBaseRotateAngle, 0, 1, 0);
		//////////////////////////////////
		glTranslated(0, cuboidBase.slideY, 0);
		if(bWireFrame)
			cuboidBase.DrawWireframe();
		else
			cuboidBase.DrawColor();
	glPopMatrix();
}
void drawCylAxis()
{
	glPushMatrix();		
		glRotated(cylBaseRotateAngle, 0, 1, 0);
		//////////////////////////////////
		glTranslated(0, cylAxis.slideY , cylAxis.slideZ);
		glRotatef(90, 1, 0, 0);
		glRotated(cylAxisRotateAngle, 0, 1, 0);
		if(bWireFrame)
			cylAxis.DrawWireframe();
		else
			cylAxis.DrawColor();
	glPopMatrix();
}
void drawUShape()
{
	glPushMatrix();
	glRotated(cylBaseRotateAngle, 0, 1, 0);
	//////////////////////////////////
	glTranslated(0, uShape.slideY, uShape.slideZ);
	glRotated(cylAxisRotateAngle, 0, 0, 1);
	if (bWireFrame)
		uShape.DrawWireframe();
	else
		uShape.DrawColor();
	glPopMatrix();

}
void drawCubAxis()
{
	glPushMatrix();
	glRotated(cylBaseRotateAngle, 0, 1, 0);
	//////////////////////////////////
	glTranslated(0, cubAxis.slideY, cubAxis.slideZ);
	glRotated(cylAxisRotateAngle, 0, 0, 1);
	glRotated(cuboidRotateAngle, 1, 0, 0);
	if (bWireFrame)
		cubAxis.DrawWireframe();
	else
		cubAxis.DrawColor();
	glPopMatrix();
}
void drawCylMain()
{
	glPushMatrix();
	glRotated(cylBaseRotateAngle, 0, 1, 0);
	//////////////////////////////////
	glTranslated(0, cylMain.slideY, cylMain.slideZ);
	glRotatef(90, 0, 0, 1);
	
	glRotated(cylAxisRotateAngle, 0, 0, 1);
	glRotated(cuboidRotateAngle, 0, 1, 0);
	if (bWireFrame)
		cylMain.DrawWireframe();
	else
		cylMain.DrawColor();
	glPopMatrix();
}
void drawSphere(int number)
{
	glPushMatrix();
	glRotated(cylBaseRotateAngle, 0, 1, 0);
	//////////////////////////////////
	glTranslated(0, sphere.slideY, sphere.slideZ);
	glRotated(cylAxisRotateAngle, 0, 0, 1);
	glRotated(-cuboidRotateAngle, 1, 0, 0);
		
	glRotated(90, 0, 0, 1);
	glRotated(number * 120, 0, 1, 0);
	glTranslatef(0, 0, 0.5);
	if (bWireFrame)
		sphere.DrawWireframe();
	else
		sphere.DrawColor();
	glPopMatrix();
}
void drawDodecahedron() 
{
	glPushMatrix();
	//////////////////////////////////
	glTranslated(dode.slideX, dode.slideY, dode.slideZ);
	glScaled(0.45, 0.45, 0.45);
	if (bWireFrame)
		dode.DrawWireframe();
	else
		dode.DrawColor();
	glPopMatrix();
}
void drawIsocahedron() 
{
	glPushMatrix();
	//////////////////////////////////
	glTranslated(iso.slideX, iso.slideY, iso.slideZ);
	glScaled(0.4, 0.4, 0.4);
	if (bWireFrame)
		iso.DrawWireframe();
	else
		iso.DrawColor();
	glPopMatrix();
}
void drawSphere()
{
	glPushMatrix();
	//////////////////////////////////
	glTranslated(sphe.slideX, sphe.slideY, sphe.slideZ);
	if (bWireFrame)
		sphe.DrawWireframe();
	else
		sphe.DrawColor();
	glPopMatrix();
}
void drawTruncatedcube() 
{
	glPushMatrix();
	//////////////////////////////////
	glTranslated(trun.slideX, trun.slideY, trun.slideZ);
	if (bWireFrame)
		trun.DrawWireframe();
	else
		trun.DrawColor();
	glPopMatrix();
}

void DisplayOneView(int nType, int left, int right, int top, int bottom)
{
	mySetupCameraVolume(nType);
	glViewport(left, top, right - left, bottom - top);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//code here
	changeCameraPos();
	if (nType == 1) 
		gluLookAt(0, camera_dis, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
	else if (nType == 2) 
		gluLookAt(0, 0.0, camera_dis, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	else if (nType == 3) 
		gluLookAt(camera_dis, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	else 
		gluLookAt(camera_X, camera_Y, camera_Z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	
	/////////////////////////////////////////////////
	drawFloor();
	drawCylinderBase();
	drawCuboidBase();
	drawCylAxis();
	drawUShape();
	drawCubAxis();
	drawCylMain();
	sphere.SetColor(0);
	drawSphere(0);
	sphere.SetColor(1);
	drawSphere(1);
	sphere.SetColor(7);
	drawSphere(2);

	drawDodecahedron();
	drawIsocahedron();
	drawSphere();
	drawTruncatedcube();
}

void myDisplay()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (b4View == false)
	{
		DisplayOneView(4, 0, screenWidth, 0, screenHeight);
	}
	else
	{
		DisplayOneView(1, 0, screenWidth / 2, 0, screenHeight / 2);
		DisplayOneView(2, 0, screenWidth / 2, screenHeight / 2, screenHeight);
		DisplayOneView(3, screenWidth / 2, screenWidth, screenHeight / 2, screenHeight);
		DisplayOneView(4, screenWidth / 2, screenWidth, 0, screenHeight / 2);
	}

	glFlush();
    glutSwapBuffers();
}

void myInit()
{
	float	fHalfSize = 3;

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glColor3f(0.0f, 0.0f, 0.0f);

	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-fHalfSize, fHalfSize, -fHalfSize, fHalfSize, -1000, 1000);

	camera_angle = 0.0; 
	camera_height = 2.0; 
	camera_dis = 11; 

	glMatrixMode(GL_MODELVIEW);
}
int main(int argc, char* argv[])
{
	cout << "1, 2: Rotate the base" << endl;
	cout << "3, 4: Rotate axes 1 (cylinder axes)" << endl;
	cout << "5, 6: Rotate axes 2 (cuboid axes)" << endl;
	cout << "W, w: Switch between wireframe and solid mode" << endl;
	cout << "V, v: to switch between 1 and 4 views." << endl;
	cout << "+   : to increase camera distance." << endl;
	cout << "-   : to decrease camera distance." << endl;
	cout << "up arrow  : to increase camera height." << endl;
	cout << "down arrow: to decrease camera height." << endl;
	cout << "<-        : to rotate camera clockwise." << endl;
	cout << "->        : to rotate camera counterclockwise." << endl;

	glutInit(&argc, (char**)argv); //initialize the tool kit
	glutInitDisplayMode(GLUT_DOUBLE |GLUT_RGB);//set the display mode
	glutInitWindowSize(screenWidth, screenHeight); //set window size
	glutInitWindowPosition(100, 100); // set window position on screen
	glutCreateWindow("Assignment1 - Vu Ngoc Linh - 51201929"); // open the screen window

	// San nha
	planeY.CreateYPlane(10, 10, 1);
	planeY.slideY = YPlanePos;

	// Cai de xanh la
	cylBase.CreateCylinder(20, cylBaseHeight, cylBaseRadius);
	cylBase.slideY = YPlanePos + cylBaseHeight/2.0;
	cylBase.SetColor(1);

	// Cai than do
	cuboidBase.CreateCuboid(cuboidBaseSizeXZ/2.0, cuboidBaseSizeY/2.0, cuboidBaseSizeXZ/2.0);
	cuboidBase.slideY = YPlanePos + cylBaseHeight + cuboidBaseSizeY/2.0;
	cuboidBase.SetColor(0);

	// Truc quay 1 xanh duong
	cylAxis.CreateCylinder(20, cylAxisHeight, cylAxisRadius);
	cylAxis.slideY = YPlanePos + cylBaseHeight + cuboidBaseSizeY - cylAxisOffset;
	cylAxis.slideZ = cylAxisHeight/2+cuboidBaseSizeXZ/2;
	cylAxis.SetColor(2);

	// Hinh chu U
	uShape.CreateUShape(1.6, 0.3, 1.4, 0.1);
	uShape.slideY = YPlanePos + cylBaseHeight + cuboidBaseSizeY - cylAxisOffset;
	uShape.slideZ = 1.4/2 + cylAxisHeight + cuboidBaseSizeXZ/2;
	uShape.SetColor(3);

	// Truc hong
	cubAxis.CreateCuboid(0.7, 0.04, 0.04);
	cubAxis.slideY = YPlanePos + cylBaseHeight + cuboidBaseSizeY - cylAxisOffset;
	cubAxis.slideZ = 1.0 + cylAxisHeight + cuboidBaseSizeXZ/2;
	cubAxis.SetColor(4);

	// Dia tron o giua
	cylMain.CreateCylinder(20, 0.1, 0.7);
	cylMain.slideY = YPlanePos + cylBaseHeight + cuboidBaseSizeY - cylAxisOffset;
	cylMain.slideZ = 1.0 + cylAxisHeight + cuboidBaseSizeXZ/2;
	cylMain.SetColor(5);

	// May vien bi
	sphere.CreateSphere(10, 10, 0.15);
	sphere.slideY = YPlanePos + cylBaseHeight + cuboidBaseSizeY - cylAxisOffset;
	sphere.slideZ = 1.0 + cylAxisHeight + cuboidBaseSizeXZ/2;

	// Cuc 12 mat
	dode.CreateDodecahedron();
	dode.slideX = 3;
	dode.slideY = YPlanePos + 0.83*0.45;
	dode.slideZ = 3;


	// Cuc 20 mat
	iso.CreateIsocahedron();
	iso.slideX = -3;
	iso.slideY = YPlanePos + 1*0.4;
	iso.slideZ = 3;

	// Cuc hinh cau
	sphe.CreateSphere(10, 10, 0.4);
	sphe.slideX = -3;
	sphe.slideY = YPlanePos + 0.4;
	sphe.slideZ = -3;

	// Cuc vuong mai goc
	trun.CreateTruncatedcube(0.4);
	trun.slideX = 3;
	trun.slideY = YPlanePos + 0.4;
	trun.slideZ = -3;

	myInit();

	glutKeyboardFunc(myKeyboard);
    glutDisplayFunc(myDisplay);
	glutSpecialFunc(mySpecialKeyboard);
	glutTimerFunc(30, cartoonAction, 2);
	  
	glutMainLoop();
	return 0;
}