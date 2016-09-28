--print("ESP8266 Server")
uart.setup(0, 9600, 8, 0, 1, 0)
wifi.setmode(wifi.STATIONAP);
wifi.ap.config({ssid="UniversalRemote",pwd="d67R92q6h4"});
--print("Server IP Address:",wifi.ap.getip())

sv = net.createServer(net.TCP) 
sv:listen(80, function(conn)
    conn:on("receive", function(conn, receivedData) 
        print(receivedData)         
    end) 
    --conn:on("sent", functiondo(conn) 
    --  collectgarbage()
    --end)
    uart.on("data", "<", 
            function(data)
                data=string.sub(data,0,string.len(data)-1)
                --print("Received:", data)
                conn:send(data)
            end, 
          0)
end)
