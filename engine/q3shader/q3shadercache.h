#pragma once

namespace q3shader
{

	class Q3Shader;

	void init();
	void acquire();
	void release();

	Q3Shader* getShader(const char* name);
};