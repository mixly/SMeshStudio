all:$(PROJECT_OBJECTFILES) $(PROJECT_LIBRARIES) contiki-$(TARGET).a

CONF_BORDER_ROUTER = 1

CFLAGS += -DARDUINO=1

# IPv6
UIP_CONF_IPV6 = 1
# RPL
UIP_CONF_RPL = 0

# Address type: set to 0 for using ds2401, 1 for EUI64_ADDRESS defined in params.c.
UIP_CONF_EUI64 = 1

# POWERMODE
CONF_LOWPOWER = 0

WITH_COAP = 7


# REST framework, requires WITH_COAP
ifeq ($(WITH_COAP), 7)
${info INFO: compiling with CoAP-07}
CFLAGS += -DWITH_COAP=7
CFLAGS += -DREST=coap_rest_implementation
CFLAGS += -DUIP_CONF_TCP=1
APPS += er-coap-07
else ifeq ($(WITH_COAP), 3)
${info INFO: compiling with CoAP-03}
CFLAGS += -DWITH_COAP=3
CFLAGS += -DREST=coap_rest_implementation
CFLAGS += -DUIP_CONF_TCP=1
APPS += er-coap-03
else
${info INFO: compiling with HTTP}
CFLAGS += -DWITH_HTTP
CFLAGS += -DREST=http_rest_implementation
CFLAGS += -DUIP_CONF_TCP=1
APPS += er-http-engine
endif

APPS += erbium
#APPS += camera-interface
#APPS += buzzer-id

ifeq ($(CONF_BORDER_ROUTER), 1)
CFLAGS += -DRS232_CONF_NUMPORTS=2
APPS += rpl-border-router
else
CFLAGS += -DRS232_CONF_NUMPORTS=0
endif

CONTIKI = ../..
include $(CONTIKI)/Makefile.include