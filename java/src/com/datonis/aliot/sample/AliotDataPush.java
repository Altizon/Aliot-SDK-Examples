/******************************************************************************* 
 *  Copyright 2014 Altizon Systems.
 *  Licensed under the Apache License, Version 2.0 (the "License"); 
 *  
 *  You may not use this file except in compliance with the License. 
 *  You may obtain a copy of the License at:
 *          http://www.apache.org/licenses/LICENSE-2.0
 *  This file is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR 
 *  CONDITIONS OF ANY KIND, either express or implied. See the License for the 
 *  specific language governing permissions and limitations under the License.
 * ***************************************************************************** 
 */


package com.datonis.aliot.sample;

import java.util.ArrayList;
import java.util.Random;

import com.altizon.aliot.gateway.AliotCommunicator;
import com.altizon.aliot.gateway.AliotGateway;
import com.altizon.aliot.gateway.GatewayConfiguration;
import com.altizon.aliot.gateway.entity.AnalogSensor;
import com.altizon.aliot.gateway.entity.Device;
import com.altizon.aliot.gateway.entity.Sensor;
import com.altizon.aliot.gateway.exception.IllegalDeviceException;
import com.altizon.aliot.gateway.exception.IllegalGatewayException;
import com.altizon.aliot.gateway.exception.IllegalSensorException;
import com.altizon.aliot.gateway.exception.InvalidRequestException;

public class AliotDataPush
{
    private static AliotGateway gateway;
    private static Device device;
    private static AnalogSensor sensor;

    private static void register()
    {

        GatewayConfiguration config;
        try
        {
            
            //First initialize the gateway with the access key and secret key, gateway key name and (optionally) description
            //Keys must be unique across your license and can be alphanumeric. Name must be unique for each object type. i.e
            //no two gateways must have the same name. 
            //Keys can be downloaded from the license page in Datonis.
            config = new GatewayConfiguration("access-key", "secret-key", "gateway-key", "gateway-name",
                    "Gateway Description");

            //Create a gateway object using the gatweay configuration 
            gateway = new AliotGateway(config);

            //Create a sensor object with a sensor key, name and (optionally) description and (optionally) timezone
            sensor = new AnalogSensor("sensor-key", "sensor-name", "Sensor Description");
            ArrayList<Sensor> sensors = new ArrayList<Sensor>();
            sensors.add(sensor);
            //Create a device that represents a logical collection of sensors. Device key, name, typeand a list of sensors are mandatory.
            //A device cannot exist without sensors
            device = new Device("device-key", "device-name", "device-type", "description", sensors);
            gateway.addDevice(device);
            //Register everything with Datonis. Once registered. You can re-register every entity repeatedly. The keys
            //define the entity uniquely within datonis
            gateway.register();
            
            //Ensure that a heartbeat is sent periodically. Datonis will declare the gateway as 'dead' if a heartbeat is not
            //sent every 120seconds.
            gateway.transmitHeartbeat();
            
            System.out.println("Registered the gateway");

        }
        catch (IllegalGatewayException e)
        {
            //This exception is thrown when you have an invalid access, secret key, gateway key or name when registering a gateway
            e.printStackTrace();
        }
        catch (IllegalSensorException e)
        {
            //This exception is thrown when you have an invalid access, secret key, sensor key or name when registering a sensor
            e.printStackTrace();
        }
        catch (IllegalDeviceException e)
        {
            //This exception is thrown when you have an invalid access, secret key, device key or sensor list when registering a device
            e.printStackTrace();
        }
        catch (InvalidRequestException e)
        {
            //This exception is thrown when you have an invalid access, secret key, sensor key or name when registering a sensor
            e.printStackTrace();
        }

    }

    public static void transmitData() 
    {
        System.out.println("Starting to transmit data");

        int heartbeat = 0;
        Random random = new Random();
        
        //Send 10 randomly generated values to Datonis
        for (int count = 0; count < 10; count++)
        {
            //generate a random value between 0 - 99
            int val = random.nextInt(100);
            int retval = 0;
            try
            {
                //Transmit the value over the wire. The data can be any floating point value
                retval = gateway.transmitAnalogData(sensor, val);
                //Transmission succeeded.
                if(retval == AliotCommunicator.OK)
                {
                    System.out.println("Transmitted: " + val +" successfully");
                }
                else if(retval == AliotCommunicator.EXCESSIVE_RATE)
                {
                    //Check your license plan to see how many requests per minute your license allows.
                    //This return value indicates that the threshold has been exceeded
                    System.out.println("Too many requests. Threshold exceeded. Please throttle down");
                }
                else if(retval == AliotCommunicator.UNAUTHORIZED)
                {
                    //Unauthorized access
                    System.out.println("Unauthorized access");
                }
                else if(retval == AliotCommunicator.FAILED)
                {
                    //Happens if the request does not reach datonis due to a network failure.
                    System.out.println("Request could not be sent for value: "+val);
                }
 
            }
            catch (IllegalSensorException e)
            {
                //Thrown if a sensor that is not registered transmits data
                e.printStackTrace();
            }
            
            heartbeat++;
            if(heartbeat == 5)
            {
                //Periodically send a heartbeat otherwise the system will show up as dead.
                gateway.transmitHeartbeat();
                heartbeat = 0;
            }
        }

        System.out.println("Transmitted data");

    }

    public static void main(String[] args)
    {
        register();
        transmitData();
        

    }

}
