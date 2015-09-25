/*
 * This file is part of Espruino, a JavaScript interpreter for Microcontrollers
 *
 * Copyright (C) 2013 Gordon Williams <gw@pur3.co.uk>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * Utilities and definitions for handling Pins
 * ----------------------------------------------------------------------------
 */

#include "jspin.h"
#include "jspininfo.h" 
#include "jshardware.h"
#include "jsinteractive.h"

bool jshIsPinValid(Pin pin) {
  return pin != NC;
}


Pin jshGetPinFromString(const char *s) {
  if(isStrInt(s))  return (Pin)stringToInt(s);

	int searchMin = 0;
	int searchMax = PINNUM - 1;

	while(searchMin <= searchMax){
		int idx = (searchMin + searchMax) >> 1;
		int cmp = strcmp(s,pinsInfo[idx].pinName);
		if(cmp == 0) return pinsInfo[idx].pin;
		else if(cmp < 0) searchMax = idx - 1;
		else searchMin = idx + 1;
	}
	return NC;
}

/** Write the pin name to a string. String must have at least 8 characters (to be safe) */
void jshGetPinString(char *result, Pin pin) {
 	result[0] = 0; // just in case

	if (jshIsPinValid(pin)) {
		itostr(pin,&result[0],10);
	}
}

/// Given a var, convert it to a pin ID (or -1 if it doesn't exist). safe for undefined!
Pin jshGetPinFromVar(JsVar *pinv) {
  if (jsvIsString(pinv) && pinv->varData.str[5]==0/*should never be more than 4 chars!*/) {
    return jshGetPinFromString(&pinv->varData.str[0]);
  } else if (jsvIsInt(pinv) /* This also tests for the Pin datatype */) {
    return (Pin)jsvGetInteger(pinv);
  } else return NC;
}

Pin jshGetPinFromVarAndUnLock(JsVar *pinv) {
  Pin pin = jshGetPinFromVar(pinv);
  jsvUnLock(pinv);
  return pin;
}
// ----------------------------------------------------------------------------

bool jshPinInput(Pin pin) {
  bool value = false;
  if (jshIsPinValid(pin)) {
    if (!jshGetPinStateIsManual(pin))
      jshPinSetState(pin, JSHPINSTATE_GPIO_IN);
    value = jshPinGetValue(pin);
  } else jsExceptionHere(JSET_ERROR, "Invalid pin!");
  return value;
}


void jshPinOutput(Pin pin, bool value) {
  if (jshIsPinValid(pin)) {
    if (!jshGetPinStateIsManual(pin))
      jshPinSetState(pin, JSHPINSTATE_GPIO_OUT);
    jshPinSetValue(pin, value);
  } else jsExceptionHere(JSET_ERROR, "Invalid pin!");
}

