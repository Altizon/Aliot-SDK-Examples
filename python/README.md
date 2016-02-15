Python - Aliot-Appliance-SDK
==========================

Pull the repository to your workspace. The python folder contains a python_agent.zip file and sample.py file.

First set the python path to point python_agent.zip file:

export PYTHONPATH=$PYTHONPATH:<path_to>/python_agent.zip

Modify the sample.py file as follows:

1. Add appropriate access_key and secret_key from the downloded key_pair in GatewayConfig function
2. Add Thing id, Thing name, Thing Description of the thing whose data you want to send to Datonis.
3. Finally add the metrics name and it value. You can also set waypoints and send it to Datonis
4. Data can be send using HTTP or MQTT protocol for which appropriate funtion should be used.

You can then run example as follows:

python sample.py