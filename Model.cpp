
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

void Model::render() {
	for(Triangle t : mesh)
		t.render();
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
		
		switch(mode) {
			case 'v':
				fileIn >> v1 >> v2 >> v3;
				vectorList.push_back(Vector(v1, v2, v3));
				break;
			case 'f':
				fileIn >> x >> y >> z;
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