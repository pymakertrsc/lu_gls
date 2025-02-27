#include <iostream>
#include <string>
#include <regex>

//利用了一组表达式的规矩，来对字符串进行匹配验证或者寻找
//010-85966598

void matchPureArray() {
	std::cout << std::endl;
	std::cout << std::endl;

	std::string text = "rands[0]";
	std::string pattern = "(\\w+)(\\])?(\\[|\\.)?";
	std::smatch result;
	std::regex regex(pattern);

	std::cout << "匹配PureArrayUniform：" << std::endl;
	std::cout << "目标字符串：" << text << std::endl;
	std::cout << "表达式：" << pattern << std::endl;

	int i = 0;
	while (std::regex_search(text, result, regex)) {
		std::cout << "第" << i << "轮匹配" << std::endl;
		std::cout << "id = 0, result = " << result[0] << std::endl;
		std::cout << "id = 1, result = " << result[1] << std::endl;
		std::cout << "id = 2, result = " << result[2] << std::endl;
		std::cout << "id = 3, result = " << result[3] << std::endl;
		text = result.suffix();
		++i;
	}
}

void matchSingle() {
	std::cout << std::endl;
	std::cout << std::endl;

	std::string text = "modelViewMatrix";
	std::string pattern = "(\\w+)(\\])?(\\[|\\.)?";
	std::smatch result;
	std::regex regex(pattern);

	std::cout << "匹配SingleUniform：" << std::endl;
	std::cout << "目标字符串：" << text << std::endl;
	std::cout << "表达式：" << pattern << std::endl;

	int i = 0;
	while (std::regex_search(text, result, regex)) {
		std::cout << "第" << i << "轮匹配" << std::endl;
		std::cout << "id = 0, result = " << result[0] << std::endl;
		std::cout << "id = 1, result = " << result[1] << std::endl;
		std::cout << "id = 2, result = " << result[2] << std::endl;
		std::cout << "id = 3, result = " << result[3] << std::endl;

		//suffix: 
		// 比如：abc_cdef_xxx,匹配小写字母组成的字符串
		// regex_search在达成第一次匹配后，即结束
		// 匹配到abc后结束，那么suffix就是_cdef_xxx
		//
		text = result.suffix();
		++i;
	}
}

void matchStructuredSimple() {
	std::cout << std::endl;
	std::cout << std::endl;

	std::string text = "light.color";
	std::string pattern = "(\\w+)(\\])?(\\[|\\.)?";
	std::smatch result;
	std::regex regex(pattern);

	std::cout << "匹配StructuredUniform(Simple)：" << std::endl;
	std::cout << "目标字符串：" << text << std::endl;
	std::cout << "表达式：" << pattern << std::endl;

	int i = 0;
	while (std::regex_search(text, result, regex)) {
		std::cout << "第" << i << "轮匹配" << std::endl;
		std::cout << "id = 0, result = " << result[0] << std::endl;
		std::cout << "id = 1, result = " << result[1] << std::endl;
		std::cout << "id = 2, result = " << result[2] << std::endl;
		std::cout << "id = 3, result = " << result[3] << std::endl;
		text = result.suffix();
		++i;
	}
}


void matchStructuredArray() {
	std::cout << std::endl;
	std::cout << std::endl;

	std::string text = "light[0].color";
	std::string pattern = "(\\w+)(\\])?(\\[|\\.)?";
	std::smatch result;
	std::regex regex(pattern);

	std::cout << "匹配StructuredUniform(Array)：" << std::endl;
	std::cout << "目标字符串：" << text << std::endl;
	std::cout << "表达式：" << pattern << std::endl;

	int i = 0;
	while (std::regex_search(text, result, regex)) {
		std::cout << "第" << i << "轮匹配" << std::endl;
		std::cout << "id = 0, result = " << result[0] << std::endl;
		std::cout << "id = 1, result = " << result[1] << std::endl;
		std::cout << "id = 2, result = " << result[2] << std::endl;
		std::cout << "id = 3, result = " << result[3] << std::endl;

		text = result.suffix();
		++i;
	}
}

