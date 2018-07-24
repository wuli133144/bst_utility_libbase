



local tabs={x="123","1234","234"}

for i,j in pairs(tabs) do 
 print (i,j)
end 

--ipairs（）迭代器下标不敏感
for i,j in ipairs(tabs) do 
  print (i,j)
end 
print(#tabs)



function http_get_method(req)
    if (string.find(req,"/")==nil)then 
	       return nil 
    else 
	       local req_cp=string.reverse(req)
		   pos_split=string.find(req_cp,"/",1)
		   if(pos_split==nil)then 
		      --like www.baid.com/get?
			   return nil  
		   else 
		         method_req=string.sub(req_cp,1,pos_split-1)
				 local method=string.reverse(method_req)
				 --like get?key1=values && key2==value2
				 pos_question=string.find(method,"?",1)
				 if(pos_question == nil ) then 
				    return method 
				 else 
				    method=string.sub(method,1,pos_question-1)
					return method
				 end 
				 
				 
		   end 
		   
	end 
	
end 


function bvalid_http_req(req)
     if (type(requests) ~= "string"  or (string.len(requests)) < 1) --checkerror
	    return false 
   	 req_reverse_pos=string.find(req,"/",1)
	 if(req_reverse_pos== string.len(req))then 
	   return false 
	 elseif (req_reverse_pos==nil)
	   return false 
	 end 
	 return true 
end 

function http_param_request(requests)
     
	  bRet=bvalid_http_req(requests)
	  local method=""
	  if(bRet) then 
	      method=http_get_method(requests)
		  if(method == nil) then 
		      return nil 
		  end 
		  if(method=="GET" or method == "get")then 
		       --todo GET 
		  end 
	  end 
	  
end 



-- lua table test



local ta1={12,23,34}


function MAX(arr) 
    local arr_size=#ta1 
	if(arr_size <1) then 
	     return nil 
	end 
	local max=ta1[1]
	for i,j in pairs(ta1) do 
	   if(ta1[i] > max)then 
	      max= ta1[j]
	   end 
	end
	return max 
end 

print (MAX(ta1))


//xcode object- c
//qt-creator c/c++ 
//linux c/c++ lua python 
//http://c.biancheng.net/cpp/objective_c/


























