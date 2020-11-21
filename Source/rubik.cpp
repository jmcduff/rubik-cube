#include "rubik.h"
#include "cube.h"

#include <vector>

#define GLEW_STATIC 1
#include <GL/glew.h> 

#include <GLFW/glfw3.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Rubik::Rubik(glm::vec3 position, GLuint shader)
{
	this->SetPosition(position);

	isAnimated = false;

	this->rubikTransformations.translation = glm::translate(glm::mat4(1.0f), position);
	this->rubikTransformations.rotation = glm::mat4(1.0f);
	this->rubikTransformations.scaling = glm::mat4(1.0f);

	// Create the Cubes
	int i = 0;
	for (int layer = 0; layer < NUMBER_OF_LAYERS; layer++) {
		for (int row = 0; row < NUMBER_OF_ROWS; row++) {
			for (int column = 0; column < NUMBER_OF_COLUMNS; column++) {
				float x = Cube::CUBE_WIDTH * column;
				float y = Cube::CUBE_HEIGHT * layer;
				float z = Cube::CUBE_DEPTH * row;

				this->cubes.push_back(new Cube(glm::vec3(x, y, z)));
				this->cubes.at(i)->SetShader(shader);
				i++;
			}
		}
	}

	// Initialize the rubikMatrix
	i = 0;
	for (int layerSection = 0; layerSection < NUMBER_OF_LAYERS * NUMBER_OF_ROWS; layerSection++) {
		for (int column = 0; column < NUMBER_OF_COLUMNS; column++) {
			rubikMatrix[layerSection][column] = i;
			i++;
		}
	}

	// Initialize the selected section
	this->selectedRubikSection = 0;
	this->selectedRubikSectionType = LAYER;

	this->GetRubikLayerCubes(0, selectedCubes);

	for (int i = 0; i < NUMBER_OF_LAYERS * NUMBER_OF_ROWS; i++) {
		cubes.at(selectedCubes[i])->SetIsSelected(true);
	}
}

Rubik::~Rubik()
{
	cubes.clear();
}

void Rubik::Draw()
{
	for (int i = 0; i < this->NUMBER_OF_CUBES; i++) {
		cubes.at(i)->Draw(this->rubikTransformations);

		if (isAnimated && cubes.at(i)->GetIsSelected()) {
			// The pivot calculation is referenced from https://community.khronos.org/t/rotation-at-the-specified-pivot-point/46463
			glm::mat4 currentRotation = cubes.at(i)->GetRotation();
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), cubeRotationAnimationIncrement, cubeRotationAnimationDirection);
			glm::vec3 distanceWithPivot = cubes.at(i)->GetPosition() - cubes.at(i)->GetPivot();

			glm::mat4 cubeRotationMatrix = glm::translate(glm::mat4(1.0f), -distanceWithPivot) * rotation * glm::translate(glm::mat4(1.0f), distanceWithPivot);
			cubeRotationMatrix = cubeRotationMatrix * currentRotation;

			cubes.at(i)->SetRotation(cubeRotationMatrix);
		}
	}

	Update();
}

void Rubik::Update()
{
	if (isAnimated) {
		// add increment to current angle
		// compare current angle to end angle (if end angle has been reached, set current angle to end angle and stop animation)
		cubeRotationAnimationCurrentAngle += cubeRotationAnimationIncrement;

		if ((cubeRotationAnimationIncrement >= 0.0f && cubeRotationAnimationCurrentAngle >= cubeRotationAnimationEndAngle) || (cubeRotationAnimationIncrement < 0.0f && cubeRotationAnimationCurrentAngle <= cubeRotationAnimationEndAngle)) {
			cubeRotationAnimationCurrentAngle = cubeRotationAnimationEndAngle;
			SetIsAnimated(false);
		}
	}
}

void Rubik::SetPosition(glm::vec3 position)
{
	this->position = position;
}

void Rubik::SetIsAnimated(bool isAnimated)
{
	this->isAnimated = isAnimated;
}

void Rubik::SetTranslation(glm::mat4 translation)
{
	this->rubikTransformations.translation = translation;
}

void Rubik::SetScaling(glm::mat4 scaling)
{
	this->rubikTransformations.scaling = scaling;
}

void Rubik::SetRotation(glm::mat4 rotation)
{
	this->rubikTransformations.rotation = rotation;
}

void Rubik::SetShader(GLuint shader)
{
	for (int i = 0; i < NUMBER_OF_CUBES; i++) {
		cubes.at(i)->SetShader(shader);
	}
}

void Rubik::SetSelectedRubikSection(int selectedSection)
{
	this->selectedRubikSection = selectedSection;
	UpdateSelectedCubes();
}

