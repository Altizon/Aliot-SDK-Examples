require 'aliot/thing'
require 'aliot/aliot_configuration'
require 'aliot/aliot_gateway_http'

#Note this will work only on linux
def get_cpu_usage
  `top -bn 2 -d 0.01 | grep '^%Cpu' | tail -n 1 | gawk '{print $2+$4+$6}'`.strip.to_f
end

def get_mem_usage
  `free | grep Mem | awk '{print $3/$2 * 100.0}'`.strip.to_f
end

#Aliot::Thing.new(key, name, description)
t = Aliot::Thing.new("3e2b1f8t81", "Compressor", "An example MQTT based thing")
#Aliot::AliotConfiguration.new(access_key, secrete_key, protocol = "http", ssl = false, url=nil)
c = Aliot::AliotConfiguration.new("72f2a8322fca8dc4ttt321t49f83at58f8t78t54", "cf5a1eba58756db5ta741t7a25ca686eba3acdaf")
#initialize the gateway
g = Aliot::AliotGatewayHttp.new(c)
#register the thing
g.register(t)

counter = 0
while true
  if counter == 0
    # Send heart beat in between (optional)
    g.send_heart_beat(t)
  end
  data = {:temperature => get_cpu_usage, :pressure => get_mem_usage}
  #waypoint format : [latitude, longitude]
  waypoint = [(18.52 + rand/10), (73.85 + rand/10)];
  timestamp = Time.now.to_i * 1000;
  #we can send the data in three ways
  g.send_data(t, data, nil, timestamp) #1. send only data
  #g.send_data(t, nil, waypoint, timestamp) #2. send only waypoint
  #g.send_data(t, data, waypoint, timestamp) #3. send both data and waypoint
  
  counter = (counter + 1) % 5
  sleep(5)
end