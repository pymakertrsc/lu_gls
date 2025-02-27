#pragma once
#include "../../../global/base.h"

namespace ff {

	//1 ����ȫ�ֶ��ܹ�ʹ�õĺ�
	//2 ������ȫ�ֶ��ܹ�ʹ�õĺ꺯��
	//3 ������ȫ�ֶ��ܹ�ʹ�õĽṹ��
	static const std::string common =
		"#define PI 3.141592653589793\n"\
		"#define PI2 6.283185307179586\n"\
		"#define PI_HALF 1.5707963267948966\n"\
		"\n"\
		"#ifndef saturate\n"\
		"#define saturate(a) clamp(a, 0.0, 1.0)\n"\
		"#endif\n"\
		"\n"\

		//����⣺�����ǵ��Դ���۹�ơ�ƽ�й⣬���ֹ�Դ��������ߵķ�ʽ��ͬ
		//�������յõ��Ľ��������Color��Direction
		"struct IncidentLight {\n"\
		"	vec3 color;\n"\
		"	vec3 direction;\n"\
		"};\n"\
		"\n"\

		//����⣺�����Ϸ�Ϊֱ�ӹ������ӹ���
		"struct ReflectedLight {\n"\
		"	vec3 directDiffuse;\n"\
		"	vec3 directSpecular;\n"\
		"	vec3 indirectDiffuse;\n"\
		"	vec3 indirectSpecular;\n"\
		"};\n"\
		"\n"\

		//����������
		//��Ȼ�Ѿ����������еļ��㶼�����������ϵ֮�������ˣ��������ǹ۲����۾���λ��Ӧ���ڣ�0.0.0��
		//viewDir ֱ���ǵ�ǰFragment�����������ϵ�µ�����
		"struct GeometricContext {\n"\
		"	vec3 position;\n"\
		"	vec3 normal;\n"\
		"	vec3 viewDir;\n"\
		"};\n"\
		"\n";
}