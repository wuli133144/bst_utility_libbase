--config 
  ipaddr="172.16.103.125"
  port= 3000

  function table_leng(t)
	  local leng=0
	  for k, v in pairs(t) do
	    leng=leng+1
	  end
	  return leng;
  end



  function http_filter(httpreq)
    
     if (type(httpreq)~= "string")
     then
        print(type (httpreq))
        return nil
     else
        local pos=nil
        local httpreq=string.reverse(httpreq)
        print(httpreq)
        pos=string.find(httpreq,"?",1)
        if(pos==nil)
        then
           --[ unable to find ?--]
           pos=string.find(httpreq,"/",1)
           --print(pos)
           if(pos~=nil)
           then
                print(httpreq)
                local method=string.sub(httpreq,1,pos-1);
                --print(method)
                method=string.reverse(method)
                return method
           end 
        else
           
           local pos1=string.find(httpreq,"/",1)
           if(pos1 ~=nil)
           then 
               local method=string.reverse(string.sub(httpreq,pos+1,pos1-1))
               return method
           else
              return nil
           end
      
        end
        --[local method=string.find(httpreq,"/",1)--]
          
     end
  end 

  
  function http_param_request(httpreq)
      if(type(httpreq)~="string")
      then 
           return nil 
      end 
      local pos_dot=string.find(httpreq,":",1)
      if(pos_dot ~= nil)
           if(string.sub(httpreq,pos_dot,pos_dot+2)~="://")
            return nil
           end 
      else
          pos_dot=string.find(httpreq,"//",1)
          if(pos_dot ~= nil)
           return nil
          else
             return true   
          end
          
      end 

      return true 
             
  end 

  function add(x,y)
  return x+y
  end 
