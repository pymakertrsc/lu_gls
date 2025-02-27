#pragma once
#include "../../../global/base.h"

namespace ff {

	//1 定义全局都能够使用的宏
	//2 定义了全局都能够使用的宏函数
	//3 定义了全局都能够使用的结构体
	static const std::string common =
		"#define PI 3.141592653589793\n"\
		"#define PI2 6.283185307179586\n"\
		"#define PI_HALF 1.5707963267948966\n"\
		"\n"\
		"#ifndef saturate\n"\
		"#define saturate(a) clamp(a, 0.0, 1.0)\n"\
		"#endif\n"\
		"\n"\

		//入射光：不管是点光源、聚光灯、平行光，多种光源，计算光线的方式不同
		//但是最终得到的结果，就是Color与Direction
		"struct IncidentLight {\n"\
		"	vec3 color;\n"\
		"	vec3 direction;\n"\
		"};\n"\
		"\n"\

		//反射光：大体上分为直接光照与间接光照
		"struct ReflectedLight {\n"\
		"	vec3 directDiffuse;\n"\
		"	vec3 directSpecular;\n"\
		"	vec3 indirectDiffuse;\n"\
		"	vec3 indirectSpecular;\n"\
		"};\n"\
		"\n"\

		//几何体描述
		//既然已经决定，所有的计算都在摄像机坐标系之内来算了，所以我们观察者眼睛的位置应该在（0.0.0）
		//viewDir 直接是当前Fragment在摄像机坐标系下的坐标
		"struct GeometricContext {\n"\
		"	vec3 position;\n"\
		"	vec3 normal;\n"\
		"	vec3 viewDir;\n"\
		"};\n"\
		"\n";
}