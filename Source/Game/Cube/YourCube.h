#pragma once


#include "BaseCube.h"

class YourCube : public BaseCube {
public:
	YourCube() = default;

	virtual void Update(_In_ FLOAT deltaTime) override;
};