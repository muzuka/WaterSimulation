
/*
 *	Model class
 *
 *  holds collection of triangles
 *
 *  Written by: Sean Brown
 */

#include "Model.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>

using namespace std;

Model::Model() {
	this->mesh = std::vector<Triangle>();
}

Model::Model(vector<Triangle> mesh) {
	this->mesh = mesh;
}

Model::Model(const char* m) {
	this->mesh = extractOBJ(m);
}

vector<Triangle> Model::getMesh() {
	return mesh;
}

vector<Triangle> Model::extractOBJ(const char* m) {
	vector<Triangle> temp = vector<Triangle>();
	vector<Vector> vectorList = std::vector<Vector>();
	int x, y, z;
	double v1, v2, v3;
	char mode;
	fstream fileIn = fstream();

	fileIn.open(m, fstream::in);
	if(!fileIn.is_open()) {
		cout << "File didn't open" << endl;
		exit(EXIT_FAILURE);
	}

	while(fileIn) {
		fileIn >> mode;
		cout << "mode = " << mode << endl;
		
		switch(mode) {
			case 'v':
				cout << "vertex input" << endl;
				fileIn >> v1 >> v2 >> v3;
				cout << "vertices = " << v1 << " " << v2 << " " << v3 << endl;
				vectorList.push_back(Vector(v1, v2, v3));
				break;
			case 'f':
				cout << "face input" << endl;
				fileIn >> x >> y >> z;
				cout << "faces = " << x << " " << y << " " << z << endl;
				temp.push_back(Triangle(vectorList[x-1], vectorList[y-1], vectorList[z-1]));
				break;
			default:
				fileIn.ignore(256, '\n');
				break;
		}
	}

	fileIn.close();

	cout << temp.size() << endl;
	return temp;
}