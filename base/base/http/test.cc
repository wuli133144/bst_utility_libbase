#include <iostream>
#include <thread>
#include <unistd.h>
#include "HttpQuery.h"


using namespace std;


int main(){


HttpQuery httpq;

#if 0
string req="www.baidu.com";
string a=httpq.get(req);

std::cout<<a<<std::endl;


std::cout<<std::endl<<">>>>>>>>>>>>>>>>>"<<std::endl;

 char *res=httpq.get2("www.baidu.com");

std::cout<<res<<std::endl;

#endif


string request_str("www.baidu.com?name=jackuw&&ps=123343");
string reqs;
httpq.post(request_str,reqs);



//test thread safe

thread work1([&]{
	
	HttpQuery httpq1;
	httpq1.post2("www.baidu.com?name=jackuw&&ps=123343",NULL);

	std::cout<<"work1"<<httpq1.getref()<<std::endl;
	usleep(1000*100);

});

thread work2([&]{
	
	HttpQuery httpq2;
	httpq2.post2("www.baidu.com?name=jackuw&&ps=123343",NULL);
	std::cout<<"work2"<<httpq2.getref()<<std::endl;
	usleep(1000*100);

});
thread work3([&]{
	
	HttpQuery httpq3;
	httpq3.post2("www.baidu.com?name=jackuw&&ps=123343",NULL);
	
	std::cout<<"work3"<<httpq3.getref()<<std::endl;
	usleep(1000*100);

});


work1.join();

work2.join();

work3.join();



usleep(2);





return 0;


}
