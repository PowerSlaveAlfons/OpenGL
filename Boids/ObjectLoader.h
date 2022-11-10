#pragma once

#include<glm/glm.hpp>
#include<vector>
#include<string>
#include "Model.h"


class ObjectLoader {
public:
    static bool loadOBJ(
        const char* path,
        Model& out_model
    );

};

