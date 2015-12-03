
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

vector<Triangle> Model::extractOBJ(const char* m) {
	vector<Triangle> temp = vector<Triangle>();
	vector<Vector> vectorList = std::vector<Vector>();
	float x, y, z;
	int v1, v2, v3;
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
				break;
			case 'f':
				fileIn >> x >> y >> z;
				break;
			default:
				fileIn.ignore(256, '\n');
				break;
		}
	}

	fileIn.close();
}