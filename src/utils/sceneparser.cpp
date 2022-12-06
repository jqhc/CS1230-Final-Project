#include "sceneparser.h"
#include "scenefilereader.h"
#include "glm/gtx/transform.hpp"

#include <chrono>
#include <memory>
#include <iostream>

glm::mat4 getTransformationMatrix(std::vector<SceneTransformation*> transformations) {
    std::vector<glm::mat4> translations;
    std::vector<glm::mat4> rotations;
    std::vector<glm::mat4> scalings;

    glm::mat4 ctm(1.0f);

    for (SceneTransformation* transformation : transformations) {
        switch(transformation->type) {
        case TransformationType::TRANSFORMATION_TRANSLATE:
            translations.push_back(glm::translate(transformation->translate));
            break;
        case TransformationType::TRANSFORMATION_ROTATE:
            rotations.push_back(glm::rotate(transformation->angle, transformation->rotate));
            break;
        case TransformationType::TRANSFORMATION_SCALE:
            scalings.push_back(glm::scale(transformation->scale));
            break;
        case TransformationType::TRANSFORMATION_MATRIX:
            scalings.push_back(transformation->matrix);
            break;
        }
    }

    for (glm::mat4 matrix : translations) {
        ctm = ctm * matrix;
    }
    for (glm::mat4 matrix : rotations) {
        ctm = ctm * matrix;
    }
    for (glm::mat4 matrix : scalings) {
        ctm = ctm * matrix;
    }

    return ctm;
}

// on first call, shapes and ctm should be empty.
void addToShapes(SceneNode* sceneGraph, std::vector<RenderShapeData> &shapes, glm::mat4 ctm) {
    ctm = ctm * getTransformationMatrix(sceneGraph->transformations);

    for (ScenePrimitive* primitive : sceneGraph->primitives) {
        shapes.push_back(RenderShapeData{*primitive, ctm});
    }

    for (SceneNode* child : sceneGraph->children) {
        addToShapes(child, shapes, ctm);
    }
}

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readXML();
    if (!success) {
        return false;
    }

    // Task 4: populate renderData with global data, lights, and camera data
    renderData.globalData = fileReader.getGlobalData();
    renderData.lights = fileReader.getLights();
    renderData.cameraData = fileReader.getCameraData();
//    renderData.textureImages = fileReader.getTextureImages();

    // Task 5: populate renderData's list of primitives and their transforms.
    //         This will involve traversing the scene graph, and we recommend you
    //         create a helper function to do so!

    renderData.shapes.clear();

    SceneNode* sceneGraph = fileReader.getRootNode();

    addToShapes(sceneGraph, renderData.shapes, glm::mat4(1.0f));

    return true;
}
