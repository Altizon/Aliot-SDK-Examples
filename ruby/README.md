Ruby - Aliot-Appliance-SDK
==========================

Structure of the downloaded repository
--------------------------------------

If you are familiar with Git, you can pull the contents of the repository down. If not you can download the archive as a repository zip and unzip the file. Once you have the repository downloaded, the ruby folder contains the aliot-3.0.0.gem and sample ruby code sample.rb.

Implementing Aliot Agent
------------------------

Installing gem in your system:

cd <path-to-aliot-gem>

gem install aliot-3.0.1.gem 

You can then run example as follows:

ruby sample.rb

Configuring the Agent
---------------------

The sample code creates a thing that sends CPU and memory usage readings from your machine to Datonis. Modify the sample.rb file as follows:

1. Add appropriate access_key and secret_key from the downloded key_pair in AliotConfiguration.
2. Add Thing id, Thing name, Thing Description of the thing whose data you want to send to Datonis.
3. Finally add the metrics name and its value in the data field. You can also set waypoints and send it to Datonis.

There is no ruby implementation for sending data through MQTT/MQTTS protocol.