void Rubik::SwitchRubikSelectedSectionType(bool forward)
{
	if (selectedRubikSectionType == LAYER) {
		selectedRubikSectionType = forward ? HORIZONTAL_CROSS_LAYER : VERTICAL_CROSS_LAYER;
	}
	else if (selectedRubikSectionType == HORIZONTAL_CROSS_LAYER) {
		selectedRubikSectionType = forward ? VERTICAL_CROSS_LAYER : LAYER;
	}
	else if (selectedRubikSectionType == VERTICAL_CROSS_LAYER) {
		selectedRubikSectionType = forward ? LAYER : HORIZONTAL_CROSS_LAYER;
	}

	UpdateSelectedCubes();
}

void Rubik::RotateRubikSelectedSection(bool forward)
{
	cubeRotationAnimationCurrentAngle = glm::radians(0.0f);
	cubeRotationAnimationIncrement = forward ? glm::radians(1.0f) : glm::radians(-1.0f);
	cubeRotationAnimationEndAngle = forward ? glm::radians(90.0f) : glm::radians(-90.0f);

	if (selectedRubikSectionType == LAYER) {
		// Set up rotation animation
		cubeRotationAnimationDirection = glm::vec3(0.0f, 1.0f, 0.0f);
		SetSelectedCubesPivot(glm::vec3(1.5f, selectedRubikSection, 1.5f));

		// Since some cubes are going to be rotated, update the configuration of the cubes within the rubikMatrix.
		int start = selectedRubikSection * NUMBER_OF_ROWS;
		int max = start + NUMBER_OF_ROWS;
		int newRubikSectionConfiguration[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
		 
		if (forward) {
			for (int rubikColumn = NUMBER_OF_COLUMNS - 1, i = 0; rubikColumn >= 0; rubikColumn--, i++) {
				for (int rubikRow = start, j = 0; rubikRow < max; rubikRow++, j++) {
					newRubikSectionConfiguration[i][j] = rubikMatrix[rubikRow][rubikColumn];
				}
			}
		}
		else {
			for (int rubikColumn = 0, i = 0; rubikColumn < NUMBER_OF_COLUMNS; rubikColumn++, i++) {
				for (int rubikRow = max - 1, j = 0; rubikRow >= start; rubikRow--, j++) {
					newRubikSectionConfiguration[i][j] = rubikMatrix[rubikRow][rubikColumn];
				}
			}
		}

		for (int rubikRow = start, i = 0; rubikRow < max; rubikRow++, i++) {
			for (int rubikColumn = 0, j = 0; rubikColumn < NUMBER_OF_COLUMNS; rubikColumn++, j++) {
				rubikMatrix[rubikRow][rubikColumn] = newRubikSectionConfiguration[i][j];
			}
		}
	} else if (selectedRubikSectionType == HORIZONTAL_CROSS_LAYER) {
		// Set up rotation animation
		cubeRotationAnimationDirection = glm::vec3(0.0f, 0.0f, 1.0f);
		SetSelectedCubesPivot(glm::vec3(1.5f, 1.5f, selectedRubikSection));

		// Since some cubes are going to be rotated, update the configuration of the cubes within the rubikMatrix.
		int start = selectedRubikSection;
		int max = selectedRubikSection + ((NUMBER_OF_ROWS - 1) * NUMBER_OF_LAYERS);
		int increment = NUMBER_OF_LAYERS;
		int newRubikSectionConfiguration[NUMBER_OF_LAYERS][NUMBER_OF_COLUMNS];

		if (!forward) {
			for (int rubikColumn = NUMBER_OF_COLUMNS - 1, i = 0; rubikColumn >= 0; rubikColumn--, i++) {
				for (int rubikRow = start, j = 0; rubikRow <= max; rubikRow += increment, j++) {
					newRubikSectionConfiguration[i][j] = rubikMatrix[rubikRow][rubikColumn];
				}
			}
		}
		else {
			for (int rubikColumn = 0, i = 0; rubikColumn < NUMBER_OF_COLUMNS; rubikColumn++, i++) {
				for (int rubikRow = max, j = 0; rubikRow >= start; rubikRow -= increment, j++) {
					newRubikSectionConfiguration[i][j] = rubikMatrix[rubikRow][rubikColumn];
				}
			}
		}

		for (int rubikRow = start, i = 0; rubikRow <= max; rubikRow += increment, i++) {
			for (int rubikColumn = 0, j = 0; rubikColumn < NUMBER_OF_COLUMNS; rubikColumn++, j++) {
				rubikMatrix[rubikRow][rubikColumn] = newRubikSectionConfiguration[i][j];
			}
		}
	}
	else if (selectedRubikSectionType == VERTICAL_CROSS_LAYER) {
		// Set up rotation animation
		cubeRotationAnimationDirection = glm::vec3(1.0f, 0.0f, 0.0f);
		SetSelectedCubesPivot(glm::vec3(selectedRubikSection, 1.5f, 1.5f));

		// Since some cubes are going to be rotated, update the configuration of the cubes within the rubikMatrix.
		int newRubikSectionConfiguration[NUMBER_OF_ROWS][NUMBER_OF_LAYERS];

		if (forward) {
			for (int rubikRow = NUMBER_OF_ROWS - 1, i = 0; rubikRow >= 0; rubikRow--, i++) {
				for (int rubikLayer = 0, j = 0; rubikLayer < NUMBER_OF_LAYERS; rubikLayer++, j++) {
					newRubikSectionConfiguration[i][j] = rubikMatrix[rubikRow + rubikLayer * NUMBER_OF_ROWS][selectedRubikSection];
				}
			}
		}
		else {
			for (int rubikRow = 0, i = 0; rubikRow < NUMBER_OF_ROWS; rubikRow++, i++) {
				for (int rubikLayer = NUMBER_OF_LAYERS - 1, j = 0; rubikLayer >= 0; rubikLayer--, j++) {
					newRubikSectionConfiguration[i][j] = rubikMatrix[rubikRow + rubikLayer * NUMBER_OF_ROWS][selectedRubikSection];
				}
			}
		}
		
		int i = 0;
		for (int row = 0; row < NUMBER_OF_ROWS; row++) {
			for (int column = 0; column < NUMBER_OF_COLUMNS; column++) {
				rubikMatrix[i][selectedRubikSection] = newRubikSectionConfiguration[row][column];
				i++;
			}
		}
	}

	SetIsAnimated(true);
}

int * Rubik::GetRubikLayerCubes(int layer, int indices[])
{
	int i = 0;
	for (int layerSection = layer * NUMBER_OF_ROWS; layerSection < (layer * NUMBER_OF_ROWS) + NUMBER_OF_ROWS; layerSection++) {
		for (int column = 0; column < NUMBER_OF_COLUMNS; column++) {
			indices[i] = this->rubikMatrix[layerSection][column];
			i++;
		}
	}

	return indices;
}

int * Rubik::GetRubikHorizontalCrossLayerCubes(int crossLayer, int indices[])
{
	int i = 0;
	int maxCrossLayerSection = crossLayer + ((NUMBER_OF_ROWS - 1) * NUMBER_OF_LAYERS);
	for (int crossLayerSection = crossLayer; crossLayerSection <= maxCrossLayerSection; crossLayerSection += NUMBER_OF_ROWS) {
		for (int column = 0; column < NUMBER_OF_COLUMNS; column++) {
			indices[i] = this->rubikMatrix[crossLayerSection][column];
			i++;
		}
	}
	
	return indices;
}

int * Rubik::GetRubikVerticalCrossLayerCubes(int crossLayer, int indices[])
{
	int i = 0;
	int maxRow = NUMBER_OF_ROWS * NUMBER_OF_LAYERS;
	for (int row = 0; row < maxRow; row++) {
			indices[i] = this->rubikMatrix[row][crossLayer];
			i++;
	}

	return indices;
}

void Rubik::UnselectAllCubes()
{
	for (int cube = 0; cube < this->NUMBER_OF_CUBES; cube++) {
		cubes.at(cube)->SetIsSelected(false);
	}
}

void Rubik::SelectCubes(int cubeIndices[], int length)
{
	for (int i = 0; i < length; i++) {
		cubes.at(cubeIndices[i])->SetIsSelected(true);
	}
}

void Rubik::UpdateSelectedCubes()
{
	UnselectAllCubes();

	if (selectedRubikSectionType == LAYER) {
		GetRubikLayerCubes(selectedRubikSection, selectedCubes);
		SelectCubes(selectedCubes, NUMBER_OF_ROWS * NUMBER_OF_COLUMNS);
	}
	else if (selectedRubikSectionType == HORIZONTAL_CROSS_LAYER) {
		GetRubikHorizontalCrossLayerCubes(selectedRubikSection, selectedCubes);
		SelectCubes(selectedCubes, NUMBER_OF_COLUMNS * NUMBER_OF_LAYERS);
	}
	else if (selectedRubikSectionType == VERTICAL_CROSS_LAYER) {
		GetRubikVerticalCrossLayerCubes(selectedRubikSection, selectedCubes);
		SelectCubes(selectedCubes, NUMBER_OF_ROWS * NUMBER_OF_LAYERS);
	}
}

void Rubik::SetSelectedCubesPivot(glm::vec3 pivot)
{
	for (int i = 0; i < NUMBER_OF_ROWS * NUMBER_OF_LAYERS; i++) {
		cubes.at(selectedCubes[i])->SetPivot(pivot);
	}
}
