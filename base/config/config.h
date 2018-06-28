#ifndef  __CONFIG__H
#define  __CONFIG__H

#include <iostream> 
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp> 
#include <boost/property_tree/xml_parser.hpp> 
#include <boost/typeof/typeof.hpp>  
using namespace std; 
using namespace boost::property_tree;


class Config{
	
public:
	Config();
	Config(const char *);
	Config(string &filepath);
	~Config();

	void Init();
	void Init(const char *);
	ptree getNode(string &node);

	template<typename T>
		T Get(string &node);
	template<typename T>
		T Get(const char *node);
	template<typename T>
		std::vector<T> GetList(const char *node);
	
	//template<typename T>
	//	T GetNodeAttrValue(const char *,const char *);

private:
	ptree m_pt;

};

#endif 
