#include <LGSM.h>

String remoteNumber = "";  // the number you will call
char charbuffer[20];

void setup()
{
  // initialize serial communications
  Serial1.begin(115200); 
  Serial1.println("Make Voice Call");
  Serial1.println("Enter phone number to call.");
}

void loop()
{
  // add any incoming characters to the String:
  while (Serial1.available() > 0)
  {
    char inChar = Serial1.read();
    // if it's a newline, that means you should make the call:
    if (inChar == '\n')
    {
      // make sure the phone number is not too long:
      if (remoteNumber.length() < 20)
      {
        // let the user know you're calling:
        Serial1.print("Calling to : ");
        Serial1.println(remoteNumber);
        Serial1.println();

        // Call the remote number
        remoteNumber.toCharArray(charbuffer, 20);
        
        // Check if the receiving end has picked up the call
        if(LVoiceCall.voiceCall(charbuffer))
        {
          Serial1.println("Call Established. Enter line to end");
          // Wait for some input from the line
          while(Serial1.read() !='\n');
          // And hang up
          LVoiceCall.hangCall();
        }
        Serial1.println("Call Finished");
        remoteNumber="";
        Serial1.println("Enter phone number to call.");
      } 
      else
      {
        Serial1.println("That's too long for a phone number. I'm forgetting it"); 
        remoteNumber = "";
      }
    } 
    else
    {
      // add the latest character to the message to send:
      if(inChar!='\r')
        remoteNumber += inChar;
    }
  } 
}
