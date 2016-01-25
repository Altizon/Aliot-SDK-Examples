Aliot-Appliance-SDK
===================

A repository of sample codes to push data from devices to the Datonis platform

Currently samples are available only in Java. We will be adding samples in Python and C soon.


Java
====

Pull the repository to your workspace. The java folder contains a pom.xml.
First install the SDK jar to your local mvn repository:

mvn install:install-file -Dfile=lib/aliot-sdk-3.0.jar -DgroupId=io.datonis.sdk -DartifactId=aliot-sdk -Dversion=3.0 -Dpackaging=jar

mvn clean install

Then, run:

mvn eclipse:eclipse

This should generate .classpath and .project files
Simply import the directory as an Eclipse Java project and you will be good to go.

Non Eclipse users, please include the jar files in the java/lib directory in your classpath and run the example
in the com.altizon.agent package.

You can also generate a package from the command line and then run it as follows:

mvn package

You can then run example as follows:

java -Dlog4j.properties=src/main/resources/log4j.properties -Daliot.properties=src/main/resources/aliot.properties -jar target/aliot-agent-1.0-jar-with-dependencies.jar

The file src/main/resources/aliot.properties contains the configuration required for the Agent to work.

