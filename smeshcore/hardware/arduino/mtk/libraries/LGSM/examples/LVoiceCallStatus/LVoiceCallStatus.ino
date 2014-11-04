#include <LGSM.h>

// PIN Number
#define PINNUMBER ""

char numtel[20];           // buffer for the incoming call

void setup()
{
  // initialize serial communications
  Serial1.begin(115200);
  Serial1.println("Receive Voice Call");

  // connection state
  boolean notConnected = true;

  // This makes sure the modem notifies correctly incoming events
  LVoiceCall.hangCall();

  Serial1.println("Waiting Call");
}

void loop()
{
  // Check the status of the voice call
  switch (LVoiceCall.getVoiceCallStatus()) 
  {
    case IDLE_CALL: // Nothing is happening

      break;

    case CALLING: // This should never happen, as we are not placing a call

      Serial1.println("CALLING");
      break;

    case RECEIVINGCALL: // Yes! Someone is calling us

      Serial1.println("RECEIVING CALL");

      // Retrieve the calling number
      LVoiceCall.retrieveCallingNumber(numtel, 20);

      // Print the calling number
      Serial1.print("Number:");
      Serial1.println(numtel);

      // Answer the call, establish the call
      LVoiceCall.answerCall();         
      break;

    case TALKING:  // In this case the call would be established

      Serial1.println("TALKING. Enter line to interrupt.");
      while(Serial1.read()!='\n')
        delay(100);
      LVoiceCall.hangCall();
      Serial1.println("HANG. Waiting Call.");      
      break;
  }
  delay(1000);
}

