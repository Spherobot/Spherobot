--print("ESP8266 Client")
uart.setup(0, 9600, 8, 0, 1, 0)
wifi.sta.disconnect()
wifi.setmode(wifi.STATION) 
--print("ESP8266 Client")
wifi.sta.disconnect()
wifi.setmode(wifi.STATION) 
wifi.sta.config("UniversalRemote","d67R92q6h4") -- connecting to server
wifi.sta.connect() 
--print("Looking for a connection")

tmr.alarm(1, 2000, 1, function()
     if(wifi.sta.getip()~=nil) then
          tmr.stop(1)
          print("Connected!")
          --print("Client IP Address:",wifi.sta.getip())
          cl=net.createConnection(net.TCP, 0)
          cl:connect(80,"192.168.4.1")
          uart.on("data", "<", 
            function(data)
                data=string.sub(data,0,string.len(data)-1)
                --print("Received:", data)
                cl:send(data)
            end, 
          0)
          cl:on("receive", function(conn, receivedData) 
              print(receivedData)         
          end)
      else
         --print("Connecting...")
      end
end)
