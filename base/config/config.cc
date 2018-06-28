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

  template<typename T>
	T Config::GetNodeAttrValue(const char *node,const char *attr)
  	{

	        string dest("");
			for(const ptree::value_type &v:m_pt.get_child(std::string(node)+".<xmlattr>")){
                  if(v.first==attr){
				  	dest=v.second.data();
				  	break;
				  }
			}
            return dest;
  	}


#if   1

int main(){

         Config config("config.xml");
         
		 
		 string attr=config.GetNodeAttrValue<string>("con.name","ip");


		 std::cout<<"attr:value"<<attr<<std::endl;
		 
	



		 

}

#endif