int main() {
	//具体匹配
	std::string text1 = "abc";
	std::regex regex1("abc");
	std::cout << "1 is " << std::regex_match(text1, regex1) << std::endl;

	//匹配任意字符
	std::string text2 = "=";
	std::regex regex2(".");
	std::cout << "2 is " << std::regex_match(text2, regex2) << std::endl;

	//匹配0-多个任意字符
	std::string text3 = "abcd";
	std::regex regex3(".*");
	std::cout << "3 is " << std::regex_match(text3, regex3) << std::endl;

	//匹配1-多个任意字符
	std::string text4 = "ab2+-";
	std::regex regex4(".+");
	std::cout << "4 is " << std::regex_match(text4, regex4) << std::endl;

	//匹配任意数字、字母、下划线 1-多个
	std::string text5 = "abcde_123";
	std::regex regex5("\\w+");
	std::cout << "5 is " << std::regex_match(text5, regex5) << std::endl;

	//匹配任意“非”数字、字母、下划线 1-多个
	std::string text6 = "+=#￥%123";
	std::regex regex6("\\W+");
	std::cout << "6 is " << std::regex_match(text6, regex6) << std::endl;

	//匹配任意数字
	std::string text7 = "0123456789";
	std::regex regex7("\\d+");
	std::cout << "7 is " << std::regex_match(text7, regex7) << std::endl;

	//匹配任意非数字
	std::string text8 = "abcdedf_+$";
	std::regex regex8("\\D+");
	std::cout << "8 is " << std::regex_match(text8, regex8) << std::endl;

	//规定开头
	std::string text9 = "abc123";
	std::regex regex9("^abc\\d+");
	std::cout << "9 is " << std::regex_match(text9, regex9) << std::endl;

	//规定结尾
	std::string text10 = "123abc";
	std::regex regex10("\\d+abc$");
	std::cout << "10 is " << std::regex_match(text10, regex10) << std::endl;

	//规定出现次数
	std::string text11 = "012abc456def";
	std::regex regex11("(\\d{3}\\w{3}){2}");
	std::cout << "11 is " << std::regex_match(text11, regex11) << std::endl;

	//分组匹配
	std::string text12 = "012abc928384tttccc";
	std::regex regex12("(\\d+)([a-z]+)(\\d+)");
	std::smatch result12;

	std::regex_search(text12, result12, regex12);
	std::cout << "分组匹配：" << std::endl;
	std::cout << "id = 0, result = " << result12[0] << std::endl;
	std::cout << "id = 1, result = " << result12[1] << std::endl;
	std::cout << "id = 2, result = " << result12[2] << std::endl;
	std::cout << "id = 3, result = " << result12[3] << std::endl;


	//循环分组匹配
	std::string text13 = "012abc456def495868ysuej";
	std::regex regex13("(\\d+)([a-z]+)");
	std::smatch result13;

	int i = 0;
	std::cout << "循环分组匹配：" << std::endl;
	while (std::regex_search(text13, result13, regex13)) {
		std::cout << "第" << i << "轮匹配" << std::endl;
		std::cout << "id = 0, result = " << result13[0] << std::endl;
		std::cout << "id = 1, result = " << result13[1] << std::endl;
		std::cout << "id = 2, result = " << result13[2] << std::endl;
		text13 = result13.suffix();
		++i;
	}

	std::cout << std::endl;
	std::cout << std::endl;

	matchSingle();
	matchPureArray();
	matchStructuredSimple();
	matchStructuredArray();
}

/*
* Uniform分类规则：
* id = result[1]
* subscript = result[3];
* matchEnd = result.position(0) + result.length(0);即当前目标字符串匹配结果的最后位置
* 
* 1 如果subscript为空串，则一定是SingleUniform
* 2 如果subscript为[，且matchEnd+2=text.length,则一定是PureArrayUniform
* 3 如果subscript不为空，且matchEnd+2！=text.length，则一定是StructuredUniform
*/
