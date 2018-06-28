#include "config.h"


Config::Config(){
}

Config::Config(const char *filepath)
{
     Init(filepath);//initializer
}

Config::Config(string & filepath)
{
    Init(filepath.c_str());
}

Config ::~Config(){}

void Config::Init(const char *filepath)
{
        if(filepath==NULL)return;
		read_xml(filepath,m_pt);
}

ptree Config::getNode(string & node)
{
   return m_pt.get_child(node.c_str());
}


template<typename T>
 T Config::Get(string & node)
 {
 
    return  m_pt.get<T>(node);
   
 }

 template<typename T>
  T Config::Get(const char *node){
         return m_pt.get<T>(node);
 }

 
  template<typename T>
  std::vector<T> Config::GetList(const char *node){
		 //ptree pt=getNode(node.c_str());
		 ptree pt=m_pt.get_child(node);
		 std::vector<T> all_object;
		 for(auto iter=pt.begin();iter!=pt.end();iter++)
		 {
		      all_object.push_back(iter->second.data());
		 }
		 return all_object;
  }

#if 0
  template<typename T>
	T Config::GetNodeAttrValue(const char *node,const char *attr)
  	{

	         
			 //auto iter=pt.begin();
			 //cout<<iter.first.data()<<std::endl;

			 string dest(node);
			 int pos=dest.find_last_of(".");
			 string src=dest.substr(0,pos);
			 string targets_node=dest.substr(pos+1,dest.size()-pos);

			 std::cout<<src<<"   "<<targets_node<<std::endl;
			 
  			 ptree pt=m_pt.get_child(src.c_str());
			 auto iter=pt.begin();
			 for(;iter!=pt.end();iter++){
			 	 if(iter->first==targets_node.c_str()){
				 	 break;
				 }
			 }
			 return iter->second.get<T>("<xmlattr>."+std::string(attr));
   
  	}
#endif 

#if   1

int main(){

         Config config("config.xml");

		 
		 string attr=config.GetNodeAttrValue<string>("con.name","value");


		 std::cout<<"attr:value"<<attr<<std::endl;




		 

}

#endif

