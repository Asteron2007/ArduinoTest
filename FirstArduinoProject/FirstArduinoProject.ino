// Tempolary test project
// Asteron Robotics Evolution
// http://asteron24.ru/
// asteron-2007@mail.ru
////+7 910 553-46-37



#include <GSM.h>

#define PINNUMBER ""

// initialize the library instance
GSM gsmAccess;
GSM_SMS sms;

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.println("SMS Messages Sender");

  // connection state
  boolean notConnected = true;

  // Start GSM shield
  // If your SIM has PIN, pass it as a parameter of begin() in quotes
  while (notConnected)
  {
    if (gsmAccess.begin(PINNUMBER) == GSM_READY)
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("GSM initialized");
}

void loop()
{

  Serial.print("Enter a mobile number: ");
  char remoteNum[20] = "+79105544321";  // telephone number to send sms
  //readSerial(remoteNum);
  Serial.println(remoteNum);

  // sms text
  Serial.print("Now, enter SMS content: ");
  char txtMsg[200] = "From Arduino Mega";
  //readSerial(txtMsg);
  Serial.println("SENDING");
  Serial.println();
  Serial.println("Message:");
  Serial.println(txtMsg);

  // send the message
  sms.beginSMS(remoteNum);
  sms.print(txtMsg);
  sms.endSMS();
  Serial.println("\nCOMPLETE!\n");
}

/*
  Read input serial
 */
//int readSerial(char result[])
//{
//  int i = 0;
//  while (1)
//  {
//    while (Serial.available() > 0)
//    {
//      char inChar = Serial.read();
//      if (inChar == '\n')
//      {
//        result[i] = '\0';
//        Serial.flush();
//        return 0;
//      }
//      if (inChar != '\r')
//      {
//        result[i] = inChar;
//        i++;
//      }
//    }
//  }
//}
