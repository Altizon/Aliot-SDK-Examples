package com.datonis.aliot.sample;

import org.json.simple.JSONObject;
import java.lang.management.ManagementFactory;
import com.sun.management.OperatingSystemMXBean;

import com.altizon.aliot.gateway.AliotCommunicator;
import com.altizon.aliot.gateway.AliotConfiguration;
import com.altizon.aliot.gateway.AliotGateway;
import com.altizon.aliot.gateway.BulkData;
import com.altizon.aliot.gateway.entity.Sensor;
import com.altizon.aliot.gateway.exception.AliotException;
import com.altizon.aliot.gateway.exception.IllegalSensorException;
import com.altizon.aliot.gateway.exception.InvalidRequestException;

/**
 * This is a sample agent that demonstrates how to push Data to Datonis. In this example we will push data in bulk to the Datonis platform.
 * Bulk transmission is the preferred and optimal way to send data. If you need near-real time performance and reduced data overheads, use 
 * this mechanism. Note, for scalability reasons you cannot push more than 25 data parameters in a bulk push.
 * 
 * For demonstration purpose, we will push CPU and Memory utilization. Please note that the example uses java's
 * OperatingSystemMXBean class which is known to be unreliable across platforms.
 * 
 * @author Ranjit Nair
 * 
 */
public class SampleAgentBulk
{

    private static Sensor sensor;
    private static AliotGateway gateway;
    private static int NUM_EVENTS = 57;

    /**
     * @param args
     * @throws InvalidRequestException
     * @throws IllegalSensorException
     */
    public static void main(String[] args) throws IllegalSensorException, InvalidRequestException
    {

        SampleAgentBulk bulkAgent = new SampleAgentBulk();
        bulkAgent.register();
        bulkAgent.transmitDataInBulk();
        System.out.println("Exiting");
        System.exit(0);

    }

    private boolean register()
    {

        try
        {

            // First construct an AliotConfiguration object using your downloaded access and secret keys.
            // The keys are available in the platform portal under your license plan.
            AliotConfiguration config = new AliotConfiguration("your_access_key", "your_secret_key");
            gateway = new AliotGateway(config);

            // Decide what the metadata format of your sensor should be. This will show up as the sensor parameters that
            // you are pushing
            // on Datonis. In this example we will be pushing cpu and memory utilization. We therefore declare them in a
            // JSON schema
            // format
            /*
             * 
             * {"cpu": { "type": "number" }, "mem": { "type": "number" } }
             */

            // Obviously escaped so that java does not complain
            String metadata = "{\"cpu\": {\"type\":\"number\"}, \"mem\": {\"type\":\"number\"}}";

            // Create a sensor. It is extremely important to have unique sensor keys or collisions may occur and data
            // could get
            // overwritten. Please create a sensor on the Datonis portal and use the key here. Sensor names are
            // non-unique, however
            // uniqueness is recommended. Use a logical 'type' to describe the sensor. For instance, System Monitor in
            // this case.
            // Multiple sensors can exist for a type.
            // This constructor will throw an illegal sensor execption if conditions are not met.
            sensor = new Sensor("8353298d72357ef747123219fcdcc5tfad6275f5", "SysMon", "System Monitor", "A monitor for CPU and Memory", metadata);

            // You can register multiple sensors and send data for them. First add the sensors and then call register.
            // In this case, there is only a single sensor object.
            gateway.addSensor(sensor);
            int registered = gateway.register();
            if (registered != AliotCommunicator.OK)
                return false;

            System.out.println("Registered a sensor");
            // Heartbeats are used to indicate to Datonis that your agent is alive even though it
            // might not be sending data. It is a good practice to send heartbeats every 5 minutes.
            gateway.transmitHeartbeat();
            System.out.println("Transmitted a heartbeat");

        }
        catch (AliotException e)
        {
            System.out.println(e.getMessage());
            return false;
        }
        catch (IllegalSensorException e)
        {
            System.out.println(e.getMessage());
            return false;
        }
        catch (InvalidRequestException e)
        {
            System.out.println(e.getMessage());
            return false;
        }
        return true;
    }

    private JSONObject getSystemInfo()
    {

        OperatingSystemMXBean os = (OperatingSystemMXBean) ManagementFactory.getOperatingSystemMXBean();
        long mem = (os.getFreePhysicalMemorySize() / (1024 * 1024));
        // Not accurate but sufficient for demonstration
        double cpu = ((os.getSystemLoadAverage() / os.getAvailableProcessors()) * 10);

        JSONObject obj = new JSONObject();
        obj.put("cpu", cpu);
        obj.put("mem", mem);
        return obj;
    }

    private String getTransmissionError(int code)
    {
        // There could be several reasons for tranmission failure.
        switch (code)
        {
        case AliotCommunicator.UNAUTHORIZED:
            return "Unauthorized access. Please check your access and secret key";

            // Your license allows you to transmit data at a defined rate. This is thrown when the rate is breached.
            // Please throttle down
        case AliotCommunicator.EXCESSIVE_RATE:
            return "You are pushing data at a rate that is greater than what your license allows";

            // Happens when the packet sent does not match the JSON metadata.
        case AliotCommunicator.INVALID_REQUEST:
            return "Request is invalid";

        case AliotCommunicator.FAILED:
            return "Failed to send data. Reasons are unknown";

        case AliotCommunicator.NOT_ACCEPTABLE:
            return "Failed to send data. Request is unacceptable";

        case AliotCommunicator.NO_CONNECTION:
            return "Not connected";

        default:
            return "Failed to send data. The response code is: " + code;

        }

    }

    public void transmitData(Sensor sensor, BulkData bulkData) throws IllegalSensorException
    {
        int retval = gateway.transmitData(sensor, bulkData);

        if (retval == AliotCommunicator.OK)
        {
            System.out.println("Transmitted " + bulkData.getMessages().size() + " packets: ");
        }
        else
        {
            System.out.println(getTransmissionError(retval));
        }
    }

    public void transmitDataInBulk() throws IllegalSensorException, InvalidRequestException
    {
        System.out.println("Starting to transmit data");

        //Create a BulkData envelope object and add your data into it. Note, bulk data only supports
        //25 messages at a time due to performance reasons. Addiional data points will be discarded.
        BulkData bulkData = new BulkData();
        int heartbeat = 0;

        for (int count = 1; count <= NUM_EVENTS; count++)
        {
            long start = System.currentTimeMillis();
            JSONObject value = getSystemInfo();
            bulkData.addData(start, value);
            System.out.println("Added packet to bulk : " + count + " value " + value.toJSONString());

            // Only 25 messages can be transmitted at a time in bulk.
            if (count % 25 == 0)
            {
                transmitData(sensor, bulkData);
                // Reset the bulkdata object to get new messages.
                bulkData = new BulkData();
            }

            // Periodically send a hearbeat. This optional. The sensor will show up as live
            // if either a heartbeat or an event is received in the last 5 minutes
            heartbeat++;
            if (heartbeat == 300)
            {
                gateway.transmitHeartbeat();
                heartbeat = 0;
            }

            try
            {
                Thread.currentThread().sleep(1000);
            }
            catch (InterruptedException e)
            {

                e.printStackTrace();
            }
        }
        // Transmit the rest of the data
        if (bulkData.getMessages().size() > 0)
        {
            transmitData(sensor, bulkData);
        }
        System.out.println("Transmitted data");

    }
}
