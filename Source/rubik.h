/*
	The main purpose of this class is to have a batch of cubes (27) and make them behave like a standard Rubik's cube.
*/

#pragma once

#include "cube.h"
#include "utils.h"

#include <vector>

#define GLEW_STATIC 1
#include <GL/glew.h> 

#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>

class Rubik {
public:
	static const int NUMBER_OF_ROWS = 3;
	static const int NUMBER_OF_COLUMNS = NUMBER_OF_ROWS;
	static const int NUMBER_OF_LAYERS = 3;
	static const int NUMBER_OF_CUBES = NUMBER_OF_ROWS * NUMBER_OF_COLUMNS * NUMBER_OF_LAYERS;

	enum RubikSection { LAYER, HORIZONTAL_CROSS_LAYER, VERTICAL_CROSS_LAYER }; // The values of this enum represent the various selection modes of a Rubik's cube

	Rubik(glm::vec3 position, GLuint shader);
	virtual ~Rubik();

	void Draw();
	void Update();

	// Setters
	void SetPosition(glm::vec3 position);

	void SetIsAnimated(bool isAnimated);

	void SetTranslation(glm::mat4 translation);
	void SetScaling(glm::mat4 scaling);
	void SetRotation(glm::mat4 rotation);

	void SetShader(GLuint shader);

	void SetSelectedRubikSection(int selectedSection);

	// Getters
	glm::vec3 GetPosition() const { return position; }

	bool GetIsAnimated() const { return isAnimated; }

	Transformations GetTransformations() const { return rubikTransformations; }
	glm::mat4 GetScaling() const { return rubikTransformations.scaling; }
	glm::mat4 GetTranslation() const { return rubikTransformations.translation; }
	glm::mat4 GetRotation() const { return rubikTransformations.rotation; }

	int GetSelectedRubikSection() const { return selectedRubikSection; }
	RubikSection GetSelectedRubikSectionType() const { return selectedRubikSectionType; }

	void SwitchRubikSelectedSectionType(bool forward);
	void RotateRubikSelectedSection(bool forward); // Perform a rotation on the selected section in the specified direction (true=forward, false=backward)

protected:
	int rubikMatrix[NUMBER_OF_ROWS * NUMBER_OF_LAYERS][NUMBER_OF_COLUMNS]; // A matrix that represents the configuration/placement of the cubes within the Rubik's cube
	std::vector<Cube*> cubes; // The Cubes that form the Rubik's cube

	bool isAnimated;

	glm::vec3 position;

	Transformations rubikTransformations; // transformations applied on the Rubik's cube as a whole

	// Properties for animating the rotation of the selected section
	float cubeRotationAnimationIncrement;
	float cubeRotationAnimationCurrentAngle;
	float cubeRotationAnimationEndAngle;
	glm::vec3 cubeRotationAnimationDirection;

	int selectedCubes[NUMBER_OF_ROWS * NUMBER_OF_LAYERS]; // An array containing the IDs (indices) of the selected cubes
	int selectedRubikSection; // Represents the selected section (0-2)
	RubikSection selectedRubikSectionType;

	// Given a selected section (the layer or crossLayer param), these methods fill an array (the indices[] param) with 
	// the IDs of the cubes contained in that section.
	int* GetRubikLayerCubes(int layer, int indices[]);
	int* GetRubikHorizontalCrossLayerCubes(int crossLayer, int indices[]);
	int* GetRubikVerticalCrossLayerCubes(int crossLayer, int indices[]);

	void UnselectAllCubes(); // Sets the isSelected property of all the Cubes to false
	void SelectCubes(int cubeIndices[], int length); // Sets the isSelected property of the provided Cubes to true
	void UpdateSelectedCubes(); // Calls the appropriate method for gettting the selected cubes given the values of selectedRubikSection and selectedRubikSectionType. Updates the values of the selectedCubes array
	void SetSelectedCubesPivot(glm::vec3 pivot);
};