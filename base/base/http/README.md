



## what's about it

> <strong> HttpQuery library is http request tools,including post get etc dependen on libcurl 
> it's thread safe ,so you can use it in multi thread env
</strong>


## how to use 

like:
```
  #include "HttpQuery.h"

  int main()
  {
	string req="www.baidu.com";
	string a=httpq.get(req);

	std::cout<<a<<std::endl;


	std::cout<<std::endl<<">>>>>>>>>>>>>>>>>"<<std::endl;

	char *res=httpq.get2("www.baidu.com");

	std::cout<<res<<std::endl;

    string request_str("www.baidu.com?name=jackuw&&ps=123343");
	string reqs;
	httpq.post(request_str,reqs);

  }
  


```
## multi-thread safe

like:
```
	  #include "HttpQuery.h"

	  int main()
	  {

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
```


## company

<strong>Copyright <2018-7-27> <COPYRIGHT JACKWUwuyujie@sunlands.com>
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
documentation files (the "Software"), to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:The above
copyright notice and this permission notice shall be included in all copies or substantial portions of the 
Software.THE SOFTWARE IS PROVIDED "AS IS" , WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.</strong>


